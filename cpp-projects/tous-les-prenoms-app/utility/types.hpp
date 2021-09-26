
#pragma once

// Qt
#include <QStringLiteral>
#include <QStringBuilder>

#define QSL QStringLiteral


namespace tool {

struct Rank{
    size_t v;
};

struct Percentage{
    float v;
};

struct Count{
    size_t v;
};

struct Order{
    std::uint16_t v;
};

}
