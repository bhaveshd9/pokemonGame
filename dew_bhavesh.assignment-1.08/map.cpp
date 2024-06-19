#include <iostream>

#include "map.h"

static int32_t turn_cmp(const void *key, const void *with) { // make heap compare turns
    int32_t next_turn = ((trainer *) key)->get_next_turn() - ((trainer *) with)->get_next_turn();
    return (next_turn == 0) ? ((trainer *) key)->get_seq_num() - ((trainer *) with)->get_seq_num() : next_turn;
}

void build_bridges(map *map)
{
    int x, y;

    for (y = 1; y < MAP_HEIGHT - 1; y++) {
        for (x = 1; x < MAP_WIDTH - 1; x++) {
            if (map->terrain_map[y][x] == road) {
                if (map->terrain_map[y - 1][x    ] == water ||
                    map->terrain_map[y    ][x + 1] == water ||
                    map->terrain_map[y + 1][x    ] == water ||
                    map->terrain_map[y    ][x - 1] == water) {
                    map->terrain_map[y][x] = bridge;
                }
            }
        }
    }
}

/*
 * Place a 2x2 grid for the building, roads around the building,
 * and connect a road to the west to east road.
 */
void building_to_map(map *map, terrain_e building, int y, int x, int gate_row) {
    int building_connected = 0;

    map->terrain_map[y][x] = building;
    map->terrain_map[y + 1][x] = building;
    map->terrain_map[y][x + 1] = building;
    map->terrain_map[y + 1][x + 1] = building;

    map->terrain_map[y - 1][x] = road;
    map->terrain_map[y - 1][x + 1] = road;
    map->terrain_map[y - 1][x + 2] = road;
    map->terrain_map[y][x + 2] = road;
    map->terrain_map[y + 1][x + 2] = road;
    map->terrain_map[y + 2][x + 2] = road;
    map->terrain_map[y + 2][x + 1] = road;
    map->terrain_map[y + 2][x] = road;
    map->terrain_map[y + 2][x - 1] = road;
    map->terrain_map[y + 1][x - 1] = road;
    map->terrain_map[y][x - 1] = road;
    map->terrain_map[y - 1][x - 1] = road;

    if (y > gate_row + 1 && y > 3) { // pave road north to connect to west east
        y--;
        while (y > 1 && map->terrain_map[y - 1][x] != road) {
            y--;
            map->terrain_map[y][x] = road;
        }

        if (y != 1) {
            building_connected = 1;
        }
    } else if (y < gate_row - 2 && y < MAP_HEIGHT - 4) { // pave road south to connect to west east
        y += 2;
        while (y < MAP_HEIGHT - 2 && map->terrain_map[y + 1][x] != road) {
            y++;
            map->terrain_map[y][x] = road;
        }

        if (y != MAP_HEIGHT - 2) {
            building_connected = 1;
        }
    }

    // Handle corner cases and east and west edge cases
    if (building_connected == 0) { // must be in a corner or edge, so try to connect
        if (map->get_w() > 0) { // If there is a west gate, move towards it
            while (x > 0 && map->terrain_map[y][x - 1] != road) {
                x--;
                map->terrain_map[y][x] = road;
            }

            if (x > 0) {
                building_connected = 1;
            }
        } else { // Otherwise go to east gate
            while (x < MAP_WIDTH - 1 && map->terrain_map[y][x + 1] != road) {
                x++;
                map->terrain_map[y][x] = road;
            }

            if (x < MAP_WIDTH - 1) {
                building_connected = 1;
            }
        }

        if (map->get_n() > 0 && building_connected == 0) { // move towards north gate
            while (y > 1 && map->terrain_map[y - 1][x] != road) {
                y--;
                map->terrain_map[y][x] = road;
            }
        } else if (building_connected == 0) { // move towards south gate
            while (y < MAP_HEIGHT - 1 && map->terrain_map[y + 1][x] != road) {
                y++;
                map->terrain_map[y][x] = road;
            }
        }
    }
}

void place_buildings(map *map, int manhattan_distance) {
    terrain_e first_building, second_building;
    int i, y, x;

    i = rand() % 2;

    first_building = (i < 1) ? pokemart : pokecenter;
    second_building = (first_building == pokemart) ? pokecenter : pokemart;

    int building_probability = ((-45 * manhattan_distance) / 200 + 50);

    // place first building in left half of the map
    if (manhattan_distance == 0 || rand() % 100 < building_probability) {
        y = rand() % 16 + 2; // rows 2 - 17
        x = rand() % 36 + 2; // cols 2 - 37

        building_to_map(map, first_building, y, x, map->get_w());
    }

    if (manhattan_distance == 0 || rand() % 100 < building_probability) {
        // place second building in the right half of the map
        y = rand() % 16 + 2; // rows 2 - 17
        x = rand() % 36 + 41; // cols 41 - 76

        building_to_map(map, second_building, y, x, map->get_e());
    }
}

