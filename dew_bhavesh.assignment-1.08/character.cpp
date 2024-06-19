#include <ncurses.h>
#include <string>
#include <cstring>

#include "character.h"

int check_random_turn(int random_direction, map *map, coordinate_t pos, trainer_type_e type) {
    int check, terrain_cost;

    check = 0;

    if (random_direction == 0) { 
        if (type == wanderer_e || type == swimmer_e) {
            if (map->terrain_map[pos.y][pos.x] == map->terrain_map[pos.y - 1][pos.x] && map->trainer_map[pos.y - 1][pos.x] == nullptr) {
                check = 1;
            }
        } else if (type == explorer_e) {
            terrain_cost = path::calculate_terrain_cost(map->terrain_map[pos.y - 1][pos.x], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y - 1][pos.x] == nullptr) {
                check = 1;
            }
        }
    } else if (random_direction == 1) { 
        if (type == wanderer_e || type == swimmer_e) {
            if (map->terrain_map[pos.y][pos.x] == map->terrain_map[pos.y][pos.x + 1] && map->trainer_map[pos.y][pos.x + 1] == nullptr) {
                check = 1;
            }
        } else if (type == explorer_e) {
            terrain_cost = path::calculate_terrain_cost(map->terrain_map[pos.y][pos.x + 1], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y][pos.x + 1] == nullptr) {
                check = 1;
            }
        }
    } else if (random_direction == 2) { 
        
        if (type == wanderer_e || type == swimmer_e) {
            if (map->terrain_map[pos.y][pos.x] == map->terrain_map[pos.y + 1][pos.x] && map->trainer_map[pos.y + 1][pos.x] == nullptr) {
                check = 1;
            }
        } else if (type == explorer_e) {
            terrain_cost = path::calculate_terrain_cost(map->terrain_map[pos.y + 1][pos.x], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y + 1][pos.x] == nullptr) {
                check = 1;
            }
        }
    }  else if (random_direction == 3) { 
        if (type == wanderer_e || type == swimmer_e) {
            if (map->terrain_map[pos.y][pos.x] == map->terrain_map[pos.y][pos.x - 1] && map->trainer_map[pos.y][pos.x - 1] == nullptr) {
                check = 1;
            }
        } else if (type == explorer_e) {
            terrain_cost = path::calculate_terrain_cost(map->terrain_map[pos.y][pos.x - 1], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y][pos.x - 1] == nullptr) {
                check = 1;
            }
        }
    }

    return check;
}

void random_turn(map *map, trainer *t, Data *data) {
    int random_direction, valid_direction;
    int num_fail;
    int battle_outcome;

    num_fail = 0;
    random_direction = rand() % 4;
    valid_direction = check_random_turn(random_direction, map, t->get_pos(), t->get_type());
    while (!valid_direction) {
        num_fail++;
        if (num_fail > 20) {
            return;
        }
        random_direction = rand() % 4;
        valid_direction = check_random_turn(random_direction, map, t->get_pos(), t->get_type());
    }

    if (random_direction == 0) {
        t->set_dir_y(-1);
        t->set_dir_x(0);
        if (map->trainer_map[t->get_pos().y - 1][t->get_pos().x    ] != nullptr &&
            map->trainer_map[t->get_pos().y - 1][t->get_pos().x    ]->get_type() == pc_e) {
            battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], (npc *)t, data);

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                random_turn(map, t, data);
            } else if (battle_outcome == -1) { 
                t->set_next_turn(-1);
            }
        } else {
            map->trainer_map[t->get_pos().y][t->get_pos().x] = nullptr;
            t->set_pos_y(t->get_pos().y - 1);
            map->trainer_map[t->get_pos().y][t->get_pos().x] = t;
        }
    } else if (random_direction == 1) {
        t->set_dir_y(0);
        t->set_dir_x(1);
        if (map->trainer_map[t->get_pos().y    ][t->get_pos().x + 1] != nullptr &&
            map->trainer_map[t->get_pos().y    ][t->get_pos().x + 1]->get_type() == pc_e) {
            battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], (npc *)t, data);

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                random_turn(map, t, data);
            } else if (battle_outcome == -1) { 
                t->set_next_turn(-1);
            }
        } else {
            map->trainer_map[t->get_pos().y][t->get_pos().x] = nullptr;
            t->set_pos_x(t->get_pos().x + 1);
            map->trainer_map[t->get_pos().y][t->get_pos().x] = t;
        }
    } else if (random_direction == 2) { 
        t->set_dir_y(1);
        t->set_dir_x(0);
        if (map->trainer_map[t->get_pos().y + 1][t->get_pos().x    ] != nullptr &&
            map->trainer_map[t->get_pos().y + 1][t->get_pos().x    ]->get_type() == pc_e) {
            battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], (npc *)t, data);

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                random_turn(map, t, data);
            } else if (battle_outcome == -1) { 
                t->set_next_turn(-1);
            }
        } else {
            map->trainer_map[t->get_pos().y][t->get_pos().x] = nullptr;
            t->set_pos_y(t->get_pos().y + 1);
            map->trainer_map[t->get_pos().y][t->get_pos().x] = t;
        }
    } else if (random_direction == 3) { 
        t->set_dir_y(0);
        t->set_dir_x(-1);
        if (map->trainer_map[t->get_pos().y    ][t->get_pos().x - 1] != nullptr &&
            map->trainer_map[t->get_pos().y    ][t->get_pos().x - 1]->get_type() == pc_e) {
            battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], (npc *)t, data);

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                random_turn(map, t, data);
            } else if (battle_outcome == -1) { 
                t->set_next_turn(-1);
            }
        } else {
            map->trainer_map[t->get_pos().y][t->get_pos().x] = nullptr;
            t->set_pos_x(t->get_pos().x - 1);
            map->trainer_map[t->get_pos().y][t->get_pos().x] = t;
        }
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

