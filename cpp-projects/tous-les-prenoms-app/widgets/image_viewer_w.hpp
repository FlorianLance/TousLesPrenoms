
#pragma once

// Qt
#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <QHBoxLayout>

namespace tool::ui {

/**
 * @brief Define a QWidget displaying an image.
 */
class ImageViewerW : public QWidget{

    Q_OBJECT

public:

    /**
     * @brief ImageLabel constructor
     * @param [in] parent
     */
    explicit ImageViewerW(QWidget *parent = nullptr);

    /**
     * @brief Return current image pointer
     */
    const QImage* image() const;

public slots:

    /**
     * @brief Set the current image
     */
    void set_image(QImage image);


protected:

    /**
     * @brief paintEvent for drawing in the widget
     */
    virtual void paintEvent(QPaintEvent *);

    QImage m_image;
    QRectF m_imageRect;
};

}
