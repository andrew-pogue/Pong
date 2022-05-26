#pragma once
#include <allegro5/allegro.h>
#include <array>

class Keyboard {
public:

    Keyboard();

    void update(const ALLEGRO_EVENT &event);
    bool operator[](unsigned char keycode) const;
    void set_key_seen(unsigned char keycode);
    void set_all_keys_seen();

private:

    void set_key_pressed(unsigned char keycode);
    void set_key_released(unsigned char keycode);

    /**
     * Chars in this array should only be one of these 4 values:
     * 00000011 ----> key pressed,      not released,    not seen
     * 00000001 ----> key pressed,      not released,    seen
     * 00000010 ----> key pressed,      released,        not seen
     * 00000000 ----> no input to process
     * 
     * "seen" ------> key has been accounted for by an input handler since being pressed
     * "pressed" ---> key was activated since last frame
     * "released" --> key was de-activated since last frame
     **/
    std::array<unsigned char, ALLEGRO_KEY_MAX> keylog_;

};