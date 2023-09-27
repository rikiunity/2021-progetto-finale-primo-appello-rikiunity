#include "gamelib.h"
//declaration of global variables used by the functions
static Player *players;//pointer to array which represent all players
static unsigned short quest_to_finish = 0;
static Room *start_room = NULL;//spawn room of all players
static Room *room_list = NULL;//pointer to the start room and contain the list of all the rooms that will be generated
static Room *room_list_last = NULL;//pointer to the last room created while playing
static int players_number = 0, impostors_number = 0, impostors1 = 0, impostors2 = 0, impostors3 = 0, game_in_progress = 1, turn_temp = 0, quest_check = 0, sabotage_check = 0, emergency_check = 0;
static int trap_count = 0, trap_check = 0, settings_check = 0, kill_check = 0, astronauts_number = 0, play_again_check = 0, room_count = 1;//room_count is set 1 because the start_room is alredy included
static time_t t;

//functions declaration, the only functions that can be called up from the main are those defined in the gamelib.h file, the others are static
static void print_players();//933 this function is used in the set game and print all the players
static char *print_color_players(Player_Name Player_name);//945 this function return the colors of all the players passing as parameter the enum type
static char *print_state_players(Player_State Player_state);//984 this function is similar to the previous one but this return the states of the players
static char *print_room_type(Room_Type Room_Type);//1005 same as above, but for the types of the rooms
static void room_tail_insertion(int n);//696 this function is used in the progress function passing as parameter the choice of the player (1 forward, 2 left, 3 right)
static void random_assign_color();//782 this function randomize all the colors in the enum and assign different colors to each player
static void random_assign_impostors();//804 this function randomize the number of impostor with a specific probability and generate different numbers based on the players number and change state to the corresponding players
static void free_list(Room *head);//1027 this function free the room list flowing all the list with temp storing variable
static int random_room_type();//917 this function is used in the room_tail_insertion for the assigning of the room type, following a probability, to the new rooms created returning the corresponding value in the enum
static void quest_assign();//903 this function is used in the set game and just ask the number of quest that the astronauts need to complete in order to win
static void start_game();//690 used in the set game, just ask to press enter and back to menu for start the game
static void progress();//346 this function is used in the play function and print a menu of possible direction the player can take while playing
static void do_quest();//370 this function is used in the play function and make possible to complete the quest if present in the room
static void emergency_call_f();//404 this function is used in the play function and defenestrate a random player following a probability
static void kill_astronaut();//505 this function is used by the impostors in the play function and print a menu with the possible victims
static void use_trap_door();//600 this function is used by the impostors in the play function and change the position to another random room
static void sabotage();//671 this function is used by the impostors in the play function and change the type of room to empty in case there are simple or complex quest

void set_game(){
  //in case the game has been already set, the settings check variable is set to 1 at the end of the set game function
  if(settings_check == 1){
    printf("\nDeallocating memory...\n");
    sleep(2);
    clr;
    free(players);
    free_list(room_list);
    quest_to_finish = 0;
    players_number = 0;
    impostors_number = 0;
    impostors1 = 0;
    impostors2 = 0;
    impostors3 = 0;
    game_in_progress = 1;
    turn_temp = 0;
    quest_check = 0;
    sabotage_check = 0;
    emergency_check = 0;
    trap_count = 0;
    trap_check = 0;
    settings_check = 0;
    kill_check = 0;
    astronauts_number = 0;
    play_again_check = 0;
    room_count = 1;
  }
  srand((unsigned) time(&t));
  clr;

  printf("|***************************************SETTINGS*****************************************|\n");
  //menu for the input of the number of players
  do{
    sleep(1);
    printf("\nInsert the number of players [4-10] --> ");
    scanf("%d", &players_number);
    if(players_number < 4 || players_number > 10){
      sleep(1);
      printf("\nNumber out of range [4-10]\n");
    }
  }while(players_number < 4 || players_number > 10);

  sleep(1);

  //first dinamic allocation of the structs based on the number of players
  players = (Player *) malloc(players_number * sizeof(Player));

  //calling the functions that set the game
  random_assign_color();
  random_assign_impostors();
  quest_assign();
  clr;

  //first room allocation and start_room initialization
  start_room = (Room *) malloc(sizeof(Room));

  //set the position of all players to the first room which is empty
  for(int i = 0; i < players_number; i++){
    players[i].actual_position = start_room;
  }

  //pointers, enum and emergency call inizialization
  start_room -> forward = NULL;
  start_room -> right = NULL;
  start_room -> left = NULL;
  start_room -> pNext = NULL;
  start_room -> previous = NULL;
  start_room -> Room_Type = 1;
  start_room -> emergency_call = false;

  //the room_list and the room_list_last intially points to the start_room
  room_list = start_room;
  room_list_last = start_room;

  int choice_menu = 0;
  //menu with the possible functions that could be called here
  do{
    printf("|***********************************SETTINGS COMPLETED*************************************|\n");
    sleep(1);
    printf("\n        What now?\n");
    printf("\n1) - Print the players");
    printf("\n2) -  Start a game\n");
    sleep(1);
    printf("\nChoice --> ");
    scanf("%d", &choice_menu);

    switch(choice_menu){
      case 1:
        print_players();
        break;
      case 2:
        start_game();
        break;
      default:
        printf("\nWrong command. Retry...\n");
        break;
    }
    while(getchar() != '\n');
  }while(choice_menu != 1 && choice_menu != 2);
  //settings_check to 1 in case a new game is set
  settings_check = 1;
}

