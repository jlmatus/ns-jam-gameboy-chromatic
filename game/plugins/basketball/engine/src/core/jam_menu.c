#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_menu.h"

static uint8_t used(const jam_menu_t *m, uint8_t id) {
    uint8_t i;
    for(i=0;i<m->stage && i<4;i++) if(m->setup.roster[i]==id)return 1;
    return 0;
}
static void choose_cursor(jam_menu_t *m) {
    if(m->stage<4) {
        m->cursor=m->setup.roster[m->stage];
        while(used(m,m->cursor))m->cursor=(m->cursor+1)%JAM_ROSTER_COUNT;
    } else m->cursor=m->setup.difficulty;
}
void jam_menu_init(jam_menu_t *m, const jam_setup_t *setup) JAM_BANKED {
    static const jam_setup_t defaults={{0,1,2,3},JAM_PICKUP};
    m->setup=setup?*setup:defaults;
    m->stage=0;
    m->page=0;
    m->previous_input=0;
    m->changed=1;
    choose_cursor(m);
}
uint8_t jam_menu_update(jam_menu_t *m, uint8_t input) JAM_BANKED {
    uint8_t pressed=(m->previous_input^input)&input;
    int8_t direction;
    m->previous_input=input;
    m->changed=0;
    if(m->stage<4 && (pressed&JAM_PAUSE)) {
        m->page=(m->page+1)%3;m->changed=1;return 0;
    }
    if(pressed&JAM_B) {
        if(m->page){m->page=0;m->changed=1;return 0;}
        if(m->stage)m->stage--;
        choose_cursor(m);m->changed=1;
        return 0;
    }
    if(m->stage==JAM_MENU_REVIEW) return (pressed&(JAM_A|JAM_TURBO))!=0;
    if(pressed&(JAM_MOVE_LEFT|JAM_MOVE_RIGHT|JAM_MOVE_UP|JAM_MOVE_DOWN)) {
        direction=(pressed&(JAM_MOVE_LEFT|JAM_MOVE_UP)) ? -1 : 1;
        do {
            if(direction<0)m->cursor=m->cursor?m->cursor-1:(m->stage<4?5:2);
            else m->cursor=(m->cursor+1)%(m->stage<4?6:3);
        } while(m->stage<4 && used(m,m->cursor));
        m->changed=1;
    }
    if(pressed&JAM_A) {
        if(m->stage<4)m->setup.roster[m->stage]=m->cursor;
        else m->setup.difficulty=m->cursor;
        m->stage++;m->page=0;choose_cursor(m);m->changed=1;
    }
    return 0;
}
