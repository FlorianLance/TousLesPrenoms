

#pragma once

// std
#include <unordered_set>

// Qt
#include <QString>
#include <QColor>

// local
#include "gender.hpp"
#include "period.hpp"
#include "popularity.hpp"
#include "department.hpp"

namespace tool {

struct FilterSettings{

    // letters
    bool filterLetters;
    bool nbLetters;
    int minLettersNb;
    int maxLettersNb;
    bool contains;
    QStringList containsTextes;
    bool startsBy;
    QStringList startsByTextes;
    bool endsBy;
    QStringList endsByTextes;
    bool doNoContains;
    QStringList doNotContainsTextes;

    // gender
    bool filterGender;
    bool onlyFemale;
    bool onlyMale;
    bool onlyOther;
    bool majorFemale;
    bool majorMale;
    bool majorOther;
    bool femaleMale;
    bool femaleOther;
    bool maleOther;

    // year
    bool filterYear;
    // # appears
    bool appearsBefore;
    bool appearsAfter;
    bool appearsDuring;
    bool appearsUnknow;
    int appearsYear;
    // # last appears
    bool lastAppearsBefore;
    bool lastAppearsAfter;
    bool lastAppearsDuring;
    bool lastAppearsUnknow;
    int lastAppearsYear;
    // # peak
    bool peakBefore;
    bool peakAfter;
    bool peakDuring;
    bool peakUnknow;
    int peakYear;

    // period
    bool filterPopPeriod;
    Period period;
    bool periodAtLeast;
    bool periodEqual;
    bool periodAtLast;
    Popularity popPeriod;

    // departments
    bool filterPopDep;    
    std::unordered_set<Department> insideDepartments;
    bool depAtLeast;
    bool depEqual;
    bool depAtLast;
    Popularity popDep;

    // sorting
    bool sortAZ;
    bool sortZA;
    bool sortPopS;
    bool sortPopI;
};

struct DisplaySettings{
    std::unordered_map<GenderRepartition, QColor> genderRepartitionsBackgroundColors ={
        {GenderRepartition::OnlyFemale,      QColor{141,226,148}},
        {GenderRepartition::OnlyMale,        QColor{185,177,207}},
        {GenderRepartition::MostlyMale,      QColor{185,177,207}},
        {GenderRepartition::MostlyFemale,    QColor{141,226,148}},
        {GenderRepartition::FemaleMale,      QColor{115,187,215}},
        {GenderRepartition::Unknow,          QColor{255,255,255}}
//        {GenderRepartition::OnlyOther,       {223,171,140}},
//        {GenderRepartition::MostlyOther,     {223,171,140}},
//        {GenderRepartition::FemaleOther,     {115,187,215}},
//        {GenderRepartition::MaleOther,       {115,187,215}},
//        {GenderRepartition::FemaleMaleOther, {115,187,215}},        
    };

    std::unordered_map<GenderRepartition, QColor> genderRepartitionsForegroundColors ={
        {GenderRepartition::OnlyFemale,      QColor{22,90,28}},
        {GenderRepartition::OnlyMale,        QColor{69,60,98}},
        {GenderRepartition::MostlyMale,      QColor{69,60,98}},
        {GenderRepartition::MostlyFemale,    QColor{22,90,28}},
        {GenderRepartition::FemaleMale,      QColor{31,85,105}},
        {GenderRepartition::Unknow,          QColor{0,0,0}}
//        {GenderRepartition::MostlyOther,     {125,68,34}},
//        {GenderRepartition::OnlyOther,       {125,68,34}},
//        {GenderRepartition::FemaleOther,     {31,85,105}},
//        {GenderRepartition::MaleOther,       {31,85,105}},
//        {GenderRepartition::FemaleMaleOther, {31,85,105}},

    };
};

struct Settings{

    // lastname
    QString lastName = "...";
    // display colors


    FilterSettings filters;
    DisplaySettings display;

    bool save_settings_file(const QString &path);
    bool read_settings_file(const QString &path);
};

}

