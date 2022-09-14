#include "../utils/matrix.h"
#include "SDL.h"

int main(void)
{
    init_sdl();
    SDL_Surface *img = load_image("image/tigre.bmp");
    display_image(img);

    return 0;
}