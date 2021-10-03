

#include "filters.hpp"

// std
#include <execution>
#include <algorithm>

using namespace tool;

bool Filters::filter_gender(const FilterSettings &s, const NameProcessedInfos &nameInfo) const{

    if(s.filterGender){


        // only
        if(s.onlyFemale && nameInfo.genderRepartition == GenderRepartition::OnlyFemale){
            return false;
        }
        if(s.onlyMale && nameInfo.genderRepartition == GenderRepartition::OnlyMale){
            return false;
        }
        if(s.onlyOther && nameInfo.genderRepartition == GenderRepartition::OnlyOther){
            return false;
        }

        // major
        if(s.majorFemale && nameInfo.genderRepartition == GenderRepartition::MostlyFemale){
            return false;
        }
        if(s.majorMale && nameInfo.genderRepartition == GenderRepartition::MostlyMale){
            return false;
        }
        if(s.majorOther && nameInfo.genderRepartition == GenderRepartition::MostlyOther){
            return false;
        }

        // balanced
        if(s.femaleMale && nameInfo.genderRepartition == GenderRepartition::FemaleMale){
            return false;
        }
        if(s.femaleOther && nameInfo.genderRepartition == GenderRepartition::FemaleOther){
            return false;
        }
        if(s.maleOther && nameInfo.genderRepartition == GenderRepartition::MaleOther){
            return false;
        }

        return true;
    }
    return false;
}

bool Filters::filter_letters(const FilterSettings &s, FirstNameV firstNameV) const{

    if(s.filterLetters){

        if(filter_nb_letters(s, firstNameV)){
            return true;
        }
        if(filter_contains(s, firstNameV)){
            return true;
        }
        if(filter_do_not_contains(s, firstNameV)){
            return true;
        }
        if(filter_starts_by(s, firstNameV)){
            return true;
        }
        if(filter_ends_by(s, firstNameV)){
            return true;
        }
    }
    return false;
}


bool Filters::filter_nb_letters(const FilterSettings &s, FirstNameV firstNameV) const{

    if(s.nbLetters){
        if(firstNameV.v.length() > s.maxLettersNb){
            return true;
        }
        if(firstNameV.v.length() < s.minLettersNb){
            return true;
        }
    }
    return false;
}

bool Filters::filter_contains(const FilterSettings &s, FirstNameV firstNameV) const{

    if(s.contains){

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
                return true;
            }
        }
    }

    return false;
}

bool Filters::filter_do_not_contains(const FilterSettings &s, FirstNameV firstNameV) const{

    if(s.doNoContains){

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
                return true;
            }
        }
    }
    return false;
}

bool Filters::filter_starts_by(const FilterSettings &s, FirstNameV firstNameV) const{

    if(s.startsBy){

        bool starts = false;
        for(const auto &text : s.startsByTextes){
            if(firstNameV.v.startsWith(text, Qt::CaseSensitivity::CaseInsensitive)){
                starts = true;
                break;
            }
        }

        if(!starts){
            return true;
        }
    }
    return false;
}

bool Filters::filter_ends_by(const FilterSettings &s, FirstNameV firstNameV) const{

    if(s.endsBy){

        bool ends = false;
        for(const auto &text : s.endsByTextes){
            if(firstNameV.v.endsWith(text, Qt::CaseSensitivity::CaseInsensitive)){
                ends = true;
                break;
            }
        }

        if(!ends){
            return true;
        }
    }
    return false;
}

bool Filters::filter_years(const FilterSettings &s, const NameProcessedInfos &nameInfo) const{

    if(s.filterYear){

        if(s.appearsBefore){

            if(nameInfo.intervalApparition.start.v == -1){
                return true;
            }

            if(s.appearsYear < nameInfo.intervalApparition.start.v){
                return true;
            }
        }

        if(s.appearsDuring){

            if(nameInfo.intervalApparition.start.v == -1){
                return true;
            }

            if(s.appearsYear != nameInfo.intervalApparition.start.v){
                return true;
            }
        }

        if(s.appearsAfter){

            if(nameInfo.intervalApparition.end.v == -1){
                return true;
            }

            if(s.appearsYear > nameInfo.intervalApparition.end.v){
                return true;
            }
        }

        if(s.appearsUnknow){
            if(nameInfo.intervalApparition.start.v != -1){
                return true;
            }
        }

        if(s.peakBefore){

            if(nameInfo.popularityPeak.v == -1){
                return true;
            }

            if(s.peakYear < nameInfo.popularityPeak.v){
                return true;
            }
        }

        if(s.peakDuring){

            if(nameInfo.popularityPeak.v == -1){
                return true;
            }

            if(s.peakYear != nameInfo.popularityPeak.v){
                return true;
            }
        }

        if(s.peakAfter){

            if(nameInfo.popularityPeak.v == -1){
                return true;
            }

            if(s.peakYear > nameInfo.popularityPeak.v){
                return true;
            }
        }
        if(s.peakUnknow){
            if(nameInfo.popularityPeak.v != -1){
                return true;
            }
        }
    }

    return false;
}


