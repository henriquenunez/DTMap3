#ifndef PART_H
#define PART_H

#include <math.h>
#include <vector>
#include "defines.h"
#include "part_data.hpp"

// this renders the actual part TODO refactor to "renderer" or something

/*
Quick note:

X is horizontal, with respect to screen coordinates.
Y is vertical,   with respect to screen coordinates.
Z is depth,      with respect to screen coordinates.

IN THE CURRENT COORDINATE SYSTEM/ENGINE/IMPLEMENTATION.
THIS MAY (AND SHOULD) CHANGE.ORG.

I have made a representation from what we call a cube, defining an origin, vertices and relative offset.
*/

//Defining bitmasks for the cube and stuff.

#define Rx 0b100
#define Ry 0b010
#define Rz 0b001

#define c_vtx_0 0b000
#define c_vtx_1 0b100
#define c_vtx_2 0b110
#define c_vtx_3 0b010
#define c_vtx_4 0b001
#define c_vtx_5 0b101
#define c_vtx_6 0b111
#define c_vtx_7 0b011

typedef struct
{
    char v0;
    char v1;
    char v2;
} rel_triangle_t;

const rel_triangle_t a_tr = { c_vtx_0, c_vtx_3, c_vtx_1 }, b_tr = { c_vtx_2, c_vtx_1, c_vtx_3 }; //Face 0123
const rel_triangle_t c_tr = { c_vtx_0, c_vtx_4, c_vtx_7 }, d_tr = { c_vtx_0, c_vtx_7, c_vtx_3 }; //Face 0374
const rel_triangle_t e_tr = { c_vtx_5, c_vtx_1, c_vtx_2 }, f_tr = { c_vtx_5, c_vtx_2, c_vtx_6 }; //Face 5126
const rel_triangle_t g_tr = { c_vtx_5, c_vtx_4, c_vtx_0 }, h_tr = { c_vtx_5, c_vtx_0, c_vtx_1 }; //Face 5401
const rel_triangle_t i_tr = { c_vtx_2, c_vtx_3, c_vtx_7 }, j_tr = { c_vtx_2, c_vtx_7, c_vtx_6 }; //Face 2376
const rel_triangle_t k_tr = { c_vtx_6, c_vtx_7, c_vtx_4 }, l_tr = { c_vtx_6, c_vtx_4, c_vtx_5 }; //Face 6745

//Here should undef this stuff.
#undef c_vtx_0
#undef c_vtx_1
#undef c_vtx_2
#undef c_vtx_3
#undef c_vtx_4
#undef c_vtx_5
#undef c_vtx_6
#undef c_vtx_7

