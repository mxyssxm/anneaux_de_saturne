import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("trajectoire_multi.txt")
sat_x, sat_y = data[:, 0], data[:, 1] 
num_aste = (data.shape[1]-2)//2

plt.plot(sat_x, sat_y, label = "Satellite", color = "blue")

for i in range(num_aste):
    ast_x = data[:, 2 + i*2]
    ast_y = data[:, 3 + i*2]
    plt.plot(ast_x, ast_y, label = f"Astéroïde {i+1}")

plt.scatter(0, 0, color = "orange", marker = "o", s = 200, label = "Saturne")

plt.xlabel("x (km)")
plt.ylabel("y (km)")
plt.title("Trajectoire des astéroïdes autour de Saturne")
plt.gca().set_aspect('equal')
plt.legend()
plt.show()