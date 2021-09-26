

#include "settings.hpp"

// Qt
#include <QFile>
#include <QDebug>

// local
#include "utility/convert.hpp"

using namespace tool;

bool Settings::save_settings_file(const QString &path){


    QFile settingsFile(path);
    if(!settingsFile.open(QFile::WriteOnly | QIODevice::Text)){
        qWarning() << "Cannot create settings file at path " % path;
        return false;
    }

    QTextStream fileStream(&settingsFile);

    // lastname
    fileStream << lastName << "\n";

    //        // sex
    //        QString sex =
    //            QString(ui.cbSexFemale->isChecked() ? "1 " : "0 ") %
    //            (ui.cbSexMale->isChecked() ? "1 " : "0 ") %
    //            (ui.cbSexBoth->isChecked() ? "1 " : "0 ") %
    //            (ui.cbSexOther->isChecked() ? "1 " : "0 ");

    //        fileStream << (ui.cbFiltersSex->isChecked() ? "1 " : "0 ") % sex % (ui.cbFilterMixedFrequency->isChecked() ? "1" : "0") % "\n";

    //        // nb letters
    //        fileStream << (ui.cbFiltersNbLetters->isChecked() ? "1 " : "0 ") % QString::number(ui.sbMinNbLetters->value()) % " " % QString::number(ui.sbMaxNbLetters->value()) % "\n";
    //        // starts by
    //        fileStream << (ui.cbFiltersStartsBy->isChecked() ? "1 " : "0 ") % ui.leStartsBy->text().remove(" ") % "\n";
    //        // ends by
    //        fileStream << (ui.cbFiltersEndsBy->isChecked() ? "1 " : "0 ") % ui.leEndsBy->text().remove(" ") % "\n";
    //        // contains
    //        fileStream << (ui.cbFiltersContains->isChecked() ? "1 " : "0 ") % ui.leContains->text().remove(" ") % "\n";
    //        // do not contains
    //        fileStream << (ui.cbFiltersDoNotContain->isChecked() ? "1 " : "0 ") % ui.leDoNotContain->text().remove(" ") % "\n";
    //        // periods
    //        fileStream <<
    //            (ui.cbFilters1900_2018->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe1900_2020->currentIndex()) % " " % QString::number(ui.cbPop1900_2020->currentIndex()) % " " %
    //                (ui.cbFilters1900_1924->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe1900_1924->currentIndex()) % " " % QString::number(ui.cbPop1900_1924->currentIndex()) % " " %
    //                (ui.cbFilters1925_1949->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe1925_1949->currentIndex()) % " " % QString::number(ui.cbPop1925_1949->currentIndex()) % " " %
    //                (ui.cbFilters1950_1969->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe1950_1969->currentIndex()) % " " % QString::number(ui.cbPop1950_1969->currentIndex()) % " " %
    //                (ui.cbFilters1970_1979->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe1970_1979->currentIndex()) % " " % QString::number(ui.cbPop1970_1979->currentIndex()) % " " %
    //                (ui.cbFilters1980_1989->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe1980_1989->currentIndex()) % " " % QString::number(ui.cbPop1980_1989->currentIndex()) % " " %
    //                (ui.cbFilters1990_1999->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe1990_1999->currentIndex()) % " " % QString::number(ui.cbPop1990_1999->currentIndex()) % " " %
    //                (ui.cbFilters2000_2009->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe2000_2009->currentIndex()) % " " % QString::number(ui.cbPop2000_2009->currentIndex()) % " " %
    //                (ui.cbFilters2010_2018->isChecked() ? "1 " : "0 ") % QString::number(ui.cbOpe2010_2020->currentIndex()) % " " % QString::number(ui.cbPop2010_2020->currentIndex()) % "\n";
    //        // departments
    //        fileStream <<
    //            (ui.cbFiltersDepartment->isChecked() ? "1 " : "0 ") %
    //                ui.leInsideDepartments->text().remove(" ") % "\n";

    //        // colors
    //        fileStream << Convert::to_str(settings.femaleCol1) % "\n";
    //        fileStream << Convert::to_str(settings.femaleCol2) % "\n";
    //        fileStream << Convert::to_str(settings.maleCol1) % "\n";
    //        fileStream << Convert::to_str(settings.maleCol2) % "\n";
    //        fileStream << Convert::to_str(settings.bothCol1) % "\n";
    //        fileStream << Convert::to_str(settings.bothCol2) % "\n";
    //        fileStream << Convert::to_str(settings.otherCol1) % "\n";
    //        fileStream << Convert::to_str(settings.otherCol2) % "\n";

    return true;
}



