


# /*******************************************************************************
# ** TousLesPrenoms                                                             **
# ** MIT License                                                                **
# ** Copyright (c) [2020] [Florian Lance]                                       **
# **                                                                            **
# ** Permission is hereby granted, free of charge, to any person obtaining a    **
# ** copy of this software and associated documentation files (the "Software"), **
# ** to deal in the Software without restriction, including without limitation  **
# ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
# ** and/or sell copies of the Software, and to permit persons to whom the      **
# ** Software is furnished to do so, subject to the following conditions:       **
# **                                                                            **
# ** The above copyright notice and this permission notice shall be included in **
# ** all copies or substantial portions of the Software.                        **
# **                                                                            **
# ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
# ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
# ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
# ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
# ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
# ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
# ** DEALINGS IN THE SOFTWARE.                                                  **
# **                                                                            **
# ********************************************************************************/

####################################### repo
TOUS_LES_PRENOMS_REPOSITORY_DIR      = $$PWD"/../.."

####################################### PRI
# defines compiling options
include(../settings.pri)
# defines projects paths and variables
include(../projects.pri)
# defines thirdparty includes and libs
include(../thirdparty.pri)

####################################### TARGET
equals(CFG, "debug"){
    TARGET = tous-les-prenoms-appd
}
equals(CFG, "release"){
    TARGET = tous-les-prenoms-app
}

####################################### TEMPLATE
equals(TOUS_LES_PRENOMS_APP_TARGET, "app"){
    TEMPLATE = app
    CONFIG += console
}

####################################### BUILD FILES
OBJECTS_DIR = $$TOUS_LES_PRENOMS_APP_OBJ
MOC_DIR     = $$TOUS_LES_PRENOMS_APP_MOC
RCC_DIR     = $$TOUS_LES_PRENOMS_APP_RCC
UI_DIR      = $$TOUS_LES_PRENOMS_APP_UI
DESTDIR     = $$TOUS_LES_PRENOMS_APP_DEST

####################################### CONFIG
CONFIG += qt
QT += core gui widgets network

####################################### INCLUDES
INCLUDEPATH += \
    # others
    $$QWT_INCLUDES \
    $$SIGNALS_INCLUDES \

####################################### LIBRAIRIES
LIBS +=\
    # others
    $$QWT_LIBS \

####################################### PROJECT FILES

SOURCES += \
    all_first_names_main.cpp \
    all_first_names_mw.cpp \
    data/filters.cpp \
    data/input_data.cpp \
    data/processed_data.cpp \
    data/settings.cpp \
    utility/benchmark.cpp \
    utility/convert.cpp \
    utility/logger.cpp \
    utility/time.cpp \
    widgets/curve_w.cpp \
    widgets/image_viewer_w.cpp \
    widgets/list_names_m.cpp \
    widgets/map_w.cpp

HEADERS += \
    all_first_names_mw.hpp \
    data/data.hpp \
    data/filters.hpp \
    data/first_name.hpp \
    data/gender.hpp \
    data/input_data.hpp \
    data/paths.hpp \
    data/popularity.hpp \
    data/processed_data.hpp \
    data/settings.hpp \
    utility/benchmark.hpp \
    utility/convert.hpp \
    utility/logger.hpp \
    utility/qt_str.hpp \
    utility/qt_ui.hpp \
    utility/time.hpp \
    utility/tuple_array.hpp \
    utility/types.hpp \
    widgets/curve_w.hpp \
    data/department.hpp \
    data/name_info.hpp \
    data/period.hpp \
    data/year.hpp \
    widgets/image_viewer_w.hpp \
    widgets/list_names_m.hpp \
    widgets/map_w.hpp \
    worker/list_worker.hpp

DISTFILES += \
  

FORMS += \
    all_first_names.ui

RESOURCES += \
    icons.qrc
