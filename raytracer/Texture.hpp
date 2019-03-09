#include <string>
#include <glm/glm.hpp>
#include <vector>
using namespace std;
using namespace glm;

class Texture {
    public:
    Texture();
    virtual ~Texture();
    void loadPNG(string filename);
    vec3 read(float beta, float gamma);

    vec3 * pixels;
    unsigned int height;
    unsigned int width;
};