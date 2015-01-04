#ifndef __SCHELLING_HPP_
#define __SCHELLING_HPP_

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <vector>
#include <list>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>


#define SCREEN_TITLE  "SchellingSim"
#define FONT          "/usr/share/fonts/truetype/freefont/FreeSerif.ttf"
#define FONT_SIZE     30
#define RADIUS        6
#define DIAMETER      (2*RADIUS)
#define CIRCLE_BUFFER 2

#define X_BUFF_PIXELS 5
#define Y_BUFF_PIXELS 5

#define BLACK_COLOR   0xffffffff // it's actually white
#define RED_COLOR     0xff0000ff

#define MIN_NBR_THRESH      0
#define MAX_NBR_THRESH      8
#define DEFAULT_NBR_THRESH  3

// number of max points to actually populate
#define POP_PCT  0.90


typedef enum _COLOR
{ BLACK, RED,
} COLOR;

typedef std::pair<int,int> POINT;


class Display
{
  private:
    SDL_Window * window;
    SDL_Rect bounds;
  public:
    SDL_Renderer * rend;
    TTF_Font * main_font;

    Display(void);
    int init(void);
    int get_xmax(void);
    int get_ymax(void);
    void draw_background(void);
    void render(void);
    void quit(void);
};


class Sim
{
  private:
    SDL_Renderer * rend;
    SDL_Rect stats_region;
    TTF_Font * font;
    SDL_Color color;
    int xmax;
    int ymax;
    int xgrid_max;
    int ygrid_max;
    int xgrid_buf;
    int ygrid_buf;
    int x_midpoint;

    int nbr_thresh;

    int font_height;

    int step_count;
    int num_moves;

    // this is how the layout of the points is represented
    std::map<POINT,COLOR> grid;
    // precalculated pixel coordinates for each grid position
    std::map<POINT,POINT> pix_pos;
    // empty spaces on left and right sides of board
    std::vector<POINT> vacants_l;
    std::vector<POINT> vacants_r;
    // list of points to be moved on any given turn
    std::list<POINT> changes;
    // fixed list of coordinate differences for any neighbor
    std::list<POINT> neighbors;

    void splash_screen(void);
    void help_screen(void);
    void draw_board_all(void);
    void draw_board_changes(void);
    void draw_stats(void);
    void display_text(std::string, int xpos, int ypos);

    void who_moves(void);
    bool wants_to_move(POINT p, COLOR c, int thresh);

    void get_grid_size(void);
    void initialize_grid(void);
    void precalculate_xy(void);
  public:
    bool splash;
    bool help;
    bool redraw;

    Sim(SDL_Renderer * rend, TTF_Font * font, int xmax, int ymax);
    void step(void);
    void display(void);
    void increase_nbr_thresh(void);
    void decrease_nbr_thresh(void);
};

#endif /* include guard */
