#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "xSDL.h"
#include "2D.h"
#include "text_image.h"
#include "screens.h"
#include "game.h"
#include "assets.h"
#include "error.h"
#include "scores.h"

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
static const SDL_Color WHITE = WHITE_INIT_CODE;
static const SDL_Color PANEL_BORDER_COLOR = WHITE_INIT_CODE;

typedef struct Point2D GridPoint2D;
typedef struct Dim2D GridDim2D;

enum {
  PANEL_ROWS = 20,
  PANEL_COLS = 10,
  PADDING_PX = 30,

  // In tetris, a piece always is made out of 4 blocks.
  NUM_PIECE_PARTS = 4,

  // Initial fall delay.
  INIT_FALL_DELAY_MS = 300,

  // At most 1 key press each KEY_PRESS_DELAY.
  KEY_PRESS_DELAY = 150,

  // There are 6 different kinds of tetris pieces.
  NUM_DIFFERENT_PIECES = 7
};

struct FallingPiece {
  GridPoint2D blocks[NUM_PIECE_PARTS];
  GridPoint2D relative; // 0,0 means bottom-left
  SDL_Color color;
};

struct Score {
  // label_text is supposed to hold the "Score" text.
  // points_text is supposed to hold a numeric string representing how many
  // points the player has.
  struct TextImage label_text, points_text;
  int points;
};

struct Panel {
  PixelDim2D block_dim;
  SDL_Rect panel_rect;

  /**
   * Empty blocks are black.
   *
   * Row 0 is the row on the bottom. Column 0 is the column on the left. Rows
   * grow from bottom->up and columns from left->right.
   */
  SDL_Color blocks[PANEL_ROWS][PANEL_COLS];

  struct FallingPiece falling_piece, next_piece;
};

