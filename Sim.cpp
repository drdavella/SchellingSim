#include "schelling.hpp"


/*******************************************************************************
 * CONSTRUCTOR:
 ******************************************************************************/
Sim::Sim(SDL_Renderer * rend, TTF_Font * font, int xmax, int ymax)
{
  this->rend = rend;
  this->font = font;
  this->xmax = xmax;
  this->ymax = ymax;

  // display splash screen up front
  splash = true;
  // display help or not
  help = false;
  redraw = false;

  step_count = 0;
  num_moves = 0;
  nbr_thresh = DEFAULT_NBR_THRESH;
  display_mode = NUM_STEPS;

  color.r = 0xff;
  color.g = 0xff;
  color.b = 0xff;
  color.a = 0xff;

  font_height = TTF_FontHeight(font);
  stats_region.x = 0;
  stats_region.y = ymax - font_height;
  stats_region.w = xmax;
  stats_region.h = font_height;

  get_grid_size();
  initialize_grid();
  precalculate_xy();

  // these are all possible coordinates for neighbors:
  neighbors = { {0,1}, {1,0}, {1,1}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,-1} };
}

/*******************************************************************************
 * METHOD: reset
 ******************************************************************************/
void Sim::reset(void)
{
  step_count = 0;
  num_moves = 0;
  nbr_thresh = DEFAULT_NBR_THRESH;
  display_mode = NUM_STEPS;

  grid.clear();
  vacants_l.clear();
  vacants_r.clear();

  initialize_grid();

  draw_stats();
  draw_board_all();
}

/*******************************************************************************
 * METHOD: step
 ******************************************************************************/
void Sim::step(void)
{
  // reseed random, just for fun
  srand(time(NULL));
  // figure out which neighbors are moving
  who_moves();

  redraw_grid.clear();
  std::vector<POINT> * vacant_dest;
  std::vector<POINT> * vacant_new;
  SIDE side = LEFT;

  num_moves = changes.size();
  while ( !changes.empty() )
  {
    POINT item;
    // check for room to move from left to right
    if (side == LEFT)
    {
      if (vacants_r.empty() || (changes.front().first > x_midpoint))
      {
        side = RIGHT;
        continue;
      }
      else
      {
        item = changes.front();
        vacant_dest = &vacants_r;
        vacant_new  = &vacants_l;
      }
    }
    // check for room to move from right to left
    else
    {
      if (vacants_l.empty() || (changes.back().first < x_midpoint))
      {
        side = LEFT;
        continue;
      }
      else
      {
        item = changes.back();
        vacant_dest = &vacants_l;
        vacant_new  = &vacants_r;
      }
    }

    // pick a random vacant space
    int index = rand() % vacant_dest->size();
    POINT dest = vacant_dest->at(index);
    // move this point to the vacant spot
    grid[dest] = grid[item];
    // remove point from vacants
    vacant_dest->erase(vacant_dest->begin() + index);
    // remove original point from grid
    grid.erase(item);
    vacant_new->push_back(item);
    // add to changes
    redraw_grid[dest] = grid[dest];
    redraw_grid[item] = VACANT;

    // switch sides
    if (side == RIGHT)
    {
      changes.pop_back();
      side = LEFT;
    }
    else
    {
      changes.pop_front();
      side = RIGHT;
    }
  }

  step_count++;
}

/*******************************************************************************
 * METHOD: switch_display_mode
 ******************************************************************************/
void Sim::switch_display_mode(void)
{
  display_mode = (DISPLAY_MODE)((display_mode + 1) % NUM_DISPLAY_MODES);
}

/*******************************************************************************
 * METHOD: display
 ******************************************************************************/
void Sim::display(void)
{
  if (splash)
  { splash_screen(); }
  else if (help)
  { help_screen(); }
  else if (step_count == 0)
  {
    draw_stats();
    draw_board_all();
  }
  else if (redraw)
  {
    draw_stats();
    draw_board_all();
    redraw = false;
  }
  else if (num_moves == 0)
  { draw_stats(); }
  else
  {
    draw_stats();
    draw_board_changes();
  }
}

/*******************************************************************************
 * METHOD: increase_nbr_thresh
 ******************************************************************************/
void Sim::increase_nbr_thresh(void)
{
  if (nbr_thresh < MAX_NBR_THRESH)
  { nbr_thresh++; }
}

/*******************************************************************************
 * METHOD: decrease_nbr_thresh
 ******************************************************************************/
