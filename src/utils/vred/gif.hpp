#ifndef GIF_HPP
#define GIF_HPP

extern unsigned char *load_gif(unsigned char*,
                               int,
                               unsigned char rgbpalette[3*256],
                               int& width,
                               int& height,
                               int Verbose
                              );

#endif