void play(){
  //in case the users already played a game, play_again_check is set 1 at the end of the do while in this function
  if(play_again_check == 1){
    printf("\nBefore starting a new game, you need to set it up again\n");
    sleep(2);
  }
  else{
    clr;
    printf("\nRandomizing turns...\n");

    int action_choice = 0;
    int turn[players_number];
    int cont = 0, temp_rand = 0, temp = 0;
    //randomize the turns, almost similiar to the function of the assigning of colors
    for (int i = 0; i < players_number; i++){
      turn[i] = i;
    }
    //shuffle algorithm
    for(int c = players_number - 1; c > 0; c--){
      temp_rand = rand() % c; //pop random number for the index of the array

      //swapping
      temp = turn[c];
      turn[c] = turn[temp_rand];
      turn[temp_rand] = temp;
    }

    sleep(2);

    do{
      clr;
      printf("|***************************************GAME*****************************************|\n");
      //check if the cont variable is equals to players_number, so the turn randomly choose others players
      if(cont == players_number){
        for (int i = 0; i < players_number; i++){
          turn[i] = i;
        }
        for(int c = players_number - 1; c > 0; c--){
          temp_rand = rand() % c;

          temp = turn[c];
          turn[c] = turn[temp_rand];
          turn[temp_rand] = temp;
        }
        cont = 0;
      }
      turn_temp = turn[cont];

      //check if the player is murdered or defenestrated
      if(players[turn_temp].Player_State == 3){
        //printf("\nThe %s astronaut has been killed\n",print_color_players(players[turn_temp].Player_Name));
        //first incrementing of cont for the flowing of the turns
        cont++;
      }else if(players[turn_temp].Player_State == 4){
        //printf("\nThe %s has been defenestrated\n",print_color_players(players[turn_temp].Player_Name));
        cont++;
      }else{
        //check if the randomized number is equal to one of the impostor
        if(players[turn_temp].Player_State == 2){
          cont++;
          sleep(1);
          printf("\nTurn of the %s player\n",print_color_players(players[turn_temp].Player_Name));
          sleep(1);
          printf("\nImpostor's Turn\n");
          printf("Type Room: %s\n",print_room_type(players[turn_temp].actual_position -> Room_Type));
          printf("Remaining quest --> %hu",quest_to_finish);

          //check if there are others players in the same room flowing all the array of players
          printf("\nPlayer's in the room: ");
          for(int i = 0; i < players_number; i++){
            if(players[turn_temp].actual_position == players[i].actual_position && i != turn_temp){
              if(players[i].Player_State == 3){
                printf("%s(murdered) ",print_color_players(players[i].Player_Name));
              }else{
                printf("%s ",print_color_players(players[i].Player_Name));
              }
            }
          }
          printf("\n");

          //variables used in the static functions in case a player can't use that function, so it is again his turn, checking it in the while condition, same as astronaut
          sabotage_check = 0;
          kill_check = 0;
          trap_check = 0;
          emergency_check = 0;

          do{
            printf("\n1) Progress\n");
            printf("2) Emergency Call\n");
            printf("3) Kill Astronaut\n");
            printf("4) Use Trap Door\n");
            printf("5) Sabotage\n");
            printf("\nChoice: ");
            scanf("%d",&action_choice);
            if(action_choice < 1 || action_choice > 5){
              printf("Wrong Value. Retry...\n");
            }else if(action_choice == 1){
              progress();
              //setting the variables to 1 so the turn can go on, same as others variables for the various function
              sabotage_check = 1;
              emergency_check = 1;
              trap_check = 1;
              kill_check = 1;
            }else if(action_choice == 2){
              emergency_call_f();
              sabotage_check = 1;
              trap_check = 1;
              kill_check = 1;
            }else if(action_choice == 3){
              kill_astronaut();
              sabotage_check = 1;
              emergency_check = 1;
              trap_check = 1;
            }else if(action_choice == 4){
              use_trap_door();
              sabotage_check = 1;
              emergency_check = 1;
              kill_check = 1;
            }else{
              sabotage();
              emergency_check = 1;
              trap_check = 1;
              kill_check = 1;
            }
          }while((action_choice != 1 && action_choice != 2 && action_choice != 3 && action_choice != 4 && action_choice != 5) || sabotage_check == 0 || emergency_check == 0 || trap_check == 0 || kill_check == 0);
          clr;
        }else{
          //same as impostors but with less possible choice
          cont++;
          sleep(1);
          printf("\nTurn of the %s player\n",print_color_players(players[turn_temp].Player_Name));
          sleep(1);
          printf("\nAstronaut's Turn\n");
          printf("Type Room: %s\n",print_room_type(players[turn_temp].actual_position -> Room_Type));
          printf("Remaining quest --> %hu",quest_to_finish);

          printf("\nPlayer's in the room:");
          for(int i = 0; i < players_number; i++){
            if((players[turn_temp].actual_position == players[i].actual_position) && i != turn_temp){
              if(players[i].Player_State == 3){
                printf("%s(murdered) ",print_color_players(players[i].Player_Name));
              }else{
                printf("%s ",print_color_players(players[i].Player_Name));
              }
            }
          }

          printf("\n");

          quest_check = 0;
          emergency_check = 0;

          do{
            printf("\n1) Progress\n");
            printf("2) Do quest\n");
            printf("3) Emergency Call\n");
            printf("\nChoice: ");
            scanf("%d",&action_choice);
            if(action_choice < 1 || action_choice > 3){
              printf("Wrong Value. Retry...\n");
            }else if(action_choice == 1){
              progress();
              quest_check = 1;
              emergency_check = 1;
            }else if(action_choice == 2){
              do_quest();
              emergency_check = 1;
            }else{
              emergency_call_f();
              quest_check = 1;
            }
          }while((action_choice != 1 && action_choice != 2 && action_choice != 3) || quest_check == 0 || emergency_check == 0);
        }
        clr;
      }
      //check if there are no more impostors. The impostors_number is decremented in case an impostor is defenestrate
      if(impostors_number == 0){
        printf("\nThere are no more impostors, so the astronauts won!\n");
        printf("\nWait...\n");
        sleep(5);
        //the game end checking it in the play function
        game_in_progress = 0;
      }
      //the variable impostors_number or astronauts_number is decremented in case of murdering or defenestration
      if(impostors_number > astronauts_number){
        printf("\nImpostors won the game!\n");
        printf("\nWait...\n");
        sleep(5);
        game_in_progress = 0;
      }
    }while(game_in_progress);
    play_again_check = 1;
    clr;
  }
}

