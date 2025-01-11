/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EffectPumpBehaviour.h
@brief:  This header file includes the EffectPumpBehaviour class which is used
		 by the logicSystemECS to handle the behaviour of the pump entities.
		 Joel Chu (c.weiyuan): declared the EffectPumpBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#include "LogicSystemECS.h"

class EffectPumpBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;

	EffectPumpBehaviour()
		: hasForceAdded(false), timer(0.0f), offDuration(5.0f), onDuration(5.0f) {}


private:
	bool hasForceAdded;
	float timer;
	float offDuration;
	float onDuration;
};