
#include "input_data.hpp"

// Qt
#include <QFile>
#include <QTextStream>
#include <QDebug>

// local
#include "utility/benchmark.hpp"
#include "utility/convert.hpp"

using namespace tool;

bool InputData::read_csv_file(const QString &csvFilePath){

    Bench::start("[open file]");
        QFile csvFile(csvFilePath);
        if(!csvFile.open(QFile::ReadOnly | QIODevice::Text)){
            qWarning()<< "Cannot open " << csvFilePath << "\n";
            Bench::stop();
            return false;
        }
    Bench::stop();
    emit set_progress_signal(5);

    Bench::start("[init QTextStream]");
        QTextStream csvStream(&csvFile);
        csvStream.readLine();
    Bench::stop();    
    emit set_progress_signal(15);

    Bench::start("[readAll]");
        dataBaseFileContent = csvStream.readAll();
        dataBaseFileContentV = dataBaseFileContent;
    Bench::stop();
    emit set_progress_signal(30);

    // reserve data
    linesPerName.reserve(40000);
    namesInfo.reserve(40000);

    size_t ii = 0;
    bool read = true;
    Bench::start("[read lines]");
    while(read){

        int endLine = dataBaseFileContentV.indexOf('\n');
        if(endLine < 0){
            break;
        }
        QStringView line = dataBaseFileContentV.left(endLine);
        if(line.length() == 0){
            break;
        }

        RawFirstNameV rawFirstName{line.mid(2, line.indexOf(';',2)-2)};

        if(namesCorr.count(rawFirstName) == 0){

            // process and store first name
            names.emplace_back(Convert::to_first_name(rawFirstName));
            const auto &processedName = names[names.size()-1];

            FirstNameV processedNameV = {processedName.v};

            // add to correspondance table
            namesCorr[rawFirstName] = processedNameV;

            // add line to name
            linesPerName[processedNameV] = {line};

            // init info per name
            namesInfo[processedNameV] = {processedNameV, ii++, {}};

        }else{
            // add line to line
            linesPerName[namesCorr[rawFirstName]].push_back(line);
        }

        dataBaseFileContentV = dataBaseFileContentV.mid(endLine+1);
    }

    // remove _prenoms_rares from list
    linesPerName.erase(FirstNameV{raresNames});
    namesInfo.erase(FirstNameV{raresNames});

    Bench::stop();
    emit set_progress_signal(45);

    return true;
}
