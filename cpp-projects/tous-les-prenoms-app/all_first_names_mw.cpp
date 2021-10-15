
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
#include <QTimer>


// local
#include "utility/convert.hpp"
#include "utility/benchmark.hpp"
#include "utility/qt_ui.hpp"
#include "utility/qt_str.hpp"


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

    // start thread
    listWorkerT.start();
}

void AllFirstNamesMW::init_ui(){

    ui.setupUi(this);

    init_ui_lists();
    init_ui_filters();
    init_ui_display();
    init_ui_curves();
    init_ui_map();

    update_ui_from_settings();
}

void AllFirstNamesMW::init_ui_lists(){

    ui.vlFiltered->addWidget(filteredNamesV = new QListView());
    filteredNamesM = std::make_unique<ui::ListNamesM>(ui::Mode::Filtered);
    filteredNamesV->setMouseTracking(true);
    filteredNamesM->nData = &data;
    filteredNamesM->dSettings = &settings.display;
    filteredNamesV->setModel(filteredNamesM.get());
    filteredNamesV->viewport()->installEventFilter(this);

    ui.vlSaved->insertWidget(0, savedNamesV = new QListView());
    savedNamesM = std::make_unique<ui::ListNamesM>(ui::Mode::Saved);
    savedNamesV->setMouseTracking(true);
    savedNamesM->nData = &data;
    savedNamesM->dSettings = &settings.display;
    savedNamesV->setModel(savedNamesM.get());
    savedNamesV->viewport()->installEventFilter(this);

    ui.vlRemoved->insertWidget(0, removedNamesV = new QListView());
    removedNamesM = std::make_unique<ui::ListNamesM>(ui::Mode::Removed);
    removedNamesV->setMouseTracking(true);
    removedNamesM->nData = &data;
    removedNamesM->dSettings = &settings.display;
    removedNamesV->setModel(removedNamesM.get());
    removedNamesV->viewport()->installEventFilter(this);
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
        "au moins", "égale", "au plus"
    };
    ui.cbOpePer->addItems(opeItems);
    ui.cbOpePer->setCurrentIndex(1);

    ui.cbOpeDep->addItems(opeItems);
    ui.cbOpeDep->setCurrentIndex(1);

    QStringList popItems;
    for(const auto &popularity : popularities.data){
        popItems << from_view(std::get<2>(popularity));
    }

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
        "2000-2009", "2010-2020", "1900-2020"
    };
    ui.cbPeriod->addItems(perItems);
    ui.cbPeriod->setCurrentIndex(8);


    const QStringList yearsOpeItems = {
        "avant", "durant", "après", "inconnu"
    };
    ui.cbOpeAppearsYear->addItems(yearsOpeItems);    
    ui.cbOpeLastAppearsYear->addItems(yearsOpeItems);
    ui.cbOpePeakYear->addItems(yearsOpeItems);

    ui.cbOpeAppearsYear->setCurrentIndex(2);
    ui.cbOpeLastAppearsYear->setCurrentIndex(0);
    ui.cbOpePeakYear->setCurrentIndex(1);

    // periods
    for(int ii = 0; ii < perItems.size(); ++ii){
        ui.twPeriodsInfo->insertRow(0);
    }
    for(int ii = 0; ii < perItems.size(); ++ii){

        std::vector<QLabel*> lineWidgets;
        for(int jj = 0; jj < ui.twPeriodsInfo->columnCount(); ++jj){
            QLabel *la = new QLabel("-");
            ui.twPeriodsInfo->setCellWidget(ii, jj, la);
            lineWidgets.push_back(la);
        }

        tablePeriodsInfoWidgets.push_back(std::move(lineWidgets));

    }
    ui.twPeriodsInfo->setVerticalHeaderLabels(perItems);

    // departments
    QStringList departmentsItems;
    for(const auto &dep : departments.data){
        departmentsItems << QSL("(") % QString::number(std::get<1>(dep).v) % QSL(") ") % from_view(std::get<2>(dep));
    }
    departmentsItems.removeLast();
    ui.cbDep->addItems(departmentsItems);
    for(int ii = 0; ii < departmentsItems.size(); ++ii){
        ui.twDepInfo->insertRow(0);
    }
    for(int ii = 0; ii < departmentsItems.size(); ++ii){

        std::vector<QLabel*> lineWidgets;
        for(int jj = 0; jj < ui.twDepInfo->columnCount(); ++jj){
            QLabel *la = new QLabel("-");
            ui.twDepInfo->setCellWidget(ii, jj, la);
            lineWidgets.push_back(la);
        }

        tableDepartmentsInfoWidgets.push_back(std::move(lineWidgets));
    }
    ui.twDepInfo->setVerticalHeaderLabels(departmentsItems);

    // tooltip
    ui.pbGenderHelp->setToolTip(
        "Utilisation du prénom:\n"
        "-totalement: plus de 98%\n"
        "-majoritairement: plus de 75%\n"
        "-équilibrée: entre 30% et 60%"
    );

}

