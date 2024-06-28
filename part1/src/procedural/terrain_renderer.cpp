#include "procedural/terrain_renderer.hpp"
#include "procedural/fault_formation.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp> 
#include "game_state.hpp"
#include <asset/ppm.hpp>
#include <procedural/foliage_generator.hpp>
using namespace std;
using namespace glm;



void addToMesh(vector<GLfloat> &mesh, vec3 &v1, const int &ix, int scale) {
	//vertices
	mesh[ix] = v1.x* scale;
	mesh[ix + 1] = v1.y* scale;
	mesh[ix + 2] = v1.z* scale;
}

void addVecToMesh(vector<GLfloat>& mesh, vec3& v1, const int& ix) {
	mesh[ix] += v1.x;
	mesh[ix + 1] += v1.y;
	mesh[ix + 2] += v1.z;
}

void normalizeVec3(vector<GLfloat>& mesh, const int& ix) {
	vec3 v(mesh[ix], mesh[ix + 1], mesh[ix + 2]);
	v = normalize(v);
	mesh[ix] = v.x;
	mesh[ix + 1] = v.y;
	mesh[ix + 2] = v.z;
}

void addVec2ToMesh(vector<GLfloat>& mesh, vec2& v, const int& ix) {
	mesh[ix] = v.x;
	mesh[ix + 1] = v.y;
}

void addToIndices(vector<GLuint>& indices,const int &vectorIx, const int &i, const int &width) {
	// Triangle 1
	indices[vectorIx] = i;
	indices[vectorIx + 1] = i + (width+1);
	indices[vectorIx + 2] = i + (width+1) + 1;
	// traingle 2
	indices[vectorIx + 3] = i;
	indices[vectorIx + 4] = i + 1;
	indices[vectorIx + 5] = i + (width+1) + 1;
}

