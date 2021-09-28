
#include "all_first_names_mw.hpp"

#define _MATH_DEFINES_DEFINED

// std
#include <execution>
#include <iostream>

// Qt
#include <QFile>
#include <QTextCodec>
#include <QListWidget>
#include <QStringBuilder>
#include <QStringLiteral>
#include <QRegExpValidator>
#include <QRegExpValidator>
#include <QTime>


// local
#include "utility/convert.hpp"
#include "utility/benchmark.hpp"
#include "utility/qt_ui.hpp"


//#include <QLineSeries>



using namespace tool;

AllFirstNamesMW::AllFirstNamesMW(QApplication *parent){

    Q_UNUSED(parent)
    QThread::currentThread()->setObjectName("main");

    // signal/slots/types
    // # Qt
    qRegisterMetaType<std::shared_ptr<QStringList>>("std::shared_ptr<QStringList>");
    // # custom
    qRegisterMetaType<Data*>("Data*");
    qRegisterMetaType<FilterSettings>("FilterSettings");

    // set locale
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // set paths
    Paths::initialize_paths();
    UiUtility::init();

    // init randomization
    gen = std::make_unique<std::mt19937>(rd());

    // read settings file
    if(!QFile(Paths::settingsFilePath).exists()){
        settings.save_settings_file(Paths::settingsFilePath);
    }else{
        settings.read_settings_file(Paths::settingsFilePath);
    }

    // set ui
    init_ui();

    // set workers
    listWorker = std::make_unique<ListWorker>();
    listWorker->moveToThread(&listWorkerT);

    // set connections
    init_connections();

    listWorkerT.start();
}

void AllFirstNamesMW::init_ui(){

    ui.setupUi(this);

    ui.vlFiltered->addWidget(filteredNamesV = new QListView());
    filteredNamesV->setMouseTracking(true);
    filteredNamesM.nData = &data;
    filteredNamesV->setModel(&filteredNamesM);

    filteredNamesV->viewport()->installEventFilter(this);


    init_ui_filters();   
    init_ui_colors();
    init_ui_curves();
    init_ui_map();

    update_ui_from_settings();
}

void AllFirstNamesMW::init_ui_filters(){

    // validators
    {
        const QRegExp reFirstNameFilter("[A-Za-zÀàÉéÈÙùèÂÊÎÔÛâêîôûÄËÏÜŸäëüÿŒœÇç;-]+$");
        QRegExpValidator *vFirstNameFilter = new QRegExpValidator(reFirstNameFilter, this);
        ui.leStartsBy->setValidator(vFirstNameFilter);
        ui.leEndsBy->setValidator(vFirstNameFilter);
        ui.leContains->setValidator(vFirstNameFilter);
        ui.leDoNotContain->setValidator(vFirstNameFilter);

        const QRegExp reDepartment("[0-9;]+$");
        QRegExpValidator *vDepartment = new QRegExpValidator(reDepartment, this);
        ui.leDep->setValidator(vDepartment);

        const QRegExp reLAstName("[0-9A-Za-zÀàÉéÈÙùèÂÊÎÔÛâêîôûÄËÏÜŸäëüÿŒœÇç-]+$");
        QRegExpValidator *vLastName = new QRegExpValidator(reLAstName, this);
        ui.leLastName->setValidator(vLastName);
    }

    const QStringList opeItems = {
        "au moins", "au plus", "égale"
    };
    ui.cbOpePer->addItems(opeItems);
    ui.cbOpePer->setCurrentIndex(0);

    const QStringList popItems = {
        "Immense", "Très haute", "Haute", "Moyenne", "Basse", "Rare", "Très rare", "Inexistante"
    };
    ui.cbPopPeriod->addItems(popItems);
    ui.cbPopPeriod->setCurrentIndex(0);

    ui.cbPopDep->addItems(popItems);
    ui.cbPopDep->setCurrentIndex(0);

    sortGroup.addButton(ui.rbSortAZ);
    sortGroup.addButton(ui.rbSortZA);
    sortGroup.addButton(ui.rbSortPopI);
    sortGroup.addButton(ui.rbSortPopS);

    const QStringList perItems = {
        "1900-1924", "1925-1949", "1950-1969",
        "1970-1979", "1980-1989", "1990-1999",
        "2000-2009", "2010-2020", "1900-2020",
        "inconnue"
    };
    ui.cbPeriod->addItems(perItems);
    ui.cbPeriod->setCurrentIndex(7);
}

void AllFirstNamesMW::init_ui_colors(){

    // dialogs

//    const auto &d = settings.display;
//    std::vector<std::tuple<GenderRepartition, QColor, QColor, QPushButton*, QPushButton*>> gendersRep = {
//        {GenderRepartition::OnlyFemale,      d.onlyFemaleName,  d.onlyFemaleBackground,     ui.pbTotalFemaleColorName, ui.pbTotalFemaleColorBackground},
//        {GenderRepartition::OnlyMale,        d.onlyMaleName,    d.onlyMaleBackground,       ui.pbTotalMaleColorName, ui.pbTotalMaleColorBackground},
//        {GenderRepartition::OnlyOther,       d.onlyOtherName,   d.onlyOtherBackground,      ui.pbTotalOtherColorName, ui.pbTotalOtherColorBackground},
//        {GenderRepartition::MostlyFemale,    d.mostlyFemaleName,d.mostlyFemaleBackground,   ui.pbFemaleColor1, ui.pbFemaleColor1},
//        {GenderRepartition::MostlyMale,      d.mostlyMaleName,  d.mostlyMaleBackground,     ui.pbFemaleColor1, ui.pbFemaleColor1},
//        {GenderRepartition::MostlyOther,     d.mostlyOtherName, d.mostlyOtherBackground,    ui.pbFemaleColor1, ui.pbFemaleColor1},
//        {GenderRepartition::FemaleMale,      d.femaleMaleName,  d.femaleMaleBackground,     ui.pbFemaleColor1, ui.pbFemaleColor1},
//        {GenderRepartition::FemaleOther,     d.femaleOtherName, d.femaleOtherBackground,    ui.pbFemaleColor1, ui.pbFemaleColor1},
//        {GenderRepartition::MaleOther,       d.maleOtherName,   d.maleOtherBackground,      ui.pbFemaleColor1, ui.pbFemaleColor1},
//        {GenderRepartition::FemaleMaleOther, d.femaleMaleName,  d.femaleMaleBackground,     ui.pbFemaleColor1, ui.pbFemaleColor1},
//    };
//    colorsGendersD[GenderRepartition::MostlyFemale] = std::make_unique<QColorDialog>();

}

void AllFirstNamesMW::init_ui_curves(){
    //    QBarSet *set0 = new QBarSet("Femmes");
    //    QBarSet *set1 = new QBarSet("Hommes");
    //    QBarSet *set2 = new QBarSet("Autres");

    //    QStackedBarSeries *series = new QStackedBarSeries();
    //    series->append(set0);
    //    series->append(set1);
    //    series->append(set2);

    //    seriesWomen = new QLineSeries();
    //    seriesWomen->setColor(Qt::blue);
    //    seriesWomen->setName("Femmes");

    //    seriesMen = new QLineSeries();
    //    seriesMen->setColor(Qt::red);
    //    seriesMen->setName("Hommes");

    //    seriesOthers = new QLineSeries();
    //    seriesOthers->setColor(Qt::green);
    //    seriesOthers->setName("Autres");

    //    QChart *chartNbPerYearPerSex = new QChart();

    ////    chartNbPerYearPerSex->addSeries(series);
    ////    chartNbPerYearPerSex->setAnimationOptions(QChart::SeriesAnimations);

    //    chartNbPerYearPerSex->addSeries(seriesWomen);
    //    chartNbPerYearPerSex->addSeries(seriesMen);
    //    chartNbPerYearPerSex->addSeries(seriesOthers);
    //    chartNbPerYearPerSex->setTitle("");

    //    // init axes
    //    axisX = new QValueAxis();
    //    axisX->setTitleText("Années");
    //    axisX->setLabelFormat("%i");
    //    axisX->setTickCount(-1);
    //    axisX->setRange(1900,2018);
    //    axisX->setTitleVisible(true);

    //    axisY = new QValueAxis();
    //    axisY->setTitleText("log(Nombre)");
    //    axisY->setLabelFormat("%i");
    ////    axisY->setBase(1.5);
    //    axisY->setMinorTickCount(-1);
    //    axisY->setTitleVisible(true);
    ////    axisY->setRange(std::numeric_limits<qreal>::epsilon(),300000);
    //    axisY->setRange(0,11);

    //    // add axes to charts
    //    chartNbPerYearPerSex->addAxis(axisX, Qt::AlignBottom);
    //    chartNbPerYearPerSex->addAxis(axisY, Qt::AlignRight);

    //    // attach axes
    //    seriesWomen->attachAxis(axisX);
    //    seriesWomen->attachAxis(axisY);

    //    seriesMen->attachAxis(axisX);
    //    seriesMen->attachAxis(axisY);

    ////    QStringList categories;
    ////    for(int ii = 1900; ii <= 2018; ++ii){
    ////        categories << QString::number(ii);
    ////    }

    ////    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    ////    axisX->append(categories);
    ////    chartNbPerYearPerSex->addAxis(axisX, Qt::AlignBottom);
    ////    series->attachAxis(axisX);

    ////    QValueAxis *axisY = new QValueAxis();
    ////    chartNbPerYearPerSex->addAxis(axisY, Qt::AlignLeft);
    ////    series->attachAxis(axisY);

    //    // init chart view
    //    chartView = new QChartView(chartNbPerYearPerSex);
    //    chartView->setRenderHint(QPainter::Antialiasing);

    //    ui.hlYearsCurve->addWidget(chartView);

}

