/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  FontSystemECS.cpp
@brief  :  This file contains the implementation of the FontSystemECS class,
           which integrates font rendering functionality within the ECS architecture.

* Javier Chua (javierjunliang.chua) :
        - Developed the FontSystemECS class to work with FontSystem in ECS, including initialization and cleanup.
        - Added update function to render text based on delta time for dynamic display.
        - Created constructors to initialize FontSystemECS with a shared FontSystem and adjustable font size.
        - Implemented getSystemECS function to identify the system type within ECS.

File Contributions: Javier Chua
_______________________________________________________________________________________________________________*/

#pragma once

#include <memory> 
#include "Systems.h"
#include "FontSystem.h"
#include "ECSCoordinator.h"
#include "FontComponent.h"

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
    std::string deltaTimeString;
};