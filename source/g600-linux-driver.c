#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input-event-codes.h>
#include <signal.h>
#include "mapper.h"
#include "yaml-parser.h"

static volatile sig_atomic_t online = true;

#define LOGITECH_G600_CURRENT_STATE 0xE0
#define LOGITECH_G600_STATE_TYPE_BUTTONS 0x80
#define LOGITECH_G600_BUTTON_COUNT 21

/*
 * Bitmask on G600 reportnum 0xE0
 * These masks are High (1) when button is pressed and
 *                 Low  (0) when depressed.
 */
#define LOGITECH_G600_BUTTON_CLEAR              0x000000  // clear (no buttons pressed)
#define LOGITECH_G600_BUTTON_LEFT_MOUSE_CLICK   0x000001  // left mouse click
#define LOGITECH_G600_BUTTON_RIGHT_MOUSE_CLICK  0x000002  // right mouse click
#define LOGITECH_G600_BUTTON_MIDDLE_MOUSE_CLICK 0x000004  // middle mouse click
#define LOGITECH_G600_BUTTON_LEFT_SCROLL_CLICK  0x000008  // left scroll click
#define LOGITECH_G600_BUTTON_RIGHT_SCROLL_CLICK 0x000010  // right scroll click
#define LOGITECH_G600_BUTTON_RIGHT_PINKY        0x000020  // right pinky
#define LOGITECH_G600_BUTTON_G07                0x000040  // G07
#define LOGITECH_G600_BUTTON_G08                0x000080  // G08
#define LOGITECH_G600_BUTTON_G09                0x000100  // G09
#define LOGITECH_G600_BUTTON_G10                0x000200  // G10
#define LOGITECH_G600_BUTTON_G11                0x000400  // G11
#define LOGITECH_G600_BUTTON_G12                0x000800  // G12
#define LOGITECH_G600_BUTTON_G13                0x001000  // G13
#define LOGITECH_G600_BUTTON_G14                0x002000  // G14
#define LOGITECH_G600_BUTTON_G15                0x004000  // G15
#define LOGITECH_G600_BUTTON_G16                0x008000  // G16
#define LOGITECH_G600_BUTTON_G17                0x010000  // G17
#define LOGITECH_G600_BUTTON_G18                0x020000  // G18
#define LOGITECH_G600_BUTTON_G19                0x040000  // G19
#define LOGITECH_G600_BUTTON_G20                0x080000  // G20

bagpipe_button_mask_t LOGITECH_G600_BUTTON_LIST[LOGITECH_G600_BUTTON_COUNT] = {
    { .bitmask.uint32=LOGITECH_G600_BUTTON_CLEAR,              .INPUT_EVENT_CODE=KEY_RESERVED,.INPUT_EVENT_TEXT="KEY_RESERVED",.mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_LEFT_MOUSE_CLICK,   .INPUT_EVENT_CODE=BTN_LEFT,    .INPUT_EVENT_TEXT="BTN_LEFT",    .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_RIGHT_MOUSE_CLICK,  .INPUT_EVENT_CODE=BTN_RIGHT,   .INPUT_EVENT_TEXT="BTN_RIGHT",   .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_MIDDLE_MOUSE_CLICK, .INPUT_EVENT_CODE=BTN_MIDDLE,  .INPUT_EVENT_TEXT="BTN_MIDDLE",  .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_LEFT_SCROLL_CLICK,  .INPUT_EVENT_CODE=BTN_BACK,    .INPUT_EVENT_TEXT="BTN_BACK",    .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_RIGHT_SCROLL_CLICK, .INPUT_EVENT_CODE=BTN_FORWARD, .INPUT_EVENT_TEXT="BTN_FORWARD", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_RIGHT_PINKY,        .INPUT_EVENT_CODE=BTN_PINKIE,  .INPUT_EVENT_TEXT="BTN_PINKIE",  .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G07,                .INPUT_EVENT_CODE=KEY_MACRO7,  .INPUT_EVENT_TEXT="KEY_MACRO7",  .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G08,                .INPUT_EVENT_CODE=KEY_MACRO8,  .INPUT_EVENT_TEXT="KEY_MACRO8",  .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G09,                .INPUT_EVENT_CODE=KEY_MACRO9,  .INPUT_EVENT_TEXT="KEY_MACRO9",  .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G10,                .INPUT_EVENT_CODE=KEY_MACRO10, .INPUT_EVENT_TEXT="KEY_MACRO10", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G11,                .INPUT_EVENT_CODE=KEY_MACRO11, .INPUT_EVENT_TEXT="KEY_MACRO11", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G12,                .INPUT_EVENT_CODE=KEY_MACRO12, .INPUT_EVENT_TEXT="KEY_MACRO12", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G13,                .INPUT_EVENT_CODE=KEY_MACRO13, .INPUT_EVENT_TEXT="KEY_MACRO13", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G14,                .INPUT_EVENT_CODE=KEY_MACRO14, .INPUT_EVENT_TEXT="KEY_MACRO14", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G15,                .INPUT_EVENT_CODE=KEY_MACRO15, .INPUT_EVENT_TEXT="KEY_MACRO15", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G16,                .INPUT_EVENT_CODE=KEY_MACRO16, .INPUT_EVENT_TEXT="KEY_MACRO16", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G17,                .INPUT_EVENT_CODE=KEY_MACRO17, .INPUT_EVENT_TEXT="KEY_MACRO17", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G18,                .INPUT_EVENT_CODE=KEY_MACRO18, .INPUT_EVENT_TEXT="KEY_MACRO18", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G19,                .INPUT_EVENT_CODE=KEY_MACRO19, .INPUT_EVENT_TEXT="KEY_MACRO19", .mask_size=32 },
    { .bitmask.uint32=LOGITECH_G600_BUTTON_G20,                .INPUT_EVENT_CODE=KEY_MACRO20, .INPUT_EVENT_TEXT="KEY_MACRO20", .mask_size=32 },
};


