#include <climits>
#include <fstream>
#include <iostream>
#include "database.h"

std::ifstream *get_file(std::string filename)
{
    std::ifstream *file = new std::ifstream(); 
    std::string path = "/share/cs327/pokedex/pokedex/data/csv/" + filename;
    std::string home_path;
    if (char *home = std::getenv("HOME")) {
        home_path = std::string(home) + "/.poke327/pokedex/pokedex/data/csv/" + filename;
    }
    std::string local_path = "/Users/bhave/Desktop/Spring2023/327cs/Coms327/pokemon/pokedex/pokedex/data/csv/" + filename;

    file->open(path);
    if (!file->is_open()) {
        file->open(home_path);
    }
    if (!file->is_open() && !home_path.empty()) {
        file->open(local_path);
    }
    if (!file->is_open()) {
        delete file;
        file = 0;
    }

    return file;
}

int Data::get_pokemon(Data &data)
{
    int got_data = 0;
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
    Pokemon *p;
    std::ifstream *pokemon_file = get_file("pokemon.csv");
    if (!pokemon_file) {
        return got_data;
    }

    std::string line;
    std::getline(*pokemon_file, line);
    while (std::getline(*pokemon_file, line, ',')) {
        id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_file, identifier, ',');
        std::getline(*pokemon_file, line, ',');
        species_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_file, line, ',');
        height = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_file, line, ',');
        weight = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_file, line, ',');
        base_experience = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_file, line, ',');
        order = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_file, line);
        is_default = (line.empty()) ? INT_MAX : std::stoi(line);

        p = new Pokemon(id,
                        identifier,
                        species_id,
                        height,
                        weight,
                        base_experience,
                        order,
                        is_default);
        data.pokemon.push_back(*p);
    }
    got_data = 1;

    return got_data;
}

int Data::get_pokemon_types(Data &data)
{
    int got_data = 0;
    int pokemon_id;
    int type_id;
    int slot;
    PokemonType *pt;
    std::ifstream *pokemon_types_file = get_file("pokemon_types.csv");
    if (!pokemon_types_file) {
        return got_data;
    }

    std::string line;
    std::getline(*pokemon_types_file, line);
    while (std::getline(*pokemon_types_file, line, ',')) {
        pokemon_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_types_file, line, ',');
        type_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_types_file, line);
        slot = (line.empty()) ? INT_MAX : std::stoi(line);

        pt = new PokemonType(pokemon_id, type_id, slot);
        data.pokemon_types.push_back(*pt);
    }
    got_data = 1;

    return got_data;
}

int Data::get_type_names(Data &data)
{
    int got_data = 0;
    int type_id;
    int local_language_id;
    std::string name;
    TypeName *tn;
    std::ifstream *type_names_file = get_file("type_names.csv");
    if (!type_names_file) {
        return got_data;
    }

    std::string line;
    std::getline(*type_names_file, line);
    while (std::getline(*type_names_file, line, ',')) {
        type_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*type_names_file, line, ',');
        local_language_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*type_names_file, name);
        if (local_language_id == 9) {
            tn = new TypeName(type_id, local_language_id, name);
            data.type_names.push_back(*tn);
        }
    }
    got_data = 1;

    return got_data;
}

int Data::get_pokemon_species(Data &data)
{
    int got_data = 0;
    int id;
    std::string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;
    PokemonSpecies *ps;
    std::ifstream *pokemon_species_file = get_file("pokemon_species.csv");
    if (!pokemon_species_file) {
        return got_data;
    }

    std::string line;
    std::getline(*pokemon_species_file, line);
    while (std::getline(*pokemon_species_file, line, ',')) {
        id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, identifier, ',');
        std::getline(*pokemon_species_file, line, ',');
        generation_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        evolves_from_species_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        evolution_chain_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        color_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        shape_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        habitat_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        gender_rate = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        capture_rate = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        base_happiness = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        is_baby = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        hatch_counter = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        has_gender_differences = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        growth_rate_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        forms_switchable = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        is_legendary = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        is_mythical = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line, ',');
        order = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_species_file, line);
        conquest_order = (line.empty()) ? INT_MAX : std::stoi(line);

        ps = new PokemonSpecies(id, 
                                identifier, 
                                generation_id,
                                evolves_from_species_id,
                                evolution_chain_id,
                                color_id,
                                shape_id,
                                habitat_id,
                                gender_rate,
                                capture_rate,
                                base_happiness,
                                is_baby,
                                hatch_counter,
                                has_gender_differences,
                                growth_rate_id,
                                forms_switchable,
                                is_legendary,
                                is_mythical,
                                order,
                                conquest_order);
        data.pokemon_species.push_back(*ps);
    }
    got_data = 1;

    return got_data;
}

int Data::get_pokemon_stats(Data &data)
{
    int got_data = 0;
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
    PokemonStat *ps;
    std::ifstream *pokemon_stats_file = get_file("pokemon_stats.csv");
    if (!pokemon_stats_file) {
        return got_data;
    }

    std::string line;
    std::getline(*pokemon_stats_file, line);
    while (std::getline(*pokemon_stats_file, line, ',')) {
        pokemon_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_stats_file, line, ',');
        stat_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_stats_file, line, ',');
        base_stat = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_stats_file, line);
        effort = (line.empty()) ? INT_MAX : std::stoi(line);

        ps = new PokemonStat(pokemon_id, stat_id, base_stat, effort);
        data.pokemon_stats.push_back(*ps);
    }
    got_data = 1;

    return got_data;
}

