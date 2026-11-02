#pragma bank 255
#include "jam_front.h"
#include "palette.h"
#include "scroll.h"
#include "data_manager.h"
#include "data/bg_managua_prototype_court.h"
#include <string.h>
void load_background(const background_t *background,UBYTE bank) BANKED;
extern uint16_t jam_stage_palettes[28];
void jam_front_scene_save(void) BANKED {
    memcpy(jam_stage_palettes,BkgPalette,sizeof(jam_stage_palettes));
}
void jam_front_scene_restore(void) BANKED {
    /* Front art uses signed BG indices 0-127 in both banks, never sprite
       patterns or resident font/gauge 192-236. Restore the court's BG payload. */
    load_background(&bg_managua_prototype_court,BANK(bg_managua_prototype_court));
    memcpy(BkgPalette,jam_stage_palettes,sizeof(jam_stage_palettes));
    set_bkg_palette(0,7,jam_stage_palettes);
    pending_w_i=pending_h_i=0;scroll_x=scroll_y=0x7fff;scroll_update();
    VBK_REG=0;
}
