
#include "list_names_m.hpp"

// Qt
#include <QColor>

using namespace tool;
using namespace tool::ui;


int ListNamesM::rowCount(const QModelIndex &) const{
    if(nData == nullptr){
        return 0;
    }

    if(m_mode == Mode::Filtered){
        return nData->countFiltered;
    }else if(m_mode == Mode::Saved){
        return nData->countSaved;
    }
    return nData->countRemoved;
}

QVariant ListNamesM::data(const QModelIndex &index, int role) const{

    if (!index.isValid() || (nData == nullptr)){
        return QVariant();
    }

    if (role == Qt::DisplayRole){
        if(m_mode == Mode::Filtered){
            return nData->filteredNames[index.row()].v.toString();
        }else if(m_mode == Mode::Saved){
            return nData->savedNames[index.row()].v.toString();
        }else if(m_mode == Mode::Removed){
            return nData->removedNames[index.row()].v.toString();
        }
    }else if (role == Qt::BackgroundRole){
        return QColor(Qt::white);
    }else if (role == Qt::ForegroundRole){

        if(m_mode == Mode::Filtered){


        }
//        ndData->filteredNames;
//        if(index.column() == 0){
//            return QColor(Qt::black);
//        }else if (index.column() == 1){
//            auto id = order[index.row()];
//            return std::get<2>(elements.at(id)) ? QColor(Qt::green) : QColor(Qt::darkYellow);
//        }
    }//else if (role == Qt::TextAlignmentRole){
//        if(index.column() == 0){
//            return Qt::AlignLeft;
//        }else if(index.column() == 1){
//            return Qt::AlignCenter;
//        }
//    }

    return QVariant();
}

//QVariant ListNamesM::headerData(int section, Qt::Orientation orientation, int role) const{

//    return QVariant();
//}

void ListNamesM::update(){
    beginResetModel();
//    emit dataChanged(QModelIndex(0,0))
    endResetModel();
}


