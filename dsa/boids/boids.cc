//
// Boids 5/9/2025
//
// Behavior rules are separated into different functions to make them easier to understand.
// Because of this, this demo is slow. The sparsely allocated linked list doesn't help either.
//
// I might write a fast 3D version some day.
//
// ref: https://cs.stanford.edu/people/eroberts/courses/soco/projects/2008-09/modeling-natural-systems/boids.html
// ref: https://vergenet.net/~conrad/boids/pseudocode.html
//

#include "common_core.hh"
#include "common_math.hh"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

struct Boid {
  Vec2 pos;
  Vec2 vel;

  Boid *next;
};

enum {
  BTN_SPAWN       = 1 << 0,
  BTN_SPAWN_MULTI = 1 << 1,
};

static struct {
  SDL_Window *wnd;
  SDL_Renderer *r;
  XorShift rng;
  Boid *boids;
  u8 buttons;
  f32 next_spawn_time;
  // Boid behavior settings
  f32 boid_speed;
  f32 boid_vision;
  f32 boid_avoid;
  f32 boid_weight_cohesion;
  f32 boid_weight_separation;
  f32 boid_weight_alignment;
  Vec2 boid_target;
} g = { };

static void SpawnBoid(Vec2 pos)
{
  Boid *boid = MemAlloc<Boid>();
  boid->pos = pos;
  boid->vel.x = g.rng.NextF32(-1.0f, 1.0f);
  boid->vel.y = g.rng.NextF32(-1.0f, 1.0f);
  boid->vel = boid->vel.Normalize() * g.boid_speed;
  
  boid->next = g.boids;
  g.boids = boid;
}

static Vec2 SimulateBoid_CalcRule1(Boid *boid)
{
  // Rule 1: Cohesion
  // Boids try to fly towards the center of mass of neighboring boids
  Vec2 result = Vec2();

  size_t num_neighbors = 0;
  for (Boid *neighbor = g.boids; neighbor; neighbor = neighbor->next) {
    const f32 distance = (boid->pos - neighbor->pos).Length();
    if (neighbor == boid || distance > g.boid_vision) {
      continue;
    }
    result += neighbor->pos;
    ++num_neighbors;
  }

  if (num_neighbors > 0) {
    result /= (f32)num_neighbors;
    result -= boid->pos;
  }

  return result * g.boid_weight_cohesion;
}

static Vec2 SimulateBoid_CalcRule2(Boid *boid)
{
  // Rule 2: Separation
  // Boids try to keep a small distance away from other boids
  Vec2 result = Vec2();

  for (Boid *neighbor = g.boids; neighbor; neighbor = neighbor->next) {
    const f32 distance = (boid->pos - neighbor->pos).Length();
    if (neighbor == boid || distance > g.boid_avoid) {
      continue;
    }
    result -= (neighbor->pos - boid->pos);
  }

  return result * g.boid_weight_separation;
}

static Vec2 SimulateBoid_CalcRule3(Boid *boid)
{
  // Rule 3: Alignment
  // Boids try to match velocity with neighboring boids
  Vec2 result = Vec2();

  size_t num_neighbors = 0;
  for (Boid *neighbor = g.boids; neighbor; neighbor = neighbor->next) {
    const f32 distance = (boid->pos - neighbor->pos).Length();
    if (neighbor == boid || distance > g.boid_vision) {
      continue;
    }
    result += neighbor->vel;
    ++num_neighbors;
  }

  if (num_neighbors > 0) {
    result /= (f32)num_neighbors;
    result -= boid->vel;
  }

  return result * g.boid_weight_alignment;
}

static void SimulateBoid(Boid *boid, f32 step)
{
  Vec2 rule1 = SimulateBoid_CalcRule1(boid);
  Vec2 rule2 = SimulateBoid_CalcRule2(boid);
  Vec2 rule3 = SimulateBoid_CalcRule3(boid);

  // Track towards target
  Vec2 track = (g.boid_target - boid->pos) * 0.0001;

  boid->vel += rule1 + rule2 + rule3 + track;

  // Limit speed
  const f32 speed = boid->vel.Length();
  if (speed > g.boid_speed) {
    boid->vel = boid->vel.Normalize() * g.boid_speed;
  }

  boid->pos += boid->vel * step;
}

