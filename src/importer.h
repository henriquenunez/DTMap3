//
// Created by hiram on 21/3/22.
//

#ifndef DTMAP3_IMPORTER_H
#define DTMAP3_IMPORTER_H

// TODO organize
#include <csv.hpp>
#include "part_data.hpp"
#include <string>

struct ValuesRanges
{
    float min_x;
    float max_x;

    float min_y;
    float max_y;

    float min_z;
    float max_z;

    float min_h;
    float max_h;
};

class PartDataCSVImporter
{
public:
    PartDataCSVImporter(std::string filename,
                        glm::vec3 resolution,
                        std::string column_h,
                        float min_h_param = 0.0f,
                        float max_h_param = 0.0f);

    PartData imported_part_data;
    ValuesRanges ranges;
    std::string column_h;

    ~PartDataCSVImporter()
    {}
private:
    ValuesRanges getValuesRanges(std::string filename);
    void processCSVData(std::string filename);

    float _min_h_param;
    float _max_h_param;
};

#endif //DTMAP3_IMPORTER_H
