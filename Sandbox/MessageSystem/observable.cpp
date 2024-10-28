#include "observable.h"

void Observable::Register(MessageId id, Observer* observer)
{
    msgsMap.insert(messagesMap::value_type(id, observer));
}

void Observable::ProcessMessage(BaseMessage* message)
{
    std::cout << "\nOBSERVABLE: " << id << " is sending its messages..." << std::endl;
    auto range = msgsMap.equal_range(message->GetId());

    for (auto it = range.first; it != range.second; ++it)
    {
        Observer* observer = it->second;
        auto handler = observer->GetHandler(message->GetId());

        if (handler)
        {
            std::cout << "Message processed by Observer: " << observer->GetId() << std::endl;
            handler(message);
        }
    }
}

std::string Observable::GetId() const noexcept
{
    return id;
}
