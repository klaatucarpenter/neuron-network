#include "modelparser.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

static std::string trim_copy(std::string s) {
    auto isspace_ = [](unsigned char c){
        return std::isspace(c);
    };
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), isspace_));
    s.erase(std::find_if_not(s.rbegin(), s.rend(), isspace_).base(), s.end());
    return s;
}

static std::vector<double> parse_numbers_line(const std::string& line){
    std::vector<double> vals;
    std::istringstream iss(line);
    double v;
    while (iss >> v)
        vals.push_back(v);
    return vals;
}

static Activation parse_activation(const std::string& s){
    std::string t = s;
    std::transform(t.begin(), t.end(), t.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    if (t=="linear")  return Activation::Linear;
    if (t=="relu")    return Activation::Relu;
    if (t=="sigmoid") return Activation::Sigmoid;
    if (t=="tanh")    return Activation::Tanh;
    if (t=="softmax") return Activation::Softmax;
    return Activation::Linear; //fallback
}

ParsedFile parseInputFile(const std::string& path){
    std::ifstream fin(path);
    if (!fin) throw std::runtime_error("Cannot open the file: " + path);

    ParsedFile result;

    std::string line;
    bool inData = false;

    int currentInDim = 0;
    int outDim = 0;
    Activation activation = Activation::Linear;
    std::vector<double> weights, biases;

    while (std::getline(fin, line)){
        line = trim_copy(line);
        if (line.empty() || line[0]=='#') continue;

        if (line == "=== DATA ==="){
            inData = true;
            continue;
        }

        if (!inData){
            if (line.find("INPUT",0) == 0){
                std::istringstream iss(line);
                std::string keyword;
                int n;
                iss >> keyword >> n;
                result.network.inputSize = n;
                currentInDim = n;
            }
            else if (line.find("LAYER",0) == 0){
                std::istringstream iss(line);
                std::string keyword, dense;
                int units;
                std::string act;
                iss >> keyword >> dense >> units >> act;
                outDim = units;
                activation = parse_activation(act);
                weights.clear();
                biases.clear();
            }
            else if (line == "WEIGHTS:"){
                for (int r=0; r < outDim; r++){
                    std::getline(fin,line);
                    auto vals = parse_numbers_line(line);
                    for (auto v : vals) weights.push_back(v);
                }
            }
            else if (line == "BIASES:"){
                std::getline(fin,line);
                auto vals = parse_numbers_line(line);
                biases = vals;

                Layer L;
                L.inDim = currentInDim;
                L.outDim = outDim;
                L.activation = activation;
                L.weights = weights;
                L.biases = biases;
                result.network.layers.push_back(L);

                currentInDim = outDim;
            }
        }
        else {
            auto vals = parse_numbers_line(line);
            result.inputs.push_back(vals);
        }
    }

    return result;
}
