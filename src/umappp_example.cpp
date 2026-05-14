#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <sstream>
#include <cstdio>
using namespace std;

#include "umappp/umappp.hpp"
// This is an example of using umappp for dimension reduction, taken from the README: https://github.com/libscran/umappp

void printVec(std::vector<double> &data) {
    for (double d : data) {
        std::cout << d << ' ';
    }
    std::cout << "\n\n";
}

int main() {
    // Assuming `data` contains high-dimensional data in column-major format,
    // i.e., each column is a observation and each row is a dimension.
    // We fill it with some random noise for the purposes of this example.
    int ndim = 10;
    int nobs = 20; // originally 2000
    std::vector<double> data(ndim * nobs);
    std::mt19937_64 rng(1000);
    std::normal_distribution<double> ndist;
    for (auto& x : data) {
        x = ndist(rng);
    }
    printVec(data);

    // Configuring the neighbor search algorithm; here, we'll be using an exact
    // search based on VP trees with a Euclidean distance metric.
    knncolle::VptreeBuilder<int, double, double> vp_builder(
        std::make_shared<knncolle::EuclideanDistance<double, double> >()
    );

    // Set number of dimensions in the output embedding.
    size_t out_dim = 2;
    std::vector<double> embedding(nobs * out_dim);

    // Initialize the UMAP state:
    umappp::Options opt;
    auto status = umappp::initialize(
        ndim,
        nobs,
        data.data(),
        vp_builder, 
        out_dim,
        embedding.data(),
        opt
    );

    // Run UMAP algorithm to completion. This updates the contents
    // of the 'embedding' vector supplied to initialize().
    status.run(embedding.data());
    printVec(embedding);

}