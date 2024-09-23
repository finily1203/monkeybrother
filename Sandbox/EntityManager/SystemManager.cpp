#include "SystemManager.h"
#include "ECSDefinitions.h"

template <typename T>
std::shared_ptr<T> SystemManager::registerSystem() {
	std::string typeName = typeid(T).name();
	//checks if system already exists
	assert(Systems.find(typeName) == Systems.end() && "Registering system more than once.");
	auto system = std::make_shared<T>();
	Systems.insert({ typeName, system });
}

template <typename T>
void SystemManager::setSystemSignature(ComponentSig signature) {
	std::string typeName = typeid(T).name();
	//checks if system already exists
	assert(Systems.find(typeName) != Systems.end() && "System used before registered.");
	systemSignatures.insert({ typeName, signature });
}

void SystemManager::entityRemoved(Entity entity) {
	//erase entity from all systems
	for (auto const& pair : Systems) {
		auto const& system = pair.second;
		system->entities.erase(entity);
	}
}

void SystemManager::entitySigChange(Entity entity, ComponentSig entitySig) {
	//update entity signature in all systems
	for (auto const& pair : Systems) {
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& systemSig = systemSignatures[type];

		//check if entity signature matches system signature
		if ((entitySig & systemSig) == systemSig) {
			system->entities.insert(entity);
		}
		else {
			system->entities.erase(entity);
		}
	}
}