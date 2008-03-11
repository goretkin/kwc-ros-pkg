#include "ros/ros_slave.h"
#include "SDL/SDL.h"
#include "image_flows/FlowImage.h"

class ImageViewer : public ROS_Slave
{
public:
  FlowImage *image;
  SDL_Surface *screen, *blit_prep;

  ImageViewer() : ROS_Slave(), blit_prep(NULL)
  {
    register_sink(image = new FlowImage("image"), ROS_CALLBACK(ImageViewer, image_received));
  }
  virtual ~ImageViewer() { if (blit_prep) SDL_FreeSurface(blit_prep); }
  bool sdl_init()
  {
    screen = SDL_SetVideoMode(320, 240, 24, 0);
    return (screen ? true : false);
  }
  void image_received()
  {
    printf("received a %d by %d image\n", image->width, image->height);
    
    if (!screen)
      return; // paranoia. shouldn't happen. we should have bailed by now.
    if (screen->h != image->height || screen->w != image->width)
    {
      screen = SDL_SetVideoMode(image->width, image->height, 24, 0);
      if (!screen)
      {
        fprintf(stderr, "woah! couldn't resize the screen to (%d,%d)\n",
          image->width, image->height);
        exit(1);
      }
    }
    if (!blit_prep || blit_prep->w != image->width || blit_prep->h != image->width)
    {
      if (blit_prep)
        SDL_FreeSurface(blit_prep);
      blit_prep = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCCOLORKEY, 
        image->width, image->height, 24, 
        0x0000ff, 0x00ff00, 0xff0000, 0);
    }
    
    int row_offset = 0;
    for (int row = 0; row < image->height; row++, row_offset += screen->pitch)
      memcpy((char *)blit_prep->pixels + row_offset, image->raster + (row * image->width * 3), image->width * 3);
    
    if (SDL_MUSTLOCK(screen))
      if (SDL_LockSurface(screen) < 0)
      {
        printf("woah, failed to lock SDL surface.\n");
        return;
      }

    SDL_BlitSurface(blit_prep, NULL, screen, NULL);
/*

    if (screen->pitch == image->width * 3)
      memcpy(screen->pixels, image->raster, image->bytes_per_raster());
    else
    {
      int row_offset = 0;
      for (int row = 0; row < image->height; row++, row_offset += screen->pitch)
        memcpy((char *)screen->pixels + row_offset, image->raster + (row * image->width * 3), image->width * 3);
    }
 */   
    if (SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);
    
    SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
  }
};

int main(int argc, char **argv)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "SDL initialization error: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);

  ImageViewer v;
  if (!v.sdl_init())
  {
    fprintf(stderr, "SDL video startup error: %s\n", SDL_GetError());
    exit(1);
  }
  while (v.happy())
  {
    usleep(1000000);
  }
  return 0;
}

