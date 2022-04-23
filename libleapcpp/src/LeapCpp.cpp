#include "LeapCpp.h"

#include "LeapC.h"

#include "ConnectionImpl.h"

namespace lpp {

    long long get_now() {
        return LeapGetNow();
    }

    Connection::Connection()
            : impl_(std::make_unique<impl>()) {
    }

    Connection::~Connection() = default;

    void Connection::open() {
        impl_->open();
    }

    void Connection::close() {
        impl_->close();
    }

    void Connection::add_listener(ListenerPtr listener) {
        impl_->add_listener(std::move(listener));
    }

    void Connection::remove_listener(const ListenerPtr &listener) {
        impl_->remove_listener(listener);
    }


} // namespace lpp
