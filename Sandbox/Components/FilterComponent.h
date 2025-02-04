#pragma once
struct FilterComponent
{
	bool isFilter;
	bool isFilterClogged;
	FilterComponent() : isFilter(false), isFilterClogged(false) {}
};