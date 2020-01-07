#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL/SDL.h"                                                            
#include "SDL/SDL_image.h"                                                      
#include "pixel_operations.h"                                                   
#include "angle_adjust.h"                                                          
#include "Matrix.h"
#include "imageload.h"

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

//Dual array quicksort algorithm
void quicksort(int *arrayKey, int *arrayValue, int left, int right)
{
    // Base case: No need to sort arrays of length <= 1
    if (left >= right)
    {
        return;
    }

    // Choose pivot to be the last element in the subarray
    int pivot = arrayKey[right];

    // Index indicating the "split" between elements smaller than pivot and 
    // elements greater than pivot
    int cnt = left;

    // Traverse through array from l to r
    for (int i = left; i <= right; i++)
    {
        // If an element less than or equal to the pivot is found...
        if (arrayKey[i] <= pivot)
        {
            // Then swap arr[cnt] and arr[i] so that the smaller element arr[i] 
            // is to the left of all elements greater than pivot
            swap(&arrayKey[cnt], &arrayKey[i]);
            swap(&arrayValue[cnt], &arrayValue[i]);

            // Make sure to increment cnt so we can keep track of what to swap
            // arr[i] with
            cnt++;
        }
    }

    // NOTE: cnt is currently at one plus the pivot's index 
    // (Hence, the cnt-2 when recursively sorting the left side of pivot)
    quicksort(arrayKey, arrayValue, left, cnt-2); // Recursively sort the left side of pivot
    quicksort(arrayKey, arrayValue, cnt, right);   // Recursively sort the right side of pivot
}

int* get_leftmost_pixels(Matrix *m, SDL_Surface *image,
        char visualFeedback, int* leftMostPixel) {
    int height = m->lines;
    int width = m->columns;
    //First search
    int* leftMostPixels1 = malloc(sizeof(int) * height);
    *leftMostPixel = width;
    for (int h = 0; h < height; h++) {
        char searching = 1;
        for (int w = 0; w < width && searching; w++) {
            if (m->list[w + h * width] == 1) {
                searching = 0;
                leftMostPixels1[h] = w;
            }
        }
        if (searching)
            leftMostPixels1[h] = -1;
    }
    //Second search
    int leftMostPixels2[width * 2];
    for (int w = 0; w < width; w++) {
        char searching = 1;
        //Bottom to top
        for (int h = 0; h < height; h++) { 
            if (m->list[w + h * width] == 1) {
                searching = 0;
                leftMostPixels2[w] = h;
            }
        }
        if (searching)
            leftMostPixels2[w] = -1;
        searching = 1;
        //Top to bottom
        for (int h = height - 1; h > 0; h--) { 
            if (m->list[w + h * width] == 1) {
                searching = 0;
                leftMostPixels2[w + width] = h;
            }
        }
        if (searching)
            leftMostPixels2[w + width] = -1;
    }
    //Crossing results
    for (int h = 0; h < height; h++) {
        //Watch for the -1 values
        if (leftMostPixels1[h] > 0) {
            int w = leftMostPixels1[h];
            if (leftMostPixels2[w] != h && leftMostPixels2[w + width] != h)
                leftMostPixels1[h] = -1;
            else {
                if (w < *leftMostPixel)
                    *leftMostPixel = w;
                if (visualFeedback) {
                    Uint32 newPixel = SDL_MapRGB(image->format, 0, 255, 0);
                    put_pixel(image, w, h, newPixel);
                }
            }
        }
    }

    return leftMostPixels1;
}

/*void get_corner(int tab[], size_t len, int* left, int* top, int* bottom) {
    size_t leftMean = 0;
    size_t leftNb = 0;
    size_t topMean = 0;
    size_t topNb = 0;
    size_t bottomMean = 0;
    size_t bottomNb = 0;
    *left = 0;
    *top = 0;
    *bottom = 0;
    for (int h = 0; h < len; h++) {
        if (tab[h] == -1) 
            continue;

    }
}*/

int* process_left_pixels(int tab[], int height,
        unsigned int blur) {
    int* blurred_tab = malloc(sizeof(int) * blur);
    long int current_mean = 0;
    unsigned int count = 0;
    unsigned int index = 0;
    for (int h = 0; h < height; h++) {
        if (h % (height / blur) == 0 && h != 0) {
            if (count == 0)
                blurred_tab[index] = -1;
            else {
                blurred_tab[index] = current_mean/count;
                count = 0;
            }
            current_mean = 0;
            index++;
        }
        if (tab[h] != -1) {
            count++;
            current_mean += tab[h];
        }
    }
    if (count == 0)
        blurred_tab[index] = -1;
    else
        blurred_tab[index] = current_mean/count;

    return blurred_tab;
}

