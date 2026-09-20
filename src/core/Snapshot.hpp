#pragma once

#include "core/Book.hpp"

#include <string>

struct Snapshot
{
    std::string symbol;
    Book book;
};
