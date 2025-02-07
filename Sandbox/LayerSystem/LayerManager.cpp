/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   LayerManager.cpp
@brief:  This source file includes a simple implementation of the LayerManager class.
		 LayerManager class is a child class of GameSystems, which is responsible for
		 the all the layers of the game. Users are able to create, delete, shift entities
		 ,clear and handle visibility of layers.

		 Joel Chu (c.weiyuan): Implemented all of the functions that belongs to
							   the LayerManager class.
							   100%
*//*___________________________________________________________________________-*/

#include "GlobalCoordinator.h"
#include "LayerManager.h"
#include <iostream>


LayerManager::LayerManager() {
	m_Layers = new std::unordered_map<int, Layer>();
	m_LayerCount = 0;
}

LayerManager::~LayerManager() {}

void LayerManager::initialise() {
	//read from json amount of layers
	loadLayerFromJSON(FilePathManager::GetLayerJSONPath());

	//for now only have four layer
	addNumLayers(m_LayerCount);

	//for testing purpose set layer 1 to invisible
	//if layer 1 is invible, entities in layer 1 will not be updated (for both graphics and logic)
	//setLayerVisibility(1, false);
}

void LayerManager::update() {
	//print how many in layer 0
	//std::cout << "Layer 0 has " << m_Layers->at(0).size() << " entities" << std::endl;
}

void LayerManager::cleanup() {
	saveLayerToJSON(FilePathManager::GetLayerJSONPath());

	delete m_Layers;
	m_Layers = nullptr;
}

SystemType LayerManager::getSystem() {
	return SystemType::LayerManagerType;
}

//function used to add entity to a specific layer
void LayerManager::addEntityToLayer(int layer, Entity entity) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		m_Layers->at(layer).entities.push_back(entity);
		std::cout << "Entity added to layer " << layer << std::endl;
	}
	else {
		std::cout << "Layer " << layer << " does not exist and pushed to layer 0" << std::endl;
		m_Layers->at(0).entities.push_back(entity);
	}
}

//function used to remove entity from a specific layer
void LayerManager::removeEntityFromLayer(int layer, Entity entity) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		auto& entities = m_Layers->at(layer).entities;
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	}
	else {
		std::cout << "Layer " << layer << " does not exist" << std::endl;
	}
}

//function used to remove entity from all layers
void LayerManager::removeEntityFromAllLayer(Entity entity) {
	//find the layer entity is in and remove it
	for (auto& layer : *m_Layers) {
		auto& entities = layer.second.entities;
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	}
}

//function can also be used to shift the entity up or down one layer
void LayerManager::shiftEntityToLayer(int oriLayer, int newLayer, Entity entity) {
	//check if both layer exists
	if (m_Layers->find(oriLayer) != m_Layers->end() && m_Layers->find(newLayer) != m_Layers->end()) {
		auto& layerToRmv = m_Layers->at(oriLayer).entities;
		auto& layerToAdd = m_Layers->at(newLayer).entities;
		//check if entity exist in the layerToRmv
		if (std::find(layerToRmv.begin(), layerToRmv.end(), entity) == layerToRmv.end()) {
			std::cout << "Entity does not exist in layer " << oriLayer << std::endl;
			return;
		}
		layerToRmv.erase(std::remove(layerToRmv.begin(), layerToRmv.end(), entity), layerToRmv.end());
		layerToAdd.push_back(entity);
	}
	else {
		std::cout << "Layer " << oriLayer << " or " << newLayer << " does not exist" << std::endl;
	}
}

const std::vector<Entity>& LayerManager::getEntitiesFromLayer(int layer) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		return m_Layers->at(layer).entities;
	}
	else {
		std::cout << "Layer does not exist! Providing layer 0 list instead!" << std::endl;
		return m_Layers->at(0).entities;
	}
}

bool LayerManager::clearLayer(int layer) {
	//if they ask to clear layer 0, deny request
	if (layer == 0) {
		std::cout << "Cannot clear layer 0" << std::endl;
		return false;
	}
	if (m_Layers->find(layer) != m_Layers->end()) {
		//add entities from that layer to layer 0 then remove the layer
		m_Layers->at(0).entities.insert(m_Layers->at(0).entities.end(), m_Layers->at(layer).entities.begin(), m_Layers->at(layer).entities.end());
		m_Layers->at(layer).entities.clear();
		return true;
	}
	else {
		std::cout << "Layer " << layer << " does not exist" << std::endl;
		return false;
	}
}

void LayerManager::addNewLayer() {
	for (int i = 0; i < m_LayerCount; i++) {
		m_Layers->insert(std::make_pair(i, Layer()));
	}
}

void LayerManager::addNumLayers(int num) {
	for (int i = 0; i < num; i++) {
		addNewLayer();
	}
}

int LayerManager::getLayerCount() {
	return m_LayerCount;
}

void LayerManager::setLayerVisibility(int layer, bool isVisible) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		m_Layers->at(layer).isVisible = isVisible;
	}
	else {
		std::cout << "Layer " << layer << " does not exist" << std::endl;
	}
}

//to check is specific layer is visible
bool LayerManager::getLayerVisibility(int layer) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		return m_Layers->at(layer).isVisible;
	}
	else {
		//if layer doesn't exist return false
		std::cout << "Layer " << layer << " does not exist" << std::endl;
		return false;
	}
}

//to check if specific entity is visible
bool LayerManager::getEntityVisibility(Entity entity) {
	for (auto& layer : *m_Layers) {
		auto& entities = layer.second.entities;
		if (std::find(entities.begin(), entities.end(), entity) != entities.end()) {
			return layer.second.isVisible;
		}
	}
	return false;
}

void LayerManager::setGuiVisibility(int layer, bool isVisible) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		m_Layers->at(layer).isGuiVisible = isVisible;
	}
	else {
		std::cout << "Layer " << layer << " does not exist" << std::endl;
	}
}

bool LayerManager::getGuiVisibility(int layer) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		return m_Layers->at(layer).isGuiVisible;
	}
	else {
		std::cout << "Layer " << layer << " does not exist" << std::endl;
		return false;
	}
}
//if entity is in any layer, return the layer number, if not in any layer it will be in layer 0
int LayerManager::getEntityLayer(Entity entity) {
	for (auto& layer : *m_Layers) {
		auto& entities = layer.second.entities;
		if (std::find(entities.begin(), entities.end(), entity) != entities.end()) {
			return layer.first; // Returns layer number
		}
	}
	return -1; // Entity not found
}

//if entity is in any layer, return the layer number, if not in any layer it will be in layer 0
int LayerManager::getLayerFromEntity(Entity entity) {
	for (auto& layer : *m_Layers) {
		auto& entities = layer.second.entities;
		if (std::find(entities.begin(), entities.end(), entity) != entities.end()) {
			return layer.first;
		}
	}
	return 0;
}

void LayerManager::loadLayerFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json currentObj = serializer.GetJSONObject();

	serializer.ReadInt(m_LayerCount, "layers");
}

void LayerManager::saveLayerToJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();
	serializer.WriteInt(m_LayerCount, "layers", filename);
}
