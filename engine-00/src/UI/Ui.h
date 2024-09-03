#pragma once
#include <list>

struct UiId {
	int owner;
	int item;
	int index;
};

class UiContext {
	UiId hot;
	UiId active;
};

bool button();