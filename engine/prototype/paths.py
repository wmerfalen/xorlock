#!/usr/bin/env python

import math
import random

# Define a map of 50x50
w, h = 50, 50

# matrix is 0,0 at top left corner
matrix = [['.' for x in range(w)] for y in range(h)]

# Place a wall at 0,10 that is 25 wide
for x in range(25):
    matrix[10][x] = 'x'

# The NPC and the PC positions
npc_pos = [0,0]
pc_pos = [0,0]


# calculate distance between two points
def distance(x1,y1,x2,y2):
    a = x1 - x2
    b = y1 - y2
    return math.sqrt(a*a + b*b)

# Define optimal path which is almost always a straight line

def next_node(x,y,dest_x,dest_y):
    north = distance(x,y-1,dest_x,dest_y)
    south = distance(x,y+1,dest_x,dest_y)
    east = distance(x+1,y,dest_x,dest_y)
    west = distance(x-1,y,dest_x,dest_y)
    north_east = distance(x+1,y-1,dest_x,dest_y)
    north_west = distance(x-1,y-1,dest_x,dest_y)
    south_east = distance(x+1,y+1,dest_x,dest_y)
    south_west = distance(x-1,y+1,dest_x,dest_y)
    dict = {'n': north,'e': east, 'w': west, 's': south, 'se': south_east, 'sw': south_west, 'ne': north_east, 'nw': north_west}
    choice = 'x'
    shortest = 999
    for direction,dist in dict.items():
        if dist < shortest:
            choice = direction
            shortest = dist
    return choice

random.seed()
npc_pos[0] = random.randrange(0,w)
npc_pos[1] = random.randrange(0,h)

pc_pos[0] = random.randrange(0,w)
pc_pos[1] = random.randrange(0,h)

matrix[npc_pos[0]][npc_pos[1]] = 'N'
matrix[pc_pos[0]][pc_pos[1]] = 'P'



path_x,path_y = npc_pos[0], npc_pos[1]
while path_x != pc_pos[0] or path_y != pc_pos[1]:
    dir = next_node(path_x,path_y,pc_pos[0],pc_pos[1])
    if dir == 'x':
        break
    if dir == 'n':
        path_y -= 1
    elif dir == 'e':
        path_x += 1
    elif dir == 'w':
        path_x -= 1
    elif dir == 's':
        path_y += 1
    elif dir == 'ne':
        path_x += 1
        path_y -= 1
    elif dir == 'nw':
        path_x -= 1
        path_y -= 1
    elif dir == 'se':
        path_x += 1
        path_y += 1
    elif dir == 'sw':
        path_x -= 1
        path_y += 1
    if path_x == pc_pos[0] and path_y == pc_pos[1]:
        matrix[path_x][path_y] = 'P'
    else:
        matrix[path_x][path_y] = '$'

for x in range(w):
    for y in range(h):
        print(matrix[x][y],end='')
    print('')
