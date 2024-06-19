#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include "heap.h"

#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401
#define START_X 200
#define START_Y 200

typedef enum {
    clearing,
    grass,
    boulder,
    edge,
    tree,
    water,
    road,
    pokemart,
    pokecenter,
    gate,
    bridge,
} terrain_e;

typedef enum trainer_type {
    pc,
    hiker,
    rival,
    swimmer,
    pacer,
    wanderer,
    sentry,
    explorer,
} trainer_type_e;

typedef struct coordinate {
    int x;
    int y;
} coordinate_t;

typedef struct trainer {
    trainer_type_e type;
    int next_turn;
    int seq_num;
    coordinate_t pos;
    coordinate_t dir; 
} trainer_t;


typedef struct map {
    terrain_e terrain[MAP_HEIGHT][MAP_WIDTH];
    int n, s, w, e;

    trainer_t *trainer_map[MAP_HEIGHT][MAP_WIDTH];
     coordinate_t pc_pos;

    int num_trainers;
} map_t;


typedef struct terrain {
    terrain_e type;
    int cost;
} terrain_t;

typedef struct path {
    heap_node_t *heap_node; 
    coordinate_t coordinate;
    terrain_t terrain;
    int cost;
} path_t;

typedef struct world {
    map_t *board[WORLD_HEIGHT][WORLD_WIDTH];
    map_t *current_map;
    coordinate_t location;

    path_t hiker_path[MAP_HEIGHT][MAP_WIDTH];
    path_t rival_path[MAP_HEIGHT][MAP_WIDTH];

    char view[MAP_HEIGHT][MAP_WIDTH + 1];
} world_t;

void build_bridges(map_t *map)
{
    int x, y;

    for (y = 1; y < MAP_HEIGHT - 1; y++) {
        for (x = 1; x < MAP_WIDTH - 1; x++) {
            if (map->terrain[y][x] == road) {
                if (map->terrain[y - 1][x    ] == water ||
                    map->terrain[y    ][x + 1] == water ||
                    map->terrain[y + 1][x    ] == water ||
                    map->terrain[y    ][x - 1] == water) {
                    map->terrain[y][x] = bridge;
                }
            }
        }
    }
}

void building_to_map(map_t *map, terrain_e building, int y, int x, int gate_row) {
    int building_connected = 0;

    map->terrain[y][x] = building;
    map->terrain[y + 1][x] = building;
    map->terrain[y][x + 1] = building;
    map->terrain[y + 1][x + 1] = building;

    map->terrain[y - 1][x] = road;
    map->terrain[y - 1][x + 1] = road;
    map->terrain[y - 1][x + 2] = road;
    map->terrain[y][x + 2] = road;
    map->terrain[y + 1][x + 2] = road;
    map->terrain[y + 2][x + 2] = road;
    map->terrain[y + 2][x + 1] = road;
    map->terrain[y + 2][x] = road;
    map->terrain[y + 2][x - 1] = road;
    map->terrain[y + 1][x - 1] = road;
    map->terrain[y][x - 1] = road;
    map->terrain[y - 1][x - 1] = road;

    if (y > gate_row + 1 && y > 3) { 
        y--;
        while (y > 1 && map->terrain[y - 1][x] != road) {
            y--;
            map->terrain[y][x] = road;
        }

        if (y != 1) {
            building_connected = 1;
        }
    } else if (y < gate_row - 2 && y < MAP_HEIGHT - 4) { 
        y += 2;
        while (y < MAP_HEIGHT - 2 && map->terrain[y + 1][x] != road) {
            y++;
            map->terrain[y][x] = road;
        }

        if (y != MAP_HEIGHT - 2) {
            building_connected = 1;
        }
    }

    if (building_connected == 0) { 
        if (map->w > 0) { 
            while (x > 0 && map->terrain[y][x - 1] != road) {
                x--;
                map->terrain[y][x] = road;
            }

            if (x > 0) {
                building_connected = 1;
            }
        } else { 
            while (x < MAP_WIDTH - 1 && map->terrain[y][x + 1] != road) {
                x++;
                map->terrain[y][x] = road;
            }

            if (x < MAP_WIDTH - 1) {
                building_connected = 1;
            }
        }

        if (map->n > 0 && building_connected == 0) { 
            while (y > 1 && map->terrain[y - 1][x] != road) {
                y--;
                map->terrain[y][x] = road;
            }
        } else if (building_connected == 0) { 
            while (y < MAP_HEIGHT - 1 && map->terrain[y + 1][x] != road) {
                y++;
                map->terrain[y][x] = road;
            }
        }
    }
}

void place_buildings(map_t *map, int manhattan_distance) {
    terrain_e first_building, second_building;
    int i, y, x;

    i = rand() % 2;

    first_building = (i < 1) ? pokemart : pokecenter;
    second_building = (first_building == pokemart) ? pokecenter : pokemart;

    int building_probability = ((-45 * manhattan_distance) / 200 + 50);
    if (manhattan_distance == 0 || rand() % 100 < building_probability) {
        y = rand() % 16 + 2; 
        x = rand() % 36 + 2; 

        building_to_map(map, first_building, y, x, map->w);
    }

    if (manhattan_distance == 0 || rand() % 100 < building_probability) {
        y = rand() % 16 + 2; 
        x = rand() % 36 + 41; 

        building_to_map(map, second_building, y, x, map->e);
    }
}

void pave_roads(map_t *map)
{
    int x_run = map->s - map->n;
    int y, x;

    y = 1;
    x = map->n;
    if (map->n > 0 && map->s > 0) {
        while (y < (MAP_HEIGHT - 2) / 2) { 
            map->terrain[y][x] = road;
            y++;
        }

        if (x_run < 0) { 
            while (x > map->s) {
                map->terrain[y][x] = road;
                x--;
            }
        } else if (x_run > 0) {
            while (x < map->s) {
                map->terrain[y][x] = road;
                x++;
            }
        }

        while (y < MAP_HEIGHT - 1) { 
            map->terrain[y][x] = road;
            y++;
        }
    }

    int y_rise = map->e - map->w;

    x = 1;
    y = map->w;
    if (map->w > 0 && map->e > 0) {
        while (x < (MAP_WIDTH - 2) / 2) { 
            map->terrain[y][x] = road;
            x++;
        }

        if (y_rise < 0) { 
            while (y > map->e) {
                map->terrain[y][x] = road;
                y--;
            }
        } else if (y_rise > 0) { 
            while (y < map->e) {
                map->terrain[y][x] = road;
                y++;
            }
        }

        while (x < MAP_WIDTH - 1) { 
            map->terrain[y][x] = road;
            x++;
        }
    }
}

