

#include "image_viewer_w.hpp"

// Qt
#include <QPainter>
#include <QPaintEvent>
#include <QCoreApplication>


using namespace tool::ui;

ImageViewerW::ImageViewerW(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
}

void ImageViewerW::paintEvent(QPaintEvent *event) {

    QWidget::paintEvent(event);

    if (m_image.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    QSize widgetSize = event->rect().size();
    QSize imageSize = m_image.size();
//    imageSize.scale(event->rect().size(), Qt::KeepAspectRatio);
    imageSize.scale(QSize(widgetSize.width()-2, widgetSize.height()-2), Qt::KeepAspectRatio);

    QImage scaledImage = m_image.scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_imageRect = QRectF(width()*0.5-scaledImage.size().width()*0.5,height()*0.5-scaledImage.size().height()*0.5,
                         scaledImage.width(), scaledImage.height());

    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawImage(static_cast<int>(m_imageRect.x()), static_cast<int>(m_imageRect.y()), scaledImage);
    painter.drawRect(QRectF(m_imageRect.x()-1, m_imageRect.y(), scaledImage.width()+1, scaledImage.height()+1));
}

const QImage* ImageViewerW::image() const {
    return &m_image;
}

void ImageViewerW::set_image (QImage image){
    m_image = std::move(image);
    repaint();
}
