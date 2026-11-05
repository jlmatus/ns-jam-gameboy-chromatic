#include "jam_sim.h"
#include "jam_menu.h"
#include "jam_audio.h"
#include "jam_results.h"
#include "jam_front.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void step(jam_game_t *g,int n,uint8_t buttons) { while(n--)jam_update(g,buttons); }
static void isolate(jam_game_t *g) {
    uint8_t i;
    for(i=1;i<4;i++) {g->players[i].x=(250+i*8)*4;g->players[i].y=70*4;g->players[i].cooldown=255;}
    g->grace=255;
}
static void movement_pause(void) {
    jam_game_t g,before;
    jam_init(&g);isolate(&g);
    g.owner=1;g.players[1].stun=255;
    step(&g,40,JAM_MOVE_RIGHT|JAM_MOVE_DOWN);
    assert(g.players[0].x==130*4 && g.players[0].y==JAM_BOTTOM*4);
    jam_update(&g,JAM_PAUSE);before=g;
    step(&g,60,JAM_MOVE_RIGHT|JAM_B);
    assert(g.players[0].x==before.players[0].x && g.tick==before.tick && g.charge==0);
    jam_update(&g,JAM_PAUSE);assert(!g.paused);
    g.players[0].x=40*4;g.players[0].y=60*4;
    step(&g,96,JAM_MOVE_LEFT|JAM_MOVE_UP);
    assert(g.players[0].x==JAM_LEFT*4);
    isolate(&g);g.players[0].x=JAM_LEFT*4+1;g.players[0].y=JAM_TOP*4+1;
    jam_update(&g,JAM_MOVE_LEFT|JAM_MOVE_UP);
    assert(g.players[0].x==JAM_LEFT*4 && g.players[0].y==JAM_TOP*4);
}
static void passing(void) {
    jam_game_t g;
    jam_init(&g);isolate(&g);
    g.players[1].x=120*4;g.players[1].y=100*4;
    jam_update(&g,JAM_A);assert(g.mode==JAM_PASS && g.owner==-1 && g.passes==1);
    step(&g,g.duration-1,0);assert(g.mode==JAM_HELD && g.owner==1);
    jam_update(&g,JAM_A);assert(g.mode==JAM_PASS && g.target==0);
    step(&g,g.duration-1,0);assert(g.owner==0 && g.passes==2);
    // An opponent in the passing lane can intercept; the passer's rating
    // decides how often. MAGIC (passing 5) is read far more than MATUS (10)
    // by the same MEAT (steal 8) lane.
    {
        static const jam_setup_t weak_setup={{1,0,5,3},JAM_PICKUP,4},strong_setup={{0,1,5,3},JAM_PICKUP,4};
        uint16_t seed,weak=0,strong=0;uint8_t k;
        for(seed=1;seed<=256;seed++)for(k=0;k<2;k++) {
            assert(jam_init_lineup(&g,k?&strong_setup:&weak_setup));isolate(&g);
            g.players[1].x=156*4;g.players[1].y=78*4;
            g.players[2].x=128*4;g.players[2].y=91*4;g.rng=seed;
            jam_update(&g,JAM_A);step(&g,g.duration-1,0);
            if(g.steals){assert(g.owner==2);if(k)strong++;else weak++;}
        }
        /* Expected 37% versus 12%. */
        assert(weak>70 && weak<120 && strong>10 && strong<50 && weak>strong*2);
        printf("PASS: lane interceptions by passer rating: MAGIC %u vs MATUS %u /256\n",weak,strong);
    }
}
static void court_edges(void) {
    jam_game_t g;
    uint8_t roster,corner,i;
    for(roster=0;roster<6;roster++)for(corner=0;corner<4;corner++) {
        jam_init(&g);isolate(&g);g.setup.roster[0]=roster;
        for(i=1;i<4;i++)g.players[i].stun=255;
        g.owner=1; /* Off-ball players may reach the apron without a whistle. */
        g.players[0].x=(corner&1?288:32)*4;
        g.players[0].y=(corner&2?116:70)*4;
        step(&g,96,(corner&1?JAM_MOVE_RIGHT:JAM_MOVE_LEFT)|(corner&2?JAM_MOVE_DOWN:JAM_MOVE_UP));
        assert(g.players[0].x==(corner&1?312:8)*4);
        assert(g.players[0].y==(corner&2?127:40)*4);
        assert(g.owner==1 && g.controlled==0);
    }
    /* Returning from a baseline inbound must enter the painted court,
       not stop at the newly expanded walkable apron. */
    for(i=0;i<2;i++) {
        jam_init(&g);g.next_team=i;g.inbounder=i*2;
        g.players[g.inbounder].x=(i?306:14)*4;
        for(corner=0;corner<24;corner++)jam_inbound_return(&g);
        assert(g.players[g.inbounder].x==(i?288:32)*4);
    }
    puts("PASS: all six players reach four painted-court/apron corners; inbound return stays inside baseline");
}
static void boundary_violations(void) {
    jam_game_t g,before;
    jam_audio_state_t audio;
    uint8_t r,p,edge,i;
    int n;
    static const int16_t xs[4]={160*4,160*4,24*4,295*4};
    static const int16_t ys[4]={40*4,124*4,88*4,88*4};
    for(r=0;r<6;r++)for(p=0;p<4;p++)for(edge=0;edge<4;edge++) {
        jam_init(&g);g.setup.roster[p]=r;g.owner=p;
        g.players[p].x=xs[edge];g.players[p].y=ys[edge];
        g.charge=24;g.players[p].windup=12;
        before=g;jam_audio_sync(&audio,&g);
        jam_update(&g,JAM_A|JAM_B|JAM_TURBO);
        assert(g.mode==JAM_RESET && g.message==JAM_MSG_OUT && g.owner==-1);
        assert(g.next_team==((p>>1)^1) && !g.charge && !g.players[p].windup);
        assert(g.passes==before.passes && g.shots==before.shots && !g.baskets && !g.steals);
        assert(!memcmp(g.stats,before.stats,sizeof(g.stats)));
        assert(jam_audio_event(&audio,&g)==JAM_SFX_WHISTLE);
        assert(jam_audio_event(&audio,&g)==JAM_SFX_NONE);
        before=g;jam_match_clock(&g,600);assert(!memcmp(&g,&before,sizeof(g)));
        jam_update(&g,JAM_PAUSE);before=g;step(&g,20,0);
        assert(g.reset_time==before.reset_time);
        jam_update(&g,JAM_PAUSE);step(&g,60,0);
        assert(g.mode==JAM_INBOUND && g.next_team==((p>>1)^1));
    }
    /* Legal corner space follows the slanted baselines, not a smaller box. */
    assert(jam_court_contains(18*4,42*4));
    assert(jam_court_contains(286*4,42*4));
    assert(jam_court_contains(31*4,122*4));
    assert(jam_court_contains(299*4,122*4));
    assert(!jam_court_contains(25*4,122*4));
    assert(!jam_court_contains(294*4,42*4));
    /* Crossing and pressing pass, or releasing a charged shot outside, cannot escape. */
    for(i=0;i<2;i++) {
        jam_init(&g);isolate(&g);g.players[0].y=40*4+1;
        if(i){g.charge=24;g.players[0].y=38*4;}
        jam_update(&g,JAM_MOVE_UP|(i?0:JAM_A));
        assert(g.mode==JAM_RESET && !g.passes && !g.shots);
    }
    /* A gathering shooter's feet are set, so arrows cannot carry him out. */
    jam_init(&g);isolate(&g);g.players[0].y=40*4+1;g.charge=10;
    jam_update(&g,JAM_MOVE_UP|JAM_B);
    assert(g.mode==JAM_HELD && g.players[0].y==40*4+1 && g.charge==11);
    /* An out-of-bounds receiver is whistled on the catch. */
    jam_init(&g);isolate(&g);g.players[1].x=120*4;g.players[1].y=124*4;
    jam_pass(&g,0,1);step(&g,g.duration,0);
    assert(g.mode==JAM_RESET && g.next_team==1 && g.message==JAM_MSG_OUT);
    /* Both legal inbounders can stand outside and complete their throw. */
    for(i=0;i<2;i++) {
        jam_init(&g);jam_begin_inbound(&g,i);jam_audio_sync(&audio,&g);
        for(n=0;n<900 && g.mode!=JAM_HELD;n++) {
            jam_update(&g,0);assert(jam_audio_event(&audio,&g)!=JAM_SFX_WHISTLE);
        }
        assert(g.mode==JAM_HELD && g.owner==i*2+1);
    }
    puts("PASS: all six profiles/four slots/four boundaries, skewed corners, exact-once whistle, clock freeze, no escape by pass/shot, outside catch and legal inbounds");
}
static void shots_rebounds_reset(void) {
    jam_game_t g;uint8_t points,scoring_team;int k;
    jam_init(&g);isolate(&g);g.players[0].x=245*4;
    step(&g,24,JAM_B);assert(g.mode==JAM_HELD && g.charge==24);
    jam_update(&g,0);assert(g.mode==JAM_SHOT && g.shot_points==2);
    g.shot_good=1;step(&g,g.duration-1,0);
    assert(g.mode==JAM_FINISH && g.score[0]==0 && g.height==JAM_RIM_HEIGHT+8);
    step(&g,8,0);assert(g.height==JAM_RIM_HEIGHT && g.score[0]==0);
    step(&g,JAM_SCORE_FRAME-9,0);assert(g.score[0]==0);
    step(&g,1,0);assert(g.score[0]==2 && g.baskets==1 && g.height<JAM_RIM_HEIGHT-4);
    step(&g,JAM_FINISH_FRAMES-JAM_SCORE_FRAME,0);
    assert(g.mode==JAM_INBOUND && g.score[0]==2 && g.baskets==1);
    for(k=0;k<600 && g.mode!=JAM_HELD;k++)jam_update(&g,0);
    assert(g.owner==3 && g.mode==JAM_HELD);
    // Exact three-point accounting and one basket per flight.
    jam_init(&g);isolate(&g);g.players[0].x=160*4;
    jam_shoot(&g,0,24);points=g.shot_points;scoring_team=g.shot_team;assert(points==3);
    g.shot_good=1;step(&g,g.duration+JAM_SCORE_FRAME,0);assert(g.score[scoring_team]==3);
    step(&g,20,0);assert(g.score[scoring_team]==3 && g.baskets==1);
    // A missed flight falls to the floor and is recovered by a nearby player.
    jam_init(&g);isolate(&g);g.players[0].x=245*4;
    jam_shoot(&g,0,24);g.shot_good=0;step(&g,g.duration+24,0);
    assert(g.mode==JAM_LOOSE && g.score[0]==0);
    for(k=0;k<240 && !g.rebounds;k++)jam_update(&g,0);
    assert(g.rebounds && g.owner>=0 && g.owner<4);
    // An unreachable ball cannot produce a live catch by an outside player.
    g.mode=JAM_LOOSE;g.owner=-1;g.ball_x=2000;g.ball_y=2000;g.flight=0;g.height=0;
    step(&g,180,0);assert(g.mode==JAM_RESET && g.message==JAM_MSG_OUT);
}
static void cpu_soak(void) {
    jam_game_t g;uint16_t i;uint8_t p;
    jam_init(&g);jam_pass(&g,0,1);
    for(i=0;i<30000;i++) {
        jam_update(&g,g.mode==JAM_HELD && g.owner==g.controlled && (i%48)<24?JAM_B:0);
        assert(g.mode<=JAM_DUNK);
        if(g.mode==JAM_HELD)assert(g.owner>=0 && g.owner<4);
        for(p=0;p<4;p++)assert(g.players[p].x>=JAM_LEFT*4 && g.players[p].x<=JAM_RIGHT*4 && g.players[p].y>=JAM_TOP*4 && g.players[p].y<=JAM_BOTTOM*4); /* legal apron; an inbounder may fetch a ball beyond 306 */
    }
    printf("CPU soak: shots=%u baskets=%u steals=%u rebounds=%u score=%u:%u\n",g.shots,g.baskets,g.steals,g.rebounds,g.score[0],g.score[1]);
    fflush(stdout);
    assert(g.shots>10 && g.baskets>0 && g.rebounds>0);
}
static void teammate_and_block(void) {
    jam_game_t g;
    jam_init(&g);isolate(&g);
    g.owner=1;g.setup.roster[0]=1;g.setup.roster[1]=0;g.players[1].x=245*4;g.players[1].cooldown=0;
    /* An open CPU teammate gathers (at a random moment) and releases 17 updates later. */
    g.players[2].x=g.players[3].x=60*4;
    {uint8_t t;for(t=0;t<60 && !g.players[1].windup;t++)jam_update(&g,0);}
    assert(g.mode==JAM_HELD && g.players[1].windup==1);
    step(&g,17,0);assert(g.mode==JAM_SHOT && g.passer==1 && g.shot_team==0);
    // Human B can disrupt an opponent's nearby early shot.
    jam_init(&g);g.owner=2;g.players[0].x=g.players[2].x;g.players[0].y=g.players[2].y;
    jam_shoot(&g,2,24);g.shot_good=1;
    g.rng=0xace1;
    jam_update(&g,JAM_B);assert(g.players[0].block_time>0 && g.block_attempts==1);
}
static void pause_during_swish(void) {
    jam_game_t g,before;
    jam_init(&g);jam_shoot(&g,0,24);g.shot_good=1;
    step(&g,g.duration+10,0);assert(g.mode==JAM_FINISH && !g.scored);
    jam_update(&g,JAM_PAUSE);before=g;
    step(&g,90,0);
    assert(g.height==before.height && g.flight==before.flight && g.score[0]==0);
    jam_update(&g,JAM_PAUSE);step(&g,30,0);
    assert(g.baskets==1 && g.score[0]==3);
}
static void painted_three_point_line(void) {
    jam_game_t g;
    jam_init(&g);g.players[0].x=200*4;g.players[0].y=112*4;
    jam_shoot(&g,0,24);assert(g.shot_points==3);
    jam_init(&g);g.players[0].x=210*4;g.players[0].y=112*4;
    jam_shoot(&g,0,24);assert(g.shot_points==2);
}
static void tap_menu(jam_menu_t *m,uint8_t key) {
    jam_menu_update(m,key);jam_menu_update(m,0);
}
static void selection_flow(void) {
    jam_menu_t m;
    jam_game_t g,before;
    uint8_t a,b,c,d,level,length,i, count=0;
    uint16_t combinations=0;
    jam_menu_init(&m,0);
    tap_menu(&m,JAM_MOVE_LEFT);assert(m.cursor==5);
    tap_menu(&m,JAM_MOVE_RIGHT);assert(m.cursor==0);
    /* Every ordered distinct lineup at every difficulty is reachable. */
    for(a=0;a<6;a++)for(b=0;b<6;b++)if(b!=a)
    for(c=0;c<6;c++)if(c!=a && c!=b)
    for(d=0;d<6;d++)if(d!=a && d!=b && d!=c)
    for(level=0;level<3;level++)for(length=0;length<3;length++) {
        uint8_t wanted[4]={a,b,c,d};
        jam_menu_init(&m,0);
        for(i=0;i<4;i++) {
            count=0;
            while(m.cursor!=wanted[i] && count++<6)tap_menu(&m,JAM_MOVE_RIGHT);
            assert(m.cursor==wanted[i]);tap_menu(&m,JAM_A);
        }
        assert(m.stage==JAM_MENU_DIFFICULTY);
        while(m.cursor!=level)tap_menu(&m,JAM_MOVE_DOWN);
        tap_menu(&m,JAM_A);assert(m.stage==JAM_MENU_PERIODS && m.cursor==2);
        while(m.cursor!=length)tap_menu(&m,JAM_MOVE_DOWN);
        tap_menu(&m,JAM_A);assert(m.stage==JAM_MENU_REVIEW);
        assert(jam_menu_update(&m,JAM_TURBO));
        assert(jam_init_lineup(&g,&m.setup));
        assert(!memcmp(g.setup.roster,wanted,4) && g.setup.difficulty==level);
        assert(g.setup.periods==(length==2?4:length+1));
        {int16_t start=g.players[0].x;jam_update(&g,JAM_MOVE_RIGHT);assert(g.players[0].x>start);}
        combinations++;
    }
    assert(combinations==3240);
    /* Backtracking skips earlier picks and requires review before play. */
    jam_menu_init(&m,0);tap_menu(&m,JAM_A);tap_menu(&m,JAM_A);
    tap_menu(&m,JAM_B);assert(m.stage==1 && m.cursor!=m.setup.roster[0]);
    tap_menu(&m,JAM_B);tap_menu(&m,JAM_MOVE_RIGHT);tap_menu(&m,JAM_A);
    assert(m.setup.roster[0]==1 && m.cursor!=1);
    jam_menu_update(&m,JAM_A);assert(m.stage==2);
    assert(!jam_menu_update(&m,JAM_A));assert(m.stage==2); /* Held A does not double-pick. */
    jam_init(&g);before=g;m.setup.roster[1]=m.setup.roster[0];
    assert(!jam_init_lineup(&g,&m.setup) && !memcmp(&g,&before,sizeof(g)));
    m.setup.roster[1]=255;assert(!jam_init_lineup(&g,&m.setup));
    m.setup.roster[1]=0;m.setup.difficulty=3;assert(!jam_init_lineup(&g,&m.setup));
    puts("PASS: 3240 lineup/difficulty/length combinations, no duplicates, backtracking, confirm, invalid setup");
}
static void ratings_pages(void) {
    jam_menu_t m;
    uint8_t player,page;
    for(player=0;player<6;player++) {
        jam_menu_init(&m,0);
        while(m.cursor!=player)tap_menu(&m,JAM_MOVE_RIGHT);
        for(page=1;page<=2;page++) {
            jam_menu_update(&m,JAM_PAUSE);assert(m.page==page && m.stage==0 && m.cursor==player);
            jam_menu_update(&m,JAM_PAUSE);assert(m.page==page); /* Held Select does not repeat. */
            jam_menu_update(&m,0);
        }
        tap_menu(&m,JAM_PAUSE);assert(m.page==0);
        tap_menu(&m,JAM_PAUSE);tap_menu(&m,JAM_B);
        assert(m.page==0 && m.stage==0 && m.cursor==player);
        tap_menu(&m,JAM_PAUSE);tap_menu(&m,JAM_A);
        assert(m.page==0 && m.stage==1 && m.setup.roster[0]==player);
        tap_menu(&m,JAM_B);assert(m.stage==0 && m.cursor==player);
    }
    puts("PASS: all six rating-page cycles, held Select, B close, A pick, page reset");
}
static void cpu_blocking_and_levels(void) {
    jam_game_t g;
    jam_setup_t setup={{0,2,1,3},JAM_PICKUP,4};
    uint16_t seed, cpu_blocks=0,total_rebounds=0;
    uint8_t level,p;
    for(seed=1;seed<=128;seed++) {
        jam_init_lineup(&g,&setup);
        g.players[0].x=g.players[2].x;g.players[0].y=g.players[2].y;
        g.tick=1;jam_shoot(&g,0,24);g.rng=seed;
        jam_update(&g,0);
        assert(g.players[2].block_time>0 && (g.block_attempts&4));
        cpu_blocks+=g.blocks;
    }
    assert(cpu_blocks>50);
    for(level=0;level<3;level++) {
        setup.difficulty=level;jam_init_lineup(&g,&setup);jam_pass(&g,0,1);
        for(seed=0;seed<12000;seed++) {
            jam_update(&g,g.mode==JAM_HELD && g.owner==g.controlled && (seed%48)<24?JAM_B:0);
            for(p=0;p<4;p++)assert(g.players[p].x>=JAM_LEFT*4 && g.players[p].x<=JAM_RIGHT*4 && g.players[p].y>=JAM_TOP*4 && g.players[p].y<=JAM_BOTTOM*4); /* legal apron; an inbounder may fetch a ball beyond 306 */
        }
        printf("Level %u soak: %u shots, %u baskets, %u rebounds, owner %d windup %u mode %u\n",level,g.shots,g.baskets,g.rebounds,g.owner,g.players[2].windup,g.mode);fflush(stdout);
        assert(g.shots>5 && g.baskets>0);total_rebounds+=g.rebounds;
    }
    assert(total_rebounds>0); /* Individual deterministic runs can contain only makes. */
    puts("PASS: CPU shot blocking and 12000-update soak at each difficulty");
}
static void selected(jam_game_t *g,uint8_t who) {
    jam_setup_t setup={{0,0,0,0},JAM_PICKUP,4};
    uint8_t i,slot=1;
    setup.roster[0]=who;
    for(i=0;i<6 && slot<4;i++)if(i!=who)setup.roster[slot++]=i;
    assert(jam_init_lineup(g,&setup));
    g->players[0].x=160*4;g->players[0].y=88*4;
    for(i=1;i<4;i++){g->players[i].x=40*4;g->players[i].y=70*4;g->players[i].cooldown=255;}
}
static void ratings_and_difficulty(void) {
    jam_game_t strong,weak;
    uint16_t seed,high_blocks=0,low_blocks=0,high_steals=0,low_steals=0;
    uint8_t baseline,i;
    static const uint8_t expected[6][8]={
        {7,10,10,8,2,3,5,9},{4,5,5,3,10,9,7,7},{4,5,6,4,4,7,10,7},
        {10,3,7,7,4,8,4,5},{5,8,5,3,7,7,9,6},{9,2,5,8,6,8,6,4}};
    for(i=0;i<6;i++)assert(!memcmp(&jam_roster[i].ratings,expected[i],8));
    assert(!strcmp(jam_roster[4].jersey,"74") && !strcmp(jam_roster[5].jersey,"00"));
    selected(&strong,3);selected(&weak,1);
    assert(jam_move_speed(&strong,0,0)>jam_move_speed(&weak,0,0));
    step(&strong,20,JAM_MOVE_RIGHT);step(&weak,20,JAM_MOVE_RIGHT);
    assert(strong.players[0].x>weak.players[0].x);
    selected(&strong,0);selected(&weak,5);
    assert(jam_shot_chance(&strong,0,24)>jam_shot_chance(&weak,0,24));
    baseline=jam_shot_chance(&strong,0,24);strong.possession_time=720;
    assert(jam_shot_chance(&strong,0,24)>baseline);
    strong.score[0]=20;assert(jam_shot_chance(&strong,0,24)==baseline);
    selected(&strong,1);selected(&weak,0);
    strong.players[0].x=weak.players[0].x=250*4;
    assert(jam_shot_chance(&strong,0,24)>jam_shot_chance(&weak,0,24));
    selected(&strong,0);selected(&weak,1);
    strong.players[1].x=weak.players[1].x=200*4;
    strong.players[1].y=weak.players[1].y=88*4;
    jam_pass(&strong,0,1);jam_pass(&weak,0,1);
    assert(strong.duration<weak.duration && strong.target_y==88*4);
    for(seed=1;seed<=256;seed++) {
        /* Same seeded close-range block attempts: MAGIC outblocks MATUS. */
        selected(&strong,1);selected(&weak,0);
        strong.owner=weak.owner=2;
        strong.players[2]=strong.players[0];weak.players[2]=weak.players[0];
        jam_shoot(&strong,2,24);jam_shoot(&weak,2,24);
        strong.rng=weak.rng=seed;strong.shot_good=weak.shot_good=1;
        jam_update(&strong,JAM_B);jam_update(&weak,JAM_B);
        high_blocks+=strong.blocks;low_blocks+=weak.blocks;
        /* MATUS steals more often than MAGIC against identical BAMBI power. */
        selected(&strong,0);selected(&weak,1);
        strong.owner=weak.owner=2;strong.grace=weak.grace=0;
        strong.possession_time=weak.possession_time=JAM_HUMAN_SWIPE_DELAY;
        strong.players[2]=strong.players[0];weak.players[2]=weak.players[0];
        strong.rng=weak.rng=seed;
        jam_update(&strong,JAM_A);jam_update(&weak,JAM_A);
        high_steals+=strong.steals;low_steals+=weak.steals;
    }
    assert(high_blocks>low_blocks && high_blocks>0 && low_blocks>0);
    assert(high_steals>low_steals && high_steals>0);
    selected(&strong,2);selected(&weak,3);
    strong.players[1]=strong.players[0];weak.players[1]=weak.players[0];
    baseline=strong.players[0].y/4;
    jam_update(&strong,0);jam_update(&weak,0);
    assert(strong.players[0].y==baseline*4); /* BAMBI holds position; CHRUX yields. */
    assert(weak.players[0].y!=baseline*4);
    jam_init(&strong);jam_init(&weak);
    strong.setup.difficulty=JAM_WINNERS_STAY;weak.setup.difficulty=JAM_WARM_UP;
    assert(jam_move_speed(&strong,2,0)>jam_move_speed(&weak,2,0));
    assert(jam_move_speed(&strong,0,0)==jam_move_speed(&weak,0,0));
    assert(jam_shot_chance(&strong,2,24)>jam_shot_chance(&weak,2,24));
    printf("PASS: all supplied ratings; speed, 3PT, pass, block %u>%u, steal %u>%u, finishing, power, clutch, difficulty\n",high_blocks,low_blocks,high_steals,low_steals);
}
static void warm_up_fairness(void) {
    jam_game_t easy,hard;
    uint16_t seed,easy_swipes=0,hard_swipes=0,easy_picks=0,hard_picks=0,cpu_makes=0;
    uint8_t i,baseline;
    jam_init(&easy);baseline=jam_shot_chance(&easy,0,24);
    easy.setup.difficulty=JAM_WARM_UP;
    assert(jam_shot_chance(&easy,0,24)==baseline); /* Preserve fun human shooting. */
    for(i=0;i<JAM_ROSTER_COUNT;i++) {
        uint8_t old;
        selected(&easy,i);old=easy.setup.roster[2];
        easy.setup.roster[2]=easy.setup.roster[0];easy.setup.roster[0]=old;
        easy.owner=2;easy.setup.difficulty=JAM_WARM_UP;
        easy.players[2].x=45*4;easy.players[2].y=88*4;
        assert(jam_shot_chance(&easy,2,0)<=58);
    }
    for(seed=1;seed<=512;seed++) {
        jam_init(&easy);isolate(&easy);easy.setup.difficulty=JAM_WARM_UP;
        easy.owner=2;easy.players[2].x=45*4;easy.players[2].y=88*4;
        easy.rng=seed;jam_shoot(&easy,2,0);cpu_makes+=easy.shot_good;
        jam_init(&easy);easy.setup.difficulty=JAM_WARM_UP;
        easy.grace=0;easy.tick=1;easy.rng=seed;
        easy.players[2]=easy.players[0];easy.players[2].x+=24;
        easy.players[1].x=260*4;easy.players[3].x=288*4;
        hard=easy;hard.setup.difficulty=JAM_WINNERS_STAY;
        jam_update(&easy,0);jam_update(&hard,0);
        easy_swipes+=easy.steals;hard_swipes+=hard.steals;
        jam_init(&easy);easy.setup.difficulty=JAM_WARM_UP;easy.rng=seed;
        easy.players[0].x=100*4;easy.players[0].y=88*4;
        easy.players[1].x=156*4;easy.players[1].y=88*4;
        easy.players[2].x=128*4;easy.players[2].y=88*4;
        easy.players[3].x=288*4;
        hard=easy;hard.setup.difficulty=JAM_WINNERS_STAY;
        jam_pass(&easy,0,1);jam_pass(&hard,0,1);
        step(&easy,easy.duration,0);step(&hard,hard.duration,0);
        easy_picks+=easy.steals;hard_picks+=hard.steals;
    }
    assert(easy_swipes>0 && easy_swipes<hard_swipes);
    assert(easy_picks>0 && easy_picks<hard_picks && hard_picks<512);
    assert(cpu_makes>230 && cpu_makes<330); /* Generous open CPU looks still miss. */
    jam_init(&easy);easy.setup.difficulty=JAM_WARM_UP;easy.grace=90;
    for(i=0;i<89;i++) {
        /* Keep this repeated-contact grace test away from the sideline. */
        easy.players[0].x=100*4;easy.players[0].y=88*4;
        easy.players[2]=easy.players[0];easy.players[2].cooldown=0;
        jam_update(&easy,0);assert(easy.owner==0 && !easy.steals);
    }
    /* A contested catch gets time to act; the passer cannot be stripped on release. */
    jam_init(&easy);easy.setup.difficulty=JAM_WARM_UP;isolate(&easy);
    easy.players[1].x=120*4;easy.players[1].y=100*4;
    jam_pass(&easy,0,1);step(&easy,easy.duration,0);
    assert(easy.owner==1 && easy.grace==90);
    easy.players[2]=easy.players[1];easy.players[2].cooldown=0;
    jam_update(&easy,JAM_A);assert(easy.mode==JAM_PASS && easy.passes==2);
    printf("PASS: Warm-up seeded swipes %u<%u, lane picks %u<%u, open CPU makes %u/512; catch protection and unchanged human shots\n",easy_swipes,hard_swipes,easy_picks,hard_picks,cpu_makes);
}
static void shooting_accuracy(void) {
    jam_game_t g;
    uint16_t seed;
    uint32_t makes[3]={0,0,0};
    uint8_t p,chance,open,deep,near,level;
    static const uint8_t roster[3]={1,4,0}; /* MAGIC, CHOMPA, MATUS. */
    static const uint8_t expected[3]={44,65,82};
    for(p=0;p<3;p++) {
        selected(&g,roster[p]);isolate(&g);
        g.players[0].x=190*4;g.players[0].y=88*4;
        g.setup.difficulty=JAM_WARM_UP;
        chance=jam_shot_chance(&g,0,24);assert(chance==expected[p]);
        for(seed=1;seed<65535;seed++) {
            /* Every nonzero 16-bit seed except one: catches modulo bias. */
            g.rng=seed;g.mode=JAM_HELD;g.owner=0;
            jam_shoot(&g,0,24);assert(g.shot_points==3);
            makes[p]+=g.shot_good;
        }
        assert(makes[p]*100>=(uint32_t)(chance-1)*65534);
        assert(makes[p]*100<=(uint32_t)(chance+1)*65534);
    }
    assert(makes[0]<makes[1] && makes[1]<makes[2]);
    selected(&g,1);isolate(&g);g.players[0].y=88*4;
    g.players[0].x=190*4;open=jam_shot_chance(&g,0,24);
    g.players[0].x=160*4;deep=jam_shot_chance(&g,0,24);
    g.players[0].x=50*4;assert(jam_shot_chance(&g,0,24)<deep && deep<open);
    g.players[0].x=260*4;near=jam_shot_chance(&g,0,24);assert(near>open);
    g.players[0].x=190*4;g.players[2]=g.players[0];
    assert(jam_shot_chance(&g,0,24)<open);
    g.players[2].x=280*4;
    assert(jam_shot_chance(&g,0,0)<open);
    for(level=0;level<3;level++) {g.setup.difficulty=level;assert(jam_shot_chance(&g,0,24)==open);}
    printf("PASS: 196602 seeded timed open threes: MAGIC %lu, CHOMPA %lu, MATUS %lu /65534; unbiased within 1 point of 44/65/82 percent; distance, pressure, timing, finishing, difficulty\n",(unsigned long)makes[0],(unsigned long)makes[1],(unsigned long)makes[2]);
}
static void fake_fixture(jam_game_t *g,uint8_t level,uint16_t seed) {
    static const jam_setup_t setup={{0,2,1,3},JAM_PICKUP,4};
    jam_setup_t s=setup;s.difficulty=level;
    assert(jam_init_lineup(g,&s));isolate(g);
    /* MAGIC (block 10) guards the human from ten pixels away. */
    g->players[2].x=g->players[0].x+40;g->players[2].y=g->players[0].y;
    g->rng=seed;
}
static void pump_fake_and_travel(void) {
    jam_game_t g,before,open;jam_audio_state_t audio;
    uint16_t seed,bites[3]={0,0,0},expected;int16_t x;uint8_t level,checked=0;
    /* A quick tap is a fake: no shot or jump, the dribble ends, feet stay set. */
    jam_init(&g);isolate(&g);
    step(&g,JAM_FAKE_FRAMES-1,JAM_B);jam_update(&g,0);
    assert(g.mode==JAM_HELD && g.owner==0 && !g.shots && !g.charge && !g.players[0].shot_time);
    assert(g.picked_up && g.fake_time==JAM_FAKE_SHOW && g.message==JAM_MSG_FAKE && !jam_jump_height(&g,0));
    step(&g,JAM_FAKE_SHOW,0);assert(g.mode==JAM_HELD && !g.fake_time && g.height==jam_body(&g,0)->height_px/2);
    /* Faking again and standing still stays legal. */
    step(&g,2,JAM_B);jam_update(&g,0);step(&g,120,0);
    assert(g.mode==JAM_HELD && g.owner==0 && !g.shots && g.picked_up>JAM_GATHER_STEP && g.height==jam_body(&g,0)->height_px/2);
    /* Any arrow press with a dead ball is traveling: whistle, CPU ball. */
    before=g;jam_audio_sync(&audio,&g);jam_update(&g,JAM_MOVE_RIGHT);
    assert(g.mode==JAM_RESET && g.next_team==1 && g.message==JAM_MSG_TRAVEL);
    assert(!g.picked_up && !g.shots && g.players[0].x==before.players[0].x);
    assert(jam_audio_event(&audio,&g)==JAM_SFX_WHISTLE);
    step(&g,30,0);assert(g.mode==JAM_INBOUND && g.next_team==1);
    /* Arrows held through the fake get a short gather step to stop... */
    jam_init(&g);isolate(&g);
    step(&g,3,JAM_MOVE_RIGHT|JAM_B);jam_update(&g,JAM_MOVE_RIGHT);
    assert(g.mode==JAM_HELD && g.picked_up==1);x=g.players[0].x;
    step(&g,JAM_GATHER_STEP-1,JAM_MOVE_RIGHT);assert(g.mode==JAM_HELD && g.players[0].x==x);
    jam_update(&g,JAM_MOVE_RIGHT);assert(g.mode==JAM_RESET && g.message==JAM_MSG_TRAVEL);
    /* ...stopping inside it is legal, but pressing again is a travel. */
    jam_init(&g);isolate(&g);
    step(&g,3,JAM_MOVE_RIGHT|JAM_B);step(&g,4,JAM_MOVE_RIGHT);step(&g,60,0);
    assert(g.mode==JAM_HELD && g.owner==0);
    jam_update(&g,JAM_MOVE_DOWN);assert(g.mode==JAM_RESET && g.message==JAM_MSG_TRAVEL);
    /* A real shot is still a hold of at least JAM_FAKE_FRAMES; feet stay set. */
    jam_init(&g);isolate(&g);x=g.players[0].x;
    jam_update(&g,JAM_B);step(&g,20,JAM_B|JAM_MOVE_LEFT);
    assert(g.mode==JAM_HELD && g.players[0].x==x);
    jam_update(&g,0);assert(g.mode==JAM_SHOT && g.shots==1 && !g.picked_up);
    /* Passing out of a fake revives play: the receiver dribbles and moves. */
    jam_init(&g);isolate(&g);g.players[1].x=120*4;g.players[1].y=100*4;
    step(&g,3,JAM_B);jam_update(&g,0);jam_update(&g,JAM_A);assert(g.mode==JAM_PASS);
    step(&g,g.duration,0);assert(g.owner==1 && g.controlled==1 && !g.picked_up);
    before=g;jam_update(&g,JAM_MOVE_RIGHT);
    assert(g.mode==JAM_HELD && g.players[1].x>before.players[1].x);
    /* Nearby defenders bite on a fake; better CPU levels read it more often. */
    for(level=0;level<3;level++)for(seed=1;seed<=400;seed++) {
        fake_fixture(&g,level,seed*977u);
        step(&g,3,JAM_B);jam_update(&g,0);
        assert(g.mode==JAM_HELD && !g.shots);
        if(!(g.bitten&4)){assert(!g.players[2].block_time);continue;}
        bites[level]++;
        assert(g.players[2].block_time==18 && g.bite_time==JAM_BITE_FRAMES);
        if(checked++)continue;
        /* A bitten defender stays in the air, stops contesting, and cannot block. */
        open=g;open.bitten=0;
        assert(jam_shot_chance(&g,0,24)>jam_shot_chance(&open,0,24));
        x=g.players[2].x;step(&g,8,JAM_B);assert(g.players[2].x==x);
        step(&g,14,JAM_B);jam_update(&g,0);
        assert(g.mode==JAM_SHOT && (g.block_attempts&4));
        step(&g,14,0);assert(!g.shot_blocked && !g.blocks);
    }
    for(level=0;level<3;level++) {
        expected=4*(65-level*10);
        assert(bites[level]+32>expected && bites[level]<expected+32);
    }
    assert(checked && bites[0]>bites[1] && bites[1]>bites[2]);
    /* The bite wears off: no stale commitment carries into the next shot. */
    fake_fixture(&g,0,977u);step(&g,3,JAM_B);jam_update(&g,0);
    step(&g,JAM_BITE_FRAMES,0);assert(!g.bitten && !g.bite_time);
    step(&g,20,JAM_B);jam_update(&g,0);assert(g.mode==JAM_SHOT && !g.block_attempts);
    printf("PASS: pump fake ends the dribble without a shot; travel on new arrows or past a %u-frame gather step; pass/shot from a dead ball; MAGIC bites %u/%u/%u of 400 fakes by level and cannot contest or block while committed\n",
        JAM_GATHER_STEP,bites[0],bites[1],bites[2]);
}
static void shooting_motion(void) {
    jam_game_t g,before;
    uint8_t i,last,jump;
    jam_init(&g);isolate(&g);
    step(&g,6,JAM_B);assert(g.charge==6 && jam_jump_height(&g,0)==0);
    last=0;
    for(i=7;i<=18;i++) {
        jam_update(&g,JAM_B);jump=jam_jump_height(&g,0);
        assert(jump>=last && jump-last<=2);last=jump;
    }
    assert(last==14 && g.height==jam_body(&g,0)->height_px+14); /* Ball rises with the hands; no dribbling. */
    jam_update(&g,0);assert(g.mode==JAM_SHOT && g.players[0].shot_time==28);
    assert(g.players[0].launch_jump==14 && g.launch_height==jam_body(&g,0)->height_px+14);
    g.shot_good=1;
    jam_update(&g,JAM_PAUSE);before=g;step(&g,30,0);
    assert(g.players[0].shot_time==before.players[0].shot_time);
    jam_update(&g,JAM_PAUSE);
    for(i=0;i<28;i++) {
        jam_update(&g,0);jump=jam_jump_height(&g,0);
        assert(jump<=last && last-jump<=1);last=jump;
    }
    assert(last==0 && !g.players[0].shot_time);
    jam_init(&g);isolate(&g);g.owner=1;g.setup.roster[0]=1;g.setup.roster[1]=0;
    g.players[1].x=245*4;g.players[1].cooldown=0;
    /* CPU shooters gather only with space and at a random moment; wait for it. */
    g.players[2].x=g.players[3].x=60*4;
    for(i=0;i<60 && !g.players[1].windup;i++)jam_update(&g,0);
    step(&g,11,0);assert(g.mode==JAM_HELD && g.players[1].windup==12);
    jam_pass(&g,1,0);jam_update(&g,0);assert(!g.players[1].windup);
    puts("PASS: gather/rise, attached ball, release/landing, paused animation, interruptible CPU windup");
}
static void defensive_fixture(jam_game_t *g,uint16_t seed,uint8_t behind) {
    jam_init(g);isolate(g);g->owner=2;g->grace=120;g->possession_time=JAM_HUMAN_SWIPE_DELAY;
    g->players[2].x=200*4;g->players[2].y=88*4;
    g->players[2].heading=JAM_FACE_LEFT;g->players[2].contact_cooldown=255;
    g->players[0].x=(behind?216:184)*4;g->players[0].y=88*4;
    g->rng=seed;
}
static void physical_defense(void) {
    jam_game_t a,b;uint16_t seed,front=0,behind=0;uint8_t i;
    static const uint16_t heights[6]={160,190,173,170,185,173};
    for(i=0;i<6;i++) {
        selected(&a,i);assert(jam_body(&a,0)->height_cm==heights[i]);
        assert(jam_body(&a,0)->weight_kg==0);
        jam_shoot(&a,0,24);assert(a.launch_height==jam_body(&a,0)->height_px);
    }
    for(seed=1;seed<=512;seed++) {
        defensive_fixture(&a,seed,0);defensive_fixture(&b,seed,1);
        jam_update(&a,JAM_A);jam_update(&b,JAM_A);
        front+=a.steals;behind+=b.steals;
        assert(a.steal_attempts==1 && b.steal_attempts==1);
        jam_update(&a,JAM_A);assert(a.steal_attempts==1); /* Holding cannot roll every frame. */
    }
    /* Front reaches still beat reaches from behind, but no longer win most possessions. */
    assert(front>behind+100 && front>180 && front<300);
    defensive_fixture(&a,1,0);a.possession_time=0;
    jam_update(&a,JAM_A);assert(a.steal_attempts==0);
    a.possession_time=JAM_HUMAN_SWIPE_DELAY-2;jam_update(&a,JAM_A);assert(a.steal_attempts==0);
    a.possession_time=JAM_HUMAN_SWIPE_DELAY;jam_update(&a,JAM_A);assert(a.steal_attempts==1);
    /* A deliberate shove uses power/build, makes a loose ball, and cannot repeat. */
    defensive_fixture(&a,1,0);a.setup.roster[2]=3;
    jam_update(&a,JAM_A|JAM_TURBO);
    assert(a.shoves==1 && a.mode==JAM_LOOSE && a.players[2].stun);
    assert(a.owner==-1 && a.players[2].windup==0);
    jam_update(&a,JAM_A|JAM_TURBO);assert(a.shoves==1 && a.owner!=2);
    step(&a,6,0);assert(a.message==JAM_MSG_SHOVE);
    step(&a,40,0);assert(!a.players[2].stun);
    defensive_fixture(&a,1,0); /* MATUS cannot casually shove BAMBI. */
    jam_update(&a,JAM_A|JAM_TURBO);assert(a.shoves==0 && !a.players[2].stun);
    /* BAMBI/CHOMPA power through a lean guard; a lean guard cannot reverse it. */
    for(i=0;i<3;i++) {
        selected(&a,i==0?2:i==1?4:3);
        a.setup.roster[2]=i==2?2:3;a.owner=0;
        a.players[0].x=160*4;a.players[0].y=88*4;
        a.players[2].x=172*4;a.players[2].y=88*4;
        jam_update(&a,JAM_MOVE_RIGHT|JAM_TURBO);
        assert(a.runovers==(i==2?0:1));
        assert(a.owner==0);
        if(i!=2)assert(a.players[2].stun && a.players[2].contact_cooldown);
        else assert(a.players[0].stun && a.message==JAM_MSG_STAND);
    }
    /* Equal ratings use the same diagonal speed on the human and CPU. */
    selected(&a,0);a.setup.roster[1]=0;a.mode=JAM_LOOSE;a.owner=-1;
    a.ball_x=260*4;a.ball_y=116*4;a.height=50;
    a.players[0].x=a.players[1].x=100*4;
    a.players[0].y=70*4;a.players[1].y=90*4;
    jam_update(&a,JAM_MOVE_RIGHT|JAM_MOVE_DOWN);
    assert(a.players[0].vx==a.players[1].vx && a.players[0].vy==a.players[1].vy);
    /* A retreating guard faces the ball and cannot backpedal at full sprint. */
    selected(&a,2);a.setup.roster[2]=3;
    a.players[0].x=160*4;a.players[0].y=88*4;
    a.players[2].x=172*4;a.players[2].y=90*4;
    jam_update(&a,0);
    assert(a.players[2].heading==JAM_FACE_LEFT);
    assert(a.players[2].vx<jam_move_speed(&a,2,0));
    /* Recovery is bounded, including corners, pause, and every real build. */
    for(i=0;i<6;i++) {
        selected(&a,i);a.players[0].x=JAM_LEFT*4;a.players[0].y=JAM_TOP*4;
        a.players[0].stun=18;a.players[0].push_x=-10;a.players[0].push_y=-8;
        jam_update(&a,JAM_PAUSE);b=a;step(&a,20,0);
        assert(a.players[0].stun==b.players[0].stun);
        jam_update(&a,JAM_PAUSE);step(&a,20,0);
        assert(!a.players[0].stun && a.players[0].x>=JAM_LEFT*4 && a.players[0].y>=JAM_TOP*4);
    }
    printf("PASS: real heights/builds; MATUS front swipes %u vs behind %u /512; early grace, held reach, shove/fumble, power drives, recovery, diagonal parity\n",front,behind);
}
static void contact_soak(void) {
    jam_game_t g;uint8_t human,level,p,input;uint16_t k,seed=2026;
    uint32_t attempts=0,shoves=0,drives=0,shots=0;
    for(human=0;human<6;human++)for(level=0;level<3;level++) {
        selected(&g,human);g.setup.difficulty=level;
        for(k=0;k<6000;k++) {
            seed=(uint16_t)(seed*109u+89u);
            input=(seed&1)?JAM_MOVE_RIGHT:JAM_MOVE_LEFT;
            if(seed&2)input|=(seed&4)?JAM_MOVE_UP:JAM_MOVE_DOWN;
            if(seed&8)input|=JAM_TURBO;
            if((k%80)<24)input|=JAM_A;
            if((k%96)>=48 && (k%96)<78)input|=JAM_B;
            jam_update(&g,input);
            assert(g.mode<=JAM_DUNK && g.controlled<2);
            if(g.mode==JAM_HELD && g.owner<2)assert(g.controlled==g.owner);
            if(g.mode==JAM_HELD)assert(g.owner>=0 && g.owner<4);
            else assert(g.owner==-1 || g.mode==JAM_RESET || g.mode==JAM_INBOUND);
            if(g.mode==JAM_PASS || g.mode==JAM_SHOT)assert(g.duration && g.flight<g.duration);
            for(p=0;p<4;p++) {
                assert(g.turbo_energy[p]<=JAM_TURBO_MAX && g.turbo_delay[p]<=JAM_TURBO_DELAY);
                assert(g.turbo_exhausted[p]<=1 && (g.turbo_energy[p] || g.turbo_exhausted[p]));
                assert(g.players[p].stun<=18 && g.players[p].contact_cooldown<=40);
                /* Everyone stays on the legal apron (8..312). An inbounder may
                   fetch a ball that went out beyond 306 before walking back. */
                assert(g.players[p].x>=JAM_LEFT*4 && g.players[p].x<=JAM_RIGHT*4);
                assert(g.players[p].y>=JAM_TOP*4 && g.players[p].y<=JAM_BOTTOM*4);
            }
        }
        attempts+=g.steal_attempts;shoves+=g.shoves;drives+=g.runovers;shots+=g.shots;
    }
    assert(attempts && shoves && drives && shots);
    printf("PASS: 108000 mixed-input updates across every human/level; %lu reaches, %lu shoves, %lu runovers, %lu shots; bounds/ownership/recovery intact\n",(unsigned long)attempts,(unsigned long)shoves,(unsigned long)drives,(unsigned long)shots);
}
static void possession_control_and_inbounds(void) {
    jam_game_t g,before;uint8_t i,t;uint16_t n;
    jam_init(&g);isolate(&g);g.players[1].x=120*4;g.players[1].y=100*4;
    jam_update(&g,JAM_A);assert(g.controlled==0 && g.mode==JAM_PASS);
    step(&g,g.duration-1,JAM_B);
    assert(g.owner==1 && g.controlled==1 && !g.charge);
    before=g;jam_update(&g,JAM_MOVE_RIGHT|JAM_B);
    assert(g.charge==1 && g.players[1].x-before.players[1].x==jam_move_speed(&g,1,0));
    step(&g,JAM_FAKE_FRAMES-1,JAM_B);jam_update(&g,0);assert(g.mode==JAM_SHOT && g.passer==1);
    jam_init(&g);isolate(&g);g.players[1].x=120*4;g.players[1].y=100*4;
    jam_update(&g,JAM_A);step(&g,g.duration+24,JAM_A);
    assert(g.passes==1 && g.owner==1 && g.controlled==1);
    jam_update(&g,0);jam_update(&g,JAM_A);
    assert(g.mode==JAM_PASS && g.passes==2 && g.target==0);
    for(t=0;t<2;t++) {
        jam_init(&g);g.ball_x=(t?JAM_RIM_RIGHT:JAM_RIM_LEFT)*4;
        g.ball_y=JAM_RIM_Y*4;g.height=24;g.score[t^1]=2;before=g;
        jam_begin_inbound(&g,t);
        for(i=0;i<4;i++)assert(g.players[i].x==before.players[i].x && g.players[i].y==before.players[i].y);
        assert(g.mode==JAM_INBOUND && g.owner==-1 && !g.charge);
        for(n=0;n<600 && g.inbound_phase!=2;n++)jam_update(&g,JAM_MOVE_RIGHT|JAM_B);
        assert(g.mode==JAM_INBOUND && g.inbound_phase==2 && g.owner==t*2);
        assert(t?g.players[g.inbounder].x>295*4:g.players[g.inbounder].x<24*4);
        assert(g.players[g.inbound_receiver].x==(t?256:64)*4 && !g.possession_time);
        assert(g.score[t^1]==2 && !g.shots);
        jam_update(&g,JAM_PAUSE);before=g;step(&g,30,0);
        assert(!memcmp(g.players,before.players,sizeof(g.players)) && g.inbound_time==before.inbound_time);
        jam_update(&g,JAM_PAUSE);jam_update(&g,0);
        if(t)for(n=0;n<40 && g.mode==JAM_INBOUND;n++)jam_update(&g,0);
        else jam_update(&g,JAM_A);
        assert(g.mode==JAM_PASS && g.inbound_pass && g.target==g.inbound_receiver);
        before=g;jam_update(&g,0);
        assert(g.players[g.inbounder].x-before.players[g.inbounder].x==(t?-4:4));
        for(n=0;n<50 && g.mode==JAM_PASS;n++) {
            /* The inbound throw is protected even with a defender on the ball. */
            i=t?0:2;g.players[i].x=g.ball_x;g.players[i].y=g.ball_y;
            jam_update(&g,JAM_A);
        }
        assert(g.mode==JAM_HELD && g.owner==g.inbound_receiver && !g.inbound_pass && !g.steals);
        assert(g.grace && g.players[g.inbounder].x>=JAM_LEFT*4 && g.players[g.inbounder].x<=JAM_RIGHT*4);
        if(!t)assert(g.controlled==1);
    }
    puts("PASS: catch-based control handoff/charge reset; visible retrieval/baseline inbound, protected throw, pause, return without teleport, both teams");
}
static void dunk_fixture(jam_game_t *g,uint8_t id,uint8_t who,uint16_t seed){
    uint8_t i,tmp;
    selected(g,id);
    tmp=g->setup.roster[who];g->setup.roster[who]=id;g->setup.roster[0]=tmp;
    for(i=0;i<4;i++){g->players[i].x=160*4;g->players[i].y=116*4;g->players[i].cooldown=255;}
    g->owner=who;g->controlled=who<2?who:0;
    g->players[who].x=(who<2?JAM_RIM_RIGHT-jam_dunk_range(g,who):JAM_RIM_LEFT+jam_dunk_range(g,who))*4;
    g->players[who].y=88*4;g->players[who].cooldown=0;g->rng=seed;
}
static void arcade_dunks(void){
    jam_game_t g,before,low;
    uint16_t seed,makes[6][2]={{0}},high_blocks=0,low_blocks=0;
    uint8_t id,side,t,p,who,peak;
    for(side=0;side<2;side++){
        who=side*2;dunk_fixture(&g,3,who,1);g.passer=who;
        g.players[who].x=(side?50:270)*4;g.players[who].y=70*4;
        assert(jam_dunk_view_x(&g,(side?42:278)*4,-42*256)==-34*256);
        assert(jam_dunk_view_x(&g,(side?40:280)*4,-42*256)==-32*256);
        assert(jam_dunk_view_y(&g,70*4,44)==44);
        assert(jam_dunk_view_y(&g,74*4,50)==42);
        g.players[who].x=(side?48:272)*4;g.players[who].y=88*4;
        assert(jam_dunk_view_x(&g,(side?40:280)*4,-8*256)==0);
        assert(jam_dunk_view_y(&g,88*4,52)==36);
    }
    for(id=0;id<6;id++)for(side=0;side<2;side++)for(seed=1;seed<=256;seed++){
        who=side*2;dunk_fixture(&g,id,who,seed);before=g;
        assert(jam_try_dunk(&g,who) && g.mode==JAM_DUNK && g.owner==-1 && g.shot_points==2);
        assert(g.dunk_attempts==1 && g.shots==1 && !g.baskets && !g.score[side]);
        peak=0;
        for(t=1;t<=JAM_DUNK_END;t++){
            jam_update(&g,0);
            if(t<20){uint8_t jump=jam_jump_height(&g,who);assert(jump>=peak);peak=jump;}
            if(t==20){peak=jam_jump_height(&g,who);assert(peak==30+jam_roster[id].ratings.dunk*2 && peak>14);}
            if(t<JAM_DUNK_SCORE)assert(!g.score[side] && !g.scored);
            if(t==JAM_DUNK_SCORE && g.dunk_result==JAM_DUNK_MADE)assert(g.height<JAM_RIM_HEIGHT && g.score[side]==2 && g.baskets==1);
            for(p=0;p<4;p++)if(p!=who)assert(g.players[p].x==before.players[p].x && g.players[p].y==before.players[p].y);
        }
        if(g.dunk_result==JAM_DUNK_MADE){makes[id][side]++;assert(g.mode==JAM_INBOUND && g.next_team==(side^1) && g.dunks==1 && g.baskets==1 && g.score[side]==2);}
        else assert(g.dunk_result==JAM_DUNK_MISSED && g.mode==JAM_LOOSE && !g.baskets && !g.dunks && !g.score[side]);
        assert(g.players[who].x==(who<2?JAM_RIM_RIGHT-8:JAM_RIM_LEFT+8)*4 && !jam_jump_height(&g,who));
    }
    for(id=0;id<6;id++)for(side=0;side<2;side++)assert(makes[id][side]>0 && makes[id][side]<256);
    assert(makes[1][0]>makes[0][0]);
    dunk_fixture(&g,1,0,1);g.players[1].x=JAM_LEFT*4-5;
    jam_update(&g,JAM_TURBO|JAM_B);assert(g.mode==JAM_DUNK && g.players[1].x==JAM_LEFT*4);
    step(&g,18,0);jam_update(&g,JAM_PAUSE);before=g;step(&g,100,JAM_MOVE_RIGHT|JAM_A|JAM_B);
    assert(g.dunk_time==before.dunk_time && g.rng==before.rng && g.ball_x==before.ball_x && g.height==before.height && !g.score[0]);
    jam_update(&g,JAM_PAUSE);assert(!g.paused && g.dunk_time==before.dunk_time+1);
    dunk_fixture(&g,1,0,1);jam_update(&g,JAM_B);assert(g.mode==JAM_HELD && g.charge==1 && !g.dunk_attempts);
    dunk_fixture(&g,1,0,1);g.players[0].x=200*4;jam_update(&g,JAM_TURBO|JAM_B);assert(g.mode==JAM_HELD && g.charge==1 && !g.dunk_attempts);
    dunk_fixture(&g,1,0,1);g.players[0].x=(JAM_RIM_RIGHT+1)*4;assert(!jam_try_dunk(&g,0));
    dunk_fixture(&g,1,0,1);g.players[0].y=JAM_BOTTOM*4;assert(!jam_try_dunk(&g,0));
    dunk_fixture(&g,1,0,1);g.players[0].stun=1;assert(!jam_try_dunk(&g,0));
    dunk_fixture(&g,1,1,1);g.controlled=0;step(&g,8,0);assert(g.mode==JAM_DUNK && g.passer==1);
    for(seed=1;seed<=512;seed++){
        dunk_fixture(&g,3,2,seed);g.players[0].x=(JAM_RIM_LEFT+24)*4;g.players[0].y=88*4;
        low=g;low.setup.roster[0]=0;low.setup.roster[1]=1;
        assert(jam_try_dunk(&g,2) && jam_try_dunk(&low,2));
        step(&g,7,0);step(&low,7,0);jam_update(&g,JAM_B);jam_update(&low,JAM_B);
        step(&g,56,0);step(&low,56,0);
        high_blocks+=g.blocks;low_blocks+=low.blocks;
        if(g.blocks)assert(g.dunk_result==JAM_DUNK_BLOCKED && g.mode==JAM_LOOSE && !g.score[1] && !g.dunks);
    }
    assert(high_blocks>low_blocks && low_blocks>0);
    /* A late reach cannot hit a ball above the defender's real vertical reach.
       MAGIC can still meet the descending finish; MATUS's jump expires below it. */
    dunk_fixture(&g,3,2,1);g.players[0].x=(JAM_RIM_LEFT+24)*4;g.players[0].y=88*4;
    low=g;low.setup.roster[0]=0;low.setup.roster[1]=1;
    assert(jam_try_dunk(&g,2) && jam_try_dunk(&low,2));
    step(&g,15,0);step(&low,15,0);jam_update(&g,JAM_B);jam_update(&low,JAM_B);
    step(&g,48,0);step(&low,48,0);
    assert(g.blocks==1 && g.dunk_defender==0 && g.dunk_result==JAM_DUNK_BLOCKED);
    assert(!low.blocks && !(low.block_attempts&1));
    printf("PASS: 3072 dunk attempts across every player/basket; MAGIC %u vs MATUS %u home makes; block timing MAGIC %u vs MATUS %u /512; turbo trigger, range/depth, fallback shot, CPU dunk, pause, one score below rim, misses/blocks, preserved positions, landing/inbound\n",makes[1][0],makes[0][0],high_blocks,low_blocks);
}
static void turbo_system(void) {
    jam_game_t g,walk,before;
    uint8_t p,id,n;
    for(id=0;id<6;id++) {
        selected(&g,id);isolate(&g);
        for(p=1;p<4;p++)g.players[p].stun=255;
        for(p=0;p<4;p++)assert(g.turbo_energy[p]==100 && !g.turbo_exhausted[p]);
        step(&g,20,JAM_TURBO);assert(g.turbo_energy[0]==100 && !g.players[0].turbo);
        walk=g;jam_update(&walk,JAM_MOVE_LEFT);jam_update(&g,JAM_MOVE_LEFT|JAM_TURBO);
        assert(walk.players[0].x-g.players[0].x==2 && g.turbo_energy[0]==99);
        /* Shuttle inside the court while testing reserve exhaustion. */
        for(n=0;n<99;n++)jam_update(&g,((n/20)&1?JAM_MOVE_LEFT:JAM_MOVE_RIGHT)|JAM_TURBO);
        assert(!g.turbo_energy[0] && g.turbo_exhausted[0]);
        for(n=0;n<100;n++)jam_update(&g,((n/20)&1?JAM_MOVE_LEFT:JAM_MOVE_RIGHT)|JAM_TURBO);
        assert(!g.turbo_energy[0] && g.turbo_exhausted[0] && !g.players[0].turbo);
        step(&g,12,0);assert(g.turbo_energy[0]==24 && g.turbo_exhausted[0]);
        jam_update(&g,0);assert(g.turbo_energy[0]==26 && !g.turbo_exhausted[0]);
        step(&g,60,0);assert(g.turbo_energy[0]==100);
    }
    jam_init(&g);isolate(&g);g.turbo_energy[0]=50;
    jam_update(&g,JAM_MOVE_LEFT|JAM_TURBO);assert(g.turbo_energy[0]==49);
    step(&g,19,0);assert(g.turbo_energy[0]==49);
    jam_update(&g,0);assert(g.turbo_energy[0]==51);
    jam_update(&g,JAM_PAUSE);before=g;step(&g,100,JAM_TURBO|JAM_MOVE_RIGHT);
    assert(!memcmp(g.turbo_energy,before.turbo_energy,4) && !memcmp(g.turbo_delay,before.turbo_delay,4));
    assert(!memcmp(g.turbo_exhausted,before.turbo_exhausted,4));
    jam_update(&g,JAM_PAUSE);
    for(p=0;p<4;p++){g.turbo_energy[p]=40;g.turbo_delay[p]=0;}
    jam_begin_inbound(&g,0);step(&g,10,JAM_TURBO);
    for(p=0;p<4;p++)assert(g.turbo_energy[p]==60);
    /* Catching changes whose reserve is used; it never transfers/refills it. */
    jam_init(&g);isolate(&g);g.players[1].x=120*4;g.players[1].y=100*4;
    g.turbo_energy[0]=0;g.turbo_exhausted[0]=1;g.turbo_energy[1]=32;
    jam_update(&g,JAM_A|JAM_TURBO);step(&g,g.duration-1,JAM_TURBO);
    assert(g.controlled==1 && !g.turbo_energy[0] && g.turbo_energy[1]>32 && g.turbo_energy[1]<100);
    before=g;jam_update(&g,JAM_TURBO);assert(g.turbo_energy[1]==before.turbo_energy[1]);
    /* A low reserve cannot be bypassed through stationary combo actions. */
    dunk_fixture(&g,1,0,1);g.turbo_energy[0]=11;
    assert(!jam_try_dunk(&g,0) && !g.dunk_attempts && g.turbo_energy[0]==11);
    g.turbo_energy[0]=12;assert(jam_try_dunk(&g,0));
    assert(!g.turbo_energy[0] && g.turbo_exhausted[0]);before=g;
    step(&g,JAM_DUNK_END,JAM_TURBO);
    assert(!memcmp(g.turbo_energy,before.turbo_energy,4) && !memcmp(g.turbo_delay,before.turbo_delay,4));
    dunk_fixture(&g,1,0,1);g.turbo_energy[0]=0;g.turbo_exhausted[0]=1;
    jam_update(&g,JAM_TURBO|JAM_B);assert(g.mode==JAM_HELD && g.charge==1 && !g.dunk_attempts);
    defensive_fixture(&g,1,0);g.setup.roster[2]=3;g.turbo_energy[0]=7;
    jam_shove(&g,0);assert(!g.shoves && !g.players[2].stun && g.turbo_energy[0]==7);
    g.turbo_energy[0]=8;jam_shove(&g,0);
    assert(g.shoves==1 && g.players[2].stun && !g.turbo_energy[0] && g.turbo_exhausted[0]);
    /* CPU power charges and finishing obey the same reserve. */
    dunk_fixture(&g,2,2,1);g.players[2].x=160*4;g.players[2].cooldown=100;
    jam_update(&g,0);assert(g.players[2].turbo && g.turbo_energy[2]==99);
    g.turbo_energy[2]=0;g.turbo_exhausted[2]=1;g.tick=0;
    jam_update(&g,0);assert(!g.players[2].turbo && !g.turbo_energy[2]);
    dunk_fixture(&g,1,1,1);g.controlled=0;g.turbo_energy[1]=0;g.turbo_exhausted[1]=1;
    g.turbo_delay[1]=JAM_TURBO_DELAY;
    for(n=0;n<18;n++)jam_update(&g,0);
    assert(g.mode==JAM_SHOT && !g.dunk_attempts);
    assert(!jam_turbo_spend(&g,255,1) && !jam_turbo_available(&g,255));
    puts("PASS: turbo reserves for six profiles; real speed boost, full depletion, held-empty lockout, delayed/capped recovery, 25-percent rearm, pause/cinematic freeze, inbounds recovery, catch-based reserve handoff, budgeted dunks/shoves and CPU fairness");
}
static void match_quarters(void) {
    jam_game_t g,before;
    uint8_t q,mode,p;
    uint32_t frames;
    jam_setup_t setup={{4,5,1,0},JAM_WARM_UP,4};
    jam_init_lineup(&g,&setup);
    assert(g.quarter==1 && g.clock_seconds==180 && !g.clock_fraction && g.match_phase==JAM_MATCH_LIVE);
    jam_match_clock(&g,59);assert(g.clock_seconds==180);
    jam_match_clock(&g,1);assert(g.clock_seconds==179);
    /* Chunking and slow simulation/render loops cannot change elapsed time. */
    before=g;jam_match_clock(&g,599);
    for(frames=0;frames<599;frames++)jam_match_clock(&before,1);
    assert(g.clock_seconds==before.clock_seconds && g.clock_fraction==before.clock_fraction);
    g.paused=1;before=g;jam_match_clock(&g,3600);
    assert(!memcmp(&g,&before,sizeof(g)));g.paused=0;
    for(mode=JAM_RESET;mode<=JAM_DUNK;mode++)if(mode!=JAM_FINISH){
        g.mode=mode;before=g;jam_match_clock(&g,3600);assert(!memcmp(&g,&before,sizeof(g)));
    }
    g.mode=JAM_PASS;g.inbound_pass=1;before=g;jam_match_clock(&g,3600);
    assert(!memcmp(&g,&before,sizeof(g)));g.inbound_pass=0;
    /* Four complete 180-second periods, preserving scores/lineup/stat counters. */
    jam_init_lineup(&g,&setup);g.score[0]=12;g.score[1]=10;g.shots=9;g.passes=4;
    for(q=1;q<=4;q++) {
        g.mode=JAM_HELD;g.owner=g.controlled=0;g.inbound_pass=0;
        for(frames=0;g.clock_seconds;frames++)jam_match_clock(&g,1);
        assert(frames==10754 && g.match_phase==JAM_MATCH_BUZZER && !g.clock_fraction);
        jam_update(&g,JAM_B|JAM_TURBO); /* A held ball cannot launch after zero. */
        assert(g.match_phase==(q==4?JAM_MATCH_FINAL:JAM_MATCH_BREAK));
        assert(g.mode==JAM_HELD && !g.dunk_attempts && g.shots==9 && !g.charge);
        before=g;step(&g,100,JAM_B|JAM_TURBO);jam_match_clock(&g,65535);
        assert(!memcmp(&g,&before,sizeof(g)));
        if(q==4)break;
        jam_update(&g,JAM_A);
        assert(g.quarter==q+1 && g.clock_seconds==180 && g.mode==JAM_INBOUND);
        assert(g.next_team==(q&1) && g.match_phase==JAM_MATCH_LIVE);
        assert(g.score[0]==12 && g.score[1]==10 && g.shots==9 && g.passes==4);
        assert(!memcmp(&g.setup,&setup,sizeof(setup)));
        for(p=0;p<4;p++)assert(g.turbo_energy[p]==100 && !g.players[p].shot_time && !g.players[p].stun);
        jam_match_clock(&g,600);assert(g.clock_seconds==180);
        g.previous_input=0;
    }
    jam_update(&g,JAM_A);
    assert(g.quarter==1 && g.clock_seconds==180 && !g.score[0] && !g.score[1] && !g.shots);
    assert(!memcmp(&g.setup,&setup,sizeof(setup)) && g.mode==JAM_HELD);
    jam_update(&g,JAM_A);assert(!g.passes); /* No held-A opening pass. */
    /* A released buzzer-beater finishes and scores exactly once. */
    jam_init(&g);isolate(&g);g.quarter=4;g.clock_seconds=1;
    g.players[0].x=245*4;jam_shoot(&g,0,24);g.shot_good=1;
    jam_match_clock(&g,60);jam_update(&g,0);
    assert(g.match_phase==JAM_MATCH_BUZZER && g.mode==JAM_SHOT);
    for(frames=0;frames<180 && g.match_phase!=JAM_MATCH_FINAL;frames++)jam_update(&g,0);
    assert(g.match_phase==JAM_MATCH_FINAL && g.score[0]==2 && g.baskets==1);
    before=g;step(&g,100,0);assert(!memcmp(&g,&before,sizeof(g)));
    /* Misses settle without a rebound/new shot; charge-at-zero is discarded. */
    jam_init(&g);isolate(&g);g.clock_seconds=1;g.players[0].x=245*4;
    jam_shoot(&g,0,24);g.shot_good=0;jam_match_clock(&g,60);
    for(frames=0;frames<180 && g.match_phase!=JAM_MATCH_BREAK;frames++)jam_update(&g,0);
    assert(g.match_phase==JAM_MATCH_BREAK && !g.score[0] && !g.rebounds);
    jam_init(&g);g.clock_seconds=1;g.charge=24;jam_match_clock(&g,60);jam_update(&g,0);
    assert(g.match_phase==JAM_MATCH_BREAK && !g.shots && !g.charge);
    puts("PASS: hardware-time clock, chunk invariance, pause/dunk/dead-ball freeze, four exact three-minute quarters, score/lineup preservation, alternating period inbounds, final/rematch, made/missed buzzer shots and late-shot rejection");
}
static void configurable_match_length(void) {
    static const uint8_t lengths[3]={1,2,4};
    jam_menu_t m;
    jam_game_t g,before;
    jam_setup_t setup={{0,1,2,3},JAM_WARM_UP,4};
    uint8_t n,q,i;
    jam_menu_init(&m,0);assert(m.setup.periods==4);
    for(i=0;i<5;i++)tap_menu(&m,JAM_A);
    assert(m.stage==JAM_MENU_PERIODS && m.cursor==2);
    tap_menu(&m,JAM_MOVE_DOWN);assert(m.cursor==0);
    tap_menu(&m,JAM_MOVE_UP);assert(m.cursor==2);
    tap_menu(&m,JAM_MOVE_UP);assert(m.cursor==1);
    jam_menu_update(&m,JAM_A);assert(m.stage==JAM_MENU_REVIEW && m.setup.periods==2);
    assert(!jam_menu_update(&m,JAM_A)); /* Held confirm cannot start review. */
    tap_menu(&m,JAM_B);assert(m.stage==JAM_MENU_PERIODS && m.cursor==1);
    tap_menu(&m,JAM_B);assert(m.stage==JAM_MENU_DIFFICULTY && m.cursor==JAM_PICKUP);
    tap_menu(&m,JAM_A);assert(m.cursor==1 && m.stage==JAM_MENU_PERIODS);
    for(n=0;n<3;n++) {
        setup.periods=lengths[n];assert(jam_init_lineup(&g,&setup));
        g.score[0]=7; /* This test exercises untied regulation finals. */
        for(q=1;q<=setup.periods;q++) {
            g.mode=JAM_HELD;g.owner=g.controlled=0;g.inbound_pass=0;
            jam_match_clock(&g,10753);assert(g.clock_seconds==1 && g.quarter==q);
            jam_match_clock(&g,1);jam_update(&g,0);
            assert(!g.clock_seconds && g.match_phase==(q==setup.periods?JAM_MATCH_FINAL:JAM_MATCH_BREAK));
            if(q<setup.periods) {
                g.score[0]=7;jam_update(&g,JAM_A);
                assert(g.clock_seconds==180 && g.quarter==q+1 && g.score[0]==7);
                jam_update(&g,0);
            }
        }
        before=g;step(&g,120,0);assert(!memcmp(&g,&before,sizeof(g)));
        jam_update(&g,JAM_A);assert(g.setup.periods==lengths[n] && g.quarter==1 && g.clock_seconds==180);
        assert(!g.score[0] && !g.score[1]);
        jam_menu_init(&m,&g.setup);for(i=0;i<5;i++)tap_menu(&m,JAM_A);
        assert(m.stage==JAM_MENU_PERIODS && m.cursor==n);
    }
    before=g;
    for(i=0;i<6;i++)if(i!=1 && i!=2 && i!=4) {
        setup.periods=i;assert(!jam_init_lineup(&g,&setup));assert(!memcmp(&g,&before,sizeof(g)));
    }
    puts("PASS: 1/2/4 three-minute period selection, default/wrap/held-input/backtracking, 180-second boundaries at every selected length, final freeze, rematch/config retention, invalid-length rejection");
}
static void overtime_matches(void) {
    static const uint8_t lengths[3]={1,2,4};
    jam_game_t g,before;
    jam_audio_state_t audio;
    jam_setup_t setup={{0,1,2,3},JAM_WARM_UP,4};
    uint16_t ot,frames;
    uint8_t n,p,mode;
    for(n=0;n<3;n++) {
        setup.periods=lengths[n];jam_init_lineup(&g,&setup);
        g.quarter=setup.periods;g.score[0]=g.score[1]=99;g.shots=8;g.passes=3;
        g.clock_seconds=1;jam_audio_sync(&audio,&g);
        jam_match_clock(&g,60);assert(jam_audio_event(&audio,&g)==JAM_SFX_HORN);
        jam_update(&g,JAM_A); /* Confirmation on the buzzer cannot skip the break. */
        assert(g.match_phase==JAM_MATCH_OVERTIME && !g.overtime);
        assert(jam_audio_event(&audio,&g)!=JAM_SFX_TIE);
        before=g;step(&g,20,JAM_A);jam_match_clock(&g,65535);
        assert(!memcmp(&before,&g,sizeof(g)));
        jam_update(&g,0);
        /* Repeated ties do not become finals or overflow an 8-bit OT count. */
        for(ot=1;ot<=260;ot++) {
            jam_update(&g,JAM_A);
            assert(g.overtime==ot && g.quarter==setup.periods && g.clock_seconds==60);
            assert(g.match_phase==JAM_MATCH_LIVE && g.mode==JAM_INBOUND);
            assert(g.next_team==((setup.periods+ot-1)&1));
            assert(g.score[0]==99 && g.score[1]==99 && g.shots==8 && g.passes==3);
            assert(!memcmp(&g.setup,&setup,sizeof(setup)));
            for(p=0;p<4;p++)assert(g.turbo_energy[p]==100 && !g.players[p].stun);
            assert(jam_audio_event(&audio,&g)==JAM_SFX_START);
            before=g;jam_match_clock(&g,3600);assert(!memcmp(&g,&before,sizeof(g)));
            g.mode=JAM_HELD;g.owner=g.controlled=0;g.inbound_pass=0;
            for(frames=0;g.clock_seconds;frames++)jam_match_clock(&g,1);
            assert(frames==3585 && g.match_phase==JAM_MATCH_BUZZER);
            assert(jam_audio_event(&audio,&g)==JAM_SFX_HORN);
            jam_update(&g,0);assert(g.match_phase==JAM_MATCH_OVERTIME);
            assert(jam_audio_event(&audio,&g)==JAM_SFX_NONE);
        }
        jam_update(&g,JAM_A);
        /* A real overtime basket can take a 99-99 game past the old score cap. */
        isolate(&g);g.mode=JAM_HELD;g.owner=g.controlled=0;g.inbound_pass=0;
        g.players[0].x=245*4;g.clock_seconds=1;
        jam_shoot(&g,0,24);g.shot_good=1;jam_match_clock(&g,60);
        for(frames=0;frames<180 && g.match_phase!=JAM_MATCH_FINAL;frames++)jam_update(&g,0);
        assert(g.match_phase==JAM_MATCH_FINAL && g.score[0]==101 && g.score[1]==99);
        assert(g.baskets==1 && g.shots==9);
        before=g;step(&g,50,0);assert(!memcmp(&g,&before,sizeof(g)));
        jam_update(&g,JAM_A);
        assert(!g.overtime && g.quarter==1 && g.clock_seconds==180);
        assert(!g.score[0] && !g.score[1] && !g.shots && g.setup.periods==lengths[n]);
    }
    /* A regulation buzzer-beater can tie the game; settle it before deciding. */
    jam_init(&g);isolate(&g);g.quarter=4;g.score[1]=2;g.clock_seconds=1;
    g.players[0].x=245*4;jam_shoot(&g,0,24);g.shot_good=1;
    jam_match_clock(&g,60);jam_update(&g,0);
    assert(g.match_phase==JAM_MATCH_BUZZER);
    for(frames=0;frames<180 && g.match_phase!=JAM_MATCH_OVERTIME;frames++)jam_update(&g,0);
    assert(g.match_phase==JAM_MATCH_OVERTIME && g.score[0]==2 && g.score[1]==2 && g.baskets==1);
    jam_update(&g,JAM_A);jam_update(&g,0);
    g.paused=1;before=g;jam_match_clock(&g,3600);jam_update(&g,0);
    assert(!memcmp(&g,&before,sizeof(g)));g.paused=0;
    for(mode=JAM_RESET;mode<=JAM_DUNK;mode++)if(mode!=JAM_FINISH) {
        g.mode=mode;before=g;jam_match_clock(&g,3600);assert(!memcmp(&g,&before,sizeof(g)));
    }
    /* A missed OT buzzer shot preserves the tie and cannot start a rebound. */
    isolate(&g);g.mode=JAM_HELD;g.owner=0;g.clock_seconds=1;g.inbound_pass=0;
    g.players[0].x=245*4;jam_shoot(&g,0,24);g.shot_good=0;jam_match_clock(&g,60);
    for(frames=0;frames<180 && g.match_phase!=JAM_MATCH_OVERTIME;frames++)jam_update(&g,0);
    assert(g.match_phase==JAM_MATCH_OVERTIME && g.score[0]==2 && g.score[1]==2 && !g.rebounds);
    puts("PASS: tied 1/2/4-period games, 60-second repeat overtime, 260 alternating OT inbounds, frozen breaks/pause/dead balls, turbo/stat preservation, buzzer tying/winning/missed shots, 99-point cap removal, audio and rematch reset");
}
static void audio_events(void) {
    jam_game_t g,before;
    jam_audio_state_t s;
    uint8_t event,blocked=0,slam=0,net=0,i;
    uint16_t seed;
    jam_init(&g);isolate(&g);jam_audio_sync(&s,&g);
    assert(jam_audio_event(&s,&g)==JAM_SFX_NONE);
    jam_update(&g,JAM_A);assert(jam_audio_event(&s,&g)==JAM_SFX_PASS);
    assert(jam_audio_event(&s,&g)==JAM_SFX_NONE); /* Rendering twice is silent. */
    jam_init(&g);isolate(&g);jam_audio_sync(&s,&g);
    jam_update(&g,JAM_MOVE_RIGHT|JAM_TURBO);assert(jam_audio_event(&s,&g)==JAM_SFX_TURBO);
    jam_update(&g,JAM_MOVE_RIGHT|JAM_TURBO);assert(jam_audio_event(&s,&g)!=JAM_SFX_TURBO);
    jam_update(&g,JAM_PAUSE);before=g;assert(jam_audio_event(&s,&g)==JAM_SFX_NONE);
    step(&g,100,0);assert(jam_audio_event(&s,&g)==JAM_SFX_NONE);
    before.previous_input=0;assert(!memcmp(&g,&before,sizeof(g)));
    jam_update(&g,JAM_PAUSE);assert(jam_audio_event(&s,&g)==JAM_SFX_BACK);
    /* Real blocked dunk: one rejection cue, never a slam or score celebration. */
    for(seed=1;seed<100 && !blocked;seed++) {
        dunk_fixture(&g,0,0,0);g.setup.roster[1]=2;g.setup.roster[2]=1;
        g.players[2].x=g.players[0].x+8;g.players[2].y=g.players[0].y;
        g.rng=seed;jam_audio_sync(&s,&g);assert(jam_try_dunk(&g,0));
        assert(jam_audio_event(&s,&g)==JAM_SFX_DUNK_LAUNCH);
        for(i=0;i<JAM_DUNK_END;i++) {
            jam_update(&g,0);event=jam_audio_event(&s,&g);
            if(event==JAM_SFX_DUNK_BLOCK)blocked++;
            if(blocked)assert(event!=JAM_SFX_DUNK_SLAM && event!=JAM_SFX_SWISH);
        }
    }
    assert(blocked==1);
    for(seed=1;seed<100 && !slam;seed++) {
        dunk_fixture(&g,1,0,1);g.rng=seed;jam_audio_sync(&s,&g);assert(jam_try_dunk(&g,0));
        assert(jam_audio_event(&s,&g)==JAM_SFX_DUNK_LAUNCH);
        for(i=0;i<JAM_DUNK_END;i++) {
            jam_update(&g,0);event=jam_audio_event(&s,&g);
            if(event==JAM_SFX_DUNK_SLAM)slam++;
            if(event==JAM_SFX_SWISH) {
                net++;assert(g.dunk_time==JAM_DUNK_SCORE && g.baskets==1);
            }
        }
    }
    assert(slam==1 && net==1);
    /* Normal makes rustle exactly at scoring; misses never rustle. */
    for(uint8_t made=0;made<2;made++) {
        jam_init(&g);isolate(&g);g.players[0].x=245*4;
        jam_shoot(&g,0,24);g.shot_good=made;net=0;jam_audio_sync(&s,&g);
        for(i=0;i<g.duration+JAM_FINISH_FRAMES;i++) {
            jam_update(&g,0);event=jam_audio_event(&s,&g);
            if(event==JAM_SFX_SWISH){net++;assert(g.baskets==1 && g.flight==JAM_SCORE_FRAME);}
        }
        assert(net==made);
    }
    jam_init(&g);jam_audio_sync(&s,&g);g.clock_seconds=0;g.match_phase=JAM_MATCH_BREAK;
    assert(jam_audio_event(&s,&g)==JAM_SFX_HORN);
    assert(jam_audio_event(&s,&g)==JAM_SFX_NONE);
    g.clock_seconds=180;g.quarter=2;g.match_phase=JAM_MATCH_LIVE;
    assert(jam_audio_event(&s,&g)==JAM_SFX_START);
    g.match_phase=JAM_MATCH_FINAL;g.score[0]=2;
    assert(jam_audio_event(&s,&g)==JAM_SFX_WIN);
    before=g;jam_audio_event(&s,&g);assert(!memcmp(&g,&before,sizeof(g)));
    puts("PASS: audio follows real pass/turbo/dunk events, one-shot rejection/impact cues, no celebration on blocked dunks, pause/resume and period/final cues, no simulation mutation");
}
static void check_box_score(const jam_game_t *g) {
    uint8_t p;
    uint16_t shots=0,made=0,steals=0,rebounds=0,blocks=0,dunks=0,points[2]={0,0};
    for(p=0;p<4;p++) {
        const jam_player_stats_t *s=&g->stats[p];
        assert(s->field_made<=s->field_attempted && s->three_made<=s->three_attempted);
        assert(s->three_attempted<=s->field_attempted && s->dunks<=s->field_made);
        assert(s->points==s->field_made*2+s->three_made);
        shots+=s->field_attempted;made+=s->field_made;steals+=s->steals;
        rebounds+=s->rebounds;blocks+=s->blocks;dunks+=s->dunks;points[p>>1]+=s->points;
    }
    assert(shots==g->shots && made==g->baskets && steals==g->steals);
    assert(rebounds==g->rebounds && blocks==g->blocks && dunks==g->dunks);
    assert(blocks<=shots);
    assert(points[0]==g->score[0] && points[1]==g->score[1]);
}
static void player_box_scores(void) {
    jam_game_t g,before;
    jam_player_stats_t saved[4];
    uint8_t p,i,points;
    uint16_t t;
    for(p=0;p<4;p++)for(points=2;points<=3;points++) {
        jam_init(&g);g.owner=p;g.controlled=p<2?p:0;
        for(i=0;i<4;i++) {g.players[i].x=(p<2?40:280)*4;g.players[i].y=70*4;g.players[i].cooldown=255;}
        g.players[p].x=(points==3?160:p<2?245:75)*4;g.players[p].y=JAM_RIM_Y*4;
        g.grace=255;
        jam_shoot(&g,p,24);assert(g.shot_points==points);g.shot_good=1;
        step(&g,g.duration+JAM_SCORE_FRAME,0);
        assert(g.stats[p].points==points && g.stats[p].field_attempted==1 && g.stats[p].field_made==1);
        assert(g.stats[p].three_made==(points==3) && g.stats[p].three_attempted==(points==3));
        before=g;jam_stats_basket(&g,0);assert(!memcmp(&g,&before,sizeof(g)));
        check_box_score(&g);
        memcpy(saved,g.stats,sizeof(saved));g.quarter=g.setup.periods;g.score[p>>1]=points;
        g.mode=JAM_HELD;g.clock_seconds=0;g.match_phase=JAM_MATCH_FINAL;
        jam_update(&g,0);assert(!memcmp(saved,g.stats,sizeof(saved)));
        jam_update(&g,JAM_A);for(i=0;i<4;i++)assert(!g.stats[i].points && !g.stats[i].field_attempted);
    }
    for(p=0;p<4;p++) {
        dunk_fixture(&g,1,p,3);assert(jam_try_dunk(&g,p));
        g.dunk_time=JAM_DUNK_SCORE-1;g.dunk_result=JAM_DUNK_MADE;
        jam_update(&g,0);assert(g.stats[p].dunks==1 && g.stats[p].points==2);
        check_box_score(&g);
    }
    /* Ordinary loose-ball recovery is not a missed-shot rebound. */
    jam_init(&g);isolate(&g);g.mode=JAM_LOOSE;g.owner=-1;
    g.ball_x=g.players[0].x;g.ball_y=g.players[0].y;g.height=0;
    jam_update(&g,0);assert(!g.rebounds && !g.stats[0].rebounds);
    /* Two nearby jumping defenders may contest, but only one gets the block. */
    for(t=1;t<=512;t++) {
        jam_init(&g);g.rng=t;g.owner=2;g.players[2].x=90*4;g.players[2].y=88*4;
        for(i=0;i<2;i++){g.players[i].x=90*4;g.players[i].y=88*4;g.players[i].block_time=18;}
        jam_shoot(&g,2,24);step(&g,14,0);
        assert(g.blocks<=1 && g.stats[0].blocks+g.stats[1].blocks==g.blocks);
    }
    /* A missed shot credits the actual rebounder, exactly once. */
    jam_init(&g);isolate(&g);g.players[0].x=245*4;
    jam_shoot(&g,0,24);g.shot_good=0;
    for(t=0;t<300 && !g.rebounds;t++)jam_update(&g,0);
    assert(g.rebounds==1 && g.stats[(uint8_t)g.owner].rebounds==1);check_box_score(&g);
    /* A long mixed-input match exercises catches/control changes and both teams. */
    jam_init(&g);
    for(t=0;t<30000;t++) {
        uint8_t input=(t%200<80?JAM_MOVE_RIGHT:JAM_MOVE_LEFT)|((t%48)<24?JAM_B:0);
        if(t%150<16)input|=JAM_A;
        if(t%300<70)input|=JAM_TURBO;
        jam_update(&g,input);check_box_score(&g);
    }
    assert(g.shots>30 && g.baskets>10 && g.steals && g.blocks && g.rebounds);
    memcpy(saved,g.stats,sizeof(saved));g.mode=JAM_HELD;g.quarter=1;
    g.match_phase=JAM_MATCH_BREAK;g.previous_input=0;jam_update(&g,JAM_A);
    assert(g.quarter==2 && !memcmp(saved,g.stats,sizeof(saved)));
    g.match_phase=JAM_MATCH_OVERTIME;g.quarter=g.setup.periods;g.previous_input=0;
    jam_update(&g,JAM_A);assert(g.overtime==1 && !memcmp(saved,g.stats,sizeof(saved)));
    puts("PASS: per-slot 2PT/3PT/dunk scoring, exact-once makes, all-player attempts, real rebound attribution, no fumble rebounds, 30000-update box-score conservation, period/OT retention and rematch reset");
}
static void results_navigation(void) {
    jam_results_t r;
    jam_game_t g,before;
    struct {uint8_t left,tiles[360],right;} screen;
    struct {uint8_t left,tiles[20],right;} row;
    uint8_t p,i,side;
    jam_results_init(&r,JAM_A);assert(!jam_results_update(&r,JAM_A));
    assert(!r.page && !r.changed);jam_results_update(&r,0);
    for(p=1;p<=JAM_RESULTS_PAGES;p++) {
        assert(!jam_results_update(&r,JAM_MOVE_RIGHT));assert(r.page==p%JAM_RESULTS_PAGES && r.changed);
        assert(!jam_results_update(&r,JAM_MOVE_RIGHT) && !r.changed);
        jam_results_update(&r,0);
    }
    jam_results_update(&r,JAM_MOVE_LEFT);assert(r.page==4);
    jam_results_update(&r,0);assert(jam_results_update(&r,JAM_B)==JAM_RESULTS_LINEUP);
    jam_results_update(&r,0);assert(jam_results_update(&r,JAM_A)==JAM_RESULTS_REMATCH);
    for(side=0;side<2;side++)for(i=0;i<6;i++) {
        jam_init(&g);g.setup.roster[0]=i;g.score[side]=1234;g.overtime=2;
        for(p=0;p<4;p++)memset(&g.stats[p],255,sizeof(g.stats[p]));
        before=g;
        for(p=0;p<JAM_RESULTS_PAGES;p++) {
            screen.left=37;screen.right=91;jam_results_tiles(&g,p,screen.tiles);
            assert(screen.left==37 && screen.right==91 && !memcmp(&g,&before,sizeof(g)));
            for(uint16_t t=0;t<360;t++)assert(screen.tiles[t]>=192 && screen.tiles[t]<232);
            /* Both actions remain present on every page. */
            assert(screen.tiles[16*20]==193 && screen.tiles[17*20]==194);
            /* Native rendering has only one row of scratch RAM. */
            for(uint8_t y=0;y<18;y++) {
                row.left=37;row.right=91;
                jam_results_row(&g,p,y,row.tiles);
                assert(row.left==37 && row.right==91);
                assert(!memcmp(row.tiles,screen.tiles+(uint16_t)y*20,20));
            }
        }
    }
    puts("PASS: results overview/four-player pages, both winners/all names/00 jersey, bounded large counters and font tiles, held-input guards/wrap, A rematch/B lineup, pure rendering with unchanged match data");
}
static void native_front_flow(void) {
    jam_front_t f,a,b;
    uint16_t i;
    for(i=0;i<JAM_FRONT_DURATION;i++) {
        jam_front_init(&f,0);jam_front_update(&f,0,i);
        assert(f.mode==JAM_FRONT_INTRO && f.frame>=8 && f.frame<=19);
        assert(!jam_front_update(&f,JAM_A,0) && f.mode==JAM_FRONT_HOME);
        assert(!jam_front_update(&f,JAM_A,1)); /* Skip never also starts selection. */
        jam_front_update(&f,0,1);assert(jam_front_update(&f,JAM_A,1)==JAM_FRONT_PLAY);
    }
    jam_front_init(&a,0);jam_front_init(&b,0);
    jam_front_update(&a,0,270);for(i=0;i<270;i++)jam_front_update(&b,0,1);
    assert(a.mode==b.mode && a.mode==JAM_FRONT_HOME && a.elapsed==b.elapsed && a.frame==b.frame);
    jam_front_init(&f,0);jam_front_update(&f,0,65535);assert(f.mode==JAM_FRONT_HOME && f.elapsed==270);
    jam_front_update(&f,JAM_MOVE_UP,1);assert(f.selected==2);
    jam_front_update(&f,JAM_MOVE_UP,1);assert(f.selected==2);
    jam_front_update(&f,0,1);jam_front_update(&f,JAM_A,1);
    assert(f.mode==JAM_FRONT_INTRO && f.elapsed==0 && f.frame==8);
    jam_front_update(&f,JAM_A,1);assert(f.mode==JAM_FRONT_INTRO);
    jam_front_update(&f,0,1);jam_front_update(&f,JAM_B,1);assert(f.mode==JAM_FRONT_HOME);
    jam_front_update(&f,0,1);jam_front_update(&f,JAM_MOVE_UP,1);assert(f.selected==1);
    jam_front_update(&f,0,1);jam_front_update(&f,JAM_A,1);assert(f.mode==JAM_FRONT_HELP && f.frame==3);
    jam_front_update(&f,0,1);jam_front_update(&f,JAM_B,1);assert(f.mode==JAM_FRONT_HOME && f.frame==1);
    jam_front_update(&f,0,1);jam_front_update(&f,JAM_MOVE_UP,1);
    jam_front_update(&f,0,1);assert(jam_front_update(&f,JAM_TURBO,1)==JAM_FRONT_PLAY);
    puts("PASS: native front flow, 270-VBlank proof, chunk invariance, all-frame skip/release guards, held navigation, help/home, intro replay and Start-to-play");
}
/* CPU carriers pass out of pressure or dribble around a defender in front,
   instead of walking into repeated reaches. */
