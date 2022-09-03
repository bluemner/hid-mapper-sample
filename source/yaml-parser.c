#include "yaml-parser.h"

int parse_bagpipe_yaml(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event)
{
    switch (yaml_state->state)
    {
    case BAGPIPE_START:
        bagpipe_start(yaml_state, yaml_event);
        break;
    case BAGPIPE_CONFIG:
        bagpipe_config(yaml_state, yaml_event);
        break;
    case BAGPIPE_CONFIG_ITEMS:
        bagpipe_config_items(yaml_state, yaml_event);
        break;
    case BAGPIPE_CONFIG_ITEMS_KEY:
        bagpipe_config_items_key(yaml_state, yaml_event);
        break;
    case BAGPIPE_CONFIG_ITEMS_VALUE:
        bagpipe_config_items_value(yaml_state, yaml_event);
        break;
    case BAGPIPE_SEQUENCE_BUTTON:
        bagpipe_sequence_button(yaml_state, yaml_event);
        break;
    case BAGPIPE_BUTTON_ITEM:
        bagpipe_button_item(yaml_state, yaml_event);
        break;
    case BAGPIPE_BUTTON_ITEM_KEY:
        bagpipe_button_item_key(yaml_state, yaml_event);
        break;
    case BAGPIPE_BUTTON_ITEM_VALUE:
        bagpipe_button_item_value(yaml_state, yaml_event);
        break;
    case BAGPIPE_SKIP:
        skip_document(yaml_state, yaml_event);
        break;
    case BAGPIPE_ERROR:
        return false;
    case BAGPIPE_END:
        break;
    }
    return true;
}

void print_event(yaml_event_t *event){
    switch (event->type)
    {
        case YAML_NO_EVENT: printf("yaml_no_event"); break;
        case YAML_STREAM_START_EVENT: printf("yaml_stream_start_event"); break;
        case YAML_STREAM_END_EVENT: printf("yaml_stream_end_event"); break;
        case YAML_DOCUMENT_START_EVENT: printf("yaml_document_start_event"); break;
        case YAML_DOCUMENT_END_EVENT: printf("yaml_document_end_event"); break;
        case YAML_ALIAS_EVENT: printf("yaml_alias_event"); break;
        case YAML_SCALAR_EVENT: printf("yaml_scalar_event %s", (char *)event->data.scalar.value); break;
        case YAML_SEQUENCE_START_EVENT: printf("yaml_sequence_start_event"); break;
        case YAML_SEQUENCE_END_EVENT: printf("yaml_sequence_end_event"); break;
        case YAML_MAPPING_START_EVENT: printf("yaml_mapping_start_event"); break;
        case YAML_MAPPING_END_EVENT: printf("yaml_mapping_end_event"); break;
        default:
            break;
    }

    puts("\n");
}

void yaml_state_free(bagpipe_parser_state_t *yaml_state)
{
    if (yaml_state->key != NULL)
        free(yaml_state->key);
    if (yaml_state->value != NULL)
        free(yaml_state->value);
    yaml_state->key = NULL;
    yaml_state->value = NULL;
}

void bagpipe_print_example()
{
    printf("Please enter the following format\n\n");
    printf("---\n");
    printf("bagpipe:\n");
    printf("  name: \"profile name\"\n");
    printf("  description:\n");
    printf("  total_buttons: 60\n");
    printf("  button:\n");
    printf("    - index: 0\n");
    printf("      input: \"button 1\"\n");
    printf("      down: \n");
    printf("      up: \n");
}

void set_yaml_error_state(bagpipe_parser_state_t *yaml_state)
{
    bagpipe_print_example();
    yaml_state->state = BAGPIPE_ERROR;
}

void bagpipe_start(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event)
{   
    switch (yaml_event->type){
        case YAML_MAPPING_START_EVENT:
            yaml_state->state = BAGPIPE_CONFIG;
            break;
        case YAML_SCALAR_EVENT:
            break;
        case YAML_SEQUENCE_START_EVENT:
            yaml_state->state = BAGPIPE_ERROR;
            break;
        case YAML_STREAM_END_EVENT:
            yaml_state->state = BAGPIPE_END;
            break;
        default:
            break;
    }
}

void bagpipe_config(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event)
{
    switch (yaml_event->type)
    {
    case YAML_SCALAR_EVENT:
        if (STRING_EQUALS( (char*) yaml_event->data.scalar.value, "bagpipe"))
        {
            yaml_state->state = BAGPIPE_CONFIG_ITEMS;
            break;
        }
        // Document is not bagpipe, skipping allows for other configs in same file
        yaml_state->state = BAGPIPE_SKIP;
        break;

    default:
        set_yaml_error_state(yaml_state);
        break;
    }
}

void bagpipe_config_items(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event){
    switch (yaml_event->type)
    {
    case YAML_MAPPING_START_EVENT:
        yaml_state->state = BAGPIPE_CONFIG_ITEMS_KEY;
        break;

    case YAML_MAPPING_END_EVENT:
        yaml_state->state = BAGPIPE_END;
        break;
    default:
        set_yaml_error_state(yaml_state);
        break;
    }
}

void bagpipe_config_items_key(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event)
{
    if (yaml_event->type != YAML_SCALAR_EVENT){   
        yaml_state->state = BAGPIPE_END;
        return;
    }

    yaml_state->key = strdup((char *)yaml_event->data.scalar.value);
    if (STRING_EQUALS(yaml_state->key, "buttons"))
    {
        yaml_state->state = BAGPIPE_SEQUENCE_BUTTON;
        return yaml_state_free(yaml_state);
    }
    yaml_state->state = BAGPIPE_CONFIG_ITEMS_VALUE;
}

