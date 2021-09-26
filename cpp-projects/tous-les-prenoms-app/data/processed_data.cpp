

#include "processed_data.hpp"

// std
#include <execution>
#include <unordered_set>

// local
#include "utility/benchmark.hpp"
#include "utility/convert.hpp"
#include "utility/qt_str.hpp"

#include <QDebug>
#include <iostream>

using namespace tool;

void ProcessedData::generate(InputData &inData){

    Bench::start("[ProcessedData::generate1]");

    // init infos per department
    for(const auto &dep : departments.data){
        infosPerDepartment[std::get<0>(dep)] = {std::get<0>(dep), initC};
    }

    // init infos per year
    infosPerYear[Year{-1}] = {Year{-1}, initC, {}};
    for(std::int16_t ii = StartYear.v; ii <= EndYear.v; ++ii){
        infosPerYear[Year{ii}] = {Year{ii}, initC, {}};
    }

    // init infos per gender
    infosPerGender[Gender::Female]  = {Gender::Female,  initC, {}, {}};
    infosPerGender[Gender::Male]    = {Gender::Male,    initC, {}, {}};
    infosPerGender[Gender::Other]   = {Gender::Other,   initC, {}, {}};

    // init infos per year
    for(std::int16_t ii = StartYear.v; ii <= EndYear.v; ++ii){
        const Year y{ii};
        infosPerYear[y] = {y, initC, {}};

        // sub gender
        for(auto &infoPerGender : infosPerGender){
            infoPerGender.second.countNameYear[y] = {};
        }
    }

    // init infos per name
    for(const auto &nameInfo : inData.namesInfo){
        infosPerName[nameInfo.first] = {
            nameInfo.first,                 // name
            initC,                          // count
            {},                             // gender repartition
            initInterval,                   // inteval apparition
            initYear,                       // year popularity peak
            initPopulparityPerPeriod,       // popularity per period
            initCountPerPeriod,             // count per period
            initOrderPerPeriod,             // order per period
            initCountPerGender              // count per gender
        };

        // sub year
//        for(auto &infoPerYear : infosPerYear){
//            infoPerYear.second.counterName[nameInfo.first] = {0};
//        }

        // sub gender
//        for(auto &infoPerGender : infosPerGender){
//            infoPerGender.second.counterName[nameInfo.first] = {0};
//            for(auto &countName : infoPerGender.second.countNameYear){
//                countName.second[nameInfo.first] = {0};
//            }
//        }
    }


    Bench::stop();

    Bench::start("[ProcessedData::generate2]");
    std::for_each(std::execution::par_unseq, std::begin(inData.linesPerName), std::end(inData.linesPerName), [&](const auto &nameLines){

        FirstNameV firstName = nameLines.first;
        int idSep;
        Gender gender;
        Year year;
        Department dep;
        Count count;
        for(auto line : nameLines.second){

            idSep = line.indexOf(lineSep, 0, Qt::CaseSensitivity::CaseInsensitive);
            gender = Convert::to_sex(line.left(idSep));
            line = line.mid(idSep+1);

            idSep = line.indexOf(lineSep, 0, Qt::CaseSensitivity::CaseInsensitive);
            line = line.mid(idSep+1);

            idSep = line.indexOf(lineSep, 0, Qt::CaseSensitivity::CaseInsensitive);
            year = Convert::to_year(line.left(idSep));
            line = line.mid(idSep+1);


            idSep = line.indexOf(lineSep, 0, Qt::CaseSensitivity::CaseInsensitive);
            dep = Convert::to_department(line.left(idSep));

            count = {static_cast<size_t>(line.mid(idSep+1).toInt())};

            // store info
            inData.namesInfo[firstName].infos.emplace_back(Info{gender,dep,year,count});
        }
    });

    Bench::stop();

    Bench::start("[ProcessedData::generate3]");

    std::for_each(std::execution::unseq, std::begin(inData.namesInfo), std::end(inData.namesInfo), [&](const auto &nameLineInfo){

        NameProcessedInfos &nameInfo = infosPerName[nameLineInfo.first];

        std::unordered_set<Year> years;
        for(const auto &info : nameLineInfo.second.infos){

            // name
            if(info.year.v != -1){

                // # apparition start
                if(nameInfo.intervalApparition.start.v == -1){
                     nameInfo.intervalApparition.start = info.year;
                }else{
                    if(info.year.v < nameInfo.intervalApparition.start.v){
                        nameInfo.intervalApparition.start = info.year;
                    }
                }

                // # apparition end
                if(info.year.v > nameInfo.intervalApparition.end.v){
                    nameInfo.intervalApparition.end = info.year;
                }
                if(years.count(info.year) == 0){
                    years.insert(info.year);
                }
            }
            // # count
            nameInfo.total.v += info.count.v;
            // ## period
            nameInfo.countPerPeriod[static_cast<std::uint8_t>(get_period(info.year))].v += info.count.v;
            nameInfo.countPerPeriod[static_cast<std::uint8_t>(Period::p1900_2020)].v += info.count.v;
            // ## gender
            nameInfo.countPerGender[static_cast<std::uint8_t>(info.gender)].v += info.count.v;

            // gender
            infosPerGender[info.gender].total.v += info.count.v;;
            infosPerGender[info.gender].counterName[nameLineInfo.first].v += info.count.v;
            infosPerGender[info.gender].countNameYear[info.year][nameLineInfo.first].v += info.count.v;

            // year
            infosPerYear[info.year].total.v += info.count.v;
            infosPerYear[info.year].counterName[nameLineInfo.first].v += info.count.v;

            // department            
            infosPerDepartment[info.department].total.v += info.count.v;
        }

        // name
        size_t maxCountPerYear = 0;
        for(auto &year : years){
            if(maxCountPerYear < infosPerYear[year].counterName[nameInfo.name].v){
                maxCountPerYear = infosPerYear[year].counterName[nameInfo.name].v;
                nameInfo.popularityPeak = year;
            }
        }

        // period
        for(const auto &period : periods.data){
            const auto p = std::get<0>(period);
            infosPerPeriod[p].countPerName.emplace_back(
                nameLineInfo.first,
                nameInfo.countPerPeriod[static_cast<std::uint8_t>(p)]
            );
        }

        // gender repartition
        nameInfo.genderRepartition = compute_repartition(
            infosPerGender[Gender::Female].counterName[nameLineInfo.first].v,
            infosPerGender[Gender::Male].counterName[nameLineInfo.first].v,
            infosPerGender[Gender::Other].counterName[nameLineInfo.first].v,
            nameInfo.total.v
        );
    });

    Bench::stop();
    Bench::start("[ProcessedData::generate4]");

    for(auto &periodInfo : infosPerPeriod){
        std::sort(std::begin(periodInfo.second.countPerName), std::end(periodInfo.second.countPerName), [](auto &left, auto &right) {
            return left.second.v > right.second.v;
        });

        for(size_t ii = 0; ii < periodInfo.second.countPerName.size(); ++ii){
            const Order order{static_cast<std::uint16_t>(ii)};
            const auto idP = static_cast<std::uint8_t>(periodInfo.first);
            const auto n = periodInfo.second.countPerName[ii].first;
            infosPerName[n].orderPerPeriod[idP] = order;
            infosPerName[n].popularityPerPeriod[idP] = get_popularity(order, infosPerName[n].countPerPeriod[idP]);
        }
    }

    Bench::stop();


}
