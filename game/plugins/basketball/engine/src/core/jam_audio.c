#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_audio.h"

void jam_audio_sync(jam_audio_state_t *s,const jam_game_t *g) JAM_BANKED {
    s->passes=g->passes;s->shots=g->shots;s->baskets=g->baskets;s->blocks=g->blocks;
    s->steals=g->steals;s->shoves=g->shoves;s->runovers=g->runovers;s->rebounds=g->rebounds;
    s->seconds=g->clock_seconds;s->mode=g->mode;s->owner=(uint8_t)g->owner;
    s->height=g->height;s->flight=g->flight;s->dunk_result=g->dunk_result;
    s->quarter=g->quarter;s->phase=g->match_phase;s->paused=g->paused;
    s->turbo=g->players[g->controlled].turbo;
}
uint8_t jam_audio_event(jam_audio_state_t *s,const jam_game_t *g) JAM_BANKED {
    uint8_t event=JAM_SFX_NONE;
    if(g->paused) {jam_audio_sync(s,g);return event;}
    if(s->paused)event=JAM_SFX_BACK;
    else if(g->match_phase==JAM_MATCH_FINAL && s->phase!=JAM_MATCH_FINAL)
        event=g->score[0]>g->score[1]?JAM_SFX_WIN:g->score[0]<g->score[1]?JAM_SFX_LOSE:JAM_SFX_TIE;
    else if(s->seconds && !g->clock_seconds)event=JAM_SFX_HORN;
    else if(g->quarter!=s->quarter || (g->match_phase==JAM_MATCH_LIVE && s->phase>=JAM_MATCH_BREAK))event=JAM_SFX_START;
    else if(g->blocks!=s->blocks)
        event=g->mode==JAM_DUNK && g->dunk_result==JAM_DUNK_BLOCKED?JAM_SFX_DUNK_BLOCK:JAM_SFX_BLOCK;
    else if(g->mode==JAM_DUNK && s->mode!=JAM_DUNK)event=JAM_SFX_DUNK_LAUNCH;
    else if(g->mode==JAM_DUNK && g->dunk_result!=s->dunk_result) {
        if(g->dunk_result==JAM_DUNK_MADE)event=JAM_SFX_DUNK_SLAM;
        else if(g->dunk_result==JAM_DUNK_MISSED)event=JAM_SFX_RIM;
    }
    else if(g->baskets!=s->baskets && g->mode!=JAM_DUNK)event=JAM_SFX_SWISH;
    else if(g->shoves!=s->shoves || g->runovers!=s->runovers)event=JAM_SFX_SHOVE;
    else if(g->steals!=s->steals)event=JAM_SFX_STEAL;
    else if(g->passes!=s->passes)event=JAM_SFX_PASS;
    else if(g->shots!=s->shots)event=JAM_SFX_SHOT;
    else if(g->rebounds!=s->rebounds)event=JAM_SFX_CATCH;
    else if(g->mode==JAM_FINISH && !g->shot_good && s->mode==JAM_FINISH && s->flight<9 && g->flight>=9)event=JAM_SFX_RIM;
    else if(g->players[g->controlled].turbo && !s->turbo)event=JAM_SFX_TURBO;
    else if(g->mode==JAM_HELD && s->mode==JAM_HELD && (uint8_t)g->owner==s->owner &&
            g->height==7 && s->height>7 && !g->charge && !g->players[g->owner].windup)event=JAM_SFX_DRIBBLE;
    jam_audio_sync(s,g);
    return event;
}
