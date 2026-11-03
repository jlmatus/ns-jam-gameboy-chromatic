#ifndef JAM_RESULTS_H
#define JAM_RESULTS_H
#include "jam_sim.h"
#define JAM_RESULTS_NONE 0
#define JAM_RESULTS_REMATCH 1
#define JAM_RESULTS_LINEUP 2
#define JAM_RESULTS_PAGES 5
typedef struct {
    uint8_t page,previous_input,changed;
} jam_results_t;
void jam_results_init(jam_results_t *r,uint8_t held) JAM_BANKED;
uint8_t jam_results_update(jam_results_t *r,uint8_t input) JAM_BANKED;
void jam_results_tiles(const jam_game_t *g,uint8_t page,uint8_t *tiles) JAM_BANKED;
void jam_results_row(const jam_game_t *g,uint8_t page,uint8_t row,uint8_t *tiles) JAM_BANKED;
#endif
