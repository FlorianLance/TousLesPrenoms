

#pragma once

// local
#include "input_data.hpp"
#include "period.hpp"

namespace tool {


struct NameCount{
    FirstNameV name;
    Count count;
};



// pop : std::uint8_t
// count : std::uint32_t
// order : std::uint16_t
struct PopularityCountOrder{
    std::uint64_t popularity : 8, count : 32, order: 16, rest : 8;
};


struct NameProcessedInfos{
    FirstNameV name;
    Count total;
    GenderRepartition genderRepartition;
    Interval intervalApparition;
    Year popularityPeak;
    Department mostCommonDepartment;
    std::array<Popularity, static_cast<int>(Period::SizeEnum)> popularityPerPeriod;
    std::array<Count, static_cast<int>(Period::SizeEnum)> countPerPeriod;
    std::array<Order, static_cast<int>(Period::SizeEnum)> orderPerPeriod;
    std::array<Count, static_cast<int>(Gender::SizeEnum)> countPerGender;
};

struct GenderNameInfos{
    Count count{0};
};

struct GenderProcessedInfos{
    Gender gender;
    Count total{0};
    um<FirstNameV, Count> counterName;
    um<Period, um<FirstNameV, Count>> countNamePeriod;
};

struct YearProcessedInfos{
    Year year;
    Count total{0};
    um<FirstNameV, Count> counterName;
};

struct PeriodProcessedInfos{
    Period period;
    Count total{0};
    std::vector<NameCount> countPerName; // for sorting
};


struct DepNameInfos{
    Count count{0};
    Count nbMales{0};
    Count nbFemales{0};
    Order order{0};
    Popularity popularity{Popularity::Very_rare};
};

struct DepartmentProcessedInfos{
    Department department{Dep::Inconnu};
    Count total{0};
    Count nbMostCommonNames{0};
    um<FirstNameV, DepNameInfos> infosPerName;
};


struct ProcessedData{

    um<FirstNameV, NameProcessedInfos> infosPerName;
    um<Gender, GenderProcessedInfos> infosPerGender;
    um<Year, YearProcessedInfos> infosPerYear;
    um<Department, DepartmentProcessedInfos> infosPerDepartment;
    um<Period, PeriodProcessedInfos> infosPerPeriod;

    void generate(InputData &inData);

    constexpr static QChar lineSep = ';';
    constexpr static Popularity initPop = Popularity::Very_rare;
    constexpr static Year initYear = {-1};
    constexpr static Count initCount = {0};
    constexpr static Order initO = {0};
    constexpr static Interval initInterval = {initYear,initYear};
    constexpr static Department initDep = {Dep::Inconnu};
    constexpr static std::array<Popularity, static_cast<int>(Period::SizeEnum)> initPopulparityPerPeriod = {initPop,initPop,initPop,initPop,initPop,initPop,initPop,initPop,initPop};
    constexpr static std::array<Count, static_cast<int>(Period::SizeEnum)> initCountPerPeriod = {initCount,initCount,initCount,initCount,initCount,initCount,initCount,initCount,initCount};
    constexpr static std::array<Order, static_cast<int>(Period::SizeEnum)> initOrderPerPeriod = {initO,initO,initO,initO,initO,initO,initO,initO,initO};
    constexpr static std::array<Count, static_cast<int>(Gender::SizeEnum)> initCountPerGender = {initCount,initCount,initCount};
};

}
