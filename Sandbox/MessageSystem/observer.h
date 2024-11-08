/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lee Jing Wen (jingwen.lee)
@team   :  MonkeHood
@course :  CSD2401
@file   :  observer.h
@brief  :  observer.h contains the Observer class, which allows objects to register, retrieve and
		   unregister message handlers for different message types. Each observer instance can handle
		   multiple handlers, identified by the MessageId, which define the action taken when a specific
		   message is received.

*Lee Jing Wen (jingwen.lee) :
		- Creates and implements the Observer class, which includes the handler attachment, retrieval and
		  unregistration mechanisms.

File Contributions: Lee Jing Wen (100%)

*//*__________________________________________________________________________________________________*/
#pragma once

#include <unordered_map>
#include <string>
#include "baseMessageSystem.h"


class Observer
{
private:
	std::unordered_map<MessageId, MESSAGE_HANDLER> msgHandlers;
	std::string id;

public:
	Observer() : id("No_Id") {}
	explicit Observer(std::string const& obsId) : id(obsId) {}
	~Observer() {}

	void AttachHandler(MessageId msgId, MESSAGE_HANDLER msgHandler);
	MESSAGE_HANDLER GetHandler(MessageId msgId) const;
	void UnregisterHandler(MessageId msgId);
	std::string GetId() const noexcept;
};