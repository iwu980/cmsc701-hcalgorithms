#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

class LSTree {
    std::unordered_map<int, std::pair<int, int>> tree_struct;
    //begins with a binary tree where the children of node n are assumed to be the node given by 2n+1 and 2n+2.
    std::unordered_map<int, std::pair<float, float>> nodes;
    std::vector<std::vector<int>> nodes_by_height;
    std::vector<std::vector<float>> w; // similarity function
    int num_leaves, total_nodes;
    public: 
        LSTree() {}
        LSTree(int l) {
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            int node_num = 0;
            nodes_by_height[0].resize(num_leaves);
            for(int i = 0; i < num_leaves; i++) {
                nodes_by_height[0][i] = node_num;
                node_num++;
            }
            int lower_levels = 0;
            for(int h = 1; h < height; h++) {
                nodes_by_height[h].resize(std::ceil(nodes_by_height[h-1].size()*0.5));
                for(int i = 0; i < std::ceil(nodes_by_height[h-1].size()*0.5); i++) {
                    nodes_by_height[h][i] = node_num;
                    if((i == nodes_by_height[h].size()-1) && (nodes_by_height[h-1].size()%2 == 1)) {
                        tree_struct[node_num] = {2*i+lower_levels, -1};
                    }
                    else {
                        tree_struct[node_num] = {2*i+lower_levels, 2*i+1+lower_levels};
                    }
                    node_num++;
                }
                lower_levels += nodes_by_height[h-1].size();
            }
            total_nodes = node_num;
        }
        LSTree(int l, std::vector<std::pair<float, float>> &leaves) {
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            int node_num = 0;
            nodes_by_height[0].resize(num_leaves);
            for(int i = 0; i < num_leaves; i++) {
                nodes_by_height[0][i] = node_num;
                node_num++;
            }
            int lower_levels = 0;
            for(int h = 1; h < height; h++) {
                nodes_by_height[h].resize(std::ceil(nodes_by_height[h-1].size()*0.5));
                for(int i = 0; i < std::ceil(nodes_by_height[h-1].size()*0.5); i++) {
                    nodes_by_height[h][i] = node_num;
                    if((i == nodes_by_height[h].size()-1) && (nodes_by_height[h-1].size()%2 == 1)) {
                        tree_struct[node_num] = {2*i+lower_levels, -1};
                    }
                    else {
                        tree_struct[node_num] = {2*i+lower_levels, 2*i+1+lower_levels};
                    }
                    node_num++;
                }
                lower_levels += nodes_by_height[h-1].size();
                std::cout << "Built height " << h << std::endl;
            }
            total_nodes = node_num;
            // initialize leaves
            for(int i = 0; i < leaves.size(); i++) {
                nodes[nodes_by_height[0][i]] = leaves[i];
            }
            //print_node_vals();
            build_w();
            std::cout << "Printing finished" << std::endl;
        }
        void print_nodes_by_height() {
            for(int i = 0; i < nodes_by_height.size(); i++) {
                std::cout << "Height: " << i << " has " << nodes_by_height[i].size() << " nodes" << std::endl;
                for(auto iter = nodes_by_height[i].begin(); iter != nodes_by_height[i].end(); ++iter) {
                    std::cout << (*iter) << " ";
                }
                std::cout << std::endl;
            }
        }
    private: 
        void print_w() {
            std::cout << "w: " << std::endl;
            for(int i = 0; i < w.size(); i++) {
                for(int j = 0; j < w[i].size(); j++) {
                    std::cout << "w[" << i << "][" << j << "]: " << w[i][j] << std::endl;
                }
            }
        }
    private: 
        void build_w() {
            w.resize(total_nodes);
            for(int i = 0; i < w.size(); i++) {
                w[i].resize(total_nodes);
            }
            for(int i = 0; i < nodes_by_height.size(); i++) { //height
                for(auto iter_i = nodes_by_height[i].begin(); iter_i != nodes_by_height[i].end(); ++iter_i) { //all nodes of height i
                    //std::cout << "Building node " << (*iter_i) << " at height " << i << std::endl;
                    for(int k = 0; k <= i; k++) { //height of second node
                        for(auto iter_k = nodes_by_height[k].begin(); iter_k != nodes_by_height[k].end(); ++iter_k) { //all nodes of height k
                            //std::cout << "against node " << (*iter_k) << " at height " << k << std::endl;
                            int node1 = (*iter_i);
                            int node2 = (*iter_k);
                            if(i == 0 && k == 0) {
                                w[node1][node2] = gaussian_kernel(node1, node2);
                                w[node2][node1] = w[node1][node2];
                            }
                            else {
                                std::pair<int, int> first;
                                std::pair<int, int> second;
                                if(i == 0) {
                                    first = {node1, -1};
                                }
                                else {
                                    first = tree_struct[node1];
                                }
                                if(k == 0) {
                                    second = {node2, -1};
                                }
                                else {
                                    second = tree_struct[node2];
                                }
                                if(first.second == -1) {
                                    w[node1][node2] = w[first.first][second.first] + w[first.first][second.second];
                                }
                                else if(second.second == -1) {
                                    w[node1][node2] = w[first.first][second.first] + w[first.second][second.first];
                                }
                                else {
                                    w[node1][node2] = w[first.first][second.first] + w[first.first][second.second] + w[first.second][second.first] + w[first.second][second.second];
                                }
                                w[node2][node1] = w[node1][node2];
                            }
                        }
                    }
                }
            }
        }
        float euclidian_distance(int a, int b) {
            return std::pow(std::pow(nodes[b].first - nodes[a].first, 2) + std::pow(nodes[b].second - nodes[a].second, 2), 0.5);
        }
        float gaussian_kernel(int a, int b) {
            float sigma = 1;
            return std::exp(-1 * std::pow(euclidian_distance(a, b), 2) / (2 * std::pow(sigma, 2)));
        }
};