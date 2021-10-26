
#include "data.hpp"

// Qt
#include <QFile>

using namespace tool;

bool Data::init(){

    // read database file
    if(!iData.read_csv_file(Paths::csvFilePath)){
        return false;
    }

    // process data
    pData.generate(iData);

    namesState.reserve(iData.names.size());
    for(const auto &nameInfo : iData.namesInfo){
        namesState[nameInfo.first] = {true, false, false};
    }
    filteredNames.resize(iData.names.size());

    return true;
}

bool Data::save_saved_names_file(const QString &path) const{

    QFile file(path);
    if(!file.open(QFile::WriteOnly | QIODevice::Text)){
        qWarning() << "Cannot open saved names file at path " % path;
        return false;
    }

    QTextStream fileStream(&file);
    for(const auto &savedName : savedNames){
        fileStream << savedName.v % "\n";
    }

    return true;
}

bool Data::save_removed_names_file(const QString &path) const{

    QFile file(path);
    if(!file.open(QFile::WriteOnly | QIODevice::Text)){
        qWarning() << "Cannot open removed names file at path " % path;
        return false;
    }

    QTextStream fileStream(&file);
    for(const auto &removedName : removedNames){
        fileStream << removedName.v % "\n";
    }

    return true;
}

bool Data::read_saved_names_file(const QString &path){

    QFile file(path);
    if(!file.open(QFile::ReadOnly | QIODevice::Text)){
        qWarning() << "Cannot open saved names file at path " % path;
        return false;
    }

    countSaved = 0;
    savedNames.clear();

    QTextStream fileStream(&file);
    bool read = true;
    auto all = fileStream.readAll();
    QStringView allW = all;
    while(read){

        int endLine = allW.indexOf('\n');
        if(endLine < 0){
            break;
        }
        FirstNameV name{allW.left(endLine)};
        if(name.v.length() == 0){
            break;
        }

        savedNames.push_back(pData.infosPerName[name].name);
        namesState[name].saved = true;
        countSaved++;

        allW = allW.mid(endLine+1);
    }
}

bool Data::read_removed_names_file(const QString &path) {

    QFile file(path);
    if(!file.open(QFile::ReadOnly | QIODevice::Text)){
        qWarning() << "Cannot open removed names file at path " % path;
        return false;
    }

    countRemoved = 0;
    removedNames.clear();

    QTextStream fileStream(&file);
    bool read = true;
    auto all = fileStream.readAll();
    QStringView allW = all;

    while(read){

        int endLine = allW.indexOf('\n');
        if(endLine < 0){
            break;
        }
        FirstNameV name{allW.left(endLine)};
        if(name.v.length() == 0){
            break;
        }

        removedNames.push_back(pData.infosPerName[name].name);
        namesState[name].removed = true;
        countRemoved++;

        allW = allW.mid(endLine+1);
    }
}
