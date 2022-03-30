/************************************************************************************//*!
 @file          Graph.cpp
 @author        Diren (diren.dbharwani@digipen.edu)
 @brief         Implementation of an Graph of integers.
 @date          2022-03-02
 
 @copyright     Copyright (c) 2022
*//*************************************************************************************/

#include "Graph.h"

#include <iostream>

Graph::Graph(int vertexCount)
: numVertices(vertexCount)
{
    adjacencyList = new std::list<int>[vertexCount];
}

Graph::~Graph()
{
    delete[] adjacencyList;
}

void Graph::AddEdge(int vertex, int vertexToLink)
{
    adjacencyList[vertex].push_back(vertexToLink);
}

void Graph::BFS(int vertex)
{
    // store bool if visited
    bool* visited = new bool[numVertices];
    for (int i = 0; i < numVertices; ++i)
    {
        visited[i] = false;
    }

    std::list<int> queue;
    visited[vertex] = true;

    queue.push_back(vertex);

    while (!queue.empty())
    {
        // Dequeue a vertex from the queue and print
        vertex = queue.front();
        std::cout << vertex << " ";
        queue.pop_front();

        for (auto it = adjacencyList[vertex].begin(); it != adjacencyList[vertex].end(); ++it)
        {
            if (!visited[*it])
            {
                visited[*it] = true;
                queue.push_back(*it);
            }
        }
    }

    std::cout << std::endl;
    delete[] visited;
}

void Graph::DFS(int vertex)
{
    bool* visited = new bool[numVertices];
    for (int i = 0; i < numVertices; ++i)
    {
        visited[i] = false;
    }

    dfs(vertex, visited);

    std::cout << std::endl;
    delete[] visited;
}

void Graph::dfs(int vertex, bool* visitedList)
{
    visitedList[vertex] = true;
    std::cout << vertex << " ";

    for (auto it = adjacencyList[vertex].begin(); it != adjacencyList[vertex].end(); ++it)
    {
        if (!visitedList[*it])
        {
            dfs(*it, visitedList);
        }
    }
}