void pave_roads_edge(map_t *map)
{
    int y, x;

    if (map->n < 0) {
        x = map->s;

        if (map->w < 0) { 
            for (y = MAP_HEIGHT - 2; y > map->e; y--) {
                map->terrain[y][x] = road;
            }

            for (x = map->s; x < MAP_WIDTH - 1; x++) {
                map->terrain[y][x] = road;
            }
        } else if (map->e < 0) { 
            for (y = MAP_HEIGHT - 2; y > map->w; y--) {
                map->terrain[y][x] = road;
            }

            for (x = map->s; x > 0; x--) {
                map->terrain[y][x] = road;
            }
        } else { 
            pave_roads(map);
            y = MAP_HEIGHT - 2;
            while (map->terrain[y][x] != road && y > 0) {
                map->terrain[y][x] = road;
                y--;
            }
        }
    } else if (map->s < 0) {
        x = map->n;

        if (map->e < 0) { 
            for (y = 0; y < map->w; y++) {
                map->terrain[y][x] = road;
            }

            for (x = map->n; x > 0; x--) {
                map->terrain[y][x] = road;
            }
        } else if (map->w < 0) { 
            for (y = 0; y < map->e; y++) {
                map->terrain[y][x] = road;
            }

            for (x = map->n; x < MAP_WIDTH - 1; x++) {
                map->terrain[y][x] = road;
            }
        } else { 
            pave_roads(map);
            y = 0;
            while (map->terrain[y][x] != road && y < MAP_HEIGHT - 1) {
                map->terrain[y][x] = road;
                y++;
            }
        }
    } else if (map->w < 0) { 
        pave_roads(map); 

        y = map->e;
        x = MAP_WIDTH - 2;
        while (map->terrain[y][x] != road && x > 0) {
            map->terrain[y][x] = road;
            x--;
        }
    } else if (map->e < 0) { 
        pave_roads(map); 

        y = map->w;
        x = 1;
        while (map->terrain[y][x] != road && x < MAP_WIDTH - 1) {
            map->terrain[y][x] = road;
            x++;
        }
    }
}

void generate_regional_terrain(map_t *map, int west_bound, int east_bound)
{
    int x, y, region_width, region_height;
    int i, row, col;
    terrain_e regions[5] = {boulder, tree, water, grass, grass};

    for (i = 0; i < 5; i++) {
        x = rand() % (east_bound - west_bound);
        x += west_bound;
        y = rand() % (MAP_HEIGHT - 3) + 2;
        region_width = rand() % 30 + 10;
        region_height = rand() % 6 + 5;

        for (row = 0; row < region_height; row++) {
            if (y + row < MAP_HEIGHT - 1) { 
                for (col = 0; col < region_width; col++) {
                    if (x + col < MAP_WIDTH - 1) {
                        map->terrain[y + row][x + col] = regions[i];
                    } else { 
                        map->terrain[y + row][x - col] = regions[i];
                    }
                }
            } else { 
                for (col = 0; col < region_width; col++) {
                    if (x + col < MAP_WIDTH - 1) { 
                        map->terrain[y - row][x + col] = regions[i];
                    } else { 
                        map->terrain[y - row][x - col] = regions[i];
                    }
                }
            }
        }
    }
}

void generate_terrain(map_t *map)
{
    generate_regional_terrain(map, 1, MAP_WIDTH / 2 - 1);
    generate_regional_terrain(map, MAP_WIDTH / 2 - 1, MAP_WIDTH - 1); 
    int i, y, x;

    for (i = 0; i < 20; i++) {
        y = rand() % (MAP_HEIGHT - 2) + 1;
        x = rand() % (MAP_WIDTH - 2) + 1;

        map->terrain[y][x] = edge;

        y = rand() % (MAP_HEIGHT - 2) + 1;
        x = rand() % (MAP_WIDTH - 2) + 1;

        map->terrain[y][x] = tree;
    }
}

void construct_border(map_t *map, int n, int s, int w, int e)
{
    int i, j;

    for (i = 0; i < MAP_HEIGHT; i++) {
        map->terrain[i][0] = edge;
        map->terrain[i][MAP_WIDTH - 1] = edge;
    }

    for (j = 0; j < MAP_WIDTH; j++) {
        map->terrain[0][j] = edge;
        map->terrain[MAP_HEIGHT - 1][j] = edge;
    }
    map->n = (n == 0) ? rand() % (MAP_WIDTH - 2) + 1 : n; 
    map->s = (s == 0) ? rand() % (MAP_WIDTH - 2) + 1 : s;
    map->w = (w == 0) ? rand() % (MAP_HEIGHT - 2) + 1 : w;
    map->e = (e == 0) ? rand() % (MAP_HEIGHT - 2) + 1 : e;
    map->terrain[0][map->n] = (n == -1) ? edge : gate;
    map->terrain[MAP_HEIGHT - 1][map->s] = (s == -1) ? edge : gate;
    map->terrain[map->w][0] = (w == -1) ? edge : gate;
    map->terrain[map->e][MAP_WIDTH - 1] = (e == -1) ? edge : gate;
}

void generate_map(map_t *map, int n, int s, int w, int e, int manhattan_distance)
{
    construct_border(map, n, s, w, e);

    generate_terrain(map);

    if (n < 0 || s < 0 || w < 0 || e < 0) { 
        pave_roads_edge(map);
    } else {
        pave_roads(map);
    }

    place_buildings(map, manhattan_distance);

    build_bridges(map);
}

int check_trainer_position(map_t *map, coordinate_t pos, trainer_type_e type)
{
    int check = 1;

    if (map->terrain[pos.y][pos.x] == edge ||
        map->terrain[pos.y][pos.x] == tree ||
        map->trainer_map[pos.y][pos.x] != NULL) {
        check = 0;
    } else if ((type == swimmer && map->terrain[pos.y][pos.x] != water) ||
               (type != swimmer && map->terrain[pos.y][pos.x] == water)) {
        check = 0;
    } else if (map->terrain[pos.y][pos.x] == boulder ||
               map->terrain[pos.y][pos.x] == tree) {
        if (type != hiker) {
            check = 0;
        }
    } else if ((map->terrain[pos.y][pos.x] == gate ||
                map->terrain[pos.y][pos.x] == road ||
                map->terrain[pos.y][pos.x] == bridge) &&
               (type != pc)) {
        check = 0;
    }

    return check;
}

void trainer_init(trainer_t *trainer, trainer_type_e type)
{
    coordinate_t start;
    start.x = 0;
    start.y = 0;

    trainer->type = type;
    trainer->next_turn = 0;
    trainer->pos = start;
    trainer->dir = start;

    switch (type)
    {
        case pc:
            trainer->seq_num = 0;
            break;
        case hiker:
            trainer->seq_num = 1;
            break;
        case rival:
            trainer->seq_num = 2;
            break;
        case pacer:
            trainer->seq_num = 3;
            break;
        case wanderer:
            trainer->seq_num = 4;
            break;
        case sentry:
            trainer->seq_num = 5;
            break;
        case explorer:
            trainer->seq_num = 6;
            break;
        case swimmer:
            trainer->seq_num = 7;
            break;
    }
}

void place_pc(heap_t *turn_heap, map_t *map)
{
    int y, x, count;
    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain[y][x] == road || map->terrain[y][x] == bridge) {
                count++;
            }
        }
    }
    coordinate_t start;
    coordinate_t start_options[count];
    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain[y][x] == road || map->terrain[y][x] == bridge) {
                start_options[count].x = x;
                start_options[count].y = y;
                count++;
            }
        }
    }

    start = start_options[rand() % count];

    map->trainer_map[start.y][start.x] = malloc(sizeof (trainer_t));
    trainer_init(map->trainer_map[start.y][start.x], pc);
    map->pc_pos.y = start.y;
    map->pc_pos.x = start.x;

    map->trainer_map[start.y][start.x]->pos = start;
    heap_insert(turn_heap, map->trainer_map[start.y][start.x]);
}

