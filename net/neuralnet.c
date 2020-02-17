#include <stdlib.h>

#include "../struct/Matrix.h"

#include "neuralnet.h"
#include "ocr_net.h"


/* ------------------------------------------- *
 * ------ Neural network initialisation ------ *
 * ------------------------------------------- */

/*
 * Initializes a neural network
 */
neuralnet *init_neural_net(size_t nb_layers, int layers[])
{
    neuralnet *res = malloc(sizeof(neuralnet));

    res->nb_layers = nb_layers;
    res->layers = layers;


    res->neurons_z = generate_neurons_z(layers, nb_layers);

    Matrix **neurons_a = generate_neurons_a(layers, nb_layers);
    res->neurons_a = neurons_a;

    Matrix **weights = generate_weights(layers, nb_layers);
    res->weights = weights;

    Matrix **biases = generate_biases(layers, nb_layers);
    res->biases = biases;

    return res;
}
