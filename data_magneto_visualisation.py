import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Read the data from the CSV file
data = pd.read_csv('data.csv', delimiter='\t', header=None)

# Extract x, y, and z coordinates
x = data.iloc[:, 0]
y = data.iloc[:, 1]
z = data.iloc[:, 2]

# Plotting
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Plot the points
ax.scatter(x, y, z, c='r', marker='o')

# Set labels and title
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('3D Scatter Plot')

# Show plot
plt.show()
