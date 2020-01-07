#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include "imageload.h"
#include "../Matrix/Matrix.h"

//SDL utils fonctions, by the ASM
void init_sdl()
{
    // Init only the video part.
    // If it fails, die with an error message.
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
}

SDL_Surface* load_image(char *path)
{
    SDL_Surface *img;

    // Load an image using SDL_image with format detection.
    // If it fails, die with an error message.
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());

    return img;
}

//Gather neighbours of grayscaled pixels
void compute_gray_pixels(Uint8* grayTab, Uint16* noiseTab,
        int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Uint8 gray = grayTab[j + i * width];
            //Put this value in its neighbours' noiseTab case
            //Minding the borders to not segfault
            //Unexisting pixels are considered white
            if (i != 0)
                noiseTab[j + (i - 1) * width] += gray;
            else
                noiseTab[j + i * width] += 255;
            if (i != height - 1)
                noiseTab[j + (i + 1) * width] += gray;
            else
                noiseTab[j + i * width] += 255;
            if (j != 0)
                noiseTab[j - 1 + i * width] += gray;
            else
                noiseTab[j + i * width] += 255;
            if (j != width - 1)
                noiseTab[j + 1 + i * width] += gray;
            else
                noiseTab[j + i * width] += 255;
        }
    }
}

//Estimate the size of letters by using pixels at letters' borders
//Used to guess the better size of the average array
Uint32 estimate_text_size(Uint8* grayTab, Uint16* noiseTab,
        int width, int height) {
    //Compute the average brightness of the image
    Uint32 count = 0;
    Uint32 average = 0;
    for (int i = 0; height > i; i++) {
        for (int j = 0; width > j; j++) {
            average += grayTab[j + i * width];
            count++;
        }
    }
    if (count == 0)
        return 0;
    average = average/count;
    
    //Count each black pixels surrounded by at least 2 white pixels,
    //relative to the average brightness we just found
    count = 0;
    for (int i = 0; height > i; i++) {
        for (int j = 0; width > j; j++) {
            if (grayTab[j + i * width] < average/2 &&
                noiseTab[j + i * width] > average * 2)
                count++;
        }
    }

    return count;
}



//Grayscale a given image
void grayscale_image(Uint8* grayTab, SDL_Surface *image) {
    int height = image->h;
    int width = image->w;
    for (int i = 0; height > i; i++) {
        for (int j = 0; width > j; j++) {
            //Gather pixel data
            Uint32 pixel = get_pixel(image, j, i);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            //Get the grayscaled value
            Uint8 gray = 0.3*r + 0.59*g + 0.11*b;
            //Store it in the array
            grayTab[j + i * width] = gray;
        }
    }
}

// Apply a filter on each pixels of the image to reduce the amount of noise
// Returns the number of black and white pixels
Uint32 process_noise(Uint8* grayTab, Uint16* noiseTab, Uint32* average,
        int width, int height, Uint8 threshold, int nbGrid) {
    Uint32 count = 0;
    for (int i = 0; height > i; i++) {
        for (int j = 0; width > j; j++) {
            Uint8 prevGray = grayTab[j + i * width];
            Uint16 noise = noiseTab[j + i * width];
            // Gets the value of the local average
            Uint8 laverage = average[j/(width/nbGrid)
                + (i/(height/nbGrid))*nbGrid];
            unsigned char d = laverage/60;
            int gray = (int) prevGray;
            if (d > 0) {
                // Apply a formula on each pixels to increase or lower its
                // brightness given if it's a lonely pixel or part of a letter
                gray += (((int) ((prevGray * 4 + noise)/8))
                        - (int) laverage/2)/d;
                if (gray <= threshold) {
                    gray = 0;
                    count++;
                }
                if (gray >= 255 - threshold) {
                    gray = 255;
                    count++;
                }
            }
            grayTab[j + i * width] = (Uint8) gray;
        }
    }

    return count;
}
// Apply a filter on each pixels of the image to increase contrast
// Returns the number of black and white pixels
Uint32 process_constrast(Uint8* grayTab, Uint16* noiseTab, Uint32* average,
        int width, int height, Uint8 threshold, int nbGrid) {
    Uint32 count = 0;
    for (int i = 0; height > i; i++) {
        for (int j = 0; width > j; j++) {
            Uint8 prevGray = grayTab[j + i * width];
            Uint16 noise = noiseTab[j + i * width];
            unsigned int d = average[j/(width/nbGrid)
                + (i/(height/nbGrid))*nbGrid]/60;
            int gray = (int) prevGray;
            if (d > 0) {
                //Apply a formula on each pixel to darken it if it's surrounded
                //by brigther pixels
                gray -= (abs(prevGray*8 - noise)/4)/d;
                if (gray <= threshold) {
                    gray = 0;
                    count++;
                }
                if (gray >= 255 - threshold) {
                    gray = 255;
                    count++;
                }
            }
            grayTab[j + i * width] = (Uint8) gray;
        }
    }

    return count;
}