int choose_starter(Pokemon *first, Pokemon *second, Pokemon *third)
{
    int key;
    mvprintw(0, 1, "Choose your starter Pokemon. Press a, b, or c to select.");
    mvprintw(1, 1, first->get_name().c_str());
    mvprintw(2, 1, second->get_name().c_str());
    mvprintw(3, 1, third->get_name().c_str());
    refresh();

    while((key = getch()))
    {
        if (key == 'a')
            break;
        else if (key == 'b')
            break;
        else if (key == 'c')
            break;
        else
            continue;
        mvprintw(0, 1, "Choose your starter Pokemon. Press a, b, or c to select.");
        mvprintw(1, 1, first->get_name().c_str());
        mvprintw(2, 1, second->get_name().c_str());
        mvprintw(3, 1, third->get_name().c_str());
        refresh();
    }

    return key;
}

void enter_building(map *map, pc *pc)
{
    int key;
    WINDOW *building_win;
    if (map->terrain_map[pc->get_pos().y][pc->get_pos().x] == pokecenter || map->terrain_map[pc->get_pos().y][pc->get_pos().x] == pokemart) {
        building_win = create_newwin(24, 80, 0, 0);
        keypad(building_win, TRUE);
        mvwprintw(building_win, 1, 1, "You are in a building. Press '<' to exit.");
        wrefresh(building_win);

        while((key = getch()) != '<')
        {
            wprintw(building_win, "You are in a building. Press '<' to exit.");
            wrefresh(building_win);
        }

        destroy_win(building_win);

        pc->set_next_turn(pc->get_next_turn() + 10);
    }
}

void trainer_info(map *map, int num_trainers)
{
    int x, y, npc_rise, npc_run, key, i, shift;
    char trainer_char;
    std::string output;
    WINDOW *trainer_win;
    char *trainer_info[num_trainers];

    trainer_win = create_newwin(10, 24, 1, 0);
    i = 0;
    keypad(trainer_win, TRUE);

    for (y = 1; y < MAP_HEIGHT - 1; y++) {
        for (x = 1; x < MAP_WIDTH - 1; x++) {
            if (map->trainer_map[y][x] != nullptr) {
                output = "";
                switch (map->trainer_map[y][x]->get_type())
                {
                    case pc_e:
                        trainer_char = '@';
                        break;
                    case hiker_e:
                        trainer_char = 'h';
                        break;
                    case rival_e:
                        trainer_char = 'r';
                        break;
                    case pacer_e:
                        trainer_char = 'p';
                        break;
                    case wanderer_e:
                        trainer_char = 'w';
                        break;
                    case sentry_e:
                        trainer_char = 's';
                        break;
                    case explorer_e:
                        trainer_char = 'e';
                        break;
                    case swimmer_e:
                        trainer_char = 'm';
                        break;
                }

                npc_rise = map->get_pc_pos().y - map->trainer_map[y][x]->get_pos().y;
                npc_run = map->get_pc_pos().x - map->trainer_map[y][x]->get_pos().x;

                if (npc_rise < 0) { 
                    if (npc_run < 0) {
                        output.append(1, trainer_char);
                        output.append(", ");
                        output.append(std::to_string(npc_rise * -1));
                        output.append(" south and east ");
                        output.append(std::to_string(npc_run * -1));
                    } else { 
                        output.append(1, trainer_char);
                        output.append(", ");
                        output.append(std::to_string(npc_rise * -1));
                        output.append(" south and west ");
                        output.append(std::to_string(npc_run));
                    }
                } else { 
                    if (npc_run < 0) { 
                        output.append(1, trainer_char);
                        output.append(", ");
                        output.append(std::to_string(npc_rise));
                        output.append(" north and east ");
                        output.append(std::to_string(npc_run * -1));
                    } else { 
                        output.append(1, trainer_char);
                        output.append(", ");
                        output.append(std::to_string(npc_rise));
                        output.append(" north and west ");
                        output.append(std::to_string(npc_run));
                    }
                }

                trainer_info[i] = (char *) malloc(output.size());
                strcpy(trainer_info[i], output.c_str());
                i++;
            }
        }
    }

    for (i = 0; i < num_trainers || i < 10; i++) {
        mvwprintw(trainer_win, i, 0, trainer_info[i]);
    }

    wrefresh(trainer_win);

    shift = 0;
    while((key = getch()) != 27)
    {
        switch (key)
        {
            case KEY_DOWN:
                if (num_trainers > 10 && num_trainers - shift > 10) {
                    shift++;
                    for (i = 0; i < 10 && i + shift < num_trainers; i++) {
                        mvwprintw(trainer_win, i, 0, trainer_info[i + shift]);
                    }
                    wrefresh(trainer_win);
                }
                break;
            case KEY_UP:
                if (shift > 0 && num_trainers > 10) {
                    shift--;
                    for (i = 0; i < 10 && i + shift < num_trainers; i++) {
                        mvwprintw(trainer_win, i, 0, trainer_info[i + shift]);
                    }
                    wrefresh(trainer_win);
                }
                break;
            default:
                continue;
        }
    }

    for (i = 0; i < num_trainers; i++) {
        free(trainer_info[i]);
    }

    destroy_win(trainer_win);
}

