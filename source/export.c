#include "hid-descriptor-vtable.h"
#include "hidraw-linux-vtable.h"

void handel_descriptor(hid_descriptor_t *descriptor){
  uint32_t i, j;
  printf("------------------------------------\n");
	for(i =0; i < descriptor->items_size; i++){
    printf("type: %d  value:%d %s %s\n", descriptor->items[i].type , descriptor->items[i].value , descriptor->items[i].text, descriptor->items[i].subtext);
  }
}
void profile_export(hidraw_collection_t *collection, libhid_descriptor_virtual_table *vtable)
{
	int i= 0, j=0;
	hidraw_t* current;
	hidraw_input_t* current_input;
	libhid_report_descriptor_t report_descriptor;
	for(; i< collection->size; i++ ){
		current = &(collection->items[i]);
		if (current == NULL || current->name == NULL) continue;
		printf("%s\n", current->name);
		printf("\tHid Path: %s\n", current->hidraw_path);
		printf("\tHid Descriptor Size: %d\n", current->hid_raw_descriptor.size);
		// printf("\tHid Descriptor:\n\t\t");
		// for(j=0; j< current->hid_raw_descriptor.size; j++ ){
		// 	printf("%02hhx ", current->hid_raw_descriptor.value[j] );
		// 	if( ((j +1) % 10) == 0 && j+1 > 1 ) printf("\n\t\t");
		// }
		// printf("\n");
		hid_descriptor_t descriptor;
		printf("\tHid Profile:\n");
		report_descriptor.size = current->hid_raw_descriptor.size;
		for(j=0; j < report_descriptor.size; j++)
			report_descriptor.value[j] = current->hid_raw_descriptor.value[j];
			
		if( current->hid_raw_descriptor.size > 0 && vtable->libhid_descriptor_create.call(&report_descriptor,&descriptor) > -1){
			//vtable->libhid_descriptor_print_tab.call(&descriptor, 4*2);
      handel_descriptor(&descriptor);
			vtable->libhid_descriptor_destroy.call(&descriptor);
		}
		printf("\tInput Count: %d\n", current->input_count);
		for(j=0; j< current->input_count; j++ ){
			current_input = &(current->inputs[j]);
			printf("\t\tIndex: %d\n", j);
			printf("\t\tInput: %s\n", current_input-> input);
			printf("\t\tEvent Path: %s\n", current_input-> event_path);
			printf("\t\tDevice Name: %s", current_input-> device_name);
			printf("\t\tId: \n");
			printf("\t\t  Bustype: 0x%04hx\n", current_input->id.bustype);
			printf("\t\t  Product: 0x%04hx\n", current_input->id.product);
			printf("\t\t  Vendor:  0x%04hx\n", current_input->id.vendor);
			printf("\t\t  Version: 0x%04hx\n", current_input->id.version);
		}
	}
}

int load_descriptor_virtual_table(const char *path, libhid_descriptor_virtual_table *vtable){
  char *lib_path = get_lib_path(path, "/lib.hid.descriptor.so");
  printf("lib path: %s\n", lib_path);
  void *lib_ptr = load_library(lib_path);
  free(lib_path);
  lib_path = NULL;  
  libhid_descriptor_virtual_table_load(lib_ptr, vtable);  
  return libhid_descriptor_virtual_table_validate(vtable);
}

int load_hidraw_virtual_table(const char *path, hidraw_virtual_table *vtable_hidraw)
{
  char *lib_path = get_lib_path(path, "/lib.hid.rawlinux.so");
  printf("lib path: %s\n", lib_path);
  void *lib_hidraw_linux = load_library(lib_path);
  free(lib_path);
  hidraw_virtual_table_load(lib_hidraw_linux, vtable_hidraw);
  return hidraw_virtual_table_validate(vtable_hidraw);
}


int main(int argc, char *argv[])
{
  hidraw_collection_t hid_devices;
  hidraw_virtual_table vtable_hidraw;
  libhid_descriptor_virtual_table vtable_descriptor;
  if (load_hidraw_virtual_table(argv[0], &vtable_hidraw))
  {
    printf("Problem with hid device\n");
    return 1;
  }
  if (load_descriptor_virtual_table(argv[0], &vtable_descriptor ))
  {
      printf("Problem with hid device\n");
      return 1;
  }
  vtable_hidraw.hidraw_collection_create.call(&hid_devices);
  printf("Hid Device Count: %d\n", hid_devices.size);
  profile_export(&hid_devices, &vtable_descriptor);
  vtable_hidraw.hidraw_collection_delete.call(&hid_devices);
  return 0;
}