//function that just deallocate all memory used during the game and close the game. Instead of the settings check in the set game function here there is no need to reinitialize all the global variables because the process ends
void end_game(){
  clr;
  if(settings_check == 1){
    printf("\nDeallocating memory...\n");
    sleep(2);
    free(players);
    free_list(room_list);
    clr;
    sleep(1);
    printf("\n                         Thanks for have played my game!\n");
    printf("\n                                   Have a nice day.     ");
    sleep(2);
  }
}

//function that make the player move through the rooms with 4 possible choice
static void progress(){
  int choice = 0;
  do{
    printf("\nPossible choices: \n");
    printf("\n1) Forward\n");
    printf("2) Left\n");
    printf("3) Right\n");
    printf("4) No Move\n");
    printf("\n Choice: ");
    scanf("%d", &choice);
    if(choice < 1 || choice > 4){
      printf("\nWrong value. Retry..\n");
    }
  }while(choice < 1 || choice > 4);

  if(choice == 4){
    printf("\nNo move registered\n");
    sleep(2);
  }else{
    room_tail_insertion(choice);
  }
}

//function that can be called up only from astronaut. Just set the room type to empty in case the room is simple quest or complex
static void do_quest(){
  if(players[turn_temp].actual_position -> Room_Type == 2){
    printf("\nSimple quest completed\n");
    sleep(2);
    //decrementing of quest_to_finish
    quest_to_finish--;
    //setting quest_check to 1, one of the variables explained before in the play function, so in this case the turn go to the next player
    quest_check = 1;
    players[turn_temp].actual_position -> Room_Type = 1;
  }else if(players[turn_temp].actual_position -> Room_Type == 3){
    printf("\nComplex quest completed\n");
    sleep(2);
    quest_to_finish -= 2;
    quest_check = 1;
    players[turn_temp].actual_position -> Room_Type = 1;
  }else{
    //in case there are no quest the player repeat the turn
    printf("\nThe room doesn't have quest\n");
    sleep(2);
    quest_check = 0;
  }
  //one of the winning condition of astronauts is to end quest, if there is only 1 quest remaining and an astronaut complete a complex quest the unsigned short print return 65535 which is close to the limit of it
  if(quest_to_finish == 0 || quest_to_finish == 65535){
    clr;
    printf("\nAstronauts completed all the quests, so they won the game!\n");
    printf("\nWait...\n");
    sleep(5);
    quest_check = 1;
    //the game end checking it in the play function
    game_in_progress = 0;
  }
}

