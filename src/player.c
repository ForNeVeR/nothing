#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "./player.h"
#include "./platforms.h"
#include "./point.h"

#define PLAYER_WIDTH 50.0f
#define PLAYER_HEIGHT 50.0f
#define PLAYER_SPEED 500.0f
#define PLAYER_GRAVITY 1500.0f

struct player_t {
    vec_t position;
    vec_t velocity;
    vec_t movement;
};

player_t *create_player(float x, float y)
{
    player_t *player = malloc(sizeof(player_t));

    if (player == NULL) {
        return NULL;
    }

    player->position.x = x;
    player->position.y = y;
    player->velocity.x = 0.0f;
    player->velocity.y = 0.0f;
    player->movement.x = 0.0f;
    player->movement.y = 0.0f;

    return player;
}

void destroy_player(player_t * player)
{
    free(player);
}

rect_t player_hitbox(const player_t *player)
{
    rect_t hitbox = {
        .x = player->position.x,
        .y = player->position.y,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };

    return hitbox;
}

int render_player(const player_t * player,
                  SDL_Renderer *renderer,
                  const camera_t *camera)
{
    assert(player);
    assert(renderer);
    assert(camera);

    if (SDL_SetRenderDrawColor(renderer, 96, 255, 96, 255) < 0) {
        return -1;
    }
    rect_t player_object = {
        .x = player->position.x,
        .y = player->position.y,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };


    return camera_fill_rect(camera, renderer, &player_object);
}

void update_player(player_t *player,
                   const platforms_t *platforms,
                   Uint32 delta_time)
{
    assert(player);
    assert(platforms);

    float d = (float) delta_time / 1000.0f;

    player->velocity.y += PLAYER_GRAVITY * d;
    player->position = vec_sum(
        player->position,
        vec_scala_mult(
            vec_sum(
                player->velocity,
                player->movement),
            d));
    player->position.y = fmodf(player->position.y, 800.0f);

    vec_t opposing_force = platforms_rect_object_collide(
        platforms,
        player_hitbox(player));

    while (vec_length(opposing_force) > 1e-6) {
        player->position = vec_sum(
            player->position,
            vec_scala_mult(
                opposing_force,
                1e-2f));

        if (fabs(opposing_force.x) > 1e-6 && (opposing_force.x < 0.0f) != ((player->velocity.x + player->movement.x) < 0.0f)) {
            player->velocity.x = 0.0f;
            player->movement.x = 0.0f;
        }

        if (fabs(opposing_force.y) > 1e-6 && (opposing_force.y < 0.0f) != ((player->velocity.y + player->movement.y) < 0.0f)) {
            player->velocity.y = 0.0f;
            player->movement.y = 0.0f;
        }

        opposing_force = platforms_rect_object_collide(
            platforms,
            player_hitbox(player));
    }
}

void player_move_left(player_t *player)
{
    assert(player);

    player->movement.x = -PLAYER_SPEED;
    player->movement.y = 0.0f;
}

void player_move_right(player_t *player)
{
    assert(player);

    player->movement.x = PLAYER_SPEED;
    player->movement.y = 0.0f;
}

void player_stop(player_t *player)
{
    assert(player);

    player->movement.x = 0.0f;
    player->movement.y = 0.0f;
}

void player_jump(player_t *player)
{
    assert(player);

    player->velocity.y = -500.0f;
}

void player_focus_camera(player_t *player,
                         camera_t *camera)
{
    assert(player);
    assert(camera);

    camera_center_at(
        camera,
        vec_sum(
            player->position,
            vec(PLAYER_WIDTH * 0.5f, PLAYER_HEIGHT * 0.5f)));
}