void bagpipe_config_items_value(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event)
{
  
    if (yaml_event->type != YAML_SCALAR_EVENT)
        return set_yaml_error_state(yaml_state);

    yaml_state->value = strdup((char *)yaml_event->data.scalar.value);

    if (STRING_EQUALS(yaml_state->key, "name"))
        yaml_state->bagpipe_profile->name = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "description"))
        yaml_state->bagpipe_profile->description = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "vendor"))
        yaml_state->bagpipe_profile->vendor = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "product"))
        yaml_state->bagpipe_profile->product = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "bustype_id"))
        yaml_state->bagpipe_profile->bustype_id = (uint16_t) strtoul(yaml_state->value, NULL, 16); 
    else if (STRING_EQUALS(yaml_state->key, "product_id"))
        yaml_state->bagpipe_profile->product_id = (uint16_t) strtoul(yaml_state->value, NULL, 16); 
    else if (STRING_EQUALS(yaml_state->key, "vendor_id"))
        yaml_state->bagpipe_profile->vendor_id = (uint16_t) strtoul(yaml_state->value, NULL, 16); 
    else if (STRING_EQUALS(yaml_state->key, "version_id"))
        yaml_state->bagpipe_profile->version_id = (uint16_t) strtoul(yaml_state->value, NULL, 16); 
    yaml_state->state = BAGPIPE_CONFIG_ITEMS_KEY;
    yaml_state_free(yaml_state);
    return;
}

void bagpipe_sequence_button(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event){
    if (yaml_event->type != YAML_SEQUENCE_START_EVENT)
        return set_yaml_error_state(yaml_state);
    yaml_state->state = BAGPIPE_BUTTON_ITEM;
}

void bagpipe_button_item(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event){
    switch (yaml_event->type)
    {
    case YAML_MAPPING_START_EVENT:
        yaml_state->state = BAGPIPE_BUTTON_ITEM_KEY;
        yaml_state->bagpipe_profile->button_count +=1;

        if (yaml_state->bagpipe_profile->button_count < BAGPIPE_MAX_MAP_COUNT)
            break;

        return set_yaml_error_state(yaml_state);

    case YAML_MAPPING_END_EVENT:
        yaml_state->state = BAGPIPE_END;
        break;
    case YAML_SEQUENCE_END_EVENT:
        yaml_state->state = BAGPIPE_CONFIG_ITEMS_KEY;
        break;
    default:
        set_yaml_error_state(yaml_state);
        break;
    }
}

void bagpipe_button_item_key(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event){
    switch (yaml_event->type)
    {
      case YAML_MAPPING_END_EVENT:
        yaml_state->state = BAGPIPE_BUTTON_ITEM;
        return yaml_state_free(yaml_state);
      case YAML_SCALAR_EVENT:
        yaml_state->key = strdup((char *)yaml_event->data.scalar.value);
        yaml_state->state = BAGPIPE_BUTTON_ITEM_VALUE;
        break;
    default:
        return set_yaml_error_state(yaml_state);
    }

}

void bagpipe_button_item_value(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event){
    
    if (yaml_event->type != YAML_SCALAR_EVENT)
        return set_yaml_error_state(yaml_state);

    yaml_state->value = strdup((char *)yaml_event->data.scalar.value);
    int index = yaml_state->bagpipe_profile->button_count-1;
    
    if (STRING_EQUALS(yaml_state->key, "key"))
        yaml_state->bagpipe_profile->buttons[index].key = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "key"))
        yaml_state->bagpipe_profile->buttons[index].key = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "press"))
        yaml_state->bagpipe_profile->buttons[index].press = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "release"))
        yaml_state->bagpipe_profile->buttons[index].release = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "modifier"))
        yaml_state->bagpipe_profile->buttons[index].modifier = STRING_EQUALS(yaml_state->value, "true");
    else if (STRING_EQUALS(yaml_state->key, "modifier_press"))
        yaml_state->bagpipe_profile->buttons[index].modifier_press = strdup((char *)yaml_state->value);
    else if (STRING_EQUALS(yaml_state->key, "modifier_release"))
        yaml_state->bagpipe_profile->buttons[index].modifier_release = strdup((char *)yaml_state->value);
    yaml_state->state = BAGPIPE_BUTTON_ITEM_KEY;
    yaml_state_free(yaml_state);
    return;
}

void skip_document(bagpipe_parser_state_t *yaml_state, yaml_event_t *yaml_event)
{
    if (yaml_event->type == YAML_STREAM_END_EVENT)
    {
        fprintf(stderr,"No profile enabled! Please make sure you have one profile enable");
        return set_yaml_error_state(yaml_state);
    }

    if (yaml_event->type != YAML_DOCUMENT_END_EVENT)
        return;
    yaml_state->state = BAGPIPE_START;
}

int bagpipe_load_profile(bagpipe_profile_t *result, const char * filepath){
    FILE *file = fopen(filepath, "r");
    if (file == NULL){
        return -1;
    }
    bagpipe_parser_state_t bagpipe_state = {
        .state = BAGPIPE_START, 
        .accepted=0, 
        .error=0,
        .bagpipe_profile = result
    };
    
    yaml_parser_t parser;
    yaml_event_t event;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file);

    while (bagpipe_state.state != BAGPIPE_END )
    {
        if (!yaml_parser_parse(&parser, &event)){
            goto error;
        }
        if (!parse_bagpipe_yaml(&bagpipe_state, &event)){
            goto error;
        }
        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    return true;
    error:
        yaml_parser_delete(&parser);
        yaml_event_delete(&event);
        return false;
}