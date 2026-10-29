#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"
static const jam_ratings_t *ratings(const jam_game_t *g,uint8_t who) {
    return &jam_roster[g->setup.roster[who]].ratings;
}
static int16_t distance(int16_t a,int16_t b){return a>b?a-b:b-a;}
static uint8_t team(uint8_t p){return p>>1;}
static int16_t near_player(const jam_game_t *g,uint8_t a,uint8_t b) {
    return distance(g->players[a].x,g->players[b].x)+distance(g->players[a].y,g->players[b].y);
}
static void message(jam_game_t *g,uint8_t m){g->message=m;g->message_time=75;}
static int16_t in_front(const jam_game_t *g,uint8_t defender,uint8_t carrier) {
    switch(g->players[carrier].heading) {
        case JAM_FACE_LEFT:return g->players[carrier].x-g->players[defender].x;
        case JAM_FACE_DOWN:return g->players[defender].y-g->players[carrier].y;
        case JAM_FACE_UP:return g->players[carrier].y-g->players[defender].y;
        default:return g->players[defender].x-g->players[carrier].x;
    }
}
/* Mass is an arcade build class, never an invented real-world weight. */
static uint8_t strength(const jam_game_t *g,uint8_t p) {
    return ratings(g,p)->power+jam_body(g,p)->mass;
}
static void knock_back(jam_game_t *g,uint8_t from,uint8_t to,uint8_t duration) {
    jam_player_t *a=&g->players[from],*b=&g->players[to];
    int16_t dx=b->x-a->x,dy=b->y-a->y;
    b->stun=duration;b->contact_cooldown=duration+16;
    b->windup=b->block_time=b->shot_time=0;
    b->push_x=dx<0?-10:10;b->push_y=dy<0?-4:4;
    if(distance(a->y,b->y)>distance(a->x,b->x)) {
        b->push_x=dx<0?-4:4;b->push_y=dy<0?-8:8;
    }
    if(to==0)g->charge=0;
}
void jam_shove(jam_game_t *g,uint8_t p) JAM_BANKED {
    uint8_t i,target=255;
    int16_t best=120,d;
    jam_player_t *a=&g->players[p];
    if(a->stun || a->contact_cooldown || g->mode!=JAM_HELD ||
       g->owner<0 || team(g->owner)==team(p))return;
    for(i=0;i<4;i++)if(team(i)!=team(p) && !g->players[i].stun &&
        distance(a->y,g->players[i].y)<48) {
        d=near_player(g,p,i);
        if(d<best){best=d;target=i;}
    }
    if(target==255)return;
    a->contact_cooldown=40;a->reach_time=10;
    if(strength(g,p)+2<strength(g,target)) {message(g,JAM_MSG_STAND);return;}
    knock_back(g,p,target,16);
    if(g->owner==(int8_t)target) {
        g->ball_x=g->players[target].x+g->players[target].push_x*3;
        g->ball_y=g->players[target].y;
        g->height=14;g->mode=JAM_LOOSE;g->owner=-1;g->flight=0;
    }
    g->shoves++;message(g,JAM_MSG_SHOVE);
}
void jam_contact(jam_game_t *g,uint8_t i,uint8_t j) JAM_BANKED {
    jam_player_t *a=&g->players[i],*b=&g->players[j];
    uint8_t radius=(jam_body(g,i)->radius+jam_body(g,j)->radius)*4;
    uint8_t depth=24+(jam_body(g,i)->mass+jam_body(g,j)->mass)*2;
    uint8_t carrier=255,defender=0,yield,hold,amount;
    int16_t dx=distance(a->x,b->x),dy=distance(a->y,b->y);
    if(dx>=radius || dy>=depth || a->stun || b->stun ||
       jam_jump_height(g,i)>5 || jam_jump_height(g,j)>5)return;
    if(team(i)!=team(j) && g->mode==JAM_HELD) {
        if(g->owner==(int8_t)i){carrier=i;defender=j;}
        else if(g->owner==(int8_t)j){carrier=j;defender=i;}
        if(carrier!=255 && g->players[carrier].turbo &&
           !g->players[carrier].contact_cooldown && !g->players[defender].contact_cooldown &&
           in_front(g,defender,carrier)>0) {
            jam_player_t *c=&g->players[carrier],*d=&g->players[defender];
            uint8_t moving_into=(c->vx>0 && d->x>c->x)||(c->vx<0 && d->x<c->x)||
                (c->vy>0 && d->y>c->y)||(c->vy<0 && d->y<c->y);
            int16_t impact=strength(g,carrier)+ratings(g,carrier)->speed/4;
            int16_t resistance=strength(g,defender)+((d->vx||d->vy)?0:2);
            if(moving_into && impact>=resistance+3) {
                knock_back(g,carrier,defender,18);
                c->contact_cooldown=34;
                g->runovers++;message(g,JAM_MSG_RUNOVER);return;
            }
            if(moving_into && impact+2<resistance) {
                knock_back(g,defender,carrier,6);
                message(g,JAM_MSG_STAND);
            }
        }
    }
    /* Resolve overlap gradually. The old eight-pixel teleport defeated swipes. */
    yield=strength(g,i)>=strength(g,j)?j:i;hold=yield==i?j:i;
    if(dy>4 || dx<8) {
        amount=depth-dy;if(amount>4)amount=4;
        g->players[yield].y+=g->players[yield].y<g->players[hold].y?-(int8_t)amount:amount;
    } else {
        amount=radius-dx;if(amount>4)amount=4;
        g->players[yield].x+=g->players[yield].x<g->players[hold].x?-(int8_t)amount:amount;
    }
}
