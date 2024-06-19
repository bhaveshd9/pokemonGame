#include "pokemon.h"

Pokemon *generate_pokemon(Data *data, 
                          int index, 
                          int gender, 
                          int level, 
                          int is_shiny,
                          int hp_iv,
                          int attack_iv,
                          int defense_iv,
                          int speed_iv,
                          int special_attack_iv,
                          int special_defense_iv)
{
    int species_id;
    std::string name;
    int hp = 0;
    int attack = 0;
    int defense = 0;
    int special_attack = 0;
    int special_defense = 0;
    int speed = 0;
    int moves[2];
    std::vector<int> move_options;
    size_t i, j;
    int moves_start = 0;
    int first_move, at_end;
    int found_move = 0;
    int second_move;

    species_id = data->pokemon_data[index].species_id;
    name = data->pokemon_data[index].identifier;

    // stats look in the assignment description
    for (i = 0; i < data->pokemon_stats.size(); i++) {
        if (data->pokemon_stats[i].pokemon_id == data->pokemon_data[index].id) {  // check that these are right
            if (data->pokemon_stats[i].stat_id == data->stats[0].id) {
                hp = data->pokemon_stats[i].base_stat;
            } else if (data->pokemon_stats[i].stat_id == data->stats[1].id) {
                attack = data->pokemon_stats[i].base_stat;
            } else if (data->pokemon_stats[i].stat_id == data->stats[2].id) {
                defense = data->pokemon_stats[i].base_stat;
            } else if (data->pokemon_stats[i].stat_id == data->stats[3].id) {
                special_attack = data->pokemon_stats[i].base_stat;
            } else if (data->pokemon_stats[i].stat_id == data->stats[4].id) {
                special_defense = data->pokemon_stats[i].base_stat;
            } else if (data->pokemon_stats[i].stat_id == data->stats[5].id) {
                speed = data->pokemon_stats[i].base_stat;
            }
        }
        if (hp > 0 && attack > 0 && defense > 0 && special_attack > 0 && special_defense > 0 && speed > 0) {
            break;
        }
    }

    // to get moves, level needs to be greater than or equal to level column in pokemon_moves,
    // and pokemon_move_method_id needs to be 1. then pokemon id needs to be equal to species id as well.
    while (move_options.size() == 0) {
        for (i = moves_start; i < data->pokemon_moves.size(); i++) {
            if (data->pokemon_moves[i].pokemon_id == species_id) {
                if (moves_start == 0) {
                    moves_start = i;
                }
                if (data->pokemon_moves[i].level <= level && data->pokemon_moves[i].pokemon_move_method_id == 1) {
                    if (data->pokemon_moves[i].pokemon_move_method_id == 1) {
                        at_end = 0;
                        j = 0;
                        while (!at_end && move_options.size() > 0 && move_options[j] != data->pokemon_moves[i].move_id) {
                            if (j == move_options.size() - 1) {
                                at_end = 1;
                            }
                            j++;
                        }
                        if (at_end || move_options.size() == 0) {
                            move_options.push_back(data->pokemon_moves[i].move_id);
                        }
                    }
                }
            }
            if (data->pokemon_moves[i].pokemon_id > species_id) {
                break;
            }
        }
        if (move_options.size() > 0) {
            first_move = rand() % move_options.size();
            if (!found_move) {
                moves[0] = move_options[first_move];
                found_move = 1;
            }
            if (move_options.size() > 1 && found_move) {
                second_move = rand() % move_options.size();
                while (moves[0] == move_options[second_move]) { // -1 because move ids start at 1 but array starts at 0
                    second_move = rand() % move_options.size();
                }
                moves[1] = move_options[second_move];
            } else if (move_options.size() == 1 && moves[0] == move_options[0] && moves[1] == 0) { // if there's only one move to choose from, and the first move is that move, and the 2nd move hasn't been picked yet, increment level
                level++;
                move_options.clear();
            }
        } else {
            level++;
            move_options.clear();
        }
    }

    return new Pokemon(data->pokemon_data[index].id,
                       species_id,
                       name,
                       gender,
                       is_shiny,
                       level,
                       hp,
                       hp_iv,
                       attack,
                       attack_iv,
                       defense,
                       defense_iv,
                       special_attack,
                       special_attack_iv,
                       special_defense,
                       special_defense_iv,
                       speed,
                       speed_iv,
                       moves);
}

int calculate_level(int manhattan_distance)
{
    int min_level;
    int max_level;

    if (manhattan_distance <= 200) {
        min_level = 1;
        max_level = manhattan_distance / 2;
    } else {
        min_level = (manhattan_distance - 200) / 2;
        max_level = 100;
    }
    
    return (max_level == 0) ? 1 : rand() % (max_level - min_level + 1) + min_level;
}

/*
    All generated trainers (NPCs) should be given at least one random Pokemon. Assuming the trainer has n Pokemon, there is a 60%
    probability that the trainer will get an (n + 1)th Pokemon, up to a maximum of 6 Pok ´ emon. The generated
    trainers’ Pokemon should be generated with levels and movesets as per the Pok ´ emon generation rules for
    their map.

    The levels of encountered pokemon will increase with the manhatten distance from the origin. When
    distance is less than or equal to 200, minimum level is 1 and maximum level is distance / 2 (integer division).
    When distance exceeds 200, minimum level becomes (distance - 200) / 2 and maximum level is 100.
*/
void generate_npc_party(Data *data, Pokemon *party[6], int manhattan_distance)
{
    int level;
    int num_pokemon = 0;
    while(num_pokemon == 0 || (num_pokemon <= 6 && rand() % 10 < 6)) {
        level = calculate_level(manhattan_distance);
        party[num_pokemon] = (Pokemon *)generate_pokemon(data, 
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
        num_pokemon++;
    }
}