/* Connect the gates with one road going north to south and another going west to east. */
void pave_roads(map *map)
{
    int x_run = map->get_s() - map->get_n();
    int y, x;

    y = 1;
    x = map->get_n();

    // pave north to south road
    if (map->get_n() > 0 && map->get_s() > 0) {
        while (y < (MAP_HEIGHT - 2) / 2) { // pave road halfway down
            map->terrain_map[y][x] = road;
            y++;
        }

        if (x_run < 0) { // south gate is to the west
            while (x > map->get_s()) {
                map->terrain_map[y][x] = road;
                x--;
            }
        } else if (x_run > 0) { // south gate is to the east
            while (x < map->get_s()) {
                map->terrain_map[y][x] = road;
                x++;
            }
        }

        while (y < MAP_HEIGHT - 1) { // pave road the rest of the way down
            map->terrain_map[y][x] = road;
            y++;
        }
    }

    int y_rise = map->get_e() - map->get_w();

    x = 1;
    y = map->get_w();

    // pave west to east road
    if (map->get_w() > 0 && map->get_e() > 0) {
        while (x < (MAP_WIDTH - 2) / 2) { // pave road halfway east
            map->terrain_map[y][x] = road;
            x++;
        }

        if (y_rise < 0) { // east gate is to the north
            while (y > map->get_e()) {
                map->terrain_map[y][x] = road;
                y--;
            }
        } else if (y_rise > 0) { // east gate is to the south
            while (y < map->get_e()) {
                map->terrain_map[y][x] = road;
                y++;
            }
        }

        while (x < MAP_WIDTH - 1) { // pave road the rest of the way over
            map->terrain_map[y][x] = road;
            x++;
        }
    }
}

/* Handle cases for paving the roads where the current map is on the edge or corner of the world */
void pave_roads_edge(map *map)
{
    int y, x;

    if (map->get_n() < 0) {
        x = map->get_s();

        if (map->get_w() < 0) { // northwest corner
            for (y = MAP_HEIGHT - 2; y > map->get_e(); y--) {
                map->terrain_map[y][x] = road;
            }

            for (x = map->get_s(); x < MAP_WIDTH - 1; x++) {
                map->terrain_map[y][x] = road;
            }
        } else if (map->get_e() < 0) { // northeast corner
            for (y = MAP_HEIGHT - 2; y > map->get_w(); y--) {
                map->terrain_map[y][x] = road;
            }

            for (x = map->get_s(); x > 0; x--) {
                map->terrain_map[y][x] = road;
            }
        } else { // along north edge
            pave_roads(map); // pave west to east road

            // pave south road up to west-east road
            y = MAP_HEIGHT - 2;
            while (map->terrain_map[y][x] != road && y > 0) {
                map->terrain_map[y][x] = road;
                y--;
            }
        }
    } else if (map->get_s() < 0) {
        x = map->get_n();

        if (map->get_e() < 0) { // southeast corner
            for (y = 0; y < map->get_w(); y++) {
                map->terrain_map[y][x] = road;
            }

            for (x = map->get_n(); x > 0; x--) {
                map->terrain_map[y][x] = road;
            }
        } else if (map->get_w() < 0) { // southwest corner
            for (y = 0; y < map->get_e(); y++) {
                map->terrain_map[y][x] = road;
            }

            for (x = map->get_n(); x < MAP_WIDTH - 1; x++) {
                map->terrain_map[y][x] = road;
            }
        } else { // south edge
            pave_roads(map); // pave west to east road

            // pave north road down to west-east road
            y = 0;
            while (map->terrain_map[y][x] != road && y < MAP_HEIGHT - 1) {
                map->terrain_map[y][x] = road;
                y++;
            }
        }
    } else if (map->get_w() < 0) { // west edge of map
        pave_roads(map); //pave north to south road

        y = map->get_e();
        x = MAP_WIDTH - 2;

        // pave road from east gate to north-south road
        while (map->terrain_map[y][x] != road && x > 0) {
            map->terrain_map[y][x] = road;
            x--;
        }
    } else if (map->get_e() < 0) { // east edge of map
        pave_roads(map); //pave north to south road

        y = map->get_w();
        x = 1;

        // pave road from east gate to north-south road
        while (map->terrain_map[y][x] != road && x < MAP_WIDTH - 1) {
            map->terrain_map[y][x] = road;
            x++;
        }
    }
}

