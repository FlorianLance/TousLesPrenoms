
#include "list_names_m.hpp"

// Qt
#include <QColor>

using namespace tool;
using namespace tool::ui;


int ListNamesM::rowCount(const QModelIndex &) const{


    if(nData == nullptr){
        return 0;
    }
//    qDebug() << "rowCount " << nData->countFiltered;
    return nData->countFiltered;
}

QVariant ListNamesM::data(const QModelIndex &index, int role) const{

    if (!index.isValid()){
        return QVariant();
    }

//    qDebug() << "data ! " << index.row() << role;
    if(nData == nullptr){
        return QVariant();
    }

    if (role == Qt::DisplayRole){
        return nData->filteredNames[index.row()].v.toString();
    }else if (role == Qt::BackgroundRole){
//        if(index.column() == 0){
//            return QColor(Qt::lightGray);
//        }else if (index.column() == 1){
            return QColor(Qt::white);
//        }
    }//else if (role == Qt::ForegroundRole){
//        if(index.column() == 0){
//            return QColor(Qt::black);
//        }else if (index.column() == 1){
//            auto id = order[index.row()];
//            return std::get<2>(elements.at(id)) ? QColor(Qt::green) : QColor(Qt::darkYellow);
//        }
//    }else if (role == Qt::TextAlignmentRole){
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
    auto currentNb = nData->countFiltered;

    endResetModel();

//    qDebug() << "ListNamesM update " << nData->countFiltered;
//    insertRows(0, currentNb);


//    for(const auto& time : times){
//        if(!elements.contains(time.first)){
//            elements[time.first] = std::make_tuple(from_view(time.first), 0, true);
//            order.emplace_back(time.first);
//        }else{
//            std::get<1>(elements[time.first]) = time.second;
//            std::get<2>(elements[time.first]) = true;
//        }
//    }

//    if(currentNb < elements.size()){
//        insertRows(0, elements.size()-currentNb);
//    }
}


