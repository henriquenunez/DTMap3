#ifndef REF_GRID_H
#define REF_GRID_H

#include <math.h>
#include <vector>
#include "defines.h"

/*
Quick note:

X is horizontal, with respect to screen coordinates.
Y is vertical,   with respect to screen coordinates.
Z is depth,      with respect to screen coordinates.

IN THE CURRENT COORDINATE SYSTEM/ENGINE/IMPLEMENTATION.
THIS MAY (AND SHOULD) CHANGE.ORG.
*/

#define GRID_EXTENT 100.0f
#define GRID_EPSILON -0.05f
/*
enum reference_grid_mode
{
    x_y = 0,
    x_z = 1,
    y_z = 2
};*/

class ReferenceGrid
{

public:
    ReferenceGrid(int ref_grid_mode)
    {
        //TODO fix this horrible hardcoding.

        // X Y Z only (for now).
        std::vector<float> vertex_buf;

        switch(ref_grid_mode)
        {
            case 0: //xy
                vertex_buf = {
                -GRID_EXTENT, -GRID_EXTENT, GRID_EPSILON, 
                 GRID_EXTENT, -GRID_EXTENT, GRID_EPSILON, 
                 GRID_EXTENT,  GRID_EXTENT, GRID_EPSILON, 
                 GRID_EXTENT,  GRID_EXTENT, GRID_EPSILON, 
                -GRID_EXTENT,  GRID_EXTENT, GRID_EPSILON, 
                -GRID_EXTENT, -GRID_EXTENT, GRID_EPSILON, 
                };
                break;

            case 1: //xz
                vertex_buf = {
                -GRID_EXTENT, GRID_EPSILON, -GRID_EXTENT, 
                 GRID_EXTENT, GRID_EPSILON, -GRID_EXTENT, 
                 GRID_EXTENT, GRID_EPSILON,  GRID_EXTENT, 
                 GRID_EXTENT, GRID_EPSILON,  GRID_EXTENT, 
                -GRID_EXTENT, GRID_EPSILON,  GRID_EXTENT, 
                -GRID_EXTENT, GRID_EPSILON, -GRID_EXTENT, 
                };
                break;

            case 2: //yz
                vertex_buf = {
                 GRID_EPSILON, -GRID_EXTENT, -GRID_EXTENT, 
                 GRID_EPSILON,  GRID_EXTENT, -GRID_EXTENT, 
                 GRID_EPSILON,  GRID_EXTENT,  GRID_EXTENT, 
                 GRID_EPSILON,  GRID_EXTENT,  GRID_EXTENT, 
                 GRID_EPSILON, -GRID_EXTENT,  GRID_EXTENT, 
                 GRID_EPSILON, -GRID_EXTENT, -GRID_EXTENT, 
                };
                break;
        }
        
        _triangle_count = 2; 
        _grid_resolution = 1.0f;
        _grid_mode = ref_grid_mode;

        // Memory layout is [f f f] for a vertex.
        // Adding vertices


        initGL(vertex_buf);

    	model = glm::mat4(1.0);
    	viewProjection = glm::mat4(1.0);
    }

    ~ReferenceGrid()
    {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
        unload_shader(_shader_program);
    }

    void bindVertexArray()
    {
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, tex_0);

        use_shader(_shader_program);

        glBindVertexArray(_vao);
    }

    void render()
    {
        //_cube_ref.bindVertexArray();
        bindVertexArray();

        //Uniforms from matrices.
        unsigned int    model_loc = glGetUniformLocation(_shader_program_id, "model"),
                        view_proj_loc  = glGetUniformLocation(_shader_program_id, "viewProjection"),
                        plane_mode_loc  = glGetUniformLocation(_shader_program_id, "plane_mode"),
                        grid_res_loc  = glGetUniformLocation(_shader_program_id, "grid_resolution");

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_proj_loc, 1, GL_FALSE, glm::value_ptr(viewProjection));
        glUniform1f(grid_res_loc, _grid_resolution);
        glUniform1i(plane_mode_loc, _grid_mode); //which plane

        //NOTE: the third parameter is the number of vertices!
        glDrawArrays(GL_TRIANGLES, 0, _triangle_count * 3);
    }

    void setViewProjectionMat(glm::mat4 new_vp)
    {
	   viewProjection = new_vp;
    }

    void setModelMat(glm::mat4 new_model)
    {
	   model = new_model;
    }

    void setOffset(glm::vec3 offset) { _offset = offset; }
    void setRotation(glm::vec3 vector, float angle)
    {
	   _rotation_vector = vector;
	   _rotation_angle = angle;
    }

    bool should_rotate;

private:
    //Matrices
    glm::mat4 model;
    glm::mat4 viewProjection;

    //PartData *_part_data;
    glm::vec3 _offset, _rotation_vector;
    float _rotation_angle;

    unsigned int tex_0;
    unsigned int _vao, _vbo;
    shader_t* _shader_program;
    unsigned int _shader_program_id;
    unsigned int _triangle_count;
    int _grid_mode;
    float _grid_resolution;

    void initGL(const std::vector<float> &vtx_buf)
    {
        //SHADER MUST BE LOADED AFTER OPENGL IS INITIATED IMPORTANT!!!!!!!!!!!
        shader_err s_err;
        _shader_program = load_shader("shaders/ref_grid_vertex.glsl", "shaders/ref_grid_fragment.glsl", &s_err);
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

                                                        //Stride is 3, since we only have pos attrib.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
        //glEnableVertexAttribArray(1);
    }
};

#endif
