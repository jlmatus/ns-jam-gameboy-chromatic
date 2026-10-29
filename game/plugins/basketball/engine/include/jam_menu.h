#ifndef JAM_MENU_H
#define JAM_MENU_H
#include "jam_sim.h"
#define JAM_MENU_DIFFICULTY 4
#define JAM_MENU_REVIEW 5
/* The menu owns configuration; the simulation starts only after review. */
typedef struct {
    jam_setup_t setup;
    uint8_t stage, cursor, previous_input, changed, page;
} jam_menu_t;
void jam_menu_init(jam_menu_t *menu, const jam_setup_t *setup) JAM_BANKED;
uint8_t jam_menu_update(jam_menu_t *menu, uint8_t input) JAM_BANKED;
#endif