void AllFirstNamesMW::init_ui_curves(){

    curveW = new CurveW();
    curveW->set_fitted_state(false);
    curveW->remove_symbol();

    QPen p(Qt::blue);
    p.setWidthF(1.5);
    curveW->set_pen(p);

    ui.vlCurve->addWidget(curveW);
}

void AllFirstNamesMW::init_ui_map(){
    mapW = new MapW(ui.hlMap);
}


void AllFirstNamesMW::init_ui_display(){

    // dialogs
    const auto &backC = settings.display.genderRepartitionsBackgroundColors;
    const auto &foreC = settings.display.genderRepartitionsForegroundColors;
    for(auto g : {GR::OnlyFemale, GR::OnlyMale, GR::MostlyFemale, GR::MostlyMale, GR::FemaleMale}){
        backgroundGendersColorsD[g] = std::make_unique<QColorDialog>(backC.at(g));
        foregroundGendersColorsD[g] = std::make_unique<QColorDialog>(foreC.at(g));
    }

    backgroundGendersColorsB = {
        {GR::OnlyFemale,    ui.pbTotalFemaleColorBackground},
        {GR::OnlyMale,      ui.pbTotalMaleColorBackground},
        {GR::MostlyMale,    ui.pbMajorFemaleColorBackground},
        {GR::MostlyFemale,  ui.pbMajorMaleColorBackground},
        {GR::FemaleMale,    ui.pbBalancedFemaleMaleColorBackground}
    };
    foregroundGendersColorsB = {
        {GR::OnlyFemale,    ui.pbTotalFemaleColorName},
        {GR::OnlyMale,      ui.pbTotalMaleColorName},
        {GR::MostlyMale,    ui.pbMajorFemaleColorName},
        {GR::MostlyFemale,  ui.pbMajorMaleColorName},
        {GR::FemaleMale,    ui.pbBalancedFemaleMaleColorName}
    };
}

void AllFirstNamesMW::init_connections(){

    init_connections_lists();
    init_connections_filters();
    init_connections_display();


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


}

