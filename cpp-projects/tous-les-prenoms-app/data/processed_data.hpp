

#pragma once

// local
#include "input_data.hpp"
#include "period.hpp"

namespace tool {






struct NameProcessedInfos{
    FirstNameV name;
    Count total;
    GenderRepartition genderRepartition;
    Interval intervalApparition;
    Year popularityPeak;    
    std::array<Popularity, static_cast<int>(Period::SizeEnum)> popularityPerPeriod;
    std::array<Count, static_cast<int>(Period::SizeEnum)> countPerPeriod;
    std::array<Order, static_cast<int>(Period::SizeEnum)> orderPerPeriod;
    std::array<Count, static_cast<int>(Gender::SizeEnum)> countPerGender;
};

struct GenderProcessedInfos{
    Gender gender;
    Count total;
    um<FirstNameV, Count> counterName;
    um<Year, um<FirstNameV, Count>> countNameYear;
};

struct YearProcessedInfos{
    Year year;
    Count total;
    um<FirstNameV, Count> counterName;
};

struct PeriodProcessedInfos{
    Period period;
    std::vector<std::pair<FirstNameV, Count>> countPerName;
};

struct DepartmentProcessedInfos{
    Department department;
    Count total;
};


struct ProcessedData{

    um<FirstNameV, NameProcessedInfos> infosPerName;
    um<Gender, GenderProcessedInfos> infosPerGender;
    um<Year, YearProcessedInfos> infosPerYear;
    um<Department, DepartmentProcessedInfos> infosPerDepartment;
    um<Period, PeriodProcessedInfos> infosPerPeriod;

    void generate(InputData &inData);

    constexpr static QChar lineSep = ';';
    constexpr static Popularity initPop = Popularity::Inexistant;
    constexpr static Year initYear = {-1};
    constexpr static Count initC = {0};
    constexpr static Order initO = {0};
    constexpr static Interval initInterval = {initYear,initYear};
    constexpr static std::array<Popularity, static_cast<int>(Period::SizeEnum)> initPopulparityPerPeriod = {initPop,initPop,initPop,initPop,initPop,initPop,initPop,initPop,initPop,initPop};
    constexpr static std::array<Count, static_cast<int>(Period::SizeEnum)> initCountPerPeriod = {initC,initC,initC,initC,initC,initC,initC,initC,initC,initC};
    constexpr static std::array<Order, static_cast<int>(Period::SizeEnum)> initOrderPerPeriod = {initO,initO,initO,initO,initO,initO,initO,initO,initO,initO};
    constexpr static std::array<Count, static_cast<int>(Gender::SizeEnum)> initCountPerGender = {initC,initC,initC};
};

}
