
#pragma once

// local
#include "processed_data.hpp"
#include "paths.hpp"

namespace tool {

struct NameState{
    bool filtered;
    bool saved;
    bool removed;
};

class Data : public QObject {
    Q_OBJECT
public:

    InputData iData;
    ProcessedData pData;

    std::unordered_map<FirstNameV, NameState> namesState;


    std::vector<FirstNameV> filteredNames;
    std::vector<FirstNameV> savedNames;
    std::vector<FirstNameV> removedNames;

    size_t countFiltered = 0;
    size_t countSaved = 0;
    size_t countRemoved = 0;

    FirstNameV currentName;

    bool init(){

        // read database file
        if(!iData.read_csv_file(Paths::csvFilePath)){
            return false;
        }

        // process data
        pData.generate(iData);

        namesState.reserve(iData.names.size());
        for(const auto &nameInfo : iData.namesInfo){
            namesState[nameInfo.first] = {true, false, false};
        }
        filteredNames.resize(iData.names.size());

        return true;
    }
};

}
