

#include "settings.hpp"

// Qt
#include <QFile>
#include <QDebug>

// local
#include "utility/convert.hpp"

using namespace tool;

bool Settings::save_settings_file(const QString &path){

    QFile file(path);
    if(!file.open(QFile::WriteOnly | QIODevice::Text)){
        qWarning() << "Cannot open settings names file at path " % path;
        return false;
    }

    QTextStream fs(&file);

    fs << lastName << "\n";

    // display
    const auto &d = display;
    using GR = GenderRepartition;
    for(auto gr : {GR::OnlyFemale, GR::OnlyMale, GR::MostlyMale, GR::MostlyFemale, GR::FemaleMale, GR::Unknow}){
        fs << Convert::to_str(d.genderRepartitionsForegroundColors.at(gr)) << "\n";
        fs << Convert::to_str(d.genderRepartitionsBackgroundColors.at(gr)) << "\n";
    }

    // filters
    const auto &f = filters;
    // # letters
    fs << (f.filterLetters ? "1" : "0")<< "\n";
    fs << (f.nbLetters ? "1" : "0")<< "\n";
    fs << f.minLettersNb<< "\n";
    fs << f.maxLettersNb<< "\n";
    fs << (f.contains ? "1" : "0")<< "\n";
    fs << f.containsTextes.join(";")<< "\n";
    fs << (f.startsBy ? "1" : "0")<< "\n";
    fs << f.startsByTextes.join(";")<< "\n";
    fs << (f.endsBy ? "1" : "0")<< "\n";
    fs << f.endsByTextes.join(";")<< "\n";
    fs << (f.doNoContains ? "1" : "0")<< "\n";
    fs << f.doNotContainsTextes.join(";")<< "\n";

    // # gender
    fs << (f.filterGender ? "1" : "0")<< "\n";
    fs << (f.onlyFemale ? "1" : "0")<< "\n";
    fs << (f.onlyMale ? "1" : "0")<< "\n";
    fs << (f.onlyOther ? "1" : "0")<< "\n";
    fs << (f.majorFemale ? "1" : "0")<< "\n";
    fs << (f.majorMale ? "1" : "0")<< "\n";
    fs << (f.majorOther ? "1" : "0")<< "\n";
    fs << (f.femaleMale ? "1" : "0")<< "\n";
    fs << (f.femaleOther ? "1" : "0")<< "\n";
    fs << (f.maleOther ? "1" : "0")<< "\n";

    // year
    fs << (f.filterYear ? "1" : "0")<< "\n";
    fs << (f.appearsBefore ? "1" : "0")<< "\n";
    fs << (f.appearsAfter ? "1" : "0")<< "\n";
    fs << (f.appearsDuring ? "1" : "0")<< "\n";
    fs << (f.appearsUnknow ? "1" : "0")<< "\n";
    fs << f.appearsYear << "\n";
    fs << (f.lastAppearsBefore ? "1" : "0") << "\n";
    fs << (f.lastAppearsAfter ? "1" : "0") << "\n";
    fs << (f.lastAppearsDuring ? "1" : "0") << "\n";
    fs << (f.lastAppearsUnknow ? "1" : "0") << "\n";
    fs << f.lastAppearsYear << "\n";
    fs << (f.peakBefore ? "1" : "0") << "\n";
    fs << (f.peakAfter ? "1" : "0") << "\n";
    fs << (f.peakDuring ? "1" : "0") << "\n";
    fs << (f.peakUnknow ? "1" : "0") << "\n";
    fs << f.peakYear << "\n";

//    // period
//    fs << (f.filterPopPeriod ? "1" : "0") << "\n";
//    fs << QString::number(static_cast<int>(f.period));
//    fs << (f.periodAtLeast ? "1" : "0") << "\n";
//    fs << (f.periodEqual ? "1" : "0") << "\n";
//    fs << (f.periodAtLast ? "1" : "0") << "\n";
//    fs << QString::number(static_cast<int>(f.popPeriod));

//    // departments
//    fs << (f.filterPopDep ? "1" : "0") << "\n";
//    QStringList deps;
//    for(const auto &d : f.insideDepartments){
//        deps  << QString::number(static_cast<std::uint8_t>(d));
//    }
//    fs << deps.join(";");
//    fs << (f.depAtLeast ? "1" : "0") << "\n";
//    fs << (f.depEqual ? "1" : "0") << "\n";
//    fs << (f.depAtLast ? "1" : "0") << "\n";
//    fs << static_cast<std::uint8_t>(f.popDep);

//    // sorting
//    fs << (f.sortAZ ? "1" : "0") << "\n";
//    fs << (f.sortZA ? "1" : "0") << "\n";
//    fs << (f.sortPopS ? "1" : "0") << "\n";
//    fs << (f.sortPopI ? "1" : "0") << "\n";


    return true;
}



