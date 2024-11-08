/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lee Jing Wen (jingwen.lee)
@team   :  MonkeHood
@course :  CSD2401
@file   :  observer.cpp
@brief  :  observer.cpp contains the definition of the member functions declared in the Observer class.
           AttachHandler function initializes the message handler and message Id pair together in the
           message handlers unordered map. GetHandler will retrieve the message handler based on the
           message Id. UnregisterHandler function will erase the pair from the unordered map based message Id.
           GetId function returns the observer message id.

*Lee Jing Wen (jingwen.lee) :
        - Creates the definition for all member functions of the Observer class, which includes the
          handler attachment, retrieval, unregistration and get Id functions.

File Contributions: Lee Jing Wen (100%)

*//*__________________________________________________________________________________________________*/
#include "observer.h"

// this function will initialize the msgHandlers unordered map with the pair 
// msgId and the function pointer to a particular handler
void Observer::AttachHandler(MessageId msgId, MESSAGE_HANDLER handler)
{
    msgHandlers[msgId] = handler;
}

// returns the handler
MESSAGE_HANDLER Observer::GetHandler(MessageId msgId) const
{
    auto it = msgHandlers.find(msgId);

    return (it != msgHandlers.end()) ? it->second : nullptr;
}

// remove the key pair object from the msgHandlers unordered map
void Observer::UnregisterHandler(MessageId msgId)
{
    msgHandlers.erase(msgId);
}

// returns the id of the observer message
std::string Observer::GetId() const noexcept
{
    return id;
}