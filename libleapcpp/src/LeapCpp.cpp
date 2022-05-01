#include "LeapCpp.h"

#include "LeapC.h"

#include "ConnectionImpl.h"
#include "Exception.h"

namespace lpp
{

Timestamp get_now()
{
    return LeapGetNow();
}

const char* get_error_message(eLeapRS result)
{
    return get_error_message_impl(result);
}

Connection::Connection()
        : impl_(std::make_unique<impl>())
{
}

Connection::~Connection() = default;

void Connection::open()
{
    impl_->open();
}

void Connection::close()
{
    impl_->close();
}

void Connection::add_listener(ListenerPtr listener)
{
    impl_->add_listener(std::move(listener));
}

void Connection::remove_listener(const ListenerPtr& listener)
{
    impl_->remove_listener(listener);
}

} // namespace lpp
