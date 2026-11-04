#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"

/* CPU ball security. A CPU carrier under pressure passes to a clearly more
   open teammate, or dribbles around a defender standing between him and the
   rim instead of walking into his reach. Returns 1 when it passed. */
static int16_t gap(int16_t a,int16_t b){return a>b?a-b:b-a;}
static int16_t near(const jam_game_t *g,uint8_t a,uint8_t b){
    return gap(g->players[a].x,g->players[b].x)+gap(g->players[a].y,g->players[b].y);
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
        if(!a->cooldown && open>press+64 && jam_roll_percent(g)<62) {
            jam_pass(g,p,mate);a->cooldown=50;return 1;
        }
        if(ahead) {
            /* Pick one side and keep it: away from a defender who is clearly
               above or below, else toward the open half of the court. Keep
               edging forward so the carrier faces the rim, not the sideline. */
            int16_t dy=g->players[close].y-a->y;
            uint8_t up=dy>32 ? 1 : dy<-32 ? 0 : a->y<JAM_RIM_Y*4;
            n=a->y+(up ? -96 : 96);
            /* At a sideline there is no room: keep driving instead. */
            if(n>=(JAM_TOP+8)*4 && n<=(JAM_BOTTOM-8)*4){*tx=a->x+(team ? -32 : 32);*ty=n;}
        }
    } else if(ahead && press<JAM_CPU_PRESSURE+64) {
        /* Just outside reach: hold the new lane rather than cutting back
           across the defender, which looped the carrier in and out. */
        *ty=a->y;
    } else if(!a->cooldown && g->possession_time>360 && jam_roll_percent(g)<15) {
        /* A long, unpressured possession still moves the ball. */
        jam_pass(g,p,mate);a->cooldown=50;return 1;
    }
    return 0;
}
