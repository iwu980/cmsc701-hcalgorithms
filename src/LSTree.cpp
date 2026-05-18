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
    std::vector<std::unordered_set<int>> nodes_by_height;
    std::vector<std::vector<float>> w; // similarity function
    std::vector<int> parents, heights, subtree_leaves;
    float revenue;
    int num_leaves, total_nodes, convergence_time;
    public: 
        LSTree() {}
        LSTree(int l) {
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            heights.resize(height*num_leaves);
            parents.resize(height*num_leaves);
            subtree_leaves.resize(height*num_leaves);
            for(int i = 0; i < num_leaves; i++) {
                nodes_by_height[0].insert(i);
                subtree_leaves[i] = 1;
                tree_struct[i] = {-1, -1};
            }
            int node_num = num_leaves;
            int lower_levels = 0;
            for(int h = 1; h < height; h++) {
                int lev_size = std::ceil(nodes_by_height[h-1].size()*0.5);
                for(int i = 0; i < lev_size; i++) {
                    nodes_by_height[h].insert(node_num);
                    if((i == lev_size-1) && (nodes_by_height[h-1].size()%2 == 1)) {
                        tree_struct[node_num] = {2*i+lower_levels, -1};
                    }
                    else {
                        tree_struct[node_num] = {2*i+lower_levels, 2*i+lower_levels+1};
                        parents[2*i+lower_levels+1] = node_num;
                    }
                    parents[2*i+lower_levels] = node_num;
                    subtree_leaves[node_num] = get_subtree_leaves(tree_struct[node_num].first) + get_subtree_leaves(tree_struct[node_num].second);
                    heights[node_num] = h;
                    node_num++;
                }
                lower_levels += nodes_by_height[h-1].size();
            }
            subtree_leaves.resize(node_num);
            parents.resize(node_num);
            heights.resize(node_num);
            total_nodes = node_num;
        }
        LSTree(int l, std::vector<std::pair<float, float>> &leaves) {
            //creating tree structure
            num_leaves = l;
            int height = std::ceil(std::log2(num_leaves*2));
            nodes_by_height.resize(height);
            heights.resize(height*num_leaves);
            parents.resize(height*num_leaves);
            subtree_leaves.resize(height*num_leaves);
            for(int i = 0; i < num_leaves; i++) {
                nodes_by_height[0].insert(i);
                subtree_leaves[i] = 1;
                tree_struct[i] = {-1, -1};
            }
            int node_num = num_leaves;
            int lower_levels = 0;
            for(int h = 1; h < height; h++) {
                int lev_size = std::ceil(nodes_by_height[h-1].size()*0.5);
                for(int i = 0; i < lev_size; i++) {
                    nodes_by_height[h].insert(node_num);
                    if((i == lev_size-1) && (nodes_by_height[h-1].size()%2 == 1)) {
                        tree_struct[node_num] = {2*i+lower_levels, -1};
                    }
                    else {
                        tree_struct[node_num] = {2*i+lower_levels, 2*i+lower_levels+1};
                        parents[2*i+lower_levels+1] = node_num;
                    }
                    parents[2*i+lower_levels] = node_num;
                    subtree_leaves[node_num] = get_subtree_leaves(tree_struct[node_num].first) + get_subtree_leaves(tree_struct[node_num].second);
                    heights[node_num] = h;
                    node_num++;
                }
                lower_levels += nodes_by_height[h-1].size();
            }
            subtree_leaves.resize(node_num);
            parents.resize(node_num);
            heights.resize(node_num);
            total_nodes = node_num;
            print_nodes_by_height();
            nodes.resize(total_nodes);
            // initialize leaves
            int leaf = 0;
            for(auto iter = nodes_by_height[0].begin(); iter != nodes_by_height[0].end(); ++iter) {
                nodes[*iter] = leaves[leaf];
                leaf++;
            }
            build_w();
            calc_revenue();
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
        int get_convergence_iterations() {
            return convergence_time;
        }
    private: 
        void calc_revenue() {
            int ancestor = num_leaves;
            for(int i = 1; i < nodes_by_height.size(); i++) {
                int seen_leaves = 0;
                while(nodes_by_height[i].count(ancestor) > 0) {
                    for(int r = seen_leaves + subtree_leaves[tree_struct[ancestor].first]; r < seen_leaves + subtree_leaves[ancestor]; r++) {
                        for(int l = seen_leaves; l < seen_leaves + subtree_leaves[tree_struct[ancestor].first]; l++) {
                            revenue += subtree_leaves[ancestor] * w[l][r];
                        }
                    }
                    seen_leaves += subtree_leaves[ancestor];
                    ancestor++;
                }
            }
        }
        int get_subtree_leaves(int node) {
            if(node == -1) {
                return 0;
            }
            return subtree_leaves[node];
        }
        void optimize() {
            convergence_time = 0;
            //iterate through greedy search until it stops returning a good revenue
            std::tuple<int, int, int, float> search = greedy_search();
            //pair c, prev
            std::pair<int, int> prev_swap = {0, 0};
            while((std::get<3>(search) > 0)) {
                convergence_time++;
                revenue += std::get<3>(search);
                int y = std::get<0>(search);
                int x = tree_struct[y].first;
                int c = tree_struct[y].second;
                if(std::get<1>(search) == 1) {
                    x = c;
                    c = tree_struct[y].first;
                }
                int t = std::get<2>(search);
                int prev = tree_struct[x].first;
                if(t == 1) {
                    prev = tree_struct[x].second;
                }
                if((prev == prev_swap.first) && (c == prev_swap.second)) {
                    search = {0, 0, 0, 0};
                }
                else {
                    update_w(x, c, prev);
                    update_struct(std::get<0>(search), x, c, std::get<2>(search));
                    //updates heights and number of subtree leaves propagating upward
                    update_upwards(x);
                    search = greedy_search();
                }
            }
        }
        void update_w(int x, int c, int prev) {
            //t = 1 -> t'; t = 2 -> t''
            for(int i = 0; i < total_nodes; i++) {
                if(i != x) {
                    //typo in jowhari
                    w[x][i] += w[c][i] - w[prev][i];
                    w[i][x] = w[x][i];
                }
            }
        }
        void update_upwards(int node) {
            //updates both height and leaves in subtree rooted at this node by propagating upwards
            int h = std::max(heights[tree_struct[node].first], heights[tree_struct[node].second]) + 1;
            while(node <= (total_nodes-1)) {
                subtree_leaves[node] = get_subtree_leaves(tree_struct[node].first) + get_subtree_leaves(tree_struct[node].second);
                if(heights[node] != h) {
                    nodes_by_height[heights[node]].erase(node);
                    heights[node] = h;
                    if(nodes_by_height.size() <= (heights[node])) {
                        nodes_by_height.resize(heights[node]+1);
                    }
                    nodes_by_height[heights[node]].insert(node);
                }
                h = std::max(heights[tree_struct[parents[node]].second]+1, heights[tree_struct[parents[node]].first]+1);
                if(node == (total_nodes-1)) {
                    node++;
                }
                else {
                    node = parents[node];
                }
            }
        }
        void update_struct(int y, int x, int c, int t) {
            //t = 1 -> t'; t = 2 -> t''
            int a = tree_struct[x].first;
            int b = tree_struct[x].second;
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
                    for(int k = 0; k <= i; k++) { //height of second node
                        for(auto iter_k = nodes_by_height[k].begin(); iter_k != nodes_by_height[k].end(); ++iter_k) { //all nodes of height k
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