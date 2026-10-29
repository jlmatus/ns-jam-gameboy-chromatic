#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"

static int16_t gap(int16_t a,int16_t b){return a>b?a-b:b-a;}
static int16_t hoop(uint8_t who){return (who<2?JAM_RIM_RIGHT:JAM_RIM_LEFT)*4;}
static const jam_ratings_t *rating(const jam_game_t *g,uint8_t p){return &jam_roster[g->setup.roster[p]].ratings;}
/* Cinematic staging must translate all foreground objects together. A short
   takeoff cannot snap a blocked ball or its defender to a rim-relative origin. */
int16_t jam_dunk_view_x(const jam_game_t *g,int16_t world_x,int16_t avatar_q8) JAM_BANKED {
    return avatar_q8+(world_x-g->players[g->passer].x)*(g->passer<2?64:-64);
}
int16_t jam_dunk_view_y(const jam_game_t *g,int16_t world_y,uint8_t height) JAM_BANKED {
    return 88+(world_y-g->players[g->passer].y)/4-height;
}
uint8_t jam_dunk_range(const jam_game_t *g,uint8_t who) JAM_BANKED {
    return 10+rating(g,who)->dunk*4;
}
uint8_t jam_dunk_chance(const jam_game_t *g,uint8_t who) JAM_BANKED {
    const jam_ratings_t *r=rating(g,who);
    int16_t chance=42+r->dunk*5+r->power/2;
    uint8_t i,ceiling=95;
    for(i=0;i<4;i++)if((i>>1)!=(who>>1) && !g->players[i].stun &&
        gap(g->players[i].x,g->players[who].x)+gap(g->players[i].y,g->players[who].y)<80)
        chance-=14+rating(g,i)->block-r->power;
    if(who>=2){
        chance+=((int8_t)g->setup.difficulty-1)*6;
        ceiling=g->setup.difficulty==JAM_WARM_UP?70:g->setup.difficulty==JAM_PICKUP?90:95;
    }
    if(chance<10)chance=10;
    if(chance>ceiling)chance=ceiling;
    return (uint8_t)chance;
}
uint8_t jam_try_dunk(jam_game_t *g,uint8_t who) JAM_BANKED {
    jam_player_t *p;
    uint8_t i;
    int16_t to_rim;
    if(who>=4 || g->mode!=JAM_HELD || g->owner!=(int8_t)who)return 0;
    p=&g->players[who];to_rim=hoop(who)-p->x;
    if(p->stun || gap(p->y,JAM_RIM_Y*4)>72 ||
       (who<2?to_rim<0:to_rim>0) || gap(to_rim,0)>jam_dunk_range(g,who)*4)return 0;
    /* Entering the cinematic skips the normal end-of-update clamp. A teammate
       may just have been pushed across a boundary earlier in this same update. */
    for(i=0;i<4;i++){
        if(g->players[i].x<JAM_LEFT*4)g->players[i].x=JAM_LEFT*4;
        if(g->players[i].x>JAM_RIGHT*4)g->players[i].x=JAM_RIGHT*4;
        if(g->players[i].y<JAM_TOP*4)g->players[i].y=JAM_TOP*4;
        if(g->players[i].y>JAM_BOTTOM*4)g->players[i].y=JAM_BOTTOM*4;
    }
    g->from_x=p->x;g->from_y=p->y;
    g->mode=JAM_DUNK;g->passer=who;g->owner=-1;
    g->shot_team=who>>1;g->shot_points=2;g->shot_good=g->scored=0;
    g->dunk_time=g->dunk_result=g->dunk_block_time=g->block_attempts=0;
    g->dunk_defender=255;g->dunk_contests=0;
    g->charge=g->block=0;p->windup=p->shot_time=0;
    p->heading=who<2?JAM_FACE_RIGHT:JAM_FACE_LEFT;
    g->ball_x=p->x+(who<2?24:-24);g->ball_y=p->y;
    g->height=jam_body(g,who)->height_px-10;
    g->message=JAM_MSG_DUNK;g->message_time=80;
    g->shots++;g->dunk_attempts++;
    return 1;
}
uint8_t jam_dunk_jump(const jam_game_t *g) JAM_BANKED {
    uint8_t t=g->dunk_time,peak=30+rating(g,g->passer)->dunk*2;
    uint8_t hang=JAM_RIM_HEIGHT+4-jam_body(g,g->passer)->height_px;
    if(t<8)return 0;
    if(t<20)return (t-8)*peak/12;
    if(t<28)return peak-(peak-hang)*(t-20)/8;
    if(g->dunk_result>=JAM_DUNK_MISSED)return t<40?hang*(40-t)/12:0;
    if(t<38)return hang;
    if(t<48)return hang*(48-t)/10;
    return 0;
}
void jam_dunk_update(jam_game_t *g,uint8_t pressed) JAM_BANKED {
    uint8_t t=++g->dunk_time,i,p=g->passer,travel;
    jam_player_t *a=&g->players[p];
    int16_t end=hoop(p)+(p<2?-32:32),reach;
    for(i=0;i<4;i++){
        g->players[i].vx=g->players[i].vy=0;
        if(g->players[i].block_time)g->players[i].block_time--;
    }
    travel=t<8?0:t<28?t-8:20;
    a->x=g->from_x+(end-g->from_x)*travel/20;
    a->y=g->from_y+(JAM_RIM_Y*4-g->from_y)*travel/20;
    if(!g->dunk_block_time){
        g->ball_x=a->x+(p<2?32:-32);g->ball_y=a->y;
        g->height=t<8?25:jam_body(g,p)->height_px+jam_dunk_jump(g)+(t<14?-1:t<24?5:4);
    }
    for(i=0;i<4;i++)if((i>>1)!=(p>>1) && !g->players[i].stun){
        reach=gap(g->players[i].x,a->x)+gap(g->players[i].y,a->y);
        if(t<24 && reach<128 && !(g->dunk_contests&(1u<<i)) &&
           ((i==g->controlled && (pressed&JAM_B)) || (i!=g->controlled && t>=8 && t<20))){
            g->players[i].block_time=8+rating(g,i)->block;
            g->dunk_contests|=1u<<i;
        }
        if(t>=9 && t<28 && g->players[i].block_time &&
           !(g->block_attempts&(1u<<i)) && reach<64+rating(g,i)->block*4+(jam_body(g,i)->height_px-36)*2 &&
           g->height<=jam_body(g,i)->height_px+jam_jump_height(g,i)+4+rating(g,i)->block/2){
            uint8_t chance=10+rating(g,i)->block*6;
            g->block_attempts|=1u<<i;
            if(!g->dunk_result && jam_roll_percent(g)<chance){
                g->dunk_result=JAM_DUNK_BLOCKED;g->blocks++;g->dunk_block_time=t;
                g->dunk_defender=i;
                g->target_x=g->ball_x;g->target_y=g->ball_y;g->launch_height=g->height;
                g->message=JAM_MSG_BLOCK;g->message_time=80;
            }
        }
    }
    if(t==JAM_DUNK_IMPACT && !g->dunk_result){
        g->shot_good=jam_roll_percent(g)<jam_dunk_chance(g,p);
        g->dunk_result=g->shot_good?JAM_DUNK_MADE:JAM_DUNK_MISSED;
    }
    if(g->dunk_block_time){
        uint8_t fall=t-g->dunk_block_time;
        g->ball_x=g->target_x+(p<2?-(int16_t)fall*5:(int16_t)fall*5);
        g->ball_y=g->target_y;
        g->height=g->launch_height>fall*3?g->launch_height-fall*3:0;
    }else if(t>=28){
        uint8_t fall=t-28;
        g->ball_x=hoop(p);g->ball_y=JAM_RIM_Y*4;
        if(g->dunk_result==JAM_DUNK_MISSED)g->ball_x+=p<2?-(int16_t)fall*4:(int16_t)fall*4;
        g->height=fall*2<JAM_RIM_HEIGHT+8?JAM_RIM_HEIGHT+8-fall*2:0;
    }
    if(t==JAM_DUNK_SCORE && g->dunk_result==JAM_DUNK_MADE && !g->scored){
        g->score[g->shot_team]+=2;if(g->score[g->shot_team]>99)g->score[g->shot_team]=99;
        g->baskets++;g->dunks++;g->scored=1;
    }
    if(t>=JAM_DUNK_END){
        a->cooldown=30;a->action=0;g->possession_time=0;
        if(g->dunk_result==JAM_DUNK_MADE)jam_begin_inbound(g,g->shot_team^1);
        else {g->mode=JAM_LOOSE;g->flight=0;g->grace=0;}
    }
}
