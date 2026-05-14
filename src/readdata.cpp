#include <hdf5.h>
#include <vector>
#include <iostream>

int main() {
    // Open the H5AD file
    hid_t file = H5Fopen("data/c6ea3545-9200-4497-8591-08f687626182.h5ad", H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file < 0) { std::cerr << "Error opening file\n"; return 1; }

    // Open /X as a group, NOT a dataset
    hid_t x_group = H5Gopen(file, "/X", H5P_DEFAULT);
    if (x_group < 0) {
        std::cerr << "Error: /X is missing or structured differently.\n";
        H5Fclose(file);
        return 1;
    }

    // 1. Open and read the sparse non-zero array values
    hid_t d_dataset = H5Dopen(x_group, "data", H5P_DEFAULT);
    hid_t d_space = H5Dget_space(d_dataset);
    hsize_t data_dims[1];
    H5Sget_simple_extent_dims(d_space, data_dims, NULL);
    
    std::vector<float> values(data_dims[0]);
    H5Dread(d_dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, values.data());

    // 2. Open and read the column/row indices mapping
    hid_t i_dataset = H5Dopen(x_group, "indices", H5P_DEFAULT);
    hid_t i_space = H5Dget_space(i_dataset);
    hsize_t index_dims[1];
    H5Sget_simple_extent_dims(i_space, index_dims, NULL);
    
    std::vector<int32_t> indices(index_dims[0]); // Can also be int64_t based on file
    H5Dread(i_dataset, H5T_NATIVE_INT32, H5S_ALL, H5S_ALL, H5P_DEFAULT, indices.data());

    // 3. Open and read index pointers (boundaries)
    hid_t p_dataset = H5Dopen(x_group, "indptr", H5P_DEFAULT);
    hid_t p_space = H5Dget_space(p_dataset);
    hsize_t ptr_dims[1];
    H5Sget_simple_extent_dims(p_space, ptr_dims, NULL);
    
    std::vector<int32_t> indptr(ptr_dims[0]);
    H5Dread(p_dataset, H5T_NATIVE_INT32, H5S_ALL, H5S_ALL, H5P_DEFAULT, indptr.data());

    std::cout << "Successfully read sparse /X matrix metadata!" << std::endl;
    std::cout << "Non-zero values count: " << values.size() << std::endl;

    // Cleanup resources
    H5Sclose(d_space); H5Dclose(d_dataset);
    H5Sclose(i_space); H5Dclose(i_dataset);
    H5Sclose(p_space); H5Dclose(p_dataset);
    H5Gclose(x_group);
    H5Fclose(file);
    return 0;
}
