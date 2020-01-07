#ifndef OCR_NET_H
#define OCR_NET_H
#include "neuralnet.h"

/* --------------------------- *
 * ------ Miscellaneous ------ *
 * --------------------------- */

/*
 * Abs function
 */
double absolute(double value);



/* --------------------------------------- *
 * ------ Generating neural network ------ *
 * --------------------------------------- */

/*
 * Generates an array of (randomized) weight matrices 
 * according to number of neurons in each layer
 */
Matrix **generate_weights(int *layers, size_t nb_layers);

/*
 * Generates an array of (randomized) biases matrices 
 * according to number of neurons in each layer
 */
Matrix **generate_biases(int *layers, size_t nb_layers);

/*
 * Generates an array of null matrices (neurons)
 * according to number of neurons in each layer
 */
Matrix **generate_neurons_z(int *layers, size_t nb_layers);

/*
 * Generates an array of null matrices (neurons)
 * according to number of neurons in each layer
 */
Matrix **generate_neurons_a(int *layers, size_t nb_layers);



/* --------------------------------------- *
 * ------ Generating delta matrices ------ *
 * --------------------------------------- */

/*
 * Generates an array of 0 deltaW matrices 
 * according to number of weights
 */
Matrix **generate_deltaW(int *layers, size_t nb_layers);

/*
 * Generates an array of 0 deltaB matrices 
 * according to number of biases in each layer
 */
Matrix **generate_deltaB(int *layers, size_t nb_layers);



/* -------------------------------- *
 * ------ Training functions ------ *
 * -------------------------------- */

/*
 * Applies functions on each layer of the NN
 */
void feed_forward(Matrix **neurons_z, Matrix **neurons_a, 
	Matrix **weights, Matrix **biases, size_t nb_layers);

/*
 * Backpropagation function
 */
void train(Matrix **deltaW, Matrix **deltaB, Matrix **inputs, 
	Matrix **weights, Matrix **neurons_z, Matrix **neurons_a, 
	Matrix **biases, size_t nb_layers, Matrix **desired_outputs,
    size_t nb_char, double *cost_fun, size_t which_input);



/* ------------------------------ *
 * ------ Output functions ------ *
 * ------------------------------ */

/*
 * Returns letter corresponding to the t-th charac
 */
char get_letter(size_t t);

/*
 * Returns letter corresponding to 
 * the most activated neurons in last layer
 */
char get_detected_letter(Matrix *last_layer, size_t nb_char);

/*
 * Returns char recognized by the ocr
 */
char matrix_to_char(neuralnet *nn, Matrix *input);



/* ------------------------------------------- *
 * ------ Neural network main functions ------ *
 * ------------------------------------------- */

/*
 * Trains a fresh new neural network
 */
void ocr_nn_train(Matrix **inputs, size_t nbinputs, Matrix **desired_outputs);

/*
 * Retrains an already trained neural network
 */
void ocr_nn_train_loaded(Matrix **inputs, size_t nbinputs, 
    Matrix **desired_outputs, neuralnet *nn);

#endif