# Run AL on all datasets
import glob
import os
import subprocess
import average_link

data_dir = r"data/"
num_runs = 5

for fp in glob.glob(data_dir + "*.h5ad"):
    if os.path.isfile(fp):
        print(f"====={os.path.basename(fp)}=======")
        AL = []
        for i in range(num_runs):
            AL.append(average_link.average_link(fp))
        print("Max revenue = ", max(AL))
        print("Average revenue = ", sum(AL) / len(AL))
        
            