#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <err.h>

#include "struct/list.h"
#include "struct/Matrix.h"

#include "imageload/imageload.h"

#include "segmentation/character_sizes.h"
#include "segmentation/xycut.h"
#include "segmentation/seg_train.h"

#include "net/neuralnet.h"
#include "net/ocr_net.h"
#include "net/savenn.h"

#include "ocr.h"

int load_start_training(char *path)
{
    DIR *dp = opendir(path);
    struct dirent *ep;

    if (!dp) {
        perror("Error: "); // prints standard error
        printf("Could not open data set folder.\n");
        return 1;
    }
    
    //we count the number of files
    size_t filecount = 0;
    while ((ep = readdir(dp))) { // one = is not a mistake
        if (ep->d_type == DT_REG) {
            filecount++;
        }
    }
    closedir(dp);

    Matrix **list_images = malloc(sizeof(Matrix) * filecount);
    size_t i = 0;

    // we get the files
    dp = opendir(path);
    if (!dp) {
        perror("Error: "); // prints standard error
        printf("Could not open data set folder.\n");
        return 1;
    }

    char *filename = NULL;
    while ((ep = readdir(dp)) && i < filecount) {
        if (ep->d_type == DT_REG) {
            filename = calloc(strlen(path) + strlen(ep->d_name) + 2, sizeof(char));
            if (!filename)
                errx(1, "not enough memory");
            sprintf(filename, "%s/%s", path, ep->d_name);
            list_images[i] = get_bw_denoised_image_fast(filename);
            i++;
            free(filename);
        }
    }
    
    list **segmented_images = malloc(sizeof(list) * filecount);
    if (!segmented_images)
        errx(1, "not enough memory");
    for (size_t i = 0; i < filecount; i++) {
        segmented_images[i] = segmentate(list_images[i]);
        free(list_images[i]);
    }
    free(list_images);

    size_t nb = 0;
    Matrix **inputs = prepare_inputs(segmented_images, filecount, &nb);

    if(nb != (filecount * 54)) {
        printf("error with segmentation\n");
        return 1;
    }

    Matrix **outputs = prepare_results(26);
    
    int layers[8] = {676, 300, 200, 200, 150, 100, 100, 26};
    neuralnet *nn = init_neural_net(8, layers);
    
    // load neural net
    i = 0;
    printf("Loading neural network...");
    Matrix **loaded_weights = load_matrix_list("savefile/weights.bin", &i);
    size_t j = 0;
    Matrix **loaded_biases = load_matrix_list("savefile/biases.bin", &j);

    if(!loaded_weights || !loaded_biases) {
        printf("failed\n");
        printf("Is the neural network trained?\n");
        printf("Train using ./main --train <train set folder>\n");
        return 1;
    }

    if (i != j || i != 8 - 1 || j != 8 - 1) {
        printf("failed\n");
        return 1;
    }

    free(nn->weights);
    free(nn->biases);

    nn->weights = loaded_weights;
    nn->biases = loaded_biases;
    feed_forward(nn->neurons_z, nn->neurons_a,
            nn->weights, nn->biases, nn->nb_layers);

    printf("ok\n");
    ocr_nn_train_loaded(inputs, 26 * filecount, outputs, nn);
    return 0;
}

