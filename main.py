# You are given a map of a region's coordinate graph that is represented as an NxM matrix. Each index represents a zone. The 1’s represent zones that have signal for communications and 0’s represent zones that have no signal. A Ground Control Station is a device that provides situational awareness to operators and can only be placed in a zone with signal. The area of effect for a GCS will cover any connecting zone(s) with signal.

# Build a solution to find the minimum number of GCS's needed to fully service a given region.

# input:
# [[1, 1, 0, 0, 1, 1, 0],
#  [1, 0, 1, 0, 1, 1, 0],
#  [1, 1, 0, 0, 0, 1, 0],
#  [1, 1, 0, 0, 1, 1, 0]]

# M = rows
# N = cols

# GCS - ground station

# connected component "chunk"

# game plan

# 12:26pm

# iterate through each cell of the matrix M rows and N columns
# i,j

# i row
# j col

# seen - set of (i,j) pairs

# for each i,j candidate if the cell is 0 -- no-op
#
# if cell is 1, then we check if its already seen -- if so skip -- no-op

# flood fill the "island" and increment count of grounds stations by 1

# 12:28pm - 12:35pm => ~7 minutes implementation

# 12:44pm - part 2

# area is total number of zones

# [[1, 1, 0, 0, 1, 1, 0],
#  [1, 0, 1, 0, 1, 1, 0],
#  [1, 1, 0, 0, 0, 1, 0],
#  [1, 1, 0, 0, 1, 1, 0]]

# island 0 has area 7
# island 1 has area 1
# island 2 has area 7

# output map of key/value pairs

# m is our map m[0] = 7, m[1] = 1, m[2] = 7

# map key is color

# rename function color -> go()

# game plan part 2 -- add the map and increment the count each time a addional
# cell is visited/colored

def min_ground_stations(A):
    """
    @param A is a input grid which is NOT empty and is comprised of M rows and N columns
    @return count which represents the minimum amount of ground stations required to cover the grid
    """
    color = 0 # monotonically increasing value used to uniquely identify connected compoents "islands"
    m = {}
    seen = set()
    M = len(A)
    N = len(A[0]) # fail if A is empty -- corner case assume this is not possible to be empty
    # set colored "islands" to 0
    # i,j is current cell -> u,v adjacent cells
    def go(i, j, color):
        nonlocal A, seen, m
        for u, v in [(i - 1, j), (i, j + 1), (i + 1, j), (i, j - 1)]:
            if 0 <= u < M and 0 <= v < N and A[u][v] == 1 and (u, v) not in seen:
                m[color] += 1 # additional surface area of the same connected component
                seen.add((u, v))
                go(u, v, color)
    for i in range(M):
        for j in range(N):
            if A[i][j] == 1 and (i, j) not in seen:
                seen.add((i, j))
                m[color] = 1 # the first time a connected component is seen
                go(i, j, color)
                color += 1
    return m

# input:
# [[1, 1, 0, 0, 1, 1, 0],
#  [1, 0, 1, 0, 1, 1, 0],
#  [1, 1, 0, 0, 0, 1, 0],
#  [1, 1, 0, 0, 1, 1, 0]]

A = [[1, 1, 0, 0, 1, 1, 0],
     [1, 0, 1, 0, 1, 1, 0],
     [1, 1, 0, 0, 0, 1, 0],
     [1, 1, 0, 0, 1, 1, 0]]

m = min_ground_stations(A)

assert(len(m.keys()) == 3)
for color in [0, 1, 2]:
    print(f'm[{color}] = {m[color]}')

m = min_ground_stations([[0]])
assert(len(m.keys()) == 0)

m = min_ground_stations([[1]])
assert(len(m.keys()) == 1)
print(f'm[0] = {m[0]}')
