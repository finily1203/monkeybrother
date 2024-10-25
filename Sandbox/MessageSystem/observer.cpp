#include "observer.h"

void Observer::AttachHandler(MessageId id, MESSAGE_HANDLER handler)
{
    msgHandlers.insert(std::pair<MessageId, MESSAGE_HANDLER>(id, handler));
}

MESSAGE_HANDLER Observer::GetHandler(MessageId id) const
{
    auto it = msgHandlers.find(id);

    return (it != msgHandlers.end()) ? it->second : nullptr;
}

std::string Observer::GetId() const noexcept
{
    return id;
}
