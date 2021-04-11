#pragma once
#include <doge/graphics.h>

typedef struct asset_s{
    doge_image_t* image;
    int width;
    int height;
} asset;

asset* asset_load(const char* filename);

void asset_free(asset* asset);