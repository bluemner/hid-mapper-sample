#include <stdio.h>
#include "macro.h"
#include "assert.h"

void test1(){
  bagpipe_macro_t macro;
  bagpipe_macro_initialize(&macro);
  bagpipe_macro_parse("+KEY_1 KEY_2 -KEY_1", &macro);
  printf("Macro Count %d\n", macro.count);
  assert(macro.count == (uint16_t) 4 && "Check size is 3");
  uint16_t index =0;
  for(; index < macro.count; ++index){
    printf("Key %s code %d state %d \n", macro.macros[index].text, macro.macros[index].code, macro.macros[index].value);
  }
  bagpipe_macro_destroy(&macro);
}

void test2(){
  printf("Test 2\n");
  bagpipe_macro_t macro;
  bagpipe_macro_initialize(&macro);
  bagpipe_macro_parse("+KEY_A KEY_B KEY_C KEY_D KEY_E -KEY_A", &macro);
  assert(macro.count == (uint16_t) 10 && "Check size is 10");
  uint16_t index =0;
  for(; index < macro.count; ++index){
    printf("Key %s code %d state %d \n", macro.macros[index].text, macro.macros[index].code, macro.macros[index].value);
  }
  bagpipe_macro_destroy(&macro);
}

void test_empty(){
  bagpipe_macro_t macro;
  bagpipe_macro_initialize(&macro);
  bagpipe_macro_parse("", &macro);
  assert(macro.count == (uint16_t) 0 && "Check size is 0");
  bagpipe_macro_parse("+KEY_A", &macro);
  assert(macro.count == (uint16_t) 1 && "Check size is 1");
  bagpipe_macro_destroy(&macro);
}
int main(){
  test1();
  test2();
  test_empty();
  return 0;
}