double compute_image_angle(Matrix *m, SDL_Surface *image, SDL_Surface *screen, 
        char visualFeedback) {
    int height = m->lines;
    int width = m->columns;
    unsigned int blur = height/4;
    //Gathering the leftmost pixels of the image for further tasks
    int leftMostPixel;
    int* leftMostPixels = get_leftmost_pixels(m, image, visualFeedback,
            &leftMostPixel);
    int* pixels = process_left_pixels(leftMostPixels, height, blur);
    free(leftMostPixels);
    if (visualFeedback) {
        update_surface(screen, image);
        wait_for_keypressed();
        refresh_bw_matrix_image(m, image);
    }
    // Calculus of A, the mean of the leftest of leftmost black pixels
    unsigned long totalTextPixelsAbs = 0;
    unsigned long totalTextPixelsOrd = 0;
    unsigned int numberOfSelectedPixels = 0;
    int depthThreshold = (width - leftMostPixel)/8;
    int numberOfMinusValues = 0;
    for (unsigned int h = 0; h < blur; h++) {
        if (pixels[h] < 0)
            numberOfMinusValues++;
        else if (pixels[h] <= leftMostPixel + depthThreshold) {
            numberOfSelectedPixels++;
            totalTextPixelsAbs += (unsigned long) pixels[h];
            totalTextPixelsOrd += (unsigned long) h;
            if (visualFeedback) {
                Uint32 newPixel = SDL_MapRGB(image->format, 0, 0, 255);
                put_pixel(image, pixels[h], h*(height/blur), newPixel);
            }
        }
    }
    int Ax = totalTextPixelsAbs/numberOfSelectedPixels;
    int Ay = totalTextPixelsOrd/numberOfSelectedPixels;
    if (visualFeedback) {
        update_surface(screen, image);
        wait_for_keypressed();
        refresh_bw_matrix_image(m, image);
    }
    // Calculus of B, the tenth of these pixels
    int pixelsHeight[blur];
    for (unsigned int i = 0; i < blur; i++)
        pixelsHeight[i] = i;
    quicksort(pixels, pixelsHeight, 0, blur - 1);
    unsigned long totalTenthPixelsAbs = 0;
    unsigned long totalTenthPixelsOrd = 0;
    for (unsigned int i = numberOfMinusValues;
            i < numberOfSelectedPixels/10 + numberOfMinusValues; i++) {
        totalTenthPixelsAbs += (unsigned long) pixels[i];
        totalTenthPixelsOrd += (unsigned long) pixelsHeight[i];
        if (visualFeedback) {
            Uint32 newPixel = SDL_MapRGB(image->format, 255, 0, 0);
            put_pixel(image, pixels[i], pixelsHeight[i]*(height/blur),
                    newPixel);
        }
    }
    if (visualFeedback) {
        update_surface(screen, image);
        wait_for_keypressed();
        refresh_bw_matrix_image(m, image);
    }
    int Bx = totalTenthPixelsAbs/(numberOfSelectedPixels/10);
    int By = totalTenthPixelsOrd/(numberOfSelectedPixels/10);
    double tetha = 0;
    printf("A = %i,%i et B = %i,%i\n", Ax, Ay, Bx, By);
    if ((unsigned int) Ay < By - blur/100 || (unsigned int) Ay > By + blur/100)
        tetha = atan(fabs((double)(Bx - Ax))/fabs((double)(By - Ay)));
    //tetha = atan(fabs(Bx - Ax));
    if (By > Ay) {
        printf(" -> Penchée à droite");
        return -tetha;
    }
    printf("-> Penchée à gauche");
    return tetha; 
}

Matrix* adjust_image_angle(Matrix *m, SDL_Surface *image, SDL_Surface *screen,  
        char visualFeedback, double acceptableAngle) {
    double tetha = compute_image_angle(m, image, screen, visualFeedback);
    int width = m->columns;
    int height = m->lines;
    printf("angle : %lf ", tetha);
    printf("is correct : %i\n", fabs(tetha)<acceptableAngle);
    for (int i = 0; i < 20 && (fabs(tetha) > acceptableAngle) 
            && (fabs(tetha) > 0.785398163 + acceptableAngle * 2 
                || fabs(tetha) < 0.785398163 - acceptableAngle * 2); i++) {
        rotate_image(&m, width, height, tetha);
        if (visualFeedback) {
            refresh_bw_matrix_image(m, image);
            update_surface(screen, image);
        }
        tetha = compute_image_angle(m, image, screen, visualFeedback);
        printf("%lf\n ", tetha);
    }
    return m;
}

void rotate_image(Matrix **m, int width, int height, double angle) {
    int cx = 0;
    int cy = 0;
    Matrix* newMatrix = init_matrix(width, height, 0);
    for (int x = 0; x < width; x++) {
        for (int y = 0;y < height; y++) {
            int newX = (x-cx)*cos(angle) - (y-cy)*sin(angle) + cx;
            int newY = (y-cy)*cos(angle) + (x-cx)*sin(angle) + cy;
            if (newX > 0 && newY > 0 && newX < width && newY < height)
                newMatrix->list[newX + width * newY] = 
                    (*m)->list[x + width * y];
        }
    }

    *m = newMatrix;
}