int battle(pc *pc, npc *npc, Data *data)
{
    int key, i, row;
    WINDOW *battle_win;

    battle_win = create_newwin(24, 80, 0, 0);
    keypad(battle_win, TRUE);
    row = 2;
    for (i = 0; i < 6; i++) {
        if (!pc->get_pokemon(i)) {
            break;
        }
        mvwprintw(battle_win, 1, 1, "You are in a battle. Press '<' to exit.");
        mvwprintw(battle_win, row, 1, "%s: Level: %d HP: %d", pc->get_pokemon(i)->get_name().c_str(), 
                                                            pc->get_pokemon(i)->get_level(),
                                                            pc->get_pokemon(i)->get_hp());
        if (pc->get_pokemon(i)->get_move(1) != 0) {
            mvwprintw(battle_win, row + 1, 1, "Moves: %s, %s", data->moves[pc->get_pokemon(i)->get_move(0) - 1].identifier.c_str(), data->moves[pc->get_pokemon(i)->get_move(1) - 1].identifier.c_str()); // subtract one because the id is 1 off of data vector index for corresponding move
        } else {
            mvwprintw(battle_win, row + 1, 1, "Moves: %s", data->moves[pc->get_pokemon(i)->get_move(0) - 1].identifier.c_str());
        }
        row = row + 2;
    }
    row = 2;
    for (i = 0; i < 6; i++) {
        if (!npc->get_pokemon(i)) {
            break;
        }
        mvwprintw(battle_win, row, 40, "%s: Level: %d HP: %d", npc->get_pokemon(i)->get_name().c_str(), 
                                                            npc->get_pokemon(i)->get_level(),
                                                            npc->get_pokemon(i)->get_hp());
        if (npc->get_pokemon(i)->get_move(1) != 0) {
            mvwprintw(battle_win, row + 1, 40, "Moves: %s, %s", data->moves[npc->get_pokemon(i)->get_move(0) - 1].identifier.c_str(), data->moves[npc->get_pokemon(i)->get_move(1) - 1].identifier.c_str()); // subtract one because the id is 1 off of data vector index for corresponding move
        } else {
            mvwprintw(battle_win, row + 1, 40, "Moves: %s", data->moves[npc->get_pokemon(i)->get_move(0) - 1].identifier.c_str());
        }
        row = row + 2;
    }
    wrefresh(battle_win);

    while((key = getch()) != '<')
    {
        row = 2;
        for (i = 0; i < 6; i++) {
            if (!pc->get_pokemon(i)) {
                break;
            }
            mvwprintw(battle_win, 1, 1, "You are in a battle. Press '<' to exit.");
            mvwprintw(battle_win, row, 1, "%s: Level: %d HP: %d", pc->get_pokemon(i)->get_name().c_str(), 
                                                                pc->get_pokemon(i)->get_level(),
                                                                pc->get_pokemon(i)->get_hp());
            if (pc->get_pokemon(i)->get_move(1) != 0) {
                mvwprintw(battle_win, row + 1, 1, "Moves: %s, %s", data->moves[pc->get_pokemon(i)->get_move(0) - 1].identifier.c_str(), data->moves[pc->get_pokemon(i)->get_move(1) - 1].identifier.c_str()); // subtract one because the id is 1 off of data vector index for corresponding move
            } else {
                mvwprintw(battle_win, row + 1, 1, "Moves: %s", data->moves[pc->get_pokemon(i)->get_move(0) - 1].identifier.c_str());
            }
            row = row + 2;
        }
        row = 2;
        for (i = 0; i < 6; i++) {
            if (!npc->get_pokemon(i)) {
                break;
            }
            mvwprintw(battle_win, row, 40, "%s: Level: %d HP: %d", npc->get_pokemon(i)->get_name().c_str(), 
                                                                npc->get_pokemon(i)->get_level(),
                                                                npc->get_pokemon(i)->get_hp());
            if (npc->get_pokemon(i)->get_move(1) != 0) {
                mvwprintw(battle_win, row + 1, 40, "Moves: %s, %s", data->moves[npc->get_pokemon(i)->get_move(0) - 1].identifier.c_str(), data->moves[npc->get_pokemon(i)->get_move(1) - 1].identifier.c_str()); // subtract one because the id is 1 off of data vector index for corresponding move
            } else {
                mvwprintw(battle_win, row + 1, 40, "Moves: %s", data->moves[npc->get_pokemon(i)->get_move(0) - 1].identifier.c_str());
            }
            row = row + 2;
        }
        wrefresh(battle_win);
    }

    destroy_win(battle_win);

    return 0;
}

