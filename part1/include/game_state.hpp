#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#if defined(LINUX) || defined(MINGW) ||defined(_WIN32)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif

#include "camera.hpp"
#include "lights.hpp"
#include "scene.hpp"
#include "asset/asset_manager.hpp"


struct GameState{
		// Screen Dimensions
		int gScreenWidth 						= 1200;
		int gScreenHeight 						= 720;
		SDL_Window* gGraphicsApplicationWindow 	= nullptr;
		SDL_GLContext gOpenGLContext			= nullptr;
		glm::vec3 defaultColor = {0.5f, 0.0f, 0.0f};
		// Main loop flag
		bool gQuit = false; // If this is quit = 'true' then the program terminates.
\
		// Draw wireframe mode
		GLenum gPolygonMode = GL_FILL;

		// Light object
		std::shared_ptr<Light> gLight;

		// Camera
		std::shared_ptr<Camera> gCamera;

		// Asset Manager
		std::shared_ptr<AssetManager> gAssetManager;

		std::unique_ptr<Scene> scene;

		GLuint gPlainObjectShader;
		GLuint gTexturedObjectShader;

		// Load scenes, objects
		void LoadGameAssets();

		void InitializeShaders();

};

// External linkage such that the
// global variable is available
// everywhere.
extern GameState gGameState;

#endif