//this function can be called only if the room didn't call another emergency_call
static void emergency_call_f(){
  int c = 0;
  //check if there are murdered players in the room and if the room had already called an emergency
  if(players[turn_temp].actual_position -> emergency_call == false){
    for(int i = 0; i < players_number; i++){
      if(players[i].actual_position == players[turn_temp].actual_position){
        if(players[i].Player_State == 3){
          c = 1;
        }
      }
    }
    //if there are murdered players
    if(c == 1){
      emergency_check = 1;
      //set the emergency_call of the room to true
      players[turn_temp].actual_position -> emergency_call = true;
      printf("\nMurdered astronaut found\n");
      sleep(2);

      int temp_astr = 0;
      int temp_impos = 0;
      int prob_impostor = 30;
      int prob_astronaut = 30;
      int temp_prob = 0;

      //check how many others astronauts or impostors are in there
      for(int i = 0; i < players_number; i++){
        if(players[i].actual_position == players[turn_temp].actual_position && i != turn_temp){
          if(players[i].Player_State == 1){
            temp_astr += 1;
          }else if(players[i].Player_State == 2){
            temp_impos += 1;
          }
        }
      }

      //calculate all probabilities
      prob_impostor = (prob_impostor) + (20 * temp_astr) - (30 * temp_impos);
      if(prob_impostor >= 100){
        prob_impostor = 100;
      }
      if(prob_impostor < 0){
        prob_impostor = 0;
      }
      printf("\n%d percentage impostor can be defenestrate\n",prob_impostor);

      prob_astronaut = (prob_astronaut) + (20 * temp_impos) - (30 * temp_astr);
      if(prob_astronaut >= 100){
        prob_astronaut = 100;
      }
      if(prob_astronaut < 0){
        prob_astronaut = 0;
      }
      printf("\n%d percentage astronaut can be defenestrate\n",prob_astronaut);
      sleep(2);

      //randomize a random player in the room to defenestrate
      for(;;){
        temp_prob = (rand () % players_number);
        if((players[temp_prob].actual_position == players[turn_temp].actual_position) && (players[temp_prob].Player_State == 1 || players[temp_prob].Player_State == 2)){
          break;
        }
      }
      int prob = ((rand() % 100) + 1);
      //check if the random player is impostor or astronaut
      if(players[temp_prob].Player_State == 2){
        if(prob <= prob_impostor){
          printf("\nThe %s impostor has been defenestrated\n",print_color_players(players[temp_prob].Player_Name));
          sleep(2);
          players[temp_prob].Player_State = 4;
          players[temp_prob].actual_position = NULL;
          impostors_number--;
        }else{
          printf("\nNobody has been defenestrate\n");
          sleep(2);
        }
      }else if(players[temp_prob].Player_State == 1){
        if(prob <= prob_astronaut){
          printf("\nThe %s astronaut has been defenestrated\n",print_color_players(players[temp_prob].Player_Name));
          astronauts_number--;
          sleep(2);
          players[temp_prob].Player_State = 4;
          players[temp_prob].actual_position = NULL;
      }else{
        printf("\nNobody has been defenestrate\n");
        sleep(2);
      }
    }
  }else{
      printf("\nNone murdered astronauts found.\n");
      sleep(2);
      //in this case the player repeat the turn, also below
      emergency_check = 0;
    }
  }else{
    printf("\nThis room have already called an emergency call\n");
    sleep(2);
    emergency_check = 0;
  }
}