void place_npc(heap_t *turn_heap, map_t *map, trainer_type_e type)
{
    coordinate_t start;
    int valid_character_position;

    start.x = (rand() % MAP_WIDTH - 2) + 1;
    start.y = (rand() % MAP_HEIGHT - 2) + 1;

    valid_character_position = check_trainer_position(map, start, type);

    while(!valid_character_position) {
        start.x = (rand() % MAP_WIDTH - 2) + 1;
        start.y = (rand() % MAP_HEIGHT - 2) + 1;

        valid_character_position = check_trainer_position(map, start, type);
    }

    map->trainer_map[start.y][start.x] = malloc(sizeof (trainer_t));
    trainer_init(map->trainer_map[start.y][start.x], type);

    map->trainer_map[start.y][start.x]->pos = start;
    heap_insert(turn_heap, map->trainer_map[start.y][start.x]);
}

void map_init(heap_t *turn_heap, map_t *map, int num_trainers)
{
    int x, y;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            map->terrain[y][x] = clearing;
        }
    }

    map->n = 0;
    map->s = 0;
    map->w = 0;
    map->e = 0;

    generate_map(map, map->n, map->s, map->w, map->e, 0);

    map->num_trainers = num_trainers;

    trainer_map_init(turn_heap, map, num_trainers);
}


void trainer_map_init(heap_t *turn_heap, map_t *map, int num_trainers)
{
    int x, y, npc_count;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            map->trainer_map[y][x] = NULL;
        }
    }

    place_pc(turn_heap, map);

    if (num_trainers == 1) {
        place_npc(turn_heap, map, rand() % 7 + 1);
    } else if (num_trainers >= 2) {
        place_npc(turn_heap, map, hiker);
        place_npc(turn_heap, map, rival);

        npc_count = 2;
        if (num_trainers >= 7) {
            place_npc(turn_heap, map, pacer);
            place_npc(turn_heap, map, wanderer);
            place_npc(turn_heap, map, sentry);
            place_npc(turn_heap, map, explorer);
            place_npc(turn_heap, map, swimmer);
            npc_count = 7;
        }

        while(npc_count < num_trainers) {
            place_npc(turn_heap, map, rand() % 7 + 1);
            npc_count++;
        }
    }
}

void trainer_delete(map_t *map)
{
    int x, y;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            if (map->trainer_map[y][x] != NULL) {
                free(map->trainer_map[y][x]);
            }
        }
    }
}

void map_delete(map_t *map)
{
    trainer_delete(map);
    free(map);
}

int get_terrain_cost(terrain_e terrain_type, trainer_type_e trainer_type) {
    int terrain_cost = INT_MAX;

    if (terrain_type == road) {
        if (trainer_type != swimmer) {
            terrain_cost = 10;
        }
    } else if (terrain_type == bridge) {
        if (trainer_type != swimmer) {
            terrain_cost = 10;
        } else {
            terrain_cost = 7;
        }
    } else if (terrain_type == pokemart || terrain_type == pokecenter) {
        if (trainer_type == pc) {
            terrain_cost = 10;
        } else if (trainer_type != swimmer) {
            terrain_cost = 50;
        }
    } else if (terrain_type == grass) {
        if (trainer_type == hiker) {
            terrain_cost = 15;
        } else if (trainer_type != swimmer) {
            terrain_cost = 20;
        }
    } else if (terrain_type == clearing) {
        if (trainer_type != swimmer) {
            terrain_cost = 10;
        }
    } else if (terrain_type == boulder || terrain_type == tree) {
        if (trainer_type == hiker) {
            terrain_cost = 15;
        }
    } else if (terrain_type == water) {
        if (trainer_type == swimmer) {
            terrain_cost = 7;
        }
    } else if (terrain_type == gate) {
        if (trainer_type == pc) {
            terrain_cost = 10;
        }
    }

    return terrain_cost;
}

void dijkstra_path(heap_t *heap, map_t *map, path_t path[MAP_HEIGHT][MAP_WIDTH], trainer_type_e trainer_type)
{
    if (trainer_type != hiker && trainer_type != rival && trainer_type != swimmer) {
        return;
    }

    path_t *p;

    int x, y;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            path[y][x].coordinate.y = y;
            path[y][x].coordinate.x = x;
        }
    }

    for (y = 0; y < MAP_HEIGHT; y++) { 
        for (x = 0; x < MAP_WIDTH; x++) {
            path[y][x].cost = INT_MAX;
        }
    }

    int move_cost;
    for (y = 0; y < MAP_HEIGHT; y++) { 
        for (x = 0; x < MAP_WIDTH; x++) {
            path[y][x].terrain.type = map->terrain[y][x];

            move_cost = get_terrain_cost(path[y][x].terrain.type, trainer_type);

            if (map->trainer_map[y][x] != NULL) {
                if (map->trainer_map[y][x]->type != pc) {
                    move_cost = INT_MAX;
                } else {
                    move_cost = 0;
                }
            }

            if (move_cost != INT_MAX) {
                if (move_cost == 0) {
                    path[y][x].cost = move_cost; 
                }
                path[y][x].heap_node = heap_insert(heap, &path[y][x]);
            } else {
                path[y][x].heap_node = NULL;
            }

            path[y][x].terrain.cost = move_cost;
        }
    }

    while ((p = heap_remove_min(heap))) { 
        p->heap_node = NULL;

        if ((path[p->coordinate.y - 1][p->coordinate.x - 1].heap_node) &&
            (path[p->coordinate.y - 1][p->coordinate.x - 1].cost >
             (p->cost + path[p->coordinate.y - 1][p->coordinate.x - 1].terrain.cost) &&
              p->cost + path[p->coordinate.y - 1][p->coordinate.x - 1].terrain.cost > 0)) {
            path[p->coordinate.y - 1][p->coordinate.x - 1].cost =
                    (p->cost + path[p->coordinate.y - 1][p->coordinate.x - 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y - 1][p->coordinate.x - 1].heap_node);
        }
        if ((path[p->coordinate.y - 1][p->coordinate.x    ].heap_node) &&
            (path[p->coordinate.y - 1][p->coordinate.x    ].cost >
             (p->cost + path[p->coordinate.y - 1][p->coordinate.x].terrain.cost) &&
              p->cost + path[p->coordinate.y - 1][p->coordinate.x].terrain.cost > 0)) {
            path[p->coordinate.y - 1][p->coordinate.x    ].cost =
                    (p->cost + path[p->coordinate.y - 1][p->coordinate.x    ].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y - 1][p->coordinate.x    ].heap_node);
        }
        if ((path[p->coordinate.y - 1][p->coordinate.x + 1].heap_node) &&
            (path[p->coordinate.y - 1][p->coordinate.x + 1].cost >
             (p->cost + path[p->coordinate.y - 1][p->coordinate.x + 1].terrain.cost) &&
              p->cost + path[p->coordinate.y - 1][p->coordinate.x + 1].terrain.cost > 0)) {
            path[p->coordinate.y - 1][p->coordinate.x + 1].cost =
                    (p->cost + path[p->coordinate.y - 1][p->coordinate.x + 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y - 1][p->coordinate.x + 1].heap_node);
        }
        if ((path[p->coordinate.y    ][p->coordinate.x - 1].heap_node) &&
            (path[p->coordinate.y    ][p->coordinate.x - 1].cost >
             (p->cost + path[p->coordinate.y    ][p->coordinate.x - 1].terrain.cost) &&
              p->cost + path[p->coordinate.y    ][p->coordinate.x - 1].terrain.cost > 0)) {
            path[p->coordinate.y    ][p->coordinate.x - 1].cost =
                    (p->cost + path[p->coordinate.y    ][p->coordinate.x - 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y    ][p->coordinate.x - 1].heap_node);
        }
        if ((path[p->coordinate.y    ][p->coordinate.x + 1].heap_node) &&
            (path[p->coordinate.y    ][p->coordinate.x + 1].cost >
             (p->cost + path[p->coordinate.y    ][p->coordinate.x + 1].terrain.cost) &&
              p->cost + path[p->coordinate.y    ][p->coordinate.x + 1].terrain.cost > 0)) {
            path[p->coordinate.y    ][p->coordinate.x + 1].cost =
                    (p->cost + path[p->coordinate.y    ][p->coordinate.x + 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y    ][p->coordinate.x + 1].heap_node);
        }
        if ((path[p->coordinate.y + 1][p->coordinate.x - 1].heap_node) &&
            (path[p->coordinate.y + 1][p->coordinate.x - 1].cost >
             (p->cost + path[p->coordinate.y + 1][p->coordinate.x - 1].terrain.cost) &&
              p->cost + path[p->coordinate.y + 1][p->coordinate.x - 1].terrain.cost > 0)) {
            path[p->coordinate.y + 1][p->coordinate.x - 1].cost =
                    (p->cost + path[p->coordinate.y + 1][p->coordinate.x - 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y + 1][p->coordinate.x - 1].heap_node);
        }
        if ((path[p->coordinate.y + 1][p->coordinate.x    ].heap_node) &&
            (path[p->coordinate.y + 1][p->coordinate.x    ].cost >
             (p->cost + path[p->coordinate.y + 1][p->coordinate.x    ].terrain.cost) &&
              p->cost + path[p->coordinate.y + 1][p->coordinate.x    ].terrain.cost > 0)) {
            path[p->coordinate.y + 1][p->coordinate.x    ].cost =
                    (p->cost + path[p->coordinate.y + 1][p->coordinate.x    ].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y + 1][p->coordinate.x    ].heap_node);
        }
        if ((path[p->coordinate.y + 1][p->coordinate.x + 1].heap_node) &&
            (path[p->coordinate.y + 1][p->coordinate.x + 1].cost >
             (p->cost + path[p->coordinate.y + 1][p->coordinate.x + 1].terrain.cost) &&
              p->cost + path[p->coordinate.y + 1][p->coordinate.x + 1].terrain.cost > 0)) {
            path[p->coordinate.y + 1][p->coordinate.x + 1].cost =
                    (p->cost + path[p->coordinate.y + 1][p->coordinate.x + 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y + 1][p->coordinate.x + 1].heap_node);
        }
    }
}

