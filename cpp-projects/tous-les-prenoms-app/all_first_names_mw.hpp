

#pragma once

// std
#include <random>
#include <stack>
#include <array>
#include <set>
#include <unordered_set>

// Qt
#include <QMainWindow>
#include <QColorDialog>
#include <QTime>
#include <QThread>
#include <QButtonGroup>
#include <QListView>
#include <QSplashScreen>
#include <QSortFilterProxyModel>

//#include <QtCharts>

// local
// # data
#include "data/filters.hpp"
#include "data/settings.hpp"
// # widgets
#include "widgets/curve_w.hpp"
#include "widgets/image_viewer_w.hpp"
#include "widgets/map_w.hpp"
#include "widgets/list_names_m.hpp"

// # worker
#include "worker/list_worker.hpp"

// generated ui
#include "ui_all_first_names.h"


namespace tool {

template<typename T1, typename T2>
using um = std::unordered_map<T1,T2>;



class SplashScreen : public QSplashScreen{

    Q_OBJECT
public:

    explicit SplashScreen() : QSplashScreen(){
        setPixmap(QPixmap(":/image/splash"));
        setCursor(Qt::BusyCursor);
    }

public slots:

    void set_progress(int value);

protected:
    void drawContents(QPainter *painter);

private:
    int m_progress = 0;
};

class ListView : public QListView{

//    QSize viewportSizeHint() const override {

//        if (model()->rowCount() == 0){
//            return QSize(width(), 0);
//        }

////        int nToShow = _nItemsToShow < model()->rowCount() ? _nItemsToShow : model()->rowCount();
//        qDebug() << "size hint " << model()->rowCount() << QSize(width(), model()->rowCount()*sizeHintForRow(0));
//        return QSize(width(), model()->rowCount()*sizeHintForRow(0));
//    }
//    QSize maximumSizeHint () const override{

//    }
};


//QSize ProposalListView::sizeHint() const
//{
//    if (model()->rowCount() == 0) return QSize(width(), 0);
//    int nToShow = _nItemsToShow < model()->rowCount() ? _nItemsToShow : model()->rowCount();
//    return QSize(width(), nToShow*sizeHintForRow(0));
//}



class AllFirstNamesMW : public QMainWindow{

    template <typename T1,typename T2>
    using m = std::map<T1,T2>;

    Q_OBJECT

public:

    AllFirstNamesMW(QApplication *parent = nullptr);
    ~AllFirstNamesMW();

    // ui
    // # init
    void init_ui();
    void init_ui_lists();
    void init_ui_curves();
    void init_ui_map();
    void init_ui_filters();
    void init_ui_display();
    // # update
    void update_ui_from_settings();
    // connections
    void init_connections();
    void init_connections_lists();
    void init_connections_filters();
    void init_connections_display();

    // data
    void init_data();

    // settings
    void update_filters_from_ui();
    void update_display_from_ui();



protected:

    bool eventFilter(QObject *obj, QEvent *event);//in header

public slots:

    void update_displayed_current_name_infos();
    void update_displayed_name(FirstNameV name);
    void update_displayed_map(FirstNameV name);
    void update_displayed_curve(FirstNameV name);
    void update_displayed_others_infos(FirstNameV name);

    void update_filtered_list();
    void update_saved_list();
    void update_removed_list();

    // actions
    // # common
    void previous_name();
    // # filtered
    void next_filtered_name();
    // # save
    void save_current_name();
    void unsave_current_name();
    void clear_all_saved_names();
    void next_saved_name();
    // # remove
    void remove_current_name();
    void unremove_current_name();
    void next_removed_name();
    void clear_all_removed_names();

signals:

    void apply_filter_signal(Data *data, FilterSettings filterS);


public:

    // ui
    // # widgets
    Ui::AllFirstNameW ui;
    MapW *mapW = nullptr;

    ListView *filteredNamesV = nullptr;
    std::unique_ptr<ui::ListNamesM> filteredNamesM = nullptr;

    ListView *savedNamesV = nullptr;
    std::unique_ptr<ui::ListNamesM> savedNamesM = nullptr;

    ListView *removedNamesV = nullptr;
    std::unique_ptr<ui::ListNamesM> removedNamesM = nullptr;

    CurveW *curveW = nullptr;

    // # forms
    using GR = GenderRepartition;
    um<GR, QPushButton*> foregroundGendersColorsB;
    um<GR, QPushButton*> backgroundGendersColorsB;
    um<GR, std::unique_ptr<QColorDialog>> foregroundGendersColorsD;
    um<GR, std::unique_ptr<QColorDialog>> backgroundGendersColorsD;    
    std::vector<std::vector<QLabel*>> tablePeriodsInfoWidgets;
    std::vector<std::vector<QLabel*>> tableDepartmentsInfoWidgets;

    QButtonGroup sortGroup;

    // rand
    std::unique_ptr<std::mt19937> gen = nullptr;
    std::random_device rd;

    // threads / workers
    QThread listWorkerT;
    std::unique_ptr<ListWorker> listWorker;

    // data
    Paths paths;
    Settings settings;
    Data data;

private:

    bool listFilteredMousePressed = false;
    bool listSavedMousePressed = false;
    bool listRemovedMousePressed = false;

    static inline const QString firstNameStartStyle = QSL("<html><head/><body><p align=\"center\"><span style=\" font-size:36pt;\"> ");
    static inline const QString lastNameStartStyle = QSL("<html><head/><body><p align=\"center\"><span style=\" font-size:24pt;\"> ");
    static inline const QString endStyle = QSL("</span></p></body></html>");
};


}