static void kill_astronaut(){
  int c = 0;
  //check if there are astronauts in the room
  for(int i = 0; i < players_number; i++){
    if(players[turn_temp].actual_position == players[i].actual_position && i != turn_temp){
      if(players[i].Player_State == 1){
        c = 1;
      }
    }
  }
  //if there are not astronauts to kill
  if(c == 0){
    printf("\nNo astronauts detected in the room\n");
    //in this case the player repeat the turn
    kill_check = 0;
  }else{
    int kill_choice = 0;
    int temp = 0;
    printf("\nPossible victims: ");
    sleep(2);
    //print all possible victims in the room with a temp variable which is the maximum value that an impostor can choose
    for(int i = 0; i < players_number; i++){
      if(players[i].actual_position == players[turn_temp].actual_position && i != turn_temp){
        if(players[i].Player_State == 1){
          printf("%d --> %s ",temp,print_color_players(players[i].Player_Name));
          temp++;
        }
      }
    }
    printf("\n");

    //kill choice with all the checking and flow of a for to find the corresponding astronaut
    do{
      do{
        printf("\nChoice --> ");
        sleep(1);
        scanf("%d",&kill_choice);
        if(kill_choice < 0 || kill_choice > temp){
          printf("\nWrong value...Retry\n");
        }
      }while(kill_choice < 0 || kill_choice > temp);

      int counter = 0;
      for(int i = 0; i < players_number; i++){
        if(players[i].Player_State == 1 && (players[turn_temp].actual_position == players[i].actual_position)){
          if(kill_choice == counter){
            printf("\nThe %s astronaut has been killed\n",print_color_players(players[i].Player_Name));
            sleep(2);
            players[i].Player_State = 3;
            kill_check = 1;
            //decrementing the astronauts_number
            astronauts_number--;
            break;
          }else{
            counter++;
          }
        }
      }

      //randomize the probability of the impostor to be defenestrate
      if(kill_check == 1){
        int cont_astro_actual = 0;
        int cont_astro_previous = 0;
        //count how many astronauts are in the previous and actual room
        for(int i = 0; i < players_number; i++){
          if(players[turn_temp].actual_position == players[i].actual_position && i != turn_temp){
            if(players[i].Player_State == 1){
              cont_astro_actual++;
            }
          }else if((players[turn_temp].actual_position -> previous == players[i].actual_position) && (i != turn_temp) && players[i].Player_State == 1){
            cont_astro_previous++;
          }
        }
        int prob_defenes = 0;
        prob_defenes = (50 * cont_astro_actual) + (20 * cont_astro_previous);
        if(prob_defenes >= 100){
          prob_defenes = 100;
        }
        printf("\n%d percentage that %s impostor can be defenestrate\n",prob_defenes,print_color_players(players[turn_temp].Player_Name));
        sleep(2);
        int random_prob_defenes = 0;
        random_prob_defenes = ((rand() % 100) + 1);
        //in case the impostor is defenestrate set his position to NULL cause we don't know where he is :)
        if(random_prob_defenes <= prob_defenes){
          players[turn_temp].Player_State = 4;
          impostors_number--;
          printf("\nThe %s impostor has been defenestrated\n",print_color_players(players[turn_temp].Player_Name));
          sleep(2);
          players[turn_temp].actual_position = NULL;
        }
      }
    }while(kill_check != 1);
  }
}

