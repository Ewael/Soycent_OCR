#ifndef IMAGELOADING_H
#define IMAGELOADING_H

#include "Matrix.h"
Matrix* get_bw_denoised_image(char* path);
Matrix* get_bw_denoised_image_fast(char* path);

#endif