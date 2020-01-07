#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "ocr_net.h"
#include "neuralnet.h"
#include "../Matrix/Matrix.h"
#include "../segmentation/seg_train.h"
#include "../savenn/savenn.h"

/* --------------------------- *
 * ------ Miscellaneous ------ *
 * --------------------------- */

/*
 * Abs function
 */
double absolute(double value)
{
  if (value < 0)
    return -value;
  return value;  
}



/* --------------------------------------- *
 * ------ Generating neural network ------ *
 * --------------------------------------- */

/*
 * Generates an array of (randomized) weight matrices 
 * according to number of neurons in each layer
 */
Matrix **generate_weights(int *layers, size_t nb_layers)
{
    //pointer to list of pointers
    Matrix **weights = malloc((nb_layers - 1) * sizeof(Matrix*));
    for (size_t i=0; i < nb_layers-1; i++)
    {
        weights[i] = init_matrix(layers[i], layers[i+1], 1);
    }
    return weights;
}

/*
 * Generates an array of (randomized) biases matrices 
 * according to number of neurons in each layer
 */
Matrix **generate_biases(int *layers, size_t nb_layers)
{
    //pointer to list of pointers
    Matrix **biases = malloc((nb_layers - 1) * sizeof(Matrix*));
    for (size_t i=0; i < nb_layers-1; i++)
    {
        biases[i] = init_matrix(1, layers[i+1], 1);
    }
    return biases;
}

/*
 * Generates an array of null matrices (neurons)
 * according to number of neurons in each layer
 */
Matrix **generate_neurons_z(int *layers, size_t nb_layers)
{
    //pointer to list of pointers
    Matrix **neurons_z = malloc((nb_layers) * sizeof(Matrix*));
    for (size_t i=0; i < nb_layers; i++)
    {
        neurons_z[i] = init_matrix(1, layers[i], 0);
    }
    return neurons_z;
}

/*
 * Generates an array of null matrices (neurons)
 * according to number of neurons in each layer
 */
Matrix **generate_neurons_a(int *layers, size_t nb_layers)
{
    //pointer to list of pointers
    Matrix **neurons_a = malloc((nb_layers) * sizeof(Matrix*));
    for (size_t i=0; i < nb_layers; i++)
    {
        neurons_a[i] = init_matrix(1, layers[i], 0);
    }
    return neurons_a;
}



/* --------------------------------------- *
 * ------ Generating delta matrices ------ *
 * --------------------------------------- */

/*
 * Generates an array of 0 deltaW matrices 
 * according to number of weights
 */
Matrix **generate_deltaW(int *layers, size_t nb_layers)
{
    //pointer to list of pointers
    Matrix **deltaW = malloc((nb_layers - 1) * sizeof(Matrix*));
    for (size_t i=0; i < nb_layers-1; i++)
    {
        deltaW[i] = init_matrix(layers[i], layers[i+1], 0);
    }
    return deltaW;
}

/*
 * Generates an array of 0 deltaB matrices 
 * according to number of biases in each layer
 */
Matrix **generate_deltaB(int *layers, size_t nb_layers)
{
    //pointer to list of pointers
    Matrix **deltaB = malloc((nb_layers - 1) * sizeof(Matrix*));
    for (size_t i=0; i < nb_layers-1; i++)
    {
        deltaB[i] = init_matrix(1, layers[i+1], 0);
    }
    return deltaB;
}



/* -------------------------------- *
 * ------ Training functions ------ *
 * -------------------------------- */

/*
 * Applies functions on each layer of the NN
 */
void feed_forward(Matrix **neurons_z, Matrix **neurons_a, 
    Matrix **weights, Matrix **biases, size_t nb_layers)
{
    for (size_t i=1; i < nb_layers; i++) //one iteration per layer-1
    {
        Matrix *Wa = multiply_matrix(weights[i-1], neurons_a[i-1]);
        free(neurons_z[i]);
        neurons_z[i] = add_matrix(Wa, biases[i-1]);
        free(Wa);
        free(neurons_a[i]);
        neurons_a[i] = sigmoid_matrix(neurons_z[i]); //sigmoid
        // neurons_a[i] = relu_matrix(neurons_z[i]); //relu
        // neurons_a[i] = tanh_matrix(neurons_z[i]); //tanh
    }
}

/*
 * Backpropagation function
 */
