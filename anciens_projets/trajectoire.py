import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("trajectoire.txt")
x, y = data[:, 0], data[:, 1]

plt.plot(x, y, label = "Trajectoire de l'astéroïde")
plt.xlabel("x (km)")
plt.ylabel("y (km)")
plt.gca().set_aspect('equal')
plt.legend()
plt.show()
