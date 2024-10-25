#include "FontSystemECS.h"
#include "ECSCoordinator.h"
#include "GlobalCoordinator.h"

FontSystemECS::FontSystemECS(std::shared_ptr<FontSystem> fontSys, int fontSize)
    : fontSystem(fontSys) {
    if (fontSystem) {
        fontSystem->loadFont(fontSize);
    } else {
        std::cerr << "Failed to initialize FontSystem" << std::endl;
    }
}

void FontSystemECS::initialise() {
  
}

void FontSystemECS::update(float dt) {
    //std::cout << "Font system update" << std::endl;
    for (auto entity : entities) {
        if (ecsCoordinator.hasComponent<FontComponent>(entity)) {
            auto& fontComp = ecsCoordinator.getComponent<FontComponent>(entity);
            
            if (fontSystem) { // Check if fontSystem is valid
                fontSystem->draw(fontComp.text, fontComp.position.x, fontComp.position.y, fontComp.scale, fontComp.color);
                std::cout << "Text Entity: " << entity << std::endl;
            } else {
                std::cerr << "FontSystem is not initialized for entity: " << entity << std::endl;
            }
        }
    }
}

void FontSystemECS::cleanup() {
   
}
