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
	BaseMessage() : msgId(MessageId::NIL) {}
	virtual ~BaseMessage() = default;

	BaseMessage(MessageId id) : msgId(id) {}
	inline MessageId GetId() const noexcept { return msgId; }
	virtual void Process() const = 0;
};

typedef std::shared_ptr<BaseMessage> messagePtr;
typedef void (*MESSAGE_HANDLER)(messagePtr);