void train(Matrix **deltaW, Matrix **deltaB, Matrix **inputs, 
    Matrix **weights, Matrix **neurons_z, Matrix **neurons_a, 
    Matrix **biases, size_t nb_layers, Matrix **desired_outputs,
    size_t nb_char, double *cost_fun, size_t which_input)
{
    size_t t = which_input; //choice of training data
    neurons_z[0] = inputs[t]; //first layer is the input
    neurons_a[0] = inputs[t]; //first layer is the input
    feed_forward(neurons_z, neurons_a, weights, biases, nb_layers);

    //error in the output layer
    Matrix *tempo_a = sub_matrix(neurons_a[nb_layers-1], 
        desired_outputs[t % nb_char]);
    Matrix *tempo_b = d_sigmoid_matrix(neurons_z[nb_layers-1]); //sigmoid
    // Matrix *tempo_b = d_relu_matrix(neurons_z[nb_layers-1]); //relu
    // Matrix *tempo_b = d_tanh_matrix(neurons_z[nb_layers-1]); //tanh
    Matrix *delta = hada_product(tempo_a, tempo_b);
    free(tempo_a);
    free(tempo_b);

    double m = 0; //momentum

    //backpropagation of the error
    for (int L=nb_layers-2; L >= 0; L--)
    {
        //deltaB is delta
        deltaB[L] = copy_matrix(delta);

        //deltaW
        Matrix *tempo_x = transpose(neurons_a[L]);
        Matrix *tempo_y = copy_matrix(deltaW[L]);
        Matrix *tempo_z = mult_matrix(m, tempo_y);
        Matrix *tempo_w = multiply_matrix(delta, tempo_x);
        free(deltaW[L]);
        deltaW[L] = add_matrix(tempo_w, tempo_z);
        free(tempo_x);
        free(tempo_y);
        free(tempo_z);
        free(tempo_w);
        
        //updates delta
        Matrix *tempo0 = transpose(weights[L]);
        Matrix *tempo1 = multiply_matrix(tempo0, delta);
        Matrix *tempo2 = d_sigmoid_matrix(neurons_z[L]); //sigmoid
        // Matrix *tempo2 = d_relu_matrix(neurons_z[L]); //relu
        // Matrix *tempo2 = d_tanh_matrix(neurons_z[L]); //tanh
        free(delta);
        delta = hada_product(tempo1, tempo2);
        free(tempo0);
        free(tempo1);
        free(tempo2);
    }
    free(delta);

    //updates cost_function
    feed_forward(neurons_z, neurons_a, weights, biases, nb_layers);
    double batch_cost = 0;
    double temp;
    for (int i = 0; i < desired_outputs[t % nb_char]->length; ++i)
    {
        temp = (desired_outputs[t % nb_char]->list[i] - 
            neurons_a[nb_layers - 1]->list[i]);
        batch_cost += temp*temp;
    }
    *cost_fun += batch_cost;
}



/* ------------------------------ *
 * ------ Output functions ------ *
 * ------------------------------ */

/*
 * Returns letter corresponding to the t-th charac
 */
char get_letter(size_t t)
{
    char letter;
    if (t <= 25)
        letter = t + 97;
    else if (t <= 51)
        letter = t + 39;
    else if (t == 52)
        letter = ',';
    else
        letter = '.';
    return letter;
}

/*
 * Returns letter corresponding to 
 * the most activated neurons in last layer
 */
char get_detected_letter(Matrix *last_layer, size_t nb_char)
{
    size_t ind = 0;
    double max = 0;
    for (size_t i = 0; i < nb_char; i++)
    {
        // printf("last_layer->list[i] = %f\n", last_layer->list[i]);
        if (last_layer->list[i] > max)
        {
            max = last_layer->list[i];
            ind = i;
        }
    }
    return get_letter(ind);
}

/*
 * Returns char recognized by the ocr
 */
char matrix_to_char(neuralnet *nn, Matrix *input)
{
    nn->neurons_z[0] = input;
    nn->neurons_a[0] = input;
    feed_forward(nn->neurons_z, nn->neurons_a, 
        nn->weights, nn->biases, nn->nb_layers);
    return get_detected_letter(nn->neurons_a[nn->nb_layers - 1], 
        (size_t) nn->neurons_a[nn->nb_layers-1]->length);
}



/* ------------------------------------------- *
 * ------ Neural network main functions ------ *
 * ------------------------------------------- */

/*
 * Trains a fresh new neural network
 */
