#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"

void jam_stats_basket(jam_game_t *g,uint8_t dunk) JAM_BANKED {
    uint16_t points;
    jam_player_stats_t *s;
    if(g->scored || g->passer<0 || g->passer>=4)return;
    s=&g->stats[(uint8_t)g->passer];
    points=g->shot_points;
    if(points>JAM_SCORE_MAX-g->score[g->shot_team])points=JAM_SCORE_MAX-g->score[g->shot_team];
    g->score[g->shot_team]+=points;s->points+=points;
    s->field_made++;
    if(g->shot_points==3)s->three_made++;
    if(dunk){s->dunks++;g->dunks++;}
    g->baskets++;g->scored=1;
}