int check_random_turn(int random_direction, map_t *map, coordinate_t pos, trainer_type_e type) {
    int check, terrain_cost;

    check = 0;

    if (random_direction == 0) { 
        if (type == wanderer || type == swimmer) {
            if (map->terrain[pos.y][pos.x] == map->terrain[pos.y - 1][pos.x] && map->trainer_map[pos.y - 1][pos.x] == NULL) {
                check = 1;
            }
        } else if (type == explorer) {
            terrain_cost = get_terrain_cost(map->terrain[pos.y - 1][pos.x], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y - 1][pos.x] == NULL) {
                check = 1;
            }
        }
    } else if (random_direction == 1) { 
        if (type == wanderer || type == swimmer) {
            if (map->terrain[pos.y][pos.x] == map->terrain[pos.y][pos.x + 1] && map->trainer_map[pos.y][pos.x + 1] == NULL) {
                check = 1;
            }
        } else if (type == explorer) {
            terrain_cost = get_terrain_cost(map->terrain[pos.y][pos.x + 1], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y][pos.x + 1] == NULL) {
                check = 1;
            }
        }
    } else if (random_direction == 2) { 
        if (type == wanderer || type == swimmer) {
            if (map->terrain[pos.y][pos.x] == map->terrain[pos.y + 1][pos.x] && map->trainer_map[pos.y][pos.x + 1] == NULL) {
                check = 1;
            }
        } else if (type == explorer) {
            terrain_cost = get_terrain_cost(map->terrain[pos.y + 1][pos.x], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y + 1][pos.x] == NULL) {
                check = 1;
            }
        }
    }  else if (random_direction == 3) { 
        if (type == wanderer || type == swimmer) {
            if (map->terrain[pos.y][pos.x] == map->terrain[pos.y][pos.x - 1] && map->trainer_map[pos.y][pos.x + 1] == NULL) {
                check = 1;
            }
        } else if (type == explorer) {
            terrain_cost = get_terrain_cost(map->terrain[pos.y][pos.x - 1], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y][pos.x - 1] == NULL) {
                check = 1;
            }
        }
    }

    return check;
}

void random_turn(map_t *map, trainer_t *t) {
    int random_direction, valid_direction;
    int num_fail;

    num_fail = 0;
    random_direction = rand() % 4;
    valid_direction = check_random_turn(random_direction, map, t->pos, t->type);
    while (!valid_direction) {
        num_fail++;
        if (num_fail > 20) {
            return;
        }
        random_direction = rand() % 4;
        valid_direction = check_random_turn(random_direction, map, t->pos, t->type);
    }

    if (random_direction == 0) {
        t->dir.y = -1;
        t->dir.x = 0;
        map->trainer_map[t->pos.y][t->pos.x] = NULL;
        t->pos.y--;
        map->trainer_map[t->pos.y][t->pos.x] = t;
    } else if (random_direction == 1) { 
        t->dir.y = 0;
        t->dir.x = 1;
        map->trainer_map[t->pos.y][t->pos.x] = NULL;
        t->pos.x++;
        map->trainer_map[t->pos.y][t->pos.x] = t;
    } else if (random_direction == 2) { 
        t->dir.y = 1;
        t->dir.x = 0;
        map->trainer_map[t->pos.y][t->pos.x] = NULL;
        t->pos.y++;
        map->trainer_map[t->pos.y][t->pos.x] = t;
    } else if (random_direction == 3) { 
        t->dir.y = 0;
        t->dir.x = -1;
        map->trainer_map[t->pos.y][t->pos.x] = NULL;
        t->pos.x--;
        map->trainer_map[t->pos.y][t->pos.x] = t;
    }
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
					
    wrefresh(local_win);		

    return local_win;
}

void destroy_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(local_win);
    delwin(local_win);
}

void enter_building(map_t *map, trainer_t *pc)
{
    int key;
    WINDOW *building_win;

    if (map->terrain[pc->pos.y][pc->pos.x] == pokecenter || map->terrain[pc->pos.y][pc->pos.x] == pokemart) {
        building_win = create_newwin(24, 80, 0, 0);
        mvwprintw(building_win, 1, 1, "You are in a building. Press '<' to exit.");
        wrefresh(building_win);

        while((key = getch()) != '<')
        {
            wprintw(building_win, "You are in a building. Press '<' to exit.");
            wrefresh(building_win);
        }

        destroy_win(building_win);

        pc->next_turn = pc->next_turn + 10;
    }
}

