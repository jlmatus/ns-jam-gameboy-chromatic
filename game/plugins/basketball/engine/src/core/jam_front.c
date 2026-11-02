#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_front.h"

void jam_front_init(jam_front_t *f,uint8_t held) JAM_BANKED {
    f->elapsed=0;f->mode=JAM_FRONT_INTRO;f->selected=0;
    f->previous_input=held;f->frame=8;f->changed=1;
}
uint8_t jam_front_update(jam_front_t *f,uint8_t input,uint16_t frames) JAM_BANKED {
    uint8_t pressed=(input^f->previous_input)&input,old=f->frame,mode=f->mode;
    f->previous_input=input;f->changed=0;
    if(f->mode==JAM_FRONT_INTRO) {
        if(frames>=JAM_FRONT_DURATION-f->elapsed)f->elapsed=JAM_FRONT_DURATION;
        else f->elapsed+=frames;
        if((pressed&(JAM_A|JAM_TURBO|JAM_B)) || f->elapsed>=JAM_FRONT_DURATION)f->mode=JAM_FRONT_HOME;
    } else if(f->mode==JAM_FRONT_HELP) {
        if(pressed&(JAM_A|JAM_B|JAM_TURBO))f->mode=JAM_FRONT_HOME;
    } else {
        if(pressed&JAM_MOVE_DOWN)f->selected=(f->selected+1)%3;
        else if(pressed&JAM_MOVE_UP)f->selected=f->selected?f->selected-1:2;
        if(pressed&(JAM_A|JAM_TURBO)) {
            if(!f->selected)return JAM_FRONT_PLAY;
            if(f->selected==1)f->mode=JAM_FRONT_HELP;
            else {f->mode=JAM_FRONT_INTRO;f->elapsed=0;}
        }
    }
    if(f->mode==JAM_FRONT_HOME)f->frame=f->selected;
    else if(f->mode==JAM_FRONT_HELP)f->frame=3;
    else {
        uint16_t t=f->elapsed<30?0:(f->elapsed-30)/16;
        f->frame=8+(t>11?11:t);
    }
    f->changed=f->frame!=old || f->mode!=mode;
    return 0;
}