Pokemon *encounter(Data *data, int manhattan_distance) 
{
    int key;
    WINDOW *encounter_win;
    int level;
    Pokemon *pokemon;
    level = calculate_level(manhattan_distance);
    pokemon = (Pokemon *)generate_pokemon(data, 
                                          rand() % 1092,
                                          rand() % 2,
                                          level,
                                          rand() % 8192 == 0 ? 1 : 0,
                                          rand() % 16,
                                          rand() % 16,
                                          rand() % 16,
                                          rand() % 16,
                                          rand() % 16,
                                          rand() % 16);

    encounter_win = create_newwin(24, 80, 0, 0);
    mvwprintw(encounter_win, 1, 1, "You encountered a Pokemon! Press the escape key to exit.");
    mvwprintw(encounter_win, 2, 1, "%s:", pokemon->get_name().c_str());
    mvwprintw(encounter_win, 3, 1, "Gender: %d", pokemon->get_gender());
    mvwprintw(encounter_win, 4, 1, "Is Shiny: %d", pokemon->get_is_shiny());
    mvwprintw(encounter_win, 5, 1, "Level: %d", pokemon->get_level());
    mvwprintw(encounter_win, 6, 1, "HP: %d", pokemon->get_hp());
    mvwprintw(encounter_win, 7, 1, "Attack: %d", pokemon->get_attack());
    mvwprintw(encounter_win, 8, 1, "Defense: %d", pokemon->get_defense());
    mvwprintw(encounter_win, 9, 1, "Special Attack: %d", pokemon->get_special_attack());
    mvwprintw(encounter_win, 10, 1, "Special Defense: %d", pokemon->get_special_defense());
    mvwprintw(encounter_win, 11, 1, "Speed: %d", pokemon->get_speed());
    if (pokemon->get_move(1) != 0) {
        mvwprintw(encounter_win, 12, 1, "Moves: %s, %s", data->moves[pokemon->get_move(0) - 1].identifier.c_str(), data->moves[pokemon->get_move(1) - 1].identifier.c_str()); // subtract one because the id is 1 off of data vector index for corresponding move
    } else {
        mvwprintw(encounter_win, 12, 1, "Moves: %s", data->moves[pokemon->get_move(0) - 1].identifier.c_str());
    }
    wrefresh(encounter_win);

    while((key = getch()) != 27)
    {
        mvwprintw(encounter_win, 1, 1, "You encountered a Pokemon! Press the escape key to exit.");
        mvwprintw(encounter_win, 2, 1, "%s:", pokemon->get_name().c_str());
        mvwprintw(encounter_win, 3, 1, "Gender: %d", pokemon->get_gender());
        mvwprintw(encounter_win, 4, 1, "Is Shiny: %d", pokemon->get_is_shiny());
        mvwprintw(encounter_win, 5, 1, "Level: %d", pokemon->get_level());
        mvwprintw(encounter_win, 6, 1, "HP: %d", pokemon->get_hp());
        mvwprintw(encounter_win, 7, 1, "Attack: %d", pokemon->get_attack());
        mvwprintw(encounter_win, 8, 1, "Defense: %d", pokemon->get_defense());
        mvwprintw(encounter_win, 9, 1, "Special Attack: %d", pokemon->get_special_attack());
        mvwprintw(encounter_win, 10, 1, "Special Defense: %d", pokemon->get_special_defense());
        mvwprintw(encounter_win, 11, 1, "Speed: %d", pokemon->get_speed());
        if (pokemon->get_move(1) != 0) {
            mvwprintw(encounter_win, 12, 1, "Moves: %s, %s", data->moves[pokemon->get_move(0) - 1].identifier.c_str(), data->moves[pokemon->get_move(1) - 1].identifier.c_str()); // subtract one because the id is 1 off of data vector index for corresponding move
        } else {
            mvwprintw(encounter_win, 12, 1, "Moves: %s", data->moves[pokemon->get_move(0) - 1].identifier.c_str());
        }
        wrefresh(encounter_win);
    }

    destroy_win(encounter_win);

    return 0;
}

