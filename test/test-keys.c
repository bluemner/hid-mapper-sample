#include <linux/input-event-codes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "keys.h"

void test_key_name(){
    printf("Check key name of range outside\n" );
    assert(key_name(KEY_MAX) == NULL);
    printf("Check key KEY_RESERVED\n" );
    assert( STRING_EQUALS(key_name(KEY_RESERVED), "KEY_RESERVED"));
    assert( STRING_EQUALS(key_name(KEY_0), "KEY_0"));
    assert( STRING_EQUALS(key_name(KEY_1), "KEY_1"));
    assert( STRING_EQUALS(key_name(KEY_2), "KEY_2"));
    assert( STRING_EQUALS(key_name(KEY_3), "KEY_3"));
}

void test_key_codes(){    
    const char* test_key_f1 = "KEY_F1";
    printf("Test %s %d\n", test_key_f1, key_code(test_key_f1));
    const char* test_key_btn_a = "BTN_A";
    printf("Test %s %d\n", test_key_btn_a, key_code(test_key_btn_a));
    const char* test_key_btn_south = "BTN_SOUTH";
    printf("Test %s %d\n", test_key_btn_south, key_code(test_key_btn_south));
}
/**
 * 
*/
int main(int argc, char ** argv)
{

    bool useable_key[KEY_CNT];
    memset(useable_key, 0, sizeof(useable_key));
    char *input = "/dev/input/event0";
    int result =  get_valid(input, useable_key, sizeof(useable_key), EV_KEY);
    
    assert(result == 0 && "Code should be 0 for Success. 1 Error with FD try running with sudo.");
    if (result)
    {
        printf("Something went wrong!\n");
        return 1;
    }

    size_t index = 0;
    for(; index < KEY_MAX; index++){
        if(useable_key[index])
            printf("Key is supported: %s\n", key_name(index));
    }
    test_key_name();
    test_key_codes();

    return 0;
}