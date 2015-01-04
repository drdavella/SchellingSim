#include "schelling.hpp"


/*******************************************************************************
 * MAIN:
 ******************************************************************************/
int main(int argc, char ** argv)
{
  Display d;
  if (d.init() == EXIT_FAILURE)
  {
    std::cerr << "Error initializing display\n\n";
    exit(1);
  }
  Sim s{ d.rend,d.main_font,d.get_xmax(),d.get_ymax() };

  bool quit = false;
  while (!quit)
  {
    SDL_Event e;
    SDL_WaitEvent(&e);

    switch(e.type)
    {
      // x-out of window
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN:
        // "press any key to continue"
        if (s.splash)
        {
          d.draw_background();
          s.splash = false;
          break;
        }
        switch (e.key.keysym.sym)
        {
          // quit program, or quit help
          case SDLK_q:
            if (s.help)
            {
              d.draw_background();
              s.help = false;
              s.redraw = true;
            }
            else
            { quit = true; }
            break;
          // enter help menu
          case SDLK_h:
            d.draw_background();
            s.help = true;
            break;
          // all of these keys advance the simulation
          case SDLK_s:
          case SDLK_RETURN:
          case SDLK_SPACE:
            if (!s.help)
            { s.step(); }
            break;
          case SDLK_UP:
            s.increase_nbr_thresh();
            break;
          case SDLK_DOWN:
            s.decrease_nbr_thresh();
            break;
        }
        break;
    }
    // do all of the drawing and rendering
    s.display();
    d.render();
  }

  d.quit();
}
