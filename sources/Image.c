//
//  Image.c
//  Inga
//
//  Created by Timo Kloss on 25/04/2020.
//  Copyright © 2020 Inutilis Software. All rights reserved.
//

#include "Image.h"

struct IBMColor {
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

struct Image *LoadImageIBM(const char *filename, SDL_Renderer *renderer, SDL_Palette *defaultPalette, bool createMask) {
    struct Image *image = NULL;
    
    SDL_RWops *file = SDL_RWFromFile(filename, "rb");
    if (!file) {
        printf("LoadImageIBM: %s\n", SDL_GetError());
    } else {
        Uint32 head = SDL_ReadBE32(file);
        if (head != 0x49424D38) {
            printf("LoadImageIBM: Invalid file format\n");
        } else {
            Uint16 width = SDL_ReadBE16(file);
            Uint16 height = SDL_ReadBE16(file);
            Uint8 depth = SDL_ReadU8(file);
            SDL_ReadU8(file);
            Uint16 bytesPerRow = SDL_ReadBE16(file);
            Uint8 flags = SDL_ReadU8(file);
            SDL_ReadU8(file);
            SDL_ReadBE32(file);
            SDL_ReadBE32(file);
            
            struct IBMColor *ibmColors = NULL;
            if (flags & 1) {
                // color palette
                ibmColors = calloc(256, sizeof(struct IBMColor));
                if (ibmColors) {
                    SDL_RWread(file, ibmColors, sizeof(struct IBMColor), 256);
                } else {
                    SDL_RWseek(file, sizeof(struct IBMColor) * 256, RW_SEEK_CUR);
                }
            }
            if (flags & 2) {
                // animation
                Uint16 numFrames = SDL_ReadBE16(file);
                Uint16 direction = SDL_ReadBE16(file);
                Uint16 frameWidth = SDL_ReadBE16(file);
                Uint16 frameHeight = SDL_ReadBE16(file);
                Uint16 pivotX = SDL_ReadBE16(file);
                Uint16 pivotY = SDL_ReadBE16(file);
                Uint16 pausesPerFrame = SDL_ReadBE16(file);
                SDL_ReadBE32(file);
                SDL_ReadBE32(file);
            }
            
            size_t size = bytesPerRow * height;
            Uint8 *pixels = calloc(sizeof(Uint8), size);
            if (!pixels) {
                printf("LoadImageIBM: Out of memory\n");
            } else {
                SDL_RWread(file, pixels, sizeof(Uint8), size);
                SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, bytesPerRow, SDL_PIXELFORMAT_INDEX8);
                if (!surface) {
                    printf("LoadImageIBM: Create surface failed\n");
                } else {
                    SDL_Palette *surfacePalette = surface->format->palette;
                    SDL_Palette *copiedPalette = NULL;
                    if (ibmColors) {
                        SDL_Color colors[surfacePalette->ncolors];
                        for (int i = 0; i < surfacePalette->ncolors; i++) {
                            SDL_Color *sdlColor = &colors[i];
                            struct IBMColor *ibmColor = &ibmColors[i];
                            sdlColor->r = ibmColor->r;
                            sdlColor->g = ibmColor->g;
                            sdlColor->b = ibmColor->b;
                            sdlColor->a = 255;
                        }
                        SDL_SetPaletteColors(surfacePalette, colors, 0, surfacePalette->ncolors);
                        copiedPalette = SDL_AllocPalette(surfacePalette->ncolors);
                        if (copiedPalette) {
                            SDL_SetPaletteColors(copiedPalette, colors, 0, surfacePalette->ncolors);
                        }
                    } else if (defaultPalette) {
                        SDL_SetPaletteColors(surfacePalette, defaultPalette->colors, 0, surfacePalette->ncolors);
                    }
                    if (createMask) {
                        SDL_Color transparentColor = {255, 0, 255, 255};
                        SDL_SetPaletteColors(surfacePalette, &transparentColor, 0, 1);
                        Uint32 key = SDL_MapRGB(surface->format, 255, 0, 255);
                        SDL_SetColorKey(surface, SDL_TRUE, key);
                    }
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                    if (!texture) {
                        printf("LoadImageIBM: Create texture failed\n");
                    } else {
                        image = calloc(1, sizeof(struct Image));
                        image->texture = texture;
                        image->palette = copiedPalette;
                        image->width = width;
                        image->height = height;
                    }
                    SDL_FreeSurface(surface);
                }
                free(pixels);
            }
            if (ibmColors) {
                free(ibmColors);
            }
        }
        SDL_RWclose(file);
    }
    return image;
}

void FreeImage(struct Image *image) {
    if (!image) return;
    SDL_DestroyTexture(image->texture);
    SDL_FreePalette(image->palette);
    free(image);
}

void DrawImage(struct Image *image, SDL_Renderer *renderer, int x, int y) {
    if (!image) return;
    SDL_Rect src = {0, 0, image->width, image->height};
    SDL_Rect dst = {x, y, image->width, image->height};
    SDL_RenderCopy(renderer, image->texture, &src, &dst);
}
