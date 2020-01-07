#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "Matrix.h"
#include "xor.h"
#include "savenn.h"

int main()
{	//everything here is col x lines
	srand(time(NULL)); //srand must only be called once
	printf("#### XOR ####\n\n");

	printf("Inputs:\n"); //inputs
	Matrix **inputs = generate_xor_inputs();
	print_matrices(inputs, 4);
	
	printf("Desired outputs:\n"); //desired outputs
	Matrix **desired_outputs = generate_xor_desired_outputs();
	print_matrices(desired_outputs, 4);

	size_t nb_layers = 3; //do not forget to change size of 'layers'
	int layers[3] = {2, 2, 1}; //number of neurons in each layer
	double eta = 1; //0.1 learning rate

	//generate neurons_z
	Matrix **neurons_z = generate_neurons_z(layers, nb_layers);
	//generate neurons_a
	Matrix **neurons_a = generate_neurons_a(nb_layers, neurons_z);
	printf("Generated neurons:\n"); //neurons
	print_matrices(neurons_z, nb_layers);

	//generate weights
	Matrix **weights = generate_weights(layers, nb_layers); 
	printf("Generated weights:\n");
	print_matrices(weights, nb_layers-1);

	//generate biases
	Matrix **biases = generate_biases(layers, nb_layers); 
	printf("Generated biases:\n");
	print_matrices(biases, nb_layers-1);

	printf("~~~ Outputs ~~~\n\n");
	printf("|*| Before training:\n\n");
	for (size_t t=0; t < 4; t++)
	{
		neurons_z[0] = inputs[t];
		neurons_a[0] = inputs[t];
		feed_forward(neurons_z, neurons_a, 
			weights, biases, nb_layers);
		printf("%d xor %d = %f\n", (int)inputs[t]->list[0], 
			(int)inputs[t]->list[1], 
			neurons_a[nb_layers-1]->list[0]);
	}
	// printf("Weights:\n");
	// print_matrices(weights, nb_layers-1);
	// printf("Biases:\n");
	// print_matrices(biases, nb_layers-1);
	// printf("Neurons:\n");
	// print_matrices(neurons_a, nb_layers);

	//cost function
	double *cost_fun = malloc(1 * sizeof(double));
	*cost_fun = 10;

	//generate deltaW
	Matrix **deltaW = generate_deltaW(layers, nb_layers);
	//generate deltaB
	Matrix **deltaB = generate_deltaB(layers, nb_layers);

	double average_error = 0;
	size_t which_input = 0;
	double nb_epochs = 100000;
	//training
	for (size_t epoch=0; epoch < nb_epochs; epoch++)
	{
		which_input = epoch % 4;
		train(deltaW, deltaB, inputs, 
		weights, neurons_z,neurons_a, 
		biases, nb_layers, desired_outputs, 
		cost_fun, which_input);

		//updates neural network
		weights = sub_listofmatrices(weights, 
			mult_listofmatrices(eta, deltaW, nb_layers-1), 
			nb_layers-1);
		biases = sub_listofmatrices(biases, 
			mult_listofmatrices(eta, deltaB, nb_layers-1), 
			nb_layers-1);

		//average error
		average_error += absolute(desired_outputs[which_input]->list[0]
			- neurons_a[nb_layers-1]->list[0]);

		// printf("\ndeltaB:\n-\n");
		// print_matrices(deltaB, nb_layers-1);
		// printf("\ndeltaW:\n-\n");
		// print_matrices(deltaW, nb_layers-1);
		// feed_forward(neurons_z, neurons_a, 
		// 	weights, biases, nb_layers);
		// printf("Weights\n");
		// print_matrices(weights, nb_layers-1);
		// printf("Biases:\n");
		// print_matrices(biases, nb_layers-1);
		// printf("Neurons:\n");
		// print_matrices(neurons_a, nb_layers);
		// printf("Cost function: %f\n", *cost_fun);
	}

	//updates cost_fun
	*cost_fun /= nb_epochs*0.5;
	//updates average error
	average_error /= nb_epochs;

	//free time
	free(deltaB);
	free(deltaW);
	free(desired_outputs);

	printf("\n|*| After training:\n\n");
	for (size_t t=0; t < 4; t++)
	{
		neurons_z[0] = inputs[t];
		neurons_a[0] = inputs[t];
		feed_forward(neurons_z, neurons_a, 
			weights, biases, nb_layers);
		printf("%d xor %d = %f\n", (int)inputs[t]->list[0], 
			(int)inputs[t]->list[1], 
			neurons_a[nb_layers-1]->list[0]);
	}
	// printf("\ndeltaB:\n-\n");
	// print_matrices(deltaB, nb_layers-1);
	// printf("\ndeltaW:\n-\n");
	// print_matrices(deltaW, nb_layers-1);
	// printf("Weights:\n");
	// print_matrices(weights, nb_layers-1);
	// printf("Biases:\n");
	// print_matrices(biases, nb_layers-1);
	// printf("Neurons:\n");
	// print_matrices(neurons_a, nb_layers);
	printf("-\nCost function: %f\n", *cost_fun);
	printf("Average error: %f\n", average_error);

	//user inputs
	double x = 0;
	double y = 1;
	neurons_z[0]->list[0] = x;
	neurons_z[0]->list[1] = y;
	neurons_a[0]->list[0] = x;
	neurons_a[0]->list[1] = y;
	feed_forward(neurons_z, neurons_a, 
		weights, biases, nb_layers);
	printf("-\nWith user inputs: ");
	printf("%d xor %d = %f\n", (int)neurons_a[0]->list[0], 
		(int)neurons_a[0]->list[1], 
		neurons_a[nb_layers-1]->list[0]);

	//saves neural network
	printf("\n\\... Saving Neural Network .../\n");
	save_matrix_list(weights, nb_layers-1, "savefile/weights.bin");
	save_matrix_list(biases, nb_layers-1, "savefile/biases.bin");

	//loads neural network
	size_t i = 0;
	printf("\n\\... Loading Neural Network .../\n");
	Matrix **loaded_weights = load_matrix_list("savefile/weights.bin", &i);
	Matrix **loaded_biases = load_matrix_list("savefile/biases.bin", &i);

	//updates neural network with loaded values
	free(weights);
	free(biases);
	weights = loaded_weights;
	biases = loaded_biases;
	feed_forward(neurons_z, neurons_a, 
		weights, biases, nb_layers);

	printf("\n|*| After loading\n\n");
	for (size_t t=0; t < 4; t++)
	{
		neurons_z[0] = inputs[t];
		neurons_a[0] = inputs[t];
		feed_forward(neurons_z, neurons_a, 
			weights, biases, nb_layers);
		printf("%d xor %d = %f\n", (int)inputs[t]->list[0], 
			(int)inputs[t]->list[1], 
			neurons_a[nb_layers-1]->list[0]);
	}
	// printf("\ndeltaB:\n-\n");
	// print_matrices(deltaB, nb_layers-1);
	// printf("\ndeltaW:\n-\n");
	// print_matrices(deltaW, nb_layers-1);
	// printf("Weights:\n");
	// print_matrices(weights, nb_layers-1);
	// printf("Biases:\n");
	// print_matrices(biases, nb_layers-1);
	// printf("Neurons:\n");
	// print_matrices(neurons_a, nb_layers);
	printf("-\nCost function: %f\n", *cost_fun);
	printf("Average error: %f\n", average_error);

	//user inputs
	x = 0;
	y = 1;
	neurons_z[0]->list[0] = x;
	neurons_z[0]->list[1] = y;
	neurons_a[0]->list[0] = x;
	neurons_a[0]->list[1] = y;
	feed_forward(neurons_z, neurons_a, 
		weights, biases, nb_layers);
	printf("-\nWith user inputs: ");
	printf("%d xor %d = %f\n", (int)neurons_a[0]->list[0], 
		(int)neurons_a[0]->list[1], 
		neurons_a[nb_layers-1]->list[0]);

	//free time
	free(inputs);
	free(biases);
	free(weights);
	free(neurons_z);
	free(neurons_a);
	free(cost_fun);

	return 0;
}