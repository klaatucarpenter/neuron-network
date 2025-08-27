#ifndef LAYER_H
#define LAYER_H

#include "activation.h"
#include <vector>

class Layer
{
public:
    int inDim = 0;
    int outDim = 0;
    Activation activation = Activation::Linear;
    std::vector<double> weights;
    std::vector<double> biases;

    Layer();
};

#endif // LAYER_H
