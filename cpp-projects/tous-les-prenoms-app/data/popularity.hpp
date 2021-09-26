
#pragma once

// local
#include "utility/tuple_array.hpp"
#include "utility/types.hpp"

namespace tool {

using namespace std::literals::string_view_literals;

enum class Popularity : int{
    Inexistant=0,
    Very_rare,
    Rare,
    Low,
    Average,
    Hight,
    Very_hight,
    Huge,
    SizeEnum
};

using Name   = std::string_view;

using TPop= std::tuple<
    Popularity,                 Percentage,         Name>;

static constexpr TupleArray<Popularity::SizeEnum, TPop> popularities = {{
    TPop
    {Popularity::Huge,          Percentage{0.995f}, "Huge"sv},
    {Popularity::Very_hight,    Percentage{0.98f},  "Very hight"sv},
    {Popularity::Hight,         Percentage{0.96f},  "Hight"sv},
    {Popularity::Average,       Percentage{0.92f},  "Average"sv},
    {Popularity::Low,           Percentage{0.85f},  "Low"sv},
    {Popularity::Rare,          Percentage{0.80f},  "Rare"sv},
    {Popularity::Very_rare,     Percentage{0.60f},  "Very rare"sv},
    {Popularity::Inexistant,    Percentage{0.f},    "Inexistant"sv},
}};


//Order{200},
//Order{500},
//Order{1000},
//Order{3000},
//Order{5000},
//Order{8000},
//Order{12000},
//Order{50000},

[[maybe_unused]] constexpr static Popularity get_popularity(Order order, Count count){

    const float percentage = 1.f*order.v/count.v;
    for(const auto &p : popularities.data){
        if(percentage > std::get<1>(p).v){
            return std::get<0>(p);
        }
    }
    return Popularity::Inexistant;
}


[[maybe_unused]] constexpr static Percentage get_percentage(Popularity popularity){
    return popularities.at<0,1>(popularity);
}

[[maybe_unused]] constexpr static Name get_name(Popularity popularity){
    return popularities.at<0,2>(popularity);
}

}
