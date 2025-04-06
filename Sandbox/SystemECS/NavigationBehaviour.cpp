/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Liu YaoTing (yaoting.liu)
@team:   MonkeHood
@course: CSD2401
@file:   NavigationBehaviour.cpp
@brief:  This source file includes the implementation of the NavigationBehaviour

         Liu YaoTing (yaoting.liu): defined the functions of NavigationBehaviour update
                               100%
*//*___________________________________________________________________________-*/
#include "NavigationBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"
#include "NavigationArrow.h"

void NavigationBehaviour::update(Entity entity) {
    //auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
    auto& navigation = ecsCoordinator.getComponent<NavigationComponent>(entity);

    // Only update visible navigation arrows
    if (!navigation.isVisible) {
        return;
    }

}