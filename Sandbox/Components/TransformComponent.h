/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   TransformComponent.h
@brief:  This header file is the Transform Component used by the ECS to handle
		 the transformation of the game objects.

		 Joel Chu (c.weiyuan): declared the struct component
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include <glm/glm.hpp>
#include "vector2D.h"
#include "matrix3x3.h"

//ori, scl, pos, mdl_xform, mdl_ndc_xform
struct TransformComponent
{
	myMath::Vector2D orientation;
	myMath::Vector2D scale;
	myMath::Vector2D position;
	//glm::mat3 mdl_xform;
	//glm::mat3 mdl_to_ndc_xform;
	myMath::Matrix3x3 mdl_xform;
	myMath::Matrix3x3 mdl_to_ndc_xform;
};