bool Filters::is_filtered(const FilterSettings &s, FirstNameV firstNameV, Data &data) const{

    if(filter_gender(s, data.pData.infosPerName[firstNameV])){
        return true;
    }

    if(filter_letters(s, firstNameV)){
        return true;
    }

    if(filter_years(s, data.pData.infosPerName[firstNameV])){
        return true;
    }

    if(s.filterPopPeriod){

        const auto popularity = data.pData.infosPerName[firstNameV].popularityPerPeriod[static_cast<int>(s.period)];

        if(s.periodAtLeast){
            if(popularity > s.popPeriod){
                return true;
            }
        }
        if(s.periodEqual){
            if(popularity != s.popPeriod){
                return true;
            }
        }
        if(s.periodAtLast){
            if(popularity < s.popPeriod){
                return true;
            }
        }
    }

    return false;
}



void Filters::apply(const FilterSettings &s, Data &data){

    using namespace std;

    // filter
    for_each(execution::par_unseq, begin(data.iData.namesInfo), end(data.iData.namesInfo),[&](auto nameInfo){
        data.filteredNamesMask[nameInfo.first] = is_filtered(s, nameInfo.first, data);
    });

    data.countFiltered = 0;
    for(const auto &m : data.filteredNamesMask){
        if(!m.second){
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

    if(data.currentName.v.length() > 0){
        if(data.filteredNamesMask[data.currentName]){
            if(data.filteredNames.size() > 0){
                data.currentName = data.filteredNames[0];
            }
        }
    }



    //    if(ui.cbFilters1900_2018->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p1900_2020)],
    //            static_cast<Popularity>(ui.cbPop1900_2020->currentIndex()),
    //            ui.cbOpe1900_2020->currentIndex())){
    //            return true;
    //        }
    //    }

    //    if(ui.cbFilters1900_1924->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p1900_1924)],
    //            static_cast<Popularity>(ui.cbPop1900_1924->currentIndex()),
    //            ui.cbOpe1900_1924->currentIndex())){
    //            return true;
    //        }
    //    }

    //    if(ui.cbFilters1925_1949->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p1925_1949)],
    //            static_cast<Popularity>(ui.cbPop1925_1949->currentIndex()),
    //            ui.cbOpe1925_1949->currentIndex())){
    //            return true;
    //        }
    //    }

    //    if(ui.cbFilters1950_1969->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p1950_1969)],
    //            static_cast<Popularity>(ui.cbPop1950_1969->currentIndex()),
    //            ui.cbOpe1950_1969->currentIndex())){
    //            return true;
    //        }
    //    }


    //    if(ui.cbFilters1970_1979->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p1970_1979)],
    //            static_cast<Popularity>(ui.cbPop1970_1979->currentIndex()),
    //            ui.cbOpe1970_1979->currentIndex())){
    //            return true;
    //        }
    //    }


    //    if(ui.cbFilters1980_1989->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p1980_1989)],
    //            static_cast<Popularity>(ui.cbPop1980_1989->currentIndex()),
    //            ui.cbOpe1980_1989->currentIndex())){
    //            return true;
    //        }
    //    }


    //    if(ui.cbFilters1990_1999->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p1990_1999)],
    //            static_cast<Popularity>(ui.cbPop1990_1999->currentIndex()),
    //            ui.cbOpe1990_1999->currentIndex())){
    //            return true;
    //        }
    //    }

    //    if(ui.cbFilters2000_2009->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p2000_2009)],
    //            static_cast<Popularity>(ui.cbPop2000_2009->currentIndex()),
    //            ui.cbOpe2000_2009->currentIndex())){
    //            return true;
    //        }
    //    }

    //    if(ui.cbFilters2010_2018->isChecked()){
    //        if(apply_popularity_operator(
    //            info.popularityPerPeriod[static_cast<size_t>(Period::p2010_2020)],
    //            static_cast<Popularity>(ui.cbPop2010_2020->currentIndex()),
    //            ui.cbOpe2010_2020->currentIndex())){
    //            return true;
    //        }
    //    }

    //    if(ui.cbFiltersDepartment->isChecked()){
    //        for(const auto &dep : insideDepartments){
    //            auto id  = get_id(dep);
    ////            if(std::get<2>(info.departmentsInfo[id]).v > 5 ){
    ////                return true;
    ////            }
    //        }
    //    }

    //    return false;

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