static void cpu_ball_fixture(jam_game_t *g,uint16_t seed,uint8_t mate_covered) {
    jam_init(g);g->owner=2;g->mode=JAM_HELD;g->grace=255;g->possession_time=60;g->rng=seed;
    g->players[2].x=200*4;g->players[2].y=88*4;g->players[2].cooldown=0;g->players[2].contact_cooldown=255;
    g->players[0].x=188*4;g->players[0].y=88*4;           /* in front, toward the left rim */
    /* The teammate trails above, with a clear lane behind the defender. */
    g->players[3].x=212*4;g->players[3].y=52*4;g->players[3].cooldown=255;
    g->players[1].x=mate_covered?212*4:60*4;g->players[1].y=mate_covered?56*4:120*4;g->players[1].cooldown=255;
}
static void cpu_ball_security(void) {
    jam_game_t g;uint16_t seed,passes=0,around=0;uint8_t t;int16_t x0,y0;
    for(seed=1;seed<=64;seed++) {
        cpu_ball_fixture(&g,seed,0);
        for(t=0;t<20 && g.mode==JAM_HELD;t++)jam_update(&g,0);
        passes+=g.mode==JAM_PASS && g.passer==2 && g.target==3;
        cpu_ball_fixture(&g,seed,1);x0=g.players[2].x;y0=g.players[2].y;
        for(t=0;t<8;t++)jam_update(&g,0);
        assert(g.mode==JAM_HELD && g.owner==2);
        /* Around, not straight into the defender standing between him and the rim. */
        around+=(g.players[2].y>y0?g.players[2].y-y0:y0-g.players[2].y)>=24 && x0-g.players[2].x<=24;
    }
    assert(passes>=60 && around>=60);
    /* A level defender no longer flips the carrier up and down every update,
       and a diagonal runner keeps one facing (the camera follows both). */
    for(seed=1;seed<=16;seed++) {
        int16_t last;int8_t way=0;uint8_t reversals=0,turns=0,facing;
        cpu_ball_fixture(&g,seed,1);g.players[0].cooldown=255;
        last=g.players[2].y;facing=g.players[2].heading;
        for(t=0;t<24 && g.mode==JAM_HELD && g.owner==2;t++) {
            jam_update(&g,0);
            /* Count changes of vertical direction; a 1-pixel contact nudge is allowed. */
            if(g.players[2].y>last+4 || g.players[2].y<last-4) {
                int8_t now=g.players[2].y>last ? 1 : -1;
                if(way && now!=way)reversals++;
                way=now;last=g.players[2].y;
            }
            if(g.players[2].heading!=facing){turns++;facing=g.players[2].heading;}
        }
        /* One cut back toward the rim after clearing the defender is natural;
           jitter would reverse every few updates. */
        assert(reversals<=1 && turns<=2);
    }
    /* Unpressured carriers still attack the rim. */
    cpu_ball_fixture(&g,1,0);g.players[0].x=60*4;g.players[0].y=50*4;x0=g.players[2].x;
    step(&g,8,0);assert(g.owner==2 && g.players[2].x<x0-16);
    printf("PASS: CPU ball security; %u/64 pressured passes to an open teammate, %u/64 dribbles around a covered lane\n",passes,around);
}
/* CPU shot choice: a dunker rises only through a clear lane, pulls up when a
   defender stands in it, and takes open mid-range jumpers. Mid-range blends
   INSIDE and three-point. */
