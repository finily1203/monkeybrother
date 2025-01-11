/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   PlatformBehaviour.h
@brief:  This header file includes the PlatformBehaviour class which is used
		 by the logicSystemECS to handle the behaviour of the platform entity.
		 Joel Chu (c.weiyuan): declared the PlatformBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#include "LogicSystemECS.h"

class PlatformBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
};