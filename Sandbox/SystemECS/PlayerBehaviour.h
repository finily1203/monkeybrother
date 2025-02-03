/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   PlayerBehaviour.h
@brief:  This header file includes the PlayerBehaviour class which is used
		 by the logicSystemECS to handle the behaviour of the player entity.
		 Joel Chu (c.weiyuan): declared the PlayerBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#include "LogicSystemECS.h"

class PlayerBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;

	double lastMouseX = 0;
	double lastMouseY = 0;
	bool isFirstFrame = true; // to handle the first frame of the game
	static float MOVEMENT_THRESHOLD;
	static float BASE_ROTATION_SPEED;
	static float MAX_ROTATION_PER_FRAME;
};