static void cpu_shot_fixture(jam_game_t *g,uint16_t seed,int16_t carrier_x,int16_t blocker_x) {
    jam_init(g);g->owner=2;g->mode=JAM_HELD;g->grace=255;g->possession_time=60;g->rng=seed;
    g->players[2].x=carrier_x*4;g->players[2].y=88*4;g->players[2].cooldown=0;g->players[2].contact_cooldown=255;
    g->players[0].x=blocker_x*4;g->players[0].y=88*4;g->players[0].cooldown=255;
    g->players[1].x=290*4;g->players[1].y=50*4;g->players[1].cooldown=255;
    g->players[3].x=290*4;g->players[3].y=120*4;g->players[3].cooldown=255;
}
static void cpu_shot_choice(void) {
    jam_game_t g;uint16_t seed,dunks=0,pullups=0,passes_out=0,jumpers=0,blocked_dunks=0;uint8_t t;
    /* BAMBI: INSIDE 7, three 5. Open close-range dunk lane versus a body in it. */
    for(seed=1;seed<=64;seed++) {
        cpu_shot_fixture(&g,seed,76,250);
        for(t=0;t<60 && g.mode==JAM_HELD;t++)jam_update(&g,0);
        dunks+=g.mode==JAM_DUNK;
        cpu_shot_fixture(&g,seed,76,56);
        for(t=0;t<60 && g.mode==JAM_HELD;t++)jam_update(&g,0);
        /* Guarded lane: pull up, pass out, or (by INSIDE rating) attack anyway. */
        blocked_dunks+=g.mode==JAM_DUNK;pullups+=g.mode==JAM_SHOT;passes_out+=g.mode==JAM_PASS;
        /* Open at mid-range (70 px): a pull-up jumper, never a dunk. */
        cpu_shot_fixture(&g,seed,110,250);
        for(t=0;t<60 && g.mode==JAM_HELD;t++)jam_update(&g,0);
        jumpers+=g.mode==JAM_SHOT && g.shot_points==2;
        assert(g.mode!=JAM_DUNK);
    }
    /* BAMBI (INSIDE 7) still attacks a guarded rim about 30% of the time. */
    assert(dunks>=56 && blocked_dunks>0 && blocked_dunks<dunks/2 && pullups>0 && pullups+passes_out+blocked_dunks>=60 && jumpers>=48);
    /* Mid-range chance: 52 + INSIDE + three-point, open and untimed. */
    selected(&g,0);isolate(&g);g.players[0].x=210*4;g.players[0].y=88*4;
    assert(jam_shot_chance(&g,0,0)==52+3+10);
    selected(&g,3);isolate(&g);g.players[0].x=210*4;g.players[0].y=88*4;
    assert(jam_shot_chance(&g,0,0)==52+8+3);
    printf("PASS: CPU shot choice; open-lane dunks %u/64, guarded lane %u dunks, %u pull-ups, %u passes /64, mid-range jumpers %u/64\n",dunks,blocked_dunks,pullups,passes_out,jumpers);
}
/* Jump-shot blocks: odds 5 + 5 x block, a short reach, and never from behind
   the shooter. MAGIC (block 10) defends a BAMBI jumper toward the left rim. */
