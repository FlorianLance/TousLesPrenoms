

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
        infosPerDepartment[std::get<0>(dep)] = {std::get<0>(dep), initCount, initCount, 0., {}};
    }

    // init infos per year
    infosPerYear[Year{-1}] = {Year{-1}, initCount, {}};
    for(std::int16_t ii = StartYear.v; ii <= EndYear.v; ++ii){
        infosPerYear[Year{ii}] = {Year{ii}, initCount, {}};
    }

    // init infos per gender
    infosPerGender[Gender::Female]  = {Gender::Female,  initCount, {}, {}};//, {}};
    infosPerGender[Gender::Male]    = {Gender::Male,    initCount, {}, {}};//, {}};
    infosPerGender[Gender::Other]   = {Gender::Other,   initCount, {}, {}};//, {}};

    // init infos per year
    for(std::int16_t ii = StartYear.v; ii <= EndYear.v; ++ii){
        const Year y{ii};
        infosPerYear[y] = {y, initCount, {}};
    }

    // init infos per name
    for(const auto &nameInfo : inData.namesInfo){
        infosPerName[nameInfo.first] = {
            nameInfo.first,                 // name
            initCount,                      // count
            {},                             // gender repartition
            initInterval,                   // inteval apparition
            initYear,                       // year popularity peak
            initDep,                        // most common department
            initPopulparityPerPeriod,       // popularity per period
            initCountPerPeriod,             // count per period
            initOrderPerPeriod,             // order per period
            initCountPerGender              // count per gender
        };
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
    emit set_progress_signal(70);

    Bench::start("[ProcessedData::generate3]");

    std::for_each(std::execution::unseq, std::begin(inData.namesInfo), std::end(inData.namesInfo), [&](const auto &nameLineInfo){

        NameProcessedInfos &nameInfo = infosPerName[nameLineInfo.first];

        std::unordered_set<Year> years;
        std::unordered_set<Department> departments;
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
                if(!years.contains(info.year)){
                    years.insert(info.year);
                }
            }
            // # count
            nameInfo.total.v += info.count.v;
            // ## period
            auto period = get_period(info.year);
            if(period != Period::p1900_2020){
                nameInfo.countPerPeriod[static_cast<std::uint8_t>(period)].v += info.count.v;
                nameInfo.countPerPeriod[static_cast<std::uint8_t>(Period::p1900_2020)].v += info.count.v;
            }else{
                nameInfo.countPerPeriod[static_cast<std::uint8_t>(Period::p1900_2020)].v += info.count.v;
            }

            // ## gender
            nameInfo.countPerGender[static_cast<std::uint8_t>(info.gender)].v += info.count.v;

            // gender
            infosPerGender[info.gender].total.v += info.count.v;;
            infosPerGender[info.gender].counterName[nameLineInfo.first].v += info.count.v;
            infosPerGender[info.gender].countNamePeriod[period][nameLineInfo.first].v += info.count.v;

            // year
            infosPerYear[info.year].total.v += info.count.v;
            if(infosPerYear[info.year].counterName.count(nameLineInfo.first) == 0){
                infosPerYear[info.year].counterName[nameLineInfo.first] = {info.count.v};
            }else{
                infosPerYear[info.year].counterName[nameLineInfo.first].v += info.count.v;
            }

            // department
            if(info.department != Dep::Inconnu){
                auto &infoD = infosPerDepartment[info.department];
                infoD.total.v += info.count.v;

                if(infoD.infosPerName.count(nameLineInfo.first) == 0){
                    infoD.infosPerName[nameLineInfo.first].count = info.count;
                }else{
                    infoD.infosPerName[nameLineInfo.first].count.v += info.count.v;
                }

                auto &infoN = infoD.infosPerName[nameLineInfo.first];

                if(info.gender == Gender::Male){
                    infoN.nbMales.v += info.count.v;
                }else if(info.gender == Gender::Female){
                    infoN.nbFemales.v += info.count.v;
                }

                if(!departments.contains(info.department)){
                    departments.insert(info.department);
                }
            }
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
            const auto count = nameInfo.countPerPeriod[static_cast<std::uint8_t>(p)];
            infosPerPeriod[p].countPerName.emplace_back(
                nameLineInfo.first,
                count
            );
            if(count.v != 0){
                infosPerPeriod[p].total.v++;
            }
        }

        // gender repartition
        nameInfo.genderRepartition = compute_repartition(
            infosPerGender[Gender::Female].counterName[nameLineInfo.first].v,
            infosPerGender[Gender::Male].counterName[nameLineInfo.first].v,
            infosPerGender[Gender::Other].counterName[nameLineInfo.first].v,
            nameInfo.total.v
        );

        // department
        size_t maxCountPerDep = 0;
        for(const auto &dep : departments){            
            if(maxCountPerDep < infosPerDepartment[dep].infosPerName[nameInfo.name].count.v){
                maxCountPerDep = infosPerDepartment[dep].infosPerName[nameInfo.name].count.v;
                nameInfo.mostCommonDepartment = dep;
            }
        }
        if(maxCountPerDep != 0){
            infosPerDepartment[nameInfo.mostCommonDepartment].nbMostCommonNames.v += 1;
        }
    });

    Bench::stop();
    Bench::start("[ProcessedData::generate4]");

    for(auto &periodInfo : infosPerPeriod){
        std::sort(std::begin(periodInfo.second.countPerName), std::end(periodInfo.second.countPerName), [](auto &left, auto &right) {
            return left.count.v > right.count.v;
        });

        for(size_t ii = 0; ii < periodInfo.second.countPerName.size(); ++ii){
            const Order order{static_cast<std::uint16_t>(ii)};
            const auto idP = static_cast<std::uint8_t>(periodInfo.first);
            const auto count = periodInfo.second.countPerName[ii].count;
            const auto name = periodInfo.second.countPerName[ii].name;
            infosPerName[name].orderPerPeriod[idP] = order;
            if(count.v != 0){
                infosPerName[name].popularityPerPeriod[idP] = get_popularity(order, infosPerPeriod[periodInfo.first].total);
            }else{
                infosPerName[name].popularityPerPeriod[idP] = Popularity::Very_rare;
            }
        }
    }

    size_t max = 0;
    for(auto &depInfo : infosPerDepartment){

        if(depInfo.second.total.v > max){
            max = depInfo.second.total.v;
        }

        std::vector<NameCount> total;
        total.reserve(depInfo.second.infosPerName.size());
        for(auto &nameCount : depInfo.second.infosPerName){
            total.emplace_back(NameCount{nameCount.first, nameCount.second.count});
        }
        std::sort(std::begin(total), std::end(total), [](auto &left, auto &right) {
            return left.count.v > right.count.v;
        });

        std::uint16_t ii = 0;
        for(const auto &nameCount : total){
            const auto order = Order{ii++};
            depInfo.second.infosPerName[nameCount.name].order = order;
            depInfo.second.infosPerName[nameCount.name].popularity = get_popularity(order, nameCount.count);
        }
    }

    for(auto &depInfo : infosPerDepartment){
        depInfo.second.factorToMax = 1.0*depInfo.second.total.v / max;
    }

    Bench::stop();
    emit set_progress_signal(95);
}