void AllFirstNamesMW::init_ui_map(){
    mapW = new MapW(ui.hlMap);
}

void AllFirstNamesMW::init_connections(){

    // this
    connect(this, &AllFirstNamesMW::apply_filter_signal, listWorker.get(), &ListWorker::apply_filter);
    // worker
    connect(listWorker.get(), &ListWorker::list_filtered_signal, this, &AllFirstNamesMW::update_filtered_list);

    // ui
    // # spin boxes
    connect(ui.sbMinNbLetters,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int value){
        if(value > ui.sbMaxNbLetters->value()){
            ui.sbMinNbLetters->setValue(ui.sbMaxNbLetters->value());
        }
        update_filter_settings_from_ui();
    });
    connect(ui.sbMaxNbLetters,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int value){
        if(value < ui.sbMinNbLetters->value()){
            ui.sbMaxNbLetters->setValue(ui.sbMinNbLetters->value());
        }
        update_filter_settings_from_ui();
    });
    connect(ui.sbAppearsYear,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](){update_filter_settings_from_ui();});
    connect(ui.sbPeakYear,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](){update_filter_settings_from_ui();});
    // # line edit
    connect(ui.leStartsBy, &QLineEdit::textChanged, this, [&](){update_filter_settings_from_ui();});
    connect(ui.leEndsBy, &QLineEdit::textChanged, this, [&](){update_filter_settings_from_ui();});
    connect(ui.leContains, &QLineEdit::textChanged, this, [&](){update_filter_settings_from_ui();});
    connect(ui.leDoNotContain, &QLineEdit::textChanged, this, [&](){update_filter_settings_from_ui();});
    connect(ui.leLastName, &QLineEdit::textChanged, this, [&](){update_displayed_info();});
    // # check boxes
    connect(ui.cbFiltersLetters, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbNbLetters, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbContains, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbDoNotContain, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbStartsBy, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbEndsBy, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbFiltersGender, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbOnlyFemale, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbOnlyMale, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbOnlyOther, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbMajorFemale, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbMajorMale, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbMajorOther, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbFemaleMale, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    // connect(ui.cbFemaleOther, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    // connect(ui.cbMaleOther, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbFiltersPopDep, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbFiltersPopPeriod, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbFiltersYears, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbAppearsYear, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbPeakYear, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    // # comboboxes
    connect(ui.cbOpeAppearsYear, &QComboBox::currentTextChanged,  this, [&](){update_filter_settings_from_ui();});
    connect(ui.cbOpePeakYear, &QComboBox::currentTextChanged,  this, [&](){update_filter_settings_from_ui();});
    // # radiobutton
    connect(ui.rbSortAZ,   &QRadioButton::toggled, this, [&](){update_filter_settings_from_ui();});
    connect(ui.rbSortZA,   &QRadioButton::toggled, this, [&](){update_filter_settings_from_ui();});
    connect(ui.rbSortPopI, &QRadioButton::toggled, this, [&](){update_filter_settings_from_ui();});
    connect(ui.rbSortPopS, &QRadioButton::toggled, this, [&](){update_filter_settings_from_ui();});

    // # list view names
    connect(filteredNamesV, &QListView::entered, this, [&](const QModelIndex &index){
        if(!listFilteredMousePressed){
            return;
        }
        FirstNameV n{data.filteredNames[index.row()]};
        if(data.filteredNamesMask.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_info();
    });
    connect(filteredNamesV, &QListView::clicked, this, [&](const QModelIndex &index){

        FirstNameV n{data.filteredNames[index.row()]};
        if(data.filteredNamesMask.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_info();
    });




//    connect(ui.twListNames, &QTabWidget::currentChanged, this, [&](int index){

//        if(index == 0){

//            //            if(ui.lwFilteredNames->currentRow() != -1){
//            //                currentInfo = &namesInfos[namesId[ui.lwFilteredNames->currentItem()->text()]];
//            //            }else{
//            //                if(ui.lwFilteredNames->count() > 0){
//            //                    ui.lwFilteredNames->setCurrentRow(0);
//            //                    currentInfo = &namesInfos[namesId[ui.lwFilteredNames->currentItem()->text()]];
//            //                }
//            //            }

//        }else if(index == 1){

//            //            if(ui.lwSavedNames->currentRow() != -1){
//            //                currentInfo = &namesInfos[namesId[ui.lwSavedNames->currentItem()->text()]];
//            //            }else{
//            //                if(ui.lwSavedNames->count() > 0){
//            //                    ui.lwSavedNames->setCurrentRow(0);
//            //                    currentInfo = &namesInfos[namesId[ui.lwSavedNames->currentItem()->text()]];
//            //                }
//            //            }

//        }else{

//            //            if(ui.lwRemoved->currentRow() != -1){
//            //                currentInfo = &namesInfos[namesId[ui.lwRemoved->currentItem()->text()]];
//            //            }else{
//            //                if(ui.lwRemoved->count() > 0){
//            //                    ui.lwRemoved->setCurrentRow(0);
//            //                    currentInfo = &namesInfos[namesId[ui.lwRemoved->currentItem()->text()]];
//            //                }
//            //            }
//        }

//        update_displayed_info();
//    });


//    connect(ui.pbKeepCurrentName, &QPushButton::clicked, this, &MainWindow::keep_current_name);
//    connect(ui.pbRemoveCurrentName, &QPushButton::clicked, this, &MainWindow::remove_current_name);
//    connect(ui.pbPrevious, &QPushButton::clicked, this, &MainWindow::previous_name);
//    connect(ui.pbNext, &QPushButton::clicked, this, &MainWindow::next_name);

//    connect(ui.pbRetireSelectionFromSaved, &QPushButton::clicked, this,  &MainWindow::retire_selection_from_saved);
//    connect(ui.pbRetireAllFromSaved, &QPushButton::clicked, this,  &MainWindow::retire_all_from_saved);


    init_connections_colors();
}

void AllFirstNamesMW::init_connections_colors(){

//    // buttons
//    connect(ui.pbFemaleColor1, &QPushButton::clicked, &femaleCol1D, &QColorDialog::show);
//    connect(ui.pbFemaleColor2, &QPushButton::clicked, &femaleCol2D, &QColorDialog::show);
//    connect(ui.pbMaleColor1,   &QPushButton::clicked, &maleCol1D,   &QColorDialog::show);
//    connect(ui.pbMaleColor2,   &QPushButton::clicked, &maleCol2D,   &QColorDialog::show);
//    connect(ui.pbBothColor1,   &QPushButton::clicked, &bothCol1D,   &QColorDialog::show);
//    connect(ui.pbBothColor2,   &QPushButton::clicked, &bothCol2D,   &QColorDialog::show);
//    connect(ui.pbOtherColor1,  &QPushButton::clicked, &otherCol1D,  &QColorDialog::show);
//    connect(ui.pbOtherColor2,  &QPushButton::clicked, &otherCol2D,  &QColorDialog::show);

//    // dialogs
//    connect(&femaleCol1D, &QColorDialog::currentColorChanged, this, [&](const QColor &c){
//        UiUtility::fill_button_icon(ui.pbFemaleColor1, settings.display.femaleCol1=c);
////        update_displayed_info();
//    });
//    connect(&femaleCol2D, &QColorDialog::currentColorChanged, this, [&](const QColor &c){
//        UiUtility::fill_button_icon(ui.pbFemaleColor2, settings.display.femaleCol2=c);
////        update_displayed_info();
//    });

//    connect(&maleCol1D, &QColorDialog::currentColorChanged, this, [&](const QColor &c){
//        UiUtility::fill_button_icon(ui.pbMaleColor1, settings.display.maleCol1 = c);
////        update_displayed_info();
//    });

//    connect(&maleCol2D, &QColorDialog::currentColorChanged, this, [&](const QColor &c){
//        UiUtility::fill_button_icon(ui.pbMaleColor2, settings.display.maleCol2 = c);
////        update_displayed_info();
//    });

//    connect(&bothCol1D, &QColorDialog::currentColorChanged, this, [&](const QColor &c){
//        UiUtility::fill_button_icon(ui.pbBothColor1, settings.display.bothCol1 = c);
////        update_displayed_info();
//    });

//    connect(&bothCol2D, &QColorDialog::currentColorChanged, this, [&](const QColor &c){
//        UiUtility::fill_button_icon(ui.pbBothColor2, settings.display.bothCol2 = c);
////        update_displayed_info();
//    });

//    connect(&otherCol1D, &QColorDialog::currentColorChanged, this, [&](const QColor &c){
//        UiUtility::fill_button_icon(ui.pbOtherColor1, settings.display.otherCol1 = c);
////        update_displayed_info();
//    });

//    connect(&otherCol2D, &QColorDialog::currentColorChanged, this, [&](const QColor &c){
//        UiUtility::fill_button_icon(ui.pbOtherColor2, settings.display.otherCol2 = c);
////        update_displayed_info();
//    });
}


void AllFirstNamesMW::init_data(){

    if(!data.init()){
        return;
    }


    Bench::display();
    Bench::reset();


    // update ui from data
//    update_ui_map();
//    update_ui_curves();


    // update lists
    update_filter_settings_from_ui();
//    update_saved_list();
//    update_removed_list();
//    update_displayed_info();

    delete ui.laLoading;
    delete ui.pbLoading;

////    Bench::stop();
////    Bench::start("[UI 2]");


//    // 35009

//    //    for(const auto& info : namesInfo){
//    //        qDebug() << info.first.v << info.second.id << info.second.infos.size();
//    //    }

//    //    QString line = "-";
//    //    while(line.length() > 0){
//    //        line = inC.readLine();
//    //    }


//    //    Bench::start("  [splitRef]");
//    //    lines = all.splitRef("\n", Qt::SkipEmptyParts);
//    //    Bench::stop();
//    Bench::display();
//    //        qDebug() << "lines: " << lines.size();

//    return;

//    ui.pbLoading->setValue(17);
//    wait_process(10);

//    //    Bench::start("  [parse]");

//    //parseId.resize(static_cast<size_t>(lines.size()));
//    //std::iota(std::begin(parseId), std::end(parseId), 0);

//    //        parsedInfos.resize(static_cast<size_t>(lines.size()));





//    //        std::for_each(std::execution::par_unseq, std::begin(parseId), std::end(parseId), [&](int id){

//    //            if(lines[id].count(lineSep) != 4){
//    //                parsedInfos[static_cast<size_t>(id)] = {};
//    //                return; // invalid line
//    //            }
//    //            parsedInfos[static_cast<size_t>(id)] = StrToData::to_line_info(lines[id]);
//    //        });

//    ui.pbLoading->setValue(47);
//    wait_process(10);
//    //    Bench::stop();


//    return;

//    //        qDebug() << "parsedInfos " << parsedInfos.size();

//    //    Bench::start("  [init map test]");

//    // ##################### START TEST

//    //        namesInfos.reserve(35000);
//    //        std::unordered_map<size_t, std::vector<const LineInfo*>> infos;

//    //        for(const auto &info : parsedInfos){
//    //            if(namesId.count(info.firstName.v) == 0){
//    //                size_t id = namesId.size();
//    //                namesId[info.firstName.v] = id;//namesInfos.size();
//    //                infos[id] = {&info};

//    //////                std::cout << id << " ";
//    ////                auto a = NameInfo{id, info};
//    //////                std::cout << a.firstName.v.toStdString() << " | ";
//    ////                namesInfos.push_back(a);
//    //////                namesInfos.emplace_back(NameInfo{id, info});
//    //            }else{
//    //                size_t id = namesId[info.firstName.v];
//    //                infos[id].emplace_back(&info);
//    //////                namesInfos[namesId[info.firstName.v]].update(info);
//    //            }
//    //        }




//    //        qDebug() <<  "end " << namesInfos.size();
//    return;


//    //        std::fill(std::begin(countPerPeriod), std::end(countPerPeriod), 0);
//    //        std::fill(std::begin(countPerDepartment), std::end(countPerDepartment), 0);

//    //        std::vector<int> idN;
//    //        idN.resize(static_cast<size_t>(namesInfos.size()));
//    //        std::iota(std::begin(idN), std::end(idN), 0);
//    //        std::for_each(std::execution::par_unseq, std::begin(idN), std::end(idN), [&](int id){

//    //            auto info = &namesInfos[static_cast<size_t>(id)];

//    //            // count nb per sex
//    //            countPerSex[0] += info->countPerSex[0]; // female
//    //            countPerSex[1] += info->countPerSex[1]; // male
//    //            countPerSex[2] += info->countPerSex[2]; // other

//    //            // count nb per period
//    //            for(size_t ii = 0; ii < static_cast<size_t>(Period::SizeEnum); ++ii){
//    //                countPerPeriod[ii] += info->countPerPeriod[ii];
//    //            }

//    //            // count nb per department
//    //            for(size_t ii = 0; ii < static_cast<size_t>(DepartmentC::SizeEnum); ++ii){
//    //                countPerDepartment[ii] += std::get<1>(info->departmentsInfo[ii]).v;//info->nbPerDepartment_test[ii];
//    //            }
//    //        });


//    //        std::fill(std::begin(maxNameNbPerPeriod), std::end(maxNameNbPerPeriod), 0);
//    //        std::fill(std::begin(maxNameNbPerDepartment), std::end(maxNameNbPerDepartment), 0);

//    //        for(const auto &info : namesInfos){

//    //            // find max nb name per period
//    //            for(size_t ii = 0; ii < static_cast<size_t>(Period::SizeEnum); ++ii){
//    //                if(info.countPerPeriod[ii] > maxNameNbPerPeriod[ii]){
//    //                    maxNameNbPerPeriod[ii] = info.countPerPeriod[ii];
//    //                }
//    //            }

//    //            // find max nb name per department
//    //            for(size_t ii = 0; ii < static_cast<size_t>(DepartmentC::SizeEnum); ++ii){
//    //                if(std::get<1>(info.departmentsInfo[ii]).v > maxNameNbPerDepartment[ii]){
//    //                    maxNameNbPerDepartment[ii] = std::get<1>(info.departmentsInfo[ii]).v;
//    //                }
//    //            }
//    //        }

//    //        maxPeriodsNameNb_test = *std::max_element(std::begin(maxNameNbPerPeriod), std::end(maxNameNbPerPeriod));
//    //        maxDepartmentsNameNb_test = *std::max_element(std::begin(maxNameNbPerDepartment), std::end(maxNameNbPerDepartment));

//    //        // sort popularity per period
//    //        for(size_t ii = 0; ii < static_cast<size_t>(Period::SizeEnum); ++ii){

//    //            auto &npPerPeriod = namesPopularityPerPeriod[ii];

//    //            npPerPeriod.resize(namesInfos.size());
//    //            std::iota(std::begin(npPerPeriod),std::end(npPerPeriod),0);

//    //            std::sort(std::execution::par_unseq, std::begin(npPerPeriod), std::end(npPerPeriod), [&](size_t a, size_t b) -> bool{
//    //                return namesInfos[a].countPerPeriod[ii] > namesInfos[b].countPerPeriod[ii];
//    //            });
//    //        }

//    //        // popularity per period
//    //        std::array<size_t,static_cast<size_t>(Period::SizeEnum)> idP;
//    //        std::iota(std::begin(idP),std::end(idP),0);
//    //        std::for_each(std::execution::par_unseq, std::begin(idP), std::end(idP), [&](size_t ii){

//    //            for(size_t jj = 0; jj < namesPopularityPerPeriod[ii].size(); ++jj){

//    //                const auto nameId = namesPopularityPerPeriod[ii][jj];
//    //                auto &pop = namesInfos[nameId].popularityPerPeriod[ii];

//    //                float percentage = 1.f-1.f*jj/namesPopularityPerPeriod[ii].size();
//    //                if(percentage >= get_percentage(Popularity::Huge).v){
//    //                    pop = Popularity::Huge;
//    //                }else if(percentage >= get_percentage(Popularity::Very_hight).v){
//    //                    pop = Popularity::Very_hight;
//    //                }else if(percentage >= get_percentage(Popularity::Hight).v){
//    //                    pop = Popularity::Hight;
//    //                }else if(percentage >= get_percentage(Popularity::Average).v){
//    //                    pop = Popularity::Average;
//    //                }else if(percentage >= get_percentage(Popularity::Low).v){
//    //                    pop = Popularity::Low;
//    //                }else if(percentage >= get_percentage(Popularity::Rare).v){
//    //                    pop = Popularity::Rare;
//    //                }else if(percentage >= get_percentage(Popularity::Very_rare).v){
//    //                    pop = Popularity::Very_rare;
//    //                }else{
//    //                    pop = Popularity::Inexistant;
//    //                }
//    //            }
//    //        });

//    //        // ranking per department per name
//    //        std::for_each(std::execution::par_unseq, std::begin(namesInfos), std::end(namesInfos), [&](NameInfo &info){

//    //            std::sort(std::execution::seq, std::begin(info.departmentsInfo), std::end(info.departmentsInfo), [&](const auto &r1, const auto &r2) -> bool{
//    //                return
//    //                    std::get<1>(r1).v > std::get<1>(r2).v;
//    //            });

//    //            for(size_t ii = 0; ii < info.departmentsInfo.size(); ++ii){
//    //                std::get<2>(info.departmentsInfo[ii]).v = ii;
//    //            }

//    //            std::sort(std::execution::seq, std::begin(info.departmentsInfo), std::end(info.departmentsInfo), [&](const auto &r1, const auto &r2) -> bool{
//    //                return get_id(std::get<0>(r1)) < get_id(std::get<0>(r2));
//    //            });

//    //        });

//    //        const auto &t = namesInfos[namesId["Marie"]];
//    //        for(size_t ii = 0; ii < t.departmentsInfo.size(); ++ii){
//    //            std::cout << ii << " " << get_department_id(std::get<0>(t.departmentsInfo[ii])) << " " << std::get<1>(t.departmentsInfo[ii]).v << " " << std::get<2>(t.departmentsInfo[ii]).v << "\n";
//    //        }


//    ui.pbLoading->setValue(88);
//    wait_process(10);
//    //    Bench::stop();

//    //    Bench::start("  [read saved/removed]", true);

//    // saved
//    QFile savedNamesFile(Paths::savedNamesFilePath);
//    if(!savedNamesFile.exists()){
//        if(!savedNamesFile.open(QFile::WriteOnly | QIODevice::Text)){
//            qWarning()<< "Cannot write " << Paths::savedNamesFilePath << "\n";
//            return;
//        }
//    }else{
//        if(!savedNamesFile.open(QFile::ReadOnly | QIODevice::Text)){
//            qWarning()<< "Cannot read " << Paths::savedNamesFilePath << "\n";
//            return;
//        }

//        QTextStream inSaved(&savedNamesFile);
//        for(const auto &savedName : inSaved.readAll().splitRef("\n")){

//            //            FirstName name = StrToData::to_first_name(savedName);
//            //            if(namesId.count(name.v) != 0){
//            //                savedNames[{name}] = &namesInfos[namesId[name.v]];
//            //            }
//        }
//    }

//    // removed
//    QFile removedNamesFile(Paths::removedNamesFilePath);
//    if(!removedNamesFile.exists()){
//        if(!removedNamesFile.open(QFile::WriteOnly | QIODevice::Text)){
//            qWarning()<< "Cannot write " << Paths::removedNamesFilePath << "\n";
//            return;
//        }
//    }else{
//        if(!removedNamesFile.open(QFile::ReadOnly | QIODevice::Text)){
//            qWarning()<< "Cannot read " << Paths::removedNamesFilePath << "\n";
//            return;
//        }

//        QTextStream inRemoved(&removedNamesFile);
//        for(const auto &removedName : inRemoved.readAll().splitRef("\n")){

//            //            FirstName name = StrToData::to_first_name(removedName);
//            //            if(namesId.count(name.v) != 0 && savedNames.count({name}) == 0){
//            //                removedNames[{name}] = &namesInfos[namesId[name.v]];
//            //            }
//        }
//    }

//    ui.pbLoading->setValue(100);
//    wait_process(10);
//    //    Bench::stop();

//    qDebug() << "end read names files";




}

void AllFirstNamesMW::update_ui_from_settings(){


    //    QFile settingsFile(path);
    //    if(!settingsFile.open(QFile::ReadOnly | QIODevice::Text)){
    //        qWarning() << "cannot open settings file at path " % path;
    //        return false;
    //    }

    //    QTextStream fileStream(&settingsFile);
    //    QStringList content = fileStream.readAll().split("\n");

    //    int id = 0;
    //    settings.lastName = content[id++];
    //    ui.leLastName->setText(settings.lastName);

    //    auto sex = content[id++].split(" ");

    //    if(sex.size() >= 6){
    //        ui.cbFiltersSex->setChecked(sex[0] == "1");
    //        ui.cbSexFemale->setChecked(sex[1] == "1");
    //        ui.cbSexMale->setChecked(sex[2] == "1");
    //        ui.cbSexBoth->setChecked(sex[3] == "1");
    //        ui.cbSexOther->setChecked(sex[4] == "1");
    //        ui.cbFilterMixedFrequency->setChecked(sex[5] == "1");
    //    }

    //    auto nbLetters      = content[id++].split(" ");
    //    ui.cbFiltersNbLetters->setChecked(nbLetters[0] == "1");
    //    ui.sbMinNbLetters->setValue(nbLetters[1].toInt());
    //    ui.sbMaxNbLetters->setValue(nbLetters[2].toInt());

    //    auto startsBy       = content[id++].split(" ");
    //    if(startsBy.size() >= 1){
    //        ui.cbFiltersStartsBy->setChecked(startsBy[0] == "1");
    //    }
    //    if(startsBy.size() == 2){
    //        ui.leStartsBy->setText(startsBy[1]);
    //    }

    //    auto endsBy         = content[id++].split(" ");
    //    if(endsBy.size() >= 1){
    //        ui.cbFiltersEndsBy->setChecked(endsBy[0] == "1");
    //    }
    //    if(endsBy.size() == 2){
    //        ui.leEndsBy->setText(endsBy[1]);
    //    }

    //    auto contains       = content[id++].split(" ");
    //    if(contains.size() >= 1){
    //        ui.cbFiltersContains->setChecked(contains[0] == "1");
    //    }
    //    if(contains.size() == 2){
    //        ui.leContains->setText(contains[1]);
    //    }

    //    auto doNotContain   = content[id++].split(" ");
    //    if(doNotContain.size() >= 1){
    //        ui.cbFiltersDoNotContain->setChecked(doNotContain[0] == "1");
    //    }
    //    if(doNotContain.size() == 2){
    //        ui.leDoNotContain->setText(doNotContain[1]);
    //    }

    //    auto periodUsage = content[id++].split(" ");
    //    if(periodUsage.size() >= 27){

    //        int idP = 0;
    //        ui.cbFilters1900_2018->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe1900_2020->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop1900_2020->setCurrentIndex(periodUsage[idP++].toInt());

    //        ui.cbFilters1900_1924->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe1900_1924->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop1900_1924->setCurrentIndex(periodUsage[idP++].toInt());

    //        ui.cbFilters1925_1949->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe1925_1949->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop1925_1949->setCurrentIndex(periodUsage[idP++].toInt());

    //        ui.cbFilters1950_1969->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe1950_1969->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop1950_1969->setCurrentIndex(periodUsage[idP++].toInt());

    //        ui.cbFilters1970_1979->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe1970_1979->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop1970_1979->setCurrentIndex(periodUsage[idP++].toInt());

    //        ui.cbFilters1980_1989->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe1980_1989->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop1980_1989->setCurrentIndex(periodUsage[idP++].toInt());

    //        ui.cbFilters1990_1999->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe1990_1999->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop1990_1999->setCurrentIndex(periodUsage[idP++].toInt());

    //        ui.cbFilters2000_2009->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe2000_2009->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop2000_2009->setCurrentIndex(periodUsage[idP++].toInt());

    //        ui.cbFilters2010_2018->setChecked(periodUsage[idP++] == "1");
    //        ui.cbOpe2010_2020->setCurrentIndex(periodUsage[idP++].toInt());
    //        ui.cbPop2010_2020->setCurrentIndex(periodUsage[idP++].toInt());
    //    }

    //    auto departmentUsage = content[id++].split(" ");
    //    if(departmentUsage.size() >= 2){
    //        int idD = 0;
    //        ui.cbFiltersDepartment->setChecked(departmentUsage[idD++] == "1");
    //        if(departmentUsage.size() == 4){
    //            ui.leInsideDepartments->setText(departmentUsage[idD++]);
    //        }
    //    }

    //    settings.femaleCol1  = Convert::to_color(content[id++]);
    //    settings.femaleCol2  = Convert::to_color(content[id++]);
    //    settings.maleCol1    = Convert::to_color(content[id++]);
    //    settings.maleCol2    = Convert::to_color(content[id++]);
    //    settings.bothCol1    = Convert::to_color(content[id++]);
    //    settings.bothCol2    = Convert::to_color(content[id++]);
    //    settings.otherCol1   = Convert::to_color(content[id++]);
    //    settings.otherCol2   = Convert::to_color(content[id++]);

    //    femaleCol1D.setCurrentColor(settings.femaleCol1);
    //    femaleCol2D.setCurrentColor(settings.femaleCol2);
    //    maleCol1D.setCurrentColor( settings.maleCol1);
    //    maleCol2D.setCurrentColor( settings.maleCol2);
    //    bothCol1D.setCurrentColor( settings.bothCol1);
    //    bothCol2D.setCurrentColor( settings.bothCol2);
    //    otherCol1D.setCurrentColor(settings.otherCol1);
    //    otherCol2D.setCurrentColor(settings.otherCol2);

    //    UiUtility::fill_button_icon(ui.pbFemaleColor1, settings.femaleCol1);
    //    UiUtility::fill_button_icon(ui.pbFemaleColor2, settings.femaleCol2);
    //    UiUtility::fill_button_icon(ui.pbMaleColor1,   settings.maleCol1);
    //    UiUtility::fill_button_icon(ui.pbMaleColor2,   settings.maleCol2);
    //    UiUtility::fill_button_icon(ui.pbBothColor1,   settings.bothCol1);
    //    UiUtility::fill_button_icon(ui.pbBothColor2,   settings.bothCol2);
    //    UiUtility::fill_button_icon(ui.pbOtherColor1,  settings.otherCol1);
    //    UiUtility::fill_button_icon(ui.pbOtherColor2,  settings.otherCol2);

}

bool AllFirstNamesMW::eventFilter(QObject *obj, QEvent *event){


    if(obj == filteredNamesV->viewport()){
        if(event->type() == QEvent::MouseButtonPress){
            qDebug() << "pressed";
            listFilteredMousePressed = true;
        }else if(event->type() == QEvent::MouseButtonRelease){
            qDebug() << "released";
            listFilteredMousePressed = false;
        }
    }
//    if (obj == ui->listView->viewport() && event->type() == QEvent::MouseButtonDblClick)
//    {
//        QMouseEvent *ev = static_cast<QMouseEvent *>(event);
//        if (ev->buttons() & Qt::RightButton)
//        {
//            qDebug()<< "double clicked" << ev->pos();
//            qDebug()<<  ui->listView->indexAt(ev->pos()).data();
//        }
//    }
    return QObject::eventFilter(obj, event);
}

AllFirstNamesMW::~AllFirstNamesMW(){

    listWorkerT.quit();
    listWorkerT.wait();

    // save session
    settings.save_settings_file(Paths::settingsFilePath);
//    save_saved_names_file(Paths::savedNamesFilePath);
//    save_removed_names_file(Paths::removedNamesFilePath);
}


void AllFirstNamesMW::update_filtered_list(std::shared_ptr<QStringList> names){

    // data.filteredNames

    Bench::start("[update_filtered_list]1");
    ui.twListNames->setTabText(0, QString("Filtrés (") % QString::number(names->size()) % QSL(")"));

    filteredNamesM.update();
    filteredNamesV->viewport()->update();

    Bench::stop();

    update_displayed_info();


    Bench::display();
    Bench::reset();
}


void AllFirstNamesMW::update_filter_settings_from_ui(){

    ui.wSubGender->setEnabled(ui.cbFiltersGender->isChecked());
    ui.wSubLetters->setEnabled(ui.cbFiltersLetters->isChecked());
    ui.wSubDep->setEnabled(ui.cbFiltersPopDep->isChecked());
    ui.wSubYear->setEnabled(ui.cbFiltersYears->isChecked());
    ui.sbAppearsYear->setEnabled(ui.cbOpeAppearsYear->currentIndex() != 3);
    ui.sbPeakYear->setEnabled(ui.cbOpePeakYear->currentIndex() != 3);


    // gender
    settings.filters.filterGender            = ui.cbFiltersGender->isChecked();
    settings.filters.onlyFemale              = ui.cbOnlyFemale->isChecked();
    settings.filters.onlyMale                = ui.cbOnlyMale->isChecked();
    settings.filters.onlyOther               = ui.cbOnlyOther->isChecked();
    settings.filters.majorFemale             = ui.cbMajorFemale->isChecked();
    settings.filters.majorMale               = ui.cbMajorMale->isChecked();
    settings.filters.majorOther              = ui.cbMajorOther->isChecked();
    settings.filters.femaleMale              = ui.cbFemaleMale->isChecked();
    settings.filters.femaleOther             = false;//ui.cbFemaleOther->isChecked();
    settings.filters.maleOther               = false;//ui.cbMaleOther->isChecked();
    // letters
    settings.filters.filterLetters           = ui.cbFiltersLetters->isChecked();
    settings.filters.startsBy                = ui.cbStartsBy->isChecked();
    settings.filters.startsByTextes          = ui.leStartsBy->text().split(Filters::sepWords);
    settings.filters.endsBy                  = ui.cbEndsBy->isChecked();
    settings.filters.endsByTextes            = ui.leEndsBy->text().split(Filters::sepWords);
    settings.filters.contains                = ui.cbContains->isChecked();
    settings.filters.containsTextes          = ui.leContains->text().split(Filters::sepWords);
    settings.filters.doNoContains            = ui.cbDoNotContain->isChecked();
    settings.filters.doNotContainsTextes     = ui.leDoNotContain->text().split(Filters::sepWords);
    settings.filters.nbLetters        = ui.cbNbLetters->isChecked();;
    settings.filters.minLettersNb            = ui.sbMinNbLetters->value();
    settings.filters.maxLettersNb            = ui.sbMaxNbLetters->value();
    // year
    settings.filters.filterYear              = ui.cbFiltersYears->isChecked();
    settings.filters.appearsBefore           = ui.cbOpeAppearsYear->currentIndex() == 0 && ui.cbAppearsYear->isChecked();
    settings.filters.appearsDuring           = ui.cbOpeAppearsYear->currentIndex() == 1 && ui.cbAppearsYear->isChecked();
    settings.filters.appearsAfter            = ui.cbOpeAppearsYear->currentIndex() == 2 && ui.cbAppearsYear->isChecked();
    settings.filters.appearsUnknow           = ui.cbOpeAppearsYear->currentIndex() == 3 && ui.cbAppearsYear->isChecked();
    settings.filters.appearsYear             = ui.sbAppearsYear->value();
    settings.filters.peakBefore              = ui.cbOpePeakYear->currentIndex() == 0 && ui.cbPeakYear->isChecked();
    settings.filters.peakDuring              = ui.cbOpePeakYear->currentIndex() == 1 && ui.cbPeakYear->isChecked();
    settings.filters.peakAfter               = ui.cbOpePeakYear->currentIndex() == 2 && ui.cbPeakYear->isChecked();
    settings.filters.peakUnknow              = ui.cbOpePeakYear->currentIndex() == 3 && ui.cbPeakYear->isChecked();
    settings.filters.peakYear                = ui.sbPeakYear->value();
    // sortings
    settings.filters.sortAZ                  = ui.rbSortAZ->isChecked();
    settings.filters.sortZA                  = ui.rbSortZA->isChecked();
    settings.filters.sortPopS                = ui.rbSortPopS->isChecked();
    settings.filters.sortPopI                = ui.rbSortPopI->isChecked();

    emit apply_filter_signal(&data, settings.filters);


    // settings.filters.insideDepartmentsTextes = ui.leInsideDepartments->text().split(Filters::sepWords);




//    filteredNamesInfos_test.clear();
//    filteredNamesInfos_test.reserve(namesInfos.size());

//    filteredNamesStr.clear();
//    filteredNamesStr.reserve(static_cast<int>(namesInfos.size()));

//    for(auto &info : namesInfos){

//        // check if name filtered
//        if(is_name_filtered(info)){
//            continue;
//        }

//        QString name = info.firstName.v;
//        filteredNamesStr << name;

//        if(savedNames.count({name}) == 0 && removedNames.count({name}) == 0){
//            filteredNamesInfos_test.emplace_back(&info);
//        }
//    }

//    // sort settings
//    std::sort(std::execution::par_unseq, std::begin(filteredNamesStr), std::end(filteredNamesStr), [&](const QString &n1, const QString &n2) -> bool{
//        return
//            namesInfos[namesId[n1]].countPerPeriod[static_cast<size_t>(Period::p1900_2020)] >
//            namesInfos[namesId[n2]].countPerPeriod[static_cast<size_t>(Period::p1900_2020)];
//    });
//    // A -> Z
//    std::sort(std::execution::par_unseq, std::begin(filteredNamesStr_test), std::end(filteredNamesStr_test), [&](const QString &n1, const QString &n2) -> bool{
//        return n1 > n2;
//    });
//    // Z -> A
//    std::sort(std::execution::par_unseq, std::begin(filteredNamesStr_test), std::end(filteredNamesStr_test), [&](const QString &n1, const QString &n2) -> bool{
//        return n1 < n2;
//    });



//    if(filteredNamesStr.size() > 0){
//        currentInfo = &namesInfos[namesId[filteredNamesStr[0]]];
//    }else{
//        currentInfo = nullptr;
//    }
}

//void MainWindow::update_saved_list(){

//    savedNamesStr.clear();
//    savedNamesStr.reserve(static_cast<int>(savedNames.size()));

//    for(const auto &info : savedNames){
//        savedNamesStr << info.first.v;
//    }

//    ui.twListNames->setTabText(1, "Retenus (" % QString::number(savedNamesStr.size()) % QStringLiteral(")"));
//    ui.lwSavedNames->clear();

//    ui.lwSavedNames->addItems(savedNamesStr);
//}

//void MainWindow::update_removed_list(){

//    removedNamesStr.clear();
//    removedNamesStr.reserve(static_cast<int>(removedNames.size()));

//    for(const auto &info : removedNames){
//        removedNamesStr << info.first.v;
//    }

//    ui.twListNames->setTabText(2, "Retirés (" % QString::number(removedNamesStr.size()) % QStringLiteral(")"));

//    ui.lwRemoved->clear();
//    ui.lwRemoved->addItems(removedNamesStr);
//}

void AllFirstNamesMW::update_displayed_info(){


    if(data.currentName.v.length() == 0){
        for(const auto &m : data.filteredNamesMask){
            if(!m.second){
                data.currentName = m.first;
                break;
            }
        }
    }

    if(data.currentName.v.length() == 0){
        qDebug() << "no name available";
        return;
    }

    QColor c1 = Qt::black;//settings.display.bothCol1;
    QColor c2 = Qt::black;//settings.display.bothCol2;

    ui.laFirstName->setStyleSheet(QSL("QLabel[objectName^=\"laFirstName\"] { color:rgb(")
        % QString::number(c2.red()) % "," % QString::number(c2.green()) % "," % QString::number(c2.blue()) % QSL("); }"));
    ui.laLastName->setStyleSheet(QSL("QLabel[objectName^=\"laLastName\"] { color:rgb(")
        % QString::number(c2.red()) % "," % QString::number(c2.green()) % "," % QString::number(c2.blue()) % QSL("); }"));
    ui.wCurrentName->setStyleSheet(QSL("QWidget[objectName^=\"wCurrentName\"] { background-color:rgb(")
        % QString::number(c1.red()) % "," % QString::number(c1.green()) % "," % QString::number(c1.blue()) % QSL("); }"));

    ui.laFirstName->setText(firstNameStartStyle % data.currentName.v % endStyle);
    ui.laLastName->setText(lastNameStartStyle % ui.leLastName->text().toUpper() % endStyle);

    const auto &nameInfo = data.pData.infosPerName[data.currentName];
    if(nameInfo.intervalApparition.start.v != -1){
        ui.laStartYear->setText(QString::number(nameInfo.intervalApparition.start.v));
    }else{
        ui.laStartYear->setText("Inconnue");
    }
    if(nameInfo.intervalApparition.end.v != -1){
        ui.laEndYear->setText(QString::number(nameInfo.intervalApparition.end.v));
    }else{
        ui.laEndYear->setText("Inconnue");
    }
    if(nameInfo.popularityPeak.v != -1){
        ui.laPeakYear->setText(QString::number(nameInfo.popularityPeak.v));
    }else{
        ui.laPeakYear->setText("Inconnue");
    }

    ui.laCount->setText(QString::number(nameInfo.total.v));
    ui.laNbFem->setText(QString::number(nameInfo.countPerGender[0].v));
    ui.laNbMale->setText(QString::number(nameInfo.countPerGender[1].v));

//    ui.pbPrevious->setEnabled(previousInfo.size() != 0);

//    if(currentInfo != nullptr){


//        // #############################################################################
////        if(currentInfo->nbPerYearPerSex[Sex::Female].count({0}) != 0){
////            ui.laWomanUnknowYear->setText(QString::number(currentInfo->nbPerYearPerSex[Sex::Female][{0}]));
////        }else{
////            ui.laWomanUnknowYear->setText("0");
////        }

////        if(currentInfo->nbPerYearPerSex[Sex::Male].count({0}) != 0){
////            ui.laManUnknowYear->setText(QString::number(currentInfo->nbPerYearPerSex[Sex::Male][{0}]));
////        }else{
////            ui.laManUnknowYear->setText("0");
////        }

////        if(currentInfo->nbPerYearPerSex[Sex::Other].count({0}) != 0){
////            ui.laOtherUnknowYear->setText(QString::number(currentInfo->nbPerYearPerSex[Sex::Other][{0}]));
////        }else{
////            ui.laOtherUnknowYear->setText("0");
////        }



////        ui.lwCurrentNameTotalPerYear->clear();

////        std::map<Year, std::tuple<int,int,int>> nbNamesPerYear;
////        for(auto &nb : currentInfo->nbPerYearPerSex[Sex::Male]){
////            nbNamesPerYear[nb.first] = {0,nb.second, 0};
////        }
////        for(auto &nb : currentInfo->nbPerYearPerSex[Sex::Female]){
////            if(nbNamesPerYear.count(nb.first) == 0){
////                nbNamesPerYear[nb.first] = {nb.second,0, 0};
////            }else{
////                nbNamesPerYear[nb.first] = {nb.second, std::get<1>(nbNamesPerYear[nb.first]), 0};
////            }
////        }
////        for(auto &nb : currentInfo->nbPerYearPerSex[Sex::Other]){
////            if(nbNamesPerYear.count(nb.first) == 0){
////                nbNamesPerYear[nb.first] = {0, 0, nb.second};
////            }else{
////                nbNamesPerYear[nb.first] = {std::get<0>(nbNamesPerYear[nb.first]), std::get<1>(nbNamesPerYear[nb.first]), nb.second};
////            }
////        }

////        for(const auto &nbPerYear : nbNamesPerYear){

////            QString year;
////            if(nbPerYear.first.v == 0){
////                // year = "année inconnue";
////                continue;
////            }else{
////                year = QString::number(nbPerYear.first.v);
////            }

////            ui.lwCurrentNameTotalPerYear->addItem(
////                year % " -> F: " % QString::number(std::get<0>(nbPerYear.second)) %
////                " M: " % QString::number(std::get<1>(nbPerYear.second)) %
////                " O: 0"
////            );
////        }

//        if(savedNames.count(currentInfo->firstName) != 0){
//            ui.pbKeepCurrentName->setEnabled(false);
//        }else{
//            ui.pbKeepCurrentName->setEnabled(true);
//        }

//        if(removedNames.count(currentInfo->firstName) != 0){
//            ui.pbRemoveCurrentName->setEnabled(false);
//        }else{
//            ui.pbRemoveCurrentName->setEnabled(true);
//        }

//        ui.pbNext->setEnabled(true);


////        QVector<QPointF> womenPoints,menPoints,othersPoints;
////        womenPoints.resize(2019-1900);
////        menPoints.resize(2019-1900);
////        othersPoints.resize(2019-1900);

////        int id = 0;
////        for(int ii = 1900; ii < 2019; ++ii){
////            if(nbNamesPerYear.count(Year{ii}) == 0){
////                womenPoints[id]     = QPointF(ii,std::numeric_limits<qreal>::epsilon());
////                menPoints[id]       = QPointF(ii,std::numeric_limits<qreal>::epsilon());
////                othersPoints[id]    = QPointF(ii,std::numeric_limits<qreal>::epsilon());
////            }else{

////                womenPoints[id]     = QPointF(ii,std::log(std::numeric_limits<qreal>::epsilon()+std::get<0>(nbNamesPerYear[Year{ii}])));
////                menPoints[id]       = QPointF(ii,std::log(std::numeric_limits<qreal>::epsilon()+std::get<1>(nbNamesPerYear[Year{ii}])));
////                othersPoints[id]    = QPointF(ii,std::log(std::numeric_limits<qreal>::epsilon()+std::get<2>(nbNamesPerYear[Year{ii}])));

////            }
////            ++id;
////        }

////        seriesWomen->replace(womenPoints);
////        seriesMen->replace(menPoints);
////        seriesOthers->replace(othersPoints);

//        // update image
//        std::map<DepartmentId,size_t> countPerDepartment;
//        int maxCurrentName = 0;
//        for(const auto &d : departmentsPixels){

//            //countPerDepartment[d.first] = std::get<1>(currentInfo->departmentsInfo[get_id(static_cast<DepartmentC>(d.first.v))]).v;//currentInfo->nbPerDepartment_test[get_department_id(static_cast<DepartmentC>(d.first.v))];

////            countPerDepartment[d.first] = currentInfo->nbPerDepartment[d.first];
//            if(maxCurrentName < countPerDepartment[d.first]){
//                maxCurrentName = countPerDepartment[d.first];
//            }
//        }

////        double maxLog = std::log(maxDepartmentsNameNb_test);
////        for(const auto &d : departmentsPixels){

////            double factor = 0;
////            if(countPerDepartment[d.first] > 0){
////                factor = std::log(countPerDepartment[d.first]) / maxLog;
////            }

////            QColor colPix = factor_to_colormap_value(factor);
////            for(const auto &pixels : d.second){
////                image.setPixelColor(pixels, colPix);
////            }
////        }

//    }else{

//        ui.laWomanTotal->setText("0");
//        ui.laManTotal->setText("0");
//        ui.laOtherTotal->setText("0");

//        ui.laWomanUnknowYear->setText("0");
//        ui.laManUnknowYear->setText("0");
//        ui.laOtherUnknowYear->setText("0");

//        ui.laFirstName->setText(QStringLiteral("<html><head/><body><p align=\"center\"><span style=\" font-size:36pt;\">...</span></p></body></html>"));
//        ui.laLastName->setText(QStringLiteral("<html><head/><body><p align=\"center\"><span style=\" font-size:24pt;\">...</span></p></body></html>"));
//        ui.lwCurrentNameTotalPerYear->clear();

//        ui.pbNext->setEnabled(false);
//        ui.pbKeepCurrentName->setEnabled(false);
//        ui.pbRemoveCurrentName->setEnabled(false);
//    }

////    chartView->update();


//    // update map image
//    mapW->set_image(image);
//    update();
}

//void MainWindow::keep_current_name(){

//    int currentRemovedId = ui.lwRemoved->currentRow();

//    if(currentInfo != nullptr){
//        previousInfo.emplace(currentInfo);
//    }

////    // if not already saved
////    if(savedNames.count(currentInfo->firstName) == 0){

////        // add current to saved
////        savedNames[currentInfo->firstName] = currentInfo;

////        // remove from removed
////        if(removedNames.count(currentInfo->firstName) != 0){
////            removedNames.erase(currentInfo->firstName);
////            update_removed_list();
////        }

////        // remove from filtered
////        if(auto pos = std::find(filteredNamesInfos_test.begin(),filteredNamesInfos_test.end(), currentInfo); pos != std::end(filteredNamesInfos_test)){
////            filteredNamesInfos_test.erase(pos);
////        }

////        // update lists
////        update_saved_list();
////    }

//    int index  = ui.twListNames->currentIndex();
//    if(index == 0){ // filtered

////        // choose new random name among filtered ones
////        if(filteredNamesInfos_test.size() > 0){

////            std::uniform_int_distribution<> distrib(0, static_cast<int>(filteredNamesInfos_test.size())-1);
////            auto id = distrib(*gen);

////            for(int ii = 0; ii < ui.lwFilteredNames->count(); ++ii){
////                if(ui.lwFilteredNames->item(ii)->text() == filteredNamesInfos_test[static_cast<size_t>(id)]->firstName.v){
////                    ui.lwFilteredNames->setCurrentRow(ii);
////                }
////            }
////        }else{
////            currentInfo = nullptr;
////        }

//    }else if(index == 2){ // removed

//        if(removedNames.size() > 0){
//            if(currentRemovedId < static_cast<int>(removedNames.size()) && currentRemovedId >= 0){
//                ui.lwRemoved->setCurrentRow(currentRemovedId);
//            }else if(currentRemovedId >= static_cast<int>(removedNames.size()) && currentRemovedId >= 0){
//                ui.lwRemoved->setCurrentRow(currentRemovedId-1);
//            }
//        }else{
//            currentInfo = nullptr;
//        }
//    }



//    // update current displayed
//    update_displayed_info();
//}

//void MainWindow::next_name(){

//    if(currentInfo != nullptr){
//        previousInfo.emplace(currentInfo);
//    }

////    // choose new random name among filtered ones
////    if(filteredNamesInfos_test.size() > 0){

////        std::uniform_int_distribution<> distrib(0, static_cast<int>(filteredNamesInfos_test.size())-1);
////        auto id = distrib(*gen);

////        for(int ii = 0; ii < ui.lwFilteredNames->count(); ++ii){
////            if(ui.lwFilteredNames->item(ii)->text() == filteredNamesInfos_test[static_cast<size_t>(id)]->firstName.v){
////                ui.lwFilteredNames->setCurrentRow(ii);
////            }
////        }
////    }else{
////        currentInfo = nullptr;
////    }

//    ui.twListNames->setCurrentIndex(0);

//    // update current displayed
//    update_displayed_info();
//}

//void MainWindow::previous_name(){

//    if(previousInfo.size() > 0){
//        currentInfo = previousInfo.top();
//        previousInfo.pop();
//    }

//    ui.twListNames->setCurrentIndex(0);

//    // update current displayed
//    update_displayed_info();
//}

//void MainWindow::remove_current_name(){

//    int currentSavedId = ui.lwSavedNames->currentRow();

//    if(currentInfo != nullptr){
//        previousInfo.emplace(currentInfo);
//    }

//    // if not already removed
//    if(removedNames.count(currentInfo->firstName) == 0){

//        // add current to remove
//        removedNames[currentInfo->firstName] = currentInfo;


//        // remove from saved
//        if(savedNames.count(currentInfo->firstName) != 0){
//            savedNames.erase(currentInfo->firstName);
//            update_saved_list();
//        }

////        // remove from filtered
////        if(auto pos = std::find(filteredNamesInfos_test.begin(),filteredNamesInfos_test.end(), currentInfo); pos != std::end(filteredNamesInfos_test)){
////            filteredNamesInfos_test.erase(pos);
////        }

//        // update lists
//        update_removed_list();
//    }


//    int index  = ui.twListNames->currentIndex();
//    if(index == 0){ // filtered

////        // choose new random name among filtered ones
////        if(filteredNamesInfos_test.size() > 0){

////            std::uniform_int_distribution<> distrib(0, static_cast<int>(filteredNamesInfos_test.size())-1);
////            auto id = distrib(*gen);

////            for(int ii = 0; ii < ui.lwFilteredNames->count(); ++ii){
////                if(ui.lwFilteredNames->item(ii)->text() == filteredNamesInfos_test[static_cast<size_t>(id)]->firstName.v){
////                    ui.lwFilteredNames->setCurrentRow(ii);
////                }
////            }
////        }else{
////            currentInfo = nullptr;
////        }

//    }else if(index == 1){ // saved

//        if(savedNames.size() > 0){
//            if(currentSavedId < static_cast<int>(savedNames.size()) && currentSavedId >= 0){
//                ui.lwSavedNames->setCurrentRow(currentSavedId);
//            }else if(currentSavedId >= static_cast<int>(savedNames.size()) && currentSavedId >= 0){
//                ui.lwSavedNames->setCurrentRow(currentSavedId-1);
//            }
//        }else{
//            currentInfo = nullptr;
//        }
//    }

//    // update current displayed
//    update_displayed_info();
//}

//void MainWindow::retire_selection_from_saved(){

//    if(currentInfo == nullptr){
//        return;
//    }
//    if(savedNames.count(currentInfo->firstName) == 0){
//        return;
//    }

//    savedNames.erase(currentInfo->firstName);
//    removedNames[currentInfo->firstName] = currentInfo;

//    int currentPosition = ui.lwSavedNames->currentRow();
//    update_saved_list();
//    if(currentPosition < ui.lwSavedNames->count()){
//        ui.lwSavedNames->setCurrentRow(currentPosition);
//    }else if(currentPosition > 0){
//        ui.lwSavedNames->setCurrentRow(currentPosition-1);
//    }

//    update_removed_list();
//}

//void MainWindow::retire_all_from_saved(){

//    for(auto &savedName : savedNames){
//        removedNames[savedName.first] = savedName.second;
//    }
//    savedNames.clear();

//    update_saved_list();
//    update_removed_list();

//}


//bool apply_popularity_operator(Popularity pop, Popularity uiPop, int uiOp){
//    if(uiOp == 0){ // at least
//        if(pop < uiPop){
//            return true;
//        }
//    }else if(uiOp == 1){
//        if(pop >= uiPop){
//            return true;
//        }
//    }else{
//        if(pop != uiPop){
//            return true;
//        }
//    }
//    return false;
//}


//bool MainWindow::is_name_filtered(const NameInfo &info){


//    if(ui.cbFiltersContains->isChecked()){

//        if(containsTextes.size() > 0){

//            bool contains = true;
//            for(const auto &text : containsTextes){
//                if(!info.firstName.v.contains(text, Qt::CaseSensitivity::CaseInsensitive)){
//                    contains = false;
//                    break;
//                }
//            }

//            if(!contains){
//                return true;
//            }
//        }
//    }

//    if(ui.cbFiltersDoNotContain->isChecked()){

//        if(doNotContainsTextes.size() > 0){

//            bool contains = false;
//            for(const auto &text : doNotContainsTextes){
//                if(info.firstName.v.contains(text, Qt::CaseSensitivity::CaseInsensitive)){
//                    contains = true;
//                    break;
//                }
//            }

//            if(contains){
//                return true;
//            }
//        }
//    }

//    if(ui.cbFiltersStartsBy->isChecked()){

//        bool starts = false;
//        for(const auto &text : startsByTextes){
//            if(info.firstName.v.startsWith(text, Qt::CaseSensitivity::CaseInsensitive)){
//                starts = true;
//                break;
//            }
//        }

//        if(!starts){
//            return true;
//        }
//    }

//    if(ui.cbFiltersEndsBy->isChecked()){

//        bool ends = false;
//        for(const auto &text : endsByTextes){
//            if(info.firstName.v.endsWith(text, Qt::CaseSensitivity::CaseInsensitive)){
//                ends = true;
//                break;
//            }
//        }

//        if(!ends){
//            return true;
//        }
//    }

//    if(ui.cbFiltersNbLetters->isChecked()){
//        if(info.firstName.v.length() > ui.sbMaxNbLetters->value()){
//            return true;
//        }
//        if(info.firstName.v.length() < ui.sbMinNbLetters->value()){
//            return true;
//        }
//    }


//    if(ui.cbFilters1900_2018->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p1900_2020)],
//            static_cast<Popularity>(ui.cbPop1900_2020->currentIndex()),
//            ui.cbOpe1900_2020->currentIndex())){
//            return true;
//        }
//    }

//    if(ui.cbFilters1900_1924->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p1900_1924)],
//            static_cast<Popularity>(ui.cbPop1900_1924->currentIndex()),
//            ui.cbOpe1900_1924->currentIndex())){
//            return true;
//        }
//    }

//    if(ui.cbFilters1925_1949->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p1925_1949)],
//            static_cast<Popularity>(ui.cbPop1925_1949->currentIndex()),
//            ui.cbOpe1925_1949->currentIndex())){
//            return true;
//        }
//    }

//    if(ui.cbFilters1950_1969->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p1950_1969)],
//            static_cast<Popularity>(ui.cbPop1950_1969->currentIndex()),
//            ui.cbOpe1950_1969->currentIndex())){
//            return true;
//        }
//    }


//    if(ui.cbFilters1970_1979->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p1970_1979)],
//            static_cast<Popularity>(ui.cbPop1970_1979->currentIndex()),
//            ui.cbOpe1970_1979->currentIndex())){
//            return true;
//        }
//    }


//    if(ui.cbFilters1980_1989->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p1980_1989)],
//            static_cast<Popularity>(ui.cbPop1980_1989->currentIndex()),
//            ui.cbOpe1980_1989->currentIndex())){
//            return true;
//        }
//    }


//    if(ui.cbFilters1990_1999->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p1990_1999)],
//            static_cast<Popularity>(ui.cbPop1990_1999->currentIndex()),
//            ui.cbOpe1990_1999->currentIndex())){
//            return true;
//        }
//    }

//    if(ui.cbFilters2000_2009->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p2000_2009)],
//            static_cast<Popularity>(ui.cbPop2000_2009->currentIndex()),
//            ui.cbOpe2000_2009->currentIndex())){
//            return true;
//        }
//    }

//    if(ui.cbFilters2010_2018->isChecked()){
//        if(apply_popularity_operator(
//            info.popularityPerPeriod[static_cast<size_t>(Period::p2010_2020)],
//            static_cast<Popularity>(ui.cbPop2010_2020->currentIndex()),
//            ui.cbOpe2010_2020->currentIndex())){
//            return true;
//        }
//    }

//    if(ui.cbFiltersDepartment->isChecked()){
//        for(const auto &dep : insideDepartments){
//            auto id  = get_id(dep);
////            if(std::get<2>(info.departmentsInfo[id]).v > 5 ){
////                return true;
////            }
//        }
//    }

//    return false;
//}




//bool MainWindow::save_saved_names_file(const QString &path) const{

//    QFile savedFile(path);
//    if(!savedFile.open(QFile::WriteOnly | QIODevice::Text)){
//        qWarning() << "Cannot save saved names file at path " % path;
//        return false;
//    }

//    QTextStream fileStream(&savedFile);
//    for(const auto &savedName : savedNames){
//        fileStream << savedName.first.v % "\n";
//    }

//    return true;
//}

//bool MainWindow::save_removed_names_file(const QString &path) const{

//    QFile removedFile(path);
//    if(!removedFile.open(QFile::WriteOnly | QIODevice::Text)){
//        qWarning() << "Cannot save removed names file at path " % path;
//        return false;
//    }

//    QTextStream fileStream(&removedFile);
//    for(const auto &removedName : removedNames){
//        fileStream << removedName.first.v % "\n";
//    }

//    return true;
//}
