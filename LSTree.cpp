#include <unordered_map>
#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
class LSTree {
    std::unordered_map<int, std::tuple<int, int>> tree_struct;
    //begins with a binary tree where the children of node n are assumed to be the node given by 2n+1 and 2n+2.
    std::unordered_map<int, std::string> nodes;
    std::vector<std::vector<int>> nodes_by_height;
    std::vector<std::vector<int>> w;
    int num_leaves;
    public: 
        LSTree() {}
        LSTree(int l) {
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            for(int i = 0; i < height; i++) {
                nodes_by_height[i].resize(fmin(std::pow(2, height-i-1), num_leaves));
                for(int j = 0; j < nodes_by_height[i].size(); j++) {
                    nodes_by_height[i][j] = std::pow(2, height-i-1) + j - 1;
                }
            }
            std::cout << "LSTree" << std::endl;
            print_nodes_by_height();
        }
        LSTree(int l, std::vector<std::string> leaves) {
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2+2));
            nodes_by_height.resize(height);
            for(int i = 0; i < height; i++) {
                nodes_by_height[i].resize(std::pow(2, height-i-1)-1);
                for(int j = 0; j < std::pow(2, i); j++) {
                    nodes_by_height[i][j] = std::pow(2, height-i-1) + j;
                }
            }
            std::cout << "LSTree" << std::endl;
            print_nodes_by_height();
            for(int i = 0; i < leaves.size(); i++) {
                nodes[2*nodes.size()+1] = leaves[i];
            }
            //build_w();
        }
        void print_nodes_by_height() {
            for(int i = 0; i < nodes_by_height.size(); i++) {
                std::cout << "Height: " << i << " has " << nodes_by_height[i].size() << " nodes" << std::endl;
                for(int j = 0; j < nodes_by_height[i].size(); j++) {
                    std::cout << nodes_by_height[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }
    private: 
        void build_w() {
            w.resize(num_leaves*2+1);
            for(int i = 0; i < nodes_by_height[0].size(); i++) {
                for(int j = i; j < nodes_by_height[0].size(); j++) {
                    w[i][j] = distance(i, j);
                    w[j][i] = distance(j, i);
                }
            }
            for(int i = 0; i < nodes_by_height.size(); i++) {
                for(int j = 0; j < nodes_by_height[i].size(); j++) {
                    for(int k = 0; k < i; k++) {
                        for(int m = 0; m < nodes_by_height[k].size(); m++) {
                            w[j][m] = w[2*j+1][2*m+1] + w[2*j+1][2*m+2] + w[2*j+2][2*m+1] + w[2*j+2][2*m+2];
                        }
                    }
                }
            }
        }
        int distance(int a, int b) {
            return std::pow(nodes[b].length() - nodes[a].length(), 2);
        }
};