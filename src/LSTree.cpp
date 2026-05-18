#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#define PRINT_PROGRESS 50
#define PRECISION 0
#define MAX_ITER 10000

class LSTree {
    std::unordered_map<int, std::pair<int, int>> tree_struct;
    //begins with a binary tree where the children of node n are assumed to be the node given by 2n+1 and 2n+2.
    std::vector<std::pair<float, float>> nodes;
    std::vector<std::unordered_set<int>> nodes_by_height;
    std::vector<std::vector<float>> w; // similarity function
    std::vector<int> parents, heights, subtree_leaves;
    float revenue, scale_factor;
    int num_leaves, total_nodes, convergence_time;
    //std::ofstream profits;
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
            nodes.resize(total_nodes);
            // initialize leaves
            int leaf = 0;
            for(auto iter = nodes_by_height[0].begin(); iter != nodes_by_height[0].end(); ++iter) {
                nodes[*iter] = leaves[leaf];
                leaf++;
            }
            build_w();
            calc_revenue();
            //profits.open("profits.csv");
            //std::cout << "Scale factor: " << scale_factor << std::endl;
            optimize();
            //print_w();
            //std::cout << "Scale factor: " << scale_factor << std::endl;
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
            print_w(w.size());
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
        float get_revenue() {
            return revenue;
        }
        void print_w(int n) {
            std::cout << "w up to n: " << std::endl;
            for(int i = 0; i < n; i++) {
                for(int j = 0; j < n; j++) {
                    std::cout << "w[" << i << "][" << j << "]: " << w[i][j] << std::endl;
                }
            }
        }
    private: 
        void calc_revenue() {
            revenue = 0;
            std::unordered_map<int, std::unordered_set<int>> clusters;
            for(auto iter = nodes_by_height[1].begin(); iter != nodes_by_height[1].end(); ++iter) {
                clusters[*iter].insert(tree_struct[*iter].first);
                if(subtree_leaves[*iter] > 1) {
                    clusters[*iter].insert(tree_struct[*iter].second);
                    //std::cout << "For nodes " << tree_struct[*iter].first << " and " << tree_struct[*iter].second << " added (" << (num_leaves - 2) << " * " << w[tree_struct[*iter].first][tree_struct[*iter].second] << ")/" << scale_factor << " to " << revenue << " = ";
                    revenue += ((num_leaves - 2) * w[tree_struct[*iter].first][tree_struct[*iter].second])/scale_factor;
                    //std::cout << revenue << std::endl;
                }
                //std::cout << "Added cluster at " << *iter << std::endl;
            }
            while(clusters.count(total_nodes-1) == 0) {
                auto clust_it = clusters.begin();
                int parent = parents[clust_it->first];
                std::pair<int, int> children = tree_struct[parent];
                int sibling = children.second;
                if(clust_it->first != children.first) {
                    sibling = children.first;
                }
                while((sibling > num_leaves) && (clusters.count(sibling) == 0)) {
                    ++clust_it;
                    parent = parents[clust_it->first];
                    std::pair<int, int> children = tree_struct[parent];
                    sibling = children.second;
                    if(clust_it->first != children.first) {
                        sibling = children.first;
                    }
                }
                std::pair<int, std::unordered_set<int>>clust = *(clust_it);
                if(sibling != -1) {
                    std::unordered_set<int> sib_list = {sibling};
                    if(clusters.count(sibling) > 0) {
                        sib_list = clusters[sibling];
                    }
                    int leaves = clust.second.size() + clusters[sibling].size();
                    for(auto iter1 = clust.second.begin(); iter1 != clust.second.end(); ++iter1) {
                        for(auto iter2 = sib_list.begin(); iter2 != sib_list.end(); ++iter2) {
                            //std::cout << "For nodes " << *iter1 << " and " << *iter2 << " added (" << (num_leaves - leaves) << " * " << w[*iter1][*iter2] << ")/" << scale_factor << " to " << revenue << " = ";
                            revenue += ((num_leaves - leaves) * w[*iter1][*iter2])/scale_factor;
                            //std::cout << revenue << std::endl;
                            clusters[parent].insert(*iter2);
                        }
                        clusters[parent].insert(*iter1);
                    }
                    clusters.erase(clust.first);
                    clusters.erase(sibling);
                    //std::cout << "Merged " << clust.first << " and " << sibling << " into " << parent << std::endl;
                }
                else {
                    clusters[parent] = std::unordered_set<int>(clust.second);
                    clusters.erase(clust.first);
                    //std::cout << "Bumped " << clust.first << " to " << parent << std::endl;
                }
            }
            std::cout << "rev: " << (revenue) << std::endl;
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
            std::pair<int, int> prev_swap = {0, 0};
            bool halt = false;
            while((std::get<3>(search) > 0) && (convergence_time < MAX_ITER) && (!halt)) {
                convergence_time++;
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
                if((prev_swap != std::make_pair(x, c)) && (prev_swap != std::make_pair(c, x))) {
                    if((convergence_time % PRINT_PROGRESS) == 0) {
                        // << "Iteration " << convergence_time << " complete; increased revenue by " << std::get<3>(search) << " by swapping nodes " << c << " and " << prev << std::endl;
                        //print_tree_struct();
                    }
                    if((convergence_time % (MAX_ITER/10)) == 0) {
                        float prev_rev = revenue;
                        std::cout << "iteration " << convergence_time << " ";
                        calc_revenue();
                        if(revenue == prev_rev) {
                            halt = true;
                        }
                    }
                    update_w(x, c, prev);
                    update_struct(std::get<0>(search), x, c, std::get<2>(search));
                    //print_tree_struct();
                    //std::cout << "Subtree leaves: ";
                    //print_arr(subtree_leaves);
                    //updates heights and number of subtree leaves propagating upward
                    update_upwards(x);
                    search = greedy_search();
                }
                else {
                    halt = true;
                }
            }
            //profits << "0";
            std::cout << "rev: " << revenue << std::endl;
            std::cout << "iterations: " << convergence_time << std::endl;
            std::cout << "halted: " << halt << std::endl;
        }
        void update_w(int x, int c, int prev) {
            //t = 1 -> t'; t = 2 -> t''
            for(int i = 0; i < total_nodes; i++) {
                //typo in jowhari
                w[x][i] += (w[c][i] - w[prev][i]);
                w[i][x] = w[x][i];
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
            //edge records node number of y, whether x is y's first or second, 1 for T' and 2 for T'' as optimal interchange, and the max revenue increase
            std::tuple<int, int, int, float> output = {-1, -1, -1, 0};
            for(int i = 2; i < nodes_by_height.size(); i++) {
                for(auto iter = nodes_by_height[i].begin(); iter != nodes_by_height[i].end(); ++iter) {
                    int y = *iter;
                    int x = tree_struct[y].first;
                    int c = tree_struct[y].second;
                    for(int k = 0; k < 2; k++) {
                        if((c != -1) && (tree_struct[x].second != -1)) {
                            int a = tree_struct[x].first;
                            int b = tree_struct[x].second;
                            float t2 = ((subtree_leaves[a] * w[b][c]) - (subtree_leaves[c] * w[a][b]));
                            //std::cout << "switching " << c << " and " << a << " where y = " << y << " and x = " << x << " results in " << subtree_leaves[a] << "*" << (w[b][c]/scale_factor) << " - " << subtree_leaves[c] << "*" << (w[a][b]/scale_factor) << " = " << t2 << std::endl;
                            float t1 = ((subtree_leaves[b] * w[a][c]) - (subtree_leaves[c] * w[a][b]));
                            //std::cout << "switching " << c << " and " << b << " results in " << subtree_leaves[b] << "*" << w[a][c]/scale_factor << " - " << subtree_leaves[c] << "*" << w[a][b]/scale_factor << " = " << t1 << std::endl;
                            if(t1 > std::get<3>(output)) {
                                output = {y, k, 1, t1};
                            }
                            if(t2 > std::get<3>(output)) {
                                output = {y, k, 2, t2};
                            }
                        }
                        x = c;
                        c = tree_struct[y].first;
                    }
                }
            }
            return output;
        }
        void build_w() {
            scale_factor = 0;
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
                                if(node1 < node2) {
                                    scale_factor += w[node1][node2];
                                }
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
            scale_factor *= (num_leaves-2);
        }
        float euclidian_distance(int a, int b) {
            return std::pow(std::pow(nodes[b].first - nodes[a].first, 2) + std::pow(nodes[b].second - nodes[a].second, 2), 0.5);
        }
        float gaussian_kernel(int a, int b) {
            float sigma = 1;
            return std::exp(-1 * std::pow(euclidian_distance(a, b), 2) / (2 * std::pow(sigma, 2)));
        }
};