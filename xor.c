#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "Matrix.h"
#include "xor.h"

/*
 * Abs function
 */
double absolute(double value)
{
  if (value < 0)
    return -value;
  return value;  
}

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
 * Generates an array of (randomized) neuron matrices
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
 * Generates the SAME neurons but we will use this array
 * to apply sigmoid function
 */
Matrix **generate_neurons_a(size_t nb_layers, Matrix **neurons_z)
{
	//pointer to list of pointers
	Matrix **neurons_a = malloc((nb_layers) * sizeof(Matrix*));
	for (size_t i=0; i < nb_layers; i++)
	{
		neurons_a[i] = neurons_z[i];
	}
	return neurons_a;
}

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

/*
 * Generates an array of 1*2 input matrices to
 * test xor with
 */
Matrix **generate_xor_inputs()
{
	//pointer to list of pointers
	Matrix **inputs = malloc(4 * sizeof(Matrix*));
	for (int i=0; i < 4; i++)
	{
		inputs[i] = init_matrix(1, 2, 0); //1x2 matrices
	}
	inputs[1]->list[1] = 1; //00
	inputs[2]->list[0] = 1; //01
	inputs[3]->list[0] = 1;	//10
	inputs[3]->list[1] = 1;	//11
	return inputs;
}

/*
 * Generates an array of 1*1 output matrices to
 * test xor with
 */
Matrix **generate_xor_desired_outputs()
{
	//pointer to list of pointers
	Matrix **desired_outputs = malloc(4 * sizeof(Matrix*));
	desired_outputs[0] = onebyone_matrix(0);
	desired_outputs[1] = onebyone_matrix(1);
	desired_outputs[2] = onebyone_matrix(1);
	desired_outputs[3] = onebyone_matrix(0);
	return desired_outputs;
}

/*
 * Applies functions on each layer of the NN
 */
void feed_forward(Matrix **neurons_z, Matrix **neurons_a, 
	Matrix **weights, Matrix **biases, size_t nb_layers)
{
	for (size_t i=1; i < nb_layers; i++) //one iteration per layer-1
	{
		Matrix *Wa = multiply_matrix(weights[i-1], neurons_a[i-1]);
		neurons_z[i] = add_matrix(Wa, biases[i-1]);
		neurons_a[i] = sigmoid_matrix(neurons_z[i]);
	}
}

/*
 * Backpropagation
 * y is the expected output
 */
void train(Matrix **deltaW, Matrix **deltaB, Matrix **inputs, 
	Matrix **weights, Matrix **neurons_z, Matrix **neurons_a, 
	Matrix **biases, size_t nb_layers, Matrix **desired_outputs, 
	double *cost_fun, size_t which_input)
{
	size_t t = which_input; //choice of training data
	neurons_z[0] = inputs[t]; //first layer is the input
	neurons_a[0] = inputs[t]; //first layer is the input
	feed_forward(neurons_z, neurons_a, weights, biases, nb_layers);

	//what we want for each neuron j in last layer
	double y = desired_outputs[t]->list[0];

	//error in the output layer
	Matrix *delta = hada_product(sub_matrix(neurons_a[nb_layers-1], 
		desired_outputs[t]), d_sigmoid_matrix(neurons_z[nb_layers-1]));

	//backpropagation of the error
	for (int L=nb_layers-2; L >= 0; L--)
	{
		//deltaB = delta
		deltaB[L] = delta;
		//deltaW
		deltaW[L] = multiply_matrix(delta, transpose(neurons_a[L]));
		//updates delta
		delta = hada_product(multiply_matrix(transpose(weights[L]), 
			delta), d_sigmoid_matrix(neurons_z[L]));
	}

	//update cost_function
	feed_forward(neurons_z, neurons_a, weights, biases, nb_layers);
	*cost_fun += absolute(y - neurons_a[nb_layers-1]->list[0])*
		absolute(y - neurons_a[nb_layers-1]->list[0]);
}