#include "observer.h"

void Observer::AttachHandler(MessageId msgId, MESSAGE_HANDLER handler)
{
    msgHandlers[msgId] = handler;
}

MESSAGE_HANDLER Observer::GetHandler(MessageId msgId) const
{
    auto it = msgHandlers.find(msgId);

    return (it != msgHandlers.end()) ? it->second : nullptr;
}

void Observer::UnregisterHandler(MessageId msgId)
{
    msgHandlers.erase(msgId);
}

std::string Observer::GetId() const noexcept
{
    return id;
}
