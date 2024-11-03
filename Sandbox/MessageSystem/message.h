#pragma once


#include "baseMessageSystem.h"
#include "ECSDefinitions.h"
#include "GUIConsole.h"

class FallMessage: public BaseMessage
{
private:
	Entity playerEntity;

public:
	FallMessage(Entity& player) : BaseMessage(MessageId::FALL), playerEntity(player) {}
	~FallMessage() {}

	inline Entity GetPlayer() const noexcept { return playerEntity; }
	void Process() const override
	{
		Console::GetLog() << "Processing fall message for entity: " << playerEntity << std::endl;
	}
};


class JumpMessage : public BaseMessage
{
private:
	Entity playerEntity;

public:
	JumpMessage(Entity& player) : BaseMessage(MessageId::JUMP), playerEntity(player) {}
	~JumpMessage() {}

	inline Entity GetPlayer() const noexcept { return playerEntity; }
	void Process() const override
	{
		Console::GetLog() << "Processing jump message for entity: " << playerEntity << std::endl;
	}
};

class CollisionMessage : public BaseMessage
{
private:
	Entity playerEntity;
	Entity platformEntity;

public:
	CollisionMessage(Entity& player, Entity platform) : BaseMessage(MessageId::COLLISION), playerEntity(player), platformEntity(platform) {}
	~CollisionMessage() {}

	inline Entity GetPlayer() const noexcept { return playerEntity; }
	inline Entity GetPlatform() const noexcept { return platformEntity; }
	void Process() const override
	{
		Console::GetLog() << "Processing collision message for entity " << playerEntity << std::endl;
	}
};
