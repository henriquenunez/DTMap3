//
// Created by hiram on 21/3/22.
//

#include "importer.h"
#include "colormap.h"

#include <glm/glm.hpp>

PartDataCSVImporter::PartDataCSVImporter(std::string filename,
                                         glm::vec3 resolution, // xyz resolution of each voxel
                                         std::string column_h,
                                         float min_h_param /*= 0.0f*/,
                                         float max_h_param /*= 0.0f*/) : column_h(column_h)
{
    // To compute the centroids
    float center_x, center_y, center_z;
    float X, Y, Z, H;

    center_x = center_y = center_z = 0.0f;
    ranges = getValuesRanges(filename);

    // Since we are still dealing with a complete matrix,
    // we just need to calculate which

    printf("\n\t\tValue ranges: \nX {%f %f}\nY {%f %f}\nZ {%f %f}\n\n", ranges.min_x, ranges.max_x, ranges.min_y, ranges.max_y, ranges.min_z, ranges.max_z);

    // Now that you know the extents, allocate space for it.
    float x_space = (ranges.max_x - ranges.min_x) / resolution.x;
    float y_space = (ranges.max_y - ranges.min_y) / resolution.y;
    float z_space = (ranges.max_z - ranges.min_z) / resolution.z;

    #ifdef DEBUG
        printf("Space needed: (%f,%f,%f)\n", x_space, y_space, z_space);
    #endif

    if (min_h_param < 0.001f && max_h_param < 0.001f)
    {
        _min_h_param = ranges.min_h;
        _max_h_param = ranges.max_h;
    }
    else
    {
        _min_h_param = min_h_param;
        _max_h_param = max_h_param;
    }

    imported_part_data.size = glm::vec3((int) x_space + 1, (int) y_space + 1, (int) z_space + 1);
    imported_part_data.resolution = resolution;

    #ifdef DEBUG
        printf("Part size: (%f,%f,%f)\n", imported_part_data.size.x, imported_part_data.size.y, imported_part_data.size.z);
    #endif

    // Imported_part_data
    //imported_part_data.start_x = min_x;
    //imported_part_data.start_y = min_y;
    //imported_part_data.start_z = min_z;

    // TODO is this even used ????
    imported_part_data.start_x = 0;
    imported_part_data.start_y = 0;
    imported_part_data.start_z = 0;

    imported_part_data.alloc();

    // Now, actually process the data.
    processCSVData(filename);

    #ifdef DEBUG
        printf("H range: %f %f\n", ranges.min_h, ranges.max_h);
    #endif
}

ValuesRanges PartDataCSVImporter::getValuesRanges(std::string filename)
{
    csv::CSVReader csv_reader(filename);

    float X, Y, Z, H; // Temporarily store the values
    float max_h = 0, min_h = BIG_FLOAT;
    float min_x, min_y, min_z;
    float max_x, max_y, max_z;

    min_x = min_y = min_z = BIG_FLOAT;
    max_x = max_y = max_z = SMALL_FLOAT;

    for (auto &row: csv_reader)
    {
        //Header is like this one.
        //,X,Y,Z,Time,Feed,ID,LaserPower,SpotArea
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

        if (H > max_h) max_h = H;
        if (H < min_h) min_h = H;

        //if (Feed > MaxFeed) MaxFeed = Feed;
        //if (LaserPower > MaxLaserPower) MaxLaserPower = LaserPower;
    }

    return { min_x, max_x, min_y, max_y, min_z, max_z, min_h, max_h };
}

void PartDataCSVImporter::processCSVData(std::string filename)
{
    float X, Y, Z, H; // Temporarily store the values
    Voxel *temp_voxel;
    csv::CSVReader csv_reader(filename);

    for (auto row = csv_reader.begin(); row != csv_reader.end(); ++row)
    {
        // Get one line of the file.
        X = (*row)["X"].get<float>();
        Y = (*row)["Y"].get<float>();
        Z = (*row)["Z"].get<float>();

        // Relative values because this is how it works.
        float rel_x = X - ranges.min_x;
        float rel_y = Y - ranges.min_y;
        float rel_z = Z - ranges.min_z;

        //Feed = (*row)["Feed"].get<int>();
        //LaserPower = (*row)["LaserPower"].get<int>();
        H = (*row)[column_h].get<float>();

        // Insert into corresponding voxel.
        temp_voxel = imported_part_data.voxel_at_coord(rel_x, rel_y, rel_z);

        #ifdef DEBUG
            printf("Adding information on vox (%f,%f,%f)\n",
                    rel_x,
                    rel_y,
                    rel_z);
        #endif

        //center_x += (float)disc_x;
        //center_y += (float)disc_y;
        //center_z += (float)disc_z;

        if (H > 0.001) // Just an epsilon
        {
            if (!temp_voxel->valid)
            {
                temp_voxel->valid = true;
                temp_voxel->origin = imported_part_data.origin_of_voxel(rel_x, rel_y, rel_z);
            }

            // Calculating average on voxel and adding a value.
            float value_until_now = (float) temp_voxel->value_count * temp_voxel->value;

            temp_voxel->value_count++;
            temp_voxel->value = (value_until_now + H) / (float) temp_voxel->value_count;
            temp_voxel->color = viridis_colormap(_min_h_param, _max_h_param, temp_voxel->value);
        }
        //voxel_n++;
    }

    //center_x /= (float) voxel_n;
    //center_y /= (float) voxel_n;
    //center_z /= (float) voxel_n;

    // imported_part_data.centroid = glm::vec3(center_x, center_y, center_z);

}
