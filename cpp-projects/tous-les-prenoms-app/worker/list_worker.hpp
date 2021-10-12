
#pragma once

// std
#include <mutex>

// data
#include "data/filters.hpp"

namespace tool {

class ListWorker : public QObject{
    Q_OBJECT
public:

    ListWorker(){}

public slots:

    void apply_filter(Data *data, FilterSettings filterS){

        lock.lock();
            filters.apply(filterS, *data);
        lock.unlock();

        emit list_filtered_signal();
    }

signals:

    void list_filtered_signal();

private:
    std::mutex lock;
    Filters filters;
};
}
