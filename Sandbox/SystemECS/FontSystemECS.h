/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  FontSystemECS.h
@brief  :  Declaration of the FontSystemECS class, an ECS-based system for managing and rendering
           text entities in the game environment. Integrates with FontSystem to provide efficient
           font rendering within the ECS framework.

* Javier Chua (javierjunliang.chua) :
        - Declared default and parameterized constructors to initialize FontSystemECS with or without custom font size.
        - Declared initialise() to manage the initialization of FontSystem, including font loading with size control.
        - Declared update() to iterate over font entities and render text based on entity components and delta time.
        - Declared cleanup() to release resources and reset FontSystemECS to a clean state.
        - Declared getSystemECS() to return the system type as a string for ECS identification purposes.

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