/*Game story
A group of astronauts are traveling on the Skelt spaceship, and their goal is to complete all the planned activities (the quests) for the maintenance of the ship,
thus arriving at its destination. Among them, however, there are also some impostors, whose purpose is to secretly eliminate the astronauts
without getting find out from them. Will they be able to get to their destination before being decimated? Trannoi is freely inspired to an existing game (Among Us).
*/

//library created for this game
#include "gamelib.h"

int main(void){
  clr;//first use of the #define clr system("clear") defined in the gamelib.h
  time_t t;
  srand((unsigned)time(&t));//random number generator inizialization based on time
  printf("         To prevent graphics issues we recommend a full-screen view");
  while (getchar() != '\n');//this condition is used to make the users interact with the game pressing enter everytime
  clr;
  int choice = 0;
  int set_check = 0;
  do{
    sleep(1);//first use of the function sleep to make the game run smoothly
    clr;
    printf("-----------------------------------------------------------------------------------------\n");
    printf("|                                                                                       |\n");
    printf("|***************************************TRANNOI*****************************************|\n");
    printf("|                                                                                       |\n");
    printf("| GAME STORY: A group of astronauts are traveling on the Skelt spaceship, and their goal|\n");
    printf("| is to complete all the planned activities (the quests) for the maintenance of the ship|\n");
    printf("| thus arriving at its destination. Among them, however, there are also some impostors, |\n");
    printf("| whose purpose is to secretly eliminate the astronauts without getting find out from   |\n");
    printf("| them. Will they be able to get to their destination before being decimated? Trannoi   |\n");
    printf("| is freely inspired to an existing game (Among Us).                                    |\n");
    printf("|                                                                                       |\n");
    printf("|                                                                                       |\n");
    printf("| Before playing game, you need to configure it through the Set Game option given below |\n");
    printf("|                                                                                       |\n");
    printf("-----------------------------------------------------------------------------------------\n");
    sleep(1);
    printf("\n********/ MAIN MENU /********\n");
    printf("\n1° -      Set Game");
    printf("\n2° -        Play");
    printf("\n3° -      End Game\n");
    sleep(1);
    printf("\nChoice --> ");
    scanf("%d", &choice);

    switch(choice){
      case 1:
        set_check = 1;
        clr;
        set_game();
        clr;
        break;
      case 2:
        if(set_check == 0){
          printf("\nBefore starting a game, you need to set up the game. Back to the menu....\n");
          sleep(3);
          clr;
        }else{
          clr;
          play();
        }
        break;
      case 3:
        clr;
        end_game();
        break;
      default:
        printf("\nWrong command. Retry...\n");
        sleep(2);
        break;
    }
  }while(choice != 3);
}