// Fill a matrix with black and white pixels given their gray value
// but only black pixels marked as part of letters
void binarize(Uint8* grayTab, Uint8* filter, Matrix* m, Uint32* average,
        int width, int height, int nbGrid) {
    for (int i = 0; height > i; i++) {
        for (int j = 0; width > j; j++) {
            Uint8 gray = grayTab[j + i * width];
            Uint8 pixelData = filter[j + i * width];
            Uint8 laverage = average[j/(width/nbGrid)
                + (i/(height/nbGrid))*nbGrid];
            if ((pixelData > 122) || (gray > laverage))
                m->list[j+i*width] = 0;
            else
                m->list[j+i*width] = 1;
        }
    }
}

// Compute the local average of pixels in the image
// The image is in that purpose divided in "nbGrid + 1" cells
void average_pixels(Uint8* grayTab, Uint32* averageTab,
        int width, int height, int nbGrid) {
    Uint32 averageIndexes[(nbGrid + 1)*(nbGrid + 1)];
    for (long int i = 0; i < nbGrid * nbGrid; i ++) {
        averageTab[i] = 0;
        averageIndexes[i] = 0;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            averageTab[j/(width/nbGrid) + (i/(height/nbGrid))*nbGrid] += 
                grayTab[j + i * width];
            averageIndexes[j/(width/nbGrid) + (i/(height/nbGrid))*nbGrid]++; 
        }
    }

    for (long int i = 0; i < nbGrid * nbGrid; i ++) {
        if (averageIndexes[i] > 0)
            averageTab[i] = averageTab[i] / averageIndexes[i];
    }
}
// Same usage as the above function but take in count only pixels marked
// as a part of letters
void average_pixels_filtered(Uint8* grayTab, Uint8* filter, Uint32* averageTab,
        int width, int height, int nbGrid) {
    Uint32 averageIndexes[(nbGrid + 1)*(nbGrid +1)];
    for (long int i = 0; i < nbGrid * nbGrid; i ++) {
        averageTab[i] = 0;
        averageIndexes[i] = 0;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (filter[j + i * width] < 122) {
                averageTab[j/(width/nbGrid) + (i/(height/nbGrid))*nbGrid] += 
                    grayTab[j + i * width];
                averageIndexes[j/(width/nbGrid) + (i/(height/nbGrid))*nbGrid]++; 
            }
        }
    }

    for (long int i = 0; i < nbGrid * nbGrid; i ++) {
        if (averageIndexes[i] > 0)
            averageTab[i] = averageTab[i] / averageIndexes[i];
    }
}

//Little absolute function
int abs(int e) {
    if (e < 0)
        return -e;
    return e;
}

// Main function of this file
// Returns the black and white matrix from the given image, after some
// Preprocessing
Matrix* get_bw_denoised_image(char* path)
{
    //Init the image surface to extract pixels
    SDL_Surface* image_surface;
    //Load image with SDL image library
    image_surface = load_image(path);
    int height = image_surface->h;
    int width = image_surface->w;
    int nbGrid = 0;
    //Init the tabs used
    Uint8 grayTab[width * height];
    Uint16 noiseTab[width * height];
    Uint32 count = 0;
    grayscale_image(grayTab, image_surface);
    Uint8 grayTab2[width * height];
    Uint32 average[width * height];
    for (long int i = 0; i < height * width; i++)
        grayTab2[i] = grayTab[i];
    // Compute a first time the image to highlight letters' pixels
    // Uses an iterative methods to recompute data like averages each time
    for (Uint8 threshold = 0; count < (Uint32) (width * height) * 0.99
            && threshold < 255; threshold++) {
        //Fill the tab to be able to perfom additions
        for (int index = 0; index < width * height; index++)
            noiseTab[index] = 0;
        compute_gray_pixels(grayTab, noiseTab, width, height);
        // If its the first loop, try to guess the better grid size for the
        // local averages
        if (nbGrid == 0) {
            Uint32 esti = estimate_text_size(grayTab, noiseTab, width, height);
            double rapport = ((double) esti) / (double) height + 1;
            if (rapport < 1)
                rapport = 1;
            if (rapport > height && height <= width)
                rapport = height;
            if (rapport > width && width <= height)
                rapport = width;
            nbGrid = (Uint32) rapport;
        }
        // Process successively noise and contrast
        average_pixels(grayTab, average, width, height, nbGrid);
        process_noise(grayTab, noiseTab, average, width, height,
                threshold/2, nbGrid);
        compute_gray_pixels(grayTab, noiseTab, width, height);
        average_pixels(grayTab, average, width, height, nbGrid);
        count = process_constrast(grayTab, noiseTab, average, width, height,
                threshold/2, nbGrid);
    }
    //Create the matrix
    Matrix *m = init_matrix(width, height, 0);
    average_pixels_filtered(grayTab2, grayTab, average, width, height, nbGrid);
    binarize(grayTab2, grayTab, m, average, width, height, nbGrid);
    SDL_FreeSurface(image_surface);
    return m;
}

