#pragma once

#include "LeapC.h"

#include <memory>

#include <cstdint>

namespace lpp
{

using Timestamp = int64_t;

[[nodiscard]] Timestamp get_now();

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
