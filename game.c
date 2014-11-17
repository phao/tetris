#include <assert.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "xSDL.h"
#include "2D.h"
#include "text_image.h"
#include "screens.h"
#include "game.h"
#include "assets.h"

#define WHITE_INIT_CODE {255, 255, 255, 255}
#define BLACK_INIT_CODE {0,   0,   0,   255}

#define COLOR1_INIT_CODE {255, 0,   0,   255}
#define COLOR2_INIT_CODE {0,   255, 0,   255}
#define COLOR3_INIT_CODE {0,   0,   255, 255}
#define COLOR4_INIT_CODE {255, 255, 0,   255}
#define COLOR5_INIT_CODE {255, 0,   255, 255}
#define COLOR6_INIT_CODE {0,   255, 255, 255}
#define COLOR7_INIT_CODE {0,   128, 255, 255}

static const SDL_Color BLACK = BLACK_INIT_CODE;
static const SDL_Color PANEL_BORDER_COLOR = WHITE_INIT_CODE;

/*
 * - a block is a small unit in the play panel (the panel on which tetris
 * happens).
 * - tetris pieces are composed out of blocks
 * - when blocks cannot go down in the panel, they become part of the fixed
 * blocks in the panel.
 * - becoming fix blocks mean you replace black/empty blocks.
 * - matrix for blocks
 * - array for blocks count per line
 * - small array for falling block (array 4 blocks)
 * - falling blocks fall starting at 1 fall/s and move faster as points
 * go up to max 5 falls/2s.
 * - points are showed above the panel
 *
 * - view:
 * ......................
 * ..Score..SSSS.........
 * ......................
 * ..PPPPPPPPPPPP........
 * ..PPPPPPPPPPPP........
 * ..PPPPPPPPPPPP........
 * ..PPPPPPPPPPPP........
 * ..PPPPPPPPPPPP........
 * ..PPPPPPPPPPPP........
 * ..PPPPPPPPPPPP........
 * ......................
 *
 * where S is a space for the score and P is a space for the panel.
 * - Each eliminated block is a point.
 * - no annimations at this version.
 * - will use stdlib.h rand for random number generation... whever is using
 * this screen can set the seed however it wishes to get different results.
 *
 * These are the tetris pieces:
 *
 * ####
 *
 *   #
 * ###
 *
 * ###
 *   #
 *
 * ##
 * ##
 *
 *  ##
 * ##
 *
 * ##
 *  ##
 *
 *  #
 * ###
 */

typedef struct Point2D GridPoint2D;
typedef struct Dim2D GridDim2D;

enum {
  PANEL_ROWS = 20,
  PANEL_COLS = 10,
  PADDING_PX = 30,

  // In tetris, a piece always is made out of 4 blocks.
  NUM_PIECE_BLOCKS = 4,

  // Initial fall delay.
  INIT_FALL_DELAY_MS = 300,

  // At most 1 key press each KEY_PRESS_DELAY.
  KEY_PRESS_DELAY = 150,

  // There are 6 different kinds of tetris pieces.
  NUM_DIFFERENT_PIECES = 7
};

struct FallingPiece {
  GridPoint2D blocks[NUM_PIECE_BLOCKS];
  GridPoint2D relative; // 0,0 means bottom-left
  SDL_Color color;
};


struct Score {
  //struct TextImage current_score, current_points;
  int points;
};


struct Panel {
  PixelDim2D block_dim;
  SDL_Rect panel_rect;

  /**
   * Empty blocks are black.
   *
   * Row 0 is the row on the bottom. Column 0 is the column on the left. Rows
   * grow grom bottom->up and columns from left->right.
   */
  SDL_Color blocks[PANEL_ROWS][PANEL_COLS];

  struct FallingPiece falling_piece;
};

struct PieceTemplate {
  GridPoint2D fills[NUM_PIECE_BLOCKS];
  GridDim2D size;
};

static const SDL_Color colors[NUM_DIFFERENT_PIECES] = {
  COLOR1_INIT_CODE, COLOR2_INIT_CODE, COLOR3_INIT_CODE,
  COLOR4_INIT_CODE, COLOR5_INIT_CODE, COLOR6_INIT_CODE,
  COLOR7_INIT_CODE
};

static const struct PieceTemplate
template[NUM_DIFFERENT_PIECES] = {
  { .fills = { {0, 0}, {1, 0}, {2, 0}, {3, 0} },
    .size = {4, 1} },

  { .fills = { {0, 0}, {0, 1}, {1, 0}, {1, 1} },
    .size = {2, 2} },

  { .fills = { {0, 0}, {1, 0}, {1, 1}, {2, 1} },
    .size = {2, 1} },

  { .fills = { {0, 1}, {1, 1}, {1, 0}, {2, 0} },
    .size = {3, 1} },

  { .fills = { {0, 0}, {1, 0}, {2, 0}, {2, 1} },
    .size = {3, 2} },

  { .fills = { {0, 1}, {1, 1}, {2, 1}, {2, 0} },
    .size = {3, 2} },

  { .fills = { {0, 0}, {1, 0}, {2, 0}, {1, 1} },
    .size = {3, 2} }
};

