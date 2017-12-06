#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "./player.h"
#include "./platforms.h"

#define PLAYER_WIDTH 50.0f
#define PLAYER_HEIGHT 50.0f
#define PLAYER_SPEED 500.0f
#define PLAYER_GRAVITY 1500.0f

struct player_t {
    point_t position;
    point_t velocity;
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

    return player;
}

void destroy_player(player_t * player)
{
    free(player);
}

int render_player(const player_t * player,
                  SDL_Renderer *renderer,
                  const camera_t *camera)
{
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

void update_player(player_t * player,
                   const platforms_t *platforms,
                   Uint32 delta_time)
{
    float d = (float) delta_time / 1000.0f;

    float dx = player->velocity.x;
    float dy = player->velocity.y + PLAYER_GRAVITY * d;

    float x = player->position.x + dx * d;
    float y = fmodf(player->position.y + dy * d, 600.0f);

    rect_t player_object = {
        .x = x,
        .y = y,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };

    /* TODO(#6): Implement collision for the left/right sides */
    if (platforms_rect_object_collide(platforms, &player_object)) {
        dy = 0.0f;//-player->dy * 0.75f;
        x = player->position.x + dx * d;
        y = fmodf(player->position.y + dy * d, 600.0f);
    }

    player->velocity.x = dx;
    player->velocity.y = dy;
    player->position.x = x;
    player->position.y = y;
}

void player_move_left(player_t *player)
{
    player->velocity.x = -PLAYER_SPEED;
}

void player_move_right(player_t *player)
{
    player->velocity.x = PLAYER_SPEED;
}

void player_stop(player_t *player)
{
    player->velocity.x = 0.0f;
}

void player_jump(player_t *player)
{
    player->velocity.y = -1000.0f;
}

void player_focus_camera(player_t *player,
                         camera_t *camera)
{
    camera_translate(camera,
                     player->position.x - 800.0f * 0.5f + PLAYER_WIDTH * 0.5f,
                     player->position.y - 600.0f * 0.5f + PLAYER_HEIGHT * 0.5f);
}
