/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Liu YaoTing (yaoting.liu)
@team:   MonkeHood
@course: CSD2401
@file:   NavigationBehaviour.h
@brief:  This header file includes the implementation of the NavigationBehaviour

         Liu YaoTing (yaoting.liu): declare the functions of NavigationBehaviour update
                               100%
*//*___________________________________________________________________________-*/
#pragma once
#include "LogicSystemECS.h"

class NavigationBehaviour : public BehaviourECS {
public:
    void update(Entity entity);
};