

#pragma once


// local
#include "utility/convert.hpp"
#include "image_viewer_w.hpp"

namespace tool {

class MapW : QWidget{
    Q_OBJECT
public:

    MapW(QVBoxLayout *hlMap);


    void fill_map(const std::unordered_map<Dep, double> &factors);

private:

    ui::ImageViewerW *gradientW = nullptr;
    ui::ImageViewerW *mapW = nullptr;

    QImage image;
    QImage gradient;

    std::unordered_map<QString, Department>  departmentPerHexaColor;
    std::unordered_map<Department, std::vector<QPoint>>  pixelsPerDepartment;
    QString textHexaColor;
    std::vector<QPoint> textPixels;
};
}
