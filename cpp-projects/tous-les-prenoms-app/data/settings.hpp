

#pragma once

// Qt
#include <QString>
#include <QColor>

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
    // # peak
    bool peakBefore;
    bool peakAfter;
    bool peakDuring;
    bool peakUnknow;
    int peakYear;

    // period
    bool filterPopPeriod;

    // departments
    bool filterPopDep;
    QStringList insideDepartmentsTextes;

    // sorting
    bool sortAZ;
    bool sortZA;
    bool sortPopS;
    bool sortPopI;
};

struct DisplaySettings{

    QColor onlyFemaleName         = {141,226,148};
    QColor onlyFemaleBackground   = {22,90,28};
    QColor onlyMaleName           = {185,177,207};
    QColor onlyMaleBackground     = {69,60,98};
    QColor onlyOtherName          = {223,171,140};
    QColor onlyOtherBackground    = {125,68,34};

    QColor mostlyFemaleName       = {141,226,148};
    QColor mostlyFemaleBackground = {22,90,28};
    QColor mostlyMaleName         = {185,177,207};
    QColor mostlyMaleBackground   = {69,60,98};
    QColor mostlyOtherName        = {223,171,140};
    QColor mostlyOtherBackground  = {125,68,34};

    QColor femaleMaleName         = {115,187,215};
    QColor femaleMaleBackground   = {31,85,105};
    QColor femaleOtherName        = {115,187,215};
    QColor femaleOtherBackground  = {31,85,105};
    QColor maleOtherName        = {115,187,215};
    QColor maleOtherBackground  = {31,85,105};

    QColor femaleMaleOtherName        = {115,187,215};
    QColor femaleMaleOtherBackground  = {31,85,105};
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
