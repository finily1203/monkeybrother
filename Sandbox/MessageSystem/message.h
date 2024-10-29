#pragma once


#include "baseMessageSystem.h"
#include "ECSDefinitions.h"

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
		std::cout << "Processing fall message for entity: " << playerEntity << std::endl;
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
		std::cout << "Processing jump message for entity: " << playerEntity << std::endl;
	}
};