/**
 * Logitech G600 active state
 */
typedef struct logitech_g600_active_state_e {
    uint8_t type;
    uint32_t button;
    uint8_t profile;
    uint16_t unknown;
} __attribute__((packed)) logitech_g600_active_state_t;


/**
 * Handel button press
 * @param[in] code active button
 * @param[in] current current button state
 * @param[in] previous previous button state
 */
void handel_button(const bagpipe_button_mask_t *button, const uint32_t current, const uint32_t previous, mapper_t *mapper){
    bool current_pressed  = ((current & button->bitmask.uint32) == button->bitmask.uint32);
    bool previous_pressed = ((previous & button->bitmask.uint32) == button->bitmask.uint32);

    if( current_pressed == previous_pressed ) return; // Don't need to do anything. KEY_HOLD

    if ( current_pressed){
        // Button has been pressed
        mapper->map.call(mapper, button, KEY_VALUE_PRESSED);
        return;
    }
    // Button is released
    mapper->map.call(mapper, button, KEY_VALUE_RELEASE);
}

/**
 * Handel G600 State
 * @param[in] current current state information
 * @param[in] previous previous state information
 */
void handel_g600_state(const logitech_g600_active_state_t *current, const logitech_g600_active_state_t *previous, mapper_t *mapper) {
    int index = 0;
    for(; index < LOGITECH_G600_BUTTON_COUNT; index++){
        handel_button( &(LOGITECH_G600_BUTTON_LIST[index]), current->button, previous->button, mapper);
    }
}

/**
 * Move previous state to current.
 * @param[in] current current state information
 * @param[in] previous previous state information
 */
void update_g600_previous_state(const logitech_g600_active_state_t *current, logitech_g600_active_state_t *previous){
    previous->button = current->button;
    previous->profile = current->profile;
    previous->type = current->type;
}
/**
 * read device feature information
 * @param[out] fd
 * @param[in] len size of buffer
 * @param[out] buf buffer object to read in
 * @param[in] report_num Report number
 **/
int read_device(int fd, const size_t len, uint8_t *buf, const unsigned char report_num ){
    uint8_t tmp_buf[4096];
    int result;
    memset(tmp_buf, 0, len);
	tmp_buf[0] = report_num;

    result = read(fd,tmp_buf,sizeof(tmp_buf));
    if (result < 0)
		return -errno;
    memcpy(buf, tmp_buf, result);
	return result;
}

/**
 * Logitech G600
 * @param[in] emitter emitter call function
 */
