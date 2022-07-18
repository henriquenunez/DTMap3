// colorbar.hpp

#ifndef COLORBAR_H
#define COLORBAR_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <vector>

class Colorbar
{
public:
    Colorbar(float max, float min);
    ~Colorbar();
    
    void render();
    void bindVertexArray();

private:

    void initGL(const std::vector<float> &vtx_buf);

    shader_t* _shader_program;
    unsigned int _vao, _vbo;
    unsigned _triangle_count = 0;
};

#endif