static uint16_t block_trials(int16_t defender_x,int16_t defender_y) {
    jam_game_t g;uint16_t seed,blocks=0;
    for(seed=1;seed<=256;seed++) {
        selected(&g,1);isolate(&g);g.owner=2;g.mode=JAM_HELD;
        g.players[2].x=150*4;g.players[2].y=88*4;g.players[2].stun=0;
        g.players[0].x=defender_x*4;g.players[0].y=defender_y*4;
        jam_shoot(&g,2,24);g.rng=seed;
        g.players[0].block_time=18;
        step(&g,14,0);blocks+=g.shot_blocked;
    }
    return blocks;
}
static void jump_shot_blocks(void) {
    uint16_t front=block_trials(140,88),behind=block_trials(162,88),far=block_trials(108,88);
    /* Expected 55% in front; nothing from behind, or from 42 px away (the
       shooter drifts forward during the flight). */
    assert(front>110 && front<170 && !behind && !far);
    printf("PASS: jump-shot blocks; MAGIC in front %u/256, behind %u, out of reach %u\n",front,behind,far);
}
int main(void) {
    court_edges();
    boundary_violations();
    native_front_flow();
    player_box_scores();results_navigation();
    overtime_matches();
    audio_events();
    configurable_match_length();
    match_quarters();
    turbo_system();
    arcade_dunks();
    possession_control_and_inbounds();
    contact_soak();
    physical_defense();
    cpu_ball_security();
    cpu_shot_choice();
    jump_shot_blocks();
    shooting_motion();
    pump_fake_and_travel();
    shooting_accuracy();
    warm_up_fairness();
    selection_flow();ratings_pages();ratings_and_difficulty();cpu_blocking_and_levels();movement_pause();passing();shots_rebounds_reset();teammate_and_block();pause_during_swish();painted_three_point_line();cpu_soak();
    puts("PASS: movement, boundaries, pause, passes, interception, shots, scoring, rebounds, reset, CPU soak");
    return 0;
}
