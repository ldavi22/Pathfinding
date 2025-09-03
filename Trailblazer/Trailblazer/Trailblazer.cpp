/******************************************************************************
 * File: Trailblazer.cpp
 *
 * Implementation of the graph algorithms that comprise the Trailblazer
 * assignment.
 */

#include "Trailblazer.h"
#include "TrailblazerGraphics.h"
#include "TrailblazerTypes.h"
#include "TrailblazerPQueue.h"
#include "map.h"
#include "foreach.h"
#include "random.h"
using namespace std;

/* Function: shortestPath
 * 
 * Finds the shortest path between the locations given by start and end in the
 * specified world.	 The cost of moving from one edge to the next is specified
 * by the given cost function.	The resulting path is then returned as a
 * Vector<Loc> containing the locations to visit in the order in which they
 * would be visited.	If no path is found, this function should report an
 * error.
 *
 * In Part Two of this assignment, you will need to add an additional parameter
 * to this function that represents the heuristic to use while performing the
 * search.  Make sure to update both this implementation prototype and the
 * function prototype in Trailblazer.h.
 */
Loc findParent(Map<Loc, Loc>& parent, Loc loc) {
    if (parent[loc] != loc) {
        parent[loc] = findParent(parent, parent[loc]); 
    }
    return parent[loc];
}

void unionSets(Map<Loc, Loc>& parent, Loc a, Loc b) {
    Loc rootA = findParent(parent, a);
    Loc rootB = findParent(parent, b);
    if (rootA != rootB) {
        parent[rootB] = rootA; 
    }
}

Vector<Loc> getNeighbours(Loc loc, Grid<double>& world) {
    Vector<Loc> neighbors;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;  

            int newRow = loc.row + dy;
            int newCol = loc.col + dx;

            if (world.inBounds(newRow, newCol)) {
                neighbors.add({ newRow, newCol });
            }
        }
    }

    return neighbors;
}



Vector<Loc> shortestPath(Loc start, Loc end, Grid<double>& world,
    double costFn(Loc from, Loc to, Grid<double>& world), double heuristic(Loc start, Loc end, Grid<double>& world)) {

    Map<Loc, Color> stateOfNode;
    TrailblazerPQueue<Loc> pq;
    Map<Loc, Loc> parent;
    Map<Loc, double> dist;
    Vector<Loc> path;
    Loc final;

    for (int row = 0; row < world.numCols(); row++) {
        for (int col = 0; col < world.numRows(); col++) {
            Loc node = { row, col };
            stateOfNode[node] = GRAY;
            dist[node] = INT_MAX;
        }
    }

    stateOfNode[start] = YELLOW;
    colorCell(world, start, YELLOW);
    dist[start] = 0;

    pq.enqueue(start, 0);

    while (!pq.isEmpty()) {
        Loc curr = pq.dequeueMin();
        stateOfNode[curr] = GREEN;
        colorCell(world, curr, GREEN);

        Vector<Loc> neighbours = getNeighbours(curr,world);
        if (curr == end) {
            final = curr;
            break;
        }

        foreach(Loc node in neighbours) {
            double currDist = dist[curr] + costFn(curr, node, world);
            if (stateOfNode[node] == GRAY) {
                stateOfNode[node] = YELLOW;
                colorCell(world, node, YELLOW);

                dist[node] =currDist;
                parent[node] = curr;
                pq.enqueue(node, currDist+ heuristic(node,end,world));
            }
            else if(stateOfNode[node] == YELLOW && dist[node] > currDist) {
                dist[node] = currDist;
                parent[node] = curr;
                pq.decreaseKey(node, currDist + heuristic(node,end,world));
            }
        }

    }
    while (final != start) {
        path.insert(0, final);
        final = parent[final];  
    }
    path.insert(0, start);

    return path;
}

Set<Edge> createMaze(int numRows, int numCols) {
    Set<Edge> result;
    Map<Loc, Loc> parent;
    TrailblazerPQueue<Edge> pq;

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            Loc node = { row,col };
            parent[node] = node;
        }
    }

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            Loc curr = { row,col };
            
            if (col + 1 < numCols) {
                Loc right = { row, col + 1 };
                Edge e = {curr,right};
                double weight = randomReal(0.0, 1.0);
                pq.enqueue(e, weight);
            }

            if (row + 1 < numCols) {
                Loc right = { row + 1, col};
                Edge e = {curr,right};
                double weight = randomReal(0.0, 1.0);
                pq.enqueue(e, weight);
            }

        }
    }

    while (!pq.isEmpty()) {
        Edge e = pq.dequeueMin();
        Loc init = e.start;
        Loc final = e.end;

        if (findParent(parent,init) != findParent(parent,final)) {
            result.add(e);
            unionSets(parent, init, final);
        }
    }
    return result;
}
