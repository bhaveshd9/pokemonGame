#include <iostream>

#include "path.h"

int path::calculate_terrain_cost(terrain_e terrain_type, trainer_type_e trainer_type) {
    int terrain_cost = INT_MAX;

    if (terrain_type == road) {
        if (trainer_type != swimmer_e) {
            terrain_cost = 10;
        }
    } else if (terrain_type == bridge) {
        if (trainer_type != swimmer_e) {
            terrain_cost = 10;
        } else {
            terrain_cost = 7;
        }
    } else if (terrain_type == pokemart || terrain_type == pokecenter) {
        if (trainer_type == pc_e) {
            terrain_cost = 10;
        } else if (trainer_type != swimmer_e) {
            terrain_cost = 50;
        }
    } else if (terrain_type == tall_grass) {
        if (trainer_type == hiker_e) {
            terrain_cost = 15;
        } else if (trainer_type != swimmer_e) {
            terrain_cost = 20;
        }
    } else if (terrain_type == grass) {
        if (trainer_type != swimmer_e) {
            terrain_cost = 10;
        }
    } else if (terrain_type == boulder || terrain_type == tree) {
        if (trainer_type == hiker_e) {
            terrain_cost = 15;
        }
    } else if (terrain_type == water) {
        if (trainer_type == swimmer_e) {
            terrain_cost = 7;
        }
    } else if (terrain_type == gate) {
        if (trainer_type == pc_e) {
            terrain_cost = 10;
        }
    }

    return terrain_cost;
}

/*
 * Fills in the Dijkstra's cost map for the trainer.
 * In the future, can return just the path for the trainer to the PC.
 */
void path::dijkstra_path(heap_t *heap, map *map, path path_map[MAP_HEIGHT][MAP_WIDTH], trainer_type_e trainer_type)
{
    if (trainer_type != hiker_e && trainer_type != rival_e && trainer_type != swimmer_e) {
        return;
    }

    coordinate_t coord;
    int x, y;
    path *p;

    // get all coordinates
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            coord.y = y;
            coord.x = x;
            path_map[y][x].set_coordinate(coord);
        }
    }

    for (y = 0; y < MAP_HEIGHT; y++) { // assign every path_map to infinity
        for (x = 0; x < MAP_WIDTH; x++) {
            path_map[y][x].set_cost(INT_MAX);
        }
    }

    int move_cost;
    for (y = 0; y < MAP_HEIGHT; y++) { // add every traversable path_map to the heap as a node
        for (x = 0; x < MAP_WIDTH; x++) {
            path_map[y][x].set_terrain(map->terrain_map[y][x]);

            move_cost = path::calculate_terrain_cost(path_map[y][x].get_terrain(), trainer_type);

            if (map->trainer_map[y][x] != nullptr) {
                if (map->trainer_map[y][x]->get_type() != pc_e) {
                    move_cost = INT_MAX;
                } else {
                    move_cost = 0;
                }
            }

            if (move_cost != INT_MAX) {
                if (move_cost == 0) {
                    path_map[y][x].set_cost(move_cost); // So the pc node is pulled out first to get cost map.
                }
                path_map[y][x].set_heap_node(heap_insert(heap, &path_map[y][x]));
            } else {
                path_map[y][x].set_heap_node(nullptr);
            }

            path_map[y][x].set_terrain_cost(move_cost);
        }
    }

    while ((p = (path *) heap_remove_min(heap))) { // visit each node and recalculate the distances of nodes around it
        p->set_heap_node(nullptr);

        if ((path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].set_cost(p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].get_heap_node()) &&
            (path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].set_cost(p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].set_cost(p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].set_cost(p->get_cost()+ path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_cost() >
             (p->get_cost()+ path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_terrain_cost()) &&
              p->get_cost()+ path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].set_cost(p->get_cost()+ path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].set_cost(p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_heap_node()) &&
            (path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_cost() >
             (p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_terrain_cost()) &&
              p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].set_cost(p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_cost() >
             (p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_terrain_cost()) &&
              p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].set_cost(p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_heap_node());
        }
    }
}