static struct Panel panel;
static struct Score score;

static Uint32 last_update_ms;
static Uint32 fall_delay_ms;

static struct Dim2D original_viewport;
static SDL_Renderer *renderer;

static void
destroy(void) {
  // Nothing needs done to destroy this screen.
}

static int
panel_has_block(int x, int y) {
  return !xSDL_ColorEq(panel.blocks[y] + x, &BLACK);
}

static int
is_colliding(void) {
  const GridPoint2D *rel = &panel.falling_piece.relative;
  const GridPoint2D *blocks = panel.falling_piece.blocks;

  for (int i = 0; i < NUM_PIECE_BLOCKS; i++) {
    int x = rel->x + blocks[i].x;
    int y = rel->y + blocks[i].y;
    if (y >= PANEL_ROWS) {
      continue;
    }
    if (x < 0 || x >= PANEL_COLS || y < 0 || panel_has_block(x, y)) {
      return 1;
    }
  }
  return 0;
}

static void
flip(void) {
  GridPoint2D *blocks = panel.falling_piece.blocks;
  int adj_x = 0;
  int adj_y = 0;

  // Perform a pi/2 rad rotation.
  for (int i = 0; i < NUM_PIECE_BLOCKS; i++) {
    int x = blocks[i].x;
    int y = blocks[i].y;
    int xp = -y;
    int yp = x;
    blocks[i].x = xp;
    blocks[i].y = yp;

    // Gather negatives.
    if (yp < 0 && -yp > adj_y) {
      adj_y = -yp;
    }
    if (xp < 0 && -xp > adj_x) {
      adj_x = -xp;
    }
  }

  // Normalize from the negatives gathered so that nothing is negative in the
  // end result.
  for (int i = 0; i < NUM_PIECE_BLOCKS; i++) {
    blocks[i].x += adj_x;
    blocks[i].y += adj_y;
  }
}

static void
flip_back(void) {
  flip();
  flip();
  flip();
}

static enum ScreenId
handle_event(const SDL_Event *e) {
  GridPoint2D *rel = &panel.falling_piece.relative;
  if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      case SDLK_DOWN:
        rel->y--;
        if (is_colliding()) {
          rel->y++;
        }
        else {
          last_update_ms = SDL_GetTicks();
        }
        break;
      case SDLK_LEFT:
        rel->x--;
        if (is_colliding()) {
          rel->x++;
        }
        break;
      case SDLK_RIGHT:
        rel->x++;
        if (is_colliding()) {
          rel->x--;
        }
        break;
      case SDLK_UP:
        flip();
        if (is_colliding()) {
          flip_back();
        }
        break;
    }
  }
  return SELF;
}

/**
 * Black colors indicate nothing is falling.
 */
static int
is_falling(void) {
  return !xSDL_ColorEq(&panel.falling_piece.color, &BLACK);
}

static void
spawn_piece(void) {
  assert(!is_falling());

  int piece_num = rand()%NUM_DIFFERENT_PIECES;

  memcpy(panel.falling_piece.blocks, template[piece_num].fills,
    NUM_PIECE_BLOCKS * sizeof (GridPoint2D));
  panel.falling_piece.color = colors[piece_num];
  panel.falling_piece.relative = (GridPoint2D) {
    .x = PANEL_COLS/2 - template[piece_num].size.w/2,
    .y = PANEL_ROWS - template[piece_num].size.h/2 - 1
  };

  assert(is_falling());
}

static void
reset_piece(void) {
  panel.falling_piece.color = BLACK;
  assert(!is_falling());
}

static void
empty_line(int line) {
  for (int j = 0; j < PANEL_COLS; j++) {
    panel.blocks[line][j] = BLACK;
  }
}

static void
eliminate_line(int line) {
  for (int i = line+1; i < PANEL_ROWS; i++) {
    memcpy(panel.blocks+i-1, panel.blocks+i,
      PANEL_COLS * sizeof (SDL_Color));
  }
  empty_line(PANEL_ROWS-1);
}

static int
try_score_line(int line) {
  int i;
  int pts = 0;
  for (i = 0; i < PANEL_COLS; i++) {
    if (xSDL_ColorEq(panel.blocks[line]+i, &BLACK)) {
      return 0;
    }
  }
  eliminate_line(line);
  pts += PANEL_COLS;
  return pts;
}

static void
try_score(void) {
  int pts = 0;
  int upper_bound = PANEL_ROWS;
  int i = 0;
  while (i < upper_bound) {
    int line_pts = try_score_line(i);
    if (line_pts != 0) {
      pts += line_pts;
      upper_bound--;
    }
    else {
      i++;
    }
  }
  pts <<= pts/PANEL_COLS - 1;
  score.points += pts;
}

