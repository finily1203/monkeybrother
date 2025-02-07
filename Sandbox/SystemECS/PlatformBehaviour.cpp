/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   PlatformBehaviour.cpp
@brief:  This source file includes the implementation of the PlatformBehaviour
         that logicSystemECS uses to handle the behaviour of the platform entity.

         Joel Chu (c.weiyuan): defined the functions of PlatformBehaviour class
                               100%
*//*___________________________________________________________________________-*/

#include "BehaviourComponent.h"
#include "PlatformBehaviour.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"



void PlatformBehaviour::update(Entity entity) {
    
	// Add closest platform component for platform collision
    if (!ecsCoordinator.hasComponent<ClosestPlatform>(entity)) {
        ClosestPlatform closestPlatform{};
        closestPlatform.isClosest = false;
        ecsCoordinator.addComponent(entity, closestPlatform);
    }
    
    
}