char move_pc(map *map, Data *data, pc *pc, int input, int manhattan_distance)
{
    if (map->get_turn_heap()) {}
    char ret_val;
    coordinate_t new_pos;
    int battle_outcome;
    Pokemon *pokemon_captured;

    ret_val = 0;

    if (input == '7' || input == 'y') {
        new_pos.y = pc->get_pos().y - 1;
        new_pos.x = pc->get_pos().x - 1;
    } else if (input == '8' || input == 'k') {
        new_pos.y = pc->get_pos().y - 1;
        new_pos.x = pc->get_pos().x    ;
    } else if (input == '9' || input == 'u') {
        new_pos.y = pc->get_pos().y - 1;
        new_pos.x = pc->get_pos().x + 1;
    } else if (input == '6' || input == 'l') {
        new_pos.y = pc->get_pos().y    ;
        new_pos.x = pc->get_pos().x + 1;
    } else if (input == '3' || input == 'n') {
        new_pos.y = pc->get_pos().y + 1;
        new_pos.x = pc->get_pos().x + 1;
    } else if (input == '2' || input == 'j') {
        new_pos.y = pc->get_pos().y + 1;
        new_pos.x = pc->get_pos().x    ;
    } else if (input == '1' || input == 'b') {
        new_pos.y = pc->get_pos().y + 1;
        new_pos.x = pc->get_pos().x - 1;
    } else if (input == '4' || input == 'h') {
        new_pos.y = pc->get_pos().y    ;
        new_pos.x = pc->get_pos().x - 1;
    }

    if (path::calculate_terrain_cost(map->terrain_map[new_pos.y][new_pos.x], pc->get_type()) != INT_MAX) {
        // Determine if in top row of map or right column of map or bottom row of map or left column of map 
        if (map->terrain_map[new_pos.y][new_pos.x] == gate) {
            if (new_pos.y == 0) {
                ret_val = 'n';
            } else if (new_pos.y == MAP_HEIGHT - 1) {
                ret_val = 's';
            } else if (new_pos.x == 0) {
                ret_val = 'w';
            } else if (new_pos.x == MAP_WIDTH - 1) {
                ret_val = 'e';
            }  
        }
        
        if (map->trainer_map[new_pos.y][new_pos.x] != nullptr) {
            if (map->trainer_map[new_pos.y][new_pos.x]->get_next_turn() > -1) {
                battle_outcome = battle(pc, (npc *)map->trainer_map[new_pos.y][new_pos.x], data);

                if (battle_outcome == 1) { 
                    pc->set_next_turn(-1);
                } else if (battle_outcome == -1) { 
                    map->trainer_map[new_pos.y][new_pos.x]->set_next_turn(-1);
                }
            }
        } else if (rand() % 10 < 1 && map->terrain_map[new_pos.y][new_pos.x] == tall_grass) {
            pokemon_captured = encounter(data, manhattan_distance);
            if (pokemon_captured) {
                pc->add_pokemon(pokemon_captured);
            }
        } else {
            map->trainer_map[pc->get_pos().y][pc->get_pos().x] = nullptr;
            pc->set_pos(new_pos);
            map->trainer_map[pc->get_pos().y][pc->get_pos().x] = pc;
            map->set_pc_pos(pc->get_pos());
        }

        if (pc->get_next_turn() > -1) {
            pc->set_next_turn(pc->get_next_turn() + path::calculate_terrain_cost(map->terrain_map[pc->get_pos().y][pc->get_pos().x], pc->get_type()));
        }
    }

    return ret_val;
}

