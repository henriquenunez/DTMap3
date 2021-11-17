#ifndef PART_DATA_H
#define PART_DATA_H

#include "colors.hpp"
#include <csv.hpp>

// Defines the representation of a Voxel, of a part, and implmements
// a CSV importer.

glm::vec3 color_map(glm::vec3 color_input)
{
    glm::vec3 ret;
    ret.x = color_input.x;
    ret.y = 0.0;//color_input.y * color_input.y;
    ret.z = color_input.z;

    return ret;
}

#define per(a, b) (((float)a/(float)b)) ///((float)b)

typedef struct
{
    glm::vec3 origin;
    bool valid;
    glm::vec3 color;
    float value;
    int value_count;
    //glm::vec3 size; for now will let size constant
} Voxel;

typedef std::vector<Voxel> vox_arr_t;
typedef std::vector<vox_arr_t> vox_2d_mat_t;
typedef std::vector<vox_2d_mat_t> vox_3d_mat_t;

struct PartData
{
    Voxel *data;
    int size;

    // For now, we're dealing with a 1mm resolution in all axes, so the
    // beginning in each direction is easy.
    float start_x, start_y, start_z; // Spatial position
    glm::vec3 centroid;

    void alloc()
    {
	data = (Voxel*) calloc(size * size * size, sizeof(Voxel));
	#ifdef DEBUG
	    printf("PartData data ptr: %p", data);
	#endif
    }

    void dealloc()
    {
	free(data);
    }

    Voxel* voxel_at(int i, int j, int k)
    {
        return data +  (i * size * size +
                        j * size +
                        k);
    }
};

class PartDataCSVImporter
{
public:
    PartDataCSVImporter(std::string filename, float resolution, std::string column_h, float min_h_param = 0.0f, float max_h_param = 0.0f)
    {
    	csv::CSVReader csv_reader(filename);
    	csv::CSVReader csv_reader2(filename);

    	#define BIG_FLOAT 100000.0f
    	#define SMALL_FLOAT -100000.0f

        // To compute the centroids
        float center_x, center_y, center_z;
        center_x = center_y = center_z = 0.0f;

        //int voxel_n;

    	//Header is like this one.
    	//,X,Y,Z,Time,Feed,ID,LaserPower,SpotArea

    	float X, Y, Z, H;
    	
    	float MaxH = 0, MinH = BIG_FLOAT;

    	float min_x = BIG_FLOAT, min_y = BIG_FLOAT, min_z = BIG_FLOAT;
    	float max_x = SMALL_FLOAT, max_y = SMALL_FLOAT, max_z = SMALL_FLOAT;

    	for(auto &row : csv_reader)
    	{
    	    // Get one line of the file.
    	    X = row["X"].get<float>();
    	    Y = row["Y"].get<float>();
    	    Z = row["Z"].get<float>();
    	    //Feed = row["Feed"].get<int>();
    	    //LaserPower = row["LaserPower"].get<int>();
    	    H = row[column_h].get<float>();

    	    // Updating extents
    	    if (X > max_x) max_x = X;
    	    if (X < min_x) min_x = X;

    	    if (Y > max_y) max_y = Y;
    	    if (Y < min_y) min_y = Y;
    	    
    	    if (Z > max_z) max_z = Z;
            if (Z < min_z) min_z = Z;

            if (H > MaxH) MaxH = H;
    	    if (H < MinH) MinH = H;
    	    
            //if (Feed > MaxFeed) MaxFeed = Feed;
    	    //if (LaserPower > MaxLaserPower) MaxLaserPower = LaserPower;
    	}

    	// Now that you know the extents, allocate space for it.
    	imported_part_data.size =
    	    (int)((max_x - min_x) > (max_y - min_y) ?
    		((max_x - min_x) > (max_z - min_z) ?
    		    (max_x - min_x) :
    		    (max_z - min_z) ) :
    		((max_y - min_y) > (max_z - min_z) ?
    		    (max_y - min_y) :
    		    (max_z - min_z))) + 1;

        #ifdef DEBUG
    	   printf("Part size: %d\n", imported_part_data.size);
        #endif

    	// Imported_part_data
    	//imported_part_data.start_x = min_x;
    	//imported_part_data.start_y = min_y;
    	//imported_part_data.start_z = min_z;

    	imported_part_data.start_x = 0;
    	imported_part_data.start_y = 0;
    	imported_part_data.start_z = 0;

    	// Now, actually process the data.

    	Voxel* temp_voxel;
    	imported_part_data.alloc();

	if (min_h_param < 0.001f && max_h_param < 0.001f)
	{
	    min_h_param = MinH;
	    max_h_param = MaxH;
	}

    	for (auto row = csv_reader2.begin(); row != csv_reader2.end(); ++row)
    	{
    	    // Get one line of the file.
    	    X = (*row)["X"].get<float>();
    	    Y = (*row)["Y"].get<float>();
    	    Z = (*row)["Z"].get<float>();

            // Discretized values.
            int disc_x = (int)(X-min_x);
            int disc_y = (int)(Y-min_y);
            int disc_z = (int)(Z-min_z);

    	    //Feed = (*row)["Feed"].get<int>();
    	    //LaserPower = (*row)["LaserPower"].get<int>();
    	    H = (*row)[column_h].get<float>();

    	    // Insert into corresponding voxel.
    	    temp_voxel = imported_part_data.voxel_at(disc_x, disc_y, disc_z);

            /*
        	#ifdef DEBUG
        	    printf("Adding information on vox (%d,%d,%d)\n",
        		    (int)(X-min_x),
        		    (int)(Y-min_y),
                	    (int)(Z-min_z));

        	#endif
        	*/

            //center_x += (float)disc_x;
            //center_y += (float)disc_y;
            //center_z += (float)disc_z;
	    if (H > 0.001) // Just an epsilon
	    {

                if (!temp_voxel->valid)
                {
                    temp_voxel->valid = true;
                    temp_voxel->origin = glm::vec3(disc_x, disc_y, disc_z);
                }
    
                // Calculating average on voxel and adding a value.
                float value_until_now = (float)temp_voxel->value_count * temp_voxel->value;
    
                temp_voxel->value_count++;
                temp_voxel->value = (value_until_now + H)/(float)temp_voxel->value_count;
                temp_voxel->color = hsv_colormap(min_h_param, max_h_param, temp_voxel->value);
	    }
            //voxel_n++;
    	}

        //center_x /= (float) voxel_n;
        //center_y /= (float) voxel_n;
        //center_z /= (float) voxel_n;

        // imported_part_data.centroid = glm::vec3(center_x, center_y, center_z);

        printf("MinH: %f %f\n", MinH, MaxH);
    }

    PartData imported_part_data;

    ~PartDataCSVImporter()
    {}
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

PartData generate_a_part()
{
    PartData ret_part_data;
    ret_part_data.size = PART_SIZE;
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
