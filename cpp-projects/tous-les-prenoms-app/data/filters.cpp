

#include "filters.hpp"

// std
#include <execution>
#include <algorithm>

using namespace tool;

bool Filters::filter_gender(const FilterSettings &s, const NameProcessedInfos &nameInfo) const{

    if(!s.filterGender){
        return true; // gender filtering disabled
    }

    // if at least one criterion is valid, keep the name
    // # only
    if(s.onlyFemale && nameInfo.genderRepartition == GenderRepartition::OnlyFemale){
        return true;
    }
    if(s.onlyMale && nameInfo.genderRepartition == GenderRepartition::OnlyMale){
        return true;
    }
//    if(s.onlyOther && nameInfo.genderRepartition == GenderRepartition::OnlyOther){
//        return true;
//    }
    // # major
    if(s.majorFemale && nameInfo.genderRepartition == GenderRepartition::MostlyFemale){
        return true;
    }
    if(s.majorMale && nameInfo.genderRepartition == GenderRepartition::MostlyMale){
        return true;
    }
//    if(s.majorOther && nameInfo.genderRepartition == GenderRepartition::MostlyOther){
//        return true;
//    }
    // # balanced
    if(s.femaleMale && nameInfo.genderRepartition == GenderRepartition::FemaleMale){
        return true;
    }
//    if(s.femaleOther && nameInfo.genderRepartition == GenderRepartition::FemaleOther){
//        return true;
//    }
//    if(s.maleOther && nameInfo.genderRepartition == GenderRepartition::MaleOther){
//        return true;
//    }

    // no criterion valid, drop the name
    return false;
}

