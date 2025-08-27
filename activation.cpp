#include "activation.h"

#include <algorithm>
#include <cmath>

void relu(std::vector<double>& v) {
    for (auto& x : v) if (x < 0.0) x = 0.0;
}

void sigmoid(std::vector<double>& v) {
    for (auto& x : v) x = 1.0 / (1.0 + std::exp(-x));
}

void tanh(std::vector<double>& v) {
    for (auto& x : v) x = std::tanh(x);
}

void softmax(std::vector<double>& v) {
    if (v.empty()) return;
    const double maxVal = *std::max_element(v.begin(), v.end());
    double sum = 0.0;
    for (auto& x : v) {
        x = std::exp(x - maxVal);
        sum += x;
    }
    if (sum == 0.0) return;
    for (auto& x : v) x /= sum;
}

void applyActivation(std::vector<double>& v, Activation act) {
    switch (act) {
    case Activation::Linear:  /* default */ break;
    case Activation::Relu:    relu(v);      break;
    case Activation::Sigmoid: sigmoid(v);   break;
    case Activation::Tanh:    tanh(v);      break;
    case Activation::Softmax: softmax(v);   break;
    }
}