static void
fixate(void) {
  const GridPoint2D *rel = &panel.falling_piece.relative;
  const GridPoint2D *blocks = panel.falling_piece.blocks;

  for (int i = 0; i < NUM_PIECE_BLOCKS; i++) {
    int x = rel->x + blocks[i].x;
    int y = rel->y + blocks[i].y;
    if (y >= PANEL_ROWS) {
      continue;
    }
    panel.blocks[y][x] = panel.falling_piece.color;
  }
  reset_piece();
  try_score();
}

static enum ScreenId
update(void) {
  Uint32 now_ms = SDL_GetTicks();
  if (is_falling()) {
    Uint32 delta_ms = now_ms - last_update_ms;
    if (delta_ms > fall_delay_ms) {
      GridPoint2D *rel = &panel.falling_piece.relative;

      rel->y--;
      if (is_colliding()) {
        rel->y++;
        fixate();
      }
      last_update_ms = now_ms;
    }
  }
  else {
    spawn_piece();
    if (is_colliding()) {
      SDL_Rect viewport = {.x = 0, .y = 0,
        .w = original_viewport.w, .h = original_viewport.h};
      SDL_RenderSetViewport(renderer, &viewport);
      return MENU_SCREEN;
    }
  }
  return SELF;
}

static void
empty_blocks(void) {
  for (int i = 0; i < PANEL_ROWS; i++) {
    empty_line(i);
  }
}

static void
focus(void) {
  last_update_ms = SDL_GetTicks();
  fall_delay_ms = INIT_FALL_DELAY_MS;

  // On focus, a new game should be started.
  empty_blocks();
  reset_piece();
}

static void
render_panel_border(SDL_Renderer *r) {
  SDL_Rect border = panel.panel_rect;
  border.x = 0;
  border.y = 0;
  xSDL_SetRenderDrawColor(r, &PANEL_BORDER_COLOR);
  SDL_RenderDrawRect(r, &border);
}

static void
render_panel_blocks(SDL_Renderer *r) {
  SDL_Rect block = (SDL_Rect) {
    .w = panel.block_dim.w,
    .h = panel.block_dim.h
  };

  for (int i = 0; i < PANEL_ROWS; i++) {
    block.y = (PANEL_ROWS - i - 1)*block.h;
    for (int j = 0; j < PANEL_COLS; j++) {
      block.x = block.w*j;
      xSDL_SetRenderDrawColor(r, panel.blocks[i] + j);
      SDL_RenderFillRect(r, &block);
    }
  }
}

static void
render_falling_piece(SDL_Renderer *r) {
  int block_w = panel.block_dim.w;
  int block_h = panel.block_dim.h;

  GridPoint2D *rel = &panel.falling_piece.relative;

  // Remembering that vertical indices grow from bottom -> up.
  int base_x_px = rel->x*block_w;
  int base_y_px = (PANEL_ROWS - rel->y - 1)*block_h;

  SDL_Rect block_rect;
  block_rect.w = panel.block_dim.w;
  block_rect.h = panel.block_dim.h;

  for (int i = 0; i < NUM_PIECE_BLOCKS; i++) {
    const GridPoint2D *block = panel.falling_piece.blocks + i;

    // Remembering, again, that vertical indices grow from bottom -> up.
    block_rect.x = block->x*block_w + base_x_px;
    block_rect.y = (-block->y)*block_h + base_y_px;

    xSDL_SetRenderDrawColor(r, &panel.falling_piece.color);
    SDL_RenderFillRect(r, &block_rect);
  }
}

static void
render(SDL_Renderer *r) {
  xSDL_SetRenderDrawColor(r, &DEFAULT_BG_COLOR);
  SDL_RenderClear(r);
  SDL_RenderSetViewport(r, &panel.panel_rect);
  render_panel_blocks(r);
  render_falling_piece(r);
  render_panel_border(r);
  SDL_RenderPresent(r);
}

int
init_game(SDL_Renderer *r, const struct Dim2D *within) {
  (void) r;

  /*
   * horizontal arrangement:
   *  - (from left) PADDING . PANEL . PADDING . PANEL FOR NEXT PIECE . PADDING
   *
   * vertical arrangement:
   *  - (from top) PADDING . SCORE . PADDING . PANEL . PADDING
   *
   * block width:
   *  - Panel width / PANEL_COLS
   *
   * block height:
   *  - Panel height / PANEL_ROWS
   *
   * panel for next piece width:
   *  - 4 * block size
   */

  int panel_top_margin = PADDING_PX*2 + MEDIUM_FONT_SIZE;
  panel.block_dim = (PixelDim2D) {
    .w = (within->w - 3*PADDING_PX)/(PANEL_COLS+4),
    .h = (within->h - panel_top_margin - PADDING_PX)/PANEL_ROWS
  };
  panel.panel_rect = (SDL_Rect) {
    .h = panel.block_dim.h*PANEL_ROWS,
    .w = panel.block_dim.w*PANEL_COLS,
    .x = PADDING_PX,
    .y = panel_top_margin
  };

  original_viewport = *within;
  renderer = r;

  AllScreens[GAME_SCREEN] = (struct ScreenObject) {
    .focus = focus,
    .render = render,
    .update = update,
    .handle_event = handle_event,
    .destroy = destroy
  };

  return 0;
}
