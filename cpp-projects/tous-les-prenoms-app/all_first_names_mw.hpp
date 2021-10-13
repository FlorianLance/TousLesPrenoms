

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
    void init_ui_filters();    
    void init_ui_curves();
    void init_ui_map();
    void init_ui_settings();
    // # update
    void update_ui_curves(){}
    void update_ui_map(){}
    void update_ui_from_settings();
    // connections
    void init_connections();
    void init_connections_colors();

    // data
    void init_data();



    // filters
    void update_filters_settings_from_ui();

    // I/O
    //    bool save_settings_file(const QString &path) const;
    bool save_saved_names_file(const QString &path) const;
    //    bool save_removed_names_file(const QString &path) const;

protected:
    bool eventFilter(QObject *obj, QEvent *event);//in header

public slots:


    void update_filtered_list();
    void update_displayed_info();


    // actions
//    void keep_current_name();
//    void next_name();
//    void previous_name();
//    void remove_current_name();
//    void retire_selection_from_saved();
//    void retire_all_from_saved();

    // lists and display
//    bool is_name_filtered(const NameInfo &info);


//    void update_saved_list();
//    void update_removed_list();




signals:

    void apply_filter_signal(Data *data, FilterSettings filterS);


    // utility



public:

    // ui
    // # widgets
    Ui::AllFirstNameW ui;
    MapW *mapW = nullptr;

    QListView *filteredNamesV = nullptr;
    std::unique_ptr<ui::ListNamesM> filteredNamesM = nullptr;

    QListView *savedNamesV = nullptr;
    std::unique_ptr<ui::ListNamesM> savedNamesM = nullptr;

    QListView *removedNamesV = nullptr;
    std::unique_ptr<ui::ListNamesM> removedNamesM = nullptr;

    CurveW *curveW = nullptr;

    // # forms
    std::unordered_map<GenderRepartition, std::unique_ptr<QColorDialog>> colorsGendersD;
//    QColorDialog femaleCol1D;
//    QColorDialog femaleCol2D;
//    QColorDialog maleCol1D;
//    QColorDialog maleCol2D;
//    QColorDialog bothCol1D;
//    QColorDialog bothCol2D;
//    QColorDialog otherCol1D;
//    QColorDialog otherCol2D;

    QButtonGroup sortGroup;
    // # curves
//    QChartView *chartView = nullptr;
//    QValueAxis *axisX = nullptr;
//    QValueAxis *axisY = nullptr;
//    QLineSeries *seriesWomen = nullptr;
//    QLineSeries *seriesMen = nullptr;
//    QLineSeries *seriesOthers = nullptr;

    Paths paths;
    Settings settings;
    Data data;


    // rand
    std::unique_ptr<std::mt19937> gen = nullptr;
    std::random_device rd;

    QThread listWorkerT;
    std::unique_ptr<ListWorker> listWorker;

    std::vector<std::vector<QLabel*>> tablePeriodsInfoWidgets;
    std::vector<std::vector<QLabel*>> tableDepartmentsInfoWidgets;

private:

    bool listFilteredMousePressed = false;
    bool listSavedMousePressed = false;
    bool listRemovedMousePressed = false;

    static inline const QString firstNameStartStyle = QSL("<html><head/><body><p align=\"center\"><span style=\" font-size:36pt;\"> ");
    static inline const QString lastNameStartStyle = QSL("<html><head/><body><p align=\"center\"><span style=\" font-size:24pt;\"> ");
    static inline const QString endStyle = QSL("</span></p></body></html>");
};


}

