#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"
#include <string.h>

static const jam_ratings_t *ratings(const jam_game_t *g, uint8_t who) {
    return &jam_roster[g->setup.roster[who]].ratings;
}
static int8_t difficulty_bonus(const jam_game_t *g, uint8_t who) {
    return who<2 ? 0 : (int8_t)g->setup.difficulty-1;
}
static const uint8_t arc[33] = {0,5,9,13,17,21,24,27,30,32,34,36,38,39,40,41,42,41,40,39,38,36,34,32,30,27,24,21,17,13,9,5,0};
static int16_t distance(int16_t a, int16_t b) { return a > b ? a-b : b-a; }
/* Bounded court deltas fit 16-bit products, avoiding costly per-frame long division. */
static int16_t interpolate(int16_t from,int16_t to,uint8_t t,uint8_t duration) {
    uint16_t offset=(uint16_t)distance(from,to)*t/duration;
    return to<from ? from-offset : from+offset;
}
static uint8_t team(uint8_t p) { return p >> 1; }
static uint8_t random_byte(jam_game_t *g) {
    g->rng ^= (uint16_t)(g->rng << 7);
    g->rng ^= g->rng >> 9;
    g->rng ^= (uint16_t)(g->rng << 8);
    return (uint8_t)(g->rng >> 8);
}
static uint8_t random_percent(jam_game_t *g) {
    uint8_t roll;
    /* Reject the partial bucket: byte % 100 would favor low rolls. */
    do { roll=random_byte(g); } while(roll>=200);
    return roll%100;
}
static int16_t rim(uint8_t t) { return (t ? JAM_RIM_LEFT : JAM_RIM_RIGHT) * 4; }
static int16_t near_player(const jam_game_t *g, uint8_t a, uint8_t b) {
    return distance(g->players[a].x,g->players[b].x) + distance(g->players[a].y,g->players[b].y);
}
static void message(jam_game_t *g, uint8_t m) { g->message=m; g->message_time=75; }
static void inbound(jam_game_t *g, uint8_t t) {
    uint8_t i;
    static const uint8_t home_x[4]={100,132,188,236};
    static const uint8_t away_x[4]={132,84,220,188};
    for(i=0;i<4;i++) {
        g->players[i].x = (t ? away_x[i] : home_x[i])*4;
        g->players[i].y = (i & 1 ? 78 : 104)*4;
        g->players[i].cooldown=0;
        g->players[i].action=0;
        g->players[i].block_time=0;
        g->players[i].shot_time=g->players[i].windup=0;
        g->players[i].stun=g->players[i].contact_cooldown=g->players[i].reach_time=0;
        g->players[i].vx=g->players[i].vy=g->players[i].push_x=g->players[i].push_y=0;
        g->players[i].heading=i<2 ? JAM_FACE_RIGHT : JAM_FACE_LEFT;
        g->players[i].turbo=0;
    }
    g->owner=t ? 2 : 0;
    g->mode=JAM_HELD;
    g->height=0;
    g->grace=120;
    g->charge=0;
    g->possession_time=0;
    g->ball_x=g->players[g->owner].x+20;
    g->ball_y=g->players[g->owner].y;
    g->scored=0;
}
uint8_t jam_init_lineup(jam_game_t *g, const jam_setup_t *setup) JAM_BANKED {
    uint8_t i, used=0;
    if(!setup || setup->difficulty>JAM_WINNERS_STAY) return 0;
    for(i=0;i<4;i++) {
        if(setup->roster[i]>=JAM_ROSTER_COUNT || (used&(1u<<setup->roster[i]))) return 0;
        used|=1u<<setup->roster[i];
    }
    memset(g,0,sizeof(*g));
    g->setup=*setup;
    g->rng=0xace1;
    inbound(g,0);
    g->ball_x=g->players[0].x;
    g->ball_y=g->players[0].y;
    return 1;
}
void jam_init(jam_game_t *g) JAM_BANKED {
    static const jam_setup_t defaults={{0,1,2,3},JAM_PICKUP};
    jam_init_lineup(g,&defaults);
}
uint8_t jam_move_speed(const jam_game_t *g, uint8_t who, uint8_t turbo) JAM_BANKED {
    return 3+ratings(g,who)->speed/3+(turbo?2:0)+difficulty_bonus(g,who);
}
uint8_t jam_jump_height(const jam_game_t *g,uint8_t who) JAM_BANKED {
    const jam_player_t *p=&g->players[who];
    uint8_t t=who==0 ? g->charge : p->windup;
    if(p->shot_time) {
        if(p->shot_time>18) {
            t=28-p->shot_time;
            return p->launch_jump+(14-p->launch_jump)*t/10;
        }
        return p->shot_time*14/18;
    }
    if(t) return t<=6 ? 0 : (t>=13 ? 14 : (t-6)*2);
    if(p->block_time) {
        uint8_t total=8+ratings(g,who)->block;
        t=total-p->block_time;
        if(t>total/2)t=total-t;
        return t*14/(total/2);
    }
    return 0;
}
static void give_ball(jam_game_t *g,uint8_t p) {
    g->owner=p;
    g->mode=JAM_HELD;
    g->height=0;
    /* Let a catch or rebound become a playable possession before another swipe. */
    g->grace=p<2 ? (g->setup.difficulty==JAM_WARM_UP ? 90 : 60) : 45;
    g->possession_time=0;
    g->players[p].cooldown=30;
    g->players[p].windup=0;
}
void jam_pass(jam_game_t *g,uint8_t from,uint8_t to) JAM_BANKED {
    if(from>=4 || to>=4 || g->mode!=JAM_HELD || g->owner!=(int8_t)from || from==to || g->players[from].stun) return;
    g->from_x=g->ball_x=g->players[from].x;
    g->from_y=g->ball_y=g->players[from].y;
    g->target=to;
    g->target_x=g->players[to].x;
    g->target_y=g->players[to].y;
    g->owner=-1;
    g->passer=from;
    g->mode=JAM_PASS;
    g->flight=0;
    g->intercept_attempts=0;
    g->duration=28-ratings(g,from)->passing;
    /* Lower passing creates a small, catchable target error. */
    if(ratings(g,from)->passing<8) {
        int8_t error=(int8_t)(random_byte(g)%3)-1;
        g->target_y+=error*(10-ratings(g,from)->passing)*2;
    }
    g->passes++;
    g->players[from].action=8;
    message(g,JAM_MSG_PASS);
}
uint8_t jam_shot_chance(const jam_game_t *g,uint8_t who,uint8_t charge) JAM_BANKED {
    const jam_ratings_t *r=ratings(g,who);
    uint8_t i;
    int16_t d=distance(g->players[who].x,rim(team(who)))/4;
    int16_t depth=distance(g->players[who].y,JAM_RIM_Y*4)/2;
    int16_t chance;
    if((uint32_t)d*d+(uint32_t)depth*depth>88u*88u) {
        chance=12+r->three_point*6-(d>100?(d-100)/8:0);
    } else chance=d<48 ? 60+r->dunk*3 : 52+r->dunk*2;
    if(charge>=18 && charge<=36) chance+=10;
    for(i=0;i<4;i++) if(!g->players[i].stun && team(i)!=team(who) && near_player(g,i,who)<64)
        chance-=18+(int8_t)ratings(g,i)->power-(int8_t)r->power;
    /* Clutch matters in a close game late in the current possession. */
    if(distance(g->score[0],g->score[1])<=4 && g->possession_time>=720)
        chance+=((int8_t)r->clutch-5)*2;
    chance+=difficulty_bonus(g,who)*8;
    /* Opponents should miss open shots too, especially while learning. */
    if(who>=2) {
        uint8_t ceiling=g->setup.difficulty==JAM_WARM_UP ? 58 :
            (g->setup.difficulty==JAM_PICKUP ? 80 : 92);
        if(chance>ceiling) chance=ceiling;
    }
    if(chance<5)chance=5;
    if(chance>95)chance=95;
    return (uint8_t)chance;
}
void jam_shoot(jam_game_t *g,uint8_t who,uint8_t charge) JAM_BANKED {
    int16_t d,depth;
    if(who>=4 || g->mode!=JAM_HELD || g->owner!=(int8_t)who || g->players[who].stun) return;
    g->players[who].launch_jump=jam_jump_height(g,who);
    g->players[who].shot_time=28;
    g->players[who].windup=0;
    g->launch_height=jam_body(g,who)->height_px+g->players[who].launch_jump;
    g->height=g->launch_height;
    g->shot_team=team(who);
    g->from_x=g->ball_x=g->players[who].x+(who<2 ? 16 : -16);
    g->from_y=g->ball_y=g->players[who].y;
    g->target_x=rim(g->shot_team);
    g->target_y=JAM_RIM_Y*4;
    d=distance(g->players[who].x,g->target_x)/4;
    depth=distance(g->from_y,g->target_y)/2;
    g->shot_points=((uint32_t)d*d+(uint32_t)depth*depth)>88u*88u ? 3 : 2;
    g->block_attempts=0;
    g->shot_good=(who>=2 ? random_percent(g) : random_byte(g)%100)<jam_shot_chance(g,who,charge);
    g->mode=JAM_SHOT;
    g->owner=-1;
    g->passer=who;
    g->flight=0;
    g->duration=28+d/10;
    if(g->duration>48) g->duration=48;
    g->scored=0;
    g->players[who].action=22;
    g->shots++;
    message(g,JAM_MSG_SHOT);
}
static void move_to(jam_player_t *p,int16_t x,int16_t y,uint8_t speed) {
    uint8_t mx=distance(p->x,x)>speed,my=distance(p->y,y)>speed;
    if(mx&&my)speed=speed*3/4; /* Same diagonal normalization as the human. */
    if(mx)p->vx=p->x<x?speed:-(int8_t)speed;
    if(my)p->vy=p->y<y?speed:-(int8_t)speed;
    if(p->vx||p->vy)p->heading=my&&distance(p->y,y)>distance(p->x,x)?(p->vy<0?JAM_FACE_UP:JAM_FACE_DOWN):(p->vx<0?JAM_FACE_LEFT:JAM_FACE_RIGHT);
    p->x+=p->vx;p->y+=p->vy;
}
static int16_t in_front(const jam_game_t *g,uint8_t defender,uint8_t carrier) {
    switch(g->players[carrier].heading) {
        case JAM_FACE_LEFT:return g->players[carrier].x-g->players[defender].x;
        case JAM_FACE_DOWN:return g->players[defender].y-g->players[carrier].y;
        case JAM_FACE_UP:return g->players[carrier].y-g->players[defender].y;
        default:return g->players[defender].x-g->players[carrier].x;
    }
}
static void steal(jam_game_t *g,uint8_t p) {
    int16_t chance,reach;
    if(g->mode!=JAM_HELD || g->owner<0 || team(p)==team(g->owner) || g->players[p].stun || g->players[p].cooldown) return;
    if(p==0 ? g->possession_time<8 : g->grace) return;
    reach=p>=2 ? 44+ratings(g,p)->steal*2 : 56+ratings(g,p)->steal*4+(int8_t)jam_body(g,p)->height_px-40;
    if(distance(g->players[p].x,g->players[g->owner].x)>=reach ||
       distance(g->players[p].y,g->players[g->owner].y)>=28+ratings(g,p)->steal*2) return;
    g->players[p].cooldown=p==0 ? 30-ratings(g,p)->steal*2 : (p==1?65:120-g->setup.difficulty*30);
    g->players[p].reach_time=8;
    g->steal_attempts++;
    chance=28+ratings(g,p)->steal*5-ratings(g,g->owner)->passing*2-ratings(g,g->owner)->power;
    chance+=in_front(g,p,g->owner)>=0?20:-17;
    if(g->height<11)chance+=8;
    if(g->players[g->owner].windup)chance-=15;
    chance+=difficulty_bonus(g,p)*8;
    if(p>=2) chance=g->setup.difficulty==JAM_WARM_UP ? chance/3 :
        (g->setup.difficulty==JAM_PICKUP ? chance*2/3 : chance);
    if(chance<5)chance=5;
    if(chance>90)chance=90;
    if(random_percent(g)<chance) {
        give_ball(g,p);
        g->steals++;
        message(g,JAM_MSG_STEAL);
    } else if(p==0)message(g,JAM_MSG_REACH);
}
static void cpu(jam_game_t *g,uint8_t p) {
    jam_player_t *a=&g->players[p];
    int16_t tx,ty;
    uint8_t ownteam=team(p),mate=p^1,speed;
    uint8_t reaction=(p<2 || g->setup.difficulty==JAM_PICKUP)?7:(g->setup.difficulty==JAM_WARM_UP?15:3);
    if(g->mode==JAM_RESET || a->stun) return;
    if(g->mode==JAM_SHOT || g->mode==JAM_LOOSE || g->mode==JAM_FINISH) {
        if(g->mode==JAM_SHOT && team(g->passer)!=ownteam && g->flight<12 &&
           near_player(g,p,g->passer)<100 && (g->tick&reaction)==(p&reaction)) {
            a->block_time=8+ratings(g,p)->block;
            a->action=a->block_time;
        }
        tx=g->mode==JAM_SHOT ? g->target_x : g->ball_x;
        ty=g->mode==JAM_SHOT ? g->target_y : g->ball_y;
        tx+=(p&1 ? 32 : -32);
        ty+=(p&1 ? 28 : -20);
    } else if(g->mode==JAM_PASS) {
        if(p==g->target) return; /* Receiver holds a catchable target. */
        tx=g->ball_x; ty=g->ball_y;
        if(ownteam==team(g->passer)) return;
    } else if(g->owner==(int8_t)p) {
        tx=rim(ownteam)+(ownteam ? 72 : -72); ty=JAM_RIM_Y*4;
        if(a->windup) {
            if(++a->windup>=18) {jam_shoot(g,p,0);a->cooldown=65;}
            return;
        }
        if(!a->cooldown && distance(a->x,rim(ownteam))<180+ratings(g,p)->three_point*24+difficulty_bonus(g,p)*24) {
            /* Readable gather/rise before release, without a perfect-timing bonus. */
            a->windup=1; return;
        }
        if(!a->cooldown && near_player(g,p,ownteam?0:2)<80 && random_byte(g)<90) {
            jam_pass(g,p,mate); a->cooldown=50; return;
        }
    } else if(g->owner>=0 && team(g->owner)==ownteam) {
        tx=g->players[g->owner].x+(ownteam ? -128 : 128);
        ty=(g->players[g->owner].y<94*4 ? 112 : 76)*4;
    } else {
        uint8_t mark=(p&1) ? (ownteam?1:3) : (ownteam?0:2);
        tx=(g->players[mark].x+rim(ownteam^1))/2;
        ty=g->players[mark].y;
        if(g->owner>=0 && (p&1)==0) {
            uint8_t gap=p>=2 ? 80-g->setup.difficulty*16 : 48;
            tx=g->players[g->owner].x+(ownteam ? gap : -(int16_t)gap);
            ty=g->players[g->owner].y+12;
        }
        if((g->tick&reaction)==(p&reaction)) steal(g,p);
    }
    /* A power carrier can commit to a short, visible turbo charge. */
    if(g->mode==JAM_HELD && g->owner==(int8_t)p && ratings(g,p)->power>=8 &&
       !a->contact_cooldown && (g->tick&63)<24) a->turbo=1;
    speed=jam_move_speed(g,p,a->turbo);
    if(g->mode==JAM_HELD && g->owner>=0 && team(g->owner)!=ownteam &&
       ((tx>a->x && a->x>g->players[g->owner].x) ||
        (tx<a->x && a->x<g->players[g->owner].x)))speed=speed*3/4;
    move_to(a,tx,ty,speed);
    if(g->mode==JAM_HELD && g->owner>=0 && team(g->owner)!=ownteam)
        a->heading=g->players[g->owner].x<a->x?JAM_FACE_LEFT:JAM_FACE_RIGHT;
}
static void flight(jam_game_t *g) {
    uint8_t i,phase;
    int16_t d,best;
    g->flight++;
    g->ball_x=interpolate(g->from_x,g->target_x,g->flight,g->duration);
    g->ball_y=interpolate(g->from_y,g->target_y,g->flight,g->duration);
    phase=(uint16_t)g->flight*32/g->duration;
    if(g->mode==JAM_SHOT) g->height=interpolate(g->launch_height,JAM_RIM_HEIGHT+8,phase,32)+arc[phase]/3;
    else g->height=16+arc[phase]/6;
    if(g->mode==JAM_PASS && g->flight>3) {
        for(i=0;i<4;i++) if(!g->players[i].stun && (int8_t)i!=g->passer && team(i)!=team(g->passer)) {
            int16_t reach=20+ratings(g,i)->steal*3-ratings(g,g->passer)->passing;
            int16_t chance=45+ratings(g,i)->steal*4-ratings(g,g->passer)->passing*3;
            if(g->intercept_attempts&(1u<<i)) continue;
            /* Warm-up permits a clean release; real mid-lane contests still count. */
            if(i>=2 && g->setup.difficulty==JAM_WARM_UP) {
                if(g->flight<=g->duration/3 || g->flight>=g->duration-3) continue;
                reach=reach*3/4;
                chance/=2;
            }
            d=distance(g->players[i].x,g->ball_x)+distance(g->players[i].y,g->ball_y);
            if(d<reach) {
                g->intercept_attempts|=1u<<i;
                if(random_percent(g)<chance) { give_ball(g,i); g->steals++; message(g,JAM_MSG_STEAL); return; }
            }
        }
    }
    if(g->mode==JAM_SHOT && g->flight<14) {
        for(i=0;i<4;i++) if(!g->players[i].stun && team(i)!=team(g->passer) && g->players[i].block_time &&
            !(g->block_attempts&(1u<<i)) && near_player(g,i,g->passer)<80+ratings(g,i)->block*4+((int8_t)jam_body(g,i)->height_px-40)*4) {
            g->block_attempts|=1u<<i;
            if(random_byte(g)%100<10+ratings(g,i)->block*7+difficulty_bonus(g,i)*8) {
                g->shot_good=0;
                g->blocks++;
                message(g,JAM_MSG_BLOCK);
            }
        }
    }
    if(g->flight<g->duration) return;
    if(g->mode==JAM_PASS) {
        /* A failed interception cannot turn into a second automatic lane steal. */
        d=distance(g->players[g->target].x,g->ball_x)+distance(g->players[g->target].y,g->ball_y);
        if(d<80) { give_ball(g,g->target); return; }
        best=32767;
        for(i=0;i<4;i++) {
            d=distance(g->players[i].x,g->ball_x)+distance(g->players[i].y,g->ball_y);
            if(d<best) { best=d; g->target=i; }
        }
        if(best<80) give_ball(g,g->target);
        else { g->mode=JAM_LOOSE; g->flight=0; g->height=0; }
    } else {
        // Arrive ABOVE the rim. Points are awarded only after a visible swish.
        g->mode=JAM_FINISH;
        g->flight=0;
        g->height=JAM_RIM_HEIGHT+8;
    }
}
static void finish(jam_game_t *g) {
    uint8_t t=++g->flight;
    g->ball_y=JAM_RIM_Y*4;
    if(g->shot_good) {
        g->ball_x=rim(g->shot_team);
        g->height=JAM_RIM_HEIGHT+8-t;
        if(t==JAM_SCORE_FRAME && !g->scored) {
            g->score[g->shot_team]+=g->shot_points;
            if(g->score[g->shot_team]>99) g->score[g->shot_team]=99;
            g->baskets++;
            g->scored=1;
            message(g,g->shot_points==3 ? JAM_MSG_THREE : JAM_MSG_TWO);
        }
        if(t==JAM_FINISH_FRAMES) {
            g->mode=JAM_RESET;
            g->reset_time=24;
            g->next_team=g->shot_team^1;
        }
    } else {
        // Touch the rim, kick back into the court, then fall for a rebound.
        if(t<=8) g->height=JAM_RIM_HEIGHT+8-t;
        else {
            g->height=JAM_RIM_HEIGHT+(t<16 ? t-8 : 24-t);
            g->ball_x=rim(g->shot_team)+(g->shot_team ? (t-8)*6 : -(int16_t)(t-8)*6);
            if(t==9 && g->message!=JAM_MSG_BLOCK) message(g,JAM_MSG_MISS);
        }
        if(t>=24) {
            g->mode=JAM_LOOSE;
            g->flight=0;
            g->height=24;
        }
    }
}
void jam_update(jam_game_t *g,uint8_t input) JAM_BANKED {
    uint8_t i,j,pressed=(input^g->previous_input)&input,speed;
    int16_t d,best;
    g->previous_input=input;
    if(pressed&JAM_PAUSE) g->paused^=1;
    if(g->paused) return;
    g->tick++;
    if(g->message_time && !--g->message_time) g->message=JAM_MSG_PLAY;
    if(g->grace) g->grace--;
    if(g->block) g->block--;
    for(i=0;i<4;i++) {
        jam_player_t *a=&g->players[i];
        a->vx=a->vy=0;a->turbo=0;
        if(a->contact_cooldown)a->contact_cooldown--;
        if(a->reach_time)a->reach_time--;
        if(a->stun) {
            a->x+=a->push_x;a->y+=a->push_y;
            if(a->push_x)a->push_x+=a->push_x<0?1:-1;
            if(a->push_y)a->push_y+=a->push_y<0?1:-1;
            a->stun--;
        }
        if(g->players[i].cooldown) g->players[i].cooldown--;
        if(g->players[i].action) g->players[i].action--;
        if(g->players[i].block_time) g->players[i].block_time--;
        if(g->players[i].shot_time) g->players[i].shot_time--;
        if(g->mode!=JAM_HELD || g->owner!=(int8_t)i)g->players[i].windup=0;
    }
    if(g->mode==JAM_RESET) {
        if(!--g->reset_time) inbound(g,g->next_team);
        return;
    }
    if(!g->players[0].stun) {
        jam_player_t *a=&g->players[0];
        a->turbo=(input&JAM_TURBO)!=0;
        speed=jam_move_speed(g,0,a->turbo);
        if((input&(JAM_MOVE_LEFT|JAM_MOVE_RIGHT)) && (input&(JAM_MOVE_UP|JAM_MOVE_DOWN))) speed=(speed*3)/4;
        if(g->charge)speed=2;
        if(input&JAM_MOVE_LEFT)a->vx-=speed;
        if(input&JAM_MOVE_RIGHT)a->vx+=speed;
        if(input&JAM_MOVE_UP)a->vy-=speed;
        if(input&JAM_MOVE_DOWN)a->vy+=speed;
        if(a->vx)a->heading=a->vx<0?JAM_FACE_LEFT:JAM_FACE_RIGHT;
        else if(a->vy)a->heading=a->vy<0?JAM_FACE_UP:JAM_FACE_DOWN;
        a->x+=a->vx;a->y+=a->vy;
        if(pressed&JAM_A) {
            if(g->mode==JAM_HELD && g->owner==0)jam_pass(g,0,1);
            else if(g->mode==JAM_HELD && g->owner==1)jam_pass(g,1,0);
        }
        if((input&JAM_A) && g->mode==JAM_HELD && g->owner>=2) {
            if(input&JAM_TURBO)jam_shove(g,0);else steal(g,0);
        }
        if(g->mode==JAM_HELD && g->owner==0) {
            if(input&JAM_B) {
                if(g->charge<42)g->charge++;
                a->action=2;
            }
            if(g->charge && (!(input&JAM_B) || g->charge==42)) {
                jam_shoot(g,0,g->charge);g->charge=0;
            }
        } else {
            g->charge=0;
            if(pressed&JAM_B){g->block=8+ratings(g,0)->block;a->block_time=g->block;a->action=g->block;}
        }
    } else g->charge=0;
    for(i=1;i<4;i++)cpu(g,i);
    for(i=0;i<3;i++)for(j=i+1;j<4;j++)jam_contact(g,i,j);
    for(i=0;i<4;i++) {
        jam_player_t *a=&g->players[i];
        if(a->x<JAM_LEFT*4) a->x=JAM_LEFT*4;
        if(a->x>JAM_RIGHT*4) a->x=JAM_RIGHT*4;
        if(a->y<JAM_TOP*4) a->y=JAM_TOP*4;
        if(a->y>JAM_BOTTOM*4) a->y=JAM_BOTTOM*4;
    }
    if(g->mode==JAM_HELD) {
        g->ball_x=g->players[g->owner].x;
        g->ball_y=g->players[g->owner].y;
        switch(g->players[g->owner].heading) {
            case JAM_FACE_LEFT:g->ball_x-=32;break;
            case JAM_FACE_UP:g->ball_x+=12;g->ball_y-=12;break;
            case JAM_FACE_DOWN:g->ball_x+=12;g->ball_y+=12;break;
            default:g->ball_x+=32;break;
        }
        g->height=7+((g->tick&15)<8 ? (g->tick&7) : 7-(g->tick&7));
        i=g->owner==0 ? g->charge : g->players[g->owner].windup;
        if(i) {
            g->height=jam_body(g,g->owner)->height_px-13+(i<6 ? i : (i<12 ? (i-6)*2 : 13))+jam_jump_height(g,g->owner);
            g->ball_x=g->players[g->owner].x+(g->owner<2 ? 24 : -24);
        }
        if(++g->possession_time>1080) {
            g->next_team=team(g->owner)^1; g->mode=JAM_RESET; g->reset_time=30;
            message(g,JAM_MSG_CLOCK);
        }
    } else if(g->mode==JAM_PASS || g->mode==JAM_SHOT) flight(g);
    else if(g->mode==JAM_FINISH) finish(g);
    else if(g->mode==JAM_LOOSE) {
        g->flight++;
        if(g->height>2) g->height-=2; else g->height=0;
        best=32767;
        for(i=0;i<4;i++)if(!g->players[i].stun) {
            d=distance(g->players[i].x,g->ball_x)+distance(g->players[i].y,g->ball_y);
            if(d<best) { best=d; g->target=i; }
        }
        if(best<32767 && g->height<10+(jam_body(g,g->target)->height_px-36)/2 && best<64) { give_ball(g,g->target); g->rebounds++;
            if(g->message!=JAM_MSG_SHOVE || g->message_time<50)message(g,JAM_MSG_REBOUND); }
        /* Dead-ball recovery guarantees an unreachable rebound cannot stall play. */
        else if(g->flight>=180 && best<32767) { give_ball(g,g->target); message(g,JAM_MSG_REBOUND); g->rebounds++; }
    }
}
