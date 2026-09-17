#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

class RuntimeError : public std::runtime_error
{
public:
    explicit RuntimeError(std::string_view description)
    : std::runtime_error(std::string(description))
    {
    }
};

class FileOpenError : public std::runtime_error
{
public:
    explicit FileOpenError(std::string_view path)
    : std::runtime_error("cannot open file: " + std::string(path))
    {
    }
};

class InvalidExchange : public std::invalid_argument
{
public:
    explicit InvalidExchange(std::string_view exchange)
    : std::invalid_argument("invalid exchange: " + std::string(exchange))
    {
    }

    explicit InvalidExchange(int exchange)
    : std::invalid_argument("invalid exchange: " + std::to_string(exchange))
    {
    }
};

class InvalidLogLevel : public std::invalid_argument
{
public:
    explicit InvalidLogLevel(int level)
    : std::invalid_argument("invalid log level: " + std::to_string(level))
    {
    }
};

class InvalidSide : public std::invalid_argument
{
public:
    explicit InvalidSide(std::string_view side)
    : std::invalid_argument("invalid side: " + std::string(side))
    {
    }
};
