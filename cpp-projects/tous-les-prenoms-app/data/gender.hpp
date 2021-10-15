

#pragma once

namespace tool {

// others line types
enum class Gender : char{
    Female=0,
    Male,
    Other,
    SizeEnum
};

enum class GenderRepartition : char{
    OnlyFemale = 0,   // > 98 %
    OnlyMale,         // > 98 %
//    OnlyOther,        // > 98 %
    MostlyFemale,     // > 75 %
    MostlyMale,       // > 75 %
//    MostlyOther,      // > 75 %
    FemaleMale,       // [30-60]% [30-60]% [10]%
//    FemaleOther,      // [30-60]% [30-60]% [10]%
//    MaleOther,        // [30-60]% [30-60]% [10]%
//    FemaleMaleOther,  // [25-35]% [25-35]% [25-35]%
    Unknow,
    SizeEnum
};


static GenderRepartition compute_repartition(size_t countFemale, size_t countMale, size_t countOther, size_t total){

    const double ratioFemale = 1.0 * countFemale / total;
    const double ratioMale   = 1.0 * countMale / total;
    const double ratioOther  = 1.0 * countOther / total;

    if(ratioFemale > 0.98){
        return GenderRepartition::OnlyFemale;
    }else if(ratioMale > 0.98){
        return GenderRepartition::OnlyMale;
//    }else if(ratioOther > 0.98){
//        return GenderRepartition::OnlyOther;
    }else if(ratioFemale > 0.60){
        return GenderRepartition::MostlyFemale;
    }else if(ratioMale > 0.60){
        return GenderRepartition::MostlyMale;
//    }else if(ratioOther > 0.60){
//        return GenderRepartition::MostlyOther;
    }else if(ratioMale > 0.4 && ratioFemale > 0.4){
        return GenderRepartition::FemaleMale;
    }
//    }else if(ratioFemale > 0.4 && ratioOther > 0.4){
//        return GenderRepartition::FemaleOther;
//    }else if(ratioMale > 0.4 && ratioOther > 0.4){
//        return GenderRepartition::MaleOther;
//    }else if(ratioMale > 0.2 && ratioOther > 0.2 && ratioFemale > 0.2){
//        return GenderRepartition::FemaleMaleOther;
//    }

    return GenderRepartition::Unknow;
}

}
