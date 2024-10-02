#include "ComponentManager.h"

void ComponentManager::entityRemoved(Entity entity) {
	for (auto const& pair : componentHandlers) {
		auto const& component = pair.second;
		component->entityRemoved(entity);
	}
}

void ComponentManager::cleanup() {
	componentHandlers.clear();
	componentTypes.clear();
	nextComponentType = 0;
}