static void use_trap_door(){
  if(players[turn_temp].actual_position -> Room_Type == 4){
    /*in case during the game have been generated some trap door, randomly choose a room with trap and flow an index, when the index equals the room and the room is different from the actual player position the while stop
    room count is incremented everytime a new room is created, in the room_tail_insertion function
    in case the index is bigger then the rand room, everything is reinitialized to make a new flow*/
    if(trap_count >= 2){
      printf("\nTrap doors revealed in others room\n");
      sleep(1);
      trap_check = 1;
      int change = 0;
      int rand_room = 0;
      int index = 0;
      rand_room = rand () % room_count;
      //creating a temp pointer initialized to room_list
      Room *tmp = room_list;
      printf("\nFinding another room...\n");
      sleep(1);
      do{
        if((index == rand_room) && (players[turn_temp].actual_position != tmp) && tmp -> Room_Type == 4){
          players[turn_temp].actual_position = tmp;
          printf("\nNew position --> %p\n",players[turn_temp].actual_position);
          sleep(2);
          change = 1;
        }
        tmp = tmp -> pNext;
        index++;
        //in case the room estracted is equals to the one where the player used the trap door
        if(index > rand_room){
          index = 0;
          rand_room = (rand () % room_count);
          tmp = room_list;
        }
      }while(change == 0);
      sleep(2);
  }else{
    //in this case there are no others trap_door so the player change his position to another random room with the same way as above, just changing the conditions
    printf("\nNo trap doors revealed in others room\n");
    sleep(1);
    trap_check = 1;
    int change = 0;
    int rand_room = 0;
    int index = 0;
    rand_room = (rand () % room_count);
    Room *tmp = room_list;
    printf("\nFinding another room...\n");
    sleep(1);
    do{
      if((index == rand_room) && (players[turn_temp].actual_position != tmp)){
        players[turn_temp].actual_position = tmp;
        printf("\nNew position --> %p\n",players[turn_temp].actual_position);
        sleep(2);
        change = 1;
      }
      tmp = tmp -> pNext;
      index++;
      if(index > rand_room){
        index = 0;
        rand_room = (rand () % room_count);
        tmp = room_list;
      }
    }while(change == 0);
   }
 }else{
   printf("\nNo trap door revealed in this room\n");
   sleep(2);
   //in this case the player repeat the turn
   trap_check = 0;
 }
}

//same as the do quest but this can be called just from impostors, just make the room empty
static void sabotage(){
  if(players[turn_temp].actual_position -> Room_Type == 2){
    printf("\nSimple quest sabotaged\n");
    sleep(2);
    players[turn_temp].actual_position -> Room_Type = 1;
    sabotage_check = 1;
  }else if(players[turn_temp].actual_position -> Room_Type == 3){
    printf("\nComplex quest sabotaged\n");
    sleep(2);
    players[turn_temp].actual_position -> Room_Type = 1;
    sabotage_check = 1;
  }else{
    printf("\nThe room doesn't have quest\n");
    sleep(2);
    //in case the room doesn't have quest the player repeat the turn, same as others function
    sabotage_check = 0;
  }
}

static void start_game(){
  printf("\nPress Enter to back to menu...");
  while (getchar() != '\n');
}

