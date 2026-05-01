import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("trajectoire.txt")
xa, ya, xs, ys = data[:, 0], data[:, 1], data[:, 2], data[:, 3]

plt.plot(xa, ya, label = "Astéroïde", color = "blue")
plt.plot(xs, ys, label = "Satellite", color = "red")
plt.xlabel("x (km)")
plt.ylabel("y (km)")
plt.gca().set_aspect('equal')
plt.legend()
plt.show()

