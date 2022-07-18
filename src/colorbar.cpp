// colorbar.cpp

#include "colorbar.hpp"
#include "colormap.h"

void push3vec(std::vector<float> &vtx_buf, float a, float b, float c)
{
    vtx_buf.push_back(a);
    vtx_buf.push_back(b);
    vtx_buf.push_back(c);
}

Colorbar::Colorbar(float min, float max)
{
    std::vector<float> colorbarData;

    float startPosition = -0.8;
    float endPosition = 0.8;
    float offset = (endPosition - startPosition)/256;
    int j = 0;
    for (float i = startPosition ;
        i < endPosition ;
        i+= offset)
    {
        float Xa, Xb, Xc, Xd;
        float Ya, Yb, Yc, Yd;

        Ya = Yb = i;
        Yc = Yd = i + offset;

        Xa = Xc = 0.7;
        Xb = Xd = 0.9;

        float r = viridis[j * 3];
        float g = viridis[j * 3 + 1];
        float b = viridis[j * 3 + 2];

        // Lower triangle
        push3vec(colorbarData, Xa, Ya, 0.0);
        push3vec(colorbarData, r, g, b);

        push3vec(colorbarData, Xb, Yb, 0.0);
        push3vec(colorbarData, r, g, b);
        
        push3vec(colorbarData, Xd, Yd, 0.0);
        push3vec(colorbarData, r, g, b);

        _triangle_count++;

        // Upper triangle
        push3vec(colorbarData, Xa, Ya, 0.0);
        push3vec(colorbarData, r, g, b);
        
        push3vec(colorbarData, Xd, Yd, 0.0);
        push3vec(colorbarData, r, g, b);
        
        push3vec(colorbarData, Xc, Yc, 0.0);
        push3vec(colorbarData, r, g, b);

        _triangle_count++;

        if (j < 255) j++;
    }

    initGL(colorbarData);
}

Colorbar::~Colorbar()
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    unload_shader(_shader_program);
}

void Colorbar::render()
{
    bindVertexArray();

    //IMPORTANT: the third parameter is the number of vertices!
    glDrawArrays(GL_TRIANGLES, 0, _triangle_count * 3);
}

void Colorbar::initGL(const std::vector<float> &vtx_buf)
{
    shader_err s_err;
    _shader_program = load_shader("shaders/colorbar_vertex.glsl", "shaders/colorbar_fragment.glsl", &s_err);
    //TODO: error check here!

    //Init stuff
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glGenBuffers(1, &_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER,
            vtx_buf.size() * sizeof(float),
            &vtx_buf[0],
            GL_STATIC_DRAW);

    // Position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Voxel origin
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6*sizeof(float)));
    // glEnableVertexAttribArray(2);
    
    //NOT DEALING WITH TEXTURES FOR NOW.
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(1);

    //initTextures();
}


void Colorbar::bindVertexArray()
{
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, tex_0);

    use_shader(_shader_program);

    glBindVertexArray(_vao);
}
