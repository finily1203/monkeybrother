/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   LayerManager.h
@brief:  This header file includes all the declaration of the LayerManager class.
		 Currently LayerManager has capabilities to create, delete, shift entities
		 ,clear and handle visibility of layers. Layer struct is also created to
		 store visibility of layers and entities in the layer.

		 Joel Chu (c.weiyuan): Declared all the functions in LayerManager class and
							   Layer struct.		
							   100%
*//*___________________________________________________________________________-*/

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

	//Entity's position
	int getEntityLayer(Entity entity);
	int getLayerFromEntity(Entity entity);

	//save and load number of layers from JSON
	void loadLayerFromJSON(std::string const& filename);
	void saveLayerToJSON(std::string const& filename);
	
private:
	std::unordered_map<int, Layer>* m_Layers;
	int m_LayerCount;

};