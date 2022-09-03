#ifndef __BAGPIPE_EMITTER_H__
#define __BAGPIPE_EMITTER_H__

#include <stdint.h>
#include <stdbool.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <keys.h>

typedef enum key_value_e {
    KEY_VALUE_RELEASE = 0,
    KEY_VALUE_PRESSED = 1,
    KEY_VALUE_HOLD = 2
} key_value_t;

typedef struct emitter emitter_t;
typedef union emit_fn
{
  void *ptr; // pointer to emitter function used if using dynamic links
  void (*call)(int fd, const uint16_t type, const uint16_t code, const int32_t val);
} emit_fn;

typedef struct emitter {
    int fd;
    emit_fn emit;
    bool modifier_active;
} emitter_t;


/**
 * @param[in] fd descriptor the virtual device
 * @param[in] type Event type list in linux/input-event-codes.h
 * @param[in] code Key code from linux/input-event-codes.h
 * @param[in] value Key value release: 0; pressed: 1; hold: 2
 */
void emit(int fd, const uint16_t type, const uint16_t code, const int32_t value);

/**
 * Modifier
 * @returns true if key is a modifier key
*/
bool modifier(uint16_t key);

/**
 * @param[in] fd descriptor the virtual device
 * @param[in] key key code
 */
void handel_modifier(const int fd, const uint16_t key);

/**
 * Create Virtual Device
 * @returns descriptor number 
 */
int create_virtual_device();

/**
 * Free virtual device
 */
void free_virtual_device(int fd);
#endif