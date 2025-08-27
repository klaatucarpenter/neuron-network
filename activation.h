#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <vector>
#include <cmath>

enum class Activation { Linear, Relu, Sigmoid, Tanh, Softmax };

void applyActivation(std::vector<double>& v, Activation act);

#endif // ACTIVATION_H
