
#pragma once

// std
#include <array>

// local
#include "utility/tuple_array.hpp"
#include "year.hpp"

namespace tool {

using namespace std::literals::string_view_literals;


enum class Period : std::uint8_t {
    p1900_1924=0,
    p1925_1949,
    p1950_1969,
    p1970_1979,
    p1980_1989,
    p1990_1999,
    p2000_2009,
    p2010_2020,
    p1900_2020,
    SizeEnum
};

using PerName   = std::string_view;
using TPer= std::tuple<
             Period, Interval, float, PerName>;
static constexpr TupleArray<Period::SizeEnum, TPer> periods = {{
    TPer
    {Period::p1900_1924, Interval{{1900}, {1924}},   25.f / fullInterval.length(), "1900-1924"sv,},
    {Period::p1925_1949, Interval{{1925}, {1949}},   25.f / fullInterval.length(), "1924-1949"sv},
    {Period::p1950_1969, Interval{{1950}, {1969}},   20.f / fullInterval.length(), "1950-1969"sv},
    {Period::p1970_1979, Interval{{1970}, {1979}},   10.f / fullInterval.length(), "1970-1979"sv},
    {Period::p1980_1989, Interval{{1980}, {1989}},   10.f / fullInterval.length(), "1980-1989"sv},
    {Period::p1990_1999, Interval{{1990}, {1999}},   10.f / fullInterval.length(), "1990-1999"sv},
    {Period::p2000_2009, Interval{{2000}, {2009}},   10.f / fullInterval.length(), "2000-2009"sv},
    {Period::p2010_2020, Interval{{2010}, {2020}},   8.f  / fullInterval.length(), "2010-2020"sv},
    {Period::p1900_2020, Interval{{1900}, {2020}},   1.f  / fullInterval.length(), "1900-2020"sv},
}};

constexpr Period get_period(Year y){

    for(const auto &p : periods.data){
        if(std::get<1>(p).inside(y)){
            return std::get<0>(p);
        }
    }
    return Period::p1900_2020;
}

constexpr float get_factor(Period period){
    return periods.at<0,2>(period);
}

static constexpr PerName get_name(Period period){
    return periods.at<0,3>(period);
}

}