void ocr_nn_train(Matrix **inputs, size_t nbinputs, Matrix **desired_outputs)
{
    srand(time(NULL)); //srand must only be called once
    printf("#### OCR ####\n\n");
    printf("~~~ Outputs ~~~\n\n");

    size_t nb_char = 26; //how many charac software recognizes

    //number of neurons in each layer
    //if changing number of layers, do not
    //forget to change it in init_neural_net
    int layers[8] = {676, 300, 200, 200, 150, 100, 100, 26};

    //initializing nn (generating weights, biases, neurons)
    neuralnet *nn = init_neural_net(8, layers);

    //before training
    printf("|*| Before training:\n\n");
    free(nn->neurons_z[0]);
    free(nn->neurons_a[0]);
    for (size_t t=0; t < nb_char; t++)
    {
        nn->neurons_z[0] = inputs[t];
        nn->neurons_a[0] = inputs[t];
        feed_forward(nn->neurons_z, nn->neurons_a, 
            nn->weights, nn->biases, nn->nb_layers);
        print_letter(inputs[t]);
        printf("\n--> [Before training] Detected letter: %c\n\n", 
            get_detected_letter(nn->neurons_a[nn->nb_layers - 1], nb_char));
    }

    //cost function
    double *cost_fun = malloc(sizeof(double));
    *cost_fun = 0;

    //generate deltaW
    Matrix **deltaW = generate_deltaW(nn->layers, nn->nb_layers);
    //generate deltaB
    Matrix **deltaB = malloc(sizeof(Matrix*) * nn->nb_layers-1);

    size_t which_input = 0;
    double nb_epochs = 50000;
    double eta = 0.06; //learning rate

    //training
    for (size_t epoch=0; epoch < nb_epochs; epoch++)
    {
        printf("\n--------\nepoch: %ld\n", epoch);
        printf("cost function: %f\n", *cost_fun/((epoch+1)*0.5));

        //updates eta
        eta = 0.013 * exp(-((double) epoch)/50000);
        printf("eta : %f\n", eta);

        //generates random input choice
        which_input = (size_t) (((double) rand() / RAND_MAX) * (nbinputs));
        printf("which_input : %ld\n", which_input);

        //backpropagation
        train(deltaW, deltaB, inputs, 
        nn->weights, nn->neurons_z, nn->neurons_a, 
        nn->biases, nn->nb_layers, desired_outputs, 
        nb_char, cost_fun, which_input);

        //updates weights
        Matrix **nn_weights_ptr = nn->weights;
        Matrix **tempo_0 = mult_listofmatrices(eta, deltaW, nn->nb_layers-1);
        nn->weights = sub_listofmatrices(nn->weights,tempo_0, nn->nb_layers-1);
        free_matrices(nn_weights_ptr, nn->nb_layers - 1);
        free_matrices(tempo_0, nn->nb_layers - 1);

        //updates weights
        Matrix **nn_biases_ptr = nn->biases;
        Matrix **tempo_1 = mult_listofmatrices(eta, deltaB, nn->nb_layers-1);
        nn->biases = sub_listofmatrices(nn->biases, tempo_1, nn->nb_layers-1);
        free_matrices(nn_biases_ptr, nn->nb_layers - 1);
        free_matrices(tempo_1, nn->nb_layers - 1);

        for (size_t i = 0; i < nn->nb_layers-1; ++i)
            free(deltaB[i]);
    }

    //updates cost_fun
    *cost_fun /= nb_epochs*0.5;

    //free time
    free_matrices(deltaW, nn->nb_layers-1);
    free_matrices(desired_outputs, nb_char);

    //after training
    printf("\n|*| After training:\n\n");
    for (size_t t=0; t < nb_char; t++)
    {
        nn->neurons_z[0] = inputs[t];
        nn->neurons_a[0] = inputs[t];
        feed_forward(nn->neurons_z, nn->neurons_a, 
            nn->weights, nn->biases, nn->nb_layers);
        print_letter(inputs[t]);
        printf("\n--> [After training] Detected letter: %c\n\n", 
            get_detected_letter(nn->neurons_a[nn->nb_layers - 1], nb_char));
    }
    printf("-\nCost function: %f\n", *cost_fun);

    //saves neural network
    printf("\n\\... Saving Neural Network .../\n");
    save_matrix_list(nn->weights, nn->nb_layers-1, "savefile/weights.bin");
    save_matrix_list(nn->biases, nn->nb_layers-1, "savefile/biases.bin");

    //free time
    free_matrices(inputs, nb_char);
    free_matrices(nn->biases, nn->nb_layers-1);
    free_matrices(nn->weights, nn->nb_layers-1);
    free_matrices_range(nn->neurons_z, 1, nn->nb_layers);
    free_matrices_range(nn->neurons_a, 1, nn->nb_layers);
    free(deltaB);
    free(nn);
    free(cost_fun);

    //exiting...
    printf("\n~ Exit ~\n");
}

/*
 * Retrains an already trained neural network
 */
