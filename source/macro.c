#include "macro.h"


void bagpipe_macro_initialize(bagpipe_macro_t *macro){
  macro->macros =NULL;
  macro->count=0;
}

bagpipe_macro_t* bagpipe_macro_create(const char* text){
  bagpipe_macro_t* macro = malloc(sizeof(bagpipe_macro_t));
  bagpipe_macro_initialize(macro);
  bagpipe_macro_parse(text, macro);
  return macro;
}


void bagpipe_macro_destroy(bagpipe_macro_t *macro){
  if (macro == NULL) return;
  uint16_t index =0;
  for(;index < macro->count; ++index ){
    free(macro->macros[index].text);
    macro->macros[index].text = NULL;
  }
  macro->count=0;
  if (macro->macros != NULL){
    free(macro->macros);
    macro->macros= NULL;
  }
}
void bagpipe_macro_config_initialize(bagpipe_macro_config_t* macro_config){
  *macro_config = (bagpipe_macro_config_t){  NULL, NULL, NULL, NULL, NULL , false };
}
bagpipe_macro_config_t* bagpipe_macro_config_create(){
  bagpipe_macro_config_t* macro_config = malloc(sizeof(bagpipe_macro_config_t));
  bagpipe_macro_config_initialize(macro_config);
  return macro_config;
}


void bagpipe_macro_config_destroy(bagpipe_macro_config_t* macro_config){
  free_ptr(macro_config->key);
  bagpipe_macro_destroy(macro_config->press);
  bagpipe_macro_destroy(macro_config->release);
  bagpipe_macro_destroy(macro_config->modifier_press);
  bagpipe_macro_destroy(macro_config->modifier_release);
  free_ptr(macro_config);
}

void bagpipe_macro_parse(const char* text, bagpipe_macro_t *macro){
  if(macro->count !=0 || text == NULL || strlen(text) < 1 ) return;
  char input[strlen(text)+1];

  strncpy(input, text,strlen(text));
  input[strlen(text)]='\0';
  macro->count = 1;
  char* pointer = (char *) text;
  while (*pointer){
    if (*pointer++ == ' '){
      ++(macro->count);
      if (*pointer != '+' && *pointer != '-'){
        ++(macro->count);
        ++pointer;
      }
    }
  }
  macro->macros = malloc(macro->count * sizeof(bagpipe_macro_item_t));
  uint16_t count =0;
  char *token = strtok(input, " ");
  if (token == NULL){
    macro->macros[count].value=MACRO_KEY_VALUE_BOTH;
    if (text[0]=='+'){
      macro->macros[count].value=MACRO_KEY_VALUE_PRESSED;
    }
    if (text[0]=='-'){
      macro->macros[count].value=MACRO_KEY_VALUE_RELEASE;
    }
    macro->macros[count].text = strdup(text);
    macro->macros[count].code = key_code(text);
    if(macro->macros[count].value == MACRO_KEY_VALUE_BOTH){
      macro->macros[count].value=MACRO_KEY_VALUE_PRESSED;
      ++count;
      macro->macros[count].text = strdup(text);
      macro->macros[count].code = key_code(text);
      macro->macros[count].value=MACRO_KEY_VALUE_RELEASE;
    }
    return;
  }

  while(token) {
    macro->macros[count].value=MACRO_KEY_VALUE_BOTH;
    if (token[0]=='+'){
      macro->macros[count].value=MACRO_KEY_VALUE_PRESSED;
      ++token;
    }
    if (token[0]=='-'){
      macro->macros[count].value=MACRO_KEY_VALUE_RELEASE;
      ++token;
    }
    macro->macros[count].text = strdup(token);
    macro->macros[count].code = key_code(token);

    if(macro->macros[count].value == MACRO_KEY_VALUE_BOTH){
      macro->macros[count].value=MACRO_KEY_VALUE_PRESSED;
      ++count;
      macro->macros[count].text = strdup(token);
      macro->macros[count].code = key_code(token);
      macro->macros[count].value=MACRO_KEY_VALUE_RELEASE;
    }

    token = strtok(NULL, " ");
    ++count;
  }
}

bool bagpipe_macro_valid(const bagpipe_macro_t *macro){
  uint16_t index = 0;
  for (;index < macro->count; ++index){
    if(macro->macros[index].code < 0)
      return false;
  }
  return true;
}