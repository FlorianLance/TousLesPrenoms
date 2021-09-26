
#pragma once

// std
#include <functional>

namespace tool {


struct Year{
    std::int16_t v;
};

[[maybe_unused]] static constexpr bool operator<(const Year &y1, const Year &y2){
    return y1.v < y2.v;
}

[[maybe_unused]] static constexpr bool operator>(const Year &y1, const Year &y2){
    return y1.v > y2.v;
}

static constexpr bool operator<=(const Year &y1, const Year &y2){
    return y1.v <= y2.v;
}

static constexpr bool operator>=(const Year &y1, const Year &y2){
    return y1.v >= y2.v;
}

static bool operator==(const Year &y1, const Year &y2){
    return y1.v == y2.v;
}


struct Interval{
    Year start;
    Year end;

    constexpr bool inside(Year y) const{
        return (y >= start) && (y <= end);
    }

    constexpr int length()const{
        return end.v-start.v;
    }


};


static constexpr Year StartYear = {1900};
static constexpr Year EndYear   = {2020};
[[maybe_unused]] static constexpr Interval fullInterval = {StartYear,EndYear};

}


namespace std{

template<>
class hash<tool::Year>{
public:
    size_t operator()(tool::Year const& y) const{
        return std::hash<int>{}(y.v);
    }
};
}
