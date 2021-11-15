#version 330 core
out vec4 FragColor;
in vec3 colorInfo;

// For cropping
in vec3 voxelOrig;

uniform float cutoff_x;
uniform float cutoff_y;
uniform float cutoff_z;

uniform bool should_cut_x;
uniform bool should_cut_y;
uniform bool should_cut_z;

void main()
{
    if ((!should_cut_x || (voxelOrig.x > cutoff_x)) &&
        (!should_cut_y || (voxelOrig.y > cutoff_y)) &&
        (!should_cut_z || (voxelOrig.z < cutoff_z)))
    {
        FragColor = vec4(colorInfo.x, colorInfo.y, colorInfo.z, 1.0f);
    }
    else discard;
}
