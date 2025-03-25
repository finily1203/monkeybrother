/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   MovPlatformBehaviour.h
@brief:  This header file includes the declaration of the Moving platform
		 behaviour class used by the LogicSystemECS to handle the behaviour of
		 the moving platforms. Though it is not used in the current game, it is
		 additional feature that can be used for the levels

		 Joel Chu (c.weiyuan): declared the functions of MovPlatformBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#include "LogicSystemECS.h"

class MovPlatformBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
};