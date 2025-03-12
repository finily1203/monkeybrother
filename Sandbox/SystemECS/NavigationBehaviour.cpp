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

    // Apply rotation for visual effect if needed
    // transform.orientation.SetY(transform.orientation.GetY() + (15.0f * GLFWFunctions::delta_time));
}