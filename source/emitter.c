#include "emitter.h"

/**
 * Emit raw
 * @param[out] fd file descriptor 
 * @param[in] type EV_* codes from linux/input-event-codes.h
 * @param[in] code KEY_* codes from linux/input-event-codes.h
 * @param[in] val key_value_e code
 */
void emit_raw(int fd, const uint16_t type, const uint16_t code, const int32_t val)
{
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    /* timestamp values below are ignored */
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    if (write(fd, &ie, sizeof(ie)) < 0)
    {
        printf("Failed to write key %d\n", code);
    }
}


void emit(int fd, const uint16_t type, const uint16_t code, const int32_t val){
    emit_raw(fd, type, code, val);
    emit_raw(fd, EV_SYN, SYN_REPORT, 0);
}


bool modifier(uint16_t key)
{
    return (
        key == KEY_LEFTSHIFT  ||
        key == KEY_LEFTALT    ||
        key == KEY_LEFTCTRL   ||
        key == KEY_LEFTMETA   ||
        key == KEY_RIGHTSHIFT ||
        key == KEY_RIGHTALT   ||
        key == KEY_RIGHTCTRL  ||
        key == KEY_RIGHTMETA  || 
        key == KEY_FN         ||
        key == KEY_MACRO
    );
}

typedef struct key_modifier_state_e
{
    key_value_t state;
    unsigned int key;
} key_modifier_state_t;

#define STATE_MODIFIERS_CNT 10

/**
 * Keeps track of modifier key state
 */
key_modifier_state_t modifiers_state[STATE_MODIFIERS_CNT] = {
    {KEY_VALUE_RELEASE, KEY_MACRO},
    {KEY_VALUE_RELEASE, KEY_LEFTSHIFT},
    {KEY_VALUE_RELEASE, KEY_LEFTALT},
    {KEY_VALUE_RELEASE, KEY_LEFTCTRL},
    {KEY_VALUE_RELEASE, KEY_LEFTMETA},
    {KEY_VALUE_RELEASE, KEY_FN},
    {KEY_VALUE_RELEASE, KEY_RIGHTSHIFT},
    {KEY_VALUE_RELEASE, KEY_RIGHTALT},
    {KEY_VALUE_RELEASE, KEY_RIGHTCTRL},
    {KEY_VALUE_RELEASE, KEY_RIGHTMETA},
};

/**
 * Handel modifier keys
 */
void handel_modifier(const int fd, const uint16_t key)
{
    unsigned short index;
    for (index = 0; index < STATE_MODIFIERS_CNT; ++index)
    {
        if (key != modifiers_state[index].key)
            continue;
        if (modifiers_state[index].state == KEY_VALUE_RELEASE)
        {
            modifiers_state[index].state = KEY_VALUE_PRESSED;
            emit(fd, EV_KEY, key, (int)KEY_VALUE_PRESSED);
            emit(fd, EV_SYN, SYN_REPORT, 0);
            return;
        }

        if (modifiers_state[index].state == KEY_VALUE_PRESSED)
        {
            modifiers_state[index].state = KEY_VALUE_RELEASE;
            emit(fd, EV_KEY, key, (int)KEY_VALUE_RELEASE);
            emit(fd, EV_SYN, SYN_REPORT, 0);
            return;
        }
    }
}

void create_virtual_device_mouse(int fd){

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
 
    unsigned int index;

    /**
     * Initialize all the keys that are in the range
     */
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(fd, UI_SET_KEYBIT, BTN_BACK);
    ioctl(fd, UI_SET_KEYBIT, BTN_FORWARD);
    ioctl(fd, UI_SET_KEYBIT, BTN_PINKIE);
    ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(fd, UI_SET_KEYBIT, BTN_SIDE);
    ioctl(fd, UI_SET_KEYBIT, BTN_EXTRA);
    ioctl(fd, UI_SET_KEYBIT, BTN_FORWARD);
    ioctl(fd, UI_SET_KEYBIT, BTN_BACK);
    ioctl(fd, UI_SET_KEYBIT, BTN_TASK);
    
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    for (index = 0; index < KEY_MAX; ++index)
        ioctl(fd, UI_SET_KEYBIT, index);
}


int create_virtual_device()
{
    struct uinput_setup input_setup;
    int fd;
    if ((fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0)
    {
        printf("Something went wrong creating uinput\n");
        return fd;
    }

    /*
     * The ioctls below will enable the device that is about to be
     * created, to pass key events, in this case the space key.
     */
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    unsigned int index;

    /**
     * Initialize all the keys that are in the range
     */
    for (index = 0; index < KEY_MAX; ++index)
        if (key_name(index) != NULL)
            ioctl(fd, UI_SET_KEYBIT, index);

    memset(&input_setup, 0, sizeof(input_setup));
    input_setup.id.bustype = BUS_USB;
    input_setup.id.vendor = 0x0000;  /* sample vendor */
    input_setup.id.product = 0x0000; /* sample product */
    input_setup.id.version = 0x0001;
    strcpy(input_setup.name, "Logitech Gaming Mouse G600");
    ioctl(fd, UI_DEV_SETUP, &input_setup);
    ioctl(fd, UI_DEV_CREATE);
    return fd;
}


void free_virtual_device(int fd)
{
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}
