

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
    void init_ui_colors();
    void init_ui_curves();
    void init_ui_map();
    // # update
    void update_ui_curves(){}
    void update_ui_map(){}
    // connections
    void init_connections();
    void init_connections_colors();

    // data
    void init_data();

    // settings
    void update_ui_from_settings();

protected:
    bool eventFilter(QObject *obj, QEvent *event);//in header

public slots:

    // actions
//    void keep_current_name();
//    void next_name();
//    void previous_name();
//    void remove_current_name();
//    void retire_selection_from_saved();
//    void retire_all_from_saved();

    // lists and display
//    bool is_name_filtered(const NameInfo &info);

    void update_filter_settings_from_ui();
    void update_filtered_list(std::shared_ptr<QStringList> names);

//    void update_saved_list();
//    void update_removed_list();
    void update_displayed_info();

    // I/O
//    bool save_settings_file(const QString &path) const;
//    bool save_saved_names_file(const QString &path) const;
//    bool save_removed_names_file(const QString &path) const;

signals:

    void apply_filter_signal(Data *data, FilterSettings filterS);


    // utility



public:

    // ui
    // # widgets
    Ui::AllFirstNameW ui;
    MapW *mapW = nullptr;
    QListView *filteredNamesV = nullptr;
    ui::ListNamesM filteredNamesM;
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

private:

    bool listFilteredMousePressed = false;

    static inline const QString firstNameStartStyle = QSL("<html><head/><body><p align=\"center\"><span style=\" font-size:36pt;\"> ");
    static inline const QString lastNameStartStyle = QSL("<html><head/><body><p align=\"center\"><span style=\" font-size:24pt;\"> ");
    static inline const QString endStyle = QSL("</span></p></body></html>");
};


}

