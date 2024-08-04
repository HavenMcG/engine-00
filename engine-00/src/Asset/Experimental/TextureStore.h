#pragma once
#include "../Data.h"
#include <functional>
#include <unordered_map>

class TextureStore {
public:
	static const int MAX_STORED_TEXTURES = 8;
	int store(TextureData);
	void drop(int);

	TextureStore();
	~TextureStore();
private:
	TextureData* stored_textures_;
	bool* statuses_;
};