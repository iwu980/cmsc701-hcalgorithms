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
    std::vector<std::unordered_set<int>> nodes_by_height;
    std::vector<std::vector<float>> w; // similarity function
    int num_leaves;
    public: 
        LSTree() {}
        LSTree(int l) {
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            int node_num = 2 * num_leaves - 2;
            while((node_num*2+1) > (2*num_leaves-2)) {
                nodes_by_height[0].insert(node_num);
                node_num--;
            }
            for(int i = 1; i < height; i++) {
                while((node_num >= 0) && (nodes_by_height[i-1].count(node_num*2+1) > 0)) {
                    nodes_by_height[i].insert(node_num);
                    node_num--;
                }
            }
            //print_nodes_by_height();
        }
        LSTree(int l, std::vector<std::pair<float, float>> &leaves) {
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            int node_num = 2 * num_leaves - 2;
            // Assign partitions H_i
            while((node_num*2+1) > (2*num_leaves-2)) {
                nodes_by_height[0].insert(node_num);
                node_num--;
            }
            for(int i = 1; i < height; i++) {
                while((node_num >= 0) && (nodes_by_height[i-1].count(node_num*2+1) > 0)) {
                    nodes_by_height[i].insert(node_num);
                    node_num--;
                }
            }
            print_nodes_by_height();
            // initialize leaves
            for(int i = 0; i < leaves.size(); i++) {
                nodes[num_leaves*2-i-2] = leaves[i];
            }
            //print_node_vals();
            build_w();
            print_w();
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
            std::cout << "starting build" << std::endl;
            w.resize(num_leaves*2-1);
            for(int i = 0; i < w.size(); i++) {
                w[i].resize(num_leaves*2-1);
            }
            for(int i = 0; i < nodes_by_height.size(); i++) { //height
                for(auto iter_i = nodes_by_height[i].begin(); iter_i != nodes_by_height[i].end(); ++iter_i) { //all nodes of height i
                    for(int k = 0; k <= i; k++) { //height of second node
                        for(auto iter_k = nodes_by_height[k].begin(); iter_k != nodes_by_height[k].end(); ++iter_k) { //all nodes of height k
                            int node1 = (*iter_i);
                            int node2 = (*iter_k);
                            if(i == 0 && k == 0) {
                                w[node1][node2] = gaussian_kernel(node1, node2);
                                w[node1][node2] = gaussian_kernel(node1, node2);
                            }
                            else {
                                std::vector<int> first;
                                std::vector<int> second;
                                if(i == 0) {
                                    first.resize(1);
                                    first[0] = node1;
                                }
                                else if((node1*2+2) > (2*num_leaves-1)) {
                                    first.resize(1);
                                    first[0] = node1*2+1;
                                }
                                else {
                                    first.resize(2);
                                    first[0] = node1*2+1;
                                    first[0] = node1*2+2;
                                }
                                if(k == 0) {
                                    second.resize(1);
                                    second[0] = node2;
                                }
                                else if((node2*2+2) > (2*num_leaves-1)) {
                                    second.resize(1);
                                    second[0] = node2*2+1;
                                }
                                else {
                                    second.resize(2);
                                    second[0] = node2*2+1;
                                    second[1] = node2*2+2;
                                }
                                w[node1][node2] = 0;
                                for(int f = 0; f < first.size(); f++) {
                                    for(int s = 0; s < second.size(); s++) {
                                        w[node1][node2] += w[f][s];
                                    }
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