#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"

uint8_t jam_turbo_available(const jam_game_t *g,uint8_t who) JAM_BANKED {
    return who<4 && g->turbo_energy[who] && !g->turbo_exhausted[who];
}

uint8_t jam_turbo_spend(jam_game_t *g,uint8_t who,uint8_t cost) JAM_BANKED {
    if(!cost || !jam_turbo_available(g,who) || g->turbo_energy[who]<cost)return 0;
    g->turbo_energy[who]-=cost;
    g->turbo_delay[who]=JAM_TURBO_DELAY;
    if(!g->turbo_energy[who])g->turbo_exhausted[who]=1;
    return 1;
}

void jam_turbo_recover(jam_game_t *g,uint8_t who,uint8_t held) JAM_BANKED {
    uint8_t energy;
    if(who>=4)return;
    if(g->turbo_delay[who])g->turbo_delay[who]--;
    /* Holding START at empty cannot produce tiny automatic sprint bursts. */
    if(held || g->turbo_delay[who])return;
    energy=g->turbo_energy[who];
    if(energy<JAM_TURBO_MAX){
        energy+=JAM_TURBO_RECOVERY;
        g->turbo_energy[who]=energy>JAM_TURBO_MAX?JAM_TURBO_MAX:energy;
    }
    if(g->turbo_energy[who]>=JAM_TURBO_READY)g->turbo_exhausted[who]=0;
}
