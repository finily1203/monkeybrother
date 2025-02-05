/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   CollectableBehaviour.h
@brief:  This header file includes the CollectableBehaviour class which is used
		 by the logicSystemECS to handle the behaviour of the collectable entities.
		 Joel Chu (c.weiyuan): declared the CollectableBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#include "LogicSystemECS.h"

class CollectableBehaviour : public BehaviourECS {
public:
    void update(Entity entity);
    void createCollectAnimation(Entity entity);
};