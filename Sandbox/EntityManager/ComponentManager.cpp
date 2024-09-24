//#include "ComponentManager.h"
//#include "ECSDefinitions.h"

//template <typename T>
//void ComponentManager::registerComponentHandler() {
//	//check if component type already exists
//	const char* typeName = typeid(T).name();
//	assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once.");
//
//	//add component type to the component types map
//	componentTypes.insert({ typeName, nextComponentType });
//	componentHandlers.insert({ typeName, std::make_shared<ComponentHandler<T>>() });
//	nextComponentType++;
//}

//template <typename T>
//void ComponentManager::addComponent(Entity entity, T component) {
//	auto* componentHandler = getComponentHandler<T>();
//	componentHandler->addComponentHandler(entity, component);
//}

//template <typename T>
//void ComponentManager::removeComponent(Entity entity) { 
//	auto* componentHandler = getComponentHandler<T>();
//	componentHandler->removeComponentHandler(entity);
//}
//
//template <typename T>
//T& ComponentManager::getComponent(Entity entity) {
//	auto* componentHandler = getComponentHandler<T>();
//	componentHandler->getComponentHandler(entity);
//}

//template <typename T>
//ComponentType ComponentManager::getComponentType() {
//	const char* typeName = typeid(T).name();
//	assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");
//
//	return componentTypes[typeName];
//}

////Helper function to get the componentHandler
//template <typename T>
//ComponentHandler<T>* ComponentManager::getComponentHandler() {
//	const char* typeName = typeid(T).name();
//	assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");
//
//	return std::static_pointer_cast<ComponentHandler<T>>(componentHandlers[typeName]);
//}

//void ComponentManager::entityRemoved(Entity entity) {
//	for (auto const& pair : componentHandlers) {
//		auto const& component = pair.second;
//		component->entityRemoved(entity);
//	}
//}


