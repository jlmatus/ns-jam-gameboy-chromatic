#ifndef JAM_DUNK_CAM_H
#define JAM_DUNK_CAM_H
#include "jam_sim.h"
#ifdef __SDCC
extern uint8_t jam_cam_row[20],jam_cam_attrs[20];
extern uint8_t jam_cam_upload,jam_cam_map_target;
extern int16_t jam_cam_zoom,jam_cam_origin,jam_cam_avatar;
void jam_dunk_art(uint8_t player,uint8_t frame,uint8_t failed,uint8_t mirror) BANKED;
void jam_dunk_cam_begin(const jam_game_t *g) BANKED;
void jam_dunk_cam_render(const jam_game_t *g) BANKED;
void jam_dunk_cam_end(void) BANKED;
extern uint8_t jam_cam_active;
#endif
#endif
