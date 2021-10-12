

#pragma once

// Qt
#include <QAbstractListModel>

// local
#include "data/data.hpp"

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
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void update();

//    std::vector<std::pair<std::string_view, std::int64_t>> times;
//    std::unordered_map<std::string_view, std::tuple<QString, std::int64_t, bool>> elements;
//    std::vector<std::string_view> order;
    tool::Data *nData = nullptr;

private:
    Mode m_mode;
};

}
