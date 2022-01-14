

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

    ListNamesM(Mode mode);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role= Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const{
//    return QVariant();
//    }
    void update();
    void update2();

    tool::Data *nData = nullptr;
    DisplaySettings *dSettings = nullptr;

    bool initialized = false;

private:
    Mode m_mode;

    // QAbstractItemModel interface

};


}
