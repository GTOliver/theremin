#pragma once

#include "LeapC.h"
#include "LeapCpp.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

namespace lpp
{

using ListenerPtr = std::shared_ptr<Connection::Listener>;

class Connection::impl
{
public:
    impl();

    ~impl();

    impl(const impl& other) = delete;

    impl& operator=(const impl& other) = delete;

    void open();

    void close();

    void add_listener(ListenerPtr listener);

    void remove_listener(const ListenerPtr& listener);

    void poll_loop();

private:
    LEAP_CONNECTION_CONFIG config_;
    LEAP_CONNECTION connection_;

    std::vector<ListenerPtr> listeners_;
    std::mutex listeners_mtx_;

    std::unique_ptr<std::thread> polling_thread_;
    std::atomic_flag stop_flag_;
};

} // namespace lpp
