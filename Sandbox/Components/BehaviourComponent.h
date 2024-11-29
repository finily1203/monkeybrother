#pragma once
struct BehaviourComponent
{
	bool none;
	bool player;
	bool enemy;
	bool pump;
	bool exit;
	bool collectable;
	bool button;
	bool platform;

	BehaviourComponent() : none(false), player(false), enemy(false), pump(false), exit(false), collectable(false), button(false), platform(false) {}
};