

#pragma once

// std
#include <map>
#include <numeric>
#include <unordered_map>

// local
#include "utility/types.hpp"
//#include "line_info.hpp"
#include "period.hpp"
#include "popularity.hpp"

namespace tool {


//struct NameInfo2{
//    size_t id = 0;
//    FirstNameV firstName;
//    Count totalCount = {0};
////    std::array<Count,  static_cast<int>(SexC::SizeEnum)> countPerSex;
////    std::unordered_map<int, std::array<Count,  static_cast<int>(Sex::SizeEnum)>> countPerYearPerSex;
////    std::unordered_map<int, std::array<Count,  static_cast<int>(Sex::SizeEnum)>> countPerSexPerDep;
////    std::array<std::array<Count, static_cast<int>(Sex::SizeEnum)>, 2020-1900+1> countPerYearPerSex;
//};


//struct NameInfo{

//    template <typename T1,typename T2>
//    using m = std::map<T1,T2>;

//    NameInfo() = default;

//    NameInfo(size_t id, const LineInfo &lineInfo) : id(id), firstName(lineInfo.firstName){

//        return;
//        std::fill(std::begin(countPerSex),          std::end(countPerSex), 0);
//        std::fill(std::begin(countPerPeriod),       std::end(countPerPeriod), 0);
//        std::fill(std::begin(popularityPerPeriod),  std::end(popularityPerPeriod), Popularity::Average);

//        for(auto &nbPerSex : nbPerYearPerSex){
//            std::fill(std::begin(nbPerSex), std::end(nbPerSex), 0);
//        }

////        for(const auto &department : departments.data){

////        }
////        for(size_t ii = 0; ii < departments.size(); ++ii){
////            std::get<0>(departmentsInfo[ii]) = std::get<0>(departmentMapping[ii]);
////            std::get<1>(departmentsInfo[ii]) = {0};
////            std::get<2>(departmentsInfo[ii]) = {0};
////            std::get<3>(departmentsInfo[ii]) = {Popularity::Average};
////        }
//        update(lineInfo);
//    }

//    void update(const LineInfo &info){

//        return;
////        // count / sex
////        countPerSex[static_cast<size_t>(info.sex)] += info.count.v;

////        // count / period
////        auto period = get_period(info.year);
////        countPerPeriod[static_cast<size_t>(period)]               += info.count.v;
////        countPerPeriod[static_cast<size_t>(Period::p1900_2020)]   += info.count.v;

////        // count / year
////        if(info.year.v >= 1900){
////            nbPerYearPerSex[static_cast<size_t>(info.year.v-1900)][static_cast<size_t>(info.sex)] += info.count.v;
////        }

////        // count / department
////        auto idDep = get_id(static_cast<DepartmentC>(info.department.v));
////        std::get<1>(departmentsInfo[idDep]).v += info.count.v;
//    }


//    size_t id = 0;
//    FirstName firstName;

//    std::array<size_t,      static_cast<int>(Gender::SizeEnum)> countPerSex;
//    std::array<size_t,      static_cast<int>(Period::SizeEnum)> countPerPeriod;
//    std::array<Popularity,  static_cast<int>(Period::SizeEnum)> popularityPerPeriod;

//    std::array<std::array<size_t, static_cast<int>(Gender::SizeEnum)>, 2020-1900+1> nbPerYearPerSex;

//    std::array<std::tuple<Department, Count, Rank, Popularity>, static_cast<int>(Department::SizeEnum)> departmentsInfo;
////    std::array<NameDepartmentInfo, static_cast<int>(DepartmentC::SizeEnum)> departmentsInfo_test;


//};


}