//passing as parameter the choice of the player 1 forward, 2 left, 3 right.
static void room_tail_insertion(int n){
  if(n == 1){
    //if the direction is null, a new room is created setting all the pointers and types
    if(players[turn_temp].actual_position -> forward == NULL){
      room_count++;//incrementing of room_count everytime a new room is created, this is used in the use trap_door function for choosing a random room
      Room* new = (Room*) malloc(sizeof(Room));//allocation of a new room
      new -> forward = NULL;
      new -> left = NULL;
      new -> right = NULL;
      new -> previous = players[turn_temp].actual_position;//linking the new room to the room where the player actually is before changing position to the new one
      new -> pNext = NULL;
      new -> emergency_call = false;
      new -> Room_Type = random_room_type();

      if(new -> Room_Type == 4){
        trap_count++;
      }

      //the pNext pointer of the last node will point to the new room, and the room_list_last become the last node
      room_list_last -> pNext = new;
      room_list_last = new;

      //setting the forward pointer of the position of the player to the new room
      players[turn_temp].actual_position -> forward = new;

      //move the player to the new room
      players[turn_temp].actual_position = new;
    }else{
      //otherwise just change the position of the player to the room already created
      players[turn_temp].actual_position = players[turn_temp].actual_position -> forward;
    }
    //same as above, just change the direction
  }else if(n == 2){
    if(players[turn_temp].actual_position -> left == NULL){
      room_count++;
      Room* new = (Room*) malloc(sizeof(Room));
      new -> forward = NULL;
      new -> left = NULL;
      new -> right = NULL;
      new -> previous = players[turn_temp].actual_position;
      new -> pNext = NULL;
      new -> emergency_call = false;
      new -> Room_Type = random_room_type();

      if(new -> Room_Type == 4){
        trap_count++;
      }

      room_list_last -> pNext = new;
      room_list_last = new;

      players[turn_temp].actual_position -> left = new;

      players[turn_temp].actual_position = new;
    }else{
      players[turn_temp].actual_position = players[turn_temp].actual_position -> left;
    }
  }else if(n == 3){
    if(players[turn_temp].actual_position -> right == NULL){
      room_count++;
      Room* new = (Room*) malloc(sizeof(Room));
      new -> forward = NULL;
      new -> left = NULL;
      new -> right = NULL;
      new -> previous = players[turn_temp].actual_position;
      new -> pNext = NULL;
      new -> emergency_call = false;
      new -> Room_Type = random_room_type();

      if(new -> Room_Type == 4){
        trap_count++;
      }

      room_list_last -> pNext = new;
      room_list_last = new;

      players[turn_temp].actual_position -> right = new;

      players[turn_temp].actual_position = new;
    }else{
      players[turn_temp].actual_position = players[turn_temp].actual_position -> right;
    }
  }
}

//algorithm used to assign colors to all the players
static void random_assign_color(){
  int c,temp,r;
  int random_color[players_number];
  //fill array based on players_number with the numbers which represent colors
  for (temp = 0, c = 1; temp < players_number; c++, temp++){
    random_color[temp] = c;
  }
  //shuffle algorithm
  for(c = players_number - 1; c > 0; c--){
    r = rand() % c; //pop random number for the index of the array
    //swapping
    temp = random_color[c];
    random_color[c] = random_color[r];
    random_color[r] = temp;
  }
  //assign color to the array of players
  for(c = 0; c < players_number; c++){
    players[c].Player_Name = random_color[c];
  }
}

//function used to assign and generate the role of all the impostor/s in the game
static void random_assign_impostors(){
  int random_impostor = ((rand() % 100) + 1);

  /*based on how many players are playing the game. If 4 players there is 1 impostor, 5 players 30% of 2 impostors, 6 players 60% of 2 impostors,
  7 players 90% of 2 impostors, 8 players 50% of 3 impostors or 2, 9 players 80% of 3 impostors or 2, 10 players 3 impostors*/
  switch(players_number){
    case 4:
      impostors_number = 1;
      break;
    case 5:
      if(random_impostor <= 30){
        impostors_number = 2;
      }else{
        impostors_number = 1;
      }
      break;
    case 6:
      if(random_impostor <= 60){
        impostors_number = 2;
      }else{
        impostors_number = 1;
      }
      break;
    case 7:
      if(random_impostor <= 90){
        impostors_number = 2;
      }else{
        impostors_number = 1;
      }
      break;
    case 8:
      if(random_impostor <= 50){
        impostors_number = 3;
      }else{
        impostors_number = 2;
      }
      break;
    case 9:
      if(random_impostor <= 80){
        impostors_number = 3;
      }else{
        impostors_number = 2;
      }
      break;
    case 10:
        impostors_number = 3;
      break;
    default:
      break;
  }

  //set the astronauts number in case the number of impostors is + 1 of the astronauts number, while playing, the impostors win the game
  printf("\nNumber of impostors --> %d\n", impostors_number);
  astronauts_number = players_number - impostors_number;

  //generate random numbers based on how many impostors will be in the game
    if(impostors_number == 1){
        impostors1 = ((rand() % players_number) + 1);
        //flow all players and search which one has been choosen
        for(int i = 1; i <= players_number; i++){
          //if verified set the state of that player to 'impostor' or to 'astronaut'
          if(i == impostors1){
            players[i-1].Player_State = 2;
          }else{
            players[i-1].Player_State = 1;
          }
        }
    }else if(impostors_number == 2){
      do{
        impostors1 = ((rand() % players_number) + 1);
        impostors2 = ((rand() % players_number) + 1);

        for(int i = 1; i <= players_number; i++){
          if(i == impostors1 || i == impostors2){
            players[i-1].Player_State = 2;
          }else{
            players[i-1].Player_State = 1;
          }
        }
      }while(impostors1 == impostors2);
    }
    else{
      do{
        impostors1 = ((rand() % players_number) + 1);
        impostors2 = ((rand() % players_number) + 1);
        impostors3 = ((rand() % players_number) + 1);

        for(int i = 1; i <= players_number; i++){
          if(i == impostors1 || i == impostors2 || i == impostors3){
            players[i-1].Player_State = 2;
          }else{
            players[i-1].Player_State = 1;
          }
        }
      }while(impostors1 == impostors2 || impostors1 == impostors3 || impostors2 == impostors3);
    }
}

