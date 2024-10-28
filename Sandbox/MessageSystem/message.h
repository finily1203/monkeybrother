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
};
