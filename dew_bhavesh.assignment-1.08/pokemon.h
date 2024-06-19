#include <string>
#include <vector>

#include "database.h"

class Pokemon {
private:
    int pokemon_id;
    int species_id;
    std::string name;
    int gender;
    int is_shiny;
    int level;
    int hp;
    int hp_iv;
    int attack;
    int attack_iv;
    int defense;
    int defense_iv;
    int special_attack;
    int special_attack_iv;
    int special_defense;
    int special_defense_iv;
    int speed;
    int speed_iv;
    int moves[2];
public:
    Pokemon(int pokemon_id,
            int species_id,
            std::string name,
            int gender,
            int is_shiny,
            int level,
            int hp,
            int hp_iv,
            int attack,
            int attack_iv,
            int defense,
            int defense_iv,
            int special_attack,
            int special_attack_iv,
            int special_defense,
            int special_defense_iv,
            int speed,
            int speed_iv,
            int moves[2]) : moves()
    {
        this->pokemon_id = pokemon_id;
        this->species_id = species_id;
        this->name = name;
        this->gender = gender;
        this->is_shiny = is_shiny;
        this->level = level;
        this->hp = hp;
        this->hp_iv = hp_iv;
        this->attack = attack;
        this->attack_iv = attack_iv;
        this->defense = defense;
        this->defense_iv = defense_iv;
        this->special_attack = special_attack;
        this->special_attack_iv = special_attack_iv;
        this->special_defense = special_defense;
        this->special_defense_iv = special_defense_iv;
        this->speed = speed;
        this->speed_iv = speed_iv;
        this->moves[0] = moves[0];
        this->moves[1] = moves[1];
    }
    ~Pokemon() {}
    std::string get_name() { return name; }  
    int get_gender() { return gender; }
    int get_is_shiny() { return is_shiny; }
    int get_level() { return level; }
    int get_hp() { return hp; }
    int get_hp_iv() { return hp_iv; }
    int get_attack() { return attack; }
    int get_attack_iv() { return attack_iv; }
    int get_defense() { return defense; }
    int get_defense_iv() { return defense_iv; }
    int get_special_attack() { return special_attack; }
    int get_special_attack_iv() { return special_attack_iv; }
    int get_special_defense() { return special_defense; }
    int get_special_defense_iv() { return special_defense_iv; }
    int get_speed() { return speed; }
    int get_speed_iv() { return speed_iv; }
    int get_move(int index) { return moves[index]; }
};

Pokemon *generate_pokemon(Data *data, 
                          int pokemon_id, 
                          int gender, 
                          int level, 
                          int is_shiny,
                          int hp_iv,
                          int attack_iv,
                          int defense_iv,
                          int speed_iv,
                          int special_attack_iv,
                          int special_defense_iv);

int calculate_level(int manhattan_distance);

void generate_npc_party(Data *data, Pokemon *party[6], int manhattan_distance);
