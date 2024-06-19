#ifndef PATH_H
#define PATH_H

#include <climits>
#include "map.h"

class path {
    private:
        heap_node_t *heap_node; // The pointer node in heap/priority queue
        coordinate_t coordinate;
        terrain_e terrain;
        int terrain_cost;
        int cost; // Dijkstra's path cost at node
    public:
        path() : coordinate()
        {
            heap_node = nullptr;
            coordinate.y = 0;
            coordinate.x = 0;
            terrain = edge;
            terrain_cost = INT_MAX;
            cost = INT_MAX;
        }
        ~path() {};
        
        static int calculate_terrain_cost(terrain_e terrain_type, trainer_type_e trainer_type);
        static void dijkstra_path(heap_t *heap, map *map, path path_map[MAP_HEIGHT][MAP_WIDTH], trainer_type_e trainer_type);

        void set_heap_node(heap_node_t *node) { heap_node = node; }
        heap_node_t *get_heap_node() { return heap_node; }
        void set_coordinate(coordinate_t c) { coordinate = c; }
        coordinate_t get_coordinate() { return coordinate; }
        void set_terrain(terrain_e t) { terrain = t; }
        terrain_e get_terrain() { return terrain; }
        void set_terrain_cost(int t_c) { terrain_cost = t_c; }
        int get_terrain_cost() const { return terrain_cost; }
        void set_cost(int c) { cost = c; }
        int get_cost() const { return cost; }
};

#endif
