#include "Texture.hpp"
#include "lodepng.hpp"
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
using namespace glm;

Texture::Texture() {}
Texture::~Texture() {
    //delete [] pixels;
}
void Texture::loadPNG(string filename) {
    //pixels.clear();
    vector<unsigned char> rawPng;
    unsigned error = lodepng::decode(rawPng, width, height, filename);
    if (error) {
        cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
        cout << "filename is " << filename << endl;
        return;
    }

    char byteType = 'r';
    int curPixel = 0;
    pixels = new vec3[height*width];
    for (unsigned char byte : rawPng) {
        if (byteType == 'r') {
            pixels[curPixel].r = byte/255.0f; 
            byteType = 'g';
        } else if (byteType == 'g') {
            pixels[curPixel].g = byte/255.0f; 
            byteType = 'b';
        } else if (byteType == 'b') {
            pixels[curPixel].b = byte/255.0f; 
            byteType = 'a';
        } else if (byteType == 'a') {
            curPixel++;
            byteType = 'r';
        }
    }
}

vec3 Texture::read(float beta, float gamma) {
    /*
    cout << "beta,gamma" << beta << "," <<  gamma << endl;
    cout << width << "," << height << endl;
    cout << "Pixel is" << endl;
    cout << pixels[1][1] << endl;
    */
    float alpha = 1 - beta - gamma;
    int x = alpha * (width-1);
    int y = gamma * (height-1);
    return pixels[y*width + x];
}