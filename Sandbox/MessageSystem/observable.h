#pragma once

#include "observer.h"
#include "baseMessageSystem.h"
#include "EntityManager.h"
#include "message.h"
#include <iostream>


class PlayerEventPublisher
{
private:
	std::unordered_map<MessageId, std::shared_ptr<Observer>> messagesMap;
	std::string id;

public:
	PlayerEventPublisher() : id("No_Id") {}
	PlayerEventPublisher(std::string const& obsId) : id(obsId) {}
	~PlayerEventPublisher() {}

	void Register(MessageId msgId, std::shared_ptr<Observer> observer);
	void Unregister(MessageId msgId, std::shared_ptr<Observer> observer);
	void ProcessMessage(messagePtr message);
	std::string GetId() const noexcept;

	void NotifyFall(Entity player);
	void NotifyJump(Entity player);
};

class PlayerActionListener : public Observer
{
public:
	PlayerActionListener();
	~PlayerActionListener() {}
};
