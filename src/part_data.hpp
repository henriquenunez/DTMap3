#ifndef PART_DATA_H
#define PART_DATA_H

#include "csv.hpp"

// Defines the representation of a Voxel, of a part, and implmements
// a CSV importer.

glm::vec3 hsv2rgb(glm::vec3 in)
{
    double      hh, p, q, t, ff;
    long        i;
    glm::vec3   out;

    if(in.y <= 0.0) {       // < is bogus, just shuts up warnings
        out.x = in.z;
        out.y = in.z;
        out.z = in.z;
        return out;
    }
    hh = in.x;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.z * (1.0 - in.y);
    q = in.z * (1.0 - (in.y * ff));
    t = in.z * (1.0 - (in.y * (1.0 - ff)));

    switch(i) {
    case 0:
        out.x = in.z;
        out.y = t;
        out.z = p;
        break;
    case 1:
        out.x = q;
        out.y = in.z;
        out.z = p;
        break;
    case 2:
        out.x = p;
        out.y = in.z;
        out.z = t;
        break;

    case 3:
        out.x = p;
        out.y = q;
        out.z = in.z;
        break;
    case 4:
        out.x = t;
        out.y = p;
        out.z = in.z;
        break;
    case 5:
    default:
        out.x = in.z;
        out.y = p;
        out.z = q;
        break;
    }
    return out;     
}

glm::vec3 hsv_to_rgb(glm::vec3 hsv)
{
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    float c = s * v;
    float x = c * (1.0 - fabs((int)(h / 60.0) % 2 - 1.0));
    float m = v - c;

    glm::vec3 rgb;

    if      (h < 60)  rgb = {c, x, 0};
    else if (h < 120) rgb = {x, c, 0};
    else if (h < 180) rgb = {0, c, x};
    else if (h < 240) rgb = {0, x, c};
    else if (h < 300) rgb = {x, 0, c};
    else            rgb = {c, 0, x};

    rgb = rgb + m;
    return rgb;
}

// Implements the Blue-White-Red colormap
glm::vec3 bwr_colormap(float min, float max, float val)
{
    float mid = (max + min) / 2.0f;
    float r, g, b;
    float interp_factor;

    if (val < mid)
    {
        interp_factor = (val - min) / (mid - min); // How close to mid

        r = interp_factor;
        g = interp_factor;
        b = 1.0f;
    }
    else
    {
        interp_factor = (val - mid) / (max - mid); // How close to max

        r = 1.0f;
        g = 1.0f - interp_factor;
        b = 1.0f - interp_factor;
    }

    return glm::vec3(r, g, b);
}

// Implements the Cool-Warm colormap
glm::vec3 coolwarm_colormap(float min, float max, float val)
{
    glm::vec3 col_max(188, 32, 44);
    glm::vec3 col_mid(216, 219, 224);
    glm::vec3 col_min(67, 90, 204);

    col_max /= (float) 255.0f;
    col_mid /= (float) 255.0f;
    col_min /= (float) 255.0f;

    glm::vec3 ret_color;

    float mid = (max + min) / 2.0f;
    float interp_factor;

    if (val < mid)
    {
        interp_factor = (val - min) / (mid - min); // How close to mid
        ret_color = interp_factor * col_mid + (1.0f - interp_factor) * col_min;
    }
    else
    {
        interp_factor = (val - mid) / (max - mid); // How close to max
        ret_color = interp_factor * col_max + (1.0f - interp_factor) * col_mid;
    }

    return ret_color;
}

// Implements the HSV colormap
glm::vec3 hsv_colormap(float min, float max, float val)
{
    return hsv2rgb(glm::vec3(
        340 * ((val - min) / (max - min)),
        1.0f,
        1.0f));
}

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
    PartDataCSVImporter(std::string filename, float resolution, std::string column_h)
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

            if (!temp_voxel->valid)
            {
                temp_voxel->valid = true;
                temp_voxel->origin = glm::vec3(disc_x, disc_y, disc_z);
            }

            // Calculating average on voxel and adding a value.
            float value_until_now = (float)temp_voxel->value_count * temp_voxel->value;

            temp_voxel->value_count++;
            temp_voxel->value = (value_until_now + H)/(float)temp_voxel->value_count;
            temp_voxel->color = hsv_colormap(MinH, MaxH, temp_voxel->value);
 
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
