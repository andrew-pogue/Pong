#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

struct Rectangle {
    bool intersects(const Rectangle &other) const {
        float ox2 = other.x + other.w,
            x2 = x + w,
            oy2 = other.y + other.h,
            y2 = y + h;

        return ((other.x >= x && other.x <= x2)
            || (ox2 >= x && ox2 <= x2)
            || (other.x <= x2 && ox2 >= x))
            && ((other.y >= y && other.y <= y2)
            || (oy2 >= y && oy2 <= y2)
            || (other.y <= y2 && oy2 >= y));
    }

    void draw() const {
        if (is_filled) {
            al_draw_filled_rectangle(
                x,
                y,
                x + w,
                y + h,
                color
            );
        } else {
            al_draw_rectangle(
                x,
                y,
                x + w,
                y + h,
                color,
                5.0f
            );
        }
    }

    float x = 0.0f,
        y = 0.0f,
        w = 0.0f,
        h = 0.0f;
    ALLEGRO_COLOR color = al_map_rgb(255,255,255);
    bool is_filled = true;
};