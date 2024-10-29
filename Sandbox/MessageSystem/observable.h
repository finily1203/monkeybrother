#pragma once

#include "observer.h"
#include "baseMessageSystem.h"
#include "EntityManager.h"
#include <iostream>


class Observable
{
private:
	std::unordered_map<MessageId, std::shared_ptr<Observer>> messagesMap;
	std::string id;

public:
	Observable() : id("No_Id") {}
	Observable(std::string const& obsId) : id(obsId) {}
	~Observable() {}

	void Register(MessageId id, std::shared_ptr<Observer> observer);
	void Unregister(MessageId id, std::shared_ptr<Observer> observer);
	void ProcessMessage(messagePtr message);
	std::string GetId() const noexcept;
};
