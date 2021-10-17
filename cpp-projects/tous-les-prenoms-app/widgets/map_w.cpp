

#include "map_w.hpp"

// Qt
#include <QFile>
#include <QTextStream>
#include <QDebug>

// local
#include "data/paths.hpp"
#include "utility/qt_str.hpp"

using namespace tool;

MapW::MapW(QVBoxLayout *vlMap){

    QWidget *lw = new QWidget();
    auto l = new QHBoxLayout();
    lw->setLayout(l);
    vlMap->addWidget(lw);
    vlMap->setStretch(0,1);
    vlMap->setStretch(1,20);

    l->addWidget(mapW = new ui::ImageViewerW());
    l->addWidget(gradientW = new ui::ImageViewerW());
    l->setStretchFactor(mapW, 10);
    l->setStretchFactor(gradientW, 1);

    QFile departmentsColorsFile(Paths::departmentsColorsFilePath);
    if(!departmentsColorsFile.open(QFile::ReadOnly | QIODevice::Text)){
        qWarning() << "cannot open departments colors file at path " % Paths::departmentsColorsFilePath;
        return;
    }

    QTextStream fileStream(&departmentsColorsFile);
    auto lines = fileStream.readAll().split("\n", Qt::SkipEmptyParts, Qt::CaseInsensitive);
    for(const auto &line : lines){
        auto split = line.split(" ");
        if(auto dep = get_department(DepartmentId{split[1].toInt()}); dep.has_value()){
            pixelsPerDepartment[dep.value()] = {};
            departmentPerHexaColor[QSL("#") % split[2].toLower()] = dep.value();
        }else{
            if(split[1].toInt() == -1){
                textHexaColor = QSL("#") % split[2].toLower();
            }
        }
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
            if(departmentPerHexaColor.count(pixelColorHexa) != 0){ // dep
                auto dep = departmentPerHexaColor[pixelColorHexa];
                pixelsPerDepartment[dep].emplace_back(QPoint{ii,jj});
                image.setPixelColor(ii,jj, Qt::gray);
            }else if(textHexaColor == pixelColorHexa){ // texte
                textPixels.emplace_back(QPoint{ii,jj});
                image.setPixelColor(ii,jj, Qt::black);
            }else{
                image.setPixelColor(ii,jj, Qt::white);
            }
        }
    }

    gradientW->set_image(gradient);
    gradientW->update();

    mapW->set_image(image);
    mapW->update();
}

void MapW::fill_map(const std::unordered_map<Dep, double> &factors){

    for(const auto &factor : factors){
        const QColor col = Convert::factor_to_colormap_value(factor.second);
        for(const auto &p : pixelsPerDepartment[factor.first]){
            image.setPixelColor(p.x(), p.y(), col);
        }
    }
    mapW->set_image(image);
    mapW->update();
}
