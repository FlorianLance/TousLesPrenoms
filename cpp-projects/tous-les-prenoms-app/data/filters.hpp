
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

    std::vector<Department> insideDepartments;
};
}
