#!/usr/bin/python

import yaml
import cartopy.crs as ccrs 
from sys import argv
import matplotlib.pyplot as plt
from numpy import array
import os

filename = 'tsp.yaml'
if len(argv) > 1 and not argv[1].isdigit():
    filename = argv[1]
    
data = yaml.load(file(filename))
cities = data['cities']
positions = [data['positions'][city] for city in cities]

# read passed order
order = []
for i in argv[1:]:
    try:
        order.append(int(i))
    except:
        pass
for i in range(len(cities)):
    if i not in order:
        order.append(i)
order.append(order[0])

# display map
proj = ccrs.PlateCarree()

plt.close('all')
plt.figure(figsize=(15,8))
ax = plt.axes(projection=proj)

im = plt.imread(os.path.abspath(os.path.dirname(__file__)) + '/earth_2400.jpg')
img_extent = (-180, 180, -90, 90)
ax.imshow(im, origin='upper', extent=img_extent, transform=proj)
plt.tight_layout()

travel = array([positions[o] for o in order])
plt.plot(travel[:,1],travel[:,0],'r-D',transform=ccrs.Geodetic())

plt.show()
