#ifndef NEURAL_NET_H
#define NEURAL_NET_H

typedef struct //neuralnet
{
    size_t nb_layers;

    Matrix **neurons_z;
    Matrix **neurons_a;

    Matrix **weights;
    Matrix **biases;

    int *layers;
} neuralnet;

/* ------------------------------------------- *
 * ------ Neural network initialisation ------ *
 * ------------------------------------------- */

/*
 * Initializes a neural network
 */
neuralnet *init_neural_net(size_t nb_layers, int layers[]);

#endif
