#ifdef __SDCC
#pragma bank 255
#endif
#include "jam_sim.h"

/* Off-ball play for CPU players while the ball is held (jam_sim.c's bank is
   nearly full). Attackers space to a rated spot, cut back door when their
   defender trails or helps, and lift toward a pressured carrier. Defenders
   split the ball and the other attacker; the helper sags toward the rim and
   steps up when the carrier beats his man. Positions are quarter pixels. */
static int16_t gap(int16_t a,int16_t b){return a>b?a-b:b-a;}
static int16_t near(const jam_game_t *g,uint8_t a,uint8_t b){
    return gap(g->players[a].x,g->players[b].x)+gap(g->players[a].y,g->players[b].y);
}
static const jam_ratings_t *rating(const jam_game_t *g,uint8_t p){return &jam_roster[g->setup.roster[p]].ratings;}
/* The rim a team attacks, and the x step from that rim into the court. */
static int16_t hoop(uint8_t team){return (team?JAM_RIM_LEFT:JAM_RIM_RIGHT)*4;}
static int16_t into(uint8_t team,int16_t px){return team?px*4:-px*4;}
/* Per-player cut timer and the wait before the next cut, in updates. */
static uint8_t cut[4],rest[4];
/* Which defender of each team guards the ball, and for which carrier. */
static uint8_t guard[2]={4,4},guarded[2]={4,4};

static int16_t clamp_y(int16_t y){
    if(y<(JAM_TOP+12)*4)return (JAM_TOP+12)*4;
    if(y>(JAM_BOTTOM-15)*4)return (JAM_BOTTOM-15)*4;
    return y;
}
uint8_t jam_cpu_cutting(uint8_t p) JAM_BANKED {return p<4 && cut[p];}
void jam_offball_reset(void) JAM_BANKED {
    uint8_t i;
    for(i=0;i<4;i++)cut[i]=rest[i]=0;
    guard[0]=guard[1]=guarded[0]=guarded[1]=4;
}
uint8_t jam_cpu_offball(jam_game_t *g,uint8_t p,int16_t *tx,int16_t *ty) JAM_BANKED {
    const jam_player_t *a=&g->players[p],*c;
    uint8_t team=p>>1,o,d,mark=4,low;
    int16_t rim,press=0x7fff,n,best=0x7fff;
    if(g->mode!=JAM_HELD || g->owner<0 || (g->owner>>1)!=team || g->owner==(int8_t)p)return 0;
    o=(uint8_t)g->owner;c=&g->players[o];rim=hoop(team);
    for(d=0;d<4;d++)if((d>>1)!=team && !g->players[d].stun){
        n=near(g,d,o);if(n<press)press=n;
        n=near(g,d,p);if(n<best){best=n;mark=d;}
    }
    if(rest[p])rest[p]--;
    /* Back door: cut when the man guarding him trails toward half court or
       turns to help on the ball, and now and then on a quick first step
       (CHRUX, speed 10, most often). */
    if(!cut[p] && !rest[p] && !((g->tick+p)&7) && gap(a->x,rim)>48*4){
        if(mark>3 || gap(g->players[mark].x,rim)>gap(a->x,rim)+48 ||
           near(g,mark,o)+32<best || jam_roll_percent(g)<rating(g,p)->speed)cut[p]=JAM_CUT_UPDATES;
    }
    /* Cut to the far side of the rim from the ball. */
    low=c->y<JAM_RIM_Y*4;
    if(cut[p]){
        *tx=rim+into(team,16);*ty=(low?JAM_RIM_Y+10:JAM_RIM_Y-10)*4;
        /* At the rim, post up briefly as a target before clearing out. */
        if(gap(a->x,*tx)<16 && gap(a->y,*ty)<16 && cut[p]>JAM_CUT_POST)cut[p]=JAM_CUT_POST;
        if(!--cut[p])rest[p]=JAM_CUT_REST;
        return 1;
    }
    if(press<JAM_CPU_PRESSURE && near(g,p,o)>96*4){
        /* Lift: come back toward a pressured carrier as an outlet. */
        *tx=c->x+into(team,40);
        *ty=clamp_y(c->y+(c->y<JAM_RIM_Y*4?40*4:-40*4));
        return 1;
    }
    /* Space away from the ball: shooters on the arc wing, the rest on the
       short corner by the baseline, on the half opposite the carrier. */
    low=c->y<JAM_RIM_Y*4;
    *tx=rim+into(team,rating(g,p)->three_point>=7?96:40);
    *ty=low?112*4:56*4;
    if(gap(*tx,c->x)+gap(*ty,c->y)<40*4)*ty=low?56*4:112*4;
    return 1;
}
/* The defender who takes the ball: the nearer one when the carrier changes,
   switching only when the other is clearly closer. Beside a human teammate,
   the CPU takes the other attacker unless the human is far from the ball. */
static uint8_t ball_guard(jam_game_t *g,uint8_t team,uint8_t o){
    uint8_t a=team*2,b=a+1,mate;
    if(team==0 && g->controlled<2){
        mate=g->controlled;
        return near(g,mate^1,o)+JAM_GUARD_SWITCH*2<near(g,mate,o)?mate^1:mate;
    }
    if(guarded[team]!=o || guard[team]>3){guarded[team]=o;guard[team]=near(g,a,o)<=near(g,b,o)?a:b;}
    else if(near(g,guard[team]^1,o)+JAM_GUARD_SWITCH<near(g,guard[team],o))guard[team]^=1;
    return guard[team];
}
uint8_t jam_cpu_defend(jam_game_t *g,uint8_t p,int16_t *tx,int16_t *ty) JAM_BANKED {
    uint8_t team=p>>1,o,man,bg;
    int16_t rim,gp;
    if(g->mode!=JAM_HELD || g->owner<0 || (g->owner>>1)==team)return 0;
    o=(uint8_t)g->owner;man=o^1;rim=hoop(team^1);
    bg=ball_guard(g,team,o);
    if(bg==p){
        gp=p>=2?80-g->setup.difficulty*16:48;
        *tx=g->players[o].x+(rim>g->players[o].x?gp:-gp);
        *ty=g->players[o].y+12;
        return 1;
    }
    /* Help: the carrier is inside 72 pixels and past his defender. */
    if(gap(g->players[o].x,rim)<72*4 && gap(g->players[bg].x,rim)>gap(g->players[o].x,rim)+32){
        *tx=g->players[o].x+(rim>g->players[o].x?64:-64);
        *ty=g->players[o].y;
        return 1;
    }
    /* A back-door cut beats him by a step: he reacts late, sooner on
       harder levels (CPU 24/16/8 updates; beside the human, 16). */
    if(jam_cpu_cutting(man) && JAM_CUT_UPDATES-cut[man]<(p>=2?24-g->setup.difficulty*8:16)){
        *tx=g->players[p].x;*ty=g->players[p].y;
        return 1;
    }
    /* Sag a third of the way from his man toward the rim, at most 12
       pixels, ball side: close enough that a cut can beat him by a step. */
    gp=(rim-g->players[man].x)/3;
    if(gp>JAM_SAG)gp=JAM_SAG;
    if(gp<-JAM_SAG)gp=-JAM_SAG;
    *tx=g->players[man].x+gp;
    *ty=g->players[man].y+(JAM_RIM_Y*4-g->players[man].y)/4;
    return 1;
}
