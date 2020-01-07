#ifndef ANGLE_ADJUST_H                                                             
#define ANGLE_ADJUST_H                                                             

#include "SDL/SDL.h"                                                            
#include "SDL/SDL_image.h"                                                      
#include "Matrix.h"                                                             
                                                                                
Matrix* adjust_image_angle(Matrix *m, SDL_Surface *image, SDL_Surface *screen,
        char visualFeedback, double acceptableAngle);

void rotate_image(Matrix **m, int width, int height, double angle);
#endif         
