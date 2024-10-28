#pragma once

#include "observer.h"
#include "baseMessageSystem.h"
#include <iostream>


class Observable
{
private:
	typedef std::multimap<MessageId, Observer*> messagesMap;
	messagesMap msgsMap;
	std::string id;

public:
	Observable() : id("No_Id") {}
	Observable(std::string const& msgId) : id(msgId) {}
	~Observable() {}

	void Register(MessageId id, Observer* observer);
	void ProcessMessage(BaseMessage* message);
	std::string GetId() const noexcept;
};
