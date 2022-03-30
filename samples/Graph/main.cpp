#include <iostream>
#include "Graph.h"

int main()
{
    Graph g{6};

    g.AddEdge(0,1);
    g.AddEdge(0,4);

    g.AddEdge(1,0);
    g.AddEdge(1,2);
    g.AddEdge(1,4);

    g.AddEdge(2,1);
    g.AddEdge(2,3);

    g.AddEdge(3,2);
    g.AddEdge(3,4);
    g.AddEdge(3,5);

    g.AddEdge(4,0);
    g.AddEdge(4,1);
    g.AddEdge(4,3);

    g.AddEdge(5,3);

    // std::cout << "BFS from 0: "; g.BFS(0);
    // std::cout << "BFS from 5: "; g.BFS(5);
    // std::cout << "BFS from 2: "; g.BFS(2);

    std::cout << "DFS from 0: "; g.DFS(0);
    // std::cout << "DFS from 5: "; g.DFS(5);
    // std::cout << "DFS from 4: "; g.DFS(4);

    // Graph g{7};

    // g.AddEdge(0, 1);
    // g.AddEdge(0, 5);

    // g.AddEdge(1, 2);

    // g.AddEdge(2, 3);

    // g.AddEdge(3, 4); 
    // g.AddEdge(3, 5);

    // g.AddEdge(4, 6);

    // g.AddEdge(5, 4);
    // g.AddEdge(5, 6);

    // g.AddEdge(6, 2);

    // std::cout << "DFS from 0: "; g.DFS(0);
}