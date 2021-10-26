
#pragma once

// std
#include <stack>

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
    std::stack<FirstNameV> previousNames;

    size_t countFiltered = 0;
    size_t countSaved = 0;
    size_t countRemoved = 0;

    FirstNameV currentName;

    bool init();

    // I/O
    bool save_saved_names_file(const QString &path) const;
    bool save_removed_names_file(const QString &path) const;
    bool read_saved_names_file(const QString &path);
    bool read_removed_names_file(const QString &path);
};

}
