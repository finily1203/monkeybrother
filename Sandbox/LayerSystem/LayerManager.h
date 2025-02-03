#pragma once
#include "EngineDefinitions.h"
#include "Systems.h"
#include "GlobalCoordinator.h"

struct Layer
{
	std::vector<Entity> entities;
	bool isVisible; //default visibility is true
	bool isGuiVisible; //default visibility is false

	Layer() : isVisible(true), isGuiVisible(false) {};
};

class LayerManager : public GameSystems
{
public:
	LayerManager();
	~LayerManager();
	void initialise() override;
	void update() override;
	void cleanup() override;
	SystemType getSystem() override;

	//Layer management
	void addNewLayer();
	void addNumLayers(int num);
	int getLayerCount();
	bool clearLayer(int layer);

	//Layer visibility
	void setLayerVisibility(int layer, bool isVisible);
	bool getLayerVisibility(int layer);
	bool getEntityVisibility(Entity entity);

	//Gui visibility
	void setGuiVisibility(int layer, bool isVisible);
	bool getGuiVisibility(int layer);

	//Movement of entity between layers
	void addEntityToLayer(int layer, Entity entity);
	void removeEntityFromLayer(int layer, Entity entity);
	void removeEntityFromAllLayer(Entity entity);
	void shiftEntityToLayer(int oriLayer, int newLayer, Entity entity);
	const std::vector<Entity>& getEntitiesFromLayer(int layer);
	
private:
	std::unordered_map<int, Layer>* m_Layers;
	int m_LayerCount;
};