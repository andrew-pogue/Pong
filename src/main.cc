#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <list>
#include <math.h>
#include <string>

#include "delayed_change.hh"
#include "keyboard.hh"
#include "line.hh"
#include "random.hh"
#include "rectangle.hh"
#include "text.hh"

void must_init(bool test, const char *description);
void paddle_redirect_ball(const Rectangle &paddle, const Rectangle &ball,
    float &dx, float &dy);

int main (int argc, char **argv) {
    // Allegro requires the main method to have the argc and argv parameters
    // To avoid the -Werror=unused-variable g++ compiler error:
    if (argv) argc += 0;

    must_init(al_init(), "Allegro");
    must_init(al_init_primitives_addon(), "primitives addon");
    must_init(al_init_font_addon(), "font addon");
    must_init(al_init_ttf_addon(), "ttf addon");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");
    
    const double FRAME_RATE = 1.0 / 120.0;
    auto frame_timer = al_create_timer(FRAME_RATE);
    
    auto *event_queue = al_create_event_queue();
    must_init(event_queue, "event_queue");

    ALLEGRO_FONT *font = al_load_font("./assets/font/Gamer.ttf", 60, 0);
    must_init(font, "font");
    
    int display_width = 650,
        display_height = 490;
    auto *display = al_create_display(display_width, display_height);
    must_init(display, "display");
    al_hide_mouse_cursor(display);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(frame_timer));

    ALLEGRO_EVENT event;
    Keyboard keyboard;
    RandomGenerator random;

    // Allows for delayed/queued changes to the ALLEGRO_COLOR variables.
    std::list<DelayedChange<ALLEGRO_COLOR>> delayed_color_change_list{};
    auto queue_color_change = 
        [&delayed_color_change_list](DelayedChange<ALLEGRO_COLOR> dc) {
            delayed_color_change_list.push_back(dc);
        };

    const ALLEGRO_COLOR
        WHITE = al_map_rgb(255,255,255),
        GREY = al_map_rgb(100,100,100),
        ORANGE = al_map_rgb(255,100,0);

    const float
        PADDLE_OFFSET = 20.0f,
        PADDLE_WIDTH = 8.0f,
        PADDLE_HEIGHT = 50.0f,
        BALL_RADIUS = 5.0f,
        BALL_SPEED = 3.6f,
        PADDLE_SPEED = 6.5f;

    bool play = true,
        reset = false,
        redraw = true,
        is_paddle_collidable = true;

    unsigned
        left_score = 0,
        right_score = 0;

    float ball_dx = 0.0f,
        ball_dy = 0.0f;

    Rectangle arena {
        (float)display_width / 2.0f - 640.0f / 2.0f,
        (float)display_height / 2.0f - 480.0f / 2.0f,
        640.0f,
        480.0f,
        GREY,
        false
    };
    
    Rectangle left_paddle {
        arena.x + PADDLE_OFFSET,
        arena.y + arena.h / 2.0f - PADDLE_HEIGHT / 2.0f,
        PADDLE_WIDTH,
        PADDLE_HEIGHT,
        WHITE
    };

    Rectangle right_paddle {
        arena.x + arena.w - PADDLE_OFFSET - PADDLE_WIDTH,
        arena.y + arena.h / 2.0f - PADDLE_HEIGHT / 2.0f,
        PADDLE_WIDTH,
        PADDLE_HEIGHT,
        WHITE
    };
    
    Rectangle ball {
        0.0f,
        0.0f,
        BALL_RADIUS * 2.0f,
        BALL_RADIUS * 2.0f,
        WHITE
    };

    Line divider {
        arena.x + arena.w / 2.0f,
        arena.y,
        arena.x + arena.w / 2.0f,
        arena.y + arena.h,
        GREY,
        3
    };

    Text left_score_text {
        arena.x + arena.w / 2.0f - 100.0f,
        arena.y + 30.0f,
        std::to_string(left_score).c_str(),
        font,
        GREY,
        ALLEGRO_ALIGN_LEFT
    };

    Text right_score_text {
        arena.x + arena.w / 2.0f + 100.0f,
        arena.y + 30.0f,
        std::to_string(right_score).c_str(),
        font,
        GREY,
        ALLEGRO_ALIGN_LEFT
    };

    // I don't want to write this code more than once, but also it needs several
    // parameters, so I'm making it a closure instead of a free method.
    auto ball_reset = [&]() {
        ball.x = arena.x + arena.w / 2.0f - ball.w;
        ball.y = arena.y + arena.h / 2.0f - ball.h;
        ball_dx = random(0.75f,1.0f) * BALL_SPEED * (float)(1 + random(0,2) * -2);
        ball_dy = (BALL_SPEED - std::abs(ball_dx)) * (float)(1 + random(0,2) * -2);
    };

    ball_reset();
    al_start_timer(frame_timer);
    while (play) {
        al_wait_for_event(event_queue, &event);
        
        keyboard.update(event);
        process_delayed_changes(delayed_color_change_list);
        
        switch (event.type) {
        case ALLEGRO_EVENT_TIMER:
            redraw = true;

            // To prevent ball from getting stuck between the paddle and
            // the arena border, the ball can't collide with the paddle
            // in the same frame it collides with the the arena's border
            is_paddle_collidable = true;

            // Handle User Input
            if (keyboard[ALLEGRO_KEY_ESCAPE]) {
                play = false;
            } if (keyboard[ALLEGRO_KEY_UP]) {
                right_paddle.y -= PADDLE_SPEED;
            } if (keyboard[ALLEGRO_KEY_DOWN]) {
                right_paddle.y += PADDLE_SPEED;
            } if (keyboard[ALLEGRO_KEY_W]) {
                left_paddle.y -= PADDLE_SPEED;
            } if (keyboard[ALLEGRO_KEY_S]) {
                left_paddle.y += PADDLE_SPEED;
            }
            keyboard.set_all_keys_seen();

            // Ensure the Paddles remain within the bounds of the Arena:
            if (right_paddle.y < arena.y) {
                right_paddle.y = arena.y;
            } if (left_paddle.y < arena.y) {
                left_paddle.y = arena.y;
            } if (right_paddle.y + right_paddle.h > arena.y + arena.h) {
                right_paddle.y = arena.y + arena.h - right_paddle.h;
            } if (left_paddle.y + left_paddle.h > arena.y + arena.h) {
                left_paddle.y = arena.y + arena.h - left_paddle.h;
            }

            // Ball Collision Detection: Arena Ceiling and Floor
            if (ball.y < arena.y) {
                ball.y = arena.y;
                ball_dy = -ball_dy;
                is_paddle_collidable = false;
            } else if (ball.y + ball.h > arena.y + arena.h) {
                ball.y = arena.y + arena.h - ball.h;
                ball_dy = -ball_dy;
                is_paddle_collidable = false;
            } 
            
            // Ball Collision Detection: Arena Left and Right Bounds
            if (ball.x < arena.x) {
                right_score++;
                right_score_text.color = ORANGE;
                queue_color_change({right_score_text.color, GREY});
                reset = true;
                is_paddle_collidable = false;
            } else if (ball.x + ball.w > arena.x + arena.w) {
                left_score++;
                left_score_text.color = ORANGE;
                queue_color_change({left_score_text.color, GREY});
                reset = true;
                is_paddle_collidable = false;
            }

            // Ball Collision Detection: Left Paddle
            if (is_paddle_collidable && ball.intersects(left_paddle)) {
                if (ball.x + ball.w / 2.0f < left_paddle.x + left_paddle.w) {
                    paddle_redirect_ball(left_paddle, ball, ball_dx, ball_dy);
                    left_paddle.color = ORANGE;
                    queue_color_change({left_paddle.color, WHITE});
                    ball.x = left_paddle.x + left_paddle.w;
                }
            }

            // Ball Collision Detection: Right Paddle
            if (is_paddle_collidable && ball.intersects(right_paddle)) {
                if (ball.x + ball.w / 2.0f > right_paddle.x) {
                    paddle_redirect_ball(right_paddle, ball, ball_dx, ball_dy);
                    right_paddle.color = ORANGE;
                    queue_color_change({right_paddle.color, WHITE});
                    ball.x = right_paddle.x - ball.h;
                }
            }

            left_score_text.string = std::to_string(left_score).c_str();
            right_score_text.string = std::to_string(right_score).c_str();

            ball.x += ball_dx;
            ball.y += ball_dy;
            break;
        // case ALLEGRO_EVENT_MOUSE_AXES:
        //     break;
        // case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
        //     // hide_cursor = false;
        //     break;
        // case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
        //     // hide_cursor = true;
        //     break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            play = false;
            break;
        }

        if (reset) {
            ball_reset();
            reset = false;
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            al_clear_to_color(al_map_rgb(0,0,0));
            
            arena.draw();
            right_paddle.draw();
            left_paddle.draw();
            ball.draw();
            divider.draw();
            left_score_text.draw();
            right_score_text.draw();

            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_display(display);
    al_destroy_timer(frame_timer);
    al_destroy_event_queue(event_queue);
    al_destroy_font(font);
    return 0;
}

void must_init(bool test, const char *description) {
    if(test) return;
    printf("Failed to initialize %s!\n", description);
    exit(1);
}

// The ball is redirected more-so (but not entirely) upwards when
// the ball hits the upper half of the paddle, more downwards when the ball
// hits the lower half of the paddle, and more along the y-axis when the ball hits
// the center of the paddle.
void paddle_redirect_ball(const Rectangle &paddle, const Rectangle &ball,
    float &dx, float &dy)
{
    dx = -dx;
    dx += dx/abs(dx) * 0.1f;
    float diff = (ball.y + ball.h / 2.0f) - (paddle.y + paddle.h / 2.0f),
        ry = abs(diff / paddle.h / 2.0f),
        rx = abs(1 - ry);
    if (diff < 0.0f) ry = -ry;
    if (dx < 0.0f) rx = -rx;
    dx += rx * abs(dy) - abs(ry) * dx;
    dy += ry * abs(dx) - abs(rx) * dy;
}