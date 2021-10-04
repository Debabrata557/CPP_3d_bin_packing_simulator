filepath="/home/achme/CPP_3d_bin_packing_simulator/test_results.txt"
#filepath="/home/achme/test_results.txt"
import matplotlib.pyplot as plt
import numpy as np
with open(filepath) as file:
    lines = file.readlines()
    a=[]
    for line in lines:
        a.append(float(line))
    a=np.array(a)
    plt.plot(a)
    plt.show()