void trainer_info(map_t *map)
{
    int x, y, npc_rise, npc_run, key, i, shift;
    char trainer_char;
    char output[25];
    WINDOW *trainer_win;
    char *trainer_info[map->num_trainers];

    trainer_win = create_newwin(21, 24, 1, 0);
    i = 0;
    keypad(trainer_win, TRUE);

    for (y = 1; y < MAP_HEIGHT - 1; y++) {
        for (x = 1; x < MAP_WIDTH - 1; x++) {
            if (map->trainer_map[y][x] != NULL) {
                switch (map->trainer_map[y][x]->type)
                {
                    case pc:
                        trainer_char = '@';
                        break;
                    case hiker:
                        trainer_char = 'h';
                        break;
                    case rival:
                        trainer_char = 'r';
                        break;
                    case pacer:
                        trainer_char = 'p';
                        break;
                    case wanderer:
                        trainer_char = 'w';
                        break;
                    case sentry:
                        trainer_char = 's';
                        break;
                    case explorer:
                        trainer_char = 'e';
                        break;
                    case swimmer:
                        trainer_char = 'm';
                        break;
                }

                npc_rise = map->pc_pos.y - map->trainer_map[y][x]->pos.y;
                npc_run = map->pc_pos.x - map->trainer_map[y][x]->pos.x;

                if (npc_rise < 0) { 
                    if (npc_run < 0) {
                        sprintf(output, "%c, %d %s and %d %s\n", trainer_char, npc_rise * -1, "south", npc_run * -1, "east");
                    } else { 
                        sprintf(output, "%c, %d %s and %d %s\n", trainer_char, npc_rise * -1, "south", npc_run, "west");
                    }
                } else { 
                    if (npc_run < 0) {
                        sprintf(output, "%c, %d %s and %d %s\n", trainer_char, npc_rise, "north", npc_run * -1, "east");
                    } else { 
                        sprintf(output, "%c, %d %s and %d %s\n", trainer_char, npc_rise, "north", npc_run, "west");
                    }
                }

                trainer_info[i] = malloc(sizeof (output));
                strcpy(trainer_info[i], output);
                i++;
            }
        }
    }

    for (i = 0; i < map->num_trainers || i < 10; i++) {
        mvwprintw(trainer_win, i, 0, trainer_info[i]);
    }

    wrefresh(trainer_win);

    shift = 0;
    while((key = getch()) != 27)
    {
        switch (key)
        {
            case KEY_DOWN:
                if (i == 10 + shift && map->num_trainers > 10) {
                    for (i = shift; i < map->num_trainers || i < 10 + shift; i++) {
                        mvwprintw(trainer_win, i, 0, trainer_info[i]);
                    }
                    shift++;
                    wrefresh(trainer_win);
                }
                break;
            case KEY_UP:
                if (shift > 0 && map->num_trainers > 10) {
                    for (i = shift; i < map->num_trainers || i < 10 + shift; i++) {
                        mvwprintw(trainer_win, i, 0, trainer_info[i]);
                    }
                    shift--;
                    wrefresh(trainer_win);
                }
                break;
            default:
                continue;
        }

    }

    for (i = 0; i < map->num_trainers; i++) {
        free(trainer_info[i]);
    }

    destroy_win(trainer_win);
}

int battle()
{
    int key;
    WINDOW *battle_win;

    battle_win = create_newwin(24, 80, 0, 0);
    mvwprintw(battle_win, 1, 1, "You are in a battle. Press '<' to exit.");
    wrefresh(battle_win);

    while((key = getch()) != '<')
    {
        wprintw(battle_win, "You are in a battle. Press '<' to exit.");
        wrefresh(battle_win);
    }

    destroy_win(battle_win);

    return 0;
}

void move_pc(map_t *map, trainer_t *pc, int input)
{
    coordinate_t new_pos;
    int battle_outcome;

    if (input == '7' || input == 'y') {
        new_pos.y = pc->pos.y - 1;
        new_pos.x = pc->pos.x - 1;
    } else if (input == '8' || input == 'k') {
        new_pos.y = pc->pos.y - 1;
        new_pos.x = pc->pos.x    ;
    } else if (input == '9' || input == 'u') {
        new_pos.y = pc->pos.y - 1;
        new_pos.x = pc->pos.x + 1;
    } else if (input == '6' || input == 'l') {
        new_pos.y = pc->pos.y    ;
        new_pos.x = pc->pos.x + 1;
    } else if (input == '3' || input == 'n') {
        new_pos.y = pc->pos.y + 1;
        new_pos.x = pc->pos.x + 1;
    } else if (input == '2' || input == 'j') {
        new_pos.y = pc->pos.y + 1;
        new_pos.x = pc->pos.x    ;
    } else if (input == '1' || input == 'b') {
        new_pos.y = pc->pos.y + 1;
        new_pos.x = pc->pos.x - 1;
    } else if (input == '4' || input == 'h') {
        new_pos.y = pc->pos.y    ;
        new_pos.x = pc->pos.x - 1;
    }

    if (get_terrain_cost(map->terrain[new_pos.y][new_pos.x], pc->type) != INT_MAX) {
        if (map->trainer_map[new_pos.y][new_pos.x] != NULL) {
            if (map->trainer_map[new_pos.y][new_pos.x]->next_turn > -1) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    pc->next_turn = -1;
                } else if (battle_outcome == -1) {
                    map->trainer_map[new_pos.y][new_pos.x]->next_turn = -1;
                }
            }
        }

        map->trainer_map[pc->pos.y][pc->pos.x] = NULL;
        pc->pos.y = new_pos.y;
        pc->pos.x = new_pos.x;
        map->trainer_map[pc->pos.y][pc->pos.x] = pc;
        map->pc_pos = pc->pos;

        if (pc->next_turn > -1) {
            pc->next_turn = pc->next_turn + get_terrain_cost(map->terrain[pc->pos.y][pc->pos.x], pc->type);
        }
    }
}

void move_dijkstra_trainer(heap_t *path_heap, path_t path[MAP_HEIGHT][MAP_WIDTH], map_t *map, trainer_t *t)
{
    path_t *cheapest_path;
    int battle_outcome;

    cheapest_path = &path[t->pos.y - 1][t->pos.x - 1];
    battle_outcome = 0;

    dijkstra_path(path_heap, map, path, t->type);

    if (path[t->pos.y - 1][t->pos.x].cost < cheapest_path->cost) {
        cheapest_path = &path[t->pos.y - 1][t->pos.x];
    }
    if (path[t->pos.y - 1][t->pos.x + 1].cost < cheapest_path->cost) {
        cheapest_path = &path[t->pos.y - 1][t->pos.x + 1];
    }
    if (path[t->pos.y][t->pos.x - 1].cost < cheapest_path->cost) {
        cheapest_path = &path[t->pos.y][t->pos.x - 1];
    }
    if (path[t->pos.y][t->pos.x + 1].cost < cheapest_path->cost) {
        cheapest_path = &path[t->pos.y][t->pos.x + 1];
    }
    if (path[t->pos.y + 1][t->pos.x - 1].cost < cheapest_path->cost) {
        cheapest_path = &path[t->pos.y + 1][t->pos.x - 1];
    }
    if (path[t->pos.y + 1][t->pos.x].cost < cheapest_path->cost) {
        cheapest_path = &path[t->pos.y + 1][t->pos.x];
    }
    if (path[t->pos.y + 1][t->pos.x + 1].cost < cheapest_path->cost) {
        cheapest_path = &path[t->pos.y + 1][t->pos.x + 1];
    }
    if (cheapest_path->cost == 0) {
        cheapest_path = &path[t->pos.y    ][t->pos.x    ];

        battle_outcome = battle();

        if (battle_outcome == 1) { 
            map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
        } else if (battle_outcome == -1) { 
            t->next_turn = -1;
        }
    }

    map->trainer_map[t->pos.y][t->pos.x] = NULL;
    t->pos.y = cheapest_path->coordinate.y;
    t->pos.x = cheapest_path->coordinate.x;
    map->trainer_map[t->pos.y][t->pos.x] = t;
    if (t->next_turn > -1) {
        t->next_turn = t->next_turn + cheapest_path->terrain.cost;
    }
}

