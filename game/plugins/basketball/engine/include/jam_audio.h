#ifndef JAM_AUDIO_H
#define JAM_AUDIO_H
#include "jam_sim.h"
enum {
    JAM_SFX_NONE, JAM_SFX_MOVE, JAM_SFX_CONFIRM, JAM_SFX_BACK, JAM_SFX_START,
    JAM_SFX_DRIBBLE, JAM_SFX_PASS, JAM_SFX_SHOT, JAM_SFX_RIM, JAM_SFX_SWISH,
    JAM_SFX_CATCH, JAM_SFX_STEAL, JAM_SFX_SHOVE, JAM_SFX_TURBO,
    JAM_SFX_DUNK_LAUNCH, JAM_SFX_DUNK_SLAM, JAM_SFX_DUNK_BLOCK, JAM_SFX_BLOCK,
    JAM_SFX_HORN, JAM_SFX_WIN, JAM_SFX_LOSE, JAM_SFX_TIE, JAM_SFX_COUNT
};
typedef struct {
    uint16_t passes,shots,baskets,blocks,steals,shoves,runovers,rebounds,seconds;
    uint8_t mode,owner,height,flight,dunk_result,quarter,phase,paused,turbo;
} jam_audio_state_t;
void jam_audio_sync(jam_audio_state_t *s,const jam_game_t *g) JAM_BANKED;
uint8_t jam_audio_event(jam_audio_state_t *s,const jam_game_t *g) JAM_BANKED;
#ifdef __SDCC
void jam_sfx_init(const jam_game_t *g) BANKED;
void jam_sfx_sync(const jam_game_t *g) BANKED;
void jam_sfx_play(uint8_t event) BANKED;
void jam_sfx_update(const jam_game_t *g) BANKED;
void jam_sfx_stop(void) BANKED;
#endif
#endif
