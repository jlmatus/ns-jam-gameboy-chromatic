#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"

static int16_t gap(int16_t a,int16_t b){return a>b?a-b:b-a;}
static uint8_t walk(jam_player_t *p,int16_t x,int16_t y){
    int16_t dx=x-p->x,dy=y-p->y;
    p->vx=dx>5?5:dx<-5?-5:(int8_t)dx;
    p->vy=dy>5?5:dy<-5?-5:(int8_t)dy;
    if(p->vx && p->vy){
        if(p->vx>3)p->vx=3;else if(p->vx<-3)p->vx=-3;
        if(p->vy>3)p->vy=3;else if(p->vy<-3)p->vy=-3;
    }
    if(p->vx)p->heading=p->vx<0?JAM_FACE_LEFT:JAM_FACE_RIGHT;
    else if(p->vy)p->heading=p->vy<0?JAM_FACE_UP:JAM_FACE_DOWN;
    p->x+=p->vx;p->y+=p->vy;
    return gap(p->x,x)<6 && gap(p->y,y)<6;
}
void jam_tipoff(jam_game_t *g) JAM_BANKED {
    uint8_t i;
    static const uint8_t xs[4]={100,132,188,236};
    for(i=0;i<4;i++){
        g->players[i].x=xs[i]*4;g->players[i].y=(i&1?78:104)*4;
        g->players[i].heading=i<2?JAM_FACE_RIGHT:JAM_FACE_LEFT;
    }
    g->owner=g->controlled=0;g->mode=JAM_HELD;g->grace=120;
}
void jam_begin_inbound(jam_game_t *g,uint8_t next_team) JAM_BANKED {
    uint8_t i;
    g->mode=JAM_INBOUND;g->next_team=next_team;
    g->inbounder=next_team*2;g->inbound_receiver=g->inbounder^1;
    g->inbound_phase=g->inbound_time=g->inbound_pass=0;
    g->owner=-1;g->charge=g->block=0;g->possession_time=0;
    if(!next_team)g->controlled=g->inbounder;
    for(i=0;i<4;i++){
        g->players[i].windup=g->players[i].block_time=g->players[i].shot_time=0;
        g->players[i].stun=g->players[i].reach_time=g->players[i].contact_cooldown=0;
        g->players[i].push_x=g->players[i].push_y=0;
    }
    g->message=JAM_MSG_INBOUND;g->message_time=255;
}
void jam_inbound_return(jam_game_t *g) JAM_BANKED {
    jam_player_t *p=&g->players[g->inbounder];
    int16_t x=g->next_team?JAM_RIGHT*4:JAM_LEFT*4;
    /* Match the shortest 18-update pass without snapping the passer inbounds. */
    if(p->x<x){p->vx=4;p->x+=4;if(p->x>x)p->x=x;}
    else if(p->x>x){p->vx=-4;p->x-=4;if(p->x<x)p->x=x;}
    p->heading=g->next_team?JAM_FACE_LEFT:JAM_FACE_RIGHT;
}
void jam_inbound_update(jam_game_t *g,uint8_t pressed) JAM_BANKED {
    uint8_t i,ready=1;
    jam_player_t *p=&g->players[g->inbounder];
    int16_t outside=(g->next_team?306:14)*4;
    for(i=0;i<4;i++)if(i!=g->inbounder){
        int16_t x=(i==g->inbound_receiver?(g->next_team?256:64):(g->next_team?224:96))*4;
        int16_t y=(i==g->inbound_receiver?104:(i&1?112:80))*4;
        if(!walk(&g->players[i],x,y))ready=0;
    }
    if(!g->inbound_phase){
        if(g->height>2)g->height-=2;else g->height=0;
        if(walk(p,g->ball_x,g->ball_y) && !g->height){
            g->inbound_phase=1;g->owner=g->inbounder;
        }
    }else{
        if(!walk(p,outside,104*4))ready=0;
        p->heading=g->next_team?JAM_FACE_LEFT:JAM_FACE_RIGHT;
        g->ball_x=p->x+(g->next_team?-12:12);g->ball_y=p->y;g->height=18;
        if(ready){
            g->inbound_phase=2;
            if(g->inbound_time<200)g->inbound_time++;
            if((g->next_team && g->inbound_time>=36) ||
               (!g->next_team && ((pressed&JAM_A) || g->inbound_time>=180))){
                g->mode=JAM_HELD;
                jam_pass(g,g->inbounder,g->inbound_receiver);
                g->inbound_pass=1;g->scored=0;
            }
        }
    }
}
