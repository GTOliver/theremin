#pragma once

#include <functional>
#include <memory>
#include <mutex>

#include "LeapCpp.h"

#include "CallbackListener.h"
#include "TrackingFrame.h"

class HandTracker
{
public:
    HandTracker();
    ~HandTracker();

    using Callback = std::function<void(TrackingFrame)>;

    void set_callback(Callback callback);

    void clear_callback();

private:
    void tracking_callback(LEAP_CONNECTION_MESSAGE message);

    static TrackingFrame process_tracking_event(const LEAP_TRACKING_EVENT* event);
    static HandData process_hand(const LEAP_HAND& hand);

    Callback callback_;
    std::mutex callback_mtx_;

    lpp::Connection connection_;
    std::shared_ptr<CallbackListener> listener_;
};
