#pragma bank 255
#include "jam_dunk_cam.h"
#include "jam_hud.h"
#include "jam_dunk_font.h"
#include "actor.h"
#include "palette.h"
#include "ui.h"
#include "interrupts.h"
#include "scroll.h"
#include "data_manager.h"
#include "data/bg_managua_prototype_court.h"
#include <string.h>
/* Stock engine implementation is banked but not exported by its public header. */
void load_background(const background_t *background,UBYTE bank) BANKED;

uint8_t jam_cam_row[20],jam_cam_attrs[20],jam_cam_active;
uint8_t jam_cam_upload,jam_cam_map_target;
int16_t jam_cam_zoom,jam_cam_origin,jam_cam_avatar;
static uint16_t saved_palettes[28];
static uint8_t last_frame,last_failed;
static uint8_t court_restored;
static uint8_t saved_overlay_actors;
static const uint16_t skins[6]={RGB(26,19,14),RGB(24,19,14),RGB(29,22,18),RGB(25,18,13),RGB(28,22,18),RGB(26,20,15)};
static void palette(uint8_t slot,uint16_t a,uint16_t b,uint16_t c,uint16_t d){
    uint16_t p[4];p[0]=a;p[1]=b;p[2]=c;p[3]=d;
    memcpy(&BkgPalette[slot],p,8);set_bkg_palette(slot,1,p);
}
void jam_dunk_cam_begin(const jam_game_t *g) BANKED {
    uint8_t id=g->setup.roster[g->passer];
    uint16_t bg=RGB(2,4,6),dark=RGB(3,5,7),skin=skins[id];
    memcpy(saved_palettes,BkgPalette,sizeof(saved_palettes));
    saved_overlay_actors=show_actors_on_overlay;show_actors_on_overlay=0;
    palette(1,bg,RGB(5,9,10),RGB(6,12,14),dark);
    palette(2,bg,skin,RGB(18,25,7),dark);
    palette(3,bg,skin,RGB(28,18,5),dark);
    palette(5,bg,RGB(31,30,24),RGB(28,18,5),bg);
    palette(6,bg,skin,g->passer<2?RGB(29,29,30):RGB(27,9,9),dark);
    VBK_REG=0;set_bkg_data(192,40,jam_dunk_font);
    last_frame=last_failed=255;jam_cam_active=1;court_restored=0;
    ui_set_pos(0,144);WY_REG=144;
}
static void restore_court(void){
    load_background(&bg_managua_prototype_court,BANK(bg_managua_prototype_court));
    /* Force a repaint without scroll_reset(), which also resets engine time. */
    pending_w_i=pending_h_i=0;scroll_x=scroll_y=0x7fff;scroll_update();
    court_restored=1;
}
void jam_dunk_cam_render(const jam_game_t *g) BANKED {
    uint8_t i,frame=g->dunk_time/2,failed=g->dunk_result>=JAM_DUNK_MISSED;
    int16_t x,y,relative;
    show_actors_on_overlay=g->dunk_time>=8 && g->dunk_time<=56;
    if(g->dunk_time<4){frame=0;failed=0;jam_cam_upload=0;jam_cam_map_target=1;}
    else if(g->dunk_time>=54){
        frame=27;jam_cam_upload=0;jam_cam_map_target=1;
    }else{
        jam_cam_upload=1+(frame&1);jam_cam_map_target=frame&1;
        WY_REG=0;
    }
    if(frame!=last_frame || failed!=last_failed){
        jam_dunk_art(g->setup.roster[g->passer],frame,failed,g->passer>=2);
        last_frame=frame;last_failed=failed;
    }
    if(g->dunk_time>=54 && !court_restored)restore_court();
    /* Retain wholly revealed court actors during the entry/exit wipe. */
    for(i=0;i<=6;i++){
        uint8_t edge=g->dunk_time<4?128-g->dunk_time*32:g->dunk_time>56?(g->dunk_time-56)*16:0;
        if(!edge || (actors[i].pos.y>>5)+8>=edge)actors[i].flags|=ACTOR_FLAG_HIDDEN;
    }
    /* One enlarged ball is still a real OBJ above the background athlete. */
    relative=jam_dunk_view_x(g,g->ball_x,jam_cam_avatar);
    x=116+(int32_t)relative*jam_cam_zoom/65536L;
    if(g->passer>=2)x=159-x;
    relative=jam_dunk_view_y(g,g->ball_y,g->height);
    y=((int32_t)relative*256-jam_cam_origin)*jam_cam_zoom/65536L;
    if(g->dunk_time>=8 && g->dunk_time<=56 && x>=8 && x<152 && y>=0 && y<120){
        actor_t *a=&actors[5];
        uint8_t large=jam_cam_zoom>=384;
        a->frame=a->frame_start=a->animations[0].start+large;a->frame_end=a->frame+1;a->anim_tick=255;
        a->flags=ACTOR_FLAG_ACTIVE|ACTOR_FLAG_PERSISTENT|ACTOR_FLAG_PINNED;
        a->pos.x=(x-(large?8:4))*32;a->pos.y=y*32;
    }
    if(g->dunk_result==JAM_DUNK_BLOCKED && g->dunk_defender<4 && g->dunk_time<g->dunk_block_time+16){
        uint8_t p=g->dunk_defender,id=g->setup.roster[p];
        actor_t *a=&actors[1];
        relative=jam_dunk_view_x(g,g->players[p].x,jam_cam_avatar);
        x=116+(int32_t)relative*jam_cam_zoom/65536L;if(g->passer>=2)x=159-x;
        relative=jam_dunk_view_y(g,g->players[p].y,jam_jump_height(g,p));
        y=((int32_t)relative*256-jam_cam_origin)*jam_cam_zoom/65536L;
        if(x>=8 && x<152 && y>=16 && y<176){
            a->frame=a->frame_start=a->animations[0].start+((p/2)*6+id)*20+(p>=2?10:0)+7;
            a->frame_end=a->frame+1;a->anim_tick=255;
            a->flags=ACTOR_FLAG_ACTIVE|ACTOR_FLAG_PERSISTENT|ACTOR_FLAG_PINNED;
            a->pos.x=(x-8)*32;a->pos.y=(y-8)*32;
        }
    }
    ui_set_pos(0,g->dunk_time<4?128-g->dunk_time*32:g->dunk_time>56?(g->dunk_time-56)*16:0);
}
void jam_dunk_cam_end(void) BANKED {
    uint8_t i;
    if(!jam_cam_active)return;
    /* Hide the cinematic before restoring palettes/font data. The caller
       reveals the ordinary HUD only after its tiles have been redrawn. */
    ui_set_pos(0,144);WY_REG=144;
    LCDC_REG|=LCDCF_WIN9C00;
    if(!court_restored)restore_court();
    memcpy(BkgPalette,saved_palettes,sizeof(saved_palettes));
    set_bkg_palette(0,7,(const uint16_t *)BkgPalette);
    VBK_REG=0;set_bkg_data(192,40,jam_font);
    for(i=0;i<20;i++)jam_cam_attrs[i]=7;
    VBK_REG=1;set_win_tiles(0,0,20,1,jam_cam_attrs);set_win_tiles(0,1,20,1,jam_cam_attrs);VBK_REG=0;
    jam_cam_active=0;
    show_actors_on_overlay=saved_overlay_actors;
}
