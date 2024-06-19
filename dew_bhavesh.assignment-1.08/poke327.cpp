#include <iostream>
#include <ncurses.h>
#include <cstring>

#include "world.h"

static int32_t path_cmp(const void *key, const void *with) { 
    return ((path *) key)->get_cost() - ((path *) with)->get_cost();
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

void render_view(map *map, coordinate_t location)
{
    int y, x;
    int color;

    clear();

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            switch (map->terrain_map[y][x])
            {
                case grass:
                    view[y][x] = '.';
                    color = COLOR_GREEN;
                    break;
                case tall_grass:
                    view[y][x] = ':';
                    color = COLOR_GREEN;
                    break;
                case boulder:
                case edge:
                    view[y][x] = '%';
                    color = COLOR_WHITE;
                    break;
                case tree:
                case willow:
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

            if  (map->trainer_map[y][x] != nullptr) {
                color = COLOR_RED;
                switch (map->trainer_map[y][x]->get_type()) {
                    case pc_e:
                        view[y][x] = '@';
                        break;
                    case hiker_e:
                        view[y][x] = 'h';
                        break;
                    case rival_e:
                        view[y][x] = 'r';
                        break;
                    case pacer_e:
                        view[y][x] = 'p';
                        break;
                    case wanderer_e:
                        view[y][x] = 'w';
                        break;
                    case sentry_e:
                        view[y][x] = 's';
                        break;
                    case explorer_e:
                        view[y][x] = 'e';
                        break;
                    case swimmer_e:
                        view[y][x] = 'm';
                        break;
                }
            }

            attron(COLOR_PAIR(color));
            mvaddch(1 + y, x, view[y][x]);
            attroff(COLOR_PAIR(color));
        }
    }

    mvprintw(22, 1, "(%d, %d)", location.x - START_X, START_Y - location.y);

    refresh();
}

Pokemon *pick_starter(Data *data)
{
    int choice;
    Pokemon *first = (Pokemon *)generate_pokemon(data, 
                                                 rand() % 1092,
                                                 rand() % 2,
                                                 1,
                                                 rand() % 8192 == 0 ? 1 : 0,
                                                 rand() % 16,
                                                 rand() % 16,
                                                 rand() % 16,
                                                 rand() % 16,
                                                 rand() % 16,
                                                 rand() % 16);
    Pokemon *second = (Pokemon *)generate_pokemon(data, 
                                                  rand() % 1092,
                                                  rand() % 2,
                                                  1,
                                                  rand() % 8192 == 0 ? 1 : 0,
                                                  rand() % 16,
                                                  rand() % 16,
                                                  rand() % 16,
                                                  rand() % 16,
                                                  rand() % 16,
                                                  rand() % 16);
    Pokemon *third = (Pokemon *)generate_pokemon(data, 
                                                 rand() % 1092,
                                                 rand() % 2,
                                                 1,
                                                 rand() % 8192 == 0 ? 1 : 0,
                                                 rand() % 16,
                                                 rand() % 16,
                                                 rand() % 16,
                                                 rand() % 16,
                                                 rand() % 16,
                                                 rand() % 16);
    
    choice = choose_starter(first, second, third);

    switch (choice) {
        case 'a':
            return first;
        case 'b':
            return second;
        case 'c':
            return third;
        default:
            return nullptr;
    }
}

void place_gates(world *world)
{
    if (world->get_location().y == 0) { 
        world->get_current_map()->set_n(-1);
    } else if (world->get_location().y == WORLD_HEIGHT - 1) { 
        world->get_current_map()->set_s(-1);
    } 
    
    if (world->get_location().x == 0) { 
        world->get_current_map()->set_w(-1);
    } else if (world->get_location().x == WORLD_WIDTH - 1) { 
        world->get_current_map()->set_e(-1);
    }

   
    if (world->get_location().y > 0 && world->board[world->get_location().y - 1][world->get_location().x] != nullptr) {
        world->get_current_map()->set_n(world->board[world->get_location().y - 1][world->get_location().x]->get_s());
    }

   
    if (world->get_location().y < WORLD_HEIGHT - 1 && world->board[world->get_location().y + 1][world->get_location().x] != nullptr) {
        world->get_current_map()->set_s(world->board[world->get_location().y + 1][world->get_location().x]->get_n());
    } 
    
    
    if (world->get_location().x > 0 && world->board[world->get_location().y][world->get_location().x - 1] != nullptr) {
        world->get_current_map()->set_w(world->board[world->get_location().y][world->get_location().x - 1]->get_e());
    }

     
    if (world->get_location().x < WORLD_WIDTH - 1 && world->board[world->get_location().y][world->get_location().x + 1] != nullptr) {
        world->get_current_map()->set_e(world->board[world->get_location().y][world->get_location().x + 1]->get_w());
    }
}

