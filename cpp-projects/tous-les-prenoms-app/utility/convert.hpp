

#pragma once

// Qt
#include <QColor>

// local
#include "utility/types.hpp"
#include "data/first_name.hpp"
#include "data/year.hpp"
#include "data/gender.hpp"
#include "data/department.hpp"

namespace tool {

class Convert{

public:

    static constexpr QChar invalid = 'X';
    static constexpr QChar maleC   = '1';
    static constexpr QChar femaleC = '2';
    static constexpr QChar fnSep   = '-';
    static constexpr QChar lineSep;

    static FirstName to_first_name(RawFirstNameV rawFirstName);
    static Year to_year(QStringView yearStrV);
    static Gender to_sex(QStringView sexStrV);
    static Department to_department(QStringView depStrV);

    static QString to_str(const QColor &c);
    static QColor to_color(QString s);
    static QColor factor_to_colormap_value(double factor);        
};




}
