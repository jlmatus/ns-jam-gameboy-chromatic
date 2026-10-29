#pragma bank 255
#include "states/basketball.h"
#include "actor.h"
#include "camera.h"
#include "input.h"
#include "scroll.h"
#include "ui.h"
#include "jam_sim.h"
#include "jam_hud.h"
#include "jam_menu.h"
#include "jam_cards.h"
#include "palette.h"
#include <string.h>

jam_game_t jam;
static int16_t view_x;
jam_menu_t jam_menu;
static uint8_t in_menu;
static uint8_t menu_tiles[360];
static uint8_t hud[40];
static uint8_t previous_hud[40];
static int16_t old_x[4],old_y[4];
static uint8_t moving[4];
static uint8_t facing[4];
/* Keep render coordinates outside SDCC's overlaid automatic scratch storage. */
static volatile int16_t ball_screen_x,ball_screen_y;
static int16_t body_top[4];
static uint8_t body_visible[4];
static const uint8_t body_slots[4]={1,2,3,4};

static uint8_t glyph(char c) {
    if(c>='A' && c<='Z') return c-'A'+1;
    if(c>='0' && c<='9') return c-'0'+27;
    if(c==':') return 37;
    if(c=='>') return 38;
    if(c=='-') return 39;
    return 0;
}
static void text(uint8_t at,const char *s) { while(*s && at<40) hud[at++]=192+glyph(*s++); }
static void number(uint8_t at,uint8_t n) { hud[at]=192+27+n/10; hud[at+1]=192+27+n%10; }
static void draw_hud(void) {
    uint8_t i,changed=0;
    for(i=0;i<40;i++) hud[i]=192;
    text(0,"YOU"); number(4,jam.score[0]); text(7,":"); number(9,jam.score[1]); text(12,"CPU");
    if(jam.paused) text(20,"SELECT PLAY B LINEUP");
    else switch(jam.message) {
        case JAM_MSG_PASS:text(20,"PASS");break;
        case JAM_MSG_STEAL:text(20,"STEAL");break;
        case JAM_MSG_SHOT:text(20,"SHOT");break;
        case JAM_MSG_REBOUND:text(20,"REBOUND");break;
        case JAM_MSG_TWO:text(20,"SWISH - TWO POINTS");break;
        case JAM_MSG_THREE:text(20,"SWISH - THREE");break;
        case JAM_MSG_MISS:text(20,"OFF THE RIM");break;
        case JAM_MSG_BLOCK:text(20,"BLOCK");break;
        case JAM_MSG_CLOCK:text(20,"POSSESSION EXPIRED");break;
        case JAM_MSG_REACH:text(20,"REACH - GET IN FRONT");break;
        case JAM_MSG_SHOVE:text(20,"SHOVE");break;
        case JAM_MSG_RUNOVER:text(20,"POWER DRIVE");break;
        case JAM_MSG_STAND:text(20,"BODY HELD GROUND");break;
        default:
            if(jam.owner==0) text(20,"A PASS HOLD B SHOOT");
            else if(jam.owner==1) text(20,"A CALL FOR PASS");
            else text(20,(jam.tick&128)?"TURBO A SHOVE":"A STEAL  B BLOCK");
    }
    if(!jam.paused && jam.charge) text(20,jam.charge>=18 && jam.charge<=36 ? "RELEASE NOW         " : "HOLD B - SET SHOT   ");
    if(!jam.paused && jam.mode==JAM_FINISH && !jam.scored) {
        for(i=20;i<40;i++)hud[i]=192;
        text(20,jam.flight<8 ? "TO THE RIM" : (jam.shot_good ? "THROUGH THE NET" : "OFF THE RIM"));
    }
    if(!jam.paused && jam.owner>=0) text(16,jam.owner<2?"YOU":"CPU");
    if(!jam.paused && !jam.message_time && (jam.players[0].x*5/16<view_x || jam.players[0].x*5/16>view_x+160)) {
        for(i=20;i<40;i++)hud[i]=192;
        text(20,jam_roster[jam.setup.roster[0]].name);
        text(27,jam.players[0].x*5/16<view_x ? "GO RIGHT" : "GO LEFT");
    }
    for(i=0;i<40;i++)if(hud[i]!=previous_hud[i])changed=1;
    if(changed) {
        VBK_REG=0;
        set_win_tiles(0,0,20,2,hud);
        for(i=0;i<40;i++)previous_hud[i]=hud[i];
    }
}
static void place(actor_t *a,int16_t x,int16_t y) {
    a->flags=ACTOR_FLAG_ACTIVE|ACTOR_FLAG_PERSISTENT|ACTOR_FLAG_PINNED;
    a->anim_tick=255;
    a->frame_start=a->frame;a->frame_end=a->frame+1;
    if(x<-7 || x>159 || y<0 || y>127) a->flags|=ACTOR_FLAG_HIDDEN;
    a->pos.x=(x<0?0:x)*32;
    a->pos.y=y*32;
}
/* Logical court quarter-pixels -> pre-authored 5:4 horizontal projection. */
static int16_t project_x(int16_t x) {return x*5/16;}
static void draw_players(void) {
    uint8_t order[4]={0,1,2,3},i,j,p,tmp,frame,pose,slot,mirror,jump,wind;
    int16_t desired,focus,human,ball,dx,dy,scan,marker_top;
    human=project_x(jam.players[0].x);
    ball=project_x(jam.ball_x);
    focus=ball;
    if(jam.mode==JAM_HELD && jam.owner>=0)focus+=jam.owner<2 ? 12 : -12;
    desired=focus-80;
    /* Keep the controlled body and nearby ball together whenever they fit. */
    if(ball-human<128 && human-ball<128) {
        if(human<desired+16)desired=human-16;
        if(human>desired+144)desired=human-144;
    }
    if(desired<0)desired=0;
    if(desired>JAM_VIEW_WIDTH-160)desired=JAM_VIEW_WIDTH-160;
    if(!jam.paused) {
        if(view_x<desired){view_x+=4;if(view_x>desired)view_x=desired;}
        if(view_x>desired){view_x-=4;if(view_x<desired)view_x=desired;}
    }
    camera_x=(view_x+80)*32;camera_y=72*32;
    for(i=0;i<3;i++)for(j=i+1;j<4;j++)if(jam.players[order[j]].y>jam.players[order[i]].y) {
        tmp=order[i];order[i]=order[j];order[j]=tmp;
    }
    for(i=0;i<4;i++) {
        p=order[i];slot=body_slots[i];
        dx=jam.players[p].x-old_x[p];dy=jam.players[p].y-old_y[p];
        if(!jam.paused) {
            moving[p]=dx||dy;
            if(moving[p]) {
                facing[p]=jam.players[p].heading;
            } else if(jam.owner!=(int8_t)p && !jam.players[p].shot_time) {
                dy=jam.ball_y-jam.players[p].y;dx=jam.ball_x-jam.players[p].x;
                facing[p]=(dy>64||dy<-64) && (dx<80&&dx>-80) ? (dy<0?3:2) : (dx<0?1:0);
            }
        }
        wind=p==0?jam.charge:jam.players[p].windup;
        mirror=facing[p]==1;
        pose=moving[p]?1+((jam.tick>>3)&1):0;
        if(facing[p]>=2){pose=facing[p]==2?3:4;mirror=moving[p]?((jam.tick>>3)&1):0;}
        if(jam.players[p].reach_time){pose=5;mirror=jam.ball_x<jam.players[p].x;}
        if(jam.players[p].stun)pose=9;
        if(jam.players[p].action && jam.mode==JAM_PASS && jam.passer==(int8_t)p)pose=5;
        if(wind){pose=wind<6?5:6;mirror=p>=2;}
        if(jam.players[p].block_time){pose=jam_jump_height(&jam,p)>5?7:5;mirror=jam.ball_x<jam.players[p].x;}
        if(jam.players[p].shot_time){pose=jam.players[p].shot_time>22?7:(jam.players[p].shot_time>10?8:9);mirror=p>=2;}
        jump=jam_jump_height(&jam,p);
        frame=actors[slot].animations[0].start+((p/2)*6+jam.setup.roster[p])*20+mirror*10+pose;
        actors[slot].frame=actors[slot].frame_start=frame;actors[slot].frame_end=frame+1;
        place(&actors[slot],project_x(jam.players[p].x)-view_x-8,jam.players[p].y/4-8-jump);
        body_top[p]=(actors[slot].pos.y>>5)-40;
        body_visible[p]=!(actors[slot].flags&ACTOR_FLAG_HIDDEN);
        old_x[p]=jam.players[p].x;old_y[p]=jam.players[p].y;
    }
    actors[5].frame=actors[5].animations[0].start;
    actors[6].frame=actors[6].animations[0].start;
    ball_screen_x=jam.ball_x;
    ball_screen_x=ball_screen_x*5/16-view_x-4;
    ball_screen_y=jam.ball_y/4-jam.height;
    place(&actors[5],ball_screen_x,ball_screen_y);
    focus=human-view_x-3;if(focus<3)focus=3;if(focus>150)focus=150;
    place(&actors[6],focus,jam.players[0].y/4-jam_body(&jam,0)->height_px-6-jam_jump_height(&jam,0));
    focus=project_x((view_x<120?JAM_RIM_LEFT:JAM_RIM_RIGHT)*4)-view_x-4;
    pose=0;
    if(jam.mode==JAM_FINISH && jam.shot_good && jam.flight>=8)pose=jam.flight<20?1:2+(jam.tick&1);
    if(jam.mode==JAM_RESET && jam.scored && jam.reset_time>8)pose=2+((jam.tick>>2)&1);
    PLAYER.frame=PLAYER.animations[0].start+pose;
    PLAYER.frame_start=PLAYER.frame;PLAYER.frame_end=PLAYER.frame+1;
    place(&PLAYER,focus,JAM_RIM_Y-JAM_RIM_HEIGHT+8);
    /* Ball/net plus four two-column bodies consume ten objects. Only suppress
       the indicator when its actual scanlines would displace an athlete. */
    marker_top=(actors[6].pos.y>>5)-8;
    if(!((actors[5].flags|PLAYER.flags)&ACTOR_FLAG_HIDDEN)) {
        /* Eleven objects require a shared interval of all four bodies and the
           three single-column objects. Intersect intervals, not 144 scanlines. */
        scan=marker_top;dx=marker_top+16;
        if(scan<ball_screen_y-8)scan=ball_screen_y-8;
        if(dx>ball_screen_y+8)dx=ball_screen_y+8;
        if(scan<JAM_RIM_Y-JAM_RIM_HEIGHT)scan=JAM_RIM_Y-JAM_RIM_HEIGHT;
        if(dx>JAM_RIM_Y-JAM_RIM_HEIGHT+16)dx=JAM_RIM_Y-JAM_RIM_HEIGHT+16;
        for(j=0;j<4&&scan<dx;j++) {
            if(!body_visible[j]){scan=dx;break;}
            if(scan<body_top[j])scan=body_top[j];
            if(dx>body_top[j]+48)dx=body_top[j]+48;
        }
        if(scan<dx&&scan<128&&dx>0)actors[6].flags|=ACTOR_FLAG_HIDDEN;
    }
}
static void menu_text(uint8_t x,uint8_t y,const char *s) {
    uint16_t at=(uint16_t)y*20+x;
    while(*s && at<(uint16_t)(y+1)*20 && at<360)menu_tiles[at++]=192+glyph(*s++);
}
static void difficulty_text(uint8_t x,uint8_t y,uint8_t level) {
    menu_text(x,y,level==JAM_WARM_UP?"WARM-UP":level==JAM_PICKUP?"PICKUP":"WINNERS STAY");
}
static void draw_menu(void) {
    uint8_t i,y,attrs[20];
    uint16_t n;
    jam_menu_style();
    for(i=0;i<=6;i++)actors[i].flags|=ACTOR_FLAG_HIDDEN;
    if(jam_menu.stage<4) {
        jam_card_render(jam_menu.cursor,jam_menu.page,jam_menu.stage);
        ui_set_pos(0,0);return;
    }
    for(n=0;n<360;n++)menu_tiles[n]=192;
    if(jam_menu.stage==JAM_MENU_DIFFICULTY) {
        menu_text(2,2,"5 CPU DIFFICULTY");
        for(i=0;i<3;i++) {difficulty_text(3,5+i*2,i);if(jam_menu.cursor==i)menu_text(1,5+i*2,">");}
        menu_text(0,12,"WARM-UP - LEARNING");
        menu_text(0,13,"PICKUP - BALANCED");
        menu_text(0,14,"WINNERS - QUICK CPU");
        menu_text(0,16,"UP DOWN A CONFIRM");menu_text(0,17,"B BACK");
    } else {
        menu_text(3,2,"CHECK LINEUP");
        menu_text(0,4,"YOU");menu_text(11,4,jam_roster[jam_menu.setup.roster[0]].name);
        menu_text(0,6,"TEAMMATE");menu_text(11,6,jam_roster[jam_menu.setup.roster[1]].name);
        menu_text(0,8,"OPPONENT 1");menu_text(11,8,jam_roster[jam_menu.setup.roster[2]].name);
        menu_text(0,10,"OPPONENT 2");menu_text(11,10,jam_roster[jam_menu.setup.roster[3]].name);
        difficulty_text(3,13,jam_menu.setup.difficulty);
        menu_text(0,16,"A OR START TO PLAY");menu_text(0,17,"B CHANGE DIFFICULTY");
    }
    /* Menu palette 5 preserves the stock UI/font palette 7. */
    for(i=0;i<20;i++)attrs[i]=5;
    VBK_REG=1;
    for(y=0;y<18;y++)set_win_tiles(0,y,20,1,attrs);
    VBK_REG=0;set_win_tiles(0,0,20,18,menu_tiles);
    ui_set_pos(0,0);
}
static void start_lineup(void) {
    uint8_t i;
    if(!jam_init_lineup(&jam,&jam_menu.setup))return;
    in_menu=0;view_x=45;
    VBK_REG=0;set_bkg_data(192,40,jam_font);
    for(i=0;i<4;i++){facing[i]=i>=2?1:0;old_x[i]=jam.players[i].x;old_y[i]=jam.players[i].y;moving[i]=0;}
    for(i=0;i<40;i++){hud[i]=7;previous_hud[i]=255;}
    VBK_REG=1;set_win_tiles(0,0,20,2,hud);VBK_REG=0;
    ui_set_pos(0,128);
    draw_hud();draw_players();
}
void basketball_init(void) BANKED {
    uint8_t i;
    jam_init(&jam);
    view_x=45;
    camera_settings=0;
    camera_offset_x=camera_offset_y=0;
    for(i=0;i<4;i++){facing[i]=i>=2?1:0;old_x[i]=jam.players[i].x;old_y[i]=jam.players[i].y;moving[i]=0;}
    for(i=1;i<=6;i++) activate_actor(&actors[i]);
    // Native OAM order: net lip, ball, then depth-sorted bodies. Players cannot hide a swish.
    for(i=0;i<=6;i++) {
        actors[i].frame=actors[i].frame_start=actors[i].animations[0].start;
        actors[i].frame_end=actors[i].frame+1;
    }
    // The scene owns a two-row window; it does not run dialogue events.
    VBK_REG=0;
    set_bkg_data(192,40,jam_font);
    for(i=0;i<40;i++){hud[i]=7;previous_hud[i]=255;}
    VBK_REG=1;set_win_tiles(0,0,20,2,hud);VBK_REG=0;
    ui_set_pos(0,128);
    in_menu=1;jam_menu_init(&jam_menu,0);draw_menu();
}
void basketball_update(void) BANKED {
    uint8_t buttons=0;
    if(INPUT_LEFT) buttons|=JAM_MOVE_LEFT;
    if(INPUT_RIGHT) buttons|=JAM_MOVE_RIGHT;
    if(INPUT_UP) buttons|=JAM_MOVE_UP;
    if(INPUT_DOWN) buttons|=JAM_MOVE_DOWN;
    if(INPUT_A) buttons|=JAM_A;
    if(INPUT_B) buttons|=JAM_B;
    if(INPUT_START) buttons|=JAM_TURBO;
    if(INPUT_SELECT) buttons|=JAM_PAUSE;
    if(in_menu) {
        if(jam_menu_update(&jam_menu,buttons)) {start_lineup();jam.previous_input=buttons;}
        else if(jam_menu.changed)draw_menu();
        return;
    }
    if(jam.paused && INPUT_B_PRESSED) {
        jam_menu_init(&jam_menu,&jam.setup);jam_menu.previous_input=buttons;
        in_menu=1;draw_menu();return;
    }
    jam_update(&jam,buttons);
    draw_players();
    if((jam.tick&7)==0 || INPUT_SELECT_PRESSED || jam.paused) draw_hud();
}
