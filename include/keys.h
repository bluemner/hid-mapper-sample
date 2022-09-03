#ifndef LIBKEYS
#define LIBKEYS

#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#ifndef STRING_EQUALS
#define STRING_EQUALS(lhs, rhs) strcmp(lhs, rhs) == 0 
#endif
/**
 * Get the key name from key code 
 * 
 *  The following have multiple mappings only one result is returned
 *      KEY_HANGUEL		KEY_HANGEUL
 *      KEY_SCREENLOCK		KEY_COFFEE
 *      KEY_DIRECTION		KEY_ROTATE_DISPLAY
 *      KEY_BRIGHTNESS_ZERO		KEY_BRIGHTNESS_AUTO
 *      KEY_WIMAX		KEY_WWAN
 *      BTN_A		BTN_SOUTH
 *      BTN_B		BTN_EAST
 *      BTN_X		BTN_NORTH
 *      BTN_Y		BTN_WEST
 *      KEY_ZOOM		KEY_FULL_SCREEN
 *      KEY_SCREEN		KEY_ASPECT_RATIO
 *      KEY_BRIGHTNESS_TOGGLE		KEY_DISPLAYTOGGLE
 * 
 *  @returns string value of key based on first marco appearance.
 */ 
const char* key_name(const unsigned int key_code);
/**
 * Get the key code from the macro name defined in linux/input-event-codes.h
 *  @see https://www.kernel.org/doc/html/v4.14/input/event-codes.html
 *  @param[in] key string value that matchs the macro in linux/input-event-codes.h
 *  @returns the key code or negative value if not found
 */
int key_code(const char* key);
/**
 * Checks to see if the bit is enable.
 * this can be used to key if a KEY is avaliable for use by a device
 * @param[in] bit - the bit you wan to check in the array
 * @param[in] array - array of data from ioctl(fd, EVIOCGBIT(EV_??,MAX) , array)
 */
bool enabled_code(unsigned int bit, unsigned long *array);
/**
 * Get valid keys for input
 * @param[in] input - input string for open which gives the file descriptor (fd)
 * @param[out] ouput - output array with 0 if disable 1 if enabled for each KEY. The size must be at least KEY_MAX.
 *                     using KEY_CNT if unsure.
 * @param[in] output_size - provide the size(length) of the output array
 * @returns 0 - Success;
 *          1 - error opening the fd;
 *          2 - output size is too small
 */
int get_valid_keys(const char *input, bool *output, const int output_size);
/**
 * @param[in] input - input string for open which gives the file descriptor (fd)
 * @param[out] output - output array with 0 if disable 1 if enabled for each REL. The size must be at least REL_MAX.
 *                     using REL_CNT if unsure.
 * @param[in] output_size - provide the size(length) of the output array
 * @returns 0 - Success; *
 *          1 - error opening the fd;
 *          2 - output size is too small;
 */
int get_valid_relative_axis(const char *input, bool *output, const int output_size);
/**
 * Get the max value based on event type
 * example EV_KEYS will return KEY_MAX.
 * @note Some event types may not be suppoted on previous linux kernal
 *       versions
 * @param[in] event_type linux kernal event as: EV_* from input-event-codes.h
 * @returns the max event type max value based on the event type.
 *
 */
unsigned int get_event_type_max(const unsigned char event_type);
/**
 * @param[in] input - input string for open which gives the file descriptor (fd)
 * @param[out] output - output array with 0 if disable 1 if enabled for each REL. The size must be at least REL_MAX.
 *                     using REL_CNT if unsure.
 * @param[in] output_size - provide the size(length) of the output array
 * @param[in] event_type linux kernal event as: EV_* from input-event-codes.h
 * @returns 0 - Success; *
 *          1 - error opening the fd; Some systems require sudo or users to be in input group.
 *          2 - output size is too small;
 */
int get_valid(const char *input, bool *output, const int output_size, const unsigned char event_type);
#endif