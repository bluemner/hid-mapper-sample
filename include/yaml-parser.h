/**
 * @file yaml-parser.h
 * @brief Parse Yaml configuration file into bagpipe config
 * 
 * Include the header file with the code:
 * @code
 * #include <yaml-parser.h>
 * @endcode
 */
#ifndef __BAGPIPE_YAML_PARSER_H__
#define __BAGPIPE_YAML_PARSER_H__
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <yaml.h>
#include <config.h>

enum bagpipe_parser_sate_value
{
    BAGPIPE_START,
    BAGPIPE_CONFIG,
    BAGPIPE_CONFIG_ITEMS,
    BAGPIPE_CONFIG_ITEMS_KEY,
    BAGPIPE_CONFIG_ITEMS_VALUE,
    BAGPIPE_SEQUENCE_BUTTON,
    BAGPIPE_BUTTON_ITEM,
    BAGPIPE_BUTTON_ITEM_KEY,
    BAGPIPE_BUTTON_ITEM_VALUE,
    BAGPIPE_SKIP, // Used to move to next document
    BAGPIPE_ERROR,
    BAGPIPE_END
};

typedef struct bagpipe_parser_state_s
{
    enum bagpipe_parser_sate_value state;
    char *key;
    char *value;  
    int accepted;
    int error;
    bagpipe_profile_t *bagpipe_profile;
} bagpipe_parser_state_t;

int parse_bagpipe_yaml(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_start(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_config(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_config_items(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_config_items_key(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_config_items_value(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_sequence_button(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_button_item(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_button_item_key(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void bagpipe_button_item_value(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);
void skip_document(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event);

void bagpipe_print_example();
/**
 * @return @c 1 if the function succeeded, @c 0 on error.
 */
int bagpipe_load_profile(bagpipe_profile_t *result, const char * file);
#endif