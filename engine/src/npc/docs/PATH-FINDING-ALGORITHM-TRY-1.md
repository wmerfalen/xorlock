# Variables
*N*: The NPC. The one doing the path-finding
*P*: The player. The target to be found.
*G*: Gateway. Used in the weighted graph we create with each map
*W*: Pathway. Created through loosely defined paths between gateways.

# Assumptions:
NPC will know where the PC is at at all times.

# System Purpose:
Find a path between NPC and PC in such a way that it allows the NPC to make it's way to the PC despite obstacles.

# MAP

```
===================================================
|                         [WWW]
| [N]                     [WWW]
|                         [WWW]
|xxxxxxxxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|xxxxxxxxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|xxxxxxxxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|                         [WWW]
|    [P]                  [WWW]
|                         [WWW]
```

## Step 1: Get heading
- NPC determines from PC's position which 2 directions it will have to go in order to arrive at PC
- It determines: South and East
```
===================================================
|                         [WWW]
| [N]                     [WWW]
|  |                      [WWW]
|xx|xxxxxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|xx|xxxxxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|xx|xxxxxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|  |                      [WWW]
|  --[P]                  [WWW]
|                         [WWW]
```

## Step 2: derive direct line
- NPC derives a line from it's position to PC's position
```
===================================================
|                         [WWW]
| [N]                     [WWW]
|  \                      [WWW]
|xx \ xxxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|xxx \ xxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|xxxx | xxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|     |                   [WWW]
|    [P]                  [WWW]
|                         [WWW]
```

## Step 3: detect walls
- NPC crawls the direct line drawn in Step 2 and detects the first wall
```
===================================================
|                         [WWW]
| [N]                     [WWW]
|  \                      [WWW]
|xx @ xxxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|xxx @ xxxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|xxxx @ xxxxxxxxxxxxxx|[G][WWW][G]|xxxxxxxxxxxxxxxxx
|     |                   [WWW]
|    [P]                  [WWW]
|                         [WWW]
```

## Step 4: find nearest gateway
- NPC creates a second path that now includes the closest Gateway
- The NPC decides that gateway 1 is the closest
- It creates a path to that gateway
```
===================================================
|                       
| [N]-------------------+      
|                       |      
|xxxxxxxxxxxxxxxxxxxxx|*1*[WWW][G]|xxxxxxxxxxxxxxxxx
|xxxxxxxxxxxxxxxxxxxxx|*2*[WWW][G]|xxxxxxxxxxxxxxxxx
|xxxxxxxxxxxxxxxxxxxxx|*3*[WWW][G]|xxxxxxxxxxxxxxxxx
|                         [WWW]
|    [P]                  [WWW]
|                         [WWW]
```

## Step 5: draw direct line to target
- NPC draws a direct line to the target
- It notices that the line contains a barrier
- It then creates a path to the nearest gateway that is to the south
  - since this is the direction that is trying to be overcome, south is the choice for the next gateway
- It chooses gateway 2, then repeating this step, chooses gateway 3
```
===================================================
|                       
| [N]-------------------+      
|                       |      
|xxxxxxxxxxxxxxxxxxxxx|*|*[WWW][G]|xxxxxxxxxxxxxxxxx
|xxxxxxxxxxxxxxxxxxxxx|*|*[WWW][G]|xxxxxxxxxxxxxxxxx
|xxxxxxxxxxxxxxxxxxxxx|[N][WWW][G]|xxxxxxxxxxxxxxxxx
|                         [WWW]
|    [P]                  [WWW]
|                         [WWW]
```

## Step 6: get heading
- Since the NPC's position in relation to the PC has changed, the NPC reacquires it's heading from Gateway 3
- Using the two direction system, it extracts two directions: South, West

## Step 7: attempt worst case right angle
- The NPC tries to draw a direct line to the PC, but it's still not quite there
- Instead, it draws a line south followed by a line west
- It then crawls this line and determines that there are no obstacles in the way

```
===================================================
|                       
| [N]-------------------+      
|                       |      
|xxxxxxxxxxxxxxxxxxxxx|*|*[WWW][G]|xxxxxxxxxxxxxxxxx
|xxxxxxxxxxxxxxxxxxxxx|*|*[WWW][G]|xxxxxxxxxxxxxxxxx
|xxxxxxxxxxxxxxxxxxxxx|*|*[WWW][G]|xxxxxxxxxxxxxxxxx
|                       | [WWW]
|    [P]----------------+ [WWW]
|                         [WWW]
```
