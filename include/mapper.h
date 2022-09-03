#ifndef __BAGPIPE_MAPPER_H__
#define __BAGPIPE_MAPPER_H__

#include "emitter.h"
#include "config.h"
#include "macro.h"

typedef struct mapper mapper_t;

/**
 *  Bagpipe Button Mask information on the input bitmask. 
 *  It also stores the list of macros.
 */
typedef struct bagpipe_button_mask
{
  union {
    uint8_t  uint8;
    uint16_t uint16;
    uint32_t uint32;
    uint64_t uint64;
  } bitmask;
  uint8_t mask_size;
  uint32_t INPUT_EVENT_CODE; // <linux/input-event-codes.h>
  char * INPUT_EVENT_TEXT;
  bagpipe_macro_config_t *macro_config;
} bagpipe_button_mask_t;

typedef union map_fn
{
    void *ptr;
    int (*call) (mapper_t *mapper, const bagpipe_button_mask_t *button, key_value_t value);
} map_fn;

/**
 * Connects a profile, mapping function, and emitter 
 */
typedef struct mapper{
    bagpipe_profile_t *profile;
    map_fn map;
    emitter_t *emitter;
} mapper_t;

/** 
 * Mapper 
 * @param[in] mapper will take uinput and map to the out put sec
 * @param[in] button mask that has the macro information in it
 * @param[in] value is the state of the key pressed, hold, or released.
 */
void map(mapper_t *mapper, const bagpipe_button_mask_t *button, key_value_t value);

/**
 * No map skips any mapping and uses the default KEY value
 * @param[in] mapper will take uinput and map to the out put sec
 * @param[in] button mask that has the macro information in it
 * @param[in] value is the state of the key pressed, hold, or released.
 */
void no_map(mapper_t *mapper, const bagpipe_button_mask_t *button, key_value_t value);
#endif