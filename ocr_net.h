#ifndef OCR_NET_H
#define OCR_NET_H
#include "neuralnet.h"

/*
 * Abs function
 */
double absolute(double value);

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

/*
 * Generates an array of 1*2 input matrices to
 * test xor with
 */
Matrix **generate_xor_inputs();

/*
 * Generates an array of 1*1 output matrices to
 * test xor with
 */
Matrix **generate_xor_desired_outputs();

/*
 * Applies functions on each layer of the NN
 */
void feed_forward(Matrix **neurons_z, Matrix **neurons_a, 
	Matrix **weights, Matrix **biases, size_t nb_layers);

/*
 * Backpropagation
 * y is the expected output
 */
void train(Matrix **deltaW, Matrix **deltaB, Matrix **inputs, 
	Matrix **weights, Matrix **neurons_z, Matrix **neurons_a, 
	Matrix **biases, size_t nb_layers, Matrix **desired_outputs,
    size_t nb_char, double *cost_fun, size_t which_input);

/*
 * Returns letter corresponding to the t-th charac
 */
char get_letter(size_t t);

/*
 * Returns letter corresponding to 
 * the most activated neurons in last layer
 */
char get_detected_letter(Matrix *last_layer, size_t nb_char);

void ocr_nn_train_loaded(Matrix **inputs, size_t nbinputs, Matrix **desired_outputs, neuralnet *nn);

/*
 * OCR
 */
void ocr_nn_train(Matrix **inputs, size_t nbinputs, Matrix **desired_outputs);

/*
 * Returns char recognized by the ocr
 */
char matrix_to_char(neuralnet *nn, Matrix *input);

#endif
