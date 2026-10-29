#ifndef JAM_SIM_H
#define JAM_SIM_H
#include <stdint.h>
#ifdef __SDCC
#include <gbdk/platform.h>
#define JAM_BANKED BANKED
#else
#define JAM_BANKED
#endif

/* Simulation positions use quarter pixels; rendering has no authority over rules. */
#define JAM_COURT_WIDTH 320
#define JAM_VIEW_WIDTH 400
#define JAM_LEFT 32
#define JAM_RIGHT 288
#define JAM_TOP 70
#define JAM_BOTTOM 116
#define JAM_RIM_LEFT 40
#define JAM_RIM_RIGHT 280
#define JAM_RIM_Y 88
#define JAM_RIM_HEIGHT 44
#define JAM_FINISH_FRAMES 28
#define JAM_SCORE_FRAME 20
#define JAM_HELD 0
#define JAM_PASS 1
#define JAM_SHOT 2
#define JAM_LOOSE 3
#define JAM_RESET 4
#define JAM_FINISH 5
#define JAM_INBOUND 6
#define JAM_DUNK 7
#define JAM_DUNK_IMPACT 28
#define JAM_DUNK_SCORE 34
#define JAM_DUNK_END 64
#define JAM_DUNK_PENDING 0
#define JAM_DUNK_MADE 1
#define JAM_DUNK_MISSED 2
#define JAM_DUNK_BLOCKED 3
#define JAM_MOVE_LEFT 1
#define JAM_MOVE_RIGHT 2
#define JAM_MOVE_UP 4
#define JAM_MOVE_DOWN 8
#define JAM_A 16
#define JAM_B 32
#define JAM_TURBO 64
#define JAM_PAUSE 128
#define JAM_MSG_PLAY 0
#define JAM_MSG_PASS 1
#define JAM_MSG_STEAL 2
#define JAM_MSG_SHOT 3
#define JAM_MSG_REBOUND 4
#define JAM_MSG_TWO 5
#define JAM_MSG_THREE 6
#define JAM_MSG_MISS 7
#define JAM_MSG_BLOCK 8
#define JAM_MSG_CLOCK 9
#define JAM_MSG_REACH 10
#define JAM_MSG_SHOVE 11
#define JAM_MSG_RUNOVER 12
#define JAM_MSG_STAND 13
#define JAM_MSG_INBOUND 14
#define JAM_MSG_DUNK 15

#define JAM_ROSTER_COUNT 6
#define JAM_WARM_UP 0
#define JAM_PICKUP 1
#define JAM_WINNERS_STAY 2

typedef struct {
    uint8_t speed, three_point, passing, steal, block, dunk, power, clutch;
} jam_ratings_t;

typedef struct {
    char name[7], jersey[3], position[6], signature[16];
    jam_ratings_t ratings;
} jam_roster_entry_t;

#include "jam_bodies.h"
#define JAM_FACE_RIGHT 0
#define JAM_FACE_LEFT 1
#define JAM_FACE_DOWN 2
#define JAM_FACE_UP 3

typedef struct {
    uint8_t roster[4], difficulty;
} jam_setup_t;

/* Canonical supplied roster. Jersey strings preserve MEAT's leading zero. */
static const jam_roster_entry_t jam_roster[JAM_ROSTER_COUNT] = {
    {"MATUS", "13", "GUARD", "TOP OF THE KEY", {7,10,10,8,2,3,5,9}},
    {"MAGIC", "32", "BIG", "OVER THE TOP", {4,5,5,3,10,9,7,7}},
    {"BAMBI", "23", "WING", "BULLDOZER", {4,5,6,4,4,7,10,7}},
    {"TORRES", "7", "GUARD", "BACK DOOR", {10,3,7,7,4,8,4,5}},
    {"CHOMPA", "74", "BIG", "STRETCH BIG", {5,8,5,3,7,7,9,6}},
    {"MEAT", "00", "GUARD", "BIG ENERGY", {9,2,5,8,6,8,6,4}}
};

typedef struct {
    int16_t x, y;
    uint8_t action, cooldown, block_time, shot_time, launch_jump, windup;
    uint8_t heading, turbo, stun, contact_cooldown, reach_time;
    int8_t vx,vy,push_x,push_y;
} jam_player_t;

typedef struct {
    jam_player_t players[4];
    jam_setup_t setup;
    int16_t ball_x, ball_y, from_x, from_y, target_x, target_y;
    int8_t owner, passer;
    uint8_t mode, height, flight, duration, target, shot_team, shot_points, shot_good;
    uint8_t score[2], tick, paused, previous_input, charge, block, grace;
    uint8_t message, message_time, reset_time, next_team;
    uint8_t scored, block_attempts, intercept_attempts, launch_height;
    uint16_t rng, possession_time, passes, steals, shots, rebounds, baskets, blocks;
    uint16_t steal_attempts,shoves,runovers;
    uint8_t controlled, inbounder, inbound_receiver, inbound_phase, inbound_time, inbound_pass;
    uint8_t dunk_time, dunk_result, dunk_block_time;
    uint16_t dunk_attempts, dunks;
    uint8_t dunk_defender, dunk_contests;
} jam_game_t;

void jam_init(jam_game_t *g) JAM_BANKED;
void jam_tipoff(jam_game_t *g) JAM_BANKED;
void jam_begin_inbound(jam_game_t *g,uint8_t next_team) JAM_BANKED;
void jam_inbound_update(jam_game_t *g,uint8_t pressed) JAM_BANKED;
void jam_inbound_return(jam_game_t *g) JAM_BANKED;
uint8_t jam_init_lineup(jam_game_t *g, const jam_setup_t *setup) JAM_BANKED;
uint8_t jam_move_speed(const jam_game_t *g, uint8_t who, uint8_t turbo) JAM_BANKED;
uint8_t jam_shot_chance(const jam_game_t *g, uint8_t who, uint8_t charge) JAM_BANKED;
uint8_t jam_jump_height(const jam_game_t *g, uint8_t who) JAM_BANKED;
uint8_t jam_roll_percent(jam_game_t *g) JAM_BANKED;
uint8_t jam_dunk_range(const jam_game_t *g,uint8_t who) JAM_BANKED;
uint8_t jam_dunk_chance(const jam_game_t *g,uint8_t who) JAM_BANKED;
uint8_t jam_try_dunk(jam_game_t *g,uint8_t who) JAM_BANKED;
uint8_t jam_dunk_jump(const jam_game_t *g) JAM_BANKED;
void jam_dunk_update(jam_game_t *g,uint8_t pressed) JAM_BANKED;
int16_t jam_dunk_view_x(const jam_game_t *g,int16_t world_x,int16_t avatar_q8) JAM_BANKED;
int16_t jam_dunk_view_y(const jam_game_t *g,int16_t world_y,uint8_t height) JAM_BANKED;
/* Each caller uses its own ROM-bank-local constant table. */
#define jam_body(g,who) (&jam_bodies[(g)->setup.roster[(who)]])
void jam_shove(jam_game_t *g,uint8_t p) JAM_BANKED;
void jam_contact(jam_game_t *g,uint8_t i,uint8_t j) JAM_BANKED;
void jam_update(jam_game_t *g, uint8_t input) JAM_BANKED;
void jam_pass(jam_game_t *g, uint8_t from, uint8_t to) JAM_BANKED;
void jam_shoot(jam_game_t *g, uint8_t who, uint8_t charge) JAM_BANKED;
#endif
