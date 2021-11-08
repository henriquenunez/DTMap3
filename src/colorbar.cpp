// colorbar.cpp

Colorbar::Colorbar(float min, float max)
{

}

Colorbar::~Colorbar()
{

}

Colorbar::render()
{

}

Colorbar::initGL(const std::vector<float> &vtx_buf)
{
    shader_err s_err;
    _shader_program = load_shader("shaders/colorbar_vertex.glsl", "shaders/colorbar_fragment.glsl", &s_err);
    //TODO: error check here!
    _shader_program_id = shader_get_id(_shader_program);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Voxel origin
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    //NOT DEALING WITH TEXTURES FOR NOW.
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(1);

    //initTextures();
}