// --

Uint16 grayscale_image_fast(Uint8* grayTab, Uint16* noiseTab,
        SDL_Surface *image) {
    //Average max value is limited by the size of the variable
    //Max limit is a 2pow24 pixels image
    Uint16 currentAverage = 0;
    Uint64 totalAverage = 0;
    Uint8 currentAverageIndex = 0;
    Uint32 currentAverageTotal = 0;
    int height = image->h;
    int width = image->w;
    for (int i = 0; height > i; i++) {
        for (int j = 0; width > j; j++) {
            //Gather pixel data
            Uint32 pixel = get_pixel(image, j, i);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            //Get the grayscaled value
            Uint8 gray = 0.3*r + 0.59*g + 0.11*b;
            currentAverage += gray;
            //Put this value in its neighbours' noiseTab case
            //Minding the borders to not segfault
            if (i != 0)
                noiseTab[j + (i - 1) * width] += gray;
            if (i != height - 1)
                noiseTab[j + (i + 1) * width] += gray;
            if (j != 0)
                noiseTab[j - 1 + i * width] += gray;
            if (j != width - 1)
                noiseTab[j + 1 + i * width] += gray;
            grayTab[j + i * width] = gray;
            //Updating pixel value in the image (alters it)
            Uint32 newPixel = SDL_MapRGB(image->format, gray, gray, gray);
            put_pixel(image, j, i, newPixel);
            //Computing average
            currentAverageIndex++;
            if (currentAverageIndex == 254) {
                currentAverageTotal++;
                currentAverage = currentAverage/currentAverageIndex;
                currentAverageIndex = 0;
                totalAverage += currentAverage;
                currentAverage = 0;
            }
        }
    }
    //Final compute of the average
    if (currentAverageIndex != 0) {
        currentAverage = currentAverage/currentAverageIndex;
        totalAverage += currentAverage;
        currentAverageTotal ++;
    }
    totalAverage = totalAverage / currentAverageTotal;

    return totalAverage;
}

//Fills the binary color (black and white) tab with the values contained
//in grayTab and noiseTab.
//Average is used for a more precise contrast augmentation
void bw_denoise(Matrix* m, Uint8* grayTab, Uint16* noiseTab,
        SDL_Surface *image, Uint16 average) {
    int height = image->h;
    int width = image->w;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            //By default, only grayTab value is used
            int finalGray = grayTab[j + i * width];
            //If its not black or dark gray, compare it with the luminosity
            //of its neighbours
            Uint16 noise = noiseTab[j + i * width];
            int notNoise = !(finalGray > average/1.5 && noise < average * 2);
            if (finalGray > average/2 && finalGray < average/1.3 && notNoise)  {
                //If the neightbours are brighter, make the pixel even darker
                //Then dimmer texts will not be removed entirely during
                //binarization
                finalGray = finalGray - abs(finalGray - noise/4)*2;
            }

            if (finalGray > 255)
                finalGray = 255;
            else if (finalGray < 0)
                finalGray = 0;

            //Binarize a pixel into the final tab
            if (finalGray > average/2) {
                m->list[j + i * width] = 0;
                Uint32 newPixel = SDL_MapRGB(image->format, 255, 255, 255);
                put_pixel(image, j, i, newPixel);
            }
            else {
                m->list[j + i * width] = 1;
                Uint32 newPixel = SDL_MapRGB(image->format, 0, 0, 0);
                put_pixel(image, j, i, newPixel);
            }
        }
    }
}

Matrix* get_bw_denoised_image_fast(char* path)
{
    //Init the iamge and the screen surface for debugging
    SDL_Surface* image_surface;
    //Load image with SDL image library
    image_surface = load_image(path);
    int height = image_surface->h;
    int width = image_surface->w;
    Uint16 average = 0;
    //Init the tabs used
    Uint8 grayTab[width * height];
    Uint16 noiseTab[width * height];
    //Fill the tab to be able to perfom additions
    for (int index = 0; index < width * height; index++)
        noiseTab[index] = 0.0;
    //Create the matrix
    Matrix *m = init_matrix(width, height, 0);
    //Grayscale of pixels and gather neighbour data
    average = grayscale_image_fast(grayTab, noiseTab, image_surface);
    //Denoising
    bw_denoise(m, grayTab, noiseTab, image_surface, average);
    SDL_FreeSurface(image_surface);
    return m;
}