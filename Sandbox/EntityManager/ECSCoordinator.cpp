//#include "ECSCoordinator.h"
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include "vector2D.h"

//ECSCoordinator::ECSCoordinator()
//{
//	entityManager = std::make_unique<EntityManager>();
//	componentManager = std::make_unique<ComponentManager>();
//	systemManager = std::make_unique<SystemManager>();
//}
//
//Entity ECSCoordinator::createEntity()
//{
//	return entityManager->createEntity();
//}
//
//void ECSCoordinator::destroyEntity(Entity entity)
//{
//	//remove entity from all systems
//	entityManager->destroyEntity(entity);
//	componentManager->entityRemoved(entity);
//	systemManager->entityRemoved(entity);
//}
//
//template <typename T>
//void ECSCoordinator::registerComponent()
//{
//	componentManager->registerComponentHandler<T>();
//}
//
//template <typename T>
//void ECSCoordinator::addComponent(Entity entity, T component)
//{
//	componentManager->addComponent<T>(entity, component);
//
//	//update entity signature
//	auto signature = entityManager->getSignature(entity);
//	signature.set(componentManager->getComponentType<T>(), true);
//	entityManager->setSignature(entity, signature);
//
//	//update system signature
//	systemManager->entitySigChange(entity, signature);
//}
//
//template <typename T>
//void ECSCoordinator::removeComponent(Entity entity)
//{
//	componentManager->removeComponent<T>(entity);
//
//	//update entity signature
//	auto signature = entityManager->getSignature(entity);
//	signature.set(componentManager->getComponentType<T>(), false);
//	entityManager->setSignature(entity, signature);
//
//	//update system signature
//	systemManager->entitySigChange(entity, signature);
//}
//
//template <typename T>
//T& ECSCoordinator::getComponent(Entity entity)
//{
//	return componentManager->getComponent<T>(entity);
//}
//
//template <typename T>
//ComponentType ECSCoordinator::getComponentType()
//{
//	return componentManager->getComponentType<T>();
//}
//
//template <typename T>
//std::shared_ptr<T> ECSCoordinator::registerSystem()
//{
//	return systemManager->registerSystem<T>();
//}
//
//template <typename T>
//void ECSCoordinator::setSystemSignature(ComponentSig signature)
//{
//	systemManager->setSystemSignature<T>(signature);
//}
//
////Entity ECSCoordinator::createEntity(const std::string& filename)
////{
////	std::ifstream ifs(filename);
////	std::string line;
////
////	Entity entity = createEntity();
////
////	std::string entityName;
////	std::vector<std::string> addSystems;
////	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> addComponents;
////
////	while (std::getline(ifs, line)) {
////		if (line == "Name:") {
////			std::getline(ifs, entityName);
////		}
////
////
////	}
////}
//
//struct Position {
//	myMath::Vector2D pos;
//};
//
//struct Size {
//	myMath::Vector2D scale;
//};
//
//struct velocity {
//	float speed;
//};
//
//class PlayerSystem : public System {
//public:
//	void update() {
//		std::cout << "PlayerSystem update" << std::endl;
//	}
//};
//
//void ECSCoordinator::test() {
//	std::cout << "testing ECS" << std::endl;
//	//create player entity
//	Entity player = createEntity();
//
//	//register components
//	registerComponent<Position>();
//	registerComponent<Size>();
//	registerComponent<velocity>();
//
//	auto playerSystem = registerSystem<PlayerSystem>();
//
//	addComponent(player, Position{ myMath::Vector2D(0, 0) });
//	addComponent(player, Size{ myMath::Vector2D(1, 1) });
//	addComponent(player, velocity{ 5 });
//
//	//set system signature
//	ComponentSig playerSig;
//	playerSig.set(getComponentType<Position>(), true);
//	playerSig.set(getComponentType<Size>(), true);
//	playerSig.set(getComponentType<velocity>(), true);
//	setSystemSignature<PlayerSystem>(playerSig);
//
//}
//
////player
////Components
////pos x, y
////speed (velocity)
////size (width, height)
////sprite (texture, width, height)