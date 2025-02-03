#pragma once
#include "EngineDefinitions.h"
#include "Systems.h"
#include "GlobalCoordinator.h"

class LayerManager : public GameSystems
{
public:
	LayerManager();
	~LayerManager();
	void initialise() override;
	void update() override;
	void cleanup() override;
	SystemType getSystem() override;

	void addNewLayer();
	void addNumLayers(int num);
	int getLayerCount();

	void addEntityToLayer(int layer, Entity entity);
	void removeEntityFromLayer(int layer, Entity entity);
	void shiftEntityToLayer(int oriLayer, int newLayer, Entity entity);

	const std::vector<Entity>& getEntitiesFromLayer(int layer);
	
	void clearLayer(int layer);
	
private:
	std::unordered_map<int, std::vector<Entity>>* m_Layers;
	int m_LayerCount;
};