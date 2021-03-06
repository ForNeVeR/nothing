#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "./platforms.h"

struct platforms_t {
    rect_t *rects;
    size_t rects_size;
};

static const vec_t opposing_rect_side_forces[RECT_SIDE_N] = {
    { .x = 1.0f,  .y =  0.0f  },  /* RECT_SIDE_LEFT = 0, */
    { .x = -1.0f, .y =  0.0f  },  /* RECT_SIDE_RIGHT, */
    { .x = 0.0f,  .y =  1.0f, },  /* RECT_SIDE_TOP, */
    { .x = 0.0f,  .y = -1.0f,  }    /* RECT_SIDE_BOTTOM, */
};

platforms_t *create_platforms(const rect_t *rects, size_t rects_size)
{
    assert(rects);

    platforms_t *platforms = malloc(sizeof(platforms_t));

    if (platforms == NULL) {
        goto platforms_malloc_fail;
    }

    platforms->rects = malloc(sizeof(rect_t) * rects_size);

    if (platforms->rects == NULL) {
        goto platforms_rects_malloc_fail;
    }

    platforms->rects = memcpy(platforms->rects, rects, sizeof(rect_t) * rects_size);
    platforms->rects_size = rects_size;

    return platforms;

platforms_rects_malloc_fail:
    free(platforms);
platforms_malloc_fail:
    return NULL;
}

void destroy_platforms(platforms_t *platforms)
{
    assert(platforms);

    free(platforms->rects);
    free(platforms);
}

int render_platforms(const platforms_t *platforms,
                     SDL_Renderer *renderer,
                     const camera_t *camera)
{
    if (SDL_SetRenderDrawColor(renderer, 255, 96, 96, 255) < 0) {
        return -1;
    }

    for (size_t i = 0; i < platforms->rects_size; ++i) {
        if (camera_fill_rect(camera, renderer, &platforms->rects[i]) < 0) {
            return -1;
        }
    }

    return 0;
}

vec_t platforms_rect_object_collide(const platforms_t *platforms,
                                    rect_t object)
{
    assert(platforms);

    int sides[4];
    memset(sides, 0, sizeof(sides));

    for (size_t i = 0; i < platforms->rects_size; ++i) {
        rect_object_impact(&object, &platforms->rects[i], sides);
    }

    vec_t opposing_force = {
        .x = 0.0f,
        .y = 0.0f
    };

    for (rect_side_t side = 0; side < RECT_SIDE_N; ++side) {
        if (sides[side]) {
            vec_add(
                &opposing_force,
                opposing_rect_side_forces[side]);
        }
    }

    return opposing_force;
}
