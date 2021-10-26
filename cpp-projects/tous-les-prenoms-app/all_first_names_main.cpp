

#include <iostream>

// std
#include <filesystem>

// Qt
#include <QDebug>
#include <QFile>
#include <QString>

// local
#include "all_first_names_mw.hpp"

using namespace tool;

int main(int argc, char** argv){

    // beer icon flatticon freepik

    QApplication a(argc, argv);
    QLocale::setDefault(QLocale::c());
    tool::AllFirstNamesMW w;
    w.init_data();
    w.init_ui();
    w.show();



    return a.exec();
}
