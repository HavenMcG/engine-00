#include "TextureStore.h"

int TextureStore::store(TextureData td) {
    for (int i = 0; i < MAX_STORED_TEXTURES; ++i) {
        if (!statuses_[i]) {
            stored_textures_[i] = td;
            return i;
        }
    }
    return -1;
}

void TextureStore::drop(int index_to_delete) {
    statuses_[index_to_delete] = false;
}

TextureStore::TextureStore() {
    stored_textures_ = new TextureData[MAX_STORED_TEXTURES];
    statuses_ = new bool[MAX_STORED_TEXTURES];
}

TextureStore::~TextureStore() {
    delete[] stored_textures_;
    delete[] statuses_;
}