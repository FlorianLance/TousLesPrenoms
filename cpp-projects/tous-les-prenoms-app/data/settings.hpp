

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
    std::unordered_map<GenderRepartition, QColor> genderRepartitionsForegroundColors ={
        {GenderRepartition::OnlyFemale,      {141,226,148}},
        {GenderRepartition::OnlyMale,        {185,177,207}},
        {GenderRepartition::OnlyOther,       {223,171,140}},
        {GenderRepartition::MostlyMale,      {185,177,207}},
        {GenderRepartition::MostlyFemale,    {141,226,148}},
        {GenderRepartition::MostlyOther,     {223,171,140}},
        {GenderRepartition::FemaleMale,      {115,187,215}},
        {GenderRepartition::FemaleOther,     {115,187,215}},
        {GenderRepartition::MaleOther,       {115,187,215}},
        {GenderRepartition::FemaleMaleOther, {115,187,215}},
        {GenderRepartition::Unknow,          {255,255,255}}
    };

    std::unordered_map<GenderRepartition, QColor> genderRepartitionsBackgroundColors ={
        {GenderRepartition::OnlyFemale,      {22,90,28}},
        {GenderRepartition::OnlyMale,        {69,60,98}},
        {GenderRepartition::OnlyOther,       {125,68,34}},
        {GenderRepartition::MostlyMale,      {69,60,98}},
        {GenderRepartition::MostlyFemale,    {22,90,28}},
        {GenderRepartition::MostlyOther,     {125,68,34}},
        {GenderRepartition::FemaleMale,      {31,85,105}},
        {GenderRepartition::FemaleOther,     {31,85,105}},
        {GenderRepartition::MaleOther,       {31,85,105}},
        {GenderRepartition::FemaleMaleOther, {31,85,105}},
        {GenderRepartition::Unknow,          {0,0,0}}
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

