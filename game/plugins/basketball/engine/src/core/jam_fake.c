#ifdef __SDCC
#pragma bank 255
#endif
/* Pump fake and traveling for the controlled ball carrier. A tap of B shorter
   than JAM_FAKE_FRAMES ends the dribble without a shot; from then on the
   player may shoot, pass or fake again, but any step is a traveling call. */
#include "jam_sim.h"

static uint8_t team(uint8_t p) { return p >> 1; }
static int16_t gap(int16_t a,int16_t b) { return a > b ? a-b : b-a; }

void jam_fake_tick(jam_game_t *g) JAM_BANKED {
    if(g->fake_time) g->fake_time--;
    if(g->bite_time && !--g->bite_time) g->bitten=0;
    /* Only the controlled ball carrier can have a dead (picked-up) dribble. */
    if(g->mode!=JAM_HELD || g->owner!=(int8_t)g->controlled) g->picked_up=g->fake_time=0;
    else if(g->picked_up && g->picked_up<255) g->picked_up++;
}

/* With a dead ball, a new arrow press, or arrows still held past the gather
   step, is traveling: whistle, other team's inbound. */
uint8_t jam_fake_travel(jam_game_t *g,uint8_t input,uint8_t pressed) JAM_BANKED {
    if(!(input&15) || (!(pressed&15) && g->picked_up<=JAM_GATHER_STEP)) return 0;
    g->charge=g->picked_up=g->fake_time=0;
    g->next_team=team(g->controlled)^1; g->mode=JAM_RESET; g->reset_time=30;
    g->message=JAM_MSG_TRAVEL; g->message_time=75;
    return 1;
}

/* Nearby CPU defenders may leave their feet. Shot blockers bite more often;
   higher CPU levels read the fake better. Real gathers are never rolled. */
void jam_fake_release(jam_game_t *g) JAM_BANKED {
    uint8_t i,who=g->controlled;
    int16_t chance;
    g->fake_time=JAM_FAKE_SHOW;
    if(!g->picked_up) g->picked_up=1;
    g->message=JAM_MSG_FAKE; g->message_time=75;
    for(i=0;i<4;i++) {
        jam_player_t *d=&g->players[i];
        const jam_ratings_t *r=&jam_roster[g->setup.roster[i]].ratings;
        if(team(i)==team(who) || d->stun || d->block_time || (g->bitten&(1u<<i))) continue;
        if(gap(d->x,g->players[who].x)+gap(d->y,g->players[who].y)>=
           80+r->block*4+((int8_t)jam_body(g,i)->height_px-40)*4) continue;
        chance=35+r->block*3-(int16_t)g->setup.difficulty*10;
        if(jam_roll_percent(g)<chance) {
            d->block_time=8+r->block;
            d->action=d->block_time;
            g->bitten|=1u<<i;
            g->bite_time=JAM_BITE_FRAMES;
        }
    }
}

/* Ball lift for the carrier: the shot gather, a visible fake, or a dead ball
   held at the waist instead of dribbled. */
uint8_t jam_fake_hold(jam_game_t *g) JAM_BANKED {
    if(g->charge) return g->charge;
    if(g->picked_up) g->height=jam_body(g,g->controlled)->height_px/2;
    return g->fake_time>5 ? 5 : g->fake_time;
}