/* Generate the regions in each half of the map to get a better terrain distribution. */
void generate_regional_terrain(map *map, int west_bound, int east_bound)
{
    int x, y, region_width, region_height;
    int i, row, col;
    terrain_e regions[5] = {boulder, tree, water, tall_grass, tall_grass};

    for (i = 0; i < 5; i++) {
        // get a random coordinate in the range
        x = rand() % (east_bound - west_bound);
        x += west_bound;
        y = rand() % (MAP_HEIGHT - 3) + 2;

        // get a specified size for the region
        region_width = rand() % 30 + 10;
        region_height = rand() % 6 + 5;

        for (row = 0; row < region_height; row++) {
            if (y + row < MAP_HEIGHT - 1) { // row is in bounds
                for (col = 0; col < region_width; col++) {
                    if (x + col < MAP_WIDTH - 1) { // column is in bounds
                        map->terrain_map[y + row][x + col] = regions[i];
                    } else { // column out of bounds
                        map->terrain_map[y + row][x - col] = regions[i];
                    }
                }
            } else { // row out of bounds
                for (col = 0; col < region_width; col++) {
                    if (x + col < MAP_WIDTH - 1) { // column is in bounds
                        map->terrain_map[y - row][x + col] = regions[i];
                    } else { // column out of bounds
                        map->terrain_map[y - row][x - col] = regions[i];
                    }
                }
            }
        }
    }
}

/* Fill in the terrain for the map. */
void generate_terrain(map *map)
{
    generate_regional_terrain(map, 1, MAP_WIDTH / 2 - 1); // covers cols 1-38
    generate_regional_terrain(map, MAP_WIDTH / 2 - 1, MAP_WIDTH - 1); // covers cols 39-78

    // now add some spontaneous terrain that is impassable
    int i, y, x;

    for (i = 0; i < 20; i++) {
        y = rand() % (MAP_HEIGHT - 2) + 1;
        x = rand() % (MAP_WIDTH - 2) + 1;

        map->terrain_map[y][x] = edge;

        y = rand() % (MAP_HEIGHT - 2) + 1;
        x = rand() % (MAP_WIDTH - 2) + 1;

        map->terrain_map[y][x] = willow;
    }
}

/* Fill in the edge of the given map with borders and gates. */
void construct_border(map *map, int n, int s, int w, int e)
{
    // First make the entire edge, then place the 4 gates
    int i, j, tmp;

    for (i = 0; i < MAP_HEIGHT; i++) {
        map->terrain_map[i][0] = edge;
        map->terrain_map[i][MAP_WIDTH - 1] = edge;
    }

    for (j = 0; j < MAP_WIDTH; j++) {
        map->terrain_map[0][j] = edge;
        map->terrain_map[MAP_HEIGHT - 1][j] = edge;
    }

    // 0 means generate a gate
    tmp = (n == 0) ? rand() % (MAP_WIDTH - 2) + 1 : n; // Minus 2 and plus 1 to ensure corners aren't gates
    map->set_n(tmp);
    tmp = (s == 0) ? rand() % (MAP_WIDTH - 2) + 1 : s;
    map->set_s(tmp);
    tmp = (w == 0) ? rand() % (MAP_HEIGHT - 2) + 1 : w;
    map->set_w(tmp);
    tmp = (e == 0) ? rand() % (MAP_HEIGHT - 2) + 1 : e;
    map->set_e(tmp);

    // -1 means don't generate a gate
    map->terrain_map[0][map->get_n()] = (n == -1) ? edge : gate;
    map->terrain_map[MAP_HEIGHT - 1][map->get_s()] = (s == -1) ? edge : gate;
    map->terrain_map[map->get_w()][0] = (w == -1) ? edge : gate;
    map->terrain_map[map->get_e()][MAP_WIDTH - 1] = (e == -1) ? edge : gate;
}

/* Take the given map and populate it with the necessary types of terrain, roads, and buildings. */
void generate_map(map *map, int n, int s, int w, int e, int manhattan_distance)
{
    construct_border(map, n, s, w, e);

    generate_terrain(map);

    if (n < 0 || s < 0 || w < 0 || e < 0) { // if we are by an edge, handle roads specially
        pave_roads_edge(map);
    } else {
        pave_roads(map);
    }

    place_buildings(map, manhattan_distance);

    build_bridges(map);
}