void move_wanderer_explorer(map_t *map, trainer_t *t)
{
    int move_cost, battle_outcome;
    terrain_e current_terrain;

    current_terrain = map->terrain[t->pos.y][t->pos.x];
    if (t->dir.x == 0 && t->dir.y <= 0) {
        move_cost = get_terrain_cost(map->terrain[t->pos.y - 1][t->pos.x], t->type);

        if ((t->type == wanderer && (current_terrain == map->terrain[t->pos.y - 1][t->pos.x])) ||
            (t->type == explorer && (move_cost != INT_MAX))) {
            t->dir.y = -1;
            if (map->trainer_map[t->pos.y - 1][t->pos.x    ] == NULL) {
                map->trainer_map[t->pos.y][t->pos.x] = NULL;
                t->pos.y--;
                map->trainer_map[t->pos.y][t->pos.x] = t;
            } else if (map->trainer_map[t->pos.y - 1][t->pos.x    ]->type == pc) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                    random_turn(map, t);
                } else if (battle_outcome == -1) { 
                    t->next_turn = -1;
                }
            } else {
                random_turn(map, t);
            }
        } else {
            random_turn(map, t);
        }
    } else if (t->dir.x >= 0 && t->dir.y == 0) {
        move_cost = get_terrain_cost(map->terrain[t->pos.y][t->pos.x + 1], t->type);

        if ((t->type == wanderer && (current_terrain == map->terrain[t->pos.y][t->pos.x + 1])) ||
            (t->type == explorer && (move_cost != INT_MAX))) {
            t->dir.x = 1;
            if (map->trainer_map[t->pos.y    ][t->pos.x + 1] == NULL) {
                map->trainer_map[t->pos.y][t->pos.x] = NULL;
                t->pos.x--;
                map->trainer_map[t->pos.y][t->pos.x] = t;
            }  else if (map->trainer_map[t->pos.y    ][t->pos.x + 1]->type == pc) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                    random_turn(map, t);
                } else if (battle_outcome == -1) {
                    t->next_turn = -1;
                }
            } else {
                random_turn(map, t);
            }
        } else {
            random_turn(map, t);
        }
    } else if (t->dir.x == 0 && t->dir.y >= 0) {
        move_cost = get_terrain_cost(map->terrain[t->pos.y + 1][t->pos.x], t->type);

        if ((t->type == wanderer && (current_terrain == map->terrain[t->pos.y + 1][t->pos.x])) ||
            (t->type == explorer && (move_cost != INT_MAX))) {
            t->dir.y = 1;
            if (map->trainer_map[t->pos.y + 1][t->pos.x    ] == NULL) {
                map->trainer_map[t->pos.y][t->pos.x] = NULL;
                t->pos.y++;
                map->trainer_map[t->pos.y][t->pos.x] = t;
            } else if (map->trainer_map[t->pos.y + 1][t->pos.x    ]->type == pc) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                    random_turn(map, t);
                } else if (battle_outcome == -1) { 
                    t->next_turn = -1;
                }
            } else {
                random_turn(map, t);
            }
        } else {
            random_turn(map, t);
        }
    } else if (t->dir.x <= 0 && t->dir.y == 0) {
        move_cost = get_terrain_cost(map->terrain[t->pos.y    ][t->pos.x - 1], t->type);

        if ((t->type == wanderer && (current_terrain == map->terrain[t->pos.y    ][t->pos.x - 1])) ||
            (t->type == explorer && (move_cost != INT_MAX))) {
            t->dir.x = -1;
            if (map->trainer_map[t->pos.y    ][t->pos.x - 1] == NULL) {
                map->trainer_map[t->pos.y][t->pos.x] = NULL;
                t->pos.x--;
                map->trainer_map[t->pos.y][t->pos.x] = t;
            } else if (map->trainer_map[t->pos.y    ][t->pos.x - 1]->type == pc) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                    random_turn(map, t);
                } else if (battle_outcome == -1) { 
                    t->next_turn = -1;
                }
            } else {
                random_turn(map, t);
            }
        } else {
            random_turn(map, t);
        }
    }

    if (t->next_turn > -1) {
        t->next_turn = t->next_turn + get_terrain_cost(map->terrain[t->pos.y][t->pos.x], t->type);
    }
}

