#pragma once

#include "LeapCpp.h"

#include <functional>


// Listener which you initialise with a callback
//
// The need for this might go away once I properly structure the app
class CallbackListener : public lpp::Connection::Listener {
public:
    using Callback = std::function<void(LEAP_CONNECTION_MESSAGE)>;

    explicit CallbackListener(Callback callback) :
            callback_(std::move(callback)) {
    }

    void on_event(LEAP_CONNECTION_MESSAGE event) override {
        callback_(event);
    }

private:
    const Callback callback_;
};
