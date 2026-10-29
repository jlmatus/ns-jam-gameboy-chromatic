#include "jam_sim.h"
#include "jam_menu.h"
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
    step(&g,40,JAM_MOVE_RIGHT|JAM_MOVE_DOWN);
    assert(g.players[0].x==130*4 && g.players[0].y==JAM_BOTTOM*4);
    jam_update(&g,JAM_PAUSE);before=g;
    step(&g,60,JAM_MOVE_RIGHT|JAM_B);
    assert(g.players[0].x==before.players[0].x && g.tick==before.tick && g.charge==0);
    jam_update(&g,JAM_PAUSE);assert(!g.paused);
    step(&g,500,JAM_MOVE_LEFT|JAM_MOVE_UP);
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
    // An opponent placed in the passing lane earns the interception.
    jam_init(&g);isolate(&g);g.players[1].x=156*4;g.players[1].y=78*4;
    g.players[2].x=128*4;g.players[2].y=91*4;
    jam_update(&g,JAM_A);step(&g,g.duration-1,0);assert(g.steals>0 && g.owner>=2);
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
    // Unreachable loose ball still has a bounded recovery path.
    g.mode=JAM_LOOSE;g.owner=-1;g.ball_x=2000;g.ball_y=2000;g.flight=0;g.height=0;
    step(&g,180,0);assert(g.mode==JAM_HELD && g.owner>=0);
}
static void cpu_soak(void) {
    jam_game_t g;uint16_t i;uint8_t p;
    jam_init(&g);jam_pass(&g,0,1);
    for(i=0;i<30000;i++) {
        jam_update(&g,g.mode==JAM_HELD && g.owner==g.controlled && (i%48)<24?JAM_B:0);
        assert(g.mode<=JAM_DUNK);
        if(g.mode==JAM_HELD)assert(g.owner>=0 && g.owner<4);
        for(p=0;p<4;p++)assert(g.players[p].x>=(g.mode==JAM_INBOUND || (g.inbound_pass && p==g.inbounder)?14:JAM_LEFT)*4 && g.players[p].x<=(g.mode==JAM_INBOUND || (g.inbound_pass && p==g.inbounder)?306:JAM_RIGHT)*4 && g.players[p].y>=JAM_TOP*4 && g.players[p].y<=JAM_BOTTOM*4);
    }
    printf("CPU soak: shots=%u baskets=%u steals=%u rebounds=%u score=%u:%u\n",g.shots,g.baskets,g.steals,g.rebounds,g.score[0],g.score[1]);
    fflush(stdout);
    assert(g.shots>10 && g.baskets>0 && g.rebounds>0);
}
static void teammate_and_block(void) {
    jam_game_t g;
    jam_init(&g);isolate(&g);
    g.owner=1;g.setup.roster[0]=1;g.setup.roster[1]=0;g.players[1].x=245*4;g.players[1].cooldown=0;
    jam_update(&g,0);assert(g.mode==JAM_HELD && g.players[1].windup==1);
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
    uint8_t a,b,c,d,level,i, count=0;
    uint16_t combinations=0;
    jam_menu_init(&m,0);
    tap_menu(&m,JAM_MOVE_LEFT);assert(m.cursor==5);
    tap_menu(&m,JAM_MOVE_RIGHT);assert(m.cursor==0);
    /* Every ordered distinct lineup at every difficulty is reachable. */
    for(a=0;a<6;a++)for(b=0;b<6;b++)if(b!=a)
    for(c=0;c<6;c++)if(c!=a && c!=b)
    for(d=0;d<6;d++)if(d!=a && d!=b && d!=c)
    for(level=0;level<3;level++) {
        uint8_t wanted[4]={a,b,c,d};
        jam_menu_init(&m,0);
        for(i=0;i<4;i++) {
            count=0;
            while(m.cursor!=wanted[i] && count++<6)tap_menu(&m,JAM_MOVE_RIGHT);
            assert(m.cursor==wanted[i]);tap_menu(&m,JAM_A);
        }
        assert(m.stage==JAM_MENU_DIFFICULTY);
        while(m.cursor!=level)tap_menu(&m,JAM_MOVE_DOWN);
        tap_menu(&m,JAM_A);assert(m.stage==JAM_MENU_REVIEW);
        assert(jam_menu_update(&m,JAM_TURBO));
        assert(jam_init_lineup(&g,&m.setup));
        assert(!memcmp(g.setup.roster,wanted,4) && g.setup.difficulty==level);
        {int16_t start=g.players[0].x;jam_update(&g,JAM_MOVE_RIGHT);assert(g.players[0].x>start);}
        combinations++;
    }
    assert(combinations==1080);
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
    puts("PASS: 1080 lineup/difficulty combinations, no duplicates, backtracking, confirm, invalid setup");
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
    jam_setup_t setup={{0,2,1,3},JAM_PICKUP};
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
            for(p=0;p<4;p++)assert(g.players[p].x>=(g.mode==JAM_INBOUND || (g.inbound_pass && p==g.inbounder)?14:JAM_LEFT)*4 && g.players[p].x<=(g.mode==JAM_INBOUND || (g.inbound_pass && p==g.inbounder)?306:JAM_RIGHT)*4 && g.players[p].y>=JAM_TOP*4 && g.players[p].y<=JAM_BOTTOM*4);
        }
        printf("Level %u soak: %u shots, %u baskets, %u rebounds, owner %d windup %u mode %u\n",level,g.shots,g.baskets,g.rebounds,g.owner,g.players[2].windup,g.mode);fflush(stdout);
        assert(g.shots>5 && g.baskets>0);total_rebounds+=g.rebounds;
    }
    assert(total_rebounds>0); /* Individual deterministic runs can contain only makes. */
    puts("PASS: CPU shot blocking and 12000-update soak at each difficulty");
}
static void selected(jam_game_t *g,uint8_t who) {
    jam_setup_t setup={{0,0,0,0},JAM_PICKUP};
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
        strong.possession_time=weak.possession_time=8;
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
    assert(strong.players[0].y==baseline*4); /* BAMBI holds position; TORRES yields. */
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
    step(&g,12,0);assert(g.mode==JAM_HELD && g.players[1].windup==12);
    jam_pass(&g,1,0);jam_update(&g,0);assert(!g.players[1].windup);
    puts("PASS: gather/rise, attached ball, release/landing, paused animation, interruptible CPU windup");
}
static void defensive_fixture(jam_game_t *g,uint16_t seed,uint8_t behind) {
    jam_init(g);isolate(g);g->owner=2;g->grace=120;g->possession_time=8;
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
    assert(front>behind+100 && front>300 && front<490);
    defensive_fixture(&a,1,0);a.possession_time=0;
    jam_update(&a,JAM_A);assert(a.steal_attempts==0);
    a.possession_time=8;jam_update(&a,JAM_A);assert(a.steal_attempts==1);
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
                assert(g.players[p].stun<=18 && g.players[p].contact_cooldown<=40);
                assert(g.players[p].x>=(g.mode==JAM_INBOUND || (g.inbound_pass && p==g.inbounder)?14:JAM_LEFT)*4 && g.players[p].x<=(g.mode==JAM_INBOUND || (g.inbound_pass && p==g.inbounder)?306:JAM_RIGHT)*4);
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
    jam_update(&g,0);assert(g.mode==JAM_SHOT && g.passer==1);
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
int main(void) {
    arcade_dunks();
    possession_control_and_inbounds();
    contact_soak();
    physical_defense();
    shooting_motion();
    shooting_accuracy();
    warm_up_fairness();
    selection_flow();ratings_pages();ratings_and_difficulty();cpu_blocking_and_levels();movement_pause();passing();shots_rebounds_reset();teammate_and_block();pause_during_swish();painted_three_point_line();cpu_soak();
    puts("PASS: movement, boundaries, pause, passes, interception, shots, scoring, rebounds, reset, CPU soak");
    return 0;
}
