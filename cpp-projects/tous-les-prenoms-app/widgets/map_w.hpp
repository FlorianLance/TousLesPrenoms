

#pragma once


// local
#include "image_viewer_w.hpp"

namespace tool {

class MapW : QWidget{
    Q_OBJECT
public:

    MapW(QHBoxLayout *hlMap);

private:

    ui::ImageViewerW *gradientW = nullptr;
    ui::ImageViewerW *mapW = nullptr;

    QImage image;
    QImage gradient;
};
}