class PartRepresentation
{

public:
    PartRepresentation(PartData part_data)// : _part_data(part_data)
    {
        std::vector<float> vertex_buf;
        rel_triangle_t first_triangle_param, second_triangle_param;
        _offset = glm::vec3(0.0, 0.0, 0.0);
        _triangle_count = 0;
        int voxel_n = 0;

        float center_x = 0.0f;
        float center_y = 0.0f;
        float center_z = 0.0f;

        printf("Centroid is: %f %f %f\n", centroid.x, centroid.y, centroid.z);

        #define size part_data.size

        for (int i = 0; i < size.x; ++i)
            for (int j = 0; j < size.y; ++j)
                for (int k = 0; k < size.z; ++k)
                {
                    //printf("Iteration no. (%d,%d,%d).\n", i, j, k);
                    //printf("Voxel is %s\n", part_data.voxel_at(i, j, k)->valid ? "Valid" : "Invalid");
                    if (part_data.voxel_at(i, j, k)->valid)
                    {
                        center_x += (float)i;
                        center_y += (float)j;
                        center_z += (float)k;
                        voxel_n++;

            		    #ifdef DEBUG
            			printf("Voxel (%d,%d,%d) is valid.\n", i, j, k);
            		    #endif
                        //For every 6 faces, check if there's the need of adding it to the whole model.
                        //we check !valid, since we need to see if the adjacent voxel won't be shown

                        //Back
                        //if (k == 0 || (k > 0 && !part_data.voxel_at(i, j, k-1)->valid))
                        {
                            first_triangle_param  = a_tr;
                            second_triangle_param = b_tr;

                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k),  first_triangle_param); //First  face
                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k), second_triangle_param); //Second face
                            _triangle_count += 2;
                        }

                        //Front
                        //if (k == (size-1) || (k < size - 1 && !part_data.voxel_at(i, j, k+1)->valid))
                        {
                            first_triangle_param  = k_tr;
                            second_triangle_param = l_tr;

                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k),  first_triangle_param); //First  face
                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k), second_triangle_param); //Second face
                            _triangle_count += 2;
                        }

                        //Down
                        //if (j==0 || (j > 0 && !part_data.voxel_at(i, j-1, k)->valid))
                        {
                            first_triangle_param  = g_tr;
                            second_triangle_param = h_tr;

                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k),  first_triangle_param); //First  face
                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k), second_triangle_param); //Second face
                            _triangle_count += 2;
                        }

                        //Up
                        //if (j == size -1 || (j < size - 1 && !part_data.voxel_at(i, j+1, k)->valid))
                        {
                            first_triangle_param  = i_tr;
                            second_triangle_param = j_tr;

                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k),  first_triangle_param); //First  face
                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k), second_triangle_param); //Second face
                            _triangle_count += 2;
                        }

                        //Left
                        //if (i == 0 || (i > 0 && !part_data.voxel_at(i-1, j, k)->valid))
                        {
                            first_triangle_param  = c_tr;
                            second_triangle_param = d_tr;

                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k),  first_triangle_param); //First  face
                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k), second_triangle_param); //Second face
                            _triangle_count += 2;
                        }

                        //Right
                        //if (i == (size-1) || (i < size - 1 && !part_data.voxel_at(i+1, j, k)->valid))
                        {
                            first_triangle_param  = e_tr;
                            second_triangle_param = f_tr;

                            //TODO: change this bizarre thing.
                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k),  first_triangle_param); //First  face
                            addFaceTriangleToMesh(vertex_buf, part_data.resolution, part_data.voxel_at(i, j, k), second_triangle_param); //Second face
                            _triangle_count += 2;
                        }
                    }
                }
            #undef size

        center_x /= (float) voxel_n;
        center_y /= (float) voxel_n;
        center_z /= (float) voxel_n;

        centroid = glm::vec3(center_x, center_y, center_z);

        should_rotate = false;
        //Adding buffers to openGL.
        initGL(vertex_buf);

        //Setting projection matrix.
        //projection_matrix = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 2000.0f);
        model = glm::mat4(1.0);
        viewProjection = glm::mat4(1.0);
    }

    ~PartRepresentation()
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

        //Create transformations

        //if (should_rotate) model = glm::rotate(model, (float)glfwGetTime() * 2, glm::vec3(0.0f, 1.0f, 0.0f));
        //else model = glm::rotate(model, _rotation_angle, _rotation_vector);
        ////view = glm::translate(view, _offset);
        //projection = projection_matrix;

        // Rotate after translating part
        model = model * glm::translate(glm::mat4(1.0), centroid * -1.0f);

        //Uniforms from matrices.
        unsigned int    model_loc = glGetUniformLocation(_shader_program_id, "model"),
                        view_proj_loc  = glGetUniformLocation(_shader_program_id, "viewProjection");

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_proj_loc, 1, GL_FALSE, glm::value_ptr(viewProjection));

        //IMPORTANT: the third parameter is the number of vertices!
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
/*
    void setAutoRotation(bool rotate)
    {
        should_rotate = rotate;
    }
*/
    /* Not needed, we can handle more stuff from outside the class.
    void setProjection(bool ortho)
    {
        if (ortho)
            projection_matrix = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 2000.0f);
        else
            projection_matrix = glm::ortho(0.0f, 40.0f, 0.0f, 30.0f, 0.0f, 1000.0f);
    }*/
    bool should_rotate;
    bool crop_x, crop_y, crop_z;
    //float crop_x_amount, crop_y_amount, crop_z_amount;

    void set_crop_x(bool yes, float cutoff)
    {
        crop_x = yes;

        // Setting uniforms
        set_uniform_float(_shader_program, cutoff, "cutoff_x");
        set_uniform_int(_shader_program, yes, "should_cut_x");
    }

    void set_crop_y(bool yes, float cutoff)
    {
        crop_y = yes;

        // Setting uniforms
        set_uniform_float(_shader_program, cutoff, "cutoff_y");
        set_uniform_int(_shader_program, yes, "should_cut_y");
    }

    void set_crop_z(bool yes, float cutoff)
    {
        crop_z = yes;

        // Setting uniforms
        set_uniform_float(_shader_program, cutoff, "cutoff_z");
        set_uniform_int(_shader_program, yes, "should_cut_z");
    }

