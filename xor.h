#ifndef XOR_H
#define XOR_H

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
 * Generates an array of (randomized) neuron matrices
 * according to number of neurons in each layer
 */
Matrix **generate_neurons_z(int *layers, size_t nb_layers);

/*
 * Generates the SAME neurons but we will use this array
 * to apply sigmoid function
 */
Matrix **generate_neurons_a(size_t nb_layers, Matrix **neurons_z);

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
	double *cost_fun, size_t which_input);

#endif