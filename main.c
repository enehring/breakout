#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#define TITLE "Breakout"
#define DEFAULT_WINDOW_HEIGHT 800
#define DEFAULT_WINDOW_WIDTH 800
#define BLOCK_COUNT 3

int _window_height = DEFAULT_WINDOW_HEIGHT;
int _window_width = DEFAULT_WINDOW_WIDTH;

// TODO: put the game "state" in a struct
SDL_Rect paddle = {.x = 0, .y = DEFAULT_WINDOW_HEIGHT - 25, .h = 25, .w = 200};
SDL_Rect projectile = {.x = 0, .y = 20, .h = 50, .w = 50};
int x_direction = 1;
int y_direction = 1;

static void update_paddle_pos(SDL_Rect * paddle, int offset) {
  int new_position = paddle->x + offset;

  if (new_position + paddle->w <= _window_width && new_position >= 0) {
    paddle->x = new_position;
  }
}

static void update_projectile_pos(
  SDL_Rect * projectile, SDL_Rect * paddle, int * x, int * y) {
  int projectile_right_edge = projectile->x + projectile->w;
  int projectile_bottom_edge = projectile->y + projectile->h;
  int projectile_midpoint = (projectile->x + (projectile->x + projectile->w)) / 2;
  int paddle_midpoint = (paddle->x + (paddle->x + paddle->w)) / 2;

  if (projectile->x == 0) {
    *x = 1;
  }
  else if (projectile_right_edge == _window_width) {
    *x = -1;
  }

  if (projectile->y == 0) {
    *y = 1;
  }
  else if (projectile_bottom_edge == _window_height) {
    *y = -1; // TODO: losing condition
  }

  // paddle collision
  if (projectile_bottom_edge >= paddle->y &&
      projectile_right_edge >= paddle->x && projectile->x <= (paddle->x + paddle->w))
  {
    *y = -1;

    if (projectile_midpoint <= paddle_midpoint) {
      *x = -1;
    }
    else if (projectile_midpoint > paddle_midpoint) {
      *x = 1;
    }
  }
  else if (0) { // TODO: target collision
    
  }

  projectile->x += *x;
  projectile->y += *y;
}

static void handle_key_event(SDL_KeyboardEvent * key_event, SDL_Rect * paddle) {
  switch (key_event->keysym.sym) {
  case SDLK_d:
  case SDLK_RIGHT:
    update_paddle_pos(paddle, 10);
    break;
  case SDLK_a:
  case SDLK_LEFT:
    update_paddle_pos(paddle, -10);
    break;
  }
}

int frame_counter = 0;
int projectile_frame_delay = 25;

static int continue_game(SDL_Window * window, SDL_Renderer * renderer) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      return 0;
    case SDL_KEYDOWN:
      handle_key_event(&(event.key), &paddle);
      break;
    }
  }
  
  SDL_RenderClear(renderer);
  SDL_GL_GetDrawableSize(window, &_window_width, &_window_height);

  frame_counter += 1;

  if (frame_counter == projectile_frame_delay) {
    update_projectile_pos(&projectile, &paddle, &x_direction, &y_direction);
    frame_counter = 0;
  }

  // TODO: draw targets

    // drow projectile
  if (SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0) < 0) {
    SDL_Log("Failed to set draw color for projectile: %s", SDL_GetError());
  }   

  if (SDL_RenderDrawRect(renderer, &projectile) < 0) {
    SDL_Log("Failed to draw projectile: %s", SDL_GetError());
  }

  if (SDL_RenderFillRect(renderer, &projectile) < 0) {
    SDL_Log("Failed to fill projectile: %s", SDL_GetError());
  }

  // draw paddle
  if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0)) {
    SDL_Log("Failed to set draw color for paddle: %s", SDL_GetError());
  }

  if (SDL_RenderDrawRect(renderer, &paddle) < 0) {
    SDL_Log("Failed to draw paddle: %s", SDL_GetError());
  }

  if (SDL_RenderFillRect(renderer, &paddle) < 0) {
    SDL_Log("Failed to fill paddle: %s", SDL_GetError());
  }
  
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderPresent(renderer);
  
  return 1;
}

static SDL_Window * init_window() {
  Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;

  return SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                          _window_width, _window_height, flags);
}

int main(int argc, char ** argv) {
  SDL_Window * window;
  SDL_Renderer * renderer;
  int error = 0;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("Could not initialize SDL: %s", SDL_GetError());
    error = 1;
    goto QUIT;
  }

  window = init_window();

  if (!window) {
    SDL_Log("Could not create SDL window: %s", SDL_GetError());
    error = 1;
    goto QUIT_VIDEO;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);

  if (!renderer) {
    SDL_Log("Could not create SDL renderer: %s", SDL_GetError());
    error = 1;
    goto CLEANUP_WINDOW;
  }

  // game loop
  while (continue_game(window, renderer)) { }

  SDL_DestroyRenderer(renderer);
  CLEANUP_WINDOW:
  SDL_DestroyWindow(window);
  QUIT_VIDEO:
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  QUIT:
  SDL_Quit();

  return error;
}

