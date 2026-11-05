#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"

/* CPU ball security. A CPU carrier under pressure passes to a clearly more
   open teammate, or dribbles around a defender standing between him and the
   rim instead of walking into his reach. Returns 1 when it passed. */
static int16_t gap(int16_t a,int16_t b){return a>b?a-b:b-a;}
/* Dribble side chosen this possession (1 up, 0 down, 2 unset). */
static uint8_t side[4]={2,2,2,2};
static int16_t near(const jam_game_t *g,uint8_t a,uint8_t b){
    return gap(g->players[a].x,g->players[b].x)+gap(g->players[a].y,g->players[b].y);
}
/* True when no upright opponent stands within 16 pixels of the straight
   line from (x0,y0) to (x1,y1), between its ends. */
static uint8_t lane(const jam_game_t *g,uint8_t team,int16_t x0,int16_t y0,int16_t x1,int16_t y1){
    uint8_t d;
    int16_t dx=x1-x0,dy=y1-y0,along,side;
    for(d=0;d<4;d++) if((d>>1)!=team && !g->players[d].stun) {
        const jam_player_t *o=&g->players[d];
        if(gap(dx,0)>=gap(dy,0)) {
            if(!dx || (dx>0 ? o->x<=x0 || o->x>=x1 : o->x>=x0 || o->x<=x1))continue;
            along=y0+(int16_t)((int32_t)dy*(o->x-x0)/dx);side=gap(o->y,along);
        } else {
            if(dy>0 ? o->y<=y0 || o->y>=y1 : o->y>=y0 || o->y<=y1)continue;
            along=x0+(int16_t)((int32_t)dx*(o->y-y0)/dy);side=gap(o->x,along);
        }
        if(side<64)return 0;
    }
    return 1;
}
uint8_t jam_cpu_rim_lane(const jam_game_t *g,uint8_t p) JAM_BANKED {
    const jam_player_t *a=&g->players[p];
    return lane(g,p>>1,a->x,a->y,(p<2?JAM_RIM_RIGHT:JAM_RIM_LEFT)*4,JAM_RIM_Y*4);
}
/* At the rise: dunk through a clear lane; through a defender, high-INSIDE
   players still attack the rim ((INSIDE - 5) x 15%: MAGIC 60%, CHRUX and
   MEAT 45%, BAMBI and CHOMPA 30%) and can be blocked; others pull up. */
uint8_t jam_cpu_attack_rim(jam_game_t *g,uint8_t p) JAM_BANKED {
    int8_t inside=(int8_t)jam_roster[g->setup.roster[p]].ratings.dunk;
    return jam_cpu_rim_lane(g,p) || (inside>5 && jam_roll_percent(g)<(inside-5)*15);
}
/* Shoot when the open chance at this spot is worth it: about 55% for a two,
   40% for a three (equal expected points). Dunkers rise only through a clear
   lane. A random gate lets carriers sometimes drive closer first. */
uint8_t jam_cpu_shot_choice(jam_game_t *g,uint8_t p) JAM_BANKED {
    const jam_player_t *a=&g->players[p];
    const jam_ratings_t *r=&jam_roster[g->setup.roster[p]].ratings;
    int16_t d=gap(a->x,(p<2?JAM_RIM_RIGHT:JAM_RIM_LEFT)*4)/4,depth=gap(a->y,JAM_RIM_Y*4)/2;
    uint8_t three=(uint32_t)d*d+(uint32_t)depth*depth>88u*88u,chance;
    /* In dunk range: gather through a clear lane, or half the time anyway
       (jam_cpu_attack_rim decides at the rise whether to go up or pull up). */
    if(r->dunk>=6 && d<=jam_dunk_range(g,p) && gap(a->y,JAM_RIM_Y*4)<=72 &&
       (jam_cpu_rim_lane(g,p) || jam_roll_percent(g)<50))return 1;
    chance=jam_shot_chance(g,p,0);
    /* Late in a possession, take any reasonable look, contested or not. */
    if(g->possession_time>480 && chance>=35)return 1;
    /* The chance already includes a close defender's contest. */
    return chance>=(three?40:55) && jam_roll_percent(g)<40;
}
uint8_t jam_cpu_carrier(jam_game_t *g,uint8_t p,int16_t *tx,int16_t *ty) JAM_BANKED {
    jam_player_t *a=&g->players[p];
    uint8_t d,close=4,team=p>>1,mate=p^1,ahead;
    int16_t press=0x7fff,open=0x7fff,n;
    for(d=0;d<4;d++) if((d>>1)!=team && !g->players[d].stun) {
        n=near(g,d,p);if(n<press){press=n;close=d;}
        n=near(g,d,mate);if(n<open)open=n;
    }
    ahead=close<4 && (team ? g->players[close].x<a->x : g->players[close].x>a->x);
    if(close<4 && press<JAM_CPU_PRESSURE) {
        if(!a->cooldown && open>press+64 && lane(g,team,a->x,a->y,g->players[mate].x,g->players[mate].y) && jam_roll_percent(g)<62) {
            jam_pass(g,p,mate);a->cooldown=50;return 1;
        }
        if(ahead) {
            /* Pick one side and keep it: away from a defender who is clearly
               above or below, else keep this possession's side (first chosen
               toward the middle of the court). Keep
               edging forward so the carrier faces the rim, not the sideline. */
            int16_t dy=g->players[close].y-a->y;
            uint8_t up;
            if(g->possession_time<2 || side[p]>1)side[p]=a->y>82*4; /* toward the roomier middle */
            up=dy>32 ? 1 : dy<-32 ? 0 : side[p];
            side[p]=up;
            n=a->y+(up ? -96 : 96);
            /* Near any edge (sidelines, the angled ends, the baseline) there
               is no room: keep driving instead of stepping out of bounds. */
            /* Stay 12 pixels inside the sidelines so a bump cannot knock him
               out; pinned at an edge, or at an angled end, keep driving. */
            if(n<(JAM_TOP+12)*4)n=(JAM_TOP+12)*4;
            if(n>(JAM_BOTTOM-15)*4)n=(JAM_BOTTOM-15)*4;
            if(gap(n,a->y)>=32 && jam_court_contains(a->x+(team ? -64 : 64),n)){*tx=a->x+(team ? -32 : 32);*ty=n;}
        }
    } else if(ahead && press<JAM_CPU_PRESSURE+64 && a->y>(JAM_TOP+12)*4 && a->y<(JAM_BOTTOM-15)*4) {
        /* Just outside reach: hold the new lane rather than cutting back
           across the defender, which looped the carrier in and out. Near a
           sideline, head back toward the middle instead of getting pinned. */
        *ty=a->y;
    } else if(!a->cooldown && g->possession_time>360 && lane(g,team,a->x,a->y,g->players[mate].x,g->players[mate].y) && jam_roll_percent(g)<15) {
        /* A long, unpressured possession still moves the ball. */
        jam_pass(g,p,mate);a->cooldown=50;return 1;
    }
    return 0;
}