int start_training(char *path)
{
    DIR *dp = opendir(path);
    struct dirent *ep;

    if (!dp) {
        perror("Error: "); // prints standard error
        printf("Could not open data set folder.\n");
        return 1;
    }
    
    //we count the number of files
    size_t filecount = 0;
    while ((ep = readdir(dp))) { // one = is not a mistake
        if (ep->d_type == DT_REG) {
            filecount++;
        }
    }
    closedir(dp);

    Matrix **list_images = malloc(sizeof(Matrix) * filecount);
    size_t i = 0;

    // we get the files
    dp = opendir(path);
    if (!dp) {
        perror("Error: "); // prints standard error
        printf("Could not open data set folder.\n");
        return 1;
    }

    char *filename = NULL;
    while ((ep = readdir(dp)) && i < filecount) {
        if (ep->d_type == DT_REG) {
            filename = calloc(strlen(path) + strlen(ep->d_name) + 2, sizeof(char));
            if (!filename)
                errx(1, "not enough memory");
            sprintf(filename, "%s/%s", path, ep->d_name);
            list_images[i] = get_bw_denoised_image_fast(filename);
            i++;
            free(filename);
        }
    }
    
    list **segmented_images = malloc(sizeof(list) * filecount);
    if (!segmented_images)
        errx(1, "not enough memory");
    for (size_t i = 0; i < filecount; i++) {
        segmented_images[i] = segmentate(list_images[i]);
        free(list_images[i]);
    }
    free(list_images);

    size_t nb = 0;
    Matrix **inputs = prepare_inputs(segmented_images, filecount, &nb);

    if(nb != (filecount * 54)) {
        printf("error with segmentation\n");
        return 1;
    }

    Matrix **outputs = prepare_results(26);
    ocr_nn_train(inputs, 26 * filecount, outputs);
    return 0;
}

char *image_to_text(char *path) {
    printf("====== OCR by Soycent ======\n\n");
    
    // load image
    printf("Loading image...");
    Matrix *image = get_bw_denoised_image_fast(path);
    if (!image) {
        printf("failed\n");
        return NULL;
    }
    printf("ok\n");

    printf("Image segmentation...");
    // segmentate
    list *chars = segmentate(image);
    printf("ok\n");

    free(image);

    // neural net init
    size_t nblayers = 8;
    //int layers[10] = {676, 200, 200, 150, 100, 100, 100, 100, 100, 54};
    int layers[8] = {676, 300, 200, 200, 150, 100, 100, 26};
    neuralnet *nn = init_neural_net(8, layers);
    
    // load neural net
    size_t i = 0;
    printf("Loading neural network...");
    Matrix **loaded_weights = load_matrix_list("savefile/weights.bin", &i);
    size_t j = 0;
    Matrix **loaded_biases = load_matrix_list("savefile/biases.bin", &j);

    if(!loaded_weights || !loaded_biases) {
        printf("failed\n");
        printf("Is the neural network trained?\n");
        printf("Train using ./main --train <train set folder>\n");
        return NULL;
    }

    if (i != j || i != nblayers - 1 || j != nblayers - 1) {
        printf("failed\n");
        return NULL;
    }

    free(nn->weights);
    free(nn->biases);

    nn->weights = loaded_weights;
    nn->biases = loaded_biases;
    feed_forward(nn->neurons_z, nn->neurons_a,
            nn->weights, nn->biases, nn->nb_layers);

    printf("ok\n");

    size_t length = list_len(chars);
    char *res = calloc(length + 1, sizeof(char));
    
    printf("Character recognition...");
    // loop sending each matrix to neural net
    i = 0;
    list *cur = NULL;
    struct txtelement *nline = NULL;
    struct txtelement *nspace = NULL;
    struct txtelement *elt = NULL;

    while (!list_is_empty(chars)) {
        cur = list_pop_front(chars);
        elt = cur->data;
        if (elt->type == 0) {
            // send to nn
            elt->matrix->columns = 1;
            elt->matrix->lines = 676;
            res[i] = matrix_to_char(nn, elt->matrix);
            free(elt->matrix);
            free(elt);
        }
        else if (elt->type == 1)
            res[i] = ' ';
        else if (elt->type == 2)
            res[i] = '\n';
        free(cur);
        i++;
    }

    res[i] = '\0';
    
    printf("done\n");

    free_matrices(nn->biases, nn->nb_layers-1);
    free_matrices(nn->weights, nn->nb_layers-1);
    free_matrices_range(nn->neurons_z, 1, nn->nb_layers);
    free_matrices_range(nn->neurons_a, 1, nn->nb_layers);
    free(nn);
    
    if (nline) {
        free(nline);
        nline = NULL;
    }
    if (nspace) {
        free(nspace);
        nspace = NULL;
    }
    free(chars);
    
    // return the result

    return res;
}

