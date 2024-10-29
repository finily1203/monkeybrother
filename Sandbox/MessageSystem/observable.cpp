#include "observable.h"

void PlayerEventPublisher::Register(MessageId msgId, std::shared_ptr<Observer> observer)
{
    messagesMap.insert({ msgId, observer });
}

void PlayerEventPublisher::Unregister(MessageId msgId, std::shared_ptr<Observer> observer)
{
    auto it = messagesMap.find(msgId);
    if (it != messagesMap.end() && it->second == observer)
    {
        messagesMap.erase(it);
        observer->UnregisterHandler(msgId);
    }
}

void PlayerEventPublisher::ProcessMessage(messagePtr message)
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

std::string PlayerEventPublisher::GetId() const noexcept
{
    return id;
}

void PlayerEventPublisher::NotifyFall(Entity player)
{
    auto fallMsg = std::make_shared<FallMessage>(player);
    ProcessMessage(fallMsg);
}

void PlayerEventPublisher::NotifyJump(Entity player)
{
    auto jmpMsg = std::make_shared<JumpMessage>(player);
    ProcessMessage(jmpMsg);
}


PlayerActionListener::PlayerActionListener()
{
    AttachHandler(MessageId::FALL, [](messagePtr msg) {
        auto fallMsg = std::dynamic_pointer_cast<FallMessage>(msg);
        if (fallMsg)
        {
            fallMsg->Process();
            std::cout << "Entity " << fallMsg->GetPlayer() << " Player is falling" << std::endl;
        }
    });

    AttachHandler(MessageId::JUMP, [](messagePtr msg) {
        auto jmpMsg = std::dynamic_pointer_cast<JumpMessage>(msg);
        if (jmpMsg)
        {
            jmpMsg->Process();
            std::cout << "Entity " << jmpMsg->GetPlayer() << " Player is jumping" << std::endl;
        }
    });
}
