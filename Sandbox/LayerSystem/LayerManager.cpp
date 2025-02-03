#include "GlobalCoordinator.h"
#include "LayerManager.h"
#include <iostream>

LayerManager::LayerManager() {
	m_Layers = new std::unordered_map<int, std::vector<Entity>>();
	m_LayerCount = 0;
}

LayerManager::~LayerManager() {}

void LayerManager::initialise() {
	//for now only have four layer
	addNumLayers(4);
}

void LayerManager::update() {
	//print how many in layer 0
	//std::cout << "Layer 0 has " << m_Layers->at(0).size() << " entities" << std::endl;
}

void LayerManager::cleanup() {
	delete m_Layers;
	m_Layers = nullptr;
}

SystemType LayerManager::getSystem() {
	return SystemType::LayerManagerType;
}

void LayerManager::addEntityToLayer(int layer, Entity entity) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		m_Layers->at(layer).push_back(entity);
		std::cout << "Entity added to layer " << layer << std::endl;
	}
	else {
		std::cout << "Layer " << layer << " does not exist and pushed to layer 0" << std::endl;
		m_Layers->at(0).push_back(entity);
	}
}

void LayerManager::removeEntityFromLayer(int layer, Entity entity) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		auto& entities = m_Layers->at(layer);
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	}
	else {
		std::cout << "Layer does not exist" << std::endl;
	}
}

//function can also be used to shift the entity up or down one layer
void LayerManager::shiftEntityToLayer(int oriLayer, int newLayer, Entity entity) {
	//check if both layer exists
	if (m_Layers->find(oriLayer) != m_Layers->end() && m_Layers->find(newLayer) != m_Layers->end()) {
		auto& layerToRmv = m_Layers->at(oriLayer);
		auto& layerToAdd = m_Layers->at(newLayer);
		//check if entity exist in the layerToRmv
		if (std::find(layerToRmv.begin(), layerToRmv.end(), entity) == layerToRmv.end()) {
			std::cout << "Entity does not exist in layer " << oriLayer << std::endl;
			return;
		}
		layerToRmv.erase(std::remove(layerToRmv.begin(), layerToRmv.end(), entity), layerToRmv.end());
		layerToAdd.push_back(entity);
	}
	else {
		std::cout << "Layer does not exist" << std::endl;
	}
}

const std::vector<Entity>& LayerManager::getEntitiesFromLayer(int layer) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		return m_Layers->at(layer);
	}
	else {
		std::cout << "Layer does not exist" << std::endl;
		return m_Layers->at(0);
	}
}

void LayerManager::clearLayer(int layer) {
	if (m_Layers->find(layer) != m_Layers->end()) {
		m_Layers->at(layer).clear();
	}
	else {
		std::cout << "Layer does not exist" << std::endl;
	}
}

void LayerManager::addNewLayer() {
	m_Layers->insert(std::make_pair(m_LayerCount, std::vector<Entity>()));
	m_LayerCount++;
}

void LayerManager::addNumLayers(int num) {
	for (int i = 0; i < num; i++) {
		addNewLayer();
	}
}

int LayerManager::getLayerCount() {
	return m_LayerCount;
}

