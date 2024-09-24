#pragma once
#include "ECSCoordinator.h"
class ObjectFactory
{
public:
	ObjectFactory();
	~ObjectFactory();

	Entity CreateObjectFromFile(const std::string& filename);


};