void TerrainRenderer::Initialize(int depth, int width, double height, unsigned int seed, int scale, float filter)
{
	std::cout << "Initializing AssetManager" << std::endl;
	std::string vertexShaderSource = LoadShaderAsString("./shaders/debug_vert.glsl");
	std::string fragmentShaderSource = LoadShaderAsString("./shaders/debug_frag.glsl");

	mShader = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

	std::string instvertexSource = LoadShaderAsString("./shaders/instance_vert.glsl");
	std::string instfragmentSource = LoadShaderAsString("./shaders/instance_frag.glsl");
	instanceShaderID = CreateShaderProgram(instvertexSource, instfragmentSource);

	largeTreeRenderer.setup(constants.MediaDir / "objects" / "trees"/ "Tree"/"Tree.obj");
	mediumTreeRenderer.setup(constants.MediaDir / "objects" / "trees" / "Lowpoly_tree.obj");
	largeRockRenderer.setup(constants.MediaDir / "objects" / "rocks" / "large.obj");
	rockRenderer.setup(constants.MediaDir / "objects" / "rocks" / "small.obj");
	flowerRenderer.setup(constants.MediaDir / "objects" / "grass" / "flower.obj");
	grassRenderer.setup(constants.MediaDir / "objects" / "grass" / "grass.obj");
	shrubRenderer.setup(constants.MediaDir / "objects" / "trees" / "small_oak.obj");
	// Model transformation by translating our object into world space
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// Projection matrix (in perspective) 
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
		(float)gGameState.gScreenWidth / (float)gGameState.gScreenHeight,
		0.1f,
		2000.0f);


	// Initialize uniforms in material shader
	glUseProgram(mShader);
	// Retrieve our location of our Model Matrix
	GLint u_ModelMatrixLocation = glGetUniformLocation(mShader, "u_ModelMatrix");
	glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);


	// Retrieve our location of our perspective matrix uniform 
	GLint u_ProjectionLocation = glGetUniformLocation(mShader, "u_Projection");
	glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);

	glUseProgram(instanceShaderID);
	// Retrieve our location of our Model Matrix
	u_ModelMatrixLocation = glGetUniformLocation(instanceShaderID, "u_ModelMatrix");
	glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);


	// Retrieve our location of our perspective matrix uniform 
	u_ProjectionLocation = glGetUniformLocation(instanceShaderID, "u_Projection");
	glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);

	glUseProgram(mShader);
	GLuint dirtMapLoc = glGetUniformLocation(mShader, "u_dirtMap");
	GLuint rockMapLoc = glGetUniformLocation(mShader, "u_rockMap");
	GLuint grassMapLoc = glGetUniformLocation(mShader, "u_grassMap");
	GLuint noiseMapLoc = glGetUniformLocation(mShader, "u_noiseMap");
	GLuint maxHeightLoc = glGetUniformLocation(mShader, "u_MaxHeight");
	glUniform1f(maxHeightLoc, height);
	glUniform1i(dirtMapLoc, 0);
	glUniform1i(rockMapLoc, 1);
	glUniform1i(grassMapLoc, 2);
	glUniform1i(noiseMapLoc, 3);
	LoadTerrains();
	glUseProgram(0);

	double maxHeight = height;
	FaultFormation f(width, depth, maxHeight, 500, filter);
	f.CreateFaultFormation();
	HeightMap map = std::move(f.mHeightMap);
	OpenSimplex simplex(0);
	HeightMap map2 = std::move(simplex.generateNoiseMap(width, depth, 0.5, 0));
	for (int i = 0; i < depth; i++) {
		for (int j = 0; j < width; j++) {
			map[i][j].height = map[i][j].height + map2[i][j].height*0.1;
		}
	}
	simulateErosion(map, 10, 0.01, 0.02, 0.01,0.10);

	
	FoliageGenerator foliageGenerator(0, maxHeight, width, depth, map);
	foliageGenerator.generateMap();
	float heightScale = maxHeight;
	for (int i = 0; i <= depth; i++) {
		for (int j = 0; j <= width; j++) {
			if (foliageGenerator.foliageMap[i][j].tallTrees.size() > 0) {
				for (const auto& pos : foliageGenerator.foliageMap[i][j].tallTrees) {
					largeTreeRenderer.instancePositions.push_back(glm::vec3((j)* scale+pos.x/scale, map[i][j].height * heightScale* scale -0.05, (i)* scale)+pos.y / scale); // Assuming y-up world
				}
			}
			if (foliageGenerator.foliageMap[i][j].shortTrees.size() > 0) {
				for (const auto& pos : foliageGenerator.foliageMap[i][j].shortTrees) {
					mediumTreeRenderer.instancePositions.push_back(glm::vec3((j) * scale+pos.x, map[i][j].height * heightScale * scale -0.20, (i) * scale)+pos.y); // Assuming y-up world
				}
			}
			if (foliageGenerator.foliageMap[i][j].shrubs.size() > 0) {
				for (const auto& pos : foliageGenerator.foliageMap[i][j].shrubs) {
					shrubRenderer.instancePositions.push_back(glm::vec3((j-50) * scale+pos.x, map[i][j].height * heightScale * scale, (i)*scale)+pos.y); // Assuming y-up world
				}
			}
			if (foliageGenerator.foliageMap[i][j].flowers.size() > 0) {
				for (const auto& pos : foliageGenerator.foliageMap[i][j].flowers) {
					flowerRenderer.instancePositions.push_back(glm::vec3((j) * scale+pos.x, map[i][j].height * heightScale * scale+0.05, (i)*scale)+pos.y); // Assuming y-up world
				}
			}
			if (foliageGenerator.foliageMap[i][j].grass.size() > 0) {
				for (const auto& pos : foliageGenerator.foliageMap[i][j].grass) {
					grassRenderer.instancePositions.push_back(glm::vec3((j) * scale+pos.x, (map[i][j]).height * heightScale * scale + 0.05, (i)*scale)+pos.y); // Assuming y-up world
				}
			}
			if (foliageGenerator.foliageMap[i][j].largeRocks.size() > 0) {
				for (const auto& pos : foliageGenerator.foliageMap[i][j].largeRocks) {
					largeRockRenderer.instancePositions.push_back(glm::vec3((j) * scale+pos.x, map[i][j].height * heightScale * scale, (i)*scale)+pos.y); // Assuming y-up world
				}
			}
			if (foliageGenerator.foliageMap[i][j].smallRocks.size() > 0) {
				for (const auto& pos : foliageGenerator.foliageMap[i][j].smallRocks) {
					rockRenderer.instancePositions.push_back(glm::vec3((j) * scale+pos.x / scale, map[i][j].height * heightScale * scale, (i)*scale)+pos.y / scale); // Assuming y-up world
				}
			}
		}
	}

	largeTreeRenderer.updateInstances();
	rockRenderer.updateInstances();
	grassRenderer.updateInstances();
	flowerRenderer.updateInstances();
	shrubRenderer.updateInstances();
	mediumTreeRenderer.updateInstances();
	largeRockRenderer.updateInstances();
	std::vector<GLfloat> mesh((depth +1)*(width+1)*(14));
	indices.resize(depth * width * (6));
	// i - x position on the floor j - z position on the floor
	for (int z = 0; z <= depth; z++) {
		for (int x = 0; x <= width; x++) {
			/*
			double color = (-noise.noise(x / 10.0, z / 10.0) + 1.0) / 2.0;
			double height = color *10.0 - 5.0;
			vec3 v = vec3(x, height, z);
			*/
			vec3 v = vec3(x, map[z][x].height * heightScale, z);
			int meshIndex = (z * (width + 1) + x)*14;
			int indicesIx = (z * width + x) * 6;
			double color = v.y/ maxHeight;
			//v.y = 0;
			addToMesh(mesh, v, meshIndex, scale);
			vec2 texCords = vec2(((float)(x )/ width)*((float)width/terrainTileWidth), ((float)(z) / height) * ((float)height / terrainTileHeight));
			addVec2ToMesh(mesh, texCords, meshIndex + 6);
			if (x < width && z < depth) {
				UpdateMesh(x, z, width,mesh, map);
				addToIndices(indices, indicesIx, meshIndex / 14, width);
			}
		}
	}
	for (int z = 0; z <= depth; z++) {
		for (int x = 0; x <= width; x++) {
			int meshIndex = (z * (width + 1) + x) * 14 + 3;
			normalizeVec3(mesh, meshIndex);
			normalizeVec3(mesh, meshIndex+5);
			normalizeVec3(mesh, meshIndex+8);
		}
	}
	// Vertex Arrays Object (VAO) Setup
	glGenVertexArrays(1, &mVertexArrayObject);
	// We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
	glBindVertexArray(mVertexArrayObject);

	glGenBuffers(1, &mVertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(GLfloat), mesh.data(), GL_STATIC_DRAW);


	// vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)0);
	// normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)(sizeof(GL_FLOAT) * 3));

	// tex
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)(sizeof(GL_FLOAT) * 6));

	// tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)(sizeof(GL_FLOAT) * 8));

	// bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)(sizeof(GL_FLOAT) * 11));

	// Indices
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void TerrainRenderer::Draw() {
	glUseProgram(mShader);
	// Update the View Matrix
	GLint u_ViewMatrixLocation = glGetUniformLocation(mShader, "u_ViewMatrix");
	glm::mat4 viewMatrix = gGameState.gCamera->GetViewMatrix();
	glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	// bind dirt map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dirtTexture);

	// bind rock map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rockTexture);
	// bind grass map
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, grassTexture);

	// bind noise map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	glBindVertexArray(mVertexArrayObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glUseProgram(0);



	largeTreeRenderer.render(instanceShaderID);
	rockRenderer.render(instanceShaderID);
	grassRenderer.render(instanceShaderID);
	flowerRenderer.render(instanceShaderID);
	//shrubRenderer.render(instanceShaderID);
	mediumTreeRenderer.render(instanceShaderID);
	largeRockRenderer.render(instanceShaderID);
}

