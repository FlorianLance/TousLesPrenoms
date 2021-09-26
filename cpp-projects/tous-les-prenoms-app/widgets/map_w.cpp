

#include "map_w.hpp"

// Qt
#include <QFile>
#include <QTextStream>
#include <QDebug>

// local
#include "data/paths.hpp"

using namespace tool;

MapW::MapW(QHBoxLayout *hlMap){

    hlMap->addWidget(mapW = new ui::ImageViewerW());
    hlMap->addWidget(gradientW = new ui::ImageViewerW());
    hlMap->setStretchFactor(mapW, 10);
    hlMap->setStretchFactor(gradientW, 1);

    QFile departmentsColorsFile(Paths::departmentsColorsFilePath);
    if(!departmentsColorsFile.open(QFile::ReadOnly | QIODevice::Text)){
        qWarning() << "cannot open departments colors file at path " % Paths::departmentsColorsFilePath;
        return;
    }

    QTextStream fileStream(&departmentsColorsFile);
    auto lines = fileStream.readAll().split("\n", Qt::SkipEmptyParts, Qt::CaseInsensitive);
    for(const auto &line : lines){
        auto split = line.split(" ");
//        departmentsInfo[{split[1].toInt()}] = {split[0], split[2]};
//        departmentsPerHexaColor["#" % split[2].toLower()] = {split[1].toInt()};
    }

    // load map image
    if(!image.load(Paths::mapFilePath)){
        return;
    }

    // load map image
    if(!gradient.load(Paths::gradientFilePath)){
        return;
    }

    for(int ii = 0; ii < image.width(); ++ii){
        for(int jj = 0; jj < image.height(); ++jj){
            const QString pixelColorHexa  = QColor(image.pixel(ii,jj)).name();
//            if(departmentsPerHexaColor.count(pixelColorHexa) != 0){
//                if(departmentsPixels.count(departmentsPerHexaColor[pixelColorHexa]) == 0){
//                    departmentsPixels[departmentsPerHexaColor[pixelColorHexa]] = {};
//                    departmentsPixels[departmentsPerHexaColor[pixelColorHexa]].reserve(40000);
//                }
//                departmentsPixels[departmentsPerHexaColor[pixelColorHexa]].emplace_back(QPoint(ii,jj));

//                image.setPixelColor(ii,jj, Qt::gray);
//            }else{
//                image.setPixelColor(ii,jj, Qt::white);
//            }
        }
    }

    gradientW->set_image(gradient);
    gradientW->update();

    mapW->set_image(image);
    mapW->update();
}