/* Check if trainer can go to location based on terrain and if it is occupied or not. Return 0 if trainer can be at location. */
int check_trainer_position(map *map, coordinate_t pos, trainer_type_e type)
{
    int check = 1;

    if (map->terrain_map[pos.y][pos.x] == edge ||
        map->terrain_map[pos.y][pos.x] == willow ||
        map->trainer_map[pos.y][pos.x] != nullptr) {
        check = 0;
    } else if ((type == swimmer_e && map->terrain_map[pos.y][pos.x] != water) ||
               (type != swimmer_e && map->terrain_map[pos.y][pos.x] == water)) {
        check = 0;
    } else if (map->terrain_map[pos.y][pos.x] == boulder ||
               map->terrain_map[pos.y][pos.x] == tree) {
        if (type != hiker_e) {
            check = 0;
        }
    } else if ((map->terrain_map[pos.y][pos.x] == gate ||
                map->terrain_map[pos.y][pos.x] == road ||
                map->terrain_map[pos.y][pos.x] == bridge) &&
               (type != pc_e)) {
        check = 0;
    }

    return check;
}

/* Add the PC to a road in this map. */
void place_new_pc(heap_t *turn_heap, map *map, Pokemon *starter)
{
    int y, x, count;
    count = 0;

    // loop through the map once counting all the roads
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain_map[y][x] == road || map->terrain_map[y][x] == bridge) {
                count++;
            }
        }
    }

    // create an array of the size of the number of roads
    coordinate_t start;
    coordinate_t start_options[count];

    // Add the coordinates of each road to the array
    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain_map[y][x] == road || map->terrain_map[y][x] == bridge) {
                start_options[count].x = x;
                start_options[count].y = y;
                count++;
            }
        }
    }

    start = start_options[rand() % count];
    map->trainer_map[start.y][start.x] = new pc(starter);
    map->set_pc_pos(start);
    map->trainer_map[start.y][start.x]->set_pos(start);
    heap_insert(turn_heap, map->trainer_map[start.y][start.x]);
}

void place_pc(map *map, pc *pc)
{
    int y, x, count;
    count = 0;

    // loop through the map once counting all the roads
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain_map[y][x] == road || map->terrain_map[y][x] == bridge) {
                count++;
            }
        }
    }

    coordinate_t start_options[count]; // create an array of the size of the number of roads

    // Add the coordinates of each road to the array
    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain_map[y][x] == road || map->terrain_map[y][x] == bridge) {
                start_options[count].x = x;
                start_options[count].y = y;
                count++;
            }
        }
    }

    pc->set_pos(start_options[rand() % count]);
}

void place_npc(heap_t *turn_heap, map *map, Data *data, trainer_type_e type, int manhattan_distance)
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

    map->trainer_map[start.y][start.x] = new npc(data, type, manhattan_distance);
    map->trainer_map[start.y][start.x]->set_pos(start);
    heap_insert(turn_heap, map->trainer_map[start.y][start.x]);
}

void trainer_map_init(map *map, Data *data, int num_trainers, pc *pc, Pokemon *starter, int manhattan_distance)
{
    int x, y, npc_count;

    map->set_turn_heap((heap_t *) malloc(sizeof (heap_t)));
    heap_init(map->get_turn_heap(), turn_cmp, nullptr);

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            map->trainer_map[y][x] = nullptr;
        }
    }

    if (pc == nullptr && starter != nullptr) {
        place_new_pc(map->get_turn_heap(), map, starter);
    } else {
        map->trainer_map[pc->get_pos().y][pc->get_pos().x] = pc;
        map->set_pc_pos(pc->get_pos());
        pc->set_next_turn(0);
    }

    if (num_trainers == 1) {
        place_npc(map->get_turn_heap(), map, data, (trainer_type_e) (rand() % 7 + 1), manhattan_distance);
    } else if (num_trainers >= 2) {
        place_npc(map->get_turn_heap(), map, data, hiker_e, manhattan_distance);
        place_npc(map->get_turn_heap(), map, data, rival_e, manhattan_distance);

        npc_count = 2;
        if (num_trainers >= 7) {
            place_npc(map->get_turn_heap(), map, data, pacer_e, manhattan_distance);
            place_npc(map->get_turn_heap(), map, data, wanderer_e, manhattan_distance);
            place_npc(map->get_turn_heap(), map, data, sentry_e, manhattan_distance);
            place_npc(map->get_turn_heap(), map, data, explorer_e, manhattan_distance);
            place_npc(map->get_turn_heap(), map, data, swimmer_e, manhattan_distance);
            npc_count = 7;
        }

        while(npc_count < num_trainers) {
            place_npc(map->get_turn_heap(), map, data, (trainer_type_e) (rand() % 7 + 1), manhattan_distance);
            npc_count++;
        }
    }
}
