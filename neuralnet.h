#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include <stdlib.h>
#include "Matrix.h"

typedef struct
{
    size_t nb_layers;

    Matrix **neurons_z;
    Matrix **neurons_a;

    Matrix **weights;
    Matrix **biases;

    int *layers;
} neuralnet;

neuralnet *init_neural_net(size_t nb_layers, int layers[]);


#endif