void computeNormals(Triangle& t) {
	// add normals
	vec3 v1 = t.vertices[1] - t.vertices[0];
	vec3 v2 = t.vertices[2] - t.vertices[0];
	vec3 normal = normalize(glm::cross(v1, v2));
	t.normals[0] = normal;
	t.normals[1] = normal;
	t.normals[2] = normal;

	vec2 deltaUV1 = t.tex[1] - t.tex[0];
	vec2 deltaUV2 = t.tex[2] - t.tex[1];
	vec3 edge1 = t.vertices[1] - t.vertices[0];
	vec3 edge2 = t.vertices[2] - t.vertices[1];
	// E1 = deltaU1.T + deltaV1.B
	// E2 = deltaU2.T + deltaV2.B
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	t.tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	t.tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	t.tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	t.tangent = normalize(t.tangent);
	t.biTangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	t.biTangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	t.biTangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	t.biTangent = normalize(t.biTangent);
}

void TerrainRenderer::UpdateMesh(int& x, int& z, int & width, vector<GLfloat>& mesh, HeightMap& map)
{
	
	Triangle t1;
	t1.vertices[0] = vec3(x, map[z][x].height, z);
	t1.vertices[1] = vec3(x, map[z + 1][x].height,z+1);
	t1.vertices[2] = vec3(x+1, map[z + 1][x + 1].height, z+1);

	//color
	computeNormals(t1);
	int meshIndex = (z * (width + 1) + x) * 14;
	//normal
	addVecToMesh(mesh, t1.normals[0], meshIndex + 3);
	addVecToMesh(mesh, t1.tangent, meshIndex + 8);
	addVecToMesh(mesh, t1.biTangent, meshIndex + 11);
	meshIndex = ((z + 1) * (width + 1) + x) * 14;
	addVecToMesh(mesh, t1.normals[1], meshIndex + 3);
	addVecToMesh(mesh, t1.tangent, meshIndex + 8);
	addVecToMesh(mesh, t1.biTangent, meshIndex + 11);
	meshIndex = ((z + 1) * (width + 1) + x+1) * 14;
	addVecToMesh(mesh, t1.normals[2], meshIndex + 3);
	addVecToMesh(mesh, t1.tangent, meshIndex + 8);
	addVecToMesh(mesh, t1.biTangent, meshIndex + 11);


	Triangle t2;
	t2.vertices[0] = vec3(x, map[z][x].height,z);
	t2.vertices[1] = vec3(x+1, map[z][x+1].height,z);
	t2.vertices[2] = vec3(x+1, map[z + 1][x + 1].height,z+1);
	computeNormals(t2);
	meshIndex = (z * (width + 1) + x+1) * 14;
	//normal
	addVecToMesh(mesh, t1.normals[0], meshIndex + 3);
	addVecToMesh(mesh, t1.tangent, meshIndex + 8);
	addVecToMesh(mesh, t1.biTangent, meshIndex + 11);
}



void TerrainRenderer::LoadTerrains()
{
	filesystem::path grass = constants.MediaDir/"textures"/"seamgrass.ppm";
	filesystem::path rock = constants.MediaDir / "textures" / "rock.ppm";
	filesystem::path dirt = constants.MediaDir / "textures" / "detailmap.ppm";
	filesystem::path noise = constants.MediaDir / "textures" / "noise.ppm";
	bool isSuccess = false;

	dirtTexture = PPM::loadTextureToRender(dirt, isSuccess);
	rockTexture = PPM::loadTextureToRender(rock, isSuccess);
	grassTexture = PPM::loadTextureToRender(grass, isSuccess);
	noiseTexture = PPM::loadTextureToRender(noise, isSuccess);
}
