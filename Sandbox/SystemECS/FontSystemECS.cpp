#include "FontSystemECS.h"
#include "ECSCoordinator.h"
#include "GlobalCoordinator.h"
#include <iostream>

FontSystemECS::FontSystemECS()
    : FontSystemECS(std::make_shared<FontSystem>(), 48) { // Default font size
}

FontSystemECS::FontSystemECS(std::shared_ptr<FontSystem> fontSys, int fontSize)
    : fontSystem(std::move(fontSys)), fontSize(fontSize) {
    initialise();
}



void FontSystemECS::initialise() {
    if (!fontSystem) {
        std::cerr << "ERROR: FontSystem pointer is null." << std::endl;
        return;
    }

    if (!fontSystem->isInitialized) {
        std::cout << "Initializing FontSystem within FontSystemECS..." << std::endl;
        fontSystem->initialise();
        if (fontSystem->isInitialized) {
            std::cout << "Loading default font in FontSystemECS with size " << fontSize << std::endl;

        }
        else {
            std::cerr << "FontSystem initialization failed. Cannot load font." << std::endl;
        }
    }
    else {
        std::cout << "FontSystem is already initialized." << std::endl;
    }
}

void FontSystemECS::update(float dt) {
    if (!fontSystem || !fontSystem->isInitialized) {
        std::cerr << "ERROR: FontSystem is null or not initialized. Cannot update font entities." << std::endl;
        return;
    }

    const float maxWidth = 450.0f;

    for (auto entity : entities) {
        if (ecsCoordinator.hasComponent<FontComponent>(entity)) {
            auto& fontComp = ecsCoordinator.getComponent<FontComponent>(entity);

           
            if (fontSystem->Fonts.find(fontComp.fontPath) == fontSystem->Fonts.end()) {
                fontSystem->loadFont(fontComp.fontPath, fontSize);
            }

            fontSystem->draw(fontComp.text, fontComp.fontPath, fontComp.position.x, fontComp.position.y, fontComp.scale, fontComp.color, maxWidth);
        }
    }
}

void FontSystemECS::cleanup() {
    if (fontSystem) {
        fontSystem->cleanup();
        fontSystem.reset();
    }
    entities.clear(); 
    std::cout << "FontSystemECS cleaned up successfully." << std::endl;
}
