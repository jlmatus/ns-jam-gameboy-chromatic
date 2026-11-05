#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"
#include <string.h>

void jam_match_clock(jam_game_t *g,uint16_t frames) JAM_BANKED {
    uint16_t before;
    if(g->paused || g->match_phase!=JAM_MATCH_LIVE || g->mode==JAM_DUNK ||
       g->mode==JAM_RESET || g->mode==JAM_INBOUND ||
       (g->mode==JAM_PASS && g->inbound_pass))return;
    /* One VBlank is exactly 70,224 / 4,194,304 seconds = 1097 / 65536.
       Carrying a 16-bit fraction avoids drift and costly long division. */
    while(frames-- && g->clock_seconds) {
        before=g->clock_fraction;
        g->clock_fraction+=1097;
        if(g->clock_fraction<before && !--g->clock_seconds) {
            g->clock_fraction=0;
            g->match_phase=JAM_MATCH_BUZZER;
        }
    }
}

static void next_period(jam_game_t *g) {
    uint8_t next_team=(g->quarter+g->overtime)&1;
    if(g->quarter>=g->setup.periods) {
        g->overtime++;
        g->clock_seconds=JAM_OVERTIME_SECONDS;
    } else {
        g->quarter++;
        g->clock_seconds=JAM_QUARTER_SECONDS;
    }
    g->clock_fraction=0;g->match_phase=JAM_MATCH_LIVE;g->cam_seen=0;
    memset(g->players,0,sizeof(g->players));
    jam_tipoff(g);
    g->ball_x=g->players[next_team*2].x;g->ball_y=g->players[next_team*2].y;
    g->from_x=g->target_x=g->ball_x;g->from_y=g->target_y=g->ball_y;
    g->height=g->flight=g->duration=g->scored=g->shot_good=0;
    g->dunk_time=g->dunk_result=g->dunk_block_time=g->block_attempts=0;
    g->launch_height=0;
    g->shot_blocked=g->loose_rebound=0;
    jam_begin_inbound(g,next_team);
}

/* Returns true while play must remain frozen. No new shot can start at 0:00. */
uint8_t jam_match_control(jam_game_t *g,uint8_t pressed) JAM_BANKED {
    jam_setup_t setup;
    if(g->match_phase==JAM_MATCH_LIVE)return 0;
    if(g->match_phase==JAM_MATCH_BUZZER) {
        if(g->mode==JAM_SHOT || g->mode==JAM_FINISH || g->mode==JAM_DUNK)return 0;
        if(g->quarter<g->setup.periods)g->match_phase=JAM_MATCH_BREAK;
        else g->match_phase=g->score[0]==g->score[1]?JAM_MATCH_OVERTIME:JAM_MATCH_FINAL;
        g->charge=g->block=0;
        return 1;
    }
    if(pressed&JAM_A) {
        if(g->match_phase==JAM_MATCH_FINAL) {
            setup=g->setup;
            jam_init_lineup(g,&setup);
            /* Holding A to rematch cannot also pass the opening possession. */
            g->previous_input=JAM_A;
        } else next_period(g);
    }
    return 1;
}