bool Settings::read_settings_file(const QString &path){

//    QFile settingsFile(path);
//    if(!settingsFile.open(QFile::ReadOnly | QIODevice::Text)){
//        qWarning() << "cannot open settings file at path " % path;
//        return false;
//    }

//    QTextStream fileStream(&settingsFile);
//    QStringList content = fileStream.readAll().split("\n");

//    int id = 0;
//    settings.lastName = content[id++];
//    ui.leLastName->setText(settings.lastName);

//    auto sex = content[id++].split(" ");

//    if(sex.size() >= 6){
//        ui.cbFiltersSex->setChecked(sex[0] == "1");
//        ui.cbSexFemale->setChecked(sex[1] == "1");
//        ui.cbSexMale->setChecked(sex[2] == "1");
//        ui.cbSexBoth->setChecked(sex[3] == "1");
//        ui.cbSexOther->setChecked(sex[4] == "1");
//        ui.cbFilterMixedFrequency->setChecked(sex[5] == "1");
//    }

//    auto nbLetters      = content[id++].split(" ");
//    ui.cbFiltersNbLetters->setChecked(nbLetters[0] == "1");
//    ui.sbMinNbLetters->setValue(nbLetters[1].toInt());
//    ui.sbMaxNbLetters->setValue(nbLetters[2].toInt());

//    auto startsBy       = content[id++].split(" ");
//    if(startsBy.size() >= 1){
//        ui.cbFiltersStartsBy->setChecked(startsBy[0] == "1");
//    }
//    if(startsBy.size() == 2){
//        ui.leStartsBy->setText(startsBy[1]);
//    }

//    auto endsBy         = content[id++].split(" ");
//    if(endsBy.size() >= 1){
//        ui.cbFiltersEndsBy->setChecked(endsBy[0] == "1");
//    }
//    if(endsBy.size() == 2){
//        ui.leEndsBy->setText(endsBy[1]);
//    }

//    auto contains       = content[id++].split(" ");
//    if(contains.size() >= 1){
//        ui.cbFiltersContains->setChecked(contains[0] == "1");
//    }
//    if(contains.size() == 2){
//        ui.leContains->setText(contains[1]);
//    }

//    auto doNotContain   = content[id++].split(" ");
//    if(doNotContain.size() >= 1){
//        ui.cbFiltersDoNotContain->setChecked(doNotContain[0] == "1");
//    }
//    if(doNotContain.size() == 2){
//        ui.leDoNotContain->setText(doNotContain[1]);
//    }

//    auto periodUsage = content[id++].split(" ");
//    if(periodUsage.size() >= 27){

//        int idP = 0;
//        ui.cbFilters1900_2018->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe1900_2020->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop1900_2020->setCurrentIndex(periodUsage[idP++].toInt());

//        ui.cbFilters1900_1924->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe1900_1924->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop1900_1924->setCurrentIndex(periodUsage[idP++].toInt());

//        ui.cbFilters1925_1949->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe1925_1949->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop1925_1949->setCurrentIndex(periodUsage[idP++].toInt());

//        ui.cbFilters1950_1969->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe1950_1969->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop1950_1969->setCurrentIndex(periodUsage[idP++].toInt());

//        ui.cbFilters1970_1979->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe1970_1979->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop1970_1979->setCurrentIndex(periodUsage[idP++].toInt());

//        ui.cbFilters1980_1989->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe1980_1989->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop1980_1989->setCurrentIndex(periodUsage[idP++].toInt());

//        ui.cbFilters1990_1999->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe1990_1999->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop1990_1999->setCurrentIndex(periodUsage[idP++].toInt());

//        ui.cbFilters2000_2009->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe2000_2009->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop2000_2009->setCurrentIndex(periodUsage[idP++].toInt());

//        ui.cbFilters2010_2018->setChecked(periodUsage[idP++] == "1");
//        ui.cbOpe2010_2020->setCurrentIndex(periodUsage[idP++].toInt());
//        ui.cbPop2010_2020->setCurrentIndex(periodUsage[idP++].toInt());
//    }

//    auto departmentUsage = content[id++].split(" ");
//    if(departmentUsage.size() >= 2){
//        int idD = 0;
//        ui.cbFiltersDepartment->setChecked(departmentUsage[idD++] == "1");
//        if(departmentUsage.size() == 4){
//            ui.leInsideDepartments->setText(departmentUsage[idD++]);
//        }
//    }

//    settings.femaleCol1  = Convert::to_color(content[id++]);
//    settings.femaleCol2  = Convert::to_color(content[id++]);
//    settings.maleCol1    = Convert::to_color(content[id++]);
//    settings.maleCol2    = Convert::to_color(content[id++]);
//    settings.bothCol1    = Convert::to_color(content[id++]);
//    settings.bothCol2    = Convert::to_color(content[id++]);
//    settings.otherCol1   = Convert::to_color(content[id++]);
//    settings.otherCol2   = Convert::to_color(content[id++]);

//    femaleCol1D.setCurrentColor(settings.femaleCol1);
//    femaleCol2D.setCurrentColor(settings.femaleCol2);
//    maleCol1D.setCurrentColor( settings.maleCol1);
//    maleCol2D.setCurrentColor( settings.maleCol2);
//    bothCol1D.setCurrentColor( settings.bothCol1);
//    bothCol2D.setCurrentColor( settings.bothCol2);
//    otherCol1D.setCurrentColor(settings.otherCol1);
//    otherCol2D.setCurrentColor(settings.otherCol2);

//    UiUtility::fill_button_icon(ui.pbFemaleColor1, settings.femaleCol1);
//    UiUtility::fill_button_icon(ui.pbFemaleColor2, settings.femaleCol2);
//    UiUtility::fill_button_icon(ui.pbMaleColor1,   settings.maleCol1);
//    UiUtility::fill_button_icon(ui.pbMaleColor2,   settings.maleCol2);
//    UiUtility::fill_button_icon(ui.pbBothColor1,   settings.bothCol1);
//    UiUtility::fill_button_icon(ui.pbBothColor2,   settings.bothCol2);
//    UiUtility::fill_button_icon(ui.pbOtherColor1,  settings.otherCol1);
//    UiUtility::fill_button_icon(ui.pbOtherColor2,  settings.otherCol2);

    return true;
}
