/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Lew Zong Han Owen (z.lew)
@team:   MonkeHood
@course: CSD2401
@file:   UIComponent.h
@brief:  This header file includes a bool variable used by ECS to ensure that
 graphics system knows which entity to render as a UI. 

 Lew Zong Han Owen (z.lew): declared the struct component
							100%
*//*___________________________________________________________________________-*/
#pragma once
struct UIComponent
{
	bool isUI;

	UIComponent() : isUI(false) {}
};
