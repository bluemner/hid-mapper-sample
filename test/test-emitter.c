#include "emitter.h"
#include <assert.h>

int main(void)
{
    int fd = create_virtual_device();
    assert( fd > -1 && "Descriptor should be positive");
 
    /* Key press, report the event, send key release, and report again */
    sleep(2);
    emit(fd, EV_KEY, KEY_LEFTSHIFT, (int)KEY_VALUE_PRESSED);
    emit(fd, EV_KEY, KEY_LEFTSHIFT, (int)KEY_VALUE_HOLD);
    emit(fd, EV_KEY, KEY_A, (int)KEY_VALUE_PRESSED);
    emit(fd, EV_KEY, KEY_A, (int)KEY_VALUE_RELEASE);
    emit(fd, EV_KEY, KEY_B, (int)KEY_VALUE_PRESSED);
    emit(fd, EV_KEY, KEY_B, (int)KEY_VALUE_RELEASE);
    emit(fd, EV_KEY, KEY_LEFTSHIFT, (int)KEY_VALUE_RELEASE);

    /*
     * Give userspace some time to read the events before we destroy the
     * device with UI_DEV_DESTOY.
     */
    sleep(2);
    free_virtual_device(fd);

    return 0;
}