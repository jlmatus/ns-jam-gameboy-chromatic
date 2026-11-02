#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_results.h"
#include <string.h>

static uint8_t glyph(char c) {
    if(c>='A' && c<='Z')return c-'A'+1;
    if(c>='0' && c<='9')return c-'0'+27;
    if(c==':')return 37;
    if(c=='>')return 38;
    if(c=='-')return 39;
    return 0;
}
static void text(uint8_t *tiles,uint8_t x,uint8_t y,const char *s) {
    uint16_t at=(uint16_t)y*20+x,end=(uint16_t)(y+1)*20;
    while(*s && at<end && at<360)tiles[at++]=192+glyph(*s++);
}
static void number(uint8_t *tiles,uint8_t x,uint8_t y,uint16_t n,uint8_t digits) {
    uint8_t i=digits;
    /* Width is explicit; never let a large statistic overwrite its label. */
    uint16_t max=digits==3?999:9999;
    if(n>max)n=max;
    while(i){tiles[(uint16_t)y*20+x+--i]=192+27+n%10;n/=10;}
}
void jam_results_init(jam_results_t *r,uint8_t held) JAM_BANKED {
    r->page=0;r->previous_input=held;r->changed=1;
}
uint8_t jam_results_update(jam_results_t *r,uint8_t input) JAM_BANKED {
    uint8_t pressed=(input^r->previous_input)&input;
    r->previous_input=input;r->changed=0;
    if(pressed&JAM_B)return JAM_RESULTS_LINEUP;
    if(pressed&JAM_A)return JAM_RESULTS_REMATCH;
    if(pressed&JAM_MOVE_RIGHT){r->page=(r->page+1)%JAM_RESULTS_PAGES;r->changed=1;}
    else if(pressed&JAM_MOVE_LEFT){r->page=r->page?r->page-1:JAM_RESULTS_PAGES-1;r->changed=1;}
    return JAM_RESULTS_NONE;
}
void jam_results_tiles(const jam_game_t *g,uint8_t page,uint8_t *tiles) JAM_BANKED {
    uint8_t winner=g->score[0]>g->score[1]?0:1,p;
    const jam_player_stats_t *s;
    memset(tiles,192,360);
    if(!page || page>=JAM_RESULTS_PAGES) {
        text(tiles,3,0,"FINAL RESULTS");
        text(tiles,4,2,winner?"CPU TEAM WINS":"YOUR TEAM WINS");
        number(tiles,4,4,g->score[0],4);text(tiles,9,4,"-");number(tiles,11,4,g->score[1],4);
        text(tiles,0,6,"YOUR TEAM");text(tiles,11,6,"CPU TEAM");
        for(p=0;p<4;p++)text(tiles,p<2?1:12,8+(p&1),jam_roster[g->setup.roster[p]].name);
        text(tiles,0,11,"WINNERS");
        text(tiles,0,12,jam_roster[g->setup.roster[winner*2]].name);
        text(tiles,11,12,jam_roster[g->setup.roster[winner*2+1]].name);
        if(g->overtime){text(tiles,0,13,"AFTER OT");number(tiles,12,13,g->overtime,4);}
        else text(tiles,3,13,"REGULATION FINAL");
        text(tiles,0,14,"LEFT RIGHT: STATS");
    } else {
        p=page-1;s=&g->stats[p];
        text(tiles,0,0,"PLAYER STATS");text(tiles,14,0," OF 4");
        tiles[13]=192+27+page;
        text(tiles,1,2,jam_roster[g->setup.roster[p]].name);
        text(tiles,10,2,jam_roster[g->setup.roster[p]].jersey);
        text(tiles,0,3,p<2?"YOUR TEAM":"CPU TEAM");
        text(tiles,12,3,(p>>1)==winner?"WINNER":"");
        text(tiles,0,5,"POINTS");number(tiles,16,5,s->points,4);
        text(tiles,12,6,"MADE-ATT");
        text(tiles,0,7,"FIELD GOALS");number(tiles,13,7,s->field_made,3);text(tiles,16,7,"-");number(tiles,17,7,s->field_attempted,3);
        text(tiles,0,8,"3 POINTERS");number(tiles,13,8,s->three_made,3);text(tiles,16,8,"-");number(tiles,17,8,s->three_attempted,3);
        text(tiles,0,10,"REBOUNDS");number(tiles,16,10,s->rebounds,4);
        text(tiles,0,11,"STEALS");number(tiles,16,11,s->steals,4);
        text(tiles,0,12,"BLOCKS");number(tiles,16,12,s->blocks,4);
        text(tiles,0,13,"DUNKS");number(tiles,16,13,s->dunks,4);
        text(tiles,0,14,"LEFT RIGHT: PLAYER");
    }
    text(tiles,0,16,"A REMATCH");
    text(tiles,0,17,"B CHANGE LINEUP");
}
