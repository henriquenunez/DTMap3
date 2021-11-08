// colorbar.hpp

#ifndef COLORBAR_H
#define COLORBAR_H

class Colorbar
{
public:
    Colorbar(float max, float min);
    ~Colorbar();
    
    render();

private:
    shader_t* _shader_program;
    unsigned int _vao, _vbo;
};

#endif
