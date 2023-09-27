//libraries used in the project
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
//definition used to clean the terminal
#define clr system("clear")
//enumeration type to specify the role/state of the players
typedef enum Player_State{
  astronaut = 1,
  impostor = 2,
  murdered = 3,
  defenestrated = 4
}Player_State;
//enumeration type to identify all the players with colors
typedef enum Player_Name{
  red = 1,
  blue = 2,
  orange = 3,
  yellow = 4,
  purple = 5,
  green = 6,
  black = 7,
  white = 8,
  grey = 9,
  brown = 10
}Player_Name;
//enumeration type to specify the type of room or the things the players can find inside
typedef enum Room_Type{
  empty = 1,
  simple_quest = 2,
  complex_quest = 3,
  trap_door = 4
}Room_Type;
//struct definition which represent all the rooms generated while playing
typedef struct Room{
  struct Room* forward;
  struct Room* left;
  struct Room* right;
  struct Room* previous;
  struct Room* pNext;
  Room_Type Room_Type;
  bool emergency_call;
}Room;
//struct definition for all the players
typedef struct Player{
  Room* actual_position;
  Player_State Player_State;
  Player_Name Player_Name;
}Player;
//functions declaration of the gamelib.c
void set_game();
void play();
void end_game();
