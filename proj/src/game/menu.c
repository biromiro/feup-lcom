#include "menu.h"

int menu_index=0;

void handle_menu_mouse(xpm_object *cursor) {
    if(cursor->x<=690 && cursor->x>=460 && cursor->y>=425 && cursor->y<=485){
        menu_index=1;
    }
    else if(cursor->x<=820 && cursor->x>=325 && cursor->y>=505 && cursor->y<=565){
        menu_index=2;
    }else if(cursor->x<=650 && cursor->x>=495 && cursor->y>=595 && cursor->y<=655){
        menu_index=3;
    }else if(cursor->x<=820 && cursor->x>=325 && cursor->y>=700 && cursor->y<=750){
        menu_index=4;
    }
    else
        menu_index=0;
}

gameState return_game() {
    if(menu_index==1){
        return GAME;
    }
    else if(menu_index==2){
        return INSTRUCTIONS;
    }
    else if(menu_index==3){
        return COOP;
    }else if(menu_index==4){
        return EXIT;
    }
    else
        return START;
}


