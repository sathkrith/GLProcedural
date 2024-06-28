#include "asset/ppm.hpp"
#include <sstream>
#include <string>
#include <fstream>
using namespace std;

bool PPM::isAscii(std::filesystem::path filePath) {
    std::ifstream image = OpenFile(filePath);
    if (!image.is_open()) {
        return false;
    }
    string line;
    getline(image, line);
    if (line != "P3") {
        std::cerr << "Invalid PPM file format. Expected P3." << std::endl;
        image.close();
        return false;
    }
    image.close();
    return true;
}

void SkipLineInIfStream(std::ifstream& inFile) {
    std::string line;
    while (inFile.peek() == '#') {
        std::getline(inFile, line);
    }
    return;
}

std::optional<PPM> PPM::LoadPPM(std::filesystem::path filePath){ 
    optional<PPM> ppm;
    std::ifstream image = OpenFile(filePath);
        if(!image.is_open()){
            return ppm;
    }
    string line;
    string token;
    stringstream stream;
    unsigned short width;
    unsigned short height;
    unsigned short maxColor;
    getline(image, line);
    if(line != "P3"){
        std::cerr << "Invalid PPM file format. Expected P3." << std::endl;
        image.close();
        return ppm;
    }

    SkipLineInIfStream(image);
    image >> width >> height;
    SkipLineInIfStream(image);
    image >> maxColor;
    SkipLineInIfStream(image);

    string word;
    cout << "height:" << height << " width:" << width <<" maxcolor:"<<maxColor<< endl;
    std::vector<unsigned char> pixels;
    pixels.reserve(height * width*3);
    int r, g, b;
    while (image >> r >> g >> b) {
        pixels.push_back(static_cast<unsigned char>(r * 255 / maxColor));
        pixels.push_back(static_cast<unsigned char>(g * 255 / maxColor));
        pixels.push_back(static_cast<unsigned char>(b * 255 / maxColor));
    }
    image.close();
    std::cout << "Successfully loaded image" << std::endl;
    ppm = PPM( filePath);
    ppm.value().width = width;
    ppm.value().height = height;
    ppm.value().maxColor = maxColor;
    ppm.value().pixels = pixels;
    ppm.value().mAssetState = AssetState::GOOD;
    ppm.value().format = GL_RGB;
    return ppm;
}


GLuint PPM::loadTextureToRender(std::filesystem::path path, bool& isLoadSuccessfull)
{
    GLuint textureID = 0;
    int width = 0, height = 0, nrComponents = 0;
    glGenTextures(1, &textureID);
    if (PPM::isAscii(path)) {
        std::optional<PPM> ppm = PPM::LoadPPM(path);
        if (ppm.has_value()) {
            glBindTexture(GL_TEXTURE_2D, textureID);
            ppm.value().FlipImageVertically();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ppm->width, ppm->height, 0, GL_RGB, GL_UNSIGNED_BYTE, &ppm.value().pixels[0]);
            GLint error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cout << "Error setting texture:" << error << "\n";
            }
            isLoadSuccessfull = true;
        }
    }
    else {
        return 0;
    }

    if (isLoadSuccessfull) {
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;

    }

    return textureID;
}