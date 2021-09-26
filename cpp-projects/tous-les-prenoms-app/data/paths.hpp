
#pragma once

// Qt
#include <QApplication>

// local
#include "utility/types.hpp"

#include <QDebug>
namespace tool {

struct Paths{

    static void initialize_paths(){

        exeDirPath                  = QApplication::applicationDirPath();
        qDebug() << "exeDirPath";
        dataDirPath                 = exeDirPath  % QSL("/data");
        settingsFilePath            = dataDirPath % QSL("/settings.config");
        csvFilePath                 = dataDirPath % QSL("/dpt2020.csv");

        savedNamesFilePath          = dataDirPath % QSL("/saved.txt");
        removedNamesFilePath        = dataDirPath % QSL("/removed.txt");
        mapFilePath                 = dataDirPath % QSL("/france-departments.png");
        gradientFilePath            = dataDirPath % QSL("/color_gradient.png");
        departmentsColorsFilePath   = dataDirPath % QSL("/colors_departments.txt");
    }

    static inline QString exeDirPath = "";
    static inline QString dataDirPath = "";
    static inline QString settingsFilePath = "";
    static inline QString csvFilePath = "";
    static inline QString savedNamesFilePath = "";
    static inline QString removedNamesFilePath = "";
    static inline QString mapFilePath = "";
    static inline QString gradientFilePath = "";
    static inline QString departmentsColorsFilePath = "";
};

}
