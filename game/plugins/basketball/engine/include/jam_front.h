#ifndef JAM_FRONT_H
#define JAM_FRONT_H
#include "jam_sim.h"
#define JAM_FRONT_INTRO 0
#define JAM_FRONT_HOME 1
#define JAM_FRONT_HELP 2
#define JAM_FRONT_PLAY 1
#define JAM_FRONT_DURATION 270
typedef struct {
    uint16_t elapsed;
    uint8_t mode,selected,previous_input,frame,changed;
} jam_front_t;
void jam_front_init(jam_front_t *f,uint8_t held) JAM_BANKED;
uint8_t jam_front_update(jam_front_t *f,uint8_t input,uint16_t frames) JAM_BANKED;
#ifdef __SDCC
void jam_front_art(uint8_t frame) BANKED;
void jam_front_scene_save(void) BANKED;
void jam_front_scene_restore(void) BANKED;
#endif
#endif
