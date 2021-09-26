
#pragma once

// Qt
#include <QHash>
#include <QString>

namespace tool {

struct FirstName{
    QString v;
};
using RawFirstName = FirstName;
static bool operator<(const FirstName &y1, const FirstName &y2){
    return y1.v < y2.v;
}
static bool operator>(const FirstName &y1, const FirstName &y2){
    return y1.v > y2.v;
}
static bool operator==(const FirstName &y1, const FirstName &y2){
    return y1.v == y2.v;
}

struct FirstNameV{
    QStringView v;
};
using RawFirstNameV = FirstNameV;

static bool operator<(const FirstNameV &y1, const FirstNameV &y2){
    return y1.v < y2.v;
}
static bool operator>(const FirstNameV &y1, const FirstNameV &y2){
    return y1.v > y2.v;
}
static bool operator==(const FirstNameV &y1, const FirstNameV &y2){
    return y1.v == y2.v;
}

}

namespace std{

template<>
class hash<tool::FirstNameV>{
public:
    size_t operator()(tool::FirstNameV const& f) const{
        return qHash(f.v);
    }
};

}
