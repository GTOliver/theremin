#include "MessageQueue.h"

MessageQueue::MessageQueue()
        : queue_{10}
{
}

void MessageQueue::send(ThereMessage message)
{
    queue_.try_emplace(message);
}

bool MessageQueue::has_next()
{
    return queue_.peek() != nullptr;
}

ThereMessage MessageQueue::next()
{
    ThereMessage msg{};
    queue_.try_dequeue(msg);
    return msg;
}
