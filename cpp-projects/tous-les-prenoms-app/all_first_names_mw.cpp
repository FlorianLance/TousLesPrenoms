
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
#include <QScrollBar>


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
}

void AllFirstNamesMW::init_ui(){

    ui.setupUi(this);
    init_ui_lists();
    init_ui_filters();
    init_ui_display();
    init_ui_curves();
    init_ui_map();

    // set workers
    listWorker = std::make_unique<ListWorker>();
    listWorker->moveToThread(&listWorkerT);

    // start thread
    listWorkerT.start();

    update_ui_from_settings();

    // set connections
    init_connections();


    update_filters_from_ui();
//    emit apply_filter_signal(&data, settings.filters);
}

void AllFirstNamesMW::init_ui_lists(){

    ui.vlFiltered->addWidget(filteredNamesV = new ListView());
    filteredNamesM = std::make_unique<ui::ListNamesM>(ui::Mode::Filtered);
    filteredNamesV->setMouseTracking(true);
    filteredNamesM->nData = &data;
    filteredNamesM->dSettings = &settings.display;
    filteredNamesV->setModel(filteredNamesM.get());
    filteredNamesV->viewport()->installEventFilter(this);

    connect(filteredNamesV->verticalScrollBar(), &QScrollBar::valueChanged, this, [&](int value){

        auto id1 = filteredNamesV->indexAt(filteredNamesV->rect().topLeft());
        auto id2 = filteredNamesV->indexAt(filteredNamesV->rect().bottomLeft());
        qDebug() <<"rects " << id1.row() << id2.row();
        emit filteredNamesM->dataChanged(id1, id2);
    });

    ui.vlSaved->insertWidget(0, savedNamesV = new ListView());
    savedNamesM = std::make_unique<ui::ListNamesM>(ui::Mode::Saved);
    savedNamesV->setMouseTracking(true);
    savedNamesM->nData = &data;
    savedNamesM->dSettings = &settings.display;
    savedNamesV->setModel(savedNamesM.get());
    savedNamesV->viewport()->installEventFilter(this);


    ui.vlRemoved->insertWidget(0, removedNamesV = new ListView());
    removedNamesM = std::make_unique<ui::ListNamesM>(ui::Mode::Removed);
    removedNamesV->setMouseTracking(true);
    removedNamesM->nData = &data;
    removedNamesM->dSettings = &settings.display;
    removedNamesV->setModel(removedNamesM.get());
    removedNamesV->viewport()->installEventFilter(this);

    QCoreApplication::processEvents();

    removedNamesM->update();
    savedNamesM->update();
    filteredNamesM->update();

    QCoreApplication::processEvents();
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
    mapW = new MapW(ui.vlMap);
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
        {GR::MostlyMale,    ui.pbMajorMaleColorBackground},
        {GR::MostlyFemale,  ui.pbMajorFemaleColorBackground},
        {GR::FemaleMale,    ui.pbBalancedFemaleMaleColorBackground}
    };
    foregroundGendersColorsB = {
        {GR::OnlyFemale,    ui.pbTotalFemaleColorName},
        {GR::OnlyMale,      ui.pbTotalMaleColorName},
        {GR::MostlyMale,    ui.pbMajorMaleColorName},
        {GR::MostlyFemale,  ui.pbMajorFemaleColorName},
        {GR::FemaleMale,    ui.pbBalancedFemaleMaleColorName}
    };
}

void AllFirstNamesMW::init_connections(){

    init_connections_lists();
    init_connections_filters();
    init_connections_display();


    connect(ui.pbKeepCurrentName, &QPushButton::clicked, this, &AllFirstNamesMW::save_current_name);
    connect(ui.pbRemoveCurrentName, &QPushButton::clicked, this, &AllFirstNamesMW::remove_current_name);
    connect(ui.pbPrevious, &QPushButton::clicked, this, &AllFirstNamesMW::previous_name);
    connect(ui.pbNext, &QPushButton::clicked, this, &AllFirstNamesMW::next_filtered_name);

    connect(ui.cbMap, &QComboBox::currentTextChanged, this, [&]{
        if(data.currentName.v.length() > 0)
            update_displayed_map(data.currentName);
        }
    );



}

