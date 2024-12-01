/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   ExitBehaviour.h
@brief:  This header file includes the ExitBehaviour class which is used
		 by the logicSystemECS to handle the behaviour of the exit entity.
		 Joel Chu (c.weiyuan): declared the ExitBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#include "LogicSystemECS.h"

class ExitBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
};