void Sim::decrease_nbr_thresh(void)
{
  if (nbr_thresh > MIN_NBR_THRESH)
  { nbr_thresh--; }
}

/*******************************************************************************
 * METHOD: splash_screen
 ******************************************************************************/
void Sim::splash_screen(void)
{
  display_text("Splash Screen",0,0);
  // do something nice later
}

/*******************************************************************************
 * METHOD: help_screen
 ******************************************************************************/
void Sim::help_screen(void)
{
  display_text("Help Screen",0,0);
  // add more later
}

/*******************************************************************************
 * METHOD: draw_board_all
 ******************************************************************************/
void Sim::draw_board_all(void)
{
  int xpos;
  int ypos;
  for (int x = 0; x < xgrid_max; x++)
  { for (int y = 0; y < ygrid_max; y++)
    {
      xpos = pix_pos[{x,y}].first;
      ypos = pix_pos[{x,y}].second;
      // this is an empty square
      if (!grid.count({x,y})) { continue; }
      if (grid[{x,y}] == BLACK)
      { filledCircleColor(rend,xpos,ypos,RADIUS,BLACK_COLOR); }
      else
      { filledCircleColor(rend,xpos,ypos,RADIUS,RED_COLOR); }
    }
  }
}

/*******************************************************************************
 * METHOD: draw_board_changes
 ******************************************************************************/
void Sim::draw_board_changes(void)
{
  int xpos;
  int ypos;
  for (auto &item : redraw_grid)
  {
    xpos = pix_pos[{item.first.first,item.first.second}].first;
    ypos = pix_pos[{item.first.first,item.first.second}].second;
    switch (item.second)
    {
      case BLACK:
        filledCircleColor(rend,xpos,ypos,RADIUS,BLACK_COLOR);
        break;
      case RED:
        filledCircleColor(rend,xpos,ypos,RADIUS,RED_COLOR);
        break;
      case VACANT:
        SDL_Rect cover_vacant;
        cover_vacant.x = xpos - RADIUS;
        cover_vacant.y = ypos - RADIUS;
        cover_vacant.h = DIAMETER + CIRCLE_BUFFER;
        cover_vacant.w = DIAMETER + CIRCLE_BUFFER;;
        SDL_SetRenderDrawColor(rend,0x00,0x00,0x00,0xff);
        SDL_RenderFillRect(rend,&cover_vacant);
        break;
    }
  }
}

/*******************************************************************************
 * METHOD: draw_stats
 ******************************************************************************/
