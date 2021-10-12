
#pragma once

// Qt
#include <QStringList>

// local
#include "data/data.hpp"
#include "data/settings.hpp"

namespace tool {

struct Filters{

    // separators
    static constexpr QChar sepWords = ';';

    void apply(const FilterSettings &s, Data &data);

private:

    bool is_filtered(const FilterSettings &s, FirstNameV firstNameV, Data &data) const;

    bool filter_gender(const FilterSettings &s, const NameProcessedInfos &nameInfo) const;

    bool filter_letters(const FilterSettings &s, FirstNameV firstNameV) const;
    bool filter_nb_letters(const FilterSettings &s, FirstNameV firstNameV) const;
    bool filter_contains(const FilterSettings &s, FirstNameV firstNameV) const;
    bool filter_do_not_contains(const FilterSettings &s, FirstNameV firstNameV) const;
    bool filter_starts_by(const FilterSettings &s, FirstNameV firstNameV) const;
    bool filter_ends_by(const FilterSettings &s, FirstNameV firstNameV) const;

    bool filter_years(const FilterSettings &s, const NameProcessedInfos &nameInfo) const;
    bool filter_appears_before(const FilterSettings &s, const Interval &apparitionInterval) const;
    bool filter_appears_during(const FilterSettings &s, const Interval &apparitionInterval) const;
    bool filter_appears_after(const FilterSettings &s, const Interval &apparitionInterval) const;
    bool filter_appears_unknow(const FilterSettings &s, const Interval &apparitionInterval) const;
    bool filter_peak_before(const FilterSettings &s, const Year &popularityPeak) const;
    bool filter_peak_during(const FilterSettings &s, const Year &popularityPeak) const;
    bool filter_peak_after(const FilterSettings &s, const Year &popularityPeak) const;
    bool filter_peak_unknow(const FilterSettings &s, const Year &popularityPeak) const;

    bool filter_popularity_period(const FilterSettings &s, const NameProcessedInfos &nameInfo) const;

    bool filter_popularity_department(const FilterSettings &s, FirstNameV firstNameV, const um<Department, DepartmentProcessedInfos> &infoPerDepartment) const;


    std::vector<Department> insideDepartments;
};
}