void move_dijkstra_trainer(heap_t *path_heap, path path_map[MAP_HEIGHT][MAP_WIDTH], map *map, npc *npc, Data *data)
{
    path *cheapest_path;
    int battle_outcome;

    cheapest_path = &path_map[npc->get_pos().y - 1][npc->get_pos().x - 1];
    battle_outcome = 0;

    path::dijkstra_path(path_heap, map, path_map, npc->get_type());

    if (path_map[npc->get_pos().y - 1][npc->get_pos().x].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y - 1][npc->get_pos().x];
    }
    if (path_map[npc->get_pos().y - 1][npc->get_pos().x + 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y - 1][npc->get_pos().x + 1];
    }
    if (path_map[npc->get_pos().y][npc->get_pos().x - 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y][npc->get_pos().x - 1];
    }
    if (path_map[npc->get_pos().y][npc->get_pos().x + 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y][npc->get_pos().x + 1];
    }
    if (path_map[npc->get_pos().y + 1][npc->get_pos().x - 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y + 1][npc->get_pos().x - 1];
    }
    if (path_map[npc->get_pos().y + 1][npc->get_pos().x].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y + 1][npc->get_pos().x];
    }
    if (path_map[npc->get_pos().y + 1][npc->get_pos().x + 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y + 1][npc->get_pos().x + 1];
    }
    if (cheapest_path->get_cost() == 0) {
        cheapest_path = &path_map[npc->get_pos().y    ][npc->get_pos().x    ];

        battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], npc, data);

        if (battle_outcome == 1) { 
            map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
        } else if (battle_outcome == -1) { 
            npc->set_next_turn(-1);
        }
    }

    map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
    npc->set_pos(cheapest_path->get_coordinate());
    map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
    if (npc->get_next_turn() > -1) {
        npc->set_next_turn(npc->get_next_turn() + cheapest_path->get_terrain_cost());
    }
}

void move_wanderer_explorer(map *map, npc *npc, Data *data)
{
    int move_cost, battle_outcome;
    terrain_e current_terrain;

    current_terrain = map->terrain_map[npc->get_pos().y][npc->get_pos().x];

    // just starting or already going up and need to continue
    if (npc->get_dir().x == 0 && npc->get_dir().y <= 0) {
        move_cost = path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y - 1][npc->get_pos().x], npc->get_type());

        if ((npc->get_type() == wanderer_e && (current_terrain == map->terrain_map[npc->get_pos().y - 1][npc->get_pos().x])) ||
            (npc->get_type() == explorer_e && (move_cost != INT_MAX))) {
            npc->set_dir_y(-1);
            if (map->trainer_map[npc->get_pos().y - 1][npc->get_pos().x    ] == nullptr) {
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
                npc->set_pos_y(npc->get_pos().y - 1);
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
            } else if (map->trainer_map[npc->get_pos().y - 1][npc->get_pos().x    ]->get_type() == pc_e) {
                battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], npc, data);

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                    random_turn(map, npc, data);
                } else if (battle_outcome == -1) { 
                    npc->set_next_turn(-1);
                }
            } else {
                random_turn(map, npc, data);
            }
        } else {
            random_turn(map, npc, data);
        }
        // just starting or already going right and need to continue
    } else if (npc->get_dir().x >= 0 && npc->get_dir().y == 0) {
        move_cost = path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y][npc->get_pos().x + 1], npc->get_type());

        if ((npc->get_type() == wanderer_e && (current_terrain == map->terrain_map[npc->get_pos().y][npc->get_pos().x + 1])) ||
            (npc->get_type() == explorer_e && (move_cost != INT_MAX))) {
            npc->set_dir_x(1);
            if (map->trainer_map[npc->get_pos().y    ][npc->get_pos().x + 1] == nullptr) {
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
                npc->set_pos_x(npc->get_pos().x + 1);
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
            }  else if (map->trainer_map[npc->get_pos().y    ][npc->get_pos().x + 1]->get_type() == pc_e) {
                battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], npc, data);

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                    random_turn(map, npc, data);
                } else if (battle_outcome == -1) { 
                    npc->set_next_turn(-1);
                }
            } else {
                random_turn(map, npc, data);
            }
        } else {
            random_turn(map, npc, data);
        }
        // just starting or already going down and need to continue
    } else if (npc->get_dir().x == 0 && npc->get_dir().y >= 0) {
        move_cost = path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y + 1][npc->get_pos().x], npc->get_type());

        if ((npc->get_type() == wanderer_e && (current_terrain == map->terrain_map[npc->get_pos().y + 1][npc->get_pos().x])) ||
            (npc->get_type() == explorer_e && (move_cost != INT_MAX))) {
            npc->set_dir_y(1);
            if (map->trainer_map[npc->get_pos().y + 1][npc->get_pos().x    ] == nullptr) {
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
                npc->set_pos_y(npc->get_pos().y + 1);
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
            } else if (map->trainer_map[npc->get_pos().y + 1][npc->get_pos().x    ]->get_type() == pc_e) {
                battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], npc, data);

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                    random_turn(map, npc, data);
                } else if (battle_outcome == -1) { 
                    npc->set_next_turn(-1);
                }
            } else {
                random_turn(map, npc, data);
            }
        } else {
            random_turn(map, npc, data);
        }
        // just starting or already going left and need to continue
    } else if (npc->get_dir().x <= 0 && npc->get_dir().y == 0) {
        move_cost = path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y    ][npc->get_pos().x - 1], npc->get_type());

        if ((npc->get_type() == wanderer_e && (current_terrain == map->terrain_map[npc->get_pos().y    ][npc->get_pos().x - 1])) ||
            (npc->get_type() == explorer_e && (move_cost != INT_MAX))) {
            npc->set_dir_x(-1);
            if (map->trainer_map[npc->get_pos().y    ][npc->get_pos().x - 1] == nullptr) {
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
                npc->set_pos_x(npc->get_pos().x - 1);
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
            } else if (map->trainer_map[npc->get_pos().y    ][npc->get_pos().x - 1]->get_type() == pc_e) {
                battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], npc, data);

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                    random_turn(map, npc, data);
                } else if (battle_outcome == -1) { 
                    npc->set_next_turn(-1);
                }
            } else {
                random_turn(map, npc, data);
            }
        } else {
            random_turn(map, npc, data);
        }
    }

    if (npc->get_next_turn() > -1) {
        npc->set_next_turn(npc->get_next_turn() + path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y][npc->get_pos().x], npc->get_type()));
    }
}

