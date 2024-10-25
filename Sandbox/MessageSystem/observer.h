#pragma once

#include <map>
#include <string>
#include "baseMessageSystem.h"


class Observer
{
private:
	std::map<MessageId, MESSAGE_HANDLER> msgHandlers;
	std::string id;

public:
	Observer() : id("No_Id") {}
	Observer(std::string const& msgId) : id(msgId) {}
	~Observer() {}

	void AttachHandler(MessageId id, MESSAGE_HANDLER msgHandler);
	MESSAGE_HANDLER GetHandler(MessageId id) const;
	std::string GetId() const noexcept;
};
