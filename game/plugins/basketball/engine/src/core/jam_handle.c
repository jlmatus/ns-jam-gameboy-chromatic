#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"

/* Ball handling and rebounding (jam_sim.c's bank is nearly full).
   A handler's edge over a defender is (HANDLE + SPEED) - (SPEED + STEAL):
   MATUS or CHRUX on BAMBI is +9, MEAT on BAMBI +6, BAMBI on MATUS below 0.
   A sharp turn (a new direction at a right angle or more) by a handler with
   an edge freezes nearby CPU defenders for a moment: the first step. The
   same edge lets him slide around a body instead of stopping (jam_contact.c).
   Positions are quarter pixels. */
static int16_t gap(int16_t a,int16_t b){return a>b?a-b:b-a;}
static const jam_ratings_t *rating(const jam_game_t *g,uint8_t p){return &jam_roster[g->setup.roster[p]].ratings;}
/* Last moving direction of each carrier (sign of vx, vy), the first-step
   cooldown, and each defender's remaining frozen updates. */
static int8_t last_x[4],last_y[4];
static uint8_t cooldown[4],frozen[4];

int8_t jam_handle_edge(const jam_game_t *g,uint8_t carrier,uint8_t defender) JAM_BANKED {
    const jam_ratings_t *c=rating(g,carrier),*d=rating(g,defender);
    return (int8_t)(c->handle+c->speed)-(int8_t)(d->speed+d->steal);
}
void jam_handle_reset(void) JAM_BANKED {
    uint8_t i;
    for(i=0;i<4;i++)last_x[i]=last_y[i]=cooldown[i]=frozen[i]=0;
}
/* Frozen while the ball stays with the carrier who beat him. */
uint8_t jam_beaten(const jam_game_t *g,uint8_t p) JAM_BANKED {
    return p<4 && frozen[p] && g->mode==JAM_HELD && g->owner>=0 && (g->owner>>1)!=(p>>1);
}
/* Once per update, after movement: detect the carrier's sharp turn. */
void jam_first_step(jam_game_t *g) JAM_BANKED {
    uint8_t i,o,d;
    int8_t sx,sy,edge,lag;
    const jam_player_t *c;
    for(i=0;i<4;i++){if(frozen[i])frozen[i]--;if(cooldown[i])cooldown[i]--;}
    if(g->mode!=JAM_HELD || g->owner<0)return;
    o=(uint8_t)g->owner;c=&g->players[o];
    sx=c->vx>0?1:c->vx<0?-1:0;sy=c->vy>0?1:c->vy<0?-1:0;
    if(!sx && !sy)return;
    /* A right angle or more: the dot product of old and new is not positive. */
    if((last_x[o]||last_y[o]) && sx*last_x[o]+sy*last_y[o]<=0 && !cooldown[o]) {
        for(d=0;d<4;d++)if((d>>1)!=(o>>1) && d!=g->controlled && !g->players[d].stun &&
            gap(g->players[d].x,c->x)+gap(g->players[d].y,c->y)<JAM_STEP_RANGE) {
            edge=jam_handle_edge(g,o,d);
            lag=edge*2-(d>=2?(int8_t)g->setup.difficulty*2:0);
            if(lag>JAM_STEP_MAX)lag=JAM_STEP_MAX;
            if(lag>0){frozen[d]=(uint8_t)lag;cooldown[o]=JAM_STEP_COOLDOWN;}
        }
    }
    last_x[o]=sx;last_y[o]=sy;
}
/* Loose ball: the nearest upright player claims it, with REBOUND worth 1.5
   pixels of reach per point from 5 on a missed or blocked shot (MAGIC 9 is
   6 pixels longer, MATUS 3 is 3 pixels shorter). Returns the player, or 4
   when everyone is down; *reach is that player's adjusted distance. */
uint8_t jam_loose_target(jam_game_t *g,int16_t *reach) JAM_BANKED {
    uint8_t i,target=4;
    int16_t d,best=32767;
    for(i=0;i<4;i++)if(!g->players[i].stun) {
        d=gap(g->players[i].x,g->ball_x)+gap(g->players[i].y,g->ball_y);
        if(g->loose_rebound)d-=((int16_t)rating(g,i)->rebound-5)*6;
        if(d<best){best=d;target=i;}
    }
    *reach=best;
    return target;
}
