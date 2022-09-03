#ifndef __BAGPIPE_CONFIG_H__
#define __BAGPIPE_CONFIG_H__
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <yaml.h>
#include <ptr.h>

#define BAGPIPE_MAX_MAP_COUNT 64
#ifndef STRING_EQUALS
#define STRING_EQUALS(lhs, rhs) strcmp(lhs, rhs) == 0
#endif


typedef struct bagpipe_profile_map_item_s{
    char* key; // key name 
    char* press; // action when pressed
    char* release; // action when released
    char* modifier_press; // action when modifier is pressed
    char* modifier_release; // action when modifier is released
    bool modifier; // is key acting as a modifier. Will disable all press and release actions
} bagpipe_profile_map_item_t;

typedef struct bagpipe_profile_s{
    char* name;
    char* description;
    char* vendor;    
    char* product;
    uint16_t bustype_id;
    uint16_t product_id;
    uint16_t vendor_id;
    uint16_t version_id;
    bagpipe_profile_map_item_t buttons[BAGPIPE_MAX_MAP_COUNT];
    uint32_t button_count;
} bagpipe_profile_t;



int bagpipe_profile_initialize(bagpipe_profile_t *item);
int bagpipe_profile_map_item_initialize(bagpipe_profile_map_item_t *item);
void bagpipe_profile_delete(bagpipe_profile_t *item);
void bagpipe_profile_map_item_delete(bagpipe_profile_map_item_t *item);
void print_profile(bagpipe_profile_t *profile);
#endif