static void RenderBoid(Boid *boid)
{
  SDL_FRect body = {
    .x = boid->pos.x - 2,
    .y = boid->pos.y - 2,
    .w = 4,
    .h = 4,
  };
  SDL_RenderFillRect(g.r, &body);

  const Vec2 flag = boid->pos + (boid->vel * 4.0f);
  SDL_RenderLine(g.r, boid->pos.x, boid->pos.y, flag.x, flag.y);
}

SDL_AppResult SDL_AppInit(void **, int, char **)
{
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    panic("Failed to initialize SDL: %s", SDL_GetError());
  }
  if (!(g.wnd = SDL_CreateWindow("boids", 1024, 768, 0))) {
    panic("Failed to create SDL window: %s", SDL_GetError());
  }
  if (!(g.r = SDL_CreateRenderer(g.wnd, 0))) {
    panic("Failed to create SDL renderer: %s", SDL_GetError());
  }
  g.boid_speed = 10.0f;
  g.boid_vision = 128.0f;
  g.boid_avoid = 20.0f;
  g.boid_weight_cohesion = 0.0001f;
  g.boid_weight_separation = 0.001f;
  g.boid_weight_alignment = 0.001f;
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *)
{
  const f32 now = (f32)SDL_GetTicks() / 1e3f;

  if (g.buttons & BTN_SPAWN && now >= g.next_spawn_time) {
    Vec2 pos = Vec2();
    SDL_GetMouseState(&pos.x, &pos.y);
    SpawnBoid(pos);

    g.next_spawn_time = now + (1.0f / 30.0f);
  
    if (!(g.buttons & BTN_SPAWN_MULTI)) {
      g.buttons &= ~BTN_SPAWN;
    }
  }

  SDL_GetMouseState(&g.boid_target.x, &g.boid_target.y);

  SDL_SetRenderDrawColor(g.r, 0x00, 0x20, 0x20, 0xFF);
  SDL_RenderClear(g.r);

  SDL_SetRenderDrawColor(g.r, 0xFF, 0xFF, 0x00, 0xFF);
  for (Boid *boid = g.boids; boid; boid = boid->next) {
    SimulateBoid(boid, 1.0f / 60.0f);
    RenderBoid(boid);
  }

  SDL_SetRenderDrawColor(g.r, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderDebugText(g.r, 5.0f,  5.0f, "[boids]");
  SDL_RenderDebugText(g.r, 5.0f, 15.0f, "  M1       Spawn");
  SDL_RenderDebugText(g.r, 5.0f, 25.0f, "  Shift+M1 Spawn continuously");
  SDL_RenderDebugText(g.r, 5.0f, 35.0f, "  D        Clear");

  u32 boid_count = 0;
  for (Boid *boid = g.boids; boid; boid = boid->next) {
    ++boid_count;
  }
  SDL_RenderDebugTextFormat(g.r, 5.0f, 55.0f, "  Count: %d", boid_count);

  SDL_RenderPresent(g.r);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *, SDL_Event *event)
{
  switch (event->type) {
    case SDL_EVENT_QUIT: {
      return SDL_APP_SUCCESS;
    } break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
      if (event->button.button == SDL_BUTTON_LEFT) {
        g.buttons |= BTN_SPAWN;
      }
    } break;
    case SDL_EVENT_KEY_DOWN: {
      if (event->key.key == SDLK_LSHIFT) {
        g.buttons |= BTN_SPAWN_MULTI;
      }
      else if (event->key.key == SDLK_D) {
        Boid *next_boid = 0;
        for (Boid *boid = g.boids; boid; boid = next_boid) {
          next_boid = boid->next;
          MemFree(boid);
        }
        g.boids = 0;
      }
    } break;
    case SDL_EVENT_KEY_UP: {
      if (event->key.key == SDLK_LSHIFT) {
        g.buttons &= ~BTN_SPAWN_MULTI;
      }
    } break;
  };
  return SDL_APP_CONTINUE;
}

void SDLCALL SDL_AppQuit(void *, SDL_AppResult)
{
  SDL_DestroyWindow(g.wnd);
  SDL_Quit();
}