void change_map(world *world, Data *data, pc *pc, char new_map)
{
    int fly_x, fly_y;
    int manhattan_distance;

    world->get_current_map()->trainer_map[pc->get_pos().y][pc->get_pos().x] = nullptr;
    world->get_current_map()->set_pc_turn(pc->get_next_turn());
    
    if (new_map == 'n' && world->get_location().y > 0) {
        world->set_location_y(world->get_location().y - 1);
        pc->set_pos_y(MAP_HEIGHT - 2);
    } else if (new_map == 's' && world->get_location().y < WORLD_HEIGHT - 1) {
        world->set_location_y(world->get_location().y + 1);
        pc->set_pos_y(1);
    } else if (new_map == 'w' && world->get_location().x > 0) {
        world->set_location_x(world->get_location().x - 1);
        pc->set_pos_x(MAP_WIDTH - 2);
    } else if (new_map == 'e' && world->get_location().x < WORLD_WIDTH - 1) {
        world->set_location_x(world->get_location().x + 1);
        pc->set_pos_x(1);
    } else if (new_map == 'f') {
        mvprintw(22, 1, "Fly to: ");
        refresh();
        echo();
        scanw((char *)"%d %d", &fly_x, &fly_y);
        noecho();
        if (fly_x >= -1 * WORLD_WIDTH / 2 && fly_x <= WORLD_WIDTH / 2) {
            world->set_location_x(START_X + fly_x);
        }

        if (fly_y >= -1 * WORLD_HEIGHT / 2 && fly_y <= WORLD_HEIGHT / 2) {
            world->set_location_y(START_Y - fly_y);
        }
    }

    if (world->board[world->get_location().y][world->get_location().x] == nullptr) {
        world->set_current_map(new map());
        world->board[world->get_location().y][world->get_location().x] = world->get_current_map();

        place_gates(world);
        manhattan_distance = abs(world->get_location().x - START_X) + abs(world->get_location().y - START_Y);
        generate_map(world->get_current_map(), world->get_current_map()->get_n(), world->get_current_map()->get_s(), world->get_current_map()->get_w(), world->get_current_map()->get_e(), manhattan_distance);

        if (new_map == 'f') {
            place_pc(world->get_current_map(), pc);
        }
        trainer_map_init(world->get_current_map(), data, world->get_num_trainers(), pc, (Pokemon *)nullptr, manhattan_distance);  
    } else if (new_map == 'f') {
        world->board[world->get_location().y][world->get_location().x]->trainer_map[pc->get_pos().y][pc->get_pos().x] = nullptr;
        place_pc(world->board[world->get_location().y][world->get_location().x], pc);
    }

    world->set_current_map(world->board[world->get_location().y][world->get_location().x]);
    pc->set_next_turn(world->get_current_map()->get_pc_turn());
    world->get_current_map()->trainer_map[pc->get_pos().y][pc->get_pos().x] = pc;
    world->get_current_map()->set_pc_pos(pc->get_pos());
}

int pc_turn(world *world, Data *data, pc *pc, int manhattan_distance)
{
    int quit_game, valid, key;
    char new_map;

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
                new_map = move_pc(world->get_current_map(), data, pc, key, manhattan_distance);
                valid = 1;
                break;
            case 'f':
                new_map = 'f';
                valid = 1;
                break;
            case '5':
            case ' ':
            case '.':
                pc->set_next_turn(pc->get_next_turn() + 15);
                valid = 1;
                break;
            case '>':
                enter_building(world->get_current_map(), pc);
                valid = 1;
                break;
            case 't':
                trainer_info(world->get_current_map(), world->get_num_trainers());
                render_view(world->get_current_map(), world->get_location());
                valid = 0;
                continue;
            case 'Q':
                quit_game = 1;
                valid = 1;
                break;
            default:
                mvprintw(22, 1, "Use movement keys, info key, or 'Q' to quit.");
                valid = 0;
        }
    }

    if (pc->get_next_turn() < 0) {
        quit_game = 1;
    }

    if (new_map != 0) {
        change_map(world, data, pc, new_map);
    }

    return quit_game;
}