void move_swimmer(heap_t *path_heap, path path_map[MAP_HEIGHT][MAP_WIDTH], map *map, npc *m, coordinate_t pc_pos, Data *data)
{
    int battle_outcome;

    // path_map to pc
    if ((map->terrain_map[pc_pos.y - 1][pc_pos.x    ] == water ||
         map->terrain_map[pc_pos.y - 1][pc_pos.x    ] == bridge) ||
        (map->terrain_map[pc_pos.y    ][pc_pos.x + 1] == water ||
         map->terrain_map[pc_pos.y    ][pc_pos.x + 1] == bridge) ||
        (map->terrain_map[pc_pos.y + 1][pc_pos.x    ] == water ||
         map->terrain_map[pc_pos.y + 1][pc_pos.x    ] == bridge) ||
        (map->terrain_map[pc_pos.y    ][pc_pos.x - 1] == water ||
         map->terrain_map[pc_pos.y    ][pc_pos.x - 1] == bridge)) {
        move_dijkstra_trainer(path_heap, path_map, map, m, data);
        // move similar to wanderer_e
    } else {
        // just starting or already going up and need to continue
        if (m->get_dir().x == 0 && m->get_dir().y <= 0) {
            if (map->terrain_map[m->get_pos().y - 1][m->get_pos().x] == water ||
                map->terrain_map[m->get_pos().y - 1][m->get_pos().x] == bridge) {
                m->set_dir_y(-1);
                if (map->trainer_map[m->get_pos().y - 1][m->get_pos().x] == nullptr) {
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = nullptr;
                    m->set_pos_y(m->get_pos().y - 1);
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = m;
                } else if (map->trainer_map[m->get_pos().y - 1][m->get_pos().x]->get_type() == pc_e) {
                    battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], m, data);

                    if (battle_outcome == 1) { // m won
                        map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                        random_turn(map, m, data);
                    } else if (battle_outcome == -1) { // m lost
                        m->set_next_turn(-1);
                    }
                } else {
                    random_turn(map, m, data);
                }
            } else {
                random_turn(map, m, data);
            }
            // just starting or already going right and need to continue
        } else if (m->get_dir().x >= 0 && m->get_dir().y == 0) {
            if (map->terrain_map[m->get_pos().y][m->get_pos().x + 1] == water ||
                map->terrain_map[m->get_pos().y][m->get_pos().x + 1] == bridge) {
                m->set_dir_x(1);
                if (map->trainer_map[m->get_pos().y][m->get_pos().x + 1] == nullptr) {
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = nullptr;
                    m->set_pos_x(m->get_pos().x + 1);
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = m;
                } else if (map->trainer_map[m->get_pos().y][m->get_pos().x + 1]->get_type() == pc_e) {
                    battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], m, data);

                    if (battle_outcome == 1) { // m won
                        map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                        random_turn(map, m, data);
                    } else if (battle_outcome == -1) { // m lost
                        m->set_next_turn(-1);
                    }
                } else {
                    random_turn(map, m, data);
                }
            } else {
                random_turn(map, m, data);
            }
            // just starting or already going down and need to continue
        } else if (m->get_dir().x == 0 && m->get_dir().y >= 0) {
            if (map->terrain_map[m->get_pos().y + 1][m->get_pos().x] == water ||
                map->terrain_map[m->get_pos().y + 1][m->get_pos().x] == bridge) {
                m->set_dir_y(1);
                if (map->trainer_map[m->get_pos().y + 1][m->get_pos().x] == nullptr) {
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = nullptr;
                    m->set_pos_y(m->get_pos().y + 1);
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = m;
                } else if (map->trainer_map[m->get_pos().y + 1][m->get_pos().x]->get_type() == pc_e) {
                    battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], m, data);

                    if (battle_outcome == 1) { // m won
                        map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                        random_turn(map, m, data);
                    } else if (battle_outcome == -1) { // m lost
                        m->set_next_turn(-1);
                    }
                } else {
                    random_turn(map, m, data);
                }
            } else {
                random_turn(map, m, data);
            }
            // just starting or already going left and need to continue
        } else if (m->get_dir().x <= 0 && m->get_dir().y == 0) {
            if (map->terrain_map[m->get_pos().y][m->get_pos().x - 1] == water ||
                map->terrain_map[m->get_pos().y][m->get_pos().x - 1] == bridge) {
                m->set_dir_x(-1);
                if (map->trainer_map[m->get_pos().y][m->get_pos().x - 1] == nullptr) {
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = nullptr;
                    m->set_pos_x(m->get_pos().x - 1);
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = m;
                } else if (map->trainer_map[m->get_pos().y][m->get_pos().x - 1]->get_type() == pc_e) {
                    battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], m, data);

                    if (battle_outcome == 1) { // m won
                        map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                        random_turn(map, m, data);
                    } else if (battle_outcome == -1) { // m lost
                        m->set_next_turn(-1);
                    }
                } else {
                    random_turn(map, m, data);
                }
            } else {
                random_turn(map, m, data);
            }
        }

        if (m->get_next_turn() > -1) {
            m->set_next_turn(m->get_next_turn() + path::calculate_terrain_cost(map->terrain_map[m->get_pos().y][m->get_pos().x], m->get_type()));
        }
    }
}

