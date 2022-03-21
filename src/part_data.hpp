#ifndef PART_DATA_H
#define PART_DATA_H

#include "defines.h"
#include "colors.hpp"
#include "voxel.h"
#include <csv.hpp>

// Defines the representation of a Voxel, of a part, and implements
// a CSV importer.

inline glm::vec3 color_map(glm::vec3 color_input)
{
    glm::vec3 ret;
    ret.x = color_input.x;
    ret.y = 0.0;//color_input.y * color_input.y;
    ret.z = color_input.z;

    return ret;
}

struct PartData
{
    Voxel *data;

    // For now, we're dealing with a 1mm resolution in all axes, so the
    // beginning in each direction is easy.
    glm::vec3 size; // Stores the number of elements in each direction.
    glm::vec3 resolution; // The resolution of the voxels in this part.

    float start_x, start_y, start_z; // Spatial position
    glm::vec3 centroid;

    void alloc()
    {
    	data = (Voxel*) calloc(size.x * size.y * size.z, sizeof(Voxel));
	    #ifdef DEBUG
	        printf("PartData data ptr: %p\n", data);
	    #endif
    }

    void dealloc()
    {
	    free(data);
    }

    inline glm::vec3 origin_of_voxel(float x, float y, float z)
    {
        return { (float)((int)(x / resolution.x)) * resolution.x,
                 (float)((int)(y / resolution.y)) * resolution.y,
                 (float)((int)(z / resolution.z)) * resolution.z
                };
    }

    Voxel* voxel_at_coord(float x, float y, float z)
    {
        // Find which index it belongs to
        // Expects it to be 0 referenced.
        int i = x / resolution.x;
        int j = y / resolution.y;
        int k = z / resolution.z;

#ifdef DEBUG
        printf("Coord (%f %f %f) -> (%d %d %d)\n", x, y, z, i, j, k);
#endif

        return voxel_at(i, j, k);
    }

    Voxel* voxel_at(int i, int j, int k)
    {
        return data + (int) (i * size.z * size.y +
                             j * size.z +
                             k);
    }
};

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    BACK,
    FRONT
} PartDirection;

#define PART_SIZE 600

inline PartData generate_a_part()
{
    PartData ret_part_data;
    ret_part_data.size = glm::vec3(PART_SIZE);
    ret_part_data.data = (Voxel*) malloc(sizeof(Voxel) * PART_SIZE * PART_SIZE * PART_SIZE);
    printf("Data is: %p\n", ret_part_data.data);

    //vox_3d_mat_t &ret = ret_part_data->voxel_matrix;

    //ret.resize(PART_SIZE);
    //now set offsets
    for (int i = 0; i < PART_SIZE; ++i)
    {
        //ret[i].resize(PART_SIZE);
        for (int j = 0; j < PART_SIZE; ++j)
        {
            //ret[i][j].resize(PART_SIZE);
            for (int k = 0; k < PART_SIZE; ++k)
            {
                bool validity = false;
                //bool validity = true;
                //if (j < k && i < 40) validity = true;
                //if (i == j && j == k) validity = true;
                if (j == k) validity = true;
                if (j < 30 || k > 200) validity = true;
                if (j < 4) validity = true;


                Voxel temp_voxel = { .origin = glm::vec3((float)i, (float)j, (float)k),
                                     .valid = validity,
                                     .color = glm::vec3(per(i, PART_SIZE), per(j, PART_SIZE), per(k, PART_SIZE)) };
                #undef per

                *ret_part_data.voxel_at(i, j, k) = temp_voxel;
            }
        }
    }

    //Check if values are correct.
    //printf("Values should match beyond.\n");
    //for (int i = 0; i < PART_SIZE; ++i)
    //    for (int j = 0; j < PART_SIZE; ++j)
    //        for (int k = 0; k < PART_SIZE; ++k)
    //            ;//printf("Voxel at (%d,%d,%d), origin: (%f,%f,%f) %s\n", i, j, k, ret[i][j][k].origin.x, ret[i][j][k].origin.y, ret[i][j][k].origin.z, ret[i][j][k].valid ? "valid" : "invalid");

    return ret_part_data;
    //return {ret, glm::vec3(1.0, 1.0, 1.0)};
}

#undef PART_SIZE

#endif
