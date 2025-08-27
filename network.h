#ifndef NETWORK_H
#define NETWORK_H

#include "layer.h"

class Network
{
public:
    int inputSize = 0;
    std::vector<Layer> layers;
    std::vector<double> inference(const std::vector<double>& input) const;

    Network() = default;
};

#endif // NETWORK_H
