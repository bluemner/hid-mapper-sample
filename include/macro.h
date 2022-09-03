#ifndef __BAGPIPE_MACRO_H__
#define __BAGPIPE_MACRO_H__

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <keys.h>
#include <ptr.h>




typedef enum bagpipe_macro_key_state_e {
    MACRO_KEY_VALUE_RELEASE = 0,
    MACRO_KEY_VALUE_PRESSED = 1,
    MACRO_KEY_VALUE_BOTH = 2
} bagpipe_macro_key_state_t;

typedef struct bagpipe_macro_item_s{
    int code;
    char* text;
    bagpipe_macro_key_state_t value;
} bagpipe_macro_item_t;

typedef struct bagpipe_macro_s{
    bagpipe_macro_item_t* macros;
    uint16_t count;
} bagpipe_macro_t;

typedef struct bagpipe_macro_config_s{
    char* key; // key name 
    bagpipe_macro_t* press; // action when pressed
    bagpipe_macro_t* release; // action when released
    bagpipe_macro_t* modifier_press; // action when modifier is pressed
    bagpipe_macro_t* modifier_release; // action when modifier is released
    bool modifier; // is key acting as a modifier. Will disable all press and release actions
} bagpipe_macro_config_t;



/**
 * Initialize macro structure
 * @param[out] macro to allocated memory
 */
void bagpipe_macro_initialize(bagpipe_macro_t *macro);

/**
 * Allocates new macro structure
 * @param[in] text to be parsed into macro structure
 */
bagpipe_macro_t* bagpipe_macro_create(const char* text);
/**
 * Destroy macro structure
 * @param[out] macro to deallocate memory
 */
void bagpipe_macro_destroy(bagpipe_macro_t *macro);

/**
 * Macro item create
 * @returns pointer to macros
 */
bagpipe_macro_config_t* bagpipe_macro_config_create();

/**
 * Initialize
 * @param[out] macro_config initialize 
 */
void bagpipe_macro_config_initialize(bagpipe_macro_config_t* macro_config);

/**
 * Initialize
 * @param[out] macro_config deallocates memory and sets to @c NULL 
 */
void bagpipe_macro_config_destroy(bagpipe_macro_config_t* macro_config);

/**
 * Prase input into the macro
 * @param[in] input to be parsed into macro structure
 * @param[out] macro structure to add input elements into 
 */
void bagpipe_macro_parse(const char* input, bagpipe_macro_t *macro);

/**
 * Checks the key code in macro; 
 * @param[in] macro structure that will be checked for negative codes
 */
bool bagpipe_macro_valid(const bagpipe_macro_t *macro);
#endif