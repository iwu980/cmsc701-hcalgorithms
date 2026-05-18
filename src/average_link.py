import sys
import anndata
import scanpy as sc
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.cluster.hierarchy import linkage, dendrogram
from sklearn.metrics.pairwise import rbf_kernel

"""
This script loads h5ad data, does HC using average linkage, and calculates the MW revenue.

$python3 src/average_link.py data/c6ea3545-9200-4497-8591-08f687626182.h5ad 

SOURCES: 
https://scverse-tutorials.readthedocs.io/en/latest/notebooks/anndata_getting_started.html
https://github.com/MajiidJafarii/Dasgupta-s-Cost-Function-And-Moseley-Wang-Revenue-Function/blob/main/main.ipynb
"""

# Define a Node class to hold tree information
class Node:
    def __init__(self, left=None, right=None, data=None, dist=0.0):
        self.left = left         # left child
        self.right = right       # right child
        self.data = data         # for leaves: holds the label; for internals: None
        self.dist = dist         # the merge distance (for internal nodes)
        if left is None and right is None:
            self.n_leaves = 1
            self.leaf_indices = []  # will store the original index for leaves
        else:
            self.n_leaves = 0
            self.leaf_indices = []
    def __repr__(self):
        if self.data is not None:
            return f"{self.data}"
        else:
            return self.n_leaves , self.dist

def find_path(root, target, cache):
    if (root, target) in cache:
        return cache[root, target]
    if root == target:
        cache[(root, target)] = [root]
        return [root]
    if root.left is not None:
        left_path = find_path(root.left, target, cache)
        if left_path is not None:
            cache[(root, target)] = [root] + left_path
            return [root] + left_path
    if root.right is not None:
        right_path = find_path(root.right, target, cache)
        if right_path is not None:
            cache[(root, target)] = [root] + right_path
            return [root] + right_path
    return None

def lca(root, leaf1, leaf2, path_cache):
    path1 = find_path(root, leaf1, path_cache)
    path2 = find_path(root, leaf2, path_cache)
    lca_node = None
    for n1, n2 in zip(path1, path2):
        if n1 == n2:
            lca_node = n1
        else:
            break
    return lca_node


def build_tree(Z, labels):
    n = len(labels)
    nodes = {}
    # Create leaf nodes for indices 0 to n-1
    for i in range(n):
        node = Node(data=labels[i], dist=0.0)
        node.leaf_indices = [i]
        nodes[i] = node
    # Each row in Z represents a merge of two clusters
    for i, row in enumerate(Z):
        idx1, idx2, dist, count = row
        idx1, idx2 = int(idx1), int(idx2)
        new_node = Node(left=nodes[idx1], right=nodes[idx2], dist=dist)
        new_node.leaf_indices = nodes[idx1].leaf_indices + nodes[idx2].leaf_indices
        new_node.n_leaves = len(new_node.leaf_indices)
        nodes[n + i] = new_node
    root = nodes[max(nodes.keys())]
    return root, nodes

def calculate_metric(df, gamma, root, nodes, metric):
    W = rbf_kernel(df.values, gamma=gamma)  # Constructing a Gaussian similarity matrix
    n = W.shape[0]
    result = 0.0
    cache = {} # cache results of find_path
    for i in range(n):
        if i % 100 == 0:
            print("Calculate metric...", i)
        for j in range(i + 1, n):
            n_leaves = lca(root, nodes[i], nodes[j], cache).n_leaves
            result += W[i, j] * metric(n, n_leaves)
    return result

if __name__ == "__main__":
    datapath = "data/c6ea3545-9200-4497-8591-08f687626182.h5ad"
    if len(sys.argv) > 1:
        datapath = sys.argv[1]
    adata = anndata.read_h5ad(datapath)

    print("Loading dataframe ...")
    umap_df = pd.DataFrame(
        adata.obsm['X_umap'], 
        index=adata.obs_names, 
        columns=['UMAP_1', 'UMAP_2']
    )
    df= umap_df.sample(n=1000) # NOTE: sample 1000 cells from the dataframe ... takes about 5 minutes to run on Caspar's machine

    print("Computing linkage...")
    labels = [f"Sample {i}" for i in range(len(df))]
    Z = linkage(df.values, method='average')
    root, nodes = build_tree(Z, labels)

    plt.figure(figsize=(18, 9))
    data = dendrogram(Z, labels=labels, leaf_rotation=90)
    plt.title("Dendrogram (Average Linkage)")
    plt.xlabel("Samples")
    plt.ylabel("Distance")
    plt.savefig("revenue.png")

    print("Computing MW revenue...")
    gamma = 1
    revenue = calculate_metric(df, gamma, root, nodes, metric=lambda n, n_leaves: n - n_leaves)
    print(f"Gamma: {gamma:.1f}, Revenue_Moseley_Wang: {revenue:.2f}")