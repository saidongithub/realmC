#include <doge/doge.h>
#include <doge/graphics.h>
#include <stdio.h>
#include <math.h>
#include "asset.h"

asset* asset_load(const char* filename){
    //load image
    doge_image_t* image;
    image = doge_image_load(filename);
    if(!image){
        printf("Failed loading img\n");
        return nullptr;
    }

    //get memory for new asset
    asset* newasset;
    newasset = (asset*)malloc(sizeof(asset));
    if(!newasset){
        doge_image_free(image);
        printf("Failed to malloc\n");
        return nullptr;
    }
    //make new asset
    newasset -> image = image;
    newasset -> width = doge_image_width(image);
    newasset -> height = doge_image_height(image);

    return newasset;
}

void asset_free(asset* asset){
    doge_image_free(asset -> image);

    free(asset);
}