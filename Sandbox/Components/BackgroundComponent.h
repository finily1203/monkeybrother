/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   BackgroundComponent.h
@brief:  This header file includes a bool variable used by ECS to ensure that
 graphics system knows which entity to render as a background.
 Joel Chu (c.weiyuan): declared the struct component
   100%
*//*___________________________________________________________________________-*/

#pragma once
struct BackgroundComponent
{
	bool isBackground;

	BackgroundComponent() : isBackground(false) {}
};