void AllFirstNamesMW::init_connections_lists(){

    // worker
    connect(listWorker.get(), &ListWorker::list_filtered_signal, this, [&]{
        update_filtered_list();
        update_displayed_current_name_infos();
    });

    // sorting
    connect(ui.rbSortAZ,   &QRadioButton::toggled, this, [&](){update_filters_from_ui();});
    connect(ui.rbSortZA,   &QRadioButton::toggled, this, [&](){update_filters_from_ui();});
    connect(ui.rbSortPopI, &QRadioButton::toggled, this, [&](){update_filters_from_ui();});
    connect(ui.rbSortPopS, &QRadioButton::toggled, this, [&](){update_filters_from_ui();});

    // last name
    connect(ui.leLastName, &QLineEdit::textChanged, this, [&](){update_displayed_current_name_infos();});

    // filtered names
    connect(filteredNamesV, &QListView::entered, this, [&](const QModelIndex &index){
        if(!listFilteredMousePressed){
            return;
        }

        if(index.row() >= static_cast<int>(data.filteredNames.size())){
            return;
        }

        FirstNameV n{data.filteredNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_current_name_infos();
    });
    connect(filteredNamesV, &QListView::clicked, this, [&](const QModelIndex &index){

        if(index.row() >= static_cast<int>(data.filteredNames.size())){
            return;
        }

        FirstNameV n{data.filteredNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_current_name_infos();
    });

    // saved names
    connect(ui.pbRetireSelectionFromSaved, &QPushButton::clicked, this, &AllFirstNamesMW::unsave_current_name);
    connect(ui.pbRetireAllFromSaved, &QPushButton::clicked, this, &AllFirstNamesMW::clear_all_saved_names);
    connect(savedNamesV, &QListView::entered, this, [&](const QModelIndex &index){
        if(!listSavedMousePressed){
            return;
        }

        if(index.row() >= static_cast<int>(data.savedNames.size())){
            return;
        }

        FirstNameV n{data.savedNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_current_name_infos();
    });
    connect(savedNamesV, &QListView::clicked, this, [&](const QModelIndex &index){

        if(index.row() >= static_cast<int>(data.savedNames.size())){
            return;
        }

        FirstNameV n{data.savedNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_current_name_infos();
    });

    // removed names
    connect(ui.pbRetireSelectionFromRemoved, &QPushButton::clicked, this, &AllFirstNamesMW::unremove_current_name);
    connect(ui.pbRetireAllFromRemoved, &QPushButton::clicked, this, &AllFirstNamesMW::clear_all_removed_names);
    connect(removedNamesV, &QListView::entered, this, [&](const QModelIndex &index){
        if(!listRemovedMousePressed){
            return;
        }

        if(index.row() >= static_cast<int>(data.removedNames.size())){
            return;
        }

        FirstNameV n{data.removedNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_current_name_infos();
    });
    connect(removedNamesV, &QListView::clicked, this, [&](const QModelIndex &index){

        if(index.row() >= static_cast<int>(data.removedNames.size())){
            return;
        }

        FirstNameV n{data.removedNames[index.row()]};
        if(data.namesState.count(n) == 0){
            return;
        }
        data.currentName = n;
        update_displayed_current_name_infos();
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
            update_display_from_ui();
        });
    }
}


void AllFirstNamesMW::init_data(){

    Bench::start("[init_data]");

    SplashScreen splash;
    splash.show();
    splash.set_progress(1);

    connect(&data.iData, &InputData::set_progress_signal, this, [&](int value){
        splash.set_progress(value);
    });
    connect(&data.pData, &ProcessedData::set_progress_signal, this, [&](int value){
        splash.set_progress(value);
    });


    data.init();
    splash.set_progress(95);

    // read list files
    data.read_saved_names_file(Paths::savedNamesFilePath);
    data.read_removed_names_file(Paths::removedNamesFilePath);

    // read settings file
    if(!QFile(Paths::settingsFilePath).exists()){
        settings.save_settings_file(Paths::settingsFilePath);
    }else{
        settings.read_settings_file(Paths::settingsFilePath);
    }
    qDebug() << "lastName " << settings.lastName;

    Bench::stop();
    splash.set_progress(100);

    // wait a little bit
    auto t = QTime::currentTime().addMSecs(300);
    while( QTime::currentTime() < t){
        QCoreApplication::processEvents( QEventLoop::AllEvents, 5);
    }
}

void AllFirstNamesMW::update_ui_from_settings(){

    update_saved_list();
    update_removed_list();
    update_filtered_list();

    ui.leLastName->setText(settings.lastName);

    // filters
    const auto &f = settings.filters;
    // # gender
    ui.cbFiltersGender->setChecked(f.filterGender);
    ui.cbOnlyFemale->setChecked(f.onlyFemale);
    ui.cbOnlyMale->setChecked(f.onlyMale);
    ui.cbOnlyOther->setChecked(f.onlyOther);
    ui.cbMajorFemale->setChecked(f.majorFemale);
    ui.cbMajorMale->setChecked(f.majorMale);
    ui.cbMajorOther->setChecked(f.majorOther);
    ui.cbFemaleMale->setChecked(f.femaleMale);
    // ui.cbFemaleOther->setChecked(f.femaleOther);
    // ui.cbMaleOther->setChecked(f.maleOther);
    // # letters
    ui.cbFiltersLetters->setChecked(f.filterLetters);
    ui.cbStartsBy->setChecked(f.startsBy);
    ui.leStartsBy->setText(f.startsByTextes.join(Filters::sepWords));
    ui.cbEndsBy->setChecked(f.endsBy);
    ui.leEndsBy->setText(f.endsByTextes.join(Filters::sepWords));
    ui.cbContains->setChecked(f.contains);
    ui.leContains->setText(f.containsTextes.join(Filters::sepWords));
    ui.cbDoNotContain->setChecked(f.doNoContains);
    ui.leDoNotContain->setText(f.doNotContainsTextes.join(Filters::sepWords));
    ui.cbNbLetters->setChecked(f.nbLetters);
    ui.sbMinNbLetters->setValue(f.minLettersNb);
    ui.sbMaxNbLetters->setValue(f.maxLettersNb);

    // # year
    ui.cbFiltersYears->setChecked(f.filterYear);
    // ## appears
    if(f.appearsBefore){
        ui.cbOpeAppearsYear->setCurrentIndex(0);
    }else if(f.appearsDuring){
        ui.cbOpeAppearsYear->setCurrentIndex(1);
    }else if(f.appearsAfter){
        ui.cbOpeAppearsYear->setCurrentIndex(2);
    }else{
        ui.cbOpeAppearsYear->setCurrentIndex(3);
    }
    ui.cbAppearsYear->setChecked(f.appearsBefore || f.appearsDuring || f.appearsAfter || f.appearsUnknow);
    ui.sbAppearsYear->setValue(f.appearsYear);
    // ## last appears
    if(f.lastAppearsBefore){
        ui.cbOpeLastAppearsYear->setCurrentIndex(0);
    }else if(f.lastAppearsDuring){
        ui.cbOpeLastAppearsYear->setCurrentIndex(1);
    }else if(f.lastAppearsAfter){
        ui.cbOpeLastAppearsYear->setCurrentIndex(2);
    }else{
        ui.cbOpeLastAppearsYear->setCurrentIndex(3);
    }
    ui.cbLastAppearsYear->setChecked(f.lastAppearsBefore || f.lastAppearsDuring || f.lastAppearsAfter || f.lastAppearsUnknow);
    ui.sbLastAppearsYear->setValue(f.lastAppearsYear);
    // ## peak
    if(f.peakBefore){
        ui.cbOpePeakYear->setCurrentIndex(0);
    }else if(f.peakDuring){
        ui.cbOpePeakYear->setCurrentIndex(1);
    }else if(f.peakAfter){
        ui.cbOpePeakYear->setCurrentIndex(2);
    }else{
        ui.cbOpePeakYear->setCurrentIndex(3);
    }
    ui.cbPeakYear->setChecked(f.peakBefore || f.peakDuring || f.peakAfter || f.peakUnknow);
    ui.sbPeakYear->setValue(f.peakYear);
    // # period
//    ui.cbFiltersPopPeriod->setChecked(f.filterPopPeriod);
////    qDebug() << "f period" << from_view(get_name(f.period));
////    ui.cbPeriod->setCurrentIndex(static_cast<int>(f.period));
//    if(f.periodAtLeast){
//        ui.cbOpePer->setCurrentIndex(0);
//    }else if(f.periodEqual){
//        ui.cbOpePer->setCurrentIndex(1);
//    }else {
//        ui.cbOpePer->setCurrentIndex(2);
//    }
////    ui.cbPopPeriod->setCurrentIndex(static_cast<std::uint8_t>(f.popPeriod));
//    // # department
//    ui.cbFiltersPopDep->setChecked(f.filterPopDep);
//    if(f.depAtLeast){
//        ui.cbOpeDep->setCurrentIndex(0);
//    }else if(f.depEqual){
//        ui.cbOpeDep->setCurrentIndex(1);
//    }else {
//        ui.cbOpeDep->setCurrentIndex(2);
//    }
////    ui.cbPopDep->setCurrentIndex(static_cast<int>(f.popDep));
//    QStringList depStr;
//    for(const auto &d : f.insideDepartments){
//        depStr << QString::number(static_cast<int>(d));
//    }
//    ui.leDep->setText(depStr.join(";"));

//    // # sorting
//    ui.rbSortAZ->setEnabled(settings.filters.sortAZ);
//    ui.rbSortZA->setEnabled(settings.filters.sortZA);
//    ui.rbSortPopS->setEnabled(settings.filters.sortPopS);
//    ui.rbSortPopI->setEnabled(settings.filters.sortPopI);

    // uppdate ui sub groups
    ui.wSubGender->setEnabled(ui.cbFiltersGender->isChecked());
    ui.wSubLetters->setEnabled(ui.cbFiltersLetters->isChecked());
    ui.wSubDep->setEnabled(ui.cbFiltersPopDep->isChecked());
    ui.wSubYear->setEnabled(ui.cbFiltersYears->isChecked());

    ui.cbOpeAppearsYear->setEnabled(ui.cbAppearsYear->isChecked());
    ui.sbAppearsYear->setEnabled(ui.cbOpeAppearsYear->currentIndex() != 3 && ui.cbAppearsYear->isChecked());

    ui.cbOpeLastAppearsYear->setEnabled(ui.cbLastAppearsYear->isChecked());
    ui.sbLastAppearsYear->setEnabled(ui.cbOpeLastAppearsYear->currentIndex() != 3 && ui.cbLastAppearsYear->isChecked());

    ui.cbOpePeakYear->setEnabled(ui.cbPeakYear->isChecked());
    ui.sbPeakYear->setEnabled(ui.cbOpePeakYear->currentIndex() != 3 && ui.cbPeakYear->isChecked());
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

    return QObject::eventFilter(obj, event);
}

AllFirstNamesMW::~AllFirstNamesMW(){

    listWorkerT.quit();
    listWorkerT.wait();

    // save session    
    update_filters_from_ui();
    update_display_from_ui();
    settings.save_settings_file(Paths::settingsFilePath);
    data.save_saved_names_file(Paths::savedNamesFilePath);
    data.save_removed_names_file(Paths::removedNamesFilePath);
}


void AllFirstNamesMW::update_filtered_list(){

    Bench::start("[update_filtered_list]");
    ui.twListNames->setTabText(0, QString("Filtrés (") % QString::number(data.countFiltered) % QSL(")"));

    auto id1 = filteredNamesV->indexAt(filteredNamesV->rect().topLeft());
    auto id2 = filteredNamesV->indexAt(filteredNamesV->rect().bottomLeft());
    qDebug() <<"rects " << id1.row() << id2.row();
    emit filteredNamesM->dataChanged(id1, id2);

//    filteredNamesV->viewport()->update();
//    filteredNamesV->verticalScrollBar()->setRange(0, data.countFiltered);

//    if(filteredNamesV->currentIndex().row() > static_cast<int>(data.countFiltered)){
//        filteredNamesV->scrollToTop();
//    }

    Bench::stop();
}

void AllFirstNamesMW::update_saved_list(){

    ui.twListNames->setTabText(1, QString("Retenus (") % QString::number(data.countSaved) % QSL(")"));

    savedNamesM->update();
    savedNamesV->viewport()->update();
    savedNamesV->verticalScrollBar()->setRange(0, data.countSaved);

    if(savedNamesV->currentIndex().row() > static_cast<int>(data.countSaved)){
        savedNamesV->scrollToTop();
    }
}

void AllFirstNamesMW::update_removed_list(){

    ui.twListNames->setTabText(2, QString("Retirés (") % QString::number(data.countRemoved) % QSL(")"));

    removedNamesM->update();
    removedNamesV->viewport()->update();    
    removedNamesV->verticalScrollBar()->setRange(0, data.countRemoved);

    if(removedNamesV->currentIndex().row() > static_cast<int>(data.countRemoved)){
        removedNamesV->scrollToTop();
    }
}


void AllFirstNamesMW::update_filters_from_ui(){

    // uppdate ui sub groups
    ui.wSubGender->setEnabled(ui.cbFiltersGender->isChecked());
    ui.wSubLetters->setEnabled(ui.cbFiltersLetters->isChecked());
    ui.wSubDep->setEnabled(ui.cbFiltersPopDep->isChecked());
    ui.wSubYear->setEnabled(ui.cbFiltersYears->isChecked());

    ui.cbOpeAppearsYear->setEnabled(ui.cbAppearsYear->isChecked());
    ui.sbAppearsYear->setEnabled(ui.cbOpeAppearsYear->currentIndex() != 3 && ui.cbAppearsYear->isChecked());

    ui.cbOpeLastAppearsYear->setEnabled(ui.cbLastAppearsYear->isChecked());
    ui.sbLastAppearsYear->setEnabled(ui.cbOpeLastAppearsYear->currentIndex() != 3 && ui.cbLastAppearsYear->isChecked());

    ui.cbOpePeakYear->setEnabled(ui.cbPeakYear->isChecked());
    ui.sbPeakYear->setEnabled(ui.cbOpePeakYear->currentIndex() != 3 && ui.cbPeakYear->isChecked());


    settings.lastName                        = ui.leLastName->text();

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
    settings.filters.lastAppearsYear         = ui.sbLastAppearsYear->value();
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
}

void AllFirstNamesMW::update_display_from_ui(){



    auto &foreC = settings.display.genderRepartitionsForegroundColors;
    auto &backC = settings.display.genderRepartitionsBackgroundColors;

    for(auto g : {GR::OnlyFemale, GR::OnlyMale, GR::MostlyFemale, GR::MostlyMale, GR::FemaleMale}){
        UiUtility::fill_button_icon(foregroundGendersColorsB[g], foreC[g]);
        UiUtility::fill_button_icon(backgroundGendersColorsB[g], backC[g]);
    }

    update_filtered_list();
}

void AllFirstNamesMW::update_displayed_name(FirstNameV name){

    Bench::start("[update_displayed_name]");

    const auto gr = data.pData.infosPerName[name].genderRepartition;
    const QColor backgroundCol = settings.display.genderRepartitionsBackgroundColors[gr];
    const QColor foregroundCol = settings.display.genderRepartitionsForegroundColors[gr];
    ui.laFirstName->setStyleSheet(QSL("QLabel[objectName^=\"laFirstName\"] { color:rgb(")
                                  % QString::number(foregroundCol.red()) % "," % QString::number(foregroundCol.green()) % "," % QString::number(foregroundCol.blue()) % QSL("); }"));
    ui.laLastName->setStyleSheet(QSL("QLabel[objectName^=\"laLastName\"] { color:rgb(")
                                 % QString::number(foregroundCol.red()) % "," % QString::number(foregroundCol.green()) % "," % QString::number(foregroundCol.blue()) % QSL("); }"));
    ui.wCurrentName->setStyleSheet(QSL("QWidget[objectName^=\"wCurrentName\"] { background-color:rgb(")
                                   % QString::number(backgroundCol.red()) % "," % QString::number(backgroundCol.green()) % "," % QString::number(backgroundCol.blue()) % QSL("); }"));

    ui.laFirstName->setText(firstNameStartStyle % name.v % endStyle);
    ui.laLastName->setText(lastNameStartStyle % ui.leLastName->text().toUpper() % endStyle);

    ui.pbKeepCurrentName->setEnabled(!data.namesState[name].saved && !data.namesState[name].removed);
    ui.pbRemoveCurrentName->setEnabled(!data.namesState[name].removed && !data.namesState[name].saved);
    ui.pbPrevious->setEnabled(data.previousNames.size() > 0);

    Bench::stop();
}

void AllFirstNamesMW::update_displayed_map(FirstNameV name){

    Bench::start("[update_displayed_map]");

    double max = 0.;
    std::unordered_map<Dep, double> factors;
    factors.reserve(data.pData.infosPerDepartment.size());
    for(auto &depInfos : data.pData.infosPerDepartment){

        if(depInfos.first == Dep::Inconnu){
            continue;
        }

        if(ui.cbMap->currentIndex() == 0){
            factors[depInfos.first] = depInfos.second.infosPerName[name].count.v*(1.0/depInfos.second.factorToMax);
        }else{
            factors[depInfos.first] = depInfos.second.infosPerName[name].count.v;
        }
        if(factors[depInfos.first] > max){
            max = factors[depInfos.first];
        }
    }

    if(max != 0){
        for(auto &v : factors){
            v.second /= max;
        }
    }

    mapW->fill_map(factors);
    Bench::stop();
}

void AllFirstNamesMW::update_displayed_curve(FirstNameV name){

    Bench::start("[update_displayed_curve]");

    const auto &nameInfo = data.pData.infosPerName[name];

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
}

void AllFirstNamesMW::update_displayed_others_infos(FirstNameV name){

    Bench::start("[update_displayed_others_infos]");

    const auto &nameInfo = data.pData.infosPerName[name];
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

    Bench::stop();
}

void AllFirstNamesMW::save_current_name(){

    if(data.currentName.v.length() == 0){
        return;
    }

    data.namesState[data.currentName].saved = true;
    data.savedNames.push_back(data.currentName);
    data.countSaved++;

    update_saved_list();

    next_filtered_name();
}

void AllFirstNamesMW::unsave_current_name(){

    if(data.currentName.v.length() == 0){
        return;
    }
    if(!data.namesState[data.currentName].saved){
        return;
    }

    data.namesState[data.currentName].saved = false;
    data.savedNames.erase(std::remove(data.savedNames.begin(), data.savedNames.end(), data.currentName), data.savedNames.end());
    --data.countSaved;
    update_saved_list();

    next_saved_name();
}

void AllFirstNamesMW::clear_all_saved_names(){       

    for(const auto name : data.savedNames){
        data.namesState[name].saved = false;
    }
    data.savedNames.clear();
    data.countSaved = 0;

    update_saved_list();
    update_displayed_current_name_infos();
}

void AllFirstNamesMW::update_displayed_current_name_infos(){

    if(data.currentName.v.length() == 0){
        return;
    }

    Bench::start("update_displayed_current_name_infos");

    ui.pbRetireSelectionFromSaved->setEnabled(data.namesState[data.currentName].saved && data.savedNames.size() > 0);
    ui.pbRetireSelectionFromRemoved->setEnabled(data.namesState[data.currentName].removed && data.removedNames.size() > 0);

    update_displayed_name(data.currentName);
    update_displayed_others_infos(data.currentName);
    update_displayed_curve(data.currentName);
    update_displayed_map(data.currentName);

    Bench::stop();

    Bench::display();
    Bench::reset();
}

void AllFirstNamesMW::next_filtered_name(){


    if(data.filteredNames.size() < 1){
        return;
    }

    if(data.currentName.v.length() != 0){
        data.previousNames.push(data.currentName);
    }


    std::vector<size_t> ids;
    size_t id = 0;
    for(const auto &name : data.filteredNames){
        if(!data.namesState[name].removed && !data.namesState[name].saved && data.namesState[name].filtered){
            ids.push_back(id);
        }
        id++;
    }
    if(ids.size() == 0){
        return;
    }

    std::uniform_int_distribution<> distrib(0, static_cast<int>(ids.size())-1);
    auto genId = distrib(*gen);

    FirstNameV n{data.filteredNames[ids[genId]]};
    data.currentName = n;
    update_displayed_current_name_infos();
}

void AllFirstNamesMW::next_saved_name(){

    if(data.savedNames.size() < 1){
        return;
    }

    if(data.currentName.v.length() != 0){
        data.previousNames.push(data.currentName);
    }

    std::uniform_int_distribution<> distrib(0, static_cast<int>(data.savedNames.size())-1);
    auto genId = distrib(*gen);

    FirstNameV n{data.savedNames[genId]};
    data.currentName = n;
    update_displayed_current_name_infos();
}

void AllFirstNamesMW::remove_current_name(){

    if(data.currentName.v.length() == 0){
        return;
    }

    data.namesState[data.currentName].removed = true;
    data.removedNames.push_back(data.currentName);
    data.countRemoved++;

    update_removed_list();

    next_filtered_name();
}

void AllFirstNamesMW::unremove_current_name(){

    if(data.currentName.v.length() == 0){
        return;
    }
    if(!data.namesState[data.currentName].removed){
        return;
    }

    data.namesState[data.currentName].removed = false;
    data.removedNames.erase(std::remove(data.removedNames.begin(), data.removedNames.end(), data.currentName), data.removedNames.end());
    --data.countRemoved;

    update_removed_list();

    next_removed_name();
}

void AllFirstNamesMW::next_removed_name(){

    if(data.removedNames.size() < 1){
        return;
    }

    if(data.currentName.v.length() != 0){
        data.previousNames.push(data.currentName);
    }

    std::uniform_int_distribution<> distrib(0, static_cast<int>(data.removedNames.size())-1);
    auto genId = distrib(*gen);

    FirstNameV n{data.removedNames[genId]};
    data.currentName = n;
    update_displayed_current_name_infos();
}

void AllFirstNamesMW::clear_all_removed_names(){

    for(const auto name : data.removedNames){
        data.namesState[name].removed = false;
    }
    data.removedNames.clear();
    data.countRemoved = 0;

    update_removed_list();
    update_displayed_current_name_infos();
}

void AllFirstNamesMW::previous_name(){

    if(data.previousNames.size() == 0){
        return;
    }

    data.currentName = data.previousNames.top();
    data.previousNames.pop();
    update_displayed_current_name_infos();
}


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
