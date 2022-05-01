#include "HandTracker.h"

HandTracker::HandTracker()
        : connection_(),
          callback_mtx_{},
          listener_(std::make_unique<CallbackListener>([this](LEAP_CONNECTION_MESSAGE event) { tracking_callback(event); }))
{
    connection_.add_listener(listener_);
    connection_.open();
}

HandTracker::~HandTracker()
{
    connection_.close();
    connection_.remove_listener(listener_);
}

void HandTracker::set_callback(Callback callback)
{
    std::scoped_lock lock(callback_mtx_);
    callback_ = std::move(callback);
}

void HandTracker::clear_callback()
{
    std::scoped_lock lock(callback_mtx_);
    callback_ = [](TrackingFrame) {};
}

void HandTracker::tracking_callback(LEAP_CONNECTION_MESSAGE message)
{
    if (message.type == eLeapEventType_Tracking) {
        TrackingFrame frame = process_tracking_event(message.tracking_event);

        std::scoped_lock lock(callback_mtx_);
        callback_(frame);
    }
}

TrackingFrame HandTracker::process_tracking_event(const LEAP_TRACKING_EVENT* event)
{
    TrackingFrame frame;
    frame.timestamp = event->info.timestamp;

    for (unsigned int i = 0; i < event->nHands; ++i) {
        const auto& hand = event->pHands[i];
        auto& target = hand.type == eLeapHandType_Left ? frame.left : frame.right;
        target = process_hand(hand);
    }
    return frame;
}

HandData HandTracker::process_hand(const LEAP_HAND& hand)
{
    const auto& position = hand.palm.position;

    HandData data{};
    data.x = position.x;
    data.y = position.y;
    data.z = position.z;
    return data;
}