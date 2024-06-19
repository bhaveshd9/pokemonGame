#ifndef WORLD_H
#define WORLD_H

#include "character.h"

class world {
    private:
        map *current_map;
        coordinate_t location;

        int num_trainers;
    public:
        map *board[WORLD_HEIGHT][WORLD_WIDTH];
        path hiker_path[MAP_HEIGHT][MAP_WIDTH];
        path rival_path[MAP_HEIGHT][MAP_WIDTH];
        explicit world(int num_t) : location(), board()
        {
            int x, y;
            coordinate_t tmp;
            for (y = 0; y < WORLD_HEIGHT; y++) {
                for (x = 0; x < WORLD_WIDTH; x++) {
                    board[y][x] = nullptr;
                }
            }

            for (y = 0; y < MAP_HEIGHT; y++) {
                for (x = 0; x < MAP_WIDTH; x++) {
                    tmp.y = y;
                    tmp.x = x;
                    hiker_path[y][x].set_heap_node(nullptr);
                    hiker_path[y][x].set_coordinate(tmp);
                    hiker_path[y][x].set_terrain(edge);
                    hiker_path[y][x].set_terrain_cost(INT_MAX);
                    hiker_path[y][x].set_cost(INT_MAX);

                    rival_path[y][x].set_heap_node(nullptr);
                    rival_path[y][x].set_coordinate(tmp);
                    rival_path[y][x].set_terrain(edge);
                    rival_path[y][x].set_terrain_cost(INT_MAX);
                    rival_path[y][x].set_cost(INT_MAX);
                }
            }
            location.x = START_X;
            location.y = START_Y;
            num_trainers = num_t;

            board[START_Y][START_X] = new map();
            current_map = board[START_Y][START_X];
        }
        ~world()
        {
            int x, y;

            for (y = 0; y < WORLD_HEIGHT; y++) {
                for (x = 0; x < WORLD_WIDTH; x++) {
                    if (board[y][x] != nullptr) {
                        board[y][x]->~map();
                    }
                }
            }
        }

        map *get_current_map() { return current_map; }
        void set_current_map(map *m) { current_map = m; }
        coordinate_t get_location() { return location; }
        void set_location_y(int y) { location.y = y; }
        void set_location_x(int x) { location.x = x; }
        int get_num_trainers() const { return num_trainers; }
};

#endif
