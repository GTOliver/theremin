#pragma once

#include <optional>

#include "readerwriterqueue.h"


namespace mc = moodycamel;

#include "ThereMessage.h"

/**
 * Loose wrapper around a lock-free queue
 *
 * Has a minimal API
 */
class MessageQueue
{
public:
    MessageQueue();

    void send(ThereMessage message);

    bool has_next();

    ThereMessage next();

private:
    mc::ReaderWriterQueue<ThereMessage> queue_;

};
