/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lee Jing Wen (jingwen.lee)
@team   :  MonkeHood
@course :  CSD2401
@file   :  observable.cpp
@brief  :  observable.cpp contains the definition of all the member functions of PlayerEventPublisher and
           PlayerActionListener class. PlayerEventPublisher helps to process message based on the type of
           message that is differentiated by message Id. PlayerActionListener will attach handler based
           on the action that is happening.

*Lee Jing Wen (jingwen.lee) :
        - Creates and define all the member functions of PlayerEventPublisher class and PlayerActionListener
          class.

File Contributions: Lee Jing Wen (100%)

*//*__________________________________________________________________________________________________*/
#include "observable.h"

// inserting a key and pair to the messagesMap class, the key being the message Id
// and the pair to be the pointer to an Observer object
void PlayerEventPublisher::Register(MessageId msgId, std::shared_ptr<Observer> observer)
{
    messagesMap.insert({ msgId, observer });
}

// remove the key and pair of messages from the messagesMap
void PlayerEventPublisher::Unregister(MessageId msgId, std::shared_ptr<Observer> observer)
{
    auto it = messagesMap.find(msgId);
    if (it != messagesMap.end() && it->second == observer)
    {
        messagesMap.erase(it);
        observer->UnregisterHandler(msgId);
    }
}

// process the messages based on the type of action
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

// returns the id of the message
std::string PlayerEventPublisher::GetId() const noexcept
{
    return id;
}

// fall action happened
void PlayerEventPublisher::NotifyFall(Entity player)
{
    auto fallMsg = std::make_shared<FallMessage>(player);
    ProcessMessage(fallMsg);
}

// jump action happened
void PlayerEventPublisher::NotifyJump(Entity player)
{
    auto jmpMsg = std::make_shared<JumpMessage>(player);
    ProcessMessage(jmpMsg);
}

// listener for the different kinds of actions
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