void move_swimmer(heap_t *path_heap, path_t path[MAP_HEIGHT][MAP_WIDTH], map_t *map, trainer_t *t, coordinate_t pc_pos)
{
    int battle_outcome;
    if ((map->terrain[pc_pos.y - 1][pc_pos.x    ] == water ||
         map->terrain[pc_pos.y - 1][pc_pos.x    ] == bridge) ||
        (map->terrain[pc_pos.y    ][pc_pos.x + 1] == water ||
         map->terrain[pc_pos.y    ][pc_pos.x + 1] == bridge) ||
        (map->terrain[pc_pos.y + 1][pc_pos.x    ] == water ||
         map->terrain[pc_pos.y + 1][pc_pos.x    ] == bridge) ||
        (map->terrain[pc_pos.y    ][pc_pos.x - 1] == water ||
         map->terrain[pc_pos.y    ][pc_pos.x - 1] == bridge)) {
        move_dijkstra_trainer(path_heap, path, map, t);
       
    } else {
        if (t->dir.x == 0 && t->dir.y <= 0) {
            if (map->terrain[t->pos.y - 1][t->pos.x] == water ||
                map->terrain[t->pos.y - 1][t->pos.x] == bridge) {
                t->dir.y = -1;
                if (map->trainer_map[t->pos.y - 1][t->pos.x] == NULL) {
                    map->trainer_map[t->pos.y][t->pos.x] = NULL;
                    t->pos.y--;
                    map->trainer_map[t->pos.y][t->pos.x] = t;
                } else if (map->trainer_map[t->pos.y - 1][t->pos.x]->type == pc) {
                    battle_outcome = battle();

                    if (battle_outcome == 1) { 
                        map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                        random_turn(map, t);
                    } else if (battle_outcome == -1) { 
                        t->next_turn = -1;
                    }
                } else {
                    random_turn(map, t);
                }
            } else if (t->dir.x >= 0 && t->dir.y == 0) {
                if (map->terrain[t->pos.y][t->pos.x + 1] == water ||
                    map->terrain[t->pos.y][t->pos.x + 1] == bridge) {
                    t->dir.x = 1;
                    if (map->trainer_map[t->pos.y][t->pos.x + 1] == NULL) {
                        map->trainer_map[t->pos.y][t->pos.x] = NULL;
                        t->pos.x--;
                        map->trainer_map[t->pos.y][t->pos.x] = t;
                    } else if (map->trainer_map[t->pos.y][t->pos.x + 1]->type == pc) {
                        battle_outcome = battle();

                        if (battle_outcome == 1) { 
                            map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                            random_turn(map, t);
                        } else if (battle_outcome == -1) {
                            t->next_turn = -1;
                        }
                    } else {
                        random_turn(map, t);
                    }
                } else {
                    random_turn(map, t);
                }
            } else if (t->dir.x == 0 && t->dir.y >= 0) {
                if (map->terrain[t->pos.y + 1][t->pos.x] == water ||
                    map->terrain[t->pos.y + 1][t->pos.x] == bridge) {
                    t->dir.y = 1;
                    if (map->trainer_map[t->pos.y + 1][t->pos.x] == NULL) {
                        map->trainer_map[t->pos.y][t->pos.x] = NULL;
                        t->pos.y++;
                        map->trainer_map[t->pos.y][t->pos.x] = t;
                    } else if (map->trainer_map[t->pos.y + 1][t->pos.x]->type == pc) {
                        battle_outcome = battle();

                        if (battle_outcome == 1) {
                            map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                            random_turn(map, t);
                        } else if (battle_outcome == -1) { 
                            t->next_turn = -1;
                        }
                    } else {
                        random_turn(map, t);
                    }
                } else {
                    random_turn(map, t);
                }
            } else if (t->dir.x <= 0 && t->dir.y == 0) {
                if (map->terrain[t->pos.y][t->pos.x - 1] == water ||
                    map->terrain[t->pos.y][t->pos.x - 1] == bridge) {
                    t->dir.x = -1;
                    if (map->trainer_map[t->pos.y][t->pos.x - 1] == NULL) {
                        map->trainer_map[t->pos.y][t->pos.x] = NULL;
                        t->pos.x--;
                        map->trainer_map[t->pos.y][t->pos.x] = t;
                    } else if (map->trainer_map[t->pos.y][t->pos.x - 1]->type == pc) {
                        battle_outcome = battle();

                        if (battle_outcome == 1) {
                            map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                            random_turn(map, t);
                        } else if (battle_outcome == -1) {
                            t->next_turn = -1;
                        }
                    } else {
                        random_turn(map, t);
                    }
                } else {
                    random_turn(map, t);
                }
            }

            if (t->next_turn > -1) {
                t->next_turn = t->next_turn + get_terrain_cost(map->terrain[t->pos.y][t->pos.x], t->type);
            }
        }
    }
}

void move_pacer(map_t *map, trainer_t *t)
{
    terrain_e current_terrain;
    int battle_outcome;
    int moved;

    current_terrain = map->terrain[t->pos.y][t->pos.x];
    moved = 0;
    if (t->dir.x >= 0 && map->terrain[t->pos.y][t->pos.x + 1] == current_terrain) {
        if (map->trainer_map[t->pos.y    ][t->pos.x + 1] == NULL) {
            moved = 1;
            t->dir.x = 1;
            map->trainer_map[t->pos.y][t->pos.x] = NULL;
            t->pos.x++;
            map->trainer_map[t->pos.y][t->pos.x] = t;
        } else if (map->trainer_map[t->pos.y    ][t->pos.x + 1]->type == pc) {
            battle_outcome = battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
            } else if (battle_outcome == -1) { 
                t->next_turn = -1;
            }
        }
    } else if (t->dir.x <= 0 && map->terrain[t->pos.y][t->pos.x - 1] == current_terrain) {
        if (map->trainer_map[t->pos.y    ][t->pos.x - 1] == NULL) {
            moved = 1;
            t->dir.x = -1;
            map->trainer_map[t->pos.y][t->pos.x] = NULL;
            t->pos.x--;
            map->trainer_map[t->pos.y][t->pos.x] = t;
        } else if (map->trainer_map[t->pos.y    ][t->pos.x - 1]->type == pc) {
            battle_outcome = battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
            } else if (battle_outcome == -1) { 
                t->next_turn = -1;
            }
        }
    }

    if (!moved && t->dir.x == -1 && map->terrain[t->pos.y][t->pos.x + 1] == current_terrain) {
        t->dir.x = 1;
        if (map->trainer_map[t->pos.y    ][t->pos.x + 1] == NULL) {
            moved = 1;
            map->trainer_map[t->pos.y][t->pos.x] = NULL;
            t->pos.x++;
            map->trainer_map[t->pos.y][t->pos.x] = t;
        } else if (map->trainer_map[t->pos.y    ][t->pos.x + 1]->type == pc) {
            battle_outcome = battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
            } else if (battle_outcome == -1) { 
                t->next_turn = -1;
            }
        }
    }
 
    if (!moved && t->dir.x == 1 && map->terrain[t->pos.y][t->pos.x - 1] == current_terrain) {
        t->dir.x = -1;
        if (map->trainer_map[t->pos.y    ][t->pos.x - 1] == NULL) {
            moved = 1;
            map->trainer_map[t->pos.y][t->pos.x] = NULL;
            t->pos.x--;
            map->trainer_map[t->pos.y][t->pos.x] = t;
        } else if (map->trainer_map[t->pos.y    ][t->pos.x - 1]->type == pc) {
            battle_outcome = battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
            } else if (battle_outcome == -1) { 
                t->next_turn = -1;
            }
        } else {
            if (map->trainer_map[t->pos.y    ][t->pos.x + 1] == NULL) {
                t->dir.x = 1;
                moved = 1;
                map->trainer_map[t->pos.y][t->pos.x] = NULL;
                t->pos.x++;
                map->trainer_map[t->pos.y][t->pos.x] = t;
            } else if (map->trainer_map[t->pos.y    ][t->pos.x + 1]->type == pc) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->pc_pos.y][map->pc_pos.x]->next_turn = -1;
                } else if (battle_outcome == -1) {
                    t->next_turn = -1;
                }
            }
        }
    }

    if (t->next_turn > -1) {
        t->next_turn = t->next_turn + get_terrain_cost(map->terrain[t->pos.y][t->pos.x], t->type);
    }
}

static int32_t path_cmp(const void *key, const void *with) { 
    return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t turn_cmp(const void *key, const void *with) { 
    int32_t next_turn = ((trainer_t *) key)->next_turn - ((trainer_t *) with)->next_turn;
    return (next_turn == 0) ? ((trainer_t *) key)->seq_num - ((trainer_t *) with)->seq_num : next_turn;
}

chtype view[MAP_HEIGHT][MAP_WIDTH];

void init_terminal(void)
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLUE);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void init_view()
{
    int x, y;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            view[y][x] = ' ';
        }
    }
}

