
#pragma once

// Qt
#include <QPushButton>
#include <QColor>
#include <QIcon>

namespace tool {

class UiUtility{

public:

    static void init(){
        p = std::make_unique<QPixmap>(20,40);
    }

    static void fill_button_icon(QPushButton *button, QColor color){
        p->fill(color);
        button->setIcon(QIcon(*p));
        button->setIconSize(QSize(20,40));
    }

private:

    static inline std::unique_ptr<QPixmap> p = nullptr;
};




}
