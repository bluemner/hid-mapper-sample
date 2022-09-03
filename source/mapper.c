#include "mapper.h"


void do_it(emitter_t *emitter, bagpipe_macro_t* macro){
    int index =0;
    if (emitter->fd < 0) return;
    for (; index< macro->count; index++){
        printf("macro %d %s %d\n", index,macro->macros[index].text, macro->macros[index].code);
        emitter->emit.call(emitter->fd, 
                           EV_KEY,
                           macro->macros[index].code,
                           (int)macro->macros[index].value);    
    }   
}

/**
 * parse release
 * @param[in] emitter
 */
void parse_press(emitter_t *emitter, const bagpipe_button_mask_t *button){
    printf("Button %s\n", button->INPUT_EVENT_TEXT);
    
    if(button->macro_config->modifier){
        emitter->modifier_active = true;
        return;
    }
    bagpipe_macro_t * macro = emitter->modifier_active ? button->macro_config->modifier_press : button->macro_config->press;
    do_it(emitter,macro);
}

/**
 * parse release
 * @param[in] emitter 
 */
void parse_release(emitter_t *emitter, const bagpipe_button_mask_t *button){
    printf("Button %s\n", button->INPUT_EVENT_TEXT);
    if(button->macro_config->modifier){
        emitter->modifier_active = false;
        return;
    }
    bagpipe_macro_t * macro = emitter->modifier_active ? button->macro_config->modifier_release : button->macro_config->release;
    do_it(emitter,macro);
}

void map(mapper_t *mapper, const bagpipe_button_mask_t *button, key_value_t value){
    switch (value){
        case KEY_VALUE_PRESSED:
            parse_press(mapper->emitter, button);
            break;
        case KEY_VALUE_RELEASE:
            parse_release(mapper->emitter, button);
            break;

        default:
            break;
    }
}

void no_map(mapper_t *mapper, const bagpipe_button_mask_t *button, key_value_t value){
    mapper->emitter->emit.call(mapper->emitter->fd, EV_KEY, button->INPUT_EVENT_CODE , value);
}