void move_pacer(map *map, npc *p, Data *data)
{
    terrain_e current_terrain;
    int battle_outcome;
    int moved;

    current_terrain = map->terrain_map[p->get_pos().y][p->get_pos().x];
    moved = 0;
    // Just starting or moving right and the right is open
    if (p->get_dir().x >= 0 && map->terrain_map[p->get_pos().y][p->get_pos().x + 1] == current_terrain) {
        if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1] == nullptr) {
            moved = 1;
            p->set_dir_x(1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
            p->set_pos_x(p->get_pos().x + 1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
        } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1]->get_type() == pc_e) {
            battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], p, data);

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
            } else if (battle_outcome == -1) { 
                p->set_next_turn(-1);
            }
        }
        // Just starting or moving left and checking if left is open
    } else if (p->get_dir().x <= 0 && map->terrain_map[p->get_pos().y][p->get_pos().x - 1] == current_terrain) {
        if (map->trainer_map[p->get_pos().y    ][p->get_pos().x - 1] == nullptr) {
            moved = 1;
            p->set_dir_x(-1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
            p->set_pos_x(p->get_pos().x - 1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
        } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x - 1]->get_type() == pc_e) {
            battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], p, data);

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
            } else if (battle_outcome == -1) { 
                p->set_next_turn(-1);
            }
        }
    }

    // moving left but need to go back right
    if (!moved && p->get_dir().x == -1 && map->terrain_map[p->get_pos().y][p->get_pos().x + 1] == current_terrain) {
        p->set_dir_x(1);
        if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1] == nullptr) {
            moved = 1;
            map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
            p->set_pos_x(p->get_pos().x + 1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
        } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1]->get_type() == pc_e) {
            battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], p, data);

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
            } else if (battle_outcome == -1) { 
                p->set_next_turn(-1);
            }
        }
    }


    if (!moved && p->get_dir().x == 1 && map->terrain_map[p->get_pos().y][p->get_pos().x - 1] == current_terrain) { 
        p->set_dir_x(-1);
        if (map->trainer_map[p->get_pos().y    ][p->get_pos().x - 1] == nullptr) {
            moved = 1;
            map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
            p->set_pos_x(p->get_pos().x - 1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
        } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x - 1]->get_type() == pc_e) {
            battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], p, data);

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
            } else if (battle_outcome == -1) { 
                p->set_next_turn(-1);
            }
        } else {
            if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1] == nullptr) {
                p->set_dir_x(1);
                moved = 1;
                map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
                p->set_pos_x(p->get_pos().x + 1);
                map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
            } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1]->get_type() == pc_e) {
                battle_outcome = battle((pc *)map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x], p, data);

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                } else if (battle_outcome == -1) { 
                    p->set_next_turn(-1);
                }
            }
        }
    }

    if (p->get_next_turn() > -1) {
        p->set_next_turn(p->get_next_turn() + path::calculate_terrain_cost(map->terrain_map[p->get_pos().y][p->get_pos().x], p->get_type()));
    }
}
