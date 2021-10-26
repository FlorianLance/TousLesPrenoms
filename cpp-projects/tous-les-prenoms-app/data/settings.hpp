

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
    bool filterLetters = true;
    bool nbLetters = true;
    int minLettersNb = 3;
    int maxLettersNb = 15;
    bool contains = false;
    QStringList containsTextes;
    bool startsBy = false;
    QStringList startsByTextes;
    bool endsBy = false;
    QStringList endsByTextes;
    bool doNoContains = false;
    QStringList doNotContainsTextes;

    // gender
    bool filterGender = false;
    bool onlyFemale = false;
    bool onlyMale = false;
    bool onlyOther = false;
    bool majorFemale = false;
    bool majorMale = false;
    bool majorOther = false;
    bool femaleMale = false;
    bool femaleOther = false;
    bool maleOther = false;

    // year
    bool filterYear = false;
    // # appears
    bool appearsBefore = false;
    bool appearsAfter = true;
    bool appearsDuring = false;
    bool appearsUnknow = false;
    int appearsYear = 1900;
    // # last appears
    bool lastAppearsBefore = true;
    bool lastAppearsAfter = false;
    bool lastAppearsDuring = false;
    bool lastAppearsUnknow = false;
    int lastAppearsYear = 2020;
    // # peak
    bool peakBefore = false;
    bool peakAfter = false;
    bool peakDuring = true;
    bool peakUnknow = false;
    int peakYear = 1988;

    // period
    bool filterPopPeriod  = false;
    Period period = Period::p1900_2020;
    bool periodAtLeast = false;
    bool periodEqual  = true;
    bool periodAtLast  = false;
    Popularity popPeriod = Popularity::Huge;

    // departments
    bool filterPopDep = false;
    std::unordered_set<Department> insideDepartments;
    bool depAtLeast = false;
    bool depEqual = true;
    bool depAtLast = false;
    Popularity popDep = Popularity::Huge;

    // sorting
    bool sortAZ = true;
    bool sortZA = false;
    bool sortPopS = false;
    bool sortPopI = false;
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