//function used to set the number of quest that the astronauts need to complete in order to win
static void quest_assign(){
  do{
    sleep(1);
    printf("\nInsert, now, the quest that the astronauts need to complete in order to win [15-35] --> ");
    scanf("%hu", &quest_to_finish);//%hu because the type of variable is unsigned short

    if(quest_to_finish < 15 || quest_to_finish > 35){
      sleep(1);
      printf("\nNumber of quest inserted wrong. Watch at the range.\n");
    }
  }while(quest_to_finish < 15 || quest_to_finish > 35);
}

//this function randomly generate the types of all the rooms in the game following a percentage (30% trap_door, 20% complex_quest, 30% simple_quest, 20% empty)
static int random_room_type(){
  int z;
  z = ((rand() % 100) + 1);

  if(z <= 30){
    return 4;//return 4 which is trap_door defined in the enum
  }else if(z <= 50){
    return 3;
  }else if(z <= 80){
    return 2;
  }else{
    return 1;
  }
}

//this function just flow all the players and print all the information
static void print_players(){
  for(int i = 0; i < players_number; i++){
    printf("\n%d° Player\n", i + 1);
    printf("Name --> %s\n", print_color_players(players[i].Player_Name));
    printf("State --> %s\n", print_state_players(players[i].Player_State));
    sleep(1);
  }
  printf("\nPress Enter to return to the menu...");
  while(getchar() != '\n');
}

//return all the colors of the players with an enum type as parameter
static char *print_color_players(Player_Name Player_name){
  switch(Player_name){
    case red:
      return "red";
      break;
    case blue:
      return "blue";
      break;
    case orange:
      return "orange";
      break;
    case yellow:
      return "yellow";
      break;
    case purple:
      return "purple";
      break;
    case green:
      return "green";
      break;
    case black:
      return "black";
      break;
    case white:
      return "white";
      break;
    case grey:
      return "grey";
      break;
    case brown:
      return "brown";
      break;
    default:
      return "Error";
      break;
  }
}

//return all the states of the players with an enum type as parameter
static char *print_state_players(Player_State Player_state){
  switch(Player_state){
    case astronaut:
      return "astronaut";
      break;
    case impostor:
      return "impostor";
      break;
    case murdered:
      return "murdered";
      break;
    case defenestrated:
      return "defenestrated";
      break;
    default:
      return "Error";
      break;
  }
}

//return all the types of the rooms generated with an enum type as parameter
static char *print_room_type(Room_Type Room_Type){
  switch(Room_Type){
    case empty:
      return "empty";
      break;
    case simple_quest:
      return "simple quest";
      break;
    case complex_quest:
      return "complex quest";
      break;
    case trap_door:
      return "trap door";
      break;
    default:
      return "Error";
      break;
  }
}

/*free all the rooms in the list created which is room_list passed by parameter above in the end game and in case the game has already been set in the set_game, the function flow all the list with a temp variable(tmp)
that store the pointer to the next room for new clear*/
static void free_list(Room* head){
  Room *tmp;

  printf("\nFreeing rooms list...\n");
  sleep(2);
  while(head != NULL){
    tmp = head;
    head = head -> pNext;
    free(tmp);
    tmp = NULL;
  }
  clr;
}
