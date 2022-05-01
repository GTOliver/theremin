#pragma once

#include "LeapC.h"

#include <exception>
#include <memory>
#include <string>

#include <cstdint>

namespace lpp
{

using Timestamp = int64_t;

[[nodiscard]] Timestamp get_now();

[[nodiscard]] const char* get_error_message(eLeapRS error);

[[nodiscard]] static std::string get_error_string(eLeapRS error)
{
    return {get_error_message(error)};
}

class Exception: public std::exception
{
public:
    explicit Exception(eLeapRS result_code)
            : error_message_(get_error_message(result_code))
    {}

    [[nodiscard]] const char* what() const noexcept override
    {
        return error_message_;
    }

private:
    const char* error_message_;
};

class Connection
{
public:
    class Listener;

    Connection();

    ~Connection();

    Connection(const Connection& other) = delete;

    Connection& operator=(const Connection& other) = delete;

    void open();

    void close();

    void add_listener(std::shared_ptr<Listener> listener);

    void remove_listener(const std::shared_ptr<Listener>& listener);

    class Listener
    {
    public:
        virtual ~Listener() = default;

        virtual void on_event([[maybe_unused]] LEAP_CONNECTION_MESSAGE event){};

        virtual void on_error([[maybe_unused]] eLeapRS error_code){};
    };

private:
    class impl;

    std::unique_ptr<impl> impl_;
};

} // namespace lpp
