

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

    QColor femaleCol1 = {141,226,148};
    QColor femaleCol2 = {22,90,28};
    QColor maleCol1 = {185,177,207};
    QColor maleCol2 = {69,60,98};
    QColor bothCol1 = {115,187,215};
    QColor bothCol2 = {31,85,105};
    QColor otherCol1 = {223,171,140};
    QColor otherCol2 = {125,68,34};
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
