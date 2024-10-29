#include "FontSystemECS.h"
#include "ECSCoordinator.h"
#include "GlobalCoordinator.h"
#include <iostream>

FontSystemECS::FontSystemECS()
    : FontSystemECS(std::make_shared<FontSystem>(), 48) { // for now adjust the font size here
}

FontSystemECS::FontSystemECS(std::shared_ptr<FontSystem> fontSys, int fontSize)
    : fontSystem(std::move(fontSys)), fontSize(fontSize) {
    initialise();
}

void FontSystemECS::loadAdditionalFont(const std::string& fontPath, int fontSize) {
    if (!fontSystem) {
        std::cerr << "ERROR: FontSystem pointer is null." << std::endl;
        return;
    }
    std::cout << "Loading additional font from: " << fontPath << " with size " << fontSize << std::endl;
    fontSystem->loadFont(fontPath, fontSize); // Assuming this method allows for different font paths
    // Here you would need to modify FontSystem to accept different font paths
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
            std::cout << "Loading font in FontSystemECS with size " << fontSize << std::endl;
            //fontSystem->loadFont(fontSize);
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

    const float maxWidth = 650.0f;

    for (auto entity : entities) {
        if (ecsCoordinator.hasComponent<FontComponent>(entity)) {
            auto& fontComp = ecsCoordinator.getComponent<FontComponent>(entity);
            fontSystem->loadFont(fontComp.fontPath, fontSize); // Load the specific font for this entity if needed
            fontSystem->draw(fontComp.text, fontComp.position.x, fontComp.position.y, fontComp.scale, fontComp.color, maxWidth);
        }
    }
}

void FontSystemECS::cleanup() {
    if (fontSystem) {
        fontSystem->cleanup(); // Call cleanup on FontSystem
        fontSystem.reset(); // Reset the shared pointer
    }
    entities.clear(); // Clear entity references
    std::cout << "FontSystemECS cleaned up successfully." << std::endl;
}

