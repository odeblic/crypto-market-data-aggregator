#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <string>
#include <string_view>

#define LOG_DEBUG(MESSAGE)   Display::getInstance().debug(MESSAGE);
#define LOG_INFO(MESSAGE)    Display::getInstance().info(MESSAGE);
#define LOG_WARNING(MESSAGE) Display::getInstance().warning(MESSAGE);
#define LOG_ERROR(MESSAGE)   Display::getInstance().error(MESSAGE);

class Display
{
public:
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
    };

    static void instantiate(bool verbose, bool color)
    {
        getInstance(verbose, color);
    }

    static auto getInstance(bool verbose = false, bool color = true) -> Display&
    {
        static Display display{verbose, color};
        return display;
    }

    void debug(std::string_view const& message)
    {
        log(LogLevel::DEBUG, message);
    }

    void info(std::string_view const& message)
    {
        log(LogLevel::INFO, message);
    }

    void warning(std::string_view const& message)
    {
        log(LogLevel::WARNING, message);
    }

    void error(std::string_view const& message)
    {
        log(LogLevel::ERROR, message);
    }

    void show(std::string_view content)
    {
        print(content);
    }

private:
    Display(bool verbose, bool color)
    : out(std::cout), verbose(verbose), color(color)
    {
    }

    auto levelToString(LogLevel const level) -> std::string_view
    {
        if (color)
        {
            switch (level)
            {
                case LogLevel::DEBUG:   return "\033[34m DEBUG   \033[0m";
                case LogLevel::INFO:    return "\033[32m INFO    \033[0m";
                case LogLevel::WARNING: return "\033[33m WARNING \033[0m";
                case LogLevel::ERROR:   return "\033[31m ERROR   \033[0m";
                default: throw std::runtime_error("invalid log level");
            }
        }
        else
        {
            switch (level)
            {
                case LogLevel::DEBUG:   return " DEBUG   ";
                case LogLevel::INFO:    return " INFO    ";
                case LogLevel::WARNING: return " WARNING ";
                case LogLevel::ERROR:   return " ERROR   ";
                default: throw std::runtime_error("invalid log level");
            }
        }
    }

    auto getTimestamp() -> std::string
    {
        using namespace std::chrono;
        auto const now = system_clock::now();
        auto const ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        auto const timer = system_clock::to_time_t(now);
        auto bt = std::tm{};
        localtime_r(&timer, &bt);
        std::ostringstream buffer;

        if (color)
        {
            buffer << "\033[30;1m" << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
                   << '.' << std::setfill('0') << std::setw(3) << ms.count() << "\033[0m";
        }
        else
        {
            buffer << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
                   << '.' << std::setfill('0') << std::setw(3) << ms.count();
        }

        return buffer.str();
    }

    void log(LogLevel level, std::string_view message)
    {
        std::ostringstream buffer;
        buffer << getTimestamp() << ' ' << levelToString(level) << ' ' << message << '\n';
        print(buffer.str());
    }

    void print(std::string_view content)
    {
        std::scoped_lock lock(mutex);
        out << content;
    }

    std::ostream& out;
    bool verbose{false};
    bool color{true};
    std::mutex mutex;
};
