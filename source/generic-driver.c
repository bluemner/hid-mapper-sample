#include "generic-driver.h"

static volatile bool online = true;

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

int main(int argc, char* argv[]){
  hidraw_collection_t hid_devices;
  hidraw_t *want = find_hid("Logitech Gaming Mouse G600 Keyboard", &hid_devices);
  return 1;
}