bool Settings::read_settings_file(const QString &path){

    QFile settingsFile(path);
    if(!settingsFile.open(QFile::ReadOnly | QIODevice::Text)){
        qWarning() << "Cannot open settings file at path " % path;
        return false;
    }

    QTextStream fs(&settingsFile);
    QStringList content = fs.readAll().split("\n");

    int id = 0;

    // display
    lastName = content[id++];
    auto &d = display;
    using GR = GenderRepartition;
    for(auto gr : {GR::OnlyFemale, GR::OnlyMale, GR::MostlyMale, GR::MostlyFemale, GR::FemaleMale, GR::Unknow}){
        d.genderRepartitionsForegroundColors[gr] = Convert::to_color(content[id++]);
        d.genderRepartitionsBackgroundColors[gr] = Convert::to_color(content[id++]);
    }



    // filters
    auto &f = filters;
    // # letters
    f.filterLetters = content[id++] == "1";
    f.nbLetters = content[id++] == "1";
    f.minLettersNb = content[id++].toInt();
    f.maxLettersNb = content[id++].toInt();
    f.contains = content[id++] == "1";
    f.containsTextes = content[id++].split(";");
    f.startsBy = content[id++] == "1";
    f.startsByTextes = content[id++].split(";");
    f.endsBy = content[id++] == "1";
    f.endsByTextes = content[id++].split(";");
    f.doNoContains = content[id++] == "1";
    f.doNotContainsTextes = content[id++].split(";");

    // # gender
    f.filterGender = content[id++] == "1";
    f.onlyFemale = content[id++] == "1";
    f.onlyMale = content[id++] == "1";
    f.onlyOther = content[id++] == "1";
    f.majorFemale = content[id++] == "1";
    f.majorMale = content[id++] == "1";
    f.majorOther = content[id++] == "1";
    f.femaleMale = content[id++] == "1";
    f.femaleOther = content[id++] == "1";
    f.maleOther = content[id++] == "1";

    // year
    f.filterYear = content[id++] == "1";
    f.appearsBefore = content[id++] == "1";
    f.appearsAfter = content[id++] == "1";
    f.appearsDuring = content[id++] == "1";
    f.appearsUnknow = content[id++] == "1";
    f.appearsYear = content[id++].toInt();
    f.lastAppearsBefore = content[id++] == "1";
    f.lastAppearsAfter = content[id++] == "1";
    f.lastAppearsDuring = content[id++] == "1";
    f.lastAppearsUnknow = content[id++] == "1";
    f.lastAppearsYear = content[id++].toInt();
    f.peakBefore = content[id++] == "1";
    f.peakAfter = content[id++] == "1";
    f.peakDuring = content[id++] == "1";
    f.peakUnknow = content[id++] == "1";
    f.peakYear = content[id++].toInt();

//    // period
//    f.filterPopPeriod = content[id++] == "1";
//    f.period = static_cast<Period>(static_cast<std::uint8_t>(content[id++].toInt()));
//    f.periodAtLeast = content[id++] == "1";
//    f.periodEqual = content[id++] == "1";
//    f.periodAtLast = content[id++] == "1";
//    f.popPeriod = static_cast<Popularity>(static_cast<std::uint8_t>(content[id++].toInt()));

//    // departments
//    f.filterPopDep = content[id++] == "1";
//    f.insideDepartments.clear();
//    for(const auto &dStr : content[id++].split(";")){
//        f.insideDepartments.insert(static_cast<Dep>(static_cast<std::uint8_t>(dStr.toInt())));
//    }
//    f.depAtLeast = content[id++] == "1";
//    f.depEqual = content[id++] == "1";
//    f.depAtLast = content[id++] == "1";
//    f.popDep = static_cast<Popularity>(static_cast<std::uint8_t>(content[id++].toInt()));

//    // sorting
//    f.sortAZ = content[id++] == "1";
//    f.sortZA = content[id++] == "1";
//    f.sortPopS = content[id++] == "1";
//    f.sortPopI = content[id++] == "1";


    return true;
}
