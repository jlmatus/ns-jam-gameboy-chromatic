#pragma bank 255
#include "jam_sim.h"
#include "palette.h"
#include <string.h>

/* Color zero is transparent on OBJ hardware; skin must occupy color one. */
static const uint16_t skin[6]={
    RGB(26,19,14),RGB(24,19,14),RGB(29,22,18),
    RGB(25,18,13),RGB(28,22,18),RGB(26,20,15)
};
void jam_player_palettes(const jam_setup_t *setup) BANKED {
    uint8_t i,id;
    uint16_t colors[4];
    colors[0]=0;colors[3]=RGB(3,5,7);
    for(i=0;i<4;i++){
        id=setup->roster[i];colors[1]=skin[id];
        colors[2]=i<2?RGB(29,29,30):RGB(27,9,9);
        memcpy(&SprPalette[id],colors,8);
        set_sprite_palette(id,1,(const uint16_t *)&SprPalette[id]);
    }
    colors[1]=skin[4];colors[2]=RGB(18,25,7);
    memcpy(&SprPalette[7],colors,8);set_sprite_palette(7,1,(const uint16_t *)&SprPalette[7]);
}
