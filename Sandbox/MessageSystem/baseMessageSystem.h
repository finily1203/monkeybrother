/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lee Jing Wen (jingwen.lee)
@team   :  MonkeHood
@course :  CSD2401
@file   :  baseMessageSystem.h
@brief  :  baseMessageSystem.h contains the class BaseMessage and an enum class MessageId. The BaseMessage
		   BaseMessage class is an abstract base class for creating and handling messages, which represent
		   different types of actions or events.

*Lee Jing Wen (jingwen.lee) :
		- Creates the BaseMessage class, which includes the interface for constructing and processing
		  messages.

File Contributions: Lee Jing Wen (100%)

*//*__________________________________________________________________________________________________*/
#pragma once

#include <memory>
#include <functional>

enum class MessageId
{
	NIL = 0,
	FALL,
	JUMP,
	COLLISION
};

class BaseMessage
{
private:
	MessageId msgId;

public:
	// constructor
	BaseMessage() : msgId(MessageId::NIL) {}

	// destructor
	virtual ~BaseMessage() = default;

	// constructor that constructs a specific type of message
	BaseMessage(MessageId id) : msgId(id) {}

	// returns id of the message
	inline MessageId GetId() const noexcept { return msgId; }

	// process for abstract class BaseMessage
	virtual void Process() const = 0;
};

typedef std::shared_ptr<BaseMessage> messagePtr;
typedef void (*MESSAGE_HANDLER)(messagePtr);