void Sim::draw_stats(void)
{
  int stats_ypos = ymax - font_height - Y_BUFF_PIXELS;
  // redraw stats region
  SDL_SetRenderDrawColor(rend,0x00,0x00,0x00,0xff);
  SDL_RenderFillRect(rend,&stats_region);

  std::string text;
  switch (display_mode)
  {
    case NUM_STEPS:
      text = "Number of steps: " + std::to_string(step_count);
      display_text(text,X_BUFF_PIXELS*2,stats_ypos);
      // display number of moves
      text = "Number of moves: " + std::to_string(num_moves);
      display_text(text,xmax/2,stats_ypos);
      break;
    case HOMOGENEITY:
      text = "Homogeneity: " + std::to_string(calc_homogeneity()) +"%";
      display_text(text,X_BUFF_PIXELS*2,stats_ypos);
      break;
    default:
      break;
  }

  // display threshold
  SDL_Rect dst;
  text = "Neighbor Threshold: " + std::to_string(nbr_thresh);
  SDL_Surface * surface = TTF_RenderText_Solid(font,text.c_str(),color);
  SDL_Texture * texture = SDL_CreateTextureFromSurface(rend,surface);
  SDL_QueryTexture(texture,NULL,NULL,&dst.w,&dst.h);
  dst.y = ymax - dst.h - Y_BUFF_PIXELS;
  dst.x = xmax - dst.w - X_BUFF_PIXELS;
  SDL_RenderCopy(rend,texture,NULL,&dst);

  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

/*******************************************************************************
 * METHOD: display_text
 ******************************************************************************/
void Sim::display_text(std::string text, int xpos, int ypos)
{
  SDL_Surface * surface = TTF_RenderText_Solid(font,text.c_str(),color);
  SDL_Texture * texture = SDL_CreateTextureFromSurface(rend,surface);

  SDL_Rect dst;
  SDL_QueryTexture(texture,NULL,NULL,&dst.w,&dst.h);
  dst.x = xpos;
  dst.y = ypos;
  SDL_RenderCopy(rend,texture,NULL,&dst);

  SDL_FreeSurface(surface);
}

/*******************************************************************************
 * METHOD: who_moves
 ******************************************************************************/
void Sim::who_moves(void)
{
  changes.clear();
  for (auto &item : grid)
  {
    if (wants_to_move(item.first,item.second,nbr_thresh))
    {
      if (item.first.first < x_midpoint)
      { changes.push_front(item.first); }
      else
      { changes.push_back(item.first); }
    }
  }
}

/*******************************************************************************
 * METHOD: wants_to_move
 ******************************************************************************/
bool Sim::wants_to_move(POINT p, COLOR c, int thresh)
{
  int x, y;
  int num_diff = 0;

  for (POINT n : neighbors)
  {
    x = p.first + n.first;
    y = p.second + n.second;
    if (grid.find({x,y}) == grid.end())
    { continue; }
    if (grid[{x,y}] != c)
    { num_diff++; }
  }

  // interpret threshold to be minimum number of neighbors to prompt moving
  if (num_diff >= thresh) return true;
  else return false;
}

/*******************************************************************************
 * METHOD: calc_homogeneity
 ******************************************************************************/
double Sim::calc_homogeneity(void)
{
  int x,y;
  int has_different_neighbor = 0;
  for (auto &item : grid)
  {
    COLOR c = item.second;
    for (POINT n : neighbors)
    {
      x = item.first.first + n.first;
      y = item.first.second + n.second;
      if (grid.find({x,y}) == grid.end())
      { continue; }
      if ((grid[{x,y}] != VACANT) && (grid[{x,y}] != c))
      {
        has_different_neighbor++;
        break;
      }
    }
  }
  double pct_homogeneity = ((double)(xgrid_max*ygrid_max-has_different_neighbor))/
                           ((double)(xgrid_max*ygrid_max));
  // account for vacant spaces
  pct_homogeneity /= POP_PCT;
  return 100*pct_homogeneity;
}

/*******************************************************************************
 * METHOD: get_grid_size
 ******************************************************************************/
void Sim::get_grid_size(void)
{
  int stats_height = TTF_FontHeight(font) + Y_BUFF_PIXELS;
  xgrid_max = (xmax - 2*X_BUFF_PIXELS)/(DIAMETER+CIRCLE_BUFFER);
  // we want the x grid be even in width so we can divide the board in half
  if ((xgrid_max % 2) == 0)
  { xgrid_max -= 1; }
  x_midpoint = xgrid_max/2;
  ygrid_max = (ymax - 2*Y_BUFF_PIXELS - stats_height)/(DIAMETER+CIRCLE_BUFFER);
  xgrid_buf = (xmax - (xgrid_max*(DIAMETER+CIRCLE_BUFFER) - CIRCLE_BUFFER))/2;
  ygrid_buf = (ymax - (ygrid_max*(DIAMETER+CIRCLE_BUFFER) - CIRCLE_BUFFER)
                                                          - stats_height)/2;
}

/*******************************************************************************
 * METHOD: initialize_grid
 ******************************************************************************/
void Sim::initialize_grid(void)
{
  double p;
  srand(time(NULL));
  // do this probabilistically
  for (int x = 0; x < xgrid_max; x++)
  { for (int y = 0; y < ygrid_max; y++)
    {
      p = rand() / ((double) RAND_MAX);
      if (p > POP_PCT)
      {
        if (x < x_midpoint)
        { vacants_l.push_back({x,y}); }
        else
        { vacants_r.push_back({x,y}); }
      }
      else if (p > POP_PCT/2)
      { grid[{x,y}] = BLACK; }
      else
      { grid[{x,y}] = RED; }
    }
  }
}

/*******************************************************************************
 * METHOD: precalculate_xy
 ******************************************************************************/
void Sim::precalculate_xy(void)
{
  int xpos;
  int ypos;
  for (int x = 0; x < xgrid_max; x++)
  { for (int y = 0; y < ygrid_max; y++)
    {
      xpos = xgrid_buf + x*(DIAMETER+CIRCLE_BUFFER) + RADIUS;
      ypos = ygrid_buf + y*(DIAMETER+CIRCLE_BUFFER) + RADIUS;
      pix_pos[{x,y}] = {xpos,ypos};
    }
  }
}
