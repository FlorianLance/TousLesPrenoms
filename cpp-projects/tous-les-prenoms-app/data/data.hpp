
#pragma once

// local
#include "processed_data.hpp"
#include "paths.hpp"

namespace tool {

struct Data{

    InputData iData;
    ProcessedData pData;

    size_t countFiltered = 0;
    std::unordered_map<FirstNameV, bool> filteredNamesMask;
    std::vector<FirstNameV> filteredNames;

    FirstNameV currentName;

//    std::vector<FirstNameV> filteredNames;
//    std::vector<FirstNameV> savedNames;
//    std::vector<FirstNameV> removedNames;

    bool init(){

        // read database file
        if(!iData.read_csv_file(Paths::csvFilePath)){
            return false;
        }

        // process data
        pData.generate(iData);

        filteredNamesMask.reserve(iData.names.size());
        for(const auto &nameInfo : iData.namesInfo){
            filteredNamesMask[nameInfo.first] = true;
        }
        filteredNames.resize(iData.names.size());

        return true;
    }

};

}