int logitech_g600_driver(mapper_t *mapper, const char* path){
    logitech_g600_active_state_t current_state;
    logitech_g600_active_state_t previous_state;
    previous_state.button = LOGITECH_G600_BUTTON_CLEAR;
    int result;
    int fd = open(path, O_RDONLY | O_NONBLOCK, 0);

    if (fd < 0){
        fprintf(stderr, "error\n");
        return  false;
    }

    do {
        /*
         * Set the polling rate to be 3 MHZ 
         * TODO: make argument uses a lot more cpu if removed
         */
        usleep(1000 * 3);
        result = read_device(fd, sizeof(current_state), (uint8_t*)&current_state, LOGITECH_G600_CURRENT_STATE);
        if (result == -EAGAIN) continue; // Needed for Non block call O_NONBLOCK. 
        if (result < 0) {
            fprintf(stderr,"Error reading device %d\n" , result);
        }
        if (current_state.type == LOGITECH_G600_STATE_TYPE_BUTTONS){
            handel_g600_state(&current_state, &previous_state, mapper);
            update_g600_previous_state(&current_state, &previous_state);
        }
    } while (online);
    close(fd);
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include <linux/input.h>
#include "hid-descriptor-vtable.h"
#include "hidraw-linux-vtable.h"
#include "yaml-parser.h"

hidraw_input_t* find_input(const char* device_name, hidraw_collection_t* hid_devices){
    uint16_t index =0;
    uint16_t sub_index =0;
    for(;index < hid_devices->size; ++index){
        for(sub_index=0 ; sub_index < hid_devices->items[index].input_count; ++sub_index ){
            if(STRING_STARTS_WITH(device_name, hid_devices->items[index].inputs[sub_index].device_name)) {
                return &hid_devices->items[index].inputs[sub_index];
            }
        }
    }
    return NULL;
}

hidraw_t* find_hid(const char* device_name, hidraw_collection_t* hid_devices){
    uint16_t index =0;
    uint16_t sub_index =0;
    for(;index < hid_devices->size; ++index){
        for(sub_index=0 ; sub_index < hid_devices->items[index].input_count; ++sub_index ){
            if(STRING_STARTS_WITH(device_name,hid_devices->items[index].inputs[sub_index].device_name)) {
                return &hid_devices->items[index];
            }
        }
    }
    return NULL;
}


void* run(void* args){
    hidraw_collection_t *hid_devices = (hidraw_collection_t*) args;
    emitter_t emitter;
    emitter.emit.ptr=emit;
    emitter.fd = create_virtual_device();
    mapper_t  mapper;
    mapper.emitter = &emitter;
    mapper.map.ptr = map;
    hidraw_t *want = find_hid("Logitech Gaming Mouse G600 Keyboard", hid_devices);
    logitech_g600_driver(&mapper, want->hidraw_path);
    free_virtual_device(emitter.fd);
    printf("Watcher Thread has stoped\n");
    return NULL;
}
/**
 * Thread that take a input path (fd) in format: /dev/input/input<N>
 * where <N> is the input that is blocked / grabbed
 * @param[in] path path to dev/input to block
 * @return fd
 */
int block_input(const char* path){
    int fd = open(path, O_RDONLY| O_NONBLOCK, 0);
    if (fd < 0){ fprintf(stderr, "Cant open %s error\n", path);  return -1;}
    if(ioctl(fd, EVIOCGRAB, 1)< 0 ){
        fprintf(stderr, "Can't grab %s\n", path);
        close(fd);
        return -1;
    }
    printf("Blocked[%d] %s \n", fd, path);
    return fd;
}

int hid(const char* path, hidraw_collection_t* hid_devices){
    char* lib_path = get_lib_path(path, "/lib.hid.descriptor.so");
	printf("lib path: %s\n", lib_path);
	void* lib_ptr  = load_library(lib_path);
	free(lib_path);
	lib_path= NULL;
	libhid_descriptor_virtual_table vtable;
	libhid_descriptor_virtual_table_load(lib_ptr, &vtable);
	if( libhid_descriptor_virtual_table_validate(&vtable)){
		fprintf(stderr, "vtable validation failed\n");
		return 1;
	}

	lib_path = get_lib_path(path, "/lib.hid.rawlinux.so");
	printf("lib path: %s\n", lib_path);
	void* lib_hidraw_linux = load_library(lib_path);
	free(lib_path);
	hidraw_virtual_table vtable_hidraw;
	hidraw_virtual_table_load(lib_hidraw_linux, &vtable_hidraw);
	if(hidraw_virtual_table_validate(&vtable_hidraw)){
		fprintf(stderr, "vtable validation failed\n");
		return 1;
	}


	vtable_hidraw.hidraw_collection_create.call(hid_devices);
	printf("Hid Device Count: %d\n", hid_devices->size);
    return 0;
}

bool load_profile(const char* config_path){
    int index =0, config_index;
    bagpipe_profile_t result;
    bagpipe_profile_initialize(&result);
    if(!bagpipe_load_profile(&result, config_path)) return false;
    printf("Loaded Profile %s\n", config_path);
    print_profile(&result);
    for(; index < LOGITECH_G600_BUTTON_COUNT; index++){
        bagpipe_macro_config_t* macro_config = bagpipe_macro_config_create();
        LOGITECH_G600_BUTTON_LIST[index].macro_config = macro_config;
        macro_config->key = LOGITECH_G600_BUTTON_LIST[index].INPUT_EVENT_TEXT;
        for(config_index=0; config_index < result.button_count; config_index++){
            if(!STRING_EQUALS(LOGITECH_G600_BUTTON_LIST[index].INPUT_EVENT_TEXT, result.buttons[config_index].key )){
                continue;
            }
            macro_config->modifier = result.buttons[config_index].modifier;
            if (result.buttons[config_index].modifier) continue; // If modifier is set then macro will not be used
            macro_config->press = bagpipe_macro_create(result.buttons[config_index].press);
            macro_config->release = bagpipe_macro_create(result.buttons[config_index].release);
            macro_config->modifier_press = bagpipe_macro_create(result.buttons[config_index].modifier_press);
            macro_config->modifier_release = bagpipe_macro_create(result.buttons[config_index].modifier_release);
            
        }
    }
    bagpipe_profile_delete(&result);
    return true;
}

void destroy_profile(){
    int index =0;
    for(; index < LOGITECH_G600_BUTTON_COUNT; index++){
        bagpipe_macro_destroy(LOGITECH_G600_BUTTON_LIST[index].macro_config->press );
        bagpipe_macro_destroy(LOGITECH_G600_BUTTON_LIST[index].macro_config->release);   
        bagpipe_macro_destroy(LOGITECH_G600_BUTTON_LIST[index].macro_config->modifier_press);        
        bagpipe_macro_destroy(LOGITECH_G600_BUTTON_LIST[index].macro_config->modifier_release);
    }
}

void stop(int stop_code) {
    puts("");
    online = false;
}

void block_inputs(hidraw_t *want, int * block, int * block_count ){
    int count =0;
    printf("Blocking inputs: %d\n", want->input_count);
    for(count=0 ; count < want->input_count; ++count ){
        block[*block_count] = block_input(want->inputs[count].event_path);
        *block_count += 1;
    }
}
/**
 * Takes vector of file descriptors and closes them
 * @param[out] block vector
 * @param[in] block number of elements in vector
 */
void unblock_inputs(int * block, const int * block_count){
    int count =0;
    int block_id = -1;
    for(count=0 ; count < *block_count; ++count ){
        block_id = block[count];
        if(block_id > 0){
            close(block_id);
            printf("UnBlocked[%d]\n", block_id);
        }
    }
}

int main(int argc, char* argv[]){
    hidraw_collection_t hid_devices;
    if (argc < 2){
        printf("Please Enter path to yaml config file\n");
        return 1;
    }
    
    if(hid(argv[0], &hid_devices)) {
        printf("Problem with hid device\n");
        return 1;
    }
    if (!load_profile(argv[1])){
        printf("Problem Reading Profile\n");
        return 1;
    }
    hidraw_t *want = find_hid("Logitech Gaming Mouse G600 Keyboard", &hid_devices);
    hidraw_input_t* index = find_input("Logitech Gaming Mouse G600 Keyboard", &hid_devices);
    int block [5] = {-1,-1,-1,-1,-1};
    int block_count = 0;
   
    if (index == NULL){
        printf("Problem searching for device\n");
        return 1;
    }
    printf("%s\n", index->device_name);
    block_inputs(want, block, &block_count);

    pthread_t thread_id_2;
    pthread_create(&thread_id_2, NULL, run, (void*) &hid_devices);

    puts("Press clt + c to quit");
    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    signal(SIGQUIT, stop);
    signal(SIGABRT, stop); 
   
    pthread_join(thread_id_2, NULL);
    unblock_inputs(block, &block_count);
    destroy_profile();
  return 0;
}