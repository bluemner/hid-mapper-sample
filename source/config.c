# include "config.h"




int bagpipe_profile_map_item_initialize(bagpipe_profile_map_item_t *item){
    item->key = NULL;
    item->press = NULL;
    item->release = NULL;
    item->modifier = false;
    item->modifier_press = NULL;
    item->modifier_release = NULL;
    return true;
}

void bagpipe_profile_map_item_delete(bagpipe_profile_map_item_t *item){
    if (item->key != NULL){
        free(item->key);
        item->key = NULL;
    }
    if (item->press != NULL){
        free(item->press);
        item->press = NULL;
    }
    if (item->release != NULL){
        free(item->release);
        item->release = NULL;
    }
    if (item->modifier_press != NULL){
        free(item->modifier_press);
        item->modifier_press = NULL;
    }
    if (item->modifier_release != NULL){
        free(item->modifier_release);
        item->modifier_release = NULL;
    }
    //item = NULL;
}

int bagpipe_profile_initialize(bagpipe_profile_t *item){
    item->name = NULL;
    item->description = NULL;
    item->vendor = NULL;
    item->product = NULL;
    item->button_count=0;
    item->bustype_id = 0;
    item->product_id = 0;
    item->vendor_id = 0;
    item->version_id = 0;
    int index=0;
    for (; index < BAGPIPE_MAX_MAP_COUNT; index++){
        bagpipe_profile_map_item_initialize(&(item->buttons[index]));
    }
    return true;
}

void bagpipe_profile_delete(bagpipe_profile_t *item){
    puts("bagpipe_profile_delete");
    free_ptr(item->name);
    free_ptr(item->description);
    free_ptr(item->vendor);
    free_ptr(item->product);  
    int index=0;
    for (; index < BAGPIPE_MAX_MAP_COUNT; index++){
        bagpipe_profile_map_item_delete(&(item->buttons[index]));
    }
}



void print_profile_buttons(bagpipe_profile_map_item_t *item){
    printf("\t\tkey: %s\n", item->key);
    printf("\t\tpress: %s\n", item->press);
    printf("\t\trelease: %s\n", item->release);
    printf("\t\tmodifier: %d\n", item->modifier);
    printf("\t\tmodifier_press: %s\n", item->modifier_press);
    printf("\t\tmodifier_release: %s\n", item->modifier_release);
}

void print_profile(bagpipe_profile_t *profile){
    printf("Profile:\n");
    printf("\tName: %s\n",profile->name);
    printf("\tdescription: %s\n",profile->description );
    printf("\tproduct: %s\n",profile->product );
    printf("\tvendor: %s\n",profile->vendor );
    printf("\tbustype_id: 0x%04hx\n", profile->bustype_id);
    printf("\tproduct_id: 0x%04hx\n", profile->product_id);
    printf("\tvendor_id:  0x%04hx\n", profile->vendor_id);
    printf("\tversion_id: 0x%04hx\n", profile->version_id);
    printf("\tbutton_count: %d\n", profile->button_count );
    int index;         
    for (index=0; index < profile->button_count; index++){
        printf("Button[%d]\n", index );
        print_profile_buttons(&(profile->buttons[index]));

    }
}
