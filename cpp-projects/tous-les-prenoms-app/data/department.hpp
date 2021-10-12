
#pragma once

// Qt
#include <QString>

// local
#include "data/popularity.hpp"


namespace tool {

using namespace std::literals::string_view_literals;

enum class Department : std::uint8_t{
    Inconnu=0,
    Ain,
    Aisne,
    Allier,
    Alpes_de_Haute_Provence,
    Hautes_Alpes,
    Alpes_Maritimes,
    Ardeche,
    Ardennes,
    Ariege,
    Aube,
    Aude,
    Aveyron,
    Bouches_du_Rhone,
    Calvados,
    Cantal,
    Charente,
    Charente_Maritime,
    Cher,
    Correze,
    Corse,
    Cote_d_Or,
    Cotes_d_Armor,
    Creuse,
    Dordogne,
    Doubs,
    Drome,
    Eure,
    Eure_et_Loir,
    Finistere,
    Gard,
    Haute_Garonne,
    Gers,
    Gironde,
    Herault,
    Ille_et_Vilaine,
    Indre,
    Indre_et_Loire,
    Isere,
    Jura,
    Landes,
    Loir_et_Cher,
    Loire,
    Haute_Loire,
    Loire_Altantique,
    Loiret,
    Lot,
    Lot_et_Garonne,
    Lozere,
    Maine_et_Loire,
    Manche,
    Marne,
    Haute_Marne,
    Mayenne,
    Meurthe_et_Moselle,
    Meuse,
    Morbihan,
    Moselle,
    Nievre,
    Nord,
    Oise,
    Orne,
    Pas_de_Calais,
    Puy_de_Dome,
    Pyrenees_Atlantiques,
    Hautes_Pyrenees,
    Pyrenees_Orientales,
    Bas_Rhin,
    Haut_Rhin,
    Rhone,
    Haute_Saone,
    Saone_et_Loire,
    Sarthe,
    Savoie,
    Haute_Savoie,
    Paris,
    Seine_Maritime,
    Seine_et_Marne,
    Yvelines,
    Deux_Sevres,
    Somme,
    Tarn,
    Tarn_et_Garonne,
    Var,
    Vaucluse,
    Vendee,
    Vienne,
    Haute_Vienne,
    Vosges,
    Yonne,
    Territoire_de_Belfort,
    Essone,
    Haut_de_Seine,
    Seine_Saint_Denis,
    Val_de_Marne,
    Val_d_Oise,
    Guadeloupe,
    Martinique,
    Guyane,
    Reunion,
    SizeEnum
};

struct DepartmentId{
    int v;
};
static bool operator<(const DepartmentId &d1, const DepartmentId &d2){
    return d1.v < d2.v;
}

static bool operator>(const DepartmentId &d1, const DepartmentId &d2){
    return d1.v > d2.v;
}

static bool operator==(const DepartmentId &d1, const DepartmentId &d2){
    return d1.v == d2.v;
}

using Dep = Department;
using DepName   = std::string_view;

using TDep= std::tuple<
    Department,                     DepartmentId,  DepName>;
static constexpr TupleArray<Dep::SizeEnum, TDep> departments = {{
    TDep    
    {Dep::Ain,                      {1},           "Ain"sv},
    {Dep::Aisne,                    {2},           "Aisne"sv},
    {Dep::Allier,                   {3},           "Allier"sv},
    {Dep::Alpes_de_Haute_Provence,  {4},           "Alpes-de-Haute-Provence"sv},
    {Dep::Hautes_Alpes,             {5},           "Hautes-Alpes"sv},
    {Dep::Alpes_Maritimes,          {6},           "Alpes-Maritimes"sv},
    {Dep::Ardeche,                  {7},           "Ardèche"sv},
    {Dep::Ardennes,                 {8},           "Ardennes"sv},
    {Dep::Ariege,                   {9},           "Ariège"sv},
    {Dep::Aube,                     {10},          "Aube"sv},
    {Dep::Aude,                     {11},          "Aude"sv},
    {Dep::Aveyron,                  {12},          "Aveyron"sv},
    {Dep::Bouches_du_Rhone,         {13},          "Bouches-du-Rhône"sv},
    {Dep::Calvados,                 {14},          "Calvados"sv},
    {Dep::Cantal,                   {15},          "Cantal"sv},
    {Dep::Charente,                 {16},          "Charente"sv},
    {Dep::Charente_Maritime,        {17},          "Charente-Maritime"sv},
    {Dep::Cher,                     {18},          "Cher"sv},
    {Dep::Correze,                  {19},          "Corrèze"sv},
    {Dep::Corse,                    {20},          "Corse"sv},
    {Dep::Cote_d_Or,                {21},          "Côte-d'Or"sv},
    {Dep::Cotes_d_Armor,            {22},          "Côtes-d'Armor"sv},
    {Dep::Creuse,                   {23},          "Creuse"sv},
    {Dep::Dordogne,                 {24},          "Dordogne"sv},
    {Dep::Doubs,                    {25},          "Doubs"sv},
    {Dep::Drome,                    {26},          "Drôme"sv},
    {Dep::Eure,                     {27},          "Eure"sv},
    {Dep::Eure_et_Loir,             {28},          "Eure-et-Loir"sv},
    {Dep::Finistere,                {29},          "Finistère"sv},
    {Dep::Gard,                     {30},          "Gard"sv},
    {Dep::Haute_Garonne,            {31},          "Haute-Garonne"sv},
    {Dep::Gers,                     {32},          "Gers"sv},
    {Dep::Gironde,                  {33},          "Gironde"sv},
    {Dep::Herault,                  {34},          "Hérault"sv},
    {Dep::Ille_et_Vilaine,          {35},          "Ille-et-Vilaine"sv},
    {Dep::Indre,                    {36},          "Indre"sv},
    {Dep::Indre_et_Loire,           {37},          "Indre-et-Loire"sv},
    {Dep::Isere,                    {38},          "Isère"sv},
    {Dep::Jura,                     {39},          "Jura"sv},
    {Dep::Landes,                   {40},          "Landes"sv},
    {Dep::Loir_et_Cher,             {41},          "Loir-et-Cher"sv},
    {Dep::Loire,                    {42},          "Loire"sv},
    {Dep::Haute_Loire,              {43},          "Haute-Loire"sv},
    {Dep::Loire_Altantique,         {44},          "Loire-Altantique"sv},
    {Dep::Loiret,                   {45},          "Loiret"sv},
    {Dep::Lot,                      {46},          "Lot"sv},
    {Dep::Lot_et_Garonne,           {47},          "Lot-et-Garonne"sv},
    {Dep::Lozere,                   {48},          "Lozère"sv},
    {Dep::Maine_et_Loire,           {49},          "Maine-et-Loire"sv},
    {Dep::Manche,                   {50},          "Manche"sv},
    {Dep::Marne,                    {51},          "Marne"sv},
    {Dep::Haute_Marne,              {52},          "Haute-Marne"sv},
    {Dep::Mayenne,                  {53},          "Mayenne"sv},
    {Dep::Meurthe_et_Moselle,       {54},          "Meurthe-et-Moselle"sv},
    {Dep::Meuse,                    {55},          "Meuse"sv},
    {Dep::Morbihan,                 {56},          "Morbihan"sv},
    {Dep::Moselle,                  {57},          "Moselle"sv},
    {Dep::Nievre,                   {58},          "Nièvre"sv},
    {Dep::Nord,                     {59},          "Nord"sv},
    {Dep::Oise,                     {60},          "Oise"sv},
    {Dep::Orne,                     {61},          "Orne"sv},
    {Dep::Pas_de_Calais,            {62},          "Pas-de-Calais"sv},
    {Dep::Puy_de_Dome,              {63},          "Puy-de-Dôme"sv},
    {Dep::Pyrenees_Atlantiques,     {64},          "Pyrénées-Atlantiques"sv},
    {Dep::Hautes_Pyrenees,          {65},          "Hautes-Pyrénées"sv},
    {Dep::Pyrenees_Orientales,      {66},          "Pyrénées-Orientales"sv},
    {Dep::Bas_Rhin,                 {67},          "Bas-Rhin"sv},
    {Dep::Haut_Rhin,                {68},          "Haut-Rhin"sv},
    {Dep::Rhone,                    {69},          "Rhône"sv},
    {Dep::Haute_Saone,              {70},          "Haute-Saône"sv},
    {Dep::Saone_et_Loire,           {71},          "Saône-et-Loire"sv},
    {Dep::Sarthe,                   {72},          "Sarthe"sv},
    {Dep::Savoie,                   {73},          "Savoie"sv},
    {Dep::Haute_Savoie,             {74},          "Haute-Savoie"sv},
    {Dep::Paris,                    {75},          "Paris"sv},
    {Dep::Seine_Maritime,           {76},          "Seine-Maritime"sv},
    {Dep::Seine_et_Marne,           {77},          "Seine-et-Marne"sv},
    {Dep::Yvelines,                 {78},          "Yvelines"sv},
    {Dep::Deux_Sevres,              {79},          "Deux-Sèvres"sv},
    {Dep::Somme,                    {80},          "Somme"sv},
    {Dep::Tarn,                     {81},          "Tarn"sv},
    {Dep::Tarn_et_Garonne,          {82},          "Tarn-et-Garonne"sv},
    {Dep::Var,                      {83},          "Var"sv},
    {Dep::Vaucluse,                 {84},          "Vaucluse"sv},
    {Dep::Vendee,                   {85},          "Vendée"sv},
    {Dep::Vienne,                   {86},          "Vienne"sv},
    {Dep::Haute_Vienne,             {87},          "Haute-Vienne"sv},
    {Dep::Vosges,                   {88},          "Vosges"sv},
    {Dep::Yonne,                    {89},          "Yonne"sv},
    {Dep::Territoire_de_Belfort,    {90},          "Territoire-de-Belfort"sv},
    {Dep::Essone,                   {91},          "Essone"sv},
    {Dep::Haut_de_Seine,            {92},          "Haut-de-Seine"sv},
    {Dep::Seine_Saint_Denis,        {93},          "Seine-Saint-Denis"sv},
    {Dep::Val_de_Marne,             {94},          "Val-de-Marne"sv},
    {Dep::Val_d_Oise,               {95},          "Val-d'Oise"sv},
    {Dep::Guadeloupe,               {971},         "Guadeloupe"sv},
    {Dep::Martinique,               {972},         "Martinique"sv},
    {Dep::Guyane,                   {973},         "Guyane"sv},
    {Dep::Reunion,                  {974},         "Réunion"sv},
    {Dep::Inconnu,                  {0},           "Inconnu"sv},
}};

[[maybe_unused]] constexpr static std::optional<Department> get_department(DepartmentId dep) {
    return departments.optional_at<1,0>(dep);
}

[[maybe_unused]] constexpr static DepartmentId get_id(Department dep){
    return departments.at<0,1>(dep);
}

[[maybe_unused]] constexpr static DepName get_name(Department dep){
    return departments.at<0,2>(dep);
}

struct DepartmentColor{
    QString name;
    QString color;
};

//struct DepartmentInfo{
//    DepartmentC department;
//    Count count;
//    Rank rank;
//    Popularity popularity;
//    Percentage percentage;
//};



}
