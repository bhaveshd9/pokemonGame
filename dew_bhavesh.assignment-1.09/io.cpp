#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

#include <vector>
#include <algorithm>
#include "io.h"
#include "poke327.h"
#include "math.h"

/*
*fighting moves, run away, switch pokemon, bag.
*/
// int move_cost[7] = {
//   // 0 -> run away, 1 -> catch Pokemon, 2 -> use Potion, 3 -> use Revive, 4 -> switch Pokemon, 5 -> Fighting move 1, 6 -> fighting move 2
//   { 0, 1, 2, 3, 4, 5, 6},
  
// };

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

//comparator for the pokemon battles.
static int32_t pokemon_battle_cmp(const void *key, const void *with)
{
  return ((WildPokemon *) key)->next_turn_cost = ((WildPokemon *) with)->next_turn_cost;
}
void io_init_terminal(void)
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
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character * const *) v1;
  const character *const *c2 = (const character * const *) v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character *io_nearest_visible_trainer()
{
  character **c, *n;
  uint32_t x, y, count;

  c = (character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  character *c;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, '%');
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, '^');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
        case ter_exit:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, '#');
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, 'M');
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, 'C');
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, ':');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, '.');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_water:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, '~');
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, '0');
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at %d %c by %d %c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] == INT_MAX ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static void io_list_trainers_display(npc **c,
                                     uint32_t count)
{
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */

  s = (char (*)[40]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  npc **c;
  uint32_t x, y, count;

  c = (npc **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = (npc *) world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  WINDOW * pokeWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
  mvwprintw(pokeWin, 0, 0, "Welcome to the Pokemon Mart.  Press 'r' to restock your bag");
  wrefresh(pokeWin);
  char command = wgetch(pokeWin);
  if(command == 'r')
  {
      world.pc.numPokeBalls += 10; 
      world.pc.numPotions += 10;
      world.pc.numRevives += 10;
    wclear(pokeWin);
    mvwprintw(pokeWin, 0, 0, "Your inventory has been restocked! Have a nice day!");
    wrefresh(pokeWin);
    wgetch(pokeWin);
    delwin(pokeWin);
  }
}

void io_pokemon_center()
{
  WINDOW * pokeWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
  mvwprintw(pokeWin, 0, 0, "Welcome to the Pokemon Center.  Press 'r' to restore your Pokemon to Full HP");
  wrefresh(pokeWin);
  char command = wgetch(pokeWin);

  if(command == 'r')
  {
    //sets PC's pokemon hp to whatever their max health is.
    long unsigned i;
    for(i = 0; i < world.pc.pokemonTeam.size(); i++)
    {
      world.pc.pokemonTeam.at(i).currHP = world.pc.pokemonTeam.at(i).hp;
    }
    mvwprintw(pokeWin, 0, 0, "Your Pokemon have been stored to full health! Good luck trainer!");
    wrefresh(pokeWin);
    wgetch(pokeWin);
    delwin(pokeWin);
  }
}
std::string io_pc_bag(WildPokemon p)
{
  std::string broke = "broke";
  bool valid = false;
  WINDOW * bagWin = newwin(getmaxy(stdscr), 50, 0, 0);
  box(bagWin, 0, 0);
  // wclear(bagWin);
  mvwprintw(bagWin, 1, 20, "BAG");
  mvwprintw(bagWin, 5, 1, "1: Pokeballs");
  mvwprintw(bagWin, 7, 1, "3: Potions");
  mvwprintw(bagWin, 6, 1, "2: Revives");
  mvwprintw(bagWin, 15, 5, "Enter 'backspace' to leave bag");
  wrefresh(bagWin);
  char bagChoice;
  while(valid == false)
  {
    bagChoice = wgetch(bagWin);

    switch(bagChoice)
    {
      case '1' :
        if(world.pc.numPokeBalls > 0 && p.capturedStatus != npc_owned)
        {
          world.pc.numPokeBalls--;
          wclear(bagWin);
          wrefresh(bagWin);
          delwin(bagWin);
          return "caught";
        }
        else if(p.capturedStatus == npc_owned)
        {
          mvwprintw(bagWin, 3, 1, "Can't capture a trainer owned Pokemon!");
          wgetch(bagWin);
        }
        else
        {
          mvwprintw(bagWin, 3, 1, "No more Pokeballs!");
        }
        break;
      case '2' :
        if(world.pc.numRevives > 0)
        {
          wclear(bagWin);
          wrefresh(bagWin);
          delwin(bagWin);
          return "revive";
        }
        else
        {
          mvwprintw(bagWin, 3, 1, "No more revives!");
        }
        break;
      case '3' :
        if(world.pc.numPotions > 0)
        {
          wclear(bagWin);
          wrefresh(bagWin);
          delwin(bagWin);
          return "potion";
        }
        else
        {
          mvwprintw(bagWin, 3, 1, "no more potions!");
        }
        break;
      case 127 :
        wclear(bagWin);
        wrefresh(bagWin);
        delwin(bagWin);
        valid = true;
        break;
      default :
        mvwprintw(bagWin, 3, 1, "Invalid input!");
        wrefresh(bagWin);
        break;
    }
  }
  return broke;
}
void io_battle(character *aggressor, character *defender)
{
  int counter;
  int damage;
  char move;
  WINDOW * pkmnWin;
  WINDOW * caughtWin;
  heap_t battleHeap;
  WildPokemon activePokemon;
  WildPokemon trainerPokemon;
  WildPokemon *heapPokemon;
  char commandInput;
  bool battle = true;
  npc *n = (npc *) ((aggressor == &world.pc) ? defender : aggressor);

  activePokemon = world.pc.pokemonTeam.at(0);
  trainerPokemon = (*n).pokemonTeam.at(0);

  WINDOW * win = newwin(getmaxy(stdscr), getmaxx(stdscr), 0, 0);
   heap_init(&battleHeap, pokemon_battle_cmp, NULL);
  while(battle)
  {
    wclear(win);
    activePokemon = world.pc.pokemonTeam.front();
    while(commandInput != 'c') // 'c' means the move is now confirmed. In place to allow for the player to choose another move if they already went to another screen for another move.
    {
      wclear(win);
          //Wild pokemon
      mvwprintw(win,1, 1, "Trainer encounter!");
      mvwprintw(win,3, 1, "Trainer Pokemon: ");
      mvwprintw(win,3, getmaxx(stdscr) / 9, "%s", trainerPokemon.name.c_str());
      mvwprintw(win,4, getmaxx(stdscr) / 9, "HP: %d/%d", trainerPokemon.currHP, trainerPokemon.hp);
          
          //Player
      mvwprintw(win,12, 1, "Your Pokemon:");
      mvwprintw(win,12, getmaxx(stdscr) / 9, "%s", activePokemon.name.c_str());
      mvwprintw(win,13, getmaxx(stdscr) / 9, "HP: %d/%d", activePokemon.currHP, activePokemon.hp);

      mvwprintw(win,14, getmaxx(stdscr) / 10, "-------------------");
      mvwprintw(win,15, getmaxx(stdscr) / 9, "FIGHT   PKMN");
      mvwprintw(win,16, getmaxx(stdscr) / 9, "BAG     RUN");
      mvwprintw(win,17, getmaxx(stdscr) / 10, "-------------------");
          
      wrefresh(win);
      commandInput = wgetch(win);
      mvwprintw(win,0, 0, "%c", commandInput);
      wrefresh(win);
      while(commandInput != 'f' && commandInput != 'r' && commandInput != 'p' && commandInput != 'B')
      {
        mvwprintw(win,18, getmaxx(stdscr) / 9, "invalid option. press f to fight, B to look through bag, p to switch pokemon or r to escape");
        wrefresh(win);
        commandInput = wgetch(win);
        wrefresh(win);
      }
      if(commandInput == 'p')
      {
        pkmnWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
        mvwprintw(pkmnWin, 8,  getmaxx(stdscr) / 9, "Active Pokemon");
        mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "%s", world.pc.pokemonTeam.at(0).name.c_str());
        mvwprintw(pkmnWin, 12,  getmaxx(stdscr) / 9, "Other Pokemon");
        int counter = 12;
        int index = 0;
        for(WildPokemon temp : world.pc.pokemonTeam)
        {
          if(index != 0)
          {
            mvwprintw(pkmnWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
          }
          index++;
        }
        mvwprintw(pkmnWin, 20,  getmaxx(stdscr) / 10, "Select the number of the pokemon you want to switch with");
        char switchMove = wgetch(pkmnWin);
        activePokemon.chosen_move = 4;
        activePokemon.next_turn_cost = 0;
        heap_insert(&battleHeap, &activePokemon);
        std::iter_swap(world.pc.pokemonTeam.begin(), world.pc.pokemonTeam.begin() + (switchMove - '0'));
        commandInput = 'c';
        delwin(pkmnWin);
        }
        else if(commandInput == 'r')
        {
          activePokemon.chosen_move = 0;
          activePokemon.next_turn_cost = 0;
          heap_insert(&battleHeap, &activePokemon);
          commandInput = 'c';
        }
        else if(commandInput == 'f')
        {
          wmove(win, 18, getmaxx(stdscr) / 9);
          wclrtoeol(win);
          mvwprintw(win,18, getmaxx(stdscr) / 9, "move 1: %s   move 2: %s", world.pc.pokemonTeam.at(0).learnedMoves[0].identifier.c_str(), world.pc.pokemonTeam.at(0).learnedMoves[1].identifier.c_str());
          wrefresh(win);
          move = wgetch(win);
          if(move == '1')
          {
            activePokemon.chosen_fighting_move = activePokemon.learnedMoves[0];
            activePokemon.chosen_move = 5;
          }
          else if(move == '2')
          {
            activePokemon.chosen_fighting_move = activePokemon.learnedMoves[1];
            activePokemon.chosen_move = 6;
          }
            heap_insert(&battleHeap, &activePokemon);
            commandInput = 'c';
        }
        else if(commandInput == 'B')
        {
          world.pc.numPokeBalls = 1;
          wmove(win, 18, getmaxx(stdscr) / 9);
          wclrtoeol(win);
          wrefresh(win);
          std::string bagInput = io_pc_bag(trainerPokemon);
          if(bagInput == "caught")
          {
            if(world.pc.pokemonTeam.size() > 5)
            {
              caughtWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
              box(caughtWin, 0, 0);
              mvwprintw(caughtWin, 1, getmaxx(stdscr) / 9, "You already have 6 Pokemon!");
              mvwprintw(caughtWin, 5, getmaxx(stdscr) / 9, "Choose another option!");
              wrefresh(caughtWin);
              wgetch(caughtWin);
              delwin(caughtWin);
            }
            else
            {
              activePokemon.chosen_move = 1;
              heap_insert(&battleHeap, &activePokemon);
              commandInput = 'c';
            }
          }
          else if(bagInput == "potion")
          {
            pkmnWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
            int counter = 12;
            int8_t index = 0;
            mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "Select the number of the Pokemon you want to restore hp to");
            for(WildPokemon temp : world.pc.pokemonTeam)
            {
              mvwprintw(pkmnWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
              index++;
            }
            char switchMove = wgetch(pkmnWin);

            while(switchMove - '0' > index)
            {
              mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "Invalid input! Select the number (1-6) of the Pokemon you want to heal");
              switchMove = wgetch(pkmnWin);
            }
            commandInput = 'c';
            activePokemon.chosen_move = 2;
            activePokemon.next_turn_cost = 0;
            heap_insert(&battleHeap, &activePokemon);
            world.pc.pokemonTeam.at(switchMove - '0').currHP += 20;
            if(world.pc.pokemonTeam.at(switchMove - '0').currHP > world.pc.pokemonTeam.at(switchMove - '0').hp)
            {
              world.pc.pokemonTeam.at(switchMove - '0').currHP = world.pc.pokemonTeam.at(switchMove - '0').hp;
            }
            commandInput = 'c';
          }
          else if(bagInput == "revive")
              {
                pkmnWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
                int counter = 12;
                int index = 0;
                mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "Select the number of the fainted Pokemon you want to revive");
                for(WildPokemon temp : world.pc.pokemonTeam)
                {
                  if(temp.currHP <= 0)
                  {
                    mvwprintw(pkmnWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
                  }
                  index++;
                }
                char switchMove = wgetch(pkmnWin);
                while(switchMove - '0' > index)
                {
                  mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "Invalid input! Choose a valid number!");
                  switchMove = wgetch(pkmnWin);
                }
                world.pc.pokemonTeam.at(switchMove - '0').currHP = 10;
                activePokemon.chosen_move = 3;
                activePokemon.next_turn_cost = 0;
                commandInput = 'c';
              }
        }
      }
            // opponent move
        int randMove = rand() % 2 + 1;
        if(randMove == 1)
        {
          trainerPokemon.chosen_fighting_move = trainerPokemon.learnedMoves[0];
          trainerPokemon.chosen_move = 5;
          trainerPokemon.next_turn_cost = 10 + trainerPokemon.learnedMoves[0].priority;
        }
        else
        {
          trainerPokemon.chosen_fighting_move = trainerPokemon.learnedMoves[1];
          trainerPokemon.chosen_move = 6;
          trainerPokemon.next_turn_cost = 10 + trainerPokemon.learnedMoves[1].priority;
        }
        heap_insert(&battleHeap, &trainerPokemon);
        commandInput = 'd'; // only here to enter while loop again if the battle continues
        
        //goes through heap
        counter = 0;
        while(counter < 2 || battle == false)
        {
          heapPokemon = (WildPokemon *) heap_remove_min(&battleHeap);
          if((*heapPokemon).capturedStatus == pc_owned)
          {
            if(activePokemon.currHP <= 0)
            {
              counter = 0;
              caughtWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
              mvwprintw(caughtWin, 1, getmaxx(stdscr) / 9, "Your %s has fainted", activePokemon.name.c_str());

              long unsigned index;
              for(index = 0; index < world.pc.pokemonTeam.size(); index++)
              {
                if(world.pc.pokemonTeam.at(index).currHP > 0)
                {
                  counter++;
                }
              }
              if(counter == 0)
              {
                mvwprintw(caughtWin, 18,  getmaxx(stdscr) / 9, "You have no more Pokemon left to battle with");
                mvwprintw(caughtWin, 20,  getmaxx(stdscr) / 9, "GAME OVER");
                mvwprintw(caughtWin, 22,  getmaxx(stdscr) / 9, "Press any key to exit the game");
                world.quit = 1;
                wrefresh(caughtWin);
                wgetch(caughtWin);
                battle = false;
                break;
              }
              else
              {
                bool condition = false;
                mvwprintw(caughtWin, 18,  getmaxx(stdscr) / 9, "Send out another Pokemon");
                int index = 0;
                int counter = 20;
                for(WildPokemon temp : world.pc.pokemonTeam)
                {
                  if(index != 0)
                  {
                    mvwprintw(caughtWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
                  }
                  index++;
                }
                mvwprintw(caughtWin, 20,  getmaxx(stdscr) / 10, "Select the number of the non-fainted pokemon you want to switch with");
                char switchMove = wgetch(caughtWin);
                while(condition == false)
                {
                  if(world.pc.pokemonTeam.at(switchMove - '0').currHP <= 0)
                  {
                    mvwprintw(caughtWin, 20,  getmaxx(stdscr) / 10, "This Pokemon has already fainted! Choose another!");
                    switchMove = wgetch(caughtWin);
                  }
                  else
                  {
                    std::iter_swap(world.pc.pokemonTeam.begin(), world.pc.pokemonTeam.begin() + (switchMove - '0'));
                    condition = true;
                  }
                }
              }
            }
            else if((*heapPokemon).chosen_move == 5 || (*heapPokemon).chosen_move == 6)
            {
                  //need to change damage later.
                  damage = 3;
                  trainerPokemon.currHP = trainerPokemon.currHP - damage;
                  if(trainerPokemon.currHP < 0)
                  {
                    trainerPokemon.currHP = 0;
                  }
                  wmove(win, 18, getmaxx(stdscr) / 9);
                  wclrtoeol(win);
                  mvwprintw(win, 10, getmaxx(stdscr) / 9, "The Trainer %s has taken damage!", trainerPokemon.name.c_str());
                  wrefresh(win);
                  if(trainerPokemon.currHP <= 0)
                  {
                    caughtWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
                    mvwprintw(caughtWin, 1, getmaxx(stdscr) / 9, "The Trainer %s has been defeated!", trainerPokemon.name.c_str());
                    mvwprintw(caughtWin, 18,  getmaxx(stdscr) / 9, "Press any key to continue.");
                    wrefresh(caughtWin);
                    wgetch(caughtWin);
                    battle = false;
                    break;
                  }
            }
            else if((*heapPokemon).chosen_move == 0)
            {
              wmove(win, 18, getmaxx(stdscr) / 9);
              wclrtoeol(win);
              mvwprintw(win, 18,  getmaxx(stdscr) / 9, "got away safely. Press any key to continue.");
              wrefresh(win);
              wgetch(win);
              battle = false;
              break;
            }
            else if((*heapPokemon).chosen_move == 4)
            {
              wclear(win);
              mvwprintw(win, 20,  getmaxx(stdscr) / 10, "Switched pokemon!");
              wrefresh(win);
              wgetch(win);
              wclear(win);
              activePokemon = world.pc.pokemonTeam.at(0);
            }
            else if((*heapPokemon).chosen_move == 2)
            {
              wclear(win);
              mvwprintw(win, 20,  getmaxx(stdscr) / 10, "Used a potion!");
              wrefresh(win);
              wgetch(win);
              wclear(win);
            }
            else if((*heapPokemon).chosen_move == 3)
            {
              wclear(win);
              mvwprintw(win, 20,  getmaxx(stdscr) / 10, "Used a revive!");
              wrefresh(win);
              wgetch(win);
              wclear(win);
            }
          }
          else
          {
            if((*heapPokemon).chosen_move == 5 || (*heapPokemon).chosen_move == 6)
            {
                  mvwprintw(win, 8, getmaxx(stdscr) / 9, "Trainer Pokemon used %s", trainerPokemon.chosen_fighting_move.identifier.c_str());
                  damage = 3;
                  world.pc.pokemonTeam.at(0).currHP = world.pc.pokemonTeam.at(0).currHP - damage;
                  if(world.pc.pokemonTeam.at(0).currHP < 0)
                  {
                    world.pc.pokemonTeam.at(0).currHP  = 0;
                  }
                  wrefresh(win);
                  activePokemon = world.pc.pokemonTeam.at(0);
            }
          }
          counter++;
        }
  }
  

  n->defeated = 1;
  if (n->ctype == char_hiker || n->ctype == char_rival) {
    n->mtype = move_wander;
  }
}
void give_pc_pokemon()
{
  std::vector<WildPokemon> starters;
  int i;
  WildPokemon p;
  int index;
  for(i = 0; i < 3; i++)
  {

    index = rand() % 1093;
    p.name = world.pokemon[index].identifier;
    p.species_id = world.pokemon[index].species_id;
    determinePokemonLevel(&p);
    determinePokemonMoves(&p);
    determinePokemonStats(&p, world.pokemon[index]);
    determineGenderAndShiny(&p);
    p.next_turn_cost = 0;
    p.capturedStatus = pc_owned;
    starters.push_back(p);
  }
  mvprintw(getmaxy(stdscr) /4 , getmaxx(stdscr) /3 , "Welcome to the game! Choose a starter!");
  mvprintw(getmaxy(stdscr) /4 + 2, getmaxx(stdscr) /3, "Option 1: %s", starters.at(0).name.c_str());
  mvprintw(getmaxy(stdscr) /4 + 3, getmaxx(stdscr) /3, "Option 2: %s", starters.at(1).name.c_str());
  mvprintw(getmaxy(stdscr) /4 + 4, getmaxx(stdscr) /3, "Option 3: %s", starters.at(2).name.c_str());
  refresh();
  char choice = getch();
  while(choice != '1' && choice != '2' && choice != '3')
  {
    mvprintw(getmaxy(stdscr) /4 + 8, getmaxx(stdscr) /3, "Invalid choice. Press 1, 2, or 3");
    refresh();
    choice = getch();
  }
  switch(choice)
  {
    case '1' :
      world.pc.pokemonTeam.push_back(starters.at(0));
      break;
    case '2' :
      world.pc.pokemonTeam.push_back(starters.at(1));
      break;
    case '3' :
      world.pc.pokemonTeam.push_back(starters.at(2));
      break;
  }
  mvprintw(getmaxy(stdscr) /4 + 6, getmaxx(stdscr) /3, "you chose %s", world.pc.pokemonTeam.at(0).name.c_str());
  mvprintw(getmaxy(stdscr) /4 + 8, getmaxx(stdscr) /3, "Press the enter key to start the game!");
  world.pc.numPokeBalls = 10;
  world.pc.numPotions = 10;
  world.pc.numRevives = 10;
  refresh();
  choice = getch();
  while(choice != '\n')
  {
    choice = getch();
  }
}
void give_npc_pokemon(character_t *c)
{
  WildPokemon p;
  int index = rand() % 1093;

  p.name = world.pokemon[index].identifier;
  p.species_id = world.pokemon[index].species_id;
  determinePokemonLevel(&p);
  determinePokemonMoves(&p);
  determinePokemonStats(&p, world.pokemon[index]);
  determineGenderAndShiny(&p);
  p.capturedStatus = npc_owned;
  refresh();
  (*c).pokemonTeam.push_back(p);
}
void determinePokemonLevel(WildPokemon *p)
{
  int minLevel, maxLevel;
  int distance = abs(200 - world.cur_idx[dim_y]) + abs(200 - world.cur_idx[dim_x]);
  if(distance <= 200)
  {
    minLevel = 1;
    maxLevel = distance / 2;
    if(maxLevel == 0)
    {
      maxLevel = 1;
    }
  }
  else
  {
    minLevel = (distance - 200) / 2;
    if(minLevel > 100)
    {
      minLevel = 100;
    }
    maxLevel = 100;
  }
  int pokemonLevel = rand() % (maxLevel - minLevel + 1) + minLevel;
  
  (*p).level = pokemonLevel;
}
void determinePokemonMoves(WildPokemon *p)
{
  int i;
  std::vector<PokemonMoves> possibleMoves;
  int length = sizeof(world.pokeMoves) / sizeof(PokemonMoves);
  for(i = 0; i < length; i++)
  {
    if(world.pokeMoves[i].pokemon_id == p->species_id && world.pokeMoves[i].pokemon_move_method_id == 1)
    {
      possibleMoves.push_back(world.pokeMoves[i]);
    }
  }
  //chooses random index from the list of possible moves to search for in moves
  int randomIndex1 = rand() % possibleMoves.size();
  int randomIndex2 = rand() % possibleMoves.size();
  std::vector<Moves> returnMoves;
  length = sizeof(world.moves) / sizeof(Moves);
  for(i = 0; i < length; i++)
  {
    if(world.moves[i].id == possibleMoves.at(randomIndex1).move_id)
    {
      returnMoves.push_back(world.moves[i]);
    }
    if(world.moves[i].id == possibleMoves.at(randomIndex2).move_id)
    {
      returnMoves.push_back(world.moves[i]);
    }
  }
  p->learnedMoves[0] = returnMoves.at(0);
  p->learnedMoves[1] = returnMoves.at(1);
}
void determinePokemonStats(WildPokemon *p, Pokemon pokemonTemp)
{
  std::vector<int> baseStats;
  int i;
  int length = sizeof(world.pokeStats) / sizeof(PokemonStats);
  for(i = 0; i < length; i++)
  {
    if(world.pokeStats[i].pokemon_id == p->species_id)
    {
      baseStats.push_back(world.pokeStats[i].base_stat);
    }
  }
  int IV = rand() % 16;
  int HP = floor((((baseStats.at(0) + IV) * 2) * (p->level))/100) + p->level + 10;

  IV = rand() % 16;
  int att = floor((((baseStats.at(1) + IV) * 2) * (p->level))/100) + 5;

  IV = rand() % 16;
  int def = floor((((baseStats.at(2) + IV) * 2) * (p->level))/100) + 5;

  IV = rand() % 16;
  int specialAtt= floor((((baseStats.at(3) + IV) * 2) * (p->level))/100) + 5;

  IV = rand() % 16;
  int specialDef = floor((((baseStats.at(4) + IV) * 2) * (p->level))/100) + 5;

  IV = rand() % 16;
  int speed = floor((((baseStats.at(5) + IV) * 2) * (p->level))/100) + 5;

  // IV = rand() % 16;
  // int accuracy = floor((((baseStats.at(6) + IV) * 2) * (p->level))/100) + 5;

  // IV = rand() % 16;
  // int evasion = floor((((baseStats.at(7) + IV) * 2) * (p->level))/100) + 5;

  (*p).hp = HP;
  (*p).attack = att;
  (*p).defense = def;
  (*p).special_attack = specialAtt;
  (*p).special_defense = specialDef;
  (*p).speed = speed;
  (*p).currHP = HP;
  // (*p).accuracy = accuracy;
  // (*p).evasion = evasion;
}
void determineGenderAndShiny(WildPokemon *p)
{
  std::string genderString;
  int gender = rand() % 2;
  if(gender == 0)
  {
    (*p).gender = "male";
  }
  else
  {
    (*p).gender = "female";
  }
  int shinyRate = rand() % 8192;
  if(shinyRate == 0)
  {
    (*p).shiny = true;
  }
  else
  {
    (*p).shiny = false;
  }
}

void io_pokemonEncounter()
{
  int counter;
  char battleOption;
  int damage;
  char move;
  WINDOW * pkmnWin;
  WINDOW * caughtWin;
  heap_t battleHeap;
  WINDOW * win = newwin(getmaxy(stdscr), getmaxx(stdscr), 0, 0);
  bool battle = true;
  WildPokemon p;
  WildPokemon activePokemon = world.pc.pokemonTeam.at(0);
  WildPokemon *heapPokemon;
  int index = rand() % 1093;
  p.name = world.pokemon[index].identifier;
  p.species_id = world.pokemon[index].species_id;
  determinePokemonLevel(&p);
  determinePokemonMoves(&p);
  determinePokemonStats(&p, world.pokemon[index]);
  determineGenderAndShiny(&p);
  p.next_turn_cost = 1;
  p.capturedStatus = wild_owned;
  

  heap_init(&battleHeap, pokemon_battle_cmp, NULL);

    while(battle)
    {
      wclear(win);
      activePokemon = world.pc.pokemonTeam.front();
      box(win, 0,0);
      wrefresh(win);
      //player move
        while(battleOption != 'c') // 'c' means the move is now confirmed. In place to allow for the player to choose another move if they already went to another screen for another move.
        {
          //Wild pokemon
          mvwprintw(win,1, 1, "Wild Pokemon encounter!");
          mvwprintw(win,3, 1, "Wild Pokemon: ");
          mvwprintw(win,3, getmaxx(stdscr) / 9, "%s", p.name.c_str());
          mvwprintw(win,4, getmaxx(stdscr) / 9, "HP: %d/%d", p.currHP, p.hp);
          
          //Player
          mvwprintw(win,12, 1, "Your Pokemon:");
          mvwprintw(win,12, getmaxx(stdscr) / 9, "%s", activePokemon.name.c_str());
          mvwprintw(win,13, getmaxx(stdscr) / 9, "HP: %d/%d", activePokemon.currHP, activePokemon.hp);

          mvwprintw(win,14, getmaxx(stdscr) / 10, "-------------------");
          mvwprintw(win,15, getmaxx(stdscr) / 9, "FIGHT   PKMN");
          mvwprintw(win,16, getmaxx(stdscr) / 9, "BAG     RUN");
          mvwprintw(win,17, getmaxx(stdscr) / 10, "-------------------");
          
          wrefresh(win);
          battleOption = wgetch(win);
            mvwprintw(win,0, 0, "%c", battleOption);
            wrefresh(win);
            while(battleOption != 'f' && battleOption != 'r' && battleOption != 'p' && battleOption != 'B')
            {
              mvwprintw(win,18, getmaxx(stdscr) / 9, "invalid option. press f to fight, B to look through bag, p to switch pokemon or r to escape");
              wrefresh(win);
              battleOption = wgetch(win);
              wrefresh(win);
            }
            if(battleOption == 'p')
            {
              pkmnWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
              mvwprintw(pkmnWin, 8,  getmaxx(stdscr) / 9, "Active Pokemon");
              mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "%s", world.pc.pokemonTeam.at(0).name.c_str());
              mvwprintw(pkmnWin, 12,  getmaxx(stdscr) / 9, "Other Pokemon");
              int counter = 12;
              int index = 0;
              for(WildPokemon temp : world.pc.pokemonTeam)
              {
                if(index != 0)
                {
                  mvwprintw(pkmnWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
                }
                index++;
              }
              mvwprintw(pkmnWin, 20,  getmaxx(stdscr) / 10, "Select the number of the Pokemon you want to switch with");
              char switchMove = wgetch(pkmnWin);
              activePokemon.chosen_move = 4;
              activePokemon.next_turn_cost = 0;
              heap_insert(&battleHeap, &activePokemon);
              std::iter_swap(world.pc.pokemonTeam.begin(), world.pc.pokemonTeam.begin() + (switchMove - '0'));
              battleOption = 'c';
              delwin(pkmnWin);
            }
            else if(battleOption == 'r')
            {
              activePokemon.chosen_move = 0;
              activePokemon.next_turn_cost = 0;
              heap_insert(&battleHeap, &activePokemon);
              battleOption = 'c';
            }
            else if(battleOption == 'f')
            {
              wmove(win, 18, getmaxx(stdscr) / 9);
              wclrtoeol(win);
              mvwprintw(win,18, getmaxx(stdscr) / 9, "move 1: %s   move 2: %s", world.pc.pokemonTeam.at(0).learnedMoves[0].identifier.c_str(), world.pc.pokemonTeam.at(0).learnedMoves[1].identifier.c_str());
              wrefresh(win);
              move = wgetch(win);
              if(move == '1')
              {
                activePokemon.chosen_fighting_move = activePokemon.learnedMoves[0];
                activePokemon.chosen_move = 5;
                activePokemon.next_turn_cost = 2;
              }
              else if(move == '2')
              {
                activePokemon.chosen_fighting_move = activePokemon.learnedMoves[1];
                activePokemon.chosen_move = 6;
                activePokemon.next_turn_cost = 2;
              }
              heap_insert(&battleHeap, &activePokemon);
              battleOption = 'c';
            }
            else if(battleOption == 'B')
            {
              world.pc.numPokeBalls = 1;
              wmove(win, 18, getmaxx(stdscr) / 9);
              wclrtoeol(win);
              wrefresh(win);
              std::string bagInput = io_pc_bag(p);
              if(bagInput == "caught")
              {
                if(world.pc.pokemonTeam.size() > 5)
                {
                  caughtWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
                  box(caughtWin, 0, 0);
                  mvwprintw(caughtWin, 1, getmaxx(stdscr) / 9, "You already have 6 Pokemon!");
                  mvwprintw(caughtWin, 5, getmaxx(stdscr) / 9, "Choose another option!");
                  wrefresh(caughtWin);
                  wgetch(caughtWin);
                  delwin(caughtWin);
                }
                else
                {
                  activePokemon.chosen_move = 1;
                  heap_insert(&battleHeap, &activePokemon);
                  battleOption = 'c';
                }
              }
              else if(bagInput == "potion")
              {
                pkmnWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
                int counter = 12;
                int index = 0;
                for(WildPokemon temp : world.pc.pokemonTeam)
                {
                  mvwprintw(pkmnWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
                  index++;
                }
                char switchMove = wgetch(pkmnWin);

                while(switchMove - '0' > index)
                {
                  mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "Invalid input! Select the number (1-6) of the Pokemon you want to heal");
                  switchMove = wgetch(pkmnWin);
                }   
                battleOption = 'c';
                activePokemon.chosen_move = 2;
                activePokemon.next_turn_cost = 0;
                heap_insert(&battleHeap, &activePokemon);
                world.pc.pokemonTeam.at(switchMove - '0').currHP += 20;
                if(world.pc.pokemonTeam.at(switchMove - '0').currHP > world.pc.pokemonTeam.at(switchMove - '0').hp)
                {
                  world.pc.pokemonTeam.at(switchMove - '0').currHP = world.pc.pokemonTeam.at(switchMove - '0').hp;
                }
              }
              else if(bagInput == "revive")
              {
                pkmnWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
                int counter = 12;
                int index = 0;
                mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "Select the number of the fainted Pokemon you want to revive");
                for(WildPokemon temp : world.pc.pokemonTeam)
                {
                  if(temp.currHP <= 0)
                  {
                    mvwprintw(pkmnWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
                  }
                  index++;
                }
                char switchMove = wgetch(pkmnWin);
                while(switchMove - '0' > index)
                {
                  mvwprintw(pkmnWin, 10,  getmaxx(stdscr) / 10, "Invalid input! Choose a valid number!");
                  switchMove = wgetch(pkmnWin);
                }
                world.pc.pokemonTeam.at(switchMove - '0').currHP = 10;
                activePokemon.chosen_move = 3;
                activePokemon.next_turn_cost = 0;
                battleOption = 'c';
              }
            }
        }
      // opponent move
        int randMove = rand() % 2 + 1;
        if(randMove == 1)
        {
          p.chosen_fighting_move = p.learnedMoves[0];
          p.chosen_move = 5;
          p.next_turn_cost = 10 + p.learnedMoves[0].priority;
        }
        else
        {
          p.chosen_fighting_move = p.learnedMoves[1];
          p.chosen_move = 6;
          p.next_turn_cost = 10 + p.learnedMoves[1].priority;
        }
        heap_insert(&battleHeap, &p);
        battleOption = 'd'; // only here to enter while loop again if the battle continues
        
        //goes through heap
        counter = 0;
        while(counter < 2 || battle == false)
        {
          heapPokemon = (WildPokemon *) heap_remove_min(&battleHeap);
          if((*heapPokemon).capturedStatus == pc_owned)
          {
            if(activePokemon.currHP <= 0)
            {
              counter = 0;
              caughtWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
              mvwprintw(caughtWin, 1, getmaxx(stdscr) / 9, "Your %s has fainted", activePokemon.name.c_str());

              long unsigned index;
              for(index = 0; index < world.pc.pokemonTeam.size(); index++)
              {
                if(world.pc.pokemonTeam.at(index).currHP > 0)
                {
                  counter++;
                }
              }
              if(counter == 0)
              {
                mvwprintw(caughtWin, 18,  getmaxx(stdscr) / 9, "You have no more Pokemon left to battle with");
                mvwprintw(caughtWin, 20,  getmaxx(stdscr) / 9, "GAME OVER");
                mvwprintw(caughtWin, 22,  getmaxx(stdscr) / 9, "Press any key to exit the game");
                world.quit = 1;
                wrefresh(caughtWin);
                wgetch(caughtWin);
                battle = false;
                break;
              }
              else
              {
                bool condition = false;
                mvwprintw(caughtWin, 18,  getmaxx(stdscr) / 9, "Send out another Pokemon");
                int index = 0;
                int counter = 20;
                for(WildPokemon temp : world.pc.pokemonTeam)
                {
                  if(index != 0)
                  {
                    mvwprintw(caughtWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
                  }
                  index++;
                }
                mvwprintw(caughtWin, 20,  getmaxx(stdscr) / 10, "Select the number of the non-fainted Pokemon you want to switch with");
                char switchMove = wgetch(caughtWin);
                while(condition == false)
                {
                  if(world.pc.pokemonTeam.at(switchMove - '0').currHP <= 0)
                  {
                    mvwprintw(caughtWin, 20,  getmaxx(stdscr) / 10, "This Pokemon has already fainted! Choose another!");
                    switchMove = wgetch(caughtWin);
                  }
                  else
                  {
                    std::iter_swap(world.pc.pokemonTeam.begin(), world.pc.pokemonTeam.begin() + (switchMove - '0'));
                    condition = true;
                  }
                }
              }
            }
            else if((*heapPokemon).chosen_move == 2)
            {
              wclear(win);
              mvwprintw(win, 20,  getmaxx(stdscr) / 10, "Used a potion!");
              wrefresh(win);
              wgetch(win);
              wclear(win);
            }
            else if((*heapPokemon).chosen_move == 3)
            {
              wclear(win);
              mvwprintw(win, 20,  getmaxx(stdscr) / 10, "Used a revive!");
              wrefresh(win);
              wgetch(win);
              wclear(win);
            }
            else if((*heapPokemon).chosen_move == 5 || (*heapPokemon).chosen_move == 6)
            {
                  //need to change damage later.
                  damage = 3;
                  p.currHP = p.currHP - damage;
                  if(p.currHP < 0)
                  {
                    p.currHP = 0;
                  }
                  wmove(win, 18, getmaxx(stdscr) / 9);
                  wclrtoeol(win);
                  mvwprintw(win, 10, getmaxx(stdscr) / 9, "The wild %s has taken damage!", p.name.c_str());
                  wrefresh(win);
                  if(p.currHP <= 0)
                  {
                    caughtWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
                    mvwprintw(caughtWin, 1, getmaxx(stdscr) / 9, "The wild %s has been defeated!", p.name.c_str());
                    mvwprintw(caughtWin, 18,  getmaxx(stdscr) / 9, "Press any key to continue.");
                    wrefresh(caughtWin);
                    wgetch(caughtWin);
                    battle = false;
                    break;
                  }
            }
            else if((*heapPokemon).chosen_move == 0)
            {
              wmove(win, 18, getmaxx(stdscr) / 9);
              wclrtoeol(win);
              mvwprintw(win, 18,  getmaxx(stdscr) / 9, "got away safely. Press any key to continue.");
              wrefresh(win);
              wgetch(win);
              battle = false;
              break;
            }
            else if((*heapPokemon).chosen_move == 1)
            {
              p.capturedStatus = pc_owned;
              world.pc.pokemonTeam.push_back(p);
              caughtWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
              box(caughtWin, 0, 0);
              mvwprintw(caughtWin, 1, getmaxx(stdscr) / 9, "Congrats! You have caught a %s", p.name.c_str());
              mvwprintw(caughtWin, 3, getmaxx(stdscr) / 9, "pokemon:%s, gender:%s, level:%d, move 1:%s, move 2:%s, hp:%d atk:%d def:%d sp-atk:%d sp-def:%d speed:%d shiny:%s", p.name.c_str(), p.gender.c_str(), p.level, p.learnedMoves[0].identifier.c_str(),p.learnedMoves[1].identifier.c_str(), p.hp, p.attack, p.defense, p.special_attack, p.special_defense, p.speed, p.shiny ? "true" : "false");
              mvwprintw(caughtWin, 5, getmaxx(stdscr) / 9, "Press any button to continue");
              wrefresh(caughtWin);
              wgetch(caughtWin);
              delwin(caughtWin);
              battle = false;
              break;
            }
            else if((*heapPokemon).chosen_move == 4)
            {
              wclear(win);
              mvwprintw(win, 20,  getmaxx(stdscr) / 10, "Switched pokemon!");
              wrefresh(win);
              wgetch(win);
              wclear(win);
              activePokemon = world.pc.pokemonTeam.at(0);
            }
          }
          else
          {
            if((*heapPokemon).chosen_move == 5 || (*heapPokemon).chosen_move == 6)
            {
                  mvwprintw(win, 8, getmaxx(stdscr) / 9, "Wild Pokemon used %s", p.chosen_fighting_move.identifier.c_str());
                  damage = 3;
                  world.pc.pokemonTeam.at(0).currHP = world.pc.pokemonTeam.at(0).currHP - damage;
                  if(world.pc.pokemonTeam.at(0).currHP < 0)
                  {
                    world.pc.pokemonTeam.at(0).currHP = 0;
                  }
                  wrefresh(win);
                  activePokemon = world.pc.pokemonTeam.at(0);
            }
          }
          counter++;
        }
        
    }
}
uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (dynamic_cast<npc *>(world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((npc *) world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if (dynamic_cast<npc *>
               (world.cur_map->cmap[dest[dim_y]][dest[dim_x]])) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  // Nathan Note: Encountering pokemon should probably go here.
  else if(world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_grass)
  {
    int spawnChance = rand() % 4 + 1;

    if(spawnChance == 1)
    {
      io_pokemonEncounter();
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      INT_MAX) {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  /* mvscanw documentation is unclear about return values.  I believe *
   * that the return value works the same way as scanf, but instead   *
   * of counting on that, we'll initialize x and y to out of bounds   *
   * values and accept their updates only if in range.                */
  int x = INT_MAX, y = INT_MAX;
  
  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  echo();
  curs_set(1);
  do {
    mvprintw(0, 0, "Enter x [-200, 200]:           ");
    refresh();
    mvscanw(0, 21, "%d", &x);
  } while (x < -200 || x > 200);
  do {
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, "%d", &y);
  } while (y < -200 || y > 200);

  refresh();
  noecho();
  curs_set(0);

  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void bag_potion()
{
  WINDOW * pokeWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
  box(pokeWin, 0, 0);
  int counter = 12;
  int index = 0;
  for(WildPokemon temp : world.pc.pokemonTeam)
  {
    mvwprintw(pokeWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
    index++;
  }
  mvwprintw(pokeWin, 20,  getmaxx(stdscr) / 10, "Select the number of the pokemon you want to restore hp to");
  char switchMove = wgetch(pokeWin);
  while(switchMove - '0' > index)
  {
    mvwprintw(pokeWin, 10,  getmaxx(stdscr) / 10, "Invalid input! Choose a valid number!");
    switchMove = wgetch(pokeWin);
  }
  if(world.pc.numPotions <= 0)
  {
    mvwprintw(pokeWin, 10,  getmaxx(stdscr) / 10, "No Potions left! Go to PokeMart for more!");
    wgetch(pokeWin);
    delwin(pokeWin);
  }
  else
  {
    world.pc.numPotions--;
    world.pc.pokemonTeam.at(switchMove - '0').currHP += 20;
    if(world.pc.pokemonTeam.at(switchMove - '0').currHP > world.pc.pokemonTeam.at(switchMove - '0').hp)
    {
      world.pc.pokemonTeam.at(switchMove - '0').currHP = world.pc.pokemonTeam.at(switchMove - '0').hp;
    }
    wclear(pokeWin);
    box(pokeWin, 0, 0);
    mvwprintw(pokeWin, 10,  getmaxx(stdscr) / 10, "Pokemon health restored!");
    wgetch(pokeWin);
    delwin(pokeWin);
  }
}
void bag_revive()
{
  WINDOW * pokeWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
  box(pokeWin, 0, 0);
  int counter = 12;
  int index = 0;
  for(WildPokemon temp : world.pc.pokemonTeam)
  {
    mvwprintw(pokeWin, ++counter,  getmaxx(stdscr) / 10, "%d: %s curr hp: %d/%d", index, temp.name.c_str(), temp.currHP, temp.hp);
    index++;
  }
  mvwprintw(pokeWin, 20,  getmaxx(stdscr) / 10, "Select the number of the pokemon you want to restore hp to");
  char switchMove = wgetch(pokeWin);
  while(switchMove - '0' > index)
  {
    mvwprintw(pokeWin, 10,  getmaxx(stdscr) / 10, "Invalid input! Choose a valid number!");
    switchMove = wgetch(pokeWin);
  }
  if(world.pc.numRevives <= 0)
  {
    mvwprintw(pokeWin, 10,  getmaxx(stdscr) / 10, "No Revives left! Go to PokeMart for more!");
    wgetch(pokeWin);
    delwin(pokeWin);
  }
  else
  {
    world.pc.numRevives--;
    world.pc.pokemonTeam.at(switchMove - '0').currHP += 10;
    if(world.pc.pokemonTeam.at(switchMove - '0').currHP > world.pc.pokemonTeam.at(switchMove - '0').hp)
    {
      world.pc.pokemonTeam.at(switchMove - '0').currHP = world.pc.pokemonTeam.at(switchMove - '0').hp;
    }
    wclear(pokeWin);
    box(pokeWin, 0, 0);
    mvwprintw(pokeWin, 10,  getmaxx(stdscr) / 10, "Pokemon revived!");
    wgetch(pokeWin);
    delwin(pokeWin);
  }
  
}
void io_handle_input(pair_t dest)
{
  char bagChoice;
  uint32_t turn_not_consumed;
  int key;
  WINDOW * bagWin;
  do {
    switch (key = getch()) {
    case 'B' :
      bagWin = newwin(getmaxy(stdscr), getmaxx(stdscr),0, 0);
      box(bagWin, 0, 0);
      mvwprintw(bagWin, 1, 20, "BAG");
      mvwprintw(bagWin, 6, 1, "1: %dx Potions", world.pc.numPotions);
      mvwprintw(bagWin, 7, 1, "2: %dx Revives", world.pc.numRevives);
      mvwprintw(bagWin, 15, 5, "Enter 'backspace' to leave bag");
      wrefresh(bagWin);
      bagChoice = wgetch(bagWin);
      while(bagChoice != '1' && bagChoice != '2' && bagChoice != 127)
      {
        mvwprintw(bagWin, 20, 5, "Invalid Input! Valid inputs are 1, 2 and 'backspace' ");
        bagChoice = wgetch(bagWin);
      }
      if(bagChoice == '1')
      {
        bag_potion();
        io_display();
      }
      else if(bagChoice == '2')
      {
        bag_revive();
        io_display();
      }
      else
      {
        io_display();
      }
      break;
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.               */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
    case 'm':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'f':
      /* Fly to any map in the world.                                */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}
