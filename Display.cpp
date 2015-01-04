#include "schelling.hpp"


/*******************************************************************************
 * CONSTRUCTOR:
 ******************************************************************************/
Display::Display(void)
{
  window = nullptr;
  rend = nullptr;
  main_font = nullptr;
}

/*******************************************************************************
 * METHOD: init
 ******************************************************************************/
int Display::init(void)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cerr << "Error initializing SDL: ";
    std::cerr << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  window = SDL_CreateWindow(SCREEN_TITLE,
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            0,0,
                            SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (window == nullptr)
  {
    std::cerr << "Error creating window: ";
    std::cerr << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  if ((rend = SDL_CreateRenderer(window,-1,0)) == nullptr)
  {
    std::cerr << "Error creating renderer: ";
    std::cerr << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  if (SDL_GetDisplayBounds(0,&bounds) < 0)
  {
    std::cerr << "Getting display bounds failed: ";
    std::cerr << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  if (TTF_Init() < 0)
  {
    std::cerr << "Error initializing TTF: ";
    std::cerr << TTF_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  if ((main_font = TTF_OpenFont(FONT,FONT_SIZE)) == nullptr)
  {
    std::cerr << "Error creating default font: ";
    std::cerr << TTF_GetError() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/*******************************************************************************
 * METHOD: get_xmax
 ******************************************************************************/
int Display::get_xmax(void)
{ return bounds.w; }

/*******************************************************************************
 * METHOD: get_ymax
 ******************************************************************************/
int Display::get_ymax(void)
{ return bounds.h; }

/*******************************************************************************
 * METHOD: draw_background
 ******************************************************************************/
void Display::draw_background(void)
{
  SDL_SetRenderDrawColor(rend,0x00,0x00,0x00,0xff);
  SDL_RenderClear(rend);
}

/*******************************************************************************
 * METHOD: render
 ******************************************************************************/
void Display::render(void)
{ SDL_RenderPresent(rend); }

/*******************************************************************************
 * METHOD: quit
 ******************************************************************************/
void Display::quit(void)
{
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(window);
  TTF_CloseFont(main_font);
  TTF_Quit();
  SDL_Quit();
}
