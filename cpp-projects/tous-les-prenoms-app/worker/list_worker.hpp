
#pragma once

// std
#include <mutex>

// Qt
#include <QObject>
#include <QDebug>

// utility
#include "utility/benchmark.hpp"
// data
#include "data/filters.hpp"

namespace tool {


class ListWorker : public QObject{
    Q_OBJECT
public:

    ListWorker(){

    }

public slots:

    void apply_filter(Data *data, FilterSettings filterS){

        lock.lock();

        tool::Bench::start("apply_filter");

        Filters filters;
        filters.apply(filterS, *data);

//        data->filteredNames

        auto names = std::make_shared<QStringList>();
        names->reserve(data->countFiltered);
        for(size_t ii = 0; ii < data->countFiltered; ++ii){
            *names << data->filteredNames[ii].v.toString();
        }

        tool::Bench::stop();

        qDebug() << "names filtered " << names->size();
        lock.unlock();
        emit list_filtered_signal(std::move(names));
    }

signals:

    void list_filtered_signal(std::shared_ptr<QStringList> names);

private:
    std::mutex lock;

};
}