void AllFirstNamesMW::init_connections_lists(){

    // worker
    connect(listWorker.get(), &ListWorker::list_filtered_signal, this, &AllFirstNamesMW::update_filtered_list);

    // sorting
    connect(ui.rbSortAZ,   &QRadioButton::toggled, this, [&](){update_filters_from_ui();});
    connect(ui.rbSortZA,   &QRadioButton::toggled, this, [&](){update_filters_from_ui();});
    connect(ui.rbSortPopI, &QRadioButton::toggled, this, [&](){update_filters_from_ui();});
    connect(ui.rbSortPopS, &QRadioButton::toggled, this, [&](){update_filters_from_ui();});

    // last name
    connect(ui.leLastName, &QLineEdit::textChanged, this, [&](){update_displayed_info();});

    // list view names
    connect(filteredNamesV, &QListView::entered, this, [&](const QModelIndex &index){
        if(!listFilteredMousePressed){
            return;
        }
        FirstNameV n{data.filteredNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_info();
    });
    connect(filteredNamesV, &QListView::clicked, this, [&](const QModelIndex &index){

        FirstNameV n{data.filteredNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_info();
    });

    connect(savedNamesV, &QListView::entered, this, [&](const QModelIndex &index){
        if(!listSavedMousePressed){
            return;
        }
        FirstNameV n{data.savedNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_info();
    });
    connect(savedNamesV, &QListView::clicked, this, [&](const QModelIndex &index){

        FirstNameV n{data.savedNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_info();
    });

    connect(removedNamesV, &QListView::entered, this, [&](const QModelIndex &index){
        if(!listRemovedMousePressed){
            return;
        }
        FirstNameV n{data.removedNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_info();
    });
    connect(removedNamesV, &QListView::clicked, this, [&](const QModelIndex &index){

        FirstNameV n{data.removedNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_info();
    });
}

void AllFirstNamesMW::init_connections_filters(){

    // this
    connect(this, &AllFirstNamesMW::apply_filter_signal, listWorker.get(), &ListWorker::apply_filter);

    // ui
    // # letters
    connect(ui.cbFiltersLetters, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbNbLetters, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbContains, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbDoNotContain, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbStartsBy, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbEndsBy, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.sbMinNbLetters,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int value){
        if(value > ui.sbMaxNbLetters->value()){
            ui.sbMinNbLetters->setValue(ui.sbMaxNbLetters->value());
        }
        update_filters_from_ui();
    });
    connect(ui.sbMaxNbLetters,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int value){
        if(value < ui.sbMinNbLetters->value()){
            ui.sbMaxNbLetters->setValue(ui.sbMinNbLetters->value());
        }
        update_filters_from_ui();
    });
    connect(ui.leStartsBy, &QLineEdit::textChanged, this, [&](){update_filters_from_ui();});
    connect(ui.leEndsBy, &QLineEdit::textChanged, this, [&](){update_filters_from_ui();});
    connect(ui.leContains, &QLineEdit::textChanged, this, [&](){update_filters_from_ui();});
    connect(ui.leDoNotContain, &QLineEdit::textChanged, this, [&](){update_filters_from_ui();});

    // # years
    connect(ui.sbAppearsYear,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](){update_filters_from_ui();});
    connect(ui.sbLastAppearsYear,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](){update_filters_from_ui();});
    connect(ui.sbPeakYear,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](){update_filters_from_ui();});
    connect(ui.cbFiltersYears, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbAppearsYear, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbLastAppearsYear, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbPeakYear, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbOpeAppearsYear, &QComboBox::currentTextChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbOpeLastAppearsYear, &QComboBox::currentTextChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbOpePeakYear, &QComboBox::currentTextChanged,  this, [&](){update_filters_from_ui();});

    // # dep
    connect(ui.leDep, &QLineEdit::textChanged, this, [&](){
        ui.leDep->blockSignals(true);
        ui.leDep->setText(ui.leDep->text().replace(";;",";"));
        ui.leDep->blockSignals(false);
        update_filters_from_ui();
    });
    connect(ui.cbOpeDep, &QComboBox::currentTextChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbPopDep, &QComboBox::currentTextChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.pbAddDep, &QPushButton::clicked, this, [&]{

        auto newDep = QString::number(std::get<1>(departments.data[ui.cbDep->currentIndex()]).v);
        auto currentTxt = ui.leDep->text();
        if(currentTxt.length() == 0){
            ui.leDep->setText(newDep);
        }else{
            for(auto depStr : currentTxt.split(";")){
                if(depStr == newDep){
                    return;
                }
            }

            if(currentTxt[currentTxt.size()-1] == ';'){
                ui.leDep->setText(currentTxt % newDep);
            }else{
                ui.leDep->setText(currentTxt % QSL(";") % newDep);
            }
        }
        update_filters_from_ui();
    });

    // # gender
    connect(ui.cbFiltersGender, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbOnlyFemale, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbOnlyMale, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbOnlyOther, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbMajorFemale, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbMajorMale, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbMajorOther, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbFemaleMale, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    // connect(ui.cbFemaleOther, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});
    // connect(ui.cbMaleOther, &QCheckBox::stateChanged,  this, [&](){update_filter_settings_from_ui();});

    // # period
    connect(ui.cbFiltersPopPeriod, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbFiltersPopDep, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbFiltersPopPeriod, &QCheckBox::stateChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbPeriod, &QComboBox::currentTextChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbOpePer, &QComboBox::currentTextChanged,  this, [&](){update_filters_from_ui();});
    connect(ui.cbPopPeriod, &QComboBox::currentTextChanged,  this, [&](){update_filters_from_ui();});
}

void AllFirstNamesMW::init_connections_display(){

    auto &foreC = settings.display.genderRepartitionsForegroundColors;
    auto &backC = settings.display.genderRepartitionsBackgroundColors;
    for(auto g : {GR::OnlyFemale, GR::OnlyMale, GR::MostlyFemale, GR::MostlyMale, GR::FemaleMale}){
        connect(foregroundGendersColorsB[g], &QPushButton::clicked, foregroundGendersColorsD[g].get(), &QColorDialog::show);
        connect(backgroundGendersColorsB[g], &QPushButton::clicked, backgroundGendersColorsD[g].get(), &QColorDialog::show);
        connect(foregroundGendersColorsD[g].get(), &QColorDialog::colorSelected, this, [&,g](const QColor &c){
            foreC[g] = c;
            update_display_from_ui();
        });
        connect(backgroundGendersColorsD[g].get(), &QColorDialog::colorSelected, this, [&,g](const QColor &c){
            backC[g] = c;
            update_filters_from_ui();
        });
    }
}


void AllFirstNamesMW::init_data(){

    SplashScreen splash;
    splash.show();
    splash.set_progress(1);

    connect(&data.iData, &InputData::set_progress_signal, this, [&](int value){
        splash.set_progress(value);
    });
    connect(&data.pData, &ProcessedData::set_progress_signal, this, [&](int value){
        splash.set_progress(value);
    });

    Bench::start("[init_data]");

    if(!data.init()){
        // ...
        return;
    }

    Bench::stop();
    Bench::display();
    Bench::reset();


    splash.set_progress(95);

    // read list files
    //    update_saved_list();
    //    update_removed_list();
    //    update_displayed_info();

    // load parameters
    // ...

    // update lists
    update_ui_from_settings();

    update_filters_from_ui(); // temp
    update_display_from_ui();

    splash.set_progress(100);

    // wait a little bit
    auto t = QTime::currentTime().addMSecs(300);
    while( QTime::currentTime() < t){
        QCoreApplication::processEvents( QEventLoop::AllEvents, 5);
    }






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
            listFilteredMousePressed = true;
        }else if(event->type() == QEvent::MouseButtonRelease){
            listFilteredMousePressed = false;
        }
    }
    if(obj == savedNamesV->viewport()){
        if(event->type() == QEvent::MouseButtonPress){
            listSavedMousePressed = true;
        }else if(event->type() == QEvent::MouseButtonRelease){
            listSavedMousePressed = false;
        }
    }
    if(obj == removedNamesV->viewport()){
        if(event->type() == QEvent::MouseButtonPress){
            listRemovedMousePressed = true;
        }else if(event->type() == QEvent::MouseButtonRelease){
            listRemovedMousePressed = false;
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
    save_saved_names_file(Paths::savedNamesFilePath);
//    save_removed_names_file(Paths::removedNamesFilePath);
}


void AllFirstNamesMW::update_filtered_list(){

    ui.twListNames->setTabText(0, QString("Filtrés (") % QString::number(data.countFiltered) % QSL(")"));

    Bench::start("[update_filtered_list]");
    filteredNamesM->update();
    filteredNamesV->viewport()->update();
    Bench::stop();

    update_displayed_info();
}


void AllFirstNamesMW::update_filters_from_ui(){

    // uppdate ui sub groups
    ui.wSubGender->setEnabled(ui.cbFiltersGender->isChecked());
    ui.wSubLetters->setEnabled(ui.cbFiltersLetters->isChecked());
    ui.wSubDep->setEnabled(ui.cbFiltersPopDep->isChecked());
    ui.wSubYear->setEnabled(ui.cbFiltersYears->isChecked());
    ui.sbAppearsYear->setEnabled(ui.cbOpeAppearsYear->currentIndex() != 3);
    ui.sbPeakYear->setEnabled(ui.cbOpePeakYear->currentIndex() != 3);

    // filters
    // # gender
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
    // # letters
    settings.filters.filterLetters           = ui.cbFiltersLetters->isChecked();
    settings.filters.startsBy                = ui.cbStartsBy->isChecked();
    settings.filters.startsByTextes          = ui.leStartsBy->text().split(Filters::sepWords);
    settings.filters.endsBy                  = ui.cbEndsBy->isChecked();
    settings.filters.endsByTextes            = ui.leEndsBy->text().split(Filters::sepWords);
    settings.filters.contains                = ui.cbContains->isChecked();
    settings.filters.containsTextes          = ui.leContains->text().split(Filters::sepWords);
    settings.filters.doNoContains            = ui.cbDoNotContain->isChecked();
    settings.filters.doNotContainsTextes     = ui.leDoNotContain->text().split(Filters::sepWords);
    settings.filters.nbLetters               = ui.cbNbLetters->isChecked();;
    settings.filters.minLettersNb            = ui.sbMinNbLetters->value();
    settings.filters.maxLettersNb            = ui.sbMaxNbLetters->value();
    // # year
    // ## appears
    settings.filters.filterYear              = ui.cbFiltersYears->isChecked();
    settings.filters.appearsBefore           = ui.cbOpeAppearsYear->currentIndex() == 0 && ui.cbAppearsYear->isChecked();
    settings.filters.appearsDuring           = ui.cbOpeAppearsYear->currentIndex() == 1 && ui.cbAppearsYear->isChecked();
    settings.filters.appearsAfter            = ui.cbOpeAppearsYear->currentIndex() == 2 && ui.cbAppearsYear->isChecked();
    settings.filters.appearsUnknow           = ui.cbOpeAppearsYear->currentIndex() == 3 && ui.cbAppearsYear->isChecked();
    settings.filters.appearsYear             = ui.sbAppearsYear->value();
    // ## last appears
    settings.filters.lastAppearsBefore       = ui.cbOpeLastAppearsYear->currentIndex() == 0 && ui.cbLastAppearsYear->isChecked();
    settings.filters.lastAppearsDuring       = ui.cbOpeLastAppearsYear->currentIndex() == 1 && ui.cbLastAppearsYear->isChecked();
    settings.filters.lastAppearsAfter        = ui.cbOpeLastAppearsYear->currentIndex() == 2 && ui.cbLastAppearsYear->isChecked();
    settings.filters.lastAppearsUnknow       = ui.cbOpeLastAppearsYear->currentIndex() == 3 && ui.cbLastAppearsYear->isChecked();
    settings.filters.lastAppearsYear         = ui.sbAppearsYear->value();
    // ## peak
    settings.filters.peakBefore              = ui.cbOpePeakYear->currentIndex() == 0 && ui.cbPeakYear->isChecked();
    settings.filters.peakDuring              = ui.cbOpePeakYear->currentIndex() == 1 && ui.cbPeakYear->isChecked();
    settings.filters.peakAfter               = ui.cbOpePeakYear->currentIndex() == 2 && ui.cbPeakYear->isChecked();
    settings.filters.peakUnknow              = ui.cbOpePeakYear->currentIndex() == 3 && ui.cbPeakYear->isChecked();
    settings.filters.peakYear                = ui.sbPeakYear->value();
    // # period
    settings.filters.filterPopPeriod         = ui.cbFiltersPopPeriod->isChecked();
    settings.filters.period                  = static_cast<Period>(ui.cbPeriod->currentIndex());
    settings.filters.periodAtLeast           = ui.cbOpePer->currentIndex() == 0;
    settings.filters.periodEqual             = ui.cbOpePer->currentIndex() == 1;
    settings.filters.periodAtLast            = ui.cbOpePer->currentIndex() == 2;
    settings.filters.popPeriod               = static_cast<Popularity>(ui.cbPopPeriod->currentIndex());
    // # department
    settings.filters.filterPopDep            = ui.cbFiltersPopDep->isChecked();
    settings.filters.depAtLeast              = ui.cbOpeDep->currentIndex() == 0;
    settings.filters.depEqual                = ui.cbOpeDep->currentIndex() == 1;
    settings.filters.depAtLast               = ui.cbOpeDep->currentIndex() == 2;
    settings.filters.popDep                  = static_cast<Popularity>(ui.cbPopDep->currentIndex());
    settings.filters.insideDepartments.clear();
    for(auto split : ui.leDep->text().split(";")){
        if(split == ";"){
            continue;
        }
        if(auto dep = get_department(DepartmentId{split.toInt()}); dep.has_value()){
            settings.filters.insideDepartments.insert(dep.value());
        }
    }

    // # sorting
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

void AllFirstNamesMW::update_display_from_ui(){

    auto &foreC = settings.display.genderRepartitionsForegroundColors;
    auto &backC = settings.display.genderRepartitionsBackgroundColors;

    for(auto g : {GR::OnlyFemale, GR::OnlyMale, GR::MostlyFemale, GR::MostlyMale, GR::FemaleMale}){
        UiUtility::fill_button_icon(foregroundGendersColorsB[g], foreC[g]);
        UiUtility::fill_button_icon(backgroundGendersColorsB[g], backC[g]);
    }

    update_filtered_list();
    update_displayed_info();
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



bool AllFirstNamesMW::save_saved_names_file(const QString &path) const{

    QFile savedFile(path);
    if(!savedFile.open(QFile::WriteOnly | QIODevice::Text)){
        qWarning() << "Cannot open saved names file at path " % path;
        return false;
    }

    QTextStream fileStream(&savedFile);
    for(const auto &savedName : data.savedNames){
        fileStream << savedName.v % "\n";
    }

    return true;
}


void AllFirstNamesMW::update_displayed_info(){

    if(data.currentName.v.length() == 0){
        return;
    }

    Bench::start("[update_displayed_info]");


    const auto gr = data.pData.infosPerName[data.currentName].genderRepartition;
    const QColor backgroundCol = settings.display.genderRepartitionsBackgroundColors[gr];
    const QColor foregroundCol = settings.display.genderRepartitionsForegroundColors[gr];

    ui.laFirstName->setStyleSheet(QSL("QLabel[objectName^=\"laFirstName\"] { color:rgb(")
        % QString::number(foregroundCol.red()) % "," % QString::number(foregroundCol.green()) % "," % QString::number(foregroundCol.blue()) % QSL("); }"));
    ui.laLastName->setStyleSheet(QSL("QLabel[objectName^=\"laLastName\"] { color:rgb(")
        % QString::number(foregroundCol.red()) % "," % QString::number(foregroundCol.green()) % "," % QString::number(foregroundCol.blue()) % QSL("); }"));
    ui.wCurrentName->setStyleSheet(QSL("QWidget[objectName^=\"wCurrentName\"] { background-color:rgb(")
        % QString::number(backgroundCol.red()) % "," % QString::number(backgroundCol.green()) % "," % QString::number(backgroundCol.blue()) % QSL("); }"));

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

    ui.laDep->setText(from_view(get_name(nameInfo.mostCommonDepartment)));

    // infos per period
    size_t ii = 0;
    for(const auto &period : periods.data){
        const auto p = std::get<0>(period);
        const auto pId = static_cast<std::uint8_t>(p);
        auto &lw = tablePeriodsInfoWidgets[ii];
        lw[0]->setText(QString::number(nameInfo.countPerPeriod[pId].v));
        lw[1]->setText(QString::number(data.pData.infosPerGender[Gender::Female].countNamePeriod[p][nameInfo.name].v));
        lw[2]->setText(QString::number(data.pData.infosPerGender[Gender::Male].countNamePeriod[p][nameInfo.name].v));
        lw[3]->setText(QString::number(nameInfo.orderPerPeriod[pId].v+1));
        lw[4]->setText(from_view(get_name(nameInfo.popularityPerPeriod[pId])));
        ++ii;
    }

    // infos per department
    ii = 0;
    for(const auto &dep : departments.data){
        const auto d = std::get<0>(dep);
        if(d == Department::Inconnu){
            continue;
        }
        auto &lw = tableDepartmentsInfoWidgets[ii];
        lw[0]->setText(QString::number(data.pData.infosPerDepartment[d].infosPerName[nameInfo.name].count.v));
        lw[1]->setText(QString::number(data.pData.infosPerDepartment[d].infosPerName[nameInfo.name].nbFemales.v));
        lw[2]->setText(QString::number(data.pData.infosPerDepartment[d].infosPerName[nameInfo.name].nbMales.v));
        lw[3]->setText(QString::number(data.pData.infosPerDepartment[d].infosPerName[nameInfo.name].order.v+1));
        lw[4]->setText(from_view(get_name(data.pData.infosPerDepartment[d].infosPerName[nameInfo.name].popularity)));
        ++ii;
    }

    // curves
    size_t max = 0;
    std::vector<double> years;
    for(size_t ii = 1900; ii <= 2020; ++ii){
        years.emplace_back(static_cast<double>(ii));
    }

    std::vector<double> counts;
    for(auto& info : data.pData.infosPerYear){
        if(info.first.v == -1){
            continue;
        }

        auto count = info.second.counterName[nameInfo.name];
        counts.emplace_back(static_cast<double>(count.v));
        if(count.v > max){
            max = count.v;
        }
    }
    if(max == 0){
        max = 10;
    }

    if(ui.cbCurves->currentIndex() == 0){

        curveW->set_title("");
        curveW->set_x_title("Années");
        curveW->set_y_title("Nombre");
        curveW->set_x_range(1900, 2020);
        curveW->set_y_range(0, 1.2*max);
        curveW->set_points(years, counts);
    }

    Bench::stop();
    Bench::display();
    Bench::reset();

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

void SplashScreen::set_progress(int value){
    m_progress = value;
    if (m_progress > 100){
        m_progress = 100;
    }
    if (m_progress < 0){
        m_progress = 0;
    }
    update();
    QCoreApplication::processEvents( QEventLoop::AllEvents, 5);
}

void SplashScreen::drawContents(QPainter *painter){

    QSplashScreen::drawContents(painter);

    // style
    QStyleOptionProgressBar pbstyle;
    pbstyle.initFrom(this);
    pbstyle.state = QStyle::State_Enabled;
    pbstyle.textVisible = false;
    pbstyle.minimum = 0;
    pbstyle.maximum = 100;
    pbstyle.progress = m_progress;
    pbstyle.invertedAppearance = false;
    pbstyle.rect = QRect(40, 210, 810, 20); // Where is it.

    // draw
    style()->drawControl(QStyle::CE_ProgressBar, &pbstyle, painter, this);
}
