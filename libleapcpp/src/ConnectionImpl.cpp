#include "ConnectionImpl.h"
#include "Exception.h"

namespace lpp
{

Connection::impl::impl()
        : config_{sizeof(LEAP_CONNECTION_CONFIG), 0, nullptr},
          connection_{},
          polling_thread_{nullptr},
          stop_flag_()
{
    const auto result = LeapCreateConnection(&config_, &connection_);
    success_or_throw(result);
}

Connection::impl::~impl()
{
    LeapDestroyConnection(connection_);
}

void Connection::impl::open()
{
    if (polling_thread_) {
        throw std::runtime_error("Connection already open");
    }
    auto result = LeapOpenConnection(connection_);
    success_or_throw(result);
    stop_flag_.test_and_set();
    polling_thread_ = std::make_unique<std::thread>([this]() { poll_loop(); });
}

void Connection::impl::close()
{
    if (!polling_thread_) {
        // Just return if the connection is not open
        return;
    }
    stop_flag_.clear();
    if (polling_thread_->joinable()) {
        polling_thread_->join();
    }
    polling_thread_.reset();
    LeapCloseConnection(connection_);
}

void Connection::impl::add_listener(ListenerPtr listener)
{
    std::scoped_lock lock(listeners_mtx_);
    listeners_.push_back(std::move(listener));
}

void Connection::impl::remove_listener(const ListenerPtr& listener)
{
    std::scoped_lock lock(listeners_mtx_);
    const auto new_end = std::remove(listeners_.begin(), listeners_.end(), listener);
    listeners_.erase(new_end, listeners_.end());
}

void Connection::impl::poll_loop()
{
    LEAP_CONNECTION_MESSAGE event;
    while (stop_flag_.test_and_set()) {
        auto result = LeapPollConnection(connection_, 1000, &event);
        {
            std::scoped_lock lock(listeners_mtx_);
            if (result == eLeapRS_Success) {
                for (auto& listener: listeners_) {
                    listener->on_event(event);
                }
            } else {
                for (auto& listener: listeners_) {
                    listener->on_error(result);
                }
            }
        } // lock
    }
}

} // namespace lpp