struct PieceTemplate {
  GridPoint2D fills[NUM_PIECE_PARTS];
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
static struct ScreenObject self;
static Uint32 last_update_ms;
static Uint32 fall_delay_ms;
static SDL_Texture *block;

static int
destroy(const struct GameContext *gx) {
  (void) gx;
  return 0;
}

static int
panel_has_block(int x, int y) {
  return !xSDL_ColorEq(panel.blocks[y] + x, &BLACK);
}

static int
is_colliding(void) {
  const GridPoint2D *rel = &panel.falling_piece.relative;
  const GridPoint2D *blocks = panel.falling_piece.blocks;

  for (int i = 0; i < NUM_PIECE_PARTS; i++) {
    int x = rel->x + blocks[i].x;
    int y = rel->y + blocks[i].y;
    if (x < 0 || x >= PANEL_COLS || y < 0 ||
        (y < PANEL_ROWS && panel_has_block(x, y)))
    {
      return 1;
    }
  }
  return 0;
}

static void
flip(struct FallingPiece *piece) {
  GridPoint2D *blocks = piece->blocks;
  int adj_x = 0;
  int adj_y = 0;

  // Perform a pi/2 rad rotation.
  for (int i = 0; i < NUM_PIECE_PARTS; i++) {
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
  for (int i = 0; i < NUM_PIECE_PARTS; i++) {
    blocks[i].x += adj_x;
    blocks[i].y += adj_y;
  }
}

static void
flip_back(struct FallingPiece *piece) {
  flip(piece);
  flip(piece);
  flip(piece);
}

static enum ScreenId
handle_event(const struct GameContext *gx, const SDL_Event *e) {
  (void) gx;
  
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
        flip(&panel.falling_piece);
        if (is_colliding()) {
          flip_back(&panel.falling_piece);
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
update_next_piece(void) {
  int piece_num = rand()%NUM_DIFFERENT_PIECES;
  memcpy(panel.next_piece.blocks, template[piece_num].fills,
    NUM_PIECE_PARTS * sizeof (GridPoint2D));
  panel.next_piece.color = colors[piece_num];
  int num_flips = rand()%4;
  for (int i = 0; i < num_flips; i++) {
    flip(&panel.next_piece);
  }
  // Depending on how many flips took place, height becomes width and
  // vice-versa.
  if (num_flips % 2 == 1) {
    panel.next_piece.relative = (GridPoint2D) {
      .x = PANEL_COLS/2 - template[piece_num].size.h/2,
      .y = PANEL_ROWS - template[piece_num].size.w/2 - 1
    };
  }
  else {
    panel.next_piece.relative = (GridPoint2D) {
      .x = PANEL_COLS/2 - template[piece_num].size.w/2,
      .y = PANEL_ROWS - template[piece_num].size.h/2 - 1
    };
  }
}

static void
spawn_piece(void) {
  assert(!is_falling());
  memcpy(&panel.falling_piece, &panel.next_piece,
    sizeof (struct FallingPiece));
  assert(is_falling());
  update_next_piece();
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
  for (i = 0; i < PANEL_COLS; i++) {
    if (xSDL_ColorEq(panel.blocks[line]+i, &BLACK)) {
      return 0;
    }
  }
  eliminate_line(line);
  // 1, 2 or 3 points depending on how high the player is.
  return line < 5 ? 1 : (line < 13 ? 2 : 3);
}

static int
refresh_points_text(SDL_Renderer *r) {
  char text[30];
  snprintf(text, sizeof text, "%d", score.points);
  SDL_DestroyTexture(score.points_text.image);
  if (init_text_image(&score.points_text, get_medium_font(), text, r) < 0) {
    return -1;
  }
  score.points_text.pos.x = PADDING_PX + panel.panel_rect.w - 
    score.points_text.dim.w;
  return 0;
}

static int
try_score(SDL_Renderer *r) {
  int pts = 0;
  int upper_bound = PANEL_ROWS;
  int i = 0;
  int lines = 0;
  while (i < upper_bound) {
    int line_pts = try_score_line(i);
    if (line_pts != 0) {
      pts += line_pts;
      upper_bound--;
      lines++;
    }
    else {
      i++;
    }
  }
  // Each extra line you remove, you should double your points. If a line
  // gives you P points, removing 2 lines will give you 2P points, but
  // removing 3 lines (at once) will give you 4P; 4 lines 8P.
  pts <<= lines - 1;
  score.points += pts;
  return refresh_points_text(r);
}

static void
fixate(SDL_Renderer *r) {
  const GridPoint2D *rel = &panel.falling_piece.relative;
  const GridPoint2D *blocks = panel.falling_piece.blocks;

  for (int i = 0; i < NUM_PIECE_PARTS; i++) {
    int x = rel->x + blocks[i].x;
    int y = rel->y + blocks[i].y;
    if (y >= PANEL_ROWS) {
      continue;
    }
    panel.blocks[y][x] = panel.falling_piece.color;
  }
  reset_piece();
  try_score(r);
}

static enum ScreenId
update(const struct GameContext *gx) {
  Uint32 now_ms = SDL_GetTicks();
  if (is_falling()) {
    Uint32 delta_ms = now_ms - last_update_ms;
    if (delta_ms > fall_delay_ms) {
      GridPoint2D *rel = &panel.falling_piece.relative;
      rel->y--;
      if (is_colliding()) {
        rel->y++;
        fixate(gx->r);
      }
      last_update_ms = now_ms;
    }
  }
  else {
    spawn_piece();
    if (is_colliding()) {
      // If right after creation of new piece, it's already colliding, then
      // this game ended: add score and leave.
      if (add_score(score.points) < 0) {
        return ERROR;
      }
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

static int
focus(const struct GameContext *gx) {
  (void) gx;
  
  last_update_ms = SDL_GetTicks();
  fall_delay_ms = INIT_FALL_DELAY_MS;

  // On focus, a new game should be started.
  empty_blocks();
  reset_piece();
  score.points = 0;
  int err = refresh_points_text(gx->r);
  if (err < 0) {
    return err;
  }
  update_next_piece();
  return 0;
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
render_block(SDL_Renderer *r,
             const SDL_Rect *rect,
             const SDL_Color *color)
{
  xSDL_SetTextureColorMod(block, color);
  SDL_RenderCopy(r, block, 0, rect);
}

static void
render_panel_blocks(SDL_Renderer *r) {
  SDL_Rect block_rect = (SDL_Rect) {
    .w = panel.block_dim.w,
    .h = panel.block_dim.h
  };
  
  for (int i = 0; i < PANEL_ROWS; i++) {
    block_rect.y = (PANEL_ROWS - i - 1)*block_rect.h;
    for (int j = 0; j < PANEL_COLS; j++) {
      block_rect.x = block_rect.w*j;
      render_block(r, &block_rect, panel.blocks[i] + j);
    }
  }
  xSDL_SetTextureColorMod(block, &WHITE);
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
  
  for (int i = 0; i < NUM_PIECE_PARTS; i++) {
    const GridPoint2D *block = panel.falling_piece.blocks + i;

    // Remembering, again, that vertical indices grow from bottom -> up.
    block_rect.x = block->x*block_w + base_x_px;
    block_rect.y = -block->y*block_h + base_y_px;

    render_block(r, &block_rect, &panel.falling_piece.color);
  }
  xSDL_SetTextureColorMod(block, &WHITE);
}

static void
render_score(SDL_Renderer *r) {
  render_text_image(r, &score.label_text);
  render_text_image(r, &score.points_text);
}

static void
render_next_piece(SDL_Renderer *r) {
  SDL_Rect block_rect;
  block_rect.w = panel.block_dim.w;
  block_rect.h = panel.block_dim.h;
  
  int base_x = PADDING_PX*2 + panel.panel_rect.w;
  int base_y = PADDING_PX*2 + MEDIUM_FONT_SIZE + 
    block_rect.h*NUM_PIECE_PARTS;
  
  for (int i = 0; i < NUM_PIECE_PARTS; i++) {
    block_rect.x = base_x + panel.next_piece.blocks[i].x*block_rect.w;
    block_rect.y = base_y - panel.next_piece.blocks[i].y*block_rect.h;
    render_block(r, &block_rect, &panel.next_piece.color);
  }
}

static int
render(const struct GameContext *gx) {
  SDL_RenderSetViewport(gx->r, &panel.panel_rect);
  render_panel_blocks(gx->r);
  render_falling_piece(gx->r);
  render_panel_border(gx->r);
  
  SDL_Rect original_viewport = (SDL_Rect) {
    .x = 0, .y = 0, .w = gx->dim.w, .h = gx->dim.h
  };
  
  SDL_RenderSetViewport(gx->r, &original_viewport);
  render_score(gx->r);
  render_next_piece(gx->r);
  
  return 0;
}

int
init_game(const struct GameContext *gx) {
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
    .w = (gx->dim.w - 3*PADDING_PX)/(PANEL_COLS+4),
    .h = (gx->dim.h - panel_top_margin - PADDING_PX)/PANEL_ROWS
  };
  panel.panel_rect = (SDL_Rect) {
    .h = panel.block_dim.h*PANEL_ROWS,
    .w = panel.block_dim.w*PANEL_COLS,
    .x = PADDING_PX,
    .y = panel_top_margin
  };
  
  TTF_Font *font = get_medium_font();
  COND_ERROR(init_text_image(&score.label_text, font, "Pts", gx->r) == 0,
    e_bad_score_text);
  COND_ERROR(init_text_image(&score.points_text, font, "0", gx->r) == 0,
    e_bad_points_text);
  
  score.label_text.pos.x = PADDING_PX;
  score.label_text.pos.y = PADDING_PX;
  score.points_text.pos.x = PADDING_PX + panel.panel_rect.w - 
    score.points_text.dim.w;
  score.points_text.pos.y = PADDING_PX;

  self = (struct ScreenObject) {
    .focus = focus,
    .render = render,
    .update = update,
    .handle_event = handle_event,
    .destroy = destroy
  };
  register_screen(GAME_SCREEN, &self);
  
  block = get_tetris_block_img();

  return 0;
  
e_bad_points_text:
  SDL_DestroyTexture(score.label_text.image);
e_bad_score_text:
  return -1;
}
