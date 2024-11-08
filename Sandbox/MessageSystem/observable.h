/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lee Jing Wen (jingwen.lee)
@team   :  MonkeHood
@course :  CSD2401
@file   :  observable.h
@brief  :  observable.h contains the PlayerEventPublisher and PlayerActionListener classes. PlayerEventPublisher
		   allows entities to register and unregister observers for specific player-related events like
		   falling, jumping and collision, and process messages to notify observers of actions.
		   PlayerActionListener is a specialized Observer that listens for playerActions.

*Lee Jing Wen (jingwen.lee) :
		- Creates and implements the PlayerEventPublisher class, which includes observer registration,
		  message processing and notification functions for player events. Creates and implements the
		  PlayerActionListener class to serve as a specific observer type for player actions.

File Contributions: Lee Jing Wen (100%)

*//*__________________________________________________________________________________________________*/
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