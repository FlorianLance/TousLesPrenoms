
#include "time.hpp"

// Qt
#include <QTime>
#include <QCoreApplication>

using namespace tool;

void Time::wait_process(int ms){
    QTime dieTime= QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < dieTime){
        QCoreApplication::processEvents( QEventLoop::AllEvents, 5 > ms ? ms : 5);
    }
}

std::chrono::nanoseconds Time::nanoseconds_since_epoch(){
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
}