private:
    //Matrices
    glm::mat4 model;
    glm::mat4 viewProjection;

    //PartData *_part_data;
    glm::vec3 _offset, _rotation_vector, centroid;
    float _rotation_angle;

    unsigned int tex_0;
    unsigned int _vao, _vbo;
    shader_t* _shader_program;
    unsigned int _shader_program_id;

    size_t _triangle_count;

    //Adding one of the square's face triangles to the mesh.
    void addFaceTriangleToMesh(std::vector<float> &buf, glm::vec3 resolution, Voxel *voxel, rel_triangle_t cube_face_triangle)
    {
        //We are going to add 3 vertices here.
        float x, y, z;
        char op;
        glm::vec3 &color_info = voxel->color;

        //V0
        op = cube_face_triangle.v0;

        x = voxel->origin.x;
        y = voxel->origin.y;
        z = voxel->origin.z + 1.0; //TODO: I BEG YOU, FIX THIS.

        if (Rx & op) x += resolution.x;
        if (Ry & op) y += resolution.y;
        if (Rz & op) z -= resolution.z; // THIS IS BECAUSE OF MY MISTAKE IN THE RIGHT HAND RULE.
        buf.push_back(x); buf.push_back(y); buf.push_back(z);
        buf.push_back(color_info.x); buf.push_back(color_info.y); buf.push_back(color_info.z);
        buf.push_back(voxel->origin.x); buf.push_back(voxel->origin.y); buf.push_back(voxel->origin.z);

        //V1
        op = cube_face_triangle.v1;

        x = voxel->origin.x;
        y = voxel->origin.y;
        z = voxel->origin.z + 1.0; //TODO: I BEG YOU, FIX THIS.

        if (Rx & op) x += resolution.x;
        if (Ry & op) y += resolution.y;
        if (Rz & op) z -= resolution.z;
        buf.push_back(x); buf.push_back(y); buf.push_back(z);
        buf.push_back(color_info.x); buf.push_back(color_info.y); buf.push_back(color_info.z);
        buf.push_back(voxel->origin.x); buf.push_back(voxel->origin.y); buf.push_back(voxel->origin.z);

        //V2
        op = cube_face_triangle.v2;

        x = voxel->origin.x;
        y = voxel->origin.y;
        z = voxel->origin.z + 1.0; //TODO: I BEG YOU, FIX THIS.

        if (Rx & op) x += resolution.x;
        if (Ry & op) y += resolution.y;
        if (Rz & op) z -= resolution.z;
        buf.push_back(x); buf.push_back(y); buf.push_back(z);
        buf.push_back(color_info.x); buf.push_back(color_info.y); buf.push_back(color_info.z);
        buf.push_back(voxel->origin.x); buf.push_back(voxel->origin.y); buf.push_back(voxel->origin.z);
    }

    void initGL(const std::vector<float> &vtx_buf)
    {
        //SHADER MUST BE LOADED AFTER OPENGL IS INITIATED IMPORTANT!!!!!!!!!!!
        shader_err s_err;
        _shader_program = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl", &s_err);
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

    /*void initTextures() IM NOT DEALING WITH TEXTURES NOW.
    {
        // load and create a texture 
        // -------------------------
        // texture 1
        // ---------
        glGenTextures(1, &tex_0);
        glBindTexture(GL_TEXTURE_2D, tex_0);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        //unsigned char *data = stbi_load("awesome.jpg", &width, &height, &nrChannels, 0);
        unsigned char *data = stbi_load("prism.png", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        
        // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        use_shader(_shader_program);
        //get uniform by value and set it ourShader.setInt("tex_0", 0);
        //ourShader.setInt("texture2", 1);
        _shader_program_id = shader_get_id(_shader_program);

        unsigned int tex_loc = glGetUniformLocation(shader_get_id(_shader_program), "tex_0");
        glUniform1i(tex_loc, 0);
    }*/
};

#endif
