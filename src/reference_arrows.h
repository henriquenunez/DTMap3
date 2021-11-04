#ifndef REF_ARROW_H
#define REF_ARROW_H

struct reference_arrows_t
{
  unsigned int vao, vbo;
  shader_t *shader_program;
  unsigned int shader_program_id;
  unsigned int triangle_n = 0;

  reference_arrows_t()
  {
    float data[] = {
      // Layout: x y z - r g b
      0.0, 0.0, 0.0, 1.0, 0.0, 0.0, // X direction
      1.0, 0.0, 0.0, 1.0, 0.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 1.0, 0.0, // Y direction
      0.0, 1.0, 0.0, 0.0, 1.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 0.0, 1.0, // Z direction
      0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
    };

    // Buffers and shader
    shader_err s_err;
    shader_program = load_shader("shaders/ref_line_vertex.glsl", "shaders/ref_line_fragment.glsl", &s_err);

    //TODO: error check here!
    shader_program_id = shader_get_id(shader_program);

    //Init stuff
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(data),
            data,
            GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
  }

  ~reference_arrows_t()
  {
    unload_shader(shader_program);
  }

  void render()
  {
    // draw
    use_shader(shader_program);
    glBindVertexArray(vao);

    glm::mat4 mvp_trans;

    // Apply transformations
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
    //model = glm::translate(model, glm::vec3(0.0, 0.0, -5.0));
    model = glm::rotate(model, part_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, part_pitch, glm::vec3(1.0f, 0.0f, 0.0f));

    //glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    //glm::mat4 projection = glm::perspective(glm::radians(20.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

    //unsigned int model_loc = glGetUniformLocation(shader_program_id, "model");
    //glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

    //unsigned int view_loc = glGetUniformLocation(shader_program_id, "view");
    //glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

    //unsigned int projection_loc = glGetUniformLocation(shader_program_id, "projection");
    //glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    mvp_trans = model;

    unsigned int mvp_trans_loc = glGetUniformLocation(shader_program_id, "mvp_trans");
    glUniformMatrix4fv(mvp_trans_loc, 1, GL_FALSE, glm::value_ptr(mvp_trans));

    glDrawArrays(GL_LINES, 0, 6);
  }
};

#endif
