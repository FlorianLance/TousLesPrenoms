
#include "convert.hpp"

using namespace tool;


FirstName Convert::to_first_name(RawFirstNameV rawFirstName){

    FirstName firstName;
    firstName.v.append(rawFirstName.v);
    firstName.v = firstName.v.toLower();
    firstName.v[0] = rawFirstName.v[0];

    const int id = rawFirstName.v.indexOf(fnSep);
    if(id != -1){
        if(id < rawFirstName.v.size()-1){
            firstName.v[id+1] = rawFirstName.v[id+1].toUpper();
        }
    }

    return firstName;
}

Year Convert::to_year(QStringView yearStrV){
    if(yearStrV[0] == invalid){
        return {-1};
    }
    return {static_cast<std::int16_t>(yearStrV.toInt())};
}

Gender Convert::to_sex(QStringView sexStrV){
    if(sexStrV[0] == maleC){
        return Gender::Male;
    }else if(sexStrV[0] == femaleC){
        return Gender::Female;
    }
    return Gender::Other;
}

Department Convert::to_department(QStringView depStrV){
    if(depStrV[0] == invalid){
        return Department::Inconnu;
    }
    return get_department(DepartmentId{depStrV.toInt()}).value();
}

QString Convert::to_str(const QColor &c){
    return QString::number(c.red()) % QSL(" ") % QString::number(c.green()) + QSL(" ") % QString::number(c.blue());
}

QColor Convert::to_color(QString s){
    auto split = s.splitRef(" ");
    return QColor(split[0].toInt(),split[1].toInt(),split[2].toInt());
}

QColor Convert::factor_to_colormap_value(double factor){

    if(factor <= 0.25){
        return QColor(0,static_cast<int>((factor*4)*255),255);
    }else if(factor <= 0.50){
        return QColor(0,255,255-static_cast<int>(((factor-0.25)*4)*255));
    }else if(factor <= 0.75){
        return QColor(static_cast<int>(((factor-0.5)*4)*255),255,0);
    }else{
        return QColor(255,255-static_cast<int>(((factor-0.75)*4)*255),0);
    }
}
