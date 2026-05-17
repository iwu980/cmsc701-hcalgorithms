#include <math.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <numeric>       
#include "utilities.h"

float euclidian_distance(std::vector<float> &v1, std::vector<float> &v2) {
    float sum = 0;
    auto itr1 = v1.begin();
    for (auto itr2 = v2.begin(); itr2 != v2.end(); itr2++) {
        sum += std::pow(*itr1 - *itr2, 2);
        itr1++;
    }    
    return std::sqrt(sum);
}

float gaussian_kernel(std::vector<float> &v1, std::vector<float> &v2) {
    float sigma = 1;
    return std::exp(-1 * std::pow(euclidian_distance(v1, v2), 2) / (2 * std::pow(sigma, 2)));
}

