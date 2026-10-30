#pragma bank 255
#include "jam_audio.h"
#include "music_manager.h"
#include "jam_sfx_data.h"

static jam_audio_state_t audio_state;
/* Diagnostic IDs are presentation-only; no audio state affects simulation RNG. */
uint8_t jam_sfx_last;
uint16_t jam_sfx_events;

void jam_sfx_stop(void) BANKED {
    CRITICAL {
        sfx_reset_sample();
        music_sound_cut_mask(MUSIC_CH_1|MUSIC_CH_4);
        music_mute_mask=MUTE_MASK_NONE;
        music_sfx_priority=MUSIC_SFX_PRIORITY_MINIMAL;
    }
    jam_sfx_last=JAM_SFX_NONE;
}
void jam_sfx_init(const jam_game_t *g) BANKED {
    jam_sfx_stop();
    jam_sfx_events=0;
    jam_audio_sync(&audio_state,g);
}
void jam_sfx_sync(const jam_game_t *g) BANKED {
    jam_audio_sync(&audio_state,g);
}
void jam_sfx_play(uint8_t event) BANKED {
    if(!event || event>=JAM_SFX_COUNT)return;
    if(jam_sfx_priority[event]<music_sfx_priority)return;
    CRITICAL {
        music_play_sfx(CURRENT_BANK,jam_sfx_samples[event],MUSIC_CH_1|MUSIC_CH_4,jam_sfx_priority[event]);
    }
    jam_sfx_last=event;jam_sfx_events++;
}
void jam_sfx_update(const jam_game_t *g) BANKED {
    uint8_t event;
    if(g->paused && !audio_state.paused)jam_sfx_stop();
    event=jam_audio_event(&audio_state,g);
    if(event)jam_sfx_play(event);
}
