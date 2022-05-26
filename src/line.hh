#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

struct Line {
    void draw() const {
        al_draw_line(x1, y1, x2, y2, color, thickness);
    }

    float x1, y1, x2, y2;
    ALLEGRO_COLOR color;
    float thickness;
};