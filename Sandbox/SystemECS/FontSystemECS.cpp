/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  FontSystemECS.cpp
@brief  :  Implementation of the FontSystemECS class, an ECS-based system for managing and rendering
           text entities in the game environment. Integrates with FontSystem to provide efficient
           font rendering within the ECS framework.

* Javier Chua (javierjunliang.chua) :
        - Developed default and parameterized constructors to initialize FontSystemECS with or without custom font size.
        - Implemented initialise() to manage the initialization of FontSystem, including font loading with size control.
        - Added update() to iterate over font entities and render text based on entity components and delta time.
        - Implemented cleanup() to release resources and reset FontSystemECS to a clean state.
        - Created getSystemECS() to return the system type as a string for ECS identification purposes.

File Contributions: Javier Chua
_______________________________________________________________________________________________________________*/

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
    UNREFERENCED_PARAMETER(dt);
    if (!fontSystem || !fontSystem->isInitialized) {
        std::cerr << "ERROR: FontSystem is null or not initialized. Cannot update font entities." << std::endl;
        return;
    }

    const float maxWidth = 450.0f;

    for (auto entity : entities) {
        if (ecsCoordinator.hasComponent<FontComponent>(entity)) {
            auto& fontComp = ecsCoordinator.getComponent<FontComponent>(entity);
            auto& fontTransform = ecsCoordinator.getComponent<TransformComponent>(entity);

            fontSystem->draw(fontComp.text, fontComp.fontId, fontTransform.position.GetX(), fontTransform.position.GetY(), fontComp.textScale, fontComp.color, maxWidth, myMath::Matrix3x3::ConvertToMatrix3x3(cameraSystem.getViewMatrix()));
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

std::string FontSystemECS::getSystemECS() {
    return "FontSystemECS";
}
