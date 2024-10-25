#pragma once


enum MessageId
{
	NIL = 0,
	FALL
};

class BaseMessage
{
private:
	MessageId msgId;

public:
	BaseMessage() : msgId(MessageId::NIL) {}
	virtual ~BaseMessage() {}

	BaseMessage(MessageId id) : msgId(id) {}
	inline MessageId GetId() const noexcept { return msgId; }
};

typedef void(*MESSAGE_HANDLER)(BaseMessage*);
