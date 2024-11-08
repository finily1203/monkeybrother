/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lee Jing Wen (jingwen.lee)
@team   :  MonkeHood
@course :  CSD2401
@file   :  message.h
@brief  :  message.h contains the different types of messages class, here we have fall, jump and collide
   messages type. These classes handle different kinds of actions, example when player entity is
   falling, the fall message will be sent out. All the different messages class overloads the
   process message function from BaseMessage class so that each will print its own set of messages.

*Lee Jing Wen (jingwen.lee) :
- Creates three different types of actions message class, FallMessage, JumpMessage and CollisionMessage.
  Each class handles its own type of actions by processing messages according to what type of action
  occured.

File Contributions: Lee Jing Wen (100%)

*//*__________________________________________________________________________________________________*/
#pragma once


#include "baseMessageSystem.h"
#include "ECSDefinitions.h"
#include "GUIConsole.h"

// fall message class
class FallMessage : public BaseMessage
{
private:
	Entity playerEntity;

public:
	// constructor for FallMessage
	FallMessage(Entity& player) : BaseMessage(MessageId::FALL), playerEntity(player) {}

	// destructor
	~FallMessage() {}

	// return the player entity
	inline Entity GetPlayer() const noexcept { return playerEntity; }

	// process the message for falling action
	void Process() const override
	{
		Console::GetLog() << "Processing fall message for entity: " << playerEntity << std::endl;
	}
};

// jump message class
class JumpMessage : public BaseMessage
{
private:
	Entity playerEntity;

public:
	// constructor for JumpMessage
	JumpMessage(Entity& player) : BaseMessage(MessageId::JUMP), playerEntity(player) {}

	// destructor
	~JumpMessage() {}

	// return the player entity
	inline Entity GetPlayer() const noexcept { return playerEntity; }

	// process the message for jumping action
	void Process() const override
	{
		Console::GetLog() << "Processing jump message for entity: " << playerEntity << std::endl;
	}
};

// CollisionMessage class
class CollisionMessage : public BaseMessage
{
private:
	Entity playerEntity;
	Entity platformEntity;

public:
	// constructor for CollisionMessage
	CollisionMessage(Entity& player, Entity platform) : BaseMessage(MessageId::COLLISION), playerEntity(player), platformEntity(platform) {}

	// destructor
	~CollisionMessage() {}

	// return player entity
	inline Entity GetPlayer() const noexcept { return playerEntity; }

	// return the platform entity
	inline Entity GetPlatform() const noexcept { return platformEntity; }

	// process message for collision action
	void Process() const override
	{
		Console::GetLog() << "Processing collision message for entity " << playerEntity << std::endl;
	}
};