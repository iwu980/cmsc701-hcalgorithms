//C++ implementation of Jowhari's greedy local search algorithm
#include "LSTree.cpp"
#include <hdf5.h>
#include <vector>
#include <iostream>

int main(int argc, char* argv[]) {
    const char* filepath = "data/c6ea3545-9200-4497-8591-08f687626182.h5ad"; // default data
    if (argc > 1) {
        filepath = argv[1];
    }
    std::cout << "Loading file: " << filepath << "\n";
    hid_t file = H5Fopen(filepath, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file < 0) { std::cerr << "Error opening file\n"; return 1; }
    
    // Open the umap embedding: https://github.com/chanzuckerberg/single-cell-curation/blob/main/schema/7.0.0/schema.md#obsm-embeddings
    hid_t dataset = H5Dopen(file, "/obsm/X_umap", H5P_DEFAULT);
    if (dataset < 0) {
        std::cerr << "Error: /obsm/X_umap not found. Check if UMAP was calculated." << std::endl;
        H5Fclose(file);
        return 1;
    }

    // Query the dataspace geometry to get number of cells and embedding dimensions
    hid_t space = H5Dget_space(dataset);
    hsize_t dims[2]; 
    int rank = H5Sget_simple_extent_dims(space, dims, NULL);

    if (rank != 2) {
        std::cerr << "Error: Expected a 2D matrix for UMAP, found rank " << rank << std::endl;
        H5Sclose(space); H5Dclose(dataset); H5Fclose(file);
        return 1;
    }

    size_t num_cells = dims[0];
    size_t num_dims = dims[1]; // Almost always 2 (X and Y coordinates)

    std::cout << "Loading UMAP dataset..." << std::endl;
    std::cout << "Cells: " << num_cells << " | Dimensions: " << num_dims << std::endl;

    // Read raw data into contiguous float vector buffer 
    std::vector<float> umap_coords(num_cells * num_dims);
    herr_t status = H5Dread(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, umap_coords.data());
    if (status < 0) {
        std::cerr << "Error reading UMAP numerical blocks." << std::endl;
    } else {
        std::cout << "Successfully loaded UMAP coordinates!" << std::endl;
    }

    // initialize LSTree with vector
    num_cells = 25; // NOTE: for the sake of testing, we will start with pretending there are <100 cells
    std::vector<std::pair<float, float>> leaves;
    leaves.reserve(num_cells);
    for (int i = 0; i < num_cells; i++) {
        float x = umap_coords[i * num_dims + 0];
        float y = umap_coords[i * num_dims + 1];
        leaves.push_back(std::pair(x, y));
    } 
    LSTree(leaves.size(), leaves);

    H5Sclose(space);
    H5Dclose(dataset);
    H5Fclose(file);
    return 0;
}