void ocr_nn_train_loaded(Matrix **inputs, size_t nbinputs, 
    Matrix **desired_outputs, neuralnet *nn)
{
    srand(time(NULL)); //srand must only be called once
    printf("#### OCR ####\n\n");
    printf("~~~ Outputs ~~~\n\n");

    size_t nb_char = 26; //how many charac software recognizes

    printf("|*| Before training:\n\n");
    free(nn->neurons_z[0]);
    free(nn->neurons_a[0]);
    for (size_t t=0; t < nb_char; t++)
    {
        nn->neurons_z[0] = inputs[t];
        nn->neurons_a[0] = inputs[t];
        feed_forward(nn->neurons_z, nn->neurons_a, 
            nn->weights, nn->biases, nn->nb_layers);
        print_letter(inputs[t]);
        printf("\n--> [Before training] Detected letter: %c\n\n", 
            get_detected_letter(nn->neurons_a[nn->nb_layers - 1], nb_char));
    }

    //cost function
    double *cost_fun = malloc(1 * sizeof(double));
    *cost_fun = 0;

    //generate deltaW
    Matrix **deltaW = generate_deltaW(nn->layers, nn->nb_layers);
    //generate deltaB
    Matrix **deltaB = malloc(sizeof(Matrix*) * nn->nb_layers-1);

    size_t which_input = 0;
    double nb_epochs = 50000;
    double eta = 0.06; //learning rate

    //training
    for (size_t epoch=0; epoch < nb_epochs; epoch++)
    {
        printf("\n--------\nepoch: %ld\n", epoch);
        printf("cost function: %f\n", *cost_fun/((epoch+1)*0.5));

        //updates eta
        eta = 0.013 * exp(-((double) epoch)/50000);
        printf("eta : %f\n", eta);

        //generates random input choice
        which_input = (size_t) (((double) rand() / RAND_MAX) * (nbinputs));
        printf("which_input : %ld\n", which_input);

        //backprop
        train(deltaW, deltaB, inputs, 
        nn->weights, nn->neurons_z, nn->neurons_a, 
        nn->biases, nn->nb_layers, desired_outputs, 
        nb_char, cost_fun, which_input);

        //updates weights
        Matrix **nn_weights_ptr = nn->weights;
        Matrix **tempo_0 = mult_listofmatrices(eta, deltaW, nn->nb_layers-1);
        nn->weights = sub_listofmatrices(nn->weights,tempo_0, nn->nb_layers-1);
        free_matrices(nn_weights_ptr, nn->nb_layers - 1);
        free_matrices(tempo_0, nn->nb_layers - 1);

        //updates weights
        Matrix **nn_biases_ptr = nn->biases;
        Matrix **tempo_1 = mult_listofmatrices(eta, deltaB, nn->nb_layers-1);
        nn->biases = sub_listofmatrices(nn->biases, tempo_1, nn->nb_layers-1);
        free_matrices(nn_biases_ptr, nn->nb_layers - 1);
        free_matrices(tempo_1, nn->nb_layers - 1);

        for (size_t i = 0; i < nn->nb_layers-1; ++i)
            free(deltaB[i]);
    }

    //updates cost_fun
    *cost_fun /= nb_epochs*0.5;

    //free time
    free_matrices(deltaW, nn->nb_layers-1);
    free_matrices(desired_outputs, nb_char);

    printf("\n|*| After training:\n\n");
    for (size_t t=0; t < nb_char; t++)
    {
        nn->neurons_z[0] = inputs[t];
        nn->neurons_a[0] = inputs[t];
        feed_forward(nn->neurons_z, nn->neurons_a, 
            nn->weights, nn->biases, nn->nb_layers);
        print_letter(inputs[t]);
        printf("\n--> [After training] Detected letter: %c\n\n", 
            get_detected_letter(nn->neurons_a[nn->nb_layers - 1], nb_char));
    }
    printf("-\nCost function: %f\n", *cost_fun);

    //saves neural network
    printf("\n\\... Saving Neural Network .../\n");
    save_matrix_list(nn->weights, nn->nb_layers-1, "savefile/weights.bin");
    save_matrix_list(nn->biases, nn->nb_layers-1, "savefile/biases.bin");

    //free time
    free_matrices(inputs, nb_char);
    free_matrices(nn->biases, nn->nb_layers-1);
    free_matrices(nn->weights, nn->nb_layers-1);
    free_matrices_range(nn->neurons_z, 1, nn->nb_layers);
    free_matrices_range(nn->neurons_a, 1, nn->nb_layers);
    free(deltaB);
    free(nn);
    free(cost_fun);

    //exiting...
    printf("\n~ Exit\n");
}