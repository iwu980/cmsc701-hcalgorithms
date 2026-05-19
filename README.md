# cmsc701-hcalgorithms

Hierarchical clustering is used in genomics to analyze genes by organizing data into a tree-like structure of clusters. 
In this project, we study an agglomerative local search algorithm for hierarchical clustering.
Agglomerative methods do clustering by assigning each data point to its own cluster, then merging clusters. 
The local search method, rather than merging nodes, uses a tree re-arrangement operation known as \textit{interchange} to swap two subtrees when the resulting tree is more profitable.
In this project, we implement [Jowhari's local search methodology of hierarchical clustering](https://arxiv.org/abs/2405.15983).
We compare the greedy variation of the local search algorithm to the average link, a common agglomerative clustering algorithm, by measure the revenue of the produced cluster. We demonstrate that our implementation does not match the revenue of average link, and analyze reasons why we fell short.

## Installation
### HD5
Instructions: https://bluebrain.github.io/HighFive/md__2home_2runner_2work_2_high_five_2_high_five_2doc_2installation.html

`sudo apt-get install libhdf5-dev`

Theoretically this is all you have to do. I did not install HighFive.

## Data download
See data/DOWNLOAD: download the indicated datasets via permalinks

## To build
`mkdir build`

initially:
`cd build ; cmake ../`

every build after, from repo directory:
`cmake --build build`

## To run
./build/greedyls <path to h5ad data>

## Average-link
```
pip install matplotlib scipy
```

`python3 src/average_link.py <path to h5ad data>`

`python3 src/average_link_all.py` will run average_link across all datasets found in the data directory
