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

private:
    std::shared_ptr<FontSystem> fontSystem;
    int fontSize;
};