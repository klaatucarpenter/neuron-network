#include "network.h"
#include "activation.h"

std::vector<double> Network::inference(const std::vector<double>& input) const {
    std::vector<double> signals = input;

    for (const auto& layer : layers) {
        std::vector<double> layerOutput(layer.outDim, 0.0);

        for (int i = 0; i < layer.outDim; ++i) {
            for (int j = 0; j < layer.inDim; ++j) {
                layerOutput[i] += layer.weights[i * layer.inDim + j] * signals[j];
            }
            layerOutput[i] += layer.biases[i];
        }

        applyActivation(layerOutput, layer.activation);

        signals = std::move(layerOutput);
    }

    return signals;
}