void render_view(map_t *map)
{
    int y, x;
    int color;

    clear();

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            switch (map->terrain[y][x])
            {
                case clearing:
                    view[y][x] = '.';
                    color = COLOR_GREEN;
                    break;
                case grass:
                    view[y][x] = ':';
                    color = COLOR_GREEN;
                    break;
                case boulder:
                case edge:
                    view[y][x] = '%';
                    color = COLOR_WHITE;
                    break;
                case tree:
                    view[y][x] = '^';
                    color = COLOR_MAGENTA;
                    break;
                case water:
                    view[y][x] = '~';
                    color = COLOR_BLUE;
                    break;
                case road:
                case gate:
                case bridge:
                    view[y][x] = '#';
                    color = COLOR_YELLOW;
                    break;
                case pokemart:
                    view[y][x] = 'M';
                    color = COLOR_CYAN;
                    break;
                case pokecenter:
                    view[y][x] = 'C';
                    color = COLOR_CYAN;
                    break;
            }

            if  (map->trainer_map[y][x] != NULL) {
                color = COLOR_RED;
                switch (map->trainer_map[y][x]->type) {
                    case pc:
                        view[y][x] = '@';
                        break;
                    case hiker:
                        view[y][x] = 'h';
                        break;
                    case rival:
                        view[y][x] = 'r';
                        break;
                    case pacer:
                        view[y][x] = 'p';
                        break;
                    case wanderer:
                        view[y][x] = 'w';
                        break;
                    case sentry:
                        view[y][x] = 's';
                        break;
                    case explorer:
                        view[y][x] = 'e';
                        break;
                    case swimmer:
                        view[y][x] = 'm';
                        break;
                }
            }

            attron(COLOR_PAIR(color));
            mvaddch(1 + y, x, view[y][x]);
            attroff(COLOR_PAIR(color));
        }
    }

    refresh();
}

int pc_turn(map_t *map, trainer_t *pc)
{
    int quit_game, valid;
    int key;

    quit_game = valid = 0;

    while (!valid) {
        key = getch();

        switch (key) {
            case '7':
            case 'y':
            case '8':
            case 'k':
            case '9':
            case 'u':
            case '6':
            case 'l':
            case '3':
            case 'n':
            case '2':
            case 'j':
            case '1':
            case 'b':
            case '4':
            case 'h':
                move_pc(map, pc, key);
                valid = 1;
                break;
            case '5':
            case ' ':
            case '.':
                pc->next_turn = pc->next_turn + 15;
                valid = 1;
                break;
            case '>':
                enter_building(map, pc);
                valid = 1;
                break;
            case 't':
                trainer_info(map);
                render_view(map);
                valid = 0;
                continue;
            case 'Q':
                quit_game = 1;
                valid = 1;
                break;
            default:
                mvprintw(22, 1,
                         "Use Keys to move PC, display info and Quit using 'Q'.");
                valid = 0;
        }
    }

    if (pc->next_turn < 0) {
        quit_game = 1;
    }

    return quit_game;
}

void game_loop(heap_t *turn_heap, heap_t *path_heap, world_t *world)
{
    trainer_t *t;
    int x, y, quit_game;
    path_t swimmer_path[MAP_HEIGHT][MAP_WIDTH];

    quit_game = 0;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            swimmer_path[y][x].heap_node = NULL;
            swimmer_path[y][x].coordinate.x = x;
            swimmer_path[y][x].coordinate.y = y;
            swimmer_path[y][x].terrain.type = edge;
            swimmer_path[y][x].terrain.cost = INT_MAX;
            swimmer_path[y][x].cost = INT_MAX;
        }
    }

    render_view(world->current_map);

    while (!quit_game) {
        t = heap_remove_min(turn_heap);

        switch (t->type) {
            case pc:
                quit_game = pc_turn(world->current_map, t);
                break;
            case hiker:
                move_dijkstra_trainer(path_heap, world->hiker_path, world->current_map, t);
                break;
            case rival:
                move_dijkstra_trainer(path_heap, world->rival_path, world->current_map, t);
                break;
            case pacer:
                move_pacer(world->current_map, t);
                break;
            case wanderer:
            case explorer:
                move_wanderer_explorer(world->current_map, t);
                break;
            case sentry:
                t->next_turn = t->next_turn + 15;
                break;
            case swimmer:
                move_swimmer(path_heap, swimmer_path, world->current_map, t, world->current_map->pc_pos);
                break;
        }

        render_view(world->current_map);

        if (t->next_turn > -1) {
            heap_insert(turn_heap, t);
        }

        if (world->current_map->trainer_map[world->current_map->pc_pos.y][world->current_map->pc_pos.x] != NULL &&
            world->current_map->trainer_map[world->current_map->pc_pos.y][world->current_map->pc_pos.x]->next_turn == -1) {
            break;
        }
    }
}

void world_init(heap_t *turn_heap, world_t *world, int num_trainers)
{
    int x, y;
    for (y = 0; y < WORLD_HEIGHT; y++) {
        for (x = 0; x < WORLD_WIDTH; x++) {
            world->board[y][x] = NULL;
        }
    }

    world->current_map = malloc(sizeof (*world->current_map));

    world->location.x = START_X;
    world->location.y = START_Y;

    world->board[START_Y][START_X] = malloc(sizeof (*world->current_map));
    world->current_map = world->board[START_Y][START_X];
    map_init(turn_heap, world->current_map, num_trainers);

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            world->hiker_path[y][x].heap_node = NULL;
            world->hiker_path[y][x].coordinate.x = x;
            world->hiker_path[y][x].coordinate.y = y;
            world->hiker_path[y][x].terrain.type = edge;
            world->hiker_path[y][x].terrain.cost = INT_MAX;
            world->hiker_path[y][x].cost = INT_MAX;

            world->rival_path[y][x].heap_node = NULL;
            world->rival_path[y][x].coordinate.x = x;
            world->rival_path[y][x].coordinate.y = y;
            world->rival_path[y][x].terrain.type = edge;
            world->rival_path[y][x].terrain.cost = INT_MAX;
            world->rival_path[y][x].cost = INT_MAX;
        }
    }

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            world->view[y][x] = '!';
        }

        world->view[y][MAP_WIDTH] = '\n';
    }
}

void world_delete(world_t *world)
{
    int x, y;

    for (y = 0; y < WORLD_HEIGHT; y++) {
        for (x = 0; x < WORLD_WIDTH; x++) {
            map_delete(world->board[y][x]);
        }
    }

    free(world);
}

int main(int argc, char *argv[])
{
    heap_t path_heap, turn_heap;
    world_t *world;

    srand(time(NULL));

    init_terminal();

    init_view();

    heap_init(&path_heap, path_cmp, NULL);
    heap_init(&turn_heap, turn_cmp, NULL);

    world = malloc(sizeof (*world));
    if (argc == 3) {
        if (!strcmp(argv[1], "--numtrainers")) {
            world_init(&turn_heap, world, atoi(argv[2]));
        } else {
            fprintf(stderr, "Usage: \"./poke327 --numtrainers <integer>\" or \"./poke327\" \n");

            return -1;
        }
    } else if (argc == 1) {
        world_init(&turn_heap, world, 10);
    } else {
        fprintf(stderr, "Usage: \"./poke327 --numtrainers <integer>\" or \"./poke327\" \n");

        return -1;
    }

    game_loop(&turn_heap, &path_heap, world);

    endwin();

    heap_delete(&path_heap);
    heap_delete(&turn_heap);
    world_delete(world);
    return 0;
}