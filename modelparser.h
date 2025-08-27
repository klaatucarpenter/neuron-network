#ifndef MODELPARSER_H
#define MODELPARSER_H

#include <string>
#include <vector>
#include "network.h"

struct ParsedFile {
    Network network;
    std::vector<std::vector<double>> inputs;
};

ParsedFile parseInputFile(const std::string& path);

#endif // MODELPARSER_H
