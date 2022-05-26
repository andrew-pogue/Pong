#include "keyboard.hh"

static const unsigned char
        KEY_PRESSED  = 1,
        KEY_RELEASED = 2;

Keyboard::Keyboard()
    : keylog_{}
{
}

void Keyboard::update(const ALLEGRO_EVENT &event) {
    switch (event.type) {
    case ALLEGRO_EVENT_KEY_DOWN:
        this->set_key_pressed((unsigned char)event.keyboard.keycode);
        break;
    case ALLEGRO_EVENT_KEY_UP:
        this->set_key_released((unsigned char)event.keyboard.keycode);
        break;
    }
}

bool Keyboard::operator[](unsigned char keycode) const {
    return this->keylog_[keycode];
}
    
void Keyboard::set_key_pressed(unsigned char keycode) {
    this->keylog_[keycode] = KEY_PRESSED | KEY_RELEASED;
}

void Keyboard::set_key_released(unsigned char keycode) {
    this->keylog_[keycode] &= KEY_RELEASED;
}

void Keyboard::set_key_seen(unsigned char keycode) {
    this->keylog_[keycode] &= KEY_PRESSED;
}

void Keyboard::set_all_keys_seen() {
    for(int i = 1; i < ALLEGRO_KEY_MAX; i++) {
        this->set_key_seen((unsigned char)i);
    }
}