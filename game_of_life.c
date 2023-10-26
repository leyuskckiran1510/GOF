#include "src/canvas.h"
#include "src/cli-engine.h"
#include "src/colors.h"
#include "src/event.h"
#include "src/shapes/shapes.h"
#include "src/utils/util.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define FPS 30

/*
  RULES:-
1 . Any live cell with fewer than two live neighbours dies, as if by
underpopulation.
2 .Any live cell with two or three live neighbours lives on to
the next generation.
3. Any live cell with more than three live neighbours dies, as if by
overpopulation.
4 .Any dead cell with exactly three live neighbours becomes a
live cell, as if by reproduction.

These rules, which compare the behaviour of the automaton to real life, can be
condensed into the following:

1 .Any live cell with two or three live neighbours survives.
2 .Any dead cell with three live neighbours becomes a live cell.
3. All other live cells die in the next generation. Similarly, all other dead
cells stay dead.




*/

Color st_col[] = {
    {.r = 0, .g = 0, .b = 0},       {.r = 255, .g = 100, .b = 100},
    {.r = 100, .g = 255, .b = 100}, {.r = 100, .g = 100, .b = 255},
    {.r = 255, .g = 255, .b = 100}, {.r = 250, .g = 250, .b = 255},
    {.r = 250, .g = 250, .b = 255}, {.r = 255, .g = 255, .b = 100},
    {.r = 100, .g = 100, .b = 255}, {.r = 100, .g = 255, .b = 100},
    {.r = 255, .g = 100, .b = 100}, {.r = 0, .g = 0, .b = 0},
};

int color_comp(Color c1, Color c2) {
  if (c1.r != c2.r || c1.g != c2.g || c1.b != c2.b) {
    return 0;
  }
  return 1;
}

void conway_gof(int *state, int width, int height) {
  int neighbours[8][2] = {{1, 1},  {0, 1},  {1, 0},  {-1, -1},
                          {-1, 0}, {0, -1}, {1, -1}, {-1, 1}};
  size_t size = sizeof(int) * width * height;
  int *new_state = malloc(size);
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      int alive = 0;
      for (size_t k = 0; k < sizeof(neighbours) / sizeof(neighbours[0]); k++) {
        if (col + neighbours[k][0] >= 0 && col + neighbours[k][0] < width &&
            row + neighbours[k][1] >= 0 && row + neighbours[k][1] < height) {
          if (state[(row + neighbours[k][1]) * width + col + neighbours[k][0]])
            alive++;
        }
      }
      switch (alive) {
      case 3: {
        new_state[row * width + col] = 1;
        break;
      }
      case 2: {
        new_state[row * width + col] = state[row * width + col];
        break;
      }
      default:
        new_state[row * width + col] = 0;
        break;
      }
    }
  }
  memcpy(state, new_state, size);
  free(new_state);
}

int primes[] = {2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,
                43,  47,  53,  59,  61,  67,  71,  73,  79,  83,  89,  97,  101,
                103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
                173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239,
                241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313,
                317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
                401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467,
                479, 487, 491, 499, 503, 509, 521, 523, 541};

void fill_random(int *state, int width, int height) {
  // int x = 10, y = 11;
  // (void)height;
  // // state[(y)*width + (x)] = 1;
  // // state[(y)*width + (x + 2)] = 1;
  // // state[(y + 1) * width + (x + 1)] = 1;
  // // state[(y - 1) * width + (x + 1)] = 1;

  // state[(y)*width + (x)] = 1;
  // state[(y)*width + (x + 1)] = 1;
  // state[(y + 1) * width + (x)] = 1;
  // state[(y + 1) * width + (x + 1)] = 1;
  int max = sizeof(primes) / sizeof(int);
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      // if()
      int x = primes[col % max] % width;
      int y = primes[row % max] % height;
      state[row * width + x] = 1;
    }
  }

  // for(int i=0;i<max;i++){
  //   int cord = primes[i];
  //   int cordx = cord%width;
  //   int cordy = cord%height;
  //   state[cordy*width+cordx]=0;
  // }
}

void inverse_state(int *state, int width, int height) {
  int neighbours[][2] = {{1, 1},{-1, 0}, {0, -1},  {-1, 1},  {0, 1},  {1, 0},};//{-1, -1},{1, -1},};
  size_t size = width*height*sizeof(int);
  int *new_state = calloc(sizeof(int), width* height);;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (state[row * width + col]) {
        for (size_t k = 0; k < sizeof(neighbours) / sizeof(neighbours[0]);
             k++) {
          if (col + neighbours[k][0] >= 0 && col + neighbours[k][0] < width &&
              row + neighbours[k][1] >= 0 && row + neighbours[k][1] < height) {
            if (state[(row + neighbours[k][1]) * width + col +
                      neighbours[k][0]])
              new_state[(row + neighbours[k][1]) * width + col +
                        neighbours[k][0]] = 0;
            else {
              new_state[(row + neighbours[k][1]) * width + col +
                        neighbours[k][0]] = k;
            }
          }
        }
      } else {
        new_state[row * width + col] = state[row*width+col];
      }
      // state[row*width+col]^=1;
    }
  }
  memcpy(state, new_state, size);
  free(new_state);
}

void plot_canvas(Canvas *c, int *state) {
  for (int row = 0; row < c->height; row++) {
    for (int col = 0; col < c->width; col++) {
      int value = state[row * c->width + col];
      canvas_place(c, col, row, st_col[value]);
    }
  }
}

void free_canvas(Canvas *c) {
  free(c->surface);
  free(c);
  c = NULL;
}

int main() {
  clear_screen();
  int HEIGHT = 86, WIDTH = 170;
  Canvas *c = InitWindow(WIDTH, HEIGHT, "Game Of Life");
  // int *state = malloc(sizeof(int)*WIDTH*HEIGHT);
  int *state = calloc(sizeof(int), WIDTH * HEIGHT);
  int loop = 1;
  int cont = 0;
  fill_random(state, WIDTH, HEIGHT);
  plot_canvas(c, state);
  c->draw(c);
  // sleep(1);
  while (loop) {
    if (!cont)
      conway_gof(state, WIDTH, HEIGHT);
    switch (keypress()) {
    case KEY_SPACE:
      cont ^= 1;
      continue;
    case KEY_Q:
    case KEY_q: {
      loop = 0;
      continue;
    }
    case KEY_i: {
      inverse_state(state, WIDTH,HEIGHT);
    }
    }
    // if (cont)
    //   continue;
    plot_canvas(c, state);
    c->draw(c);
    usleep(1000 * 1000 / FPS);
    c->clear(c);
  }
  free_canvas(c);
  free(state);
  reset_all();
  return 0;
}
