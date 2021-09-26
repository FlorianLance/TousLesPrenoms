

#pragma once

// std
#include "unordered_map"

// local
#include "utility/types.hpp"
#include "first_name.hpp"
#include "department.hpp"
#include "year.hpp"
#include "gender.hpp"

namespace tool {

template<typename T1, typename T2>
using um = std::unordered_map<T1,T2>;

struct Info{
    Gender gender;
    Department department;
    Year year;
    Count count;
};

struct NameLineInfos{
    FirstNameV name;
    size_t id;
    std::vector<Info> infos;
};

struct InputData{
    QString dataBaseFileContent;
    QStringView dataBaseFileContentV;

    std::vector<FirstName> names;  // processed names, memory allocated only here
    um<RawFirstNameV, FirstNameV> namesCorr; // correspondance between raw name and proceessed ones
    um<FirstNameV, QVector<QStringView>> linesPerName; //
    um<FirstNameV, NameLineInfos> namesInfo;

    bool read_csv_file(const QString &csvFilePath);

private:
    const QString raresNames = "_prenoms_rares";
};



}