void game_loop(heap_t *path_heap, world *world, Data *data)
{
    trainer *t;
    pc *p;
    npc *n;
    int manhattan_distance;
    int x, y, quit_game;
    path swimmer_path[MAP_HEIGHT][MAP_WIDTH];

    quit_game = 0;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            swimmer_path[y][x] = *new path();
        }
    }

    render_view(world->get_current_map(), world->get_location());

    while (!quit_game) {
        t = (trainer *) heap_remove_min(world->get_current_map()->get_turn_heap());

        if (t->get_type() != pc_e) {
            n = (npc *)t;
        }

        switch (t->get_type()) {
            case pc_e:
                p = (pc *)t;
                manhattan_distance = abs(world->get_location().x - START_X) + abs(world->get_location().y - START_Y);
                quit_game = pc_turn(world, data, p, manhattan_distance);
                t = p;
                break;
            case hiker_e:
                move_dijkstra_trainer(path_heap, world->hiker_path, world->get_current_map(), n, data);
                break;
            case rival_e:
                move_dijkstra_trainer(path_heap, world->rival_path, world->get_current_map(), n, data);
                break;
            case pacer_e:
                move_pacer(world->get_current_map(), n, data);
                break;
            case wanderer_e:
            case explorer_e:
                move_wanderer_explorer(world->get_current_map(), n, data);
                break;
            case sentry_e:
                n->set_next_turn(n->get_next_turn() + 15);
                break;
            case swimmer_e:
                move_swimmer(path_heap, swimmer_path, world->get_current_map(), n, world->get_current_map()->get_pc_pos(), data);
                break;
        }
        if (t->get_type() != pc_e) {
            t = n;
        }

        render_view(world->get_current_map(), world->get_location());

        if (t->get_next_turn() > -1) {
            heap_insert(world->get_current_map()->get_turn_heap(), t);
        }

        if (world->get_current_map()->trainer_map[world->get_current_map()->get_pc_pos().y][world->get_current_map()->get_pc_pos().x] != nullptr &&
            world->get_current_map()->trainer_map[world->get_current_map()->get_pc_pos().y][world->get_current_map()->get_pc_pos().x]->get_next_turn() == -1) {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int manhattan_distance;
    heap_t path_heap;
    world *w;
    int got_data;
    Data data;
    Pokemon *starter;

    if (argc == 2) {
        if (strcmp(argv[1], "--numtrainers") == 0) {
            w = new world(atoi(argv[2]));
            got_data = data.get_data(data);

            if (!got_data) {
                std::cerr << "No data found." << std::endl;
            }
        } else {

            got_data = 0;
        }
    } else if (argc == 1) {
        w = new world(10);
        got_data = data.get_data(data);

        if (!got_data) {
            std::cerr << "No data found." << std::endl;

            got_data = 0;
        }
    } else {

        got_data = 0;
    }

    if (got_data) {
        srand(time(nullptr));

        init_terminal();

        init_view();

        heap_init(&path_heap, path_cmp, nullptr);

        place_gates(w);
        manhattan_distance = abs(w->get_location().x - START_X) + abs(w->get_location().y - START_Y);
        generate_map(w->get_current_map(), w->get_current_map()->get_n(), w->get_current_map()->get_s(), w->get_current_map()->get_w(), w->get_current_map()->get_e(), manhattan_distance);
        starter = (Pokemon *)pick_starter(&data);
        trainer_map_init(w->get_current_map(), &data, w->get_num_trainers(), (pc *)nullptr, (Pokemon *)starter, 0); 

        game_loop(&path_heap, w, &data);

        endwin();

        heap_delete(&path_heap);
    } 

    return !got_data;
}