int Data::get_stats(Data &data)
{
    int got_data = 0;
    int id;
    int damage_class_id;
    std::string identifier;
    int is_battle_only;
    int game_index;
    Stat *s;
    std::ifstream *stats_file = get_file("stats.csv");
    if (!stats_file) {
        return got_data;
    }

    std::string line;
    std::getline(*stats_file, line);
    while (std::getline(*stats_file, line, ',')) {
        id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*stats_file, line, ',');
        damage_class_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*stats_file, identifier, ',');
        std::getline(*stats_file, line, ',');
        is_battle_only = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*stats_file, line);
        game_index = (line.empty()) ? INT_MAX : std::stoi(line);

        s = new Stat(id, damage_class_id, identifier, is_battle_only, game_index);
        data.stats.push_back(*s);
    }
    got_data = 1;

    return got_data;
}

int Data::get_pokemon_moves(Data &data)
{
    int got_data = 0;
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
    PokemonMove *pm;
    std::ifstream *pokemon_moves_file = get_file("pokemon_moves.csv");
    if (!pokemon_moves_file) {
        return got_data;
    }

    std::string line;
    std::getline(*pokemon_moves_file, line);
    while (std::getline(*pokemon_moves_file, line, ',')) {
        pokemon_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_moves_file, line, ',');
        version_group_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_moves_file, line, ',');
        move_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_moves_file, line, ',');
        pokemon_move_method_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_moves_file, line, ',');
        level = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*pokemon_moves_file, line);
        order = (line.empty()) ? INT_MAX : std::stoi(line);

        pm = new PokemonMove(pokemon_id, 
                             version_group_id, 
                             move_id,
                             pokemon_move_method_id,
                             level,
                             order);
        data.pokemon_moves.push_back(*pm);
    }
    got_data = 1;

    return got_data;
}

int Data::get_moves(Data &data)
{
    int got_data = 0;
    int id;
    std::string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;
    Move *m;
    std::ifstream *moves_file = get_file("moves.csv");
    if (!moves_file) {
        return got_data;
    }

    std::string line;
    std::getline(*moves_file, line);
    while (std::getline(*moves_file, line, ',')) {
        id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, identifier, ',');
        std::getline(*moves_file, line, ',');
        generation_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        type_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        power = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        pp = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        accuracy = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        priority = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        target_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        damage_class_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        effect_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        effect_chance = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        contest_type_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line, ',');
        contest_effect_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*moves_file, line);
        super_contest_effect_id = (line.empty()) ? INT_MAX : std::stoi(line);

        m = new Move(id, 
                     identifier, 
                     generation_id,
                     type_id,
                     power,
                     pp,
                     accuracy,
                     priority,
                     target_id,
                     damage_class_id,
                     effect_id,
                     effect_chance,
                     contest_type_id,
                     contest_effect_id,
                     super_contest_effect_id);
        data.moves.push_back(*m);
    }
    got_data = 1;

    return got_data;
}

int Data::get_experience(Data &data)
{
    int got_data = 0;
    int growth_rate_id;
    int level;
    int experience;
    Experience *e;
    std::ifstream *experience_file = get_file("experience.csv");
    if (!experience_file) {
        return got_data;
    }

    std::string line;
    std::getline(*experience_file, line);
    while (std::getline(*experience_file, line, ',')) {
        growth_rate_id = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*experience_file, line, ',');
        level = (line.empty()) ? INT_MAX : std::stoi(line);
        std::getline(*experience_file, line);
        experience = (line.empty()) ? INT_MAX : std::stoi(line);

        e = new Experience(growth_rate_id, 
                           level, 
                           experience);
        data.experience.push_back(*e);
    }
    got_data = 1;

    return got_data;
}

int Data::get_data(Data &data)
{
    int got_data;
    got_data = get_pokemon(data);
    if (!got_data) {
        std::cerr << "No pokemon data." << std::endl;
    }
    got_data = get_pokemon_types(data);
    if (!got_data) {
        std::cerr << "No pokemon types data." << std::endl;
    }
    got_data = get_type_names(data);
    if (!got_data) {
        std::cerr << "No type names data." << std::endl;
    }
    got_data = get_pokemon_species(data);
    if (!got_data) {
        std::cerr << "No pokemon species data." << std::endl;
    }
    got_data = get_pokemon_stats(data);
    if (!got_data) {
        std::cerr << "No pokemon stats data." << std::endl;
    }
    got_data = get_stats(data);
    if (!got_data) {
        std::cerr << "No stats data." << std::endl;
    }
    got_data = get_pokemon_moves(data);
    if (!got_data) {
        std::cerr << "No pokemon moves data." << std::endl;
    }
    got_data = get_moves(data);
    if (!got_data) {
        std::cerr << "No moves data." << std::endl;
    }
    got_data = get_experience(data);
    if (!got_data) {
        std::cerr << "No experience data." << std::endl;
    }

    return got_data;
}