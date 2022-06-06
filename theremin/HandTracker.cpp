#include "HandTracker.h"

#include <array>
#include <numeric>

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
    const auto& palm_data = hand.palm.position;

    Vector3 palm_position = {palm_data.x, palm_data.y, palm_data.z};

    const auto get_tracked_joint = [](const auto digit) { return digit.distal.next_joint; };

    constexpr size_t N = 3;
    std::array<LEAP_VECTOR, N> finger_positions{
            get_tracked_joint(hand.index),
            get_tracked_joint(hand.middle),
            get_tracked_joint(hand.ring)};

    LEAP_VECTOR finger_pos_sum = std::accumulate(finger_positions.begin(), finger_positions.end(), LEAP_VECTOR{0.0, 0.0, 0.0},
                                                 [](const auto& v1, const auto& v2) -> LEAP_VECTOR { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; });
    Vector3 average_finger_pos = {
            finger_pos_sum.x / static_cast<double>(N),
            finger_pos_sum.y / static_cast<double>(N),
            finger_pos_sum.z / static_cast<double>(N)};

    HandData data{
            palm_position,
            average_finger_pos};

    return data;
}