//
// Copyright (c) 2020 Timo Kloss
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef Font_h
#define Font_h

#include <stdio.h>
#include <SDL2/SDL.h>

#ifdef __APPLE__
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL2/SDL_ttf.h>
#endif

#include "Image.h"

typedef struct Font {
    TTF_Font *ttfFont;
} Font;

Font *LoadFont(const char *filename, int size);
void FreeFont(Font *font);
Image *CreateImageFromText(const char *text, Font *font, SDL_Color color);

#endif /* Font_h */
