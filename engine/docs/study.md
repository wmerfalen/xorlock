# Ambiguities in SDL2 API

## Drawing
  - What exactly does the `SDL_RenderPresent` method do?
    - A: SDL has what's called a "backbuffer". Updates to that buffer occur like when you call `SDL_RenderCopyEx()` or it's simpler (no "Ex") counterpart.
  - What exactly does the `SDL_RenderClear` method do? 
    - A: Paints the render draw color thus clearing the screen
    - Do I always have to call it when updating a texture?
      - A: no, only when you need to blank the *entire* screen with the current draw color
    - Is it possible (or more efficient) to not call clear every time I draw?
      - A: it can be, especially if lots of things have changed in the environment.
    - Is it possible to not call clear and copy a texture?
      - A: yes, but each copy will remain on the screen
