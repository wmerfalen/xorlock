#!/usr/bin/env python

import math
import sys
import random

use_rand_pc_pos = False

for i in range(50):
    print('')

# Define a map of 50x50
w, h = 50, 50

# matrix is 0,0 at top left corner
matrix = [['.' for x in range(w)] for y in range(h)]

# Place a wall at 0,10 that is 25 wide
for y in range(25):
    matrix[y][10] = 'x'
for y in range(25):
    matrix[y][11] = 'x'

# Place a wall at 20,20
y = 20
while y < w:
    matrix[y][20] = 'x'
    y += 1
y = 20
while y < w:
    matrix[y][21] = 'x'
    y += 1

# The NPC and the PC positions
npc_pos = [0,0]
pc_pos = [35,25]

if use_rand_pc_pos:
    random.seed()
    pc_pos = [random.randrange(20,w),random.randrange(20,h)]
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

east_escape = -1
def should_go_east(x,y):
    east_escape = -1
    path_x = x
    while path_x < w and matrix[path_x][y] == 'x':
        path_x += 1
        if matrix[path_x][y+1] != 'x':
            east_escape = path_x
            return True
    return False

west_escape = -1
def should_go_west(x,y):
    west_escape = -1
    path_x, path_y = x, y
    while path_x >= 0 and matrix[path_x][path_y] == 'x':
        path_x -= 1
        if matrix[path_x][path_y+1] != 'x':
            west_escape = path_x
            return True
    return False

matrix[npc_pos[0]][npc_pos[1]] = 'N'
matrix[pc_pos[0]][pc_pos[1]] = 'P'

def get_unimpeded_path(src_x,src_y,dst_x,dst_y,depth=0):
    path = []
    path_x,path_y = src_x, src_y
    corrections = 0
    while path_x != dst_x or path_y != dst_y:
        dir = next_node(path_x,path_y,dst_x,dst_y)
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
        if matrix[path_x][path_y] == 'x':
            corrections += 1
            if dir == 's':
                # if we can go west, try it
                if matrix[path_x-1][path_y] != 'x':
                    path_x -= 1
                # if we can go east, try it
                elif matrix[path_x+1][path_y-1] != 'x':
                    path_x += 1
                    path_y -= 1
                # if we can backtrack and go back north, do it
                elif matrix[path_x][path_y-1] != 'x':
                    path_y -= 2
            elif dir == 'se':
                # if we can go east
                if matrix[path_x+1][path_y-1] != 'x':
                    path_x += 1
                    path_y += 2
                elif matrix[path_x][path_y-y] != 'x':
                    path_y -= 1
            else:
                print('none')
        if corrections >= 4:
            return get_unimpeded_path(src_x,src_y,path_x,path_y)
        path.append([path_x,path_y])
    return path

path = get_unimpeded_path(npc_pos[0],npc_pos[1],pc_pos[0],pc_pos[1])
last_x, last_y = -1, -1
while last_x != pc_pos[0] and last_y != pc_pos[1]:
    for p in path:
        if matrix[p[0]][p[1]] == 'P':
            break
        matrix[p[0]][p[1]] = '@'
        last_x = p[0]
        last_y = p[1]
    path = get_unimpeded_path(last_x,last_y,pc_pos[0],pc_pos[1])

for y in range(h):
    for x in range(w):
        print(matrix[x][y],end='')
    print('')
    if y > 40:
        break
