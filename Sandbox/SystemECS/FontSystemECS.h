/*All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Javier Chua (javierjunliang.chua)@team   :  MonkeHood
@course :  CSD2401@file   :  FontSystemECS.cpp
@brief  :  Provides the implementation of the FontSystemECS class, an Entity-Component-System (ECS)-based system 
           designed for efficient management and rendering of text entities within the game environment. This class
           integrates with FontSystem, enabling optimized font rendering by linking text components to entities           
           within the ECS framework.
Detailed Description:
    - FontSystemECS is responsible for handling all text rendering tasks for entities with font components in an ECS context.    - It utilizes FontSystem to load and render fonts, controlling font size and other text attributes as defined per entity.
    - Core functionalities include initialization, updating (to handle text rendering per frame), and cleanup (to release resources).
* Javier Chua (javierjunliang.chua) :    - Developed constructors (default and parameterized) for FontSystemECS to initialize with or without a custom font size.
    - Implemented initialise(), which sets up FontSystem, loading the font with specified size settings.    - Added update(float dt) to render text entities based on their components and delta time, ensuring real-time rendering.
    - Defined cleanup() to free resources and reset FontSystemECS.    - Created getSystemECS(), which returns the system type as a string identifier, supporting ECS integration and debugging.
File Contributions: Javier Chua
_______________________________________________________________________________________________________________*/
#pragma once

#include <memory> 
#include "Systems.h"
#include "FontSystem.h"
#include "ECSCoordinator.h"
#include "FontComponent.h"
#include "CameraSystem2D.h"
class FontSystemECS : public System {
public:
    FontSystemECS();
    FontSystemECS(std::shared_ptr<FontSystem> fontSys, int fontSize);

    void initialise() override;
    void update(float dt) override;
    void cleanup() override;

    std::string getSystemECS() override;
   
private:
    std::shared_ptr<FontSystem> fontSystem;
    int fontSize;
};