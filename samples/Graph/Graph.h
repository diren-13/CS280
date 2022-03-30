/************************************************************************************//*!
 @file          Graph.h
 @author        Diren (diren.dbharwani@digipen.edu)
 @brief         Interface of an Graph of integers.
 @date          2022-03-02
 
 @copyright     Copyright (c) 2022
*//*************************************************************************************/

#include <list>

class Graph
{
public:
    Graph(int vertexCount);
    ~Graph();

    void AddEdge(int vertex, int vertexToLink);

    void BFS(int vertex);
    void DFS(int vertex);

private:
    int             numVertices;
    std::list<int>* adjacencyList;

    void dfs(int vertex, bool* visitedList);
};

/*
    // An EXAMPLE of some other classes you may want to create and 
    // implement in ALGraph.cpp
    class GNode; 

    class GEdge;

    struct AdjInfo
    {
    public:
        GNode*      node;
        unsigned    weight;
        unsigned    cost;
        
        AdjInfo();

        bool operator<(const AdjInfo& rhs) const;
        bool operator>(const AdjInfo& rhs) const;
    };
    
    // Other private fields and methods
*/