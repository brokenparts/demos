#include "common_core.hh"
#include "common_math.hh"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

struct Boid {
  Vec2 pos;
  f32  ang;
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
  // Boid behavior
  f32 boid_vision;
  f32 boid_speed;
  f32 boid_avoid;
  Vec2 boid_target;
} g = { };

static void SpawnBoid(Vec2 pos)
{
  Boid *boid = MemAlloc<Boid>();
  boid->pos = pos;
  boid->ang = g.rng.NextF32(0.0f, 360.0f);
  boid->next = g.boids;
  g.boids = boid;
}

static void SimulateBoid(Boid* boid, f32 step)
{
  // Running averages
  Vec2 avg_pos = Vec2();
  Vec2 avg_vel = Vec2();
  f32  avg_ang = 0.0f;
  usize num_neighbors = 0;
  Vec2 vel_sep = Vec2();
  for (Boid *neighbor = g.boids; neighbor; neighbor = neighbor->next) {
    const f32 dist = (boid->pos - neighbor->pos).Length();
    if (neighbor == boid || dist > g.boid_vision) {
      continue;
    }
    // Separation
    if (dist < g.boid_avoid) {
      vel_sep += (boid->pos - neighbor->pos);
    }
    avg_pos += neighbor->pos;
    avg_ang += neighbor->ang;
    ++num_neighbors;
  }

  if (num_neighbors > 0) {
    avg_pos /= (f32)num_neighbors;
    avg_ang /= (f32)num_neighbors;

    Vec2 vel = Vec2::FromEuler(boid->ang);

    // Separation
    Vec2 vel_s = vel_sep * 0.0005;

    // Alignment
    Vec2 vel_a = (Vec2::FromEuler(avg_ang) - vel) * 0.001;

    // Cohesion
    Vec2 vel_c = (avg_pos - boid->pos) * 0.000003;

    // Track towards target
    Vec2 vel_t = (g.boid_target - boid->pos) * 0.00005;

    boid->ang = (vel + vel_s + vel_a + vel_c + vel_t).ToEuler();
  }

  boid->pos += Vec2::FromEuler(boid->ang) * step * g.boid_speed;
}

static void RenderBoid(Boid* boid)
{
  SDL_FRect body = {
    .x = boid->pos.x - 2,
    .y = boid->pos.y - 2,
    .w = 4,
    .h = 4,
  };
  SDL_RenderFillRect(g.r, &body);

  Vec2 dir = Vec2::FromEuler(boid->ang);
  Vec2 flag = boid->pos + (dir * 10.0f);
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
  g.boid_vision = 250.0f;
  g.boid_speed = 15.0f;
  g.boid_avoid = 20.0f;
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

  int wnd_x = 0;
  int wnd_y = 0;
  SDL_GetRenderOutputSize(g.r, &wnd_x, &wnd_y);
  g.boid_target.x = (f32)wnd_x / 2.0f;
  g.boid_target.y = (f32)wnd_y / 2.0f;

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
