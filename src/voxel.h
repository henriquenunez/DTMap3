//
// Created by hiram on 21/3/22.
//

#ifndef DTMAP3_VOXEL_H
#define DTMAP3_VOXEL_H

typedef struct
{
    glm::vec3 origin;
    bool valid;
    glm::vec3 color;
    float value;
    int value_count;
} Voxel;

typedef std::vector<Voxel> vox_arr_t;
typedef std::vector<vox_arr_t> vox_2d_mat_t;
typedef std::vector<vox_2d_mat_t> vox_3d_mat_t;

#endif //DTMAP3_VOXEL_H
