#version 330 core

// Fragment shader for drawing a reference grid.

out vec4 FragColor;
uniform float grid_resolution;
uniform int plane_mode; //X-Y plane, Y-Z plane or X-Z plane.

in vec3 pointCoordinate;

void main()
{
    float h_pos, v_pos;
    const float thin_line = 0.02f;
    const float thick_line = 0.03f;
    vec4 grid_color;

    switch (plane_mode)
    {
        case 0: //X-Y
            h_pos = pointCoordinate.x;
            v_pos = pointCoordinate.y;
            grid_color = vec4(0.5f, 0.0f, 0.0f, 1.0f);

            break;

        case 1: //X-Z
            h_pos = pointCoordinate.x;
            v_pos = pointCoordinate.z;
            grid_color = vec4(0.0f, 0.5f, 0.0f, 1.0f);

            break;

        case 2: //Y-Z
            h_pos = pointCoordinate.y;
            v_pos = pointCoordinate.z;
            grid_color = vec4(0.0f, 0.0f, 0.5f, 1.0f);

            break;
    }
    
    //Major lines

    //Minor lines
    if (fract(h_pos / (grid_resolution * 5.0f)) < thick_line || fract(v_pos / (grid_resolution * 5.0f)) < thick_line)
        FragColor = grid_color;
    else if (fract(h_pos / grid_resolution) < thin_line || fract(v_pos / grid_resolution) < thin_line)
        FragColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);
    else
        discard;
}
