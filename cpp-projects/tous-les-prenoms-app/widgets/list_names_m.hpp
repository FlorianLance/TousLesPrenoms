

#pragma once

// Qt
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

// local
#include "data/data.hpp"
#include "data/settings.hpp"

namespace tool::ui {

enum class Mode {
  Filtered,Saved,Removed
};

class ListNamesM : public QAbstractListModel{
Q_OBJECT
public:

    ListNamesM(Mode mode) : m_mode(mode){}

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role= Qt::DisplayRole) const;
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const{
//    return QVariant();
//    }
    void update(){
        beginResetModel();
        endResetModel();
    }

    tool::Data *nData = nullptr;
    DisplaySettings *dSettings = nullptr;

    bool initialized = false;

private:
    Mode m_mode;
};


}
