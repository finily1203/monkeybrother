//#include "TestEntityAndComponent.h"
//#include "EntityManager.h"
//#include "ComponentManager.h"
//
//struct HealthComponent {
//	float health;
//};
//
//struct PositionComponent {
//	float x, y, z;
//};
//
//void TestEntityAndComponent() {
//	std::cout << "Testing Entity and Component!" << std::endl << std::endl;
//	
//	std::cout << "Creating Entity and Component Managers" << std::endl;
//	EntityManager em;
//	ComponentManager<HealthComponent> cm;
//	ComponentManager<PositionComponent> pm;
//	std::cout << std::endl;
//
//	std::cout << "Creating Entities" << std::endl;
//	Entity player = em.createEntity();
//	Entity enemy1 = em.createEntity();
//	Entity enemy2 = em.createEntity();
//	Entity enemy3 = em.createEntity();
//	std::cout << std::endl;
//
//	std::cout << "Check No. Entities & Available Entities" << std::endl;
//	std::cout << "Entities: " << em.getLiveEntCount() << std::endl;
//	std::cout << "Available Entities: " << em.getAvailableEntCount() << std::endl;
//	std::cout << std::endl;
//
//	std::cout << "Creating Components and setting Component Signatures" << std::endl;
//	HealthComponent playerHealth = { 100 };
//	HealthComponent enemyHealth = { 200 }; //DONT MIND THIS, OUR ENEMY CANT BE KILLED
//	PositionComponent playerPos = { 0.f, 0.f, 0.f };
//	PositionComponent enemyPos1 = { 50.f, 200.0f, 0.f };
//	PositionComponent enemyPos2 = { 100.f, 50.0f, 0.f };
//	PositionComponent enemyPos3 = { 200.f, 150.0f, 0.f };
//
//	ComponentSig healthSig;
//	ComponentSig positionSig;
//
//	healthSig.set(0);
//	positionSig.set(1);
//
//	em.setSignature(player, healthSig);
//	em.setSignature(player, positionSig);
//
//	std::cout << "Player Components: " << em.getSignature(player) << std::endl;
//	std::cout << "" << std::endl;
//
//}

//void testComponentManager() {
//	std::cout << "Testing HealthMgr" << std::endl;
//	ComponentManager<HealthComponent> HealthMgr;
//	Entity ent1 = 1;
//	Entity ent2 = 2;
//	Entity ent3 = 3;
//
//	HealthComponent health1 = { 100 };
//	HealthComponent health2 = { 200 };
//
//	std::cout << "Adding Health Component" << std::endl;
//	HealthMgr.addComponent(ent1, health1, "Health");
//	HealthMgr.addComponent(ent2, health2, "Health");
//	std::cout << std::endl;
//
//	std::cout << "Retrieve Health Component" << std::endl;
//	HealthComponent& health1Ref = HealthMgr.getComponent(ent1);
//	HealthComponent& health2Ref = HealthMgr.getComponent(ent2);
//	std::cout << "Entity 1 Health: " << health1Ref.health << std::endl;
//	std::cout << "Entity 2 Health: " << health2Ref.health << std::endl;
//
//	std::cout << "Check Entity Component" << std::endl;
//	std::cout << "Checking Entity 1..." << (HealthMgr.hasComponent(ent1, "Health") ? "Yes" : "No") << std::endl;
//	std::cout << "Checking Entity 2..." << (HealthMgr.hasComponent(ent2, "Health") ? "Yes" : "No") << std::endl;
//	std::cout << "Checking Entity 3..." << (HealthMgr.hasComponent(ent3, "Health") ? "Yes" : "No") << std::endl;
//	std::cout << std::endl;
//
//	//EXPAND ON THIS
//	std::cout << "Removing All Component: WARNING NEEDS TO UPDATE" << std::endl;
//	HealthMgr.removeAllComponent(ent1);
//	std::cout << "Checking Entity 1..." << (HealthMgr.hasComponent(ent1, "Health") ? "Yes" : "No") << std::endl;
//	std::cout << "Checking Entity 2..." << (HealthMgr.hasComponent(ent2, "Health") ? "Yes" : "No") << std::endl;
//	std::cout << "Checking Entity 3..." << (HealthMgr.hasComponent(ent3, "Health") ? "Yes" : "No") << std::endl;
//	std::cout << std::endl;
//
//
//	std::cout << "Removing Specific Component" << std::endl;
//	HealthMgr.addComponent(ent3, health1, "Health");
//	HealthComponent& health3Ref = HealthMgr.getComponent(ent3);
//	std::cout << "Checking Entity 3..." << (HealthMgr.hasComponent(ent3, "Health") ? "Yes" : "No") << std::endl;
//	std::cout << "Now Removing....." << std::endl;
//	HealthMgr.removeSpeComponent(ent3, "Health");
//	std::cout << "Checking Entity 3..." << (HealthMgr.hasComponent(ent3, "Health") ? "Yes" : "No") << std::endl;
//}

//void EntityManager::testEntityManager() {
//	std::cout << "Create Entity Test" << std::endl;
//
//	Entity ent1 = createEntity();
//	Entity ent2 = createEntity();
//	Entity ent3 = createEntity();
//
//	std::cout << "Entity 1: " << ent1 << std::endl;
//	std::cout << "Entity 2: " << ent2 << std::endl;
//	std::cout << "Entity 3: " << ent3 << std::endl;
//
//	std::cout << "Set Signature Test" << std::endl;
//	ComponentSig physicsSig;
//	ComponentSig renderSig;
//	ComponentSig inputSig;
//	physicsSig.set(0);
//	renderSig.set(1);
//	inputSig.set(2);
//	setSignature(ent1, physicsSig);
//	setSignature(ent2, renderSig);
//	setSignature(ent3, inputSig);
//
//	std::cout << "1st Entity contains: " << getSignature(ent1) << std::endl; //physicsSig
//	std::cout << "2nd Entity contains: " << getSignature(ent2) << std::endl; //renderSig
//	std::cout << "3rd Entity contains: " << getSignature(ent3) << std::endl; //inputSig
//	std::cout << "Entities Left: " << liveEntCount << std::endl; //3
//	std::cout << "Available Entities: " << availableEnt.size() << std::endl; //4997
//
//
//	std::cout << "Destroy Entity Test" << std::endl;
//	destroyEntity(ent1);
//
//	std::cout << "1st Entity contains: " << getSignature(ent1) << std::endl; //empty
//	std::cout << "2nd Entity contains: " << getSignature(ent2) << std::endl; //renderSig
//	std::cout << "3rd Entity contains: " << getSignature(ent3) << std::endl; //inputSig
//	std::cout << "Entities Left: " << liveEntCount << std::endl; //2
//	std::cout << "Available Entities: " << availableEnt.size() << std::endl; //4997
//}