bool Filters::filter_letters(const FilterSettings &s, FirstNameV firstNameV) const{

    if(!s.filterLetters){
        return true; // letter filtering disabled
    }

    // if at least one criterion is invalid, drop the name
    if(!filter_nb_letters(s, firstNameV)){
        return false;
    }
    if(!filter_contains(s, firstNameV)){
        return false;
    }
    if(!filter_do_not_contains(s, firstNameV)){
        return false;
    }
    if(!filter_starts_by(s, firstNameV)){
        return false;
    }
    if(!filter_ends_by(s, firstNameV)){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}


bool Filters::filter_nb_letters(const FilterSettings &s, FirstNameV firstNameV) const{

    if(!s.nbLetters){
        return true; // nb letter filtering disabled
    }

    // if at least one criterion is invalid, drop the name
    if(firstNameV.v.length() > s.maxLettersNb){
        return false;
    }
    if(firstNameV.v.length() < s.minLettersNb){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_contains(const FilterSettings &s, FirstNameV firstNameV) const{

    if(!s.contains){
        return true; // contains letter filtering disabled
    }


    // contains
    if(s.containsTextes.size() > 0){

        bool contains = true;
        for(const auto &text : s.containsTextes){
            if(!firstNameV.v.contains(text, Qt::CaseSensitivity::CaseInsensitive)){
                contains = false;
                break;
            }
        }

        if(!contains){
            // at least one criterion is invalid, drop the name
            return false;
        }
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_do_not_contains(const FilterSettings &s, FirstNameV firstNameV) const{

    if(!s.doNoContains){
        return true; // do not contains letter filtering disabled
    }

    // do not contains
    if(s.doNotContainsTextes.size() > 0){

        bool contains = false;
        for(const auto &text : s.doNotContainsTextes){
            if(firstNameV.v.contains(text, Qt::CaseSensitivity::CaseInsensitive)){
                contains = true;
                break;
            }
        }

        if(contains){
            // at least one criterion is invalid, drop the name
            return false;
        }
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_starts_by(const FilterSettings &s, FirstNameV firstNameV) const{

    if(!s.startsBy){
        return true; // starts by letter filtering disabled
    }


    // starts by
    if(s.startsByTextes.size() > 0){

        bool starts = false;
        for(const auto &text : s.startsByTextes){
            if(firstNameV.v.startsWith(text, Qt::CaseSensitivity::CaseInsensitive)){
                starts = true;
                break;
            }
        }

        if(!starts){
            // at least one criterion is invalid, drop the name
            return false;
        }
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_ends_by(const FilterSettings &s, FirstNameV firstNameV) const{

    if(!s.endsBy){
        return true; // ends by letter filtering disabled
    }

    // ends by
    if(s.endsByTextes.size() > 0){

        bool ends = false;
        for(const auto &text : s.endsByTextes){
            if(firstNameV.v.endsWith(text, Qt::CaseSensitivity::CaseInsensitive)){
                ends = true;
                break;
            }
        }

        if(!ends){
            // at least one criterion is invalid, drop the name
            return false;
        }
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_years(const FilterSettings &s, const NameProcessedInfos &nameInfo) const{

    if(!s.filterYear){
        return true; // years filtering disabled
    }

    // if at least one criterion is invalid, drop the name
    if(!filter_appears_before(s, nameInfo.intervalApparition)){
        return false;
    }
    if(!filter_appears_during(s, nameInfo.intervalApparition)){
        return false;
    }
    if(!filter_appears_after(s, nameInfo.intervalApparition)){
        return false;
    }
    if(!filter_appears_unknow(s, nameInfo.intervalApparition)){
        return false;
    }

    if(!filter_peak_before(s, nameInfo.popularityPeak)){
        return false;
    }
    if(!filter_peak_during(s, nameInfo.popularityPeak)){
        return false;
    }
    if(!filter_peak_after(s, nameInfo.popularityPeak)){
        return false;
    }
    if(!filter_peak_unknow(s, nameInfo.popularityPeak)){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_appears_before(const FilterSettings &s, const Interval &apparitionInterval) const{

    if(!s.appearsBefore){
        return true; // appears before filtering disabled
    }

    // if at least one criterion is invalid, drop the name
    if(apparitionInterval.start.v == -1){
        return false;
    }
    if(s.appearsYear < apparitionInterval.start.v){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_appears_during(const FilterSettings &s, const Interval &apparitionInterval) const{

    if(!s.appearsDuring){
        return true; // appears during filtering disabled
    }

    if(apparitionInterval.start.v == -1){
        return false;
    }

    if(s.appearsYear != apparitionInterval.start.v){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_appears_after(const FilterSettings &s, const Interval &apparitionInterval) const{

    if(!s.appearsAfter){
        return true; // appears after filtering disabled
    }

    if(apparitionInterval.end.v == -1){
        return false;
    }

    if(s.appearsYear > apparitionInterval.end.v){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_appears_unknow(const FilterSettings &s, const Interval &apparitionInterval) const{

    if(!s.appearsUnknow){
        return true; // appears unknow filtering disabled
    }

    if(apparitionInterval.start.v != -1){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_peak_before(const FilterSettings &s, const Year &popularityPeak) const{

    if(!s.peakBefore){
        return true; // peak before filtering disabled
    }

    if(popularityPeak.v == -1){
        return false;
    }

    if(s.peakYear < popularityPeak.v){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_peak_during(const FilterSettings &s, const Year &popularityPeak) const{

    if(!s.peakDuring){
        return true; // peak during filtering disabled
    }

    if(popularityPeak.v == -1){
        return false;
    }

    if(s.peakYear != popularityPeak.v){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_peak_after(const FilterSettings &s, const Year &popularityPeak) const{

    if(!s.peakAfter){
        return true; // peak after filtering disabled
    }

    if(popularityPeak.v == -1){
        return false;
    }

    if(s.peakYear > popularityPeak.v){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_peak_unknow(const FilterSettings &s, const Year &popularityPeak) const{

    if(!s.peakUnknow){
        return true; // peak unknow filtering disabled
    }

    if(popularityPeak.v != -1){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_popularity_period(const FilterSettings &s, const NameProcessedInfos &nameInfo) const{

    if(!s.filterPopPeriod){
        return true; // popularity period filtering disabled
    }

    const auto popularity = nameInfo.popularityPerPeriod[static_cast<int>(s.period)];
    if(s.periodAtLeast && (popularity > s.popPeriod)){
        return false;
    }
    if(s.periodEqual && (popularity != s.popPeriod)){
        return false;
    }
    if(s.periodAtLast && (popularity < s.popPeriod)){
        return false;
    }

    // no criterion invalid, keep the name
    return true;
}

bool Filters::filter_popularity_department(const FilterSettings &s, FirstNameV firstNameV, const um<Department, DepartmentProcessedInfos> &infoPerDepartment) const{

    if(!s.filterPopDep){
        return true; // popularity department filtering disabled
    }

    for(const auto dep : s.insideDepartments){

        const auto &infos = infoPerDepartment.at(dep).infosPerName;
        if(!infos.contains(firstNameV)){
            return false; // not sure yet...
        }

        const auto popularity = infos.at(firstNameV).popularity;
        if(s.depAtLeast && (popularity > s.popDep)){
            return false;
        }
        if(s.depEqual && (popularity != s.popDep)){
            return false;
        }
        if(s.depAtLast && (popularity < s.popDep)){
            return false;
        }
    }

    // no criterion invalid, keep the name
    return true;
}


bool Filters::is_filtered(const FilterSettings &s, FirstNameV firstNameV, Data &data) const{

    // if at least one criterion is invalid, drop the name
    if(!filter_gender(s, data.pData.infosPerName[firstNameV])){
        return false;
    }

    if(!filter_letters(s, firstNameV)){
        return false;
    }

    if(!filter_years(s, data.pData.infosPerName[firstNameV])){
        return false;
    }

    if(!filter_popularity_period(s, data.pData.infosPerName[firstNameV])){
        return false;
    }

    if(!filter_popularity_department(s,firstNameV,  data.pData.infosPerDepartment)){
        return false;
    }

    //    if(s.filterPopDep){

    //        bool found = false;
    //        for(const auto dep : s.insideDepartments){
    //            if(dep == data.pData.infosPerName[firstNameV].mostCommonDepartment){
    //                found = true;
    //                break;
    //            }
    //        }

    //        if(!found){
    //            return false;
    //        }
    //    }



    // no criterion invalid, keep the name
    return true;
}

void Filters::apply(const FilterSettings &s, Data &data){

    using namespace std;

    // filter
    for_each(execution::par_unseq, begin(data.iData.namesInfo), end(data.iData.namesInfo),[&](auto nameInfo){
        data.namesState[nameInfo.first].filtered = is_filtered(s, nameInfo.first, data);
    });

    data.countFiltered = 0;
    for(const auto &m : data.namesState){
        if(m.second.filtered){
            data.filteredNames[data.countFiltered++] = m.first;
        }
    }

    // sort names
    if(s.sortAZ){
        sort(execution::par_unseq, begin(data.filteredNames), begin(data.filteredNames) + data.countFiltered, [](auto &left, auto &right) {
            return left.v < right.v;
        });
    }else if(s.sortZA){
        sort(execution::par_unseq, begin(data.filteredNames), begin(data.filteredNames) + data.countFiltered, [](auto &left, auto &right) {
            return left.v > right.v;
        });
    }else if(s.sortPopS){
        sort(execution::par_unseq, begin(data.filteredNames), begin(data.filteredNames) + data.countFiltered, [&](auto &left, auto &right) {
            return data.pData.infosPerName[left].total.v < data.pData.infosPerName[right].total.v;
        });
    }else if(s.sortPopI){
        sort(execution::par_unseq, begin(data.filteredNames), begin(data.filteredNames) + data.countFiltered, [&](auto &left, auto &right) {
            return data.pData.infosPerName[left].total.v > data.pData.infosPerName[right].total.v;
        });
    }

    if((data.currentName.v.length() == 0) || (!data.namesState[data.currentName].filtered)){
        if(data.countFiltered > 0){
            data.currentName = data.filteredNames[0];
        }
    }

    // fill departments
//    insideDepartments.clear();
//    for(const auto &departementStr : insideDepartmentsTextes){
//        if(departementStr.length() > 0){
//            //            if(auto dep = get_department(departementStr.toInt()); dep.has_value()){
//            //                if(dep.value() != DepartmentC::Inconnu){
//            //                    insideDepartments.push_back(dep.value());
//            //                }
//            //            }

//        }
//    }


}


