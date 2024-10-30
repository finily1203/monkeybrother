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
