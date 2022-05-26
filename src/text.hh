#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

struct Text {

    void draw() const {
        al_draw_text(font, color, x, y, flags, string);
    }

    float x = 0.0f, y = 0.0f;
    const char *string = "";
    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_COLOR color = al_map_rgb(255,255,255);
    int flags = 0;
};