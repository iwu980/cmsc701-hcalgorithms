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
    std::vector<std::pair<float, float>> nodes;
    std::unordered_map<int, int> subtree_leaves;
    std::vector<std::unordered_set<int>> nodes_by_height;
    std::vector<std::vector<float>> w; // similarity function
    std::vector<int> parents, heights;
    int num_leaves, total_nodes;
    public: 
        LSTree() {}
        LSTree(int l) {
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            heights.resize(height*num_leaves);
            parents.resize(height*num_leaves);
            for(int i = 0; i < num_leaves; i++) {
                nodes_by_height[0].insert(i);
                subtree_leaves[i] = 1;
            }
            int node_num = num_leaves;
            int lower_levels = 0;
            subtree_leaves[-1] = 0;
            for(int h = 1; h < height; h++) {
                for(int i = 0; i < std::ceil(nodes_by_height[h-1].size()*0.5); i++) {
                    nodes_by_height[h].insert(node_num);
                    if((i == nodes_by_height[h].size()-1) && (nodes_by_height[h-1].size()%2 == 1)) {
                        tree_struct[node_num] = {2*i+lower_levels, -1};
                    }
                    else {
                        tree_struct[node_num] = {2*i+lower_levels, 2*i+lower_levels+1};
                        parents[2*i+lower_levels+1] = node_num;
                    }
                    parents[2*i+lower_levels] = node_num;
                    subtree_leaves[node_num] = subtree_leaves[tree_struct[node_num].first] + subtree_leaves[tree_struct[node_num].second];
                    heights[node_num] = h;
                    node_num++;
                }
                lower_levels += nodes_by_height[h-1].size();
            }
            parents.resize(node_num);
            heights.resize(node_num);
            total_nodes = node_num;
            nodes.resize(total_nodes);
        }
        LSTree(int l, std::vector<std::pair<float, float>> &leaves) {
            //update first half for subtree nodes
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            heights.resize(height*num_leaves);
            parents.resize(height*num_leaves);
            for(int i = 0; i < num_leaves; i++) {
                nodes_by_height[0].insert(i);
                subtree_leaves[i] = 1;
            }
            int node_num = num_leaves;
            int lower_levels = 0;
            subtree_leaves[-1] = 0;
            for(int h = 1; h < height; h++) {
                for(int i = 0; i < std::ceil(nodes_by_height[h-1].size()*0.5); i++) {
                    nodes_by_height[h].insert(node_num);
                    if((i == nodes_by_height[h].size()-1) && (nodes_by_height[h-1].size()%2 == 1)) {
                        tree_struct[node_num] = {2*i+lower_levels, -1};
                    }
                    else {
                        tree_struct[node_num] = {2*i+lower_levels, 2*i+lower_levels+1};
                        parents[2*i+lower_levels+1] = node_num;
                    }
                    parents[2*i+lower_levels] = node_num;
                    subtree_leaves[node_num] = subtree_leaves[tree_struct[node_num].first] + subtree_leaves[tree_struct[node_num].second];
                    heights[node_num] = h;
                    node_num++;
                }
                lower_levels += nodes_by_height[h-1].size();
            }
            parents.resize(node_num);
            heights.resize(node_num);
            total_nodes = node_num;
            nodes.resize(total_nodes);
            // initialize leaves
            int leaf = 0;
            for(auto iter = nodes_by_height[0].begin(); iter != nodes_by_height[0].end(); ++iter) {
                nodes[*iter] = leaves[leaf];
                leaf++;
            }
            build_w();
            optimize();
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
        void print_w() {
            std::cout << "w: " << std::endl;
            for(int i = 0; i < w.size(); i++) {
                for(int j = 0; j < w[i].size(); j++) {
                    std::cout << "w[" << i << "][" << j << "]: " << w[i][j] << std::endl;
                }
            }
        }
        void print_tree_struct() {
            std::cout << "Tree struct: " << std::endl;
            for(int i = num_leaves; i < total_nodes; i++) {
                std::cout << "Children of node " << i << ": " << tree_struct[i].first << " and " << tree_struct[i].second << std::endl;
            }
        }
        void print_arr(std::vector<int> arr) {
            for(int i = 0; i < arr.size(); i++) {
                std::cout << arr[i] << " ";
            }
            std::cout << std::endl;
        }
    private: 
    void optimize() {
            //iterate through greedy search until it stops returning a good revenue
            std::tuple<int, int, int, float> search = greedy_search();
            while((std::get<3>(search) > 0)) {
                int y = std::get<0>(search);
                //interchange: (x, c)
                std::pair<int, int> interchange = tree_struct[y];
                if(std::get<1>(search) == 1) {
                    interchange.first = interchange.second;
                    interchange.second = tree_struct[y].first;
                }
                int t = std::get<2>(search);
                //std::cout << "Optimal interchange is y = " << y << "; x = " << interchange.first << "; T" << t << " with profit " << std::get<3>(search) << std::endl;
                update_w(interchange, t);
                update_struct(std::get<0>(search), interchange, std::get<2>(search));
                update_heights(interchange.first);
                search = greedy_search();
            }
            nodes_by_height.shrink_to_fit();
        }
        void update_w(std::pair<int, int> interchange, int t) {
            //t = 1 -> t'; t = 2 -> t''
            int x = interchange.first;
            int c = interchange.second;
            int prev = tree_struct[x].first;
            if(t == 1) {
                prev = tree_struct[x].second;
            }
            for(int i = 0; i < total_nodes; i++) {
                if(i != x) {
                    w[x][i] += w[c][i] - w[prev][i];
                }
            }
        }
        void update_heights(int node) {
            int h = std::max(heights[tree_struct[node].first], heights[tree_struct[node].second]) + 1;
            while(h != heights[node]) {
                nodes_by_height[heights[node]].erase(node);
                heights[node] = h;
                if(nodes_by_height.size() <= (heights[node])) {
                    nodes_by_height.resize(heights[node]+1);
                }
                nodes_by_height[heights[node]].insert(node);
                h+=1;
                if(node < total_nodes-1) {
                    if(node == tree_struct[parents[node]].second) {
                        h = std::max(h, heights[tree_struct[parents[node]].first]+1);
                    }
                    else {
                        h = std::max(h, heights[tree_struct[parents[node]].second]+1);
                    }
                    node = parents[node];
                }
                else {
                    h = heights[node];
                }
            }
        }
        void update_struct(int y, std::pair<int, int> interchange, int t) {
            //t = 1 -> t'; t = 2 -> t''
            int x = interchange.first;
            int a = tree_struct[x].first;
            int b = tree_struct[x].second;
            int c = interchange.second;
            parents[c] = x;
            if(t == 1) {
                parents[b] = y;
                tree_struct[x].second = c;
                if(c == tree_struct[y].first) {
                    tree_struct[y].first = b;
                }
                else{
                    tree_struct[y].second = b;
                }
            }
            else {
                parents[a] = y;
                tree_struct[x].first = c;
                if(c == tree_struct[y].first) {
                    tree_struct[y].first = a;
                }
                else{
                    tree_struct[y].second = a;
                }
            }
        }
        std::tuple<int, int, int, float> greedy_search() {
            //look for the optimal interchange
            //edge records node number of y, whether x is y's first or second, 1 for T' and 2 for T'' as optimal interchange
            std::tuple<int, int, int> edge = {-1, -1, -1};
            float rev_inc = 0;
            for(int i = 2; i < nodes_by_height.size(); i++) {
                for(auto iter = nodes_by_height[i].begin(); iter != nodes_by_height[i].end(); ++iter) {
                    int y = *iter;
                    int x = tree_struct[y].first;
                    int c = tree_struct[y].second;
                    for(int k = 0; k < 2; k++) {
                        if((c != -1) && (tree_struct[x].second != -1)) {
                            int a = tree_struct[x].first;
                            int b = tree_struct[x].second;
                            float t2 = subtree_leaves[a] * w[b][c] - subtree_leaves[c] * w[a][b];
                            float t1 = subtree_leaves[b] * w[a][c] - subtree_leaves[c] * w[a][b];
                            if(t1 > rev_inc) {
                                edge = {y, k, 1};
                                rev_inc = t1;
                            }
                            if(t2 > rev_inc) {
                                edge = {y, k, 2};
                                rev_inc = t2;
                            }
                        }
                        x = c;
                        c = tree_struct[y].first;
                    }
                }
            }
            return {std::get<0>(edge), std::get<1>(edge), std::get<2>(edge), rev_inc};
        }
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