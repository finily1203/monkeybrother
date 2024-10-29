#include "observable.h"

void Observable::Register(MessageId id, std::shared_ptr<Observer> observer)
{
    messagesMap.insert({ id, observer });
}

void Observable::Unregister(MessageId id, std::shared_ptr<Observer> observer)
{
    auto range = messagesMap.equal_range(id);
    for (auto it = range.first; it != range.second; ++it)
    {
        if (it->second == observer)
        {
            messagesMap.erase(it);
            break;
        }
    }
}

void Observable::ProcessMessage(messagePtr message)
{
    auto range = messagesMap.equal_range(message->GetId());

    for (auto it = range.first; it != range.second; ++it)
    {
        auto observer = it->second;
        auto handler = observer->GetHandler(message->GetId());

        if (handler)
        {
            handler(message);
        }
    }
}

std::string Observable::GetId() const noexcept
{
    return id;
}
