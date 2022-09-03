# Diagrams
* [Overview](#overview)
* [Keys](#keys)
* [Macro](#macro)
* [Emitter](#emitter)
* [Mapper](#mapper)
* [Config](#config)

# Overview
```mermaid
flowchart LR
  driver   --o hid-descriptor
  driver   --o mapper 
  driver   --x grab-input
  
  mapper   --o emitter
  mapper --o config
  
  mapper   --o macro
  emitter  --o keys
  macro    --o keys
```
[↥ TOP ↥](#diagrams)

# Keys
Gets the linux key codes and names and checks the validity of the keys

[↥ TOP ↥](#diagrams)
# Macro
Loads text based macros for program that simulate button functionality this will map a key press to the correct macro state. 

* `KEY_MACRO11` this is pressed than release
* `+KEY_MACRO11` this is pressed
* `-KEY_MACRO11` this is release

```yaml
    key: "BTN_LEFT" 
    press: "+BTN_LEFT"
    release: "-BTN_LEFT"
    modifier: false # if set to true all other fields but key will be ignored
    modifier_press: null
    modifier_release: null
```


```mermaid
classDiagram
  class bagpipe_macro_key_state_t {
      <<enumeration>>
      MACRO_KEY_VALUE_RELEASE = 0
      MACRO_KEY_VALUE_PRESSED = 1
      MACRO_KEY_VALUE_BOTH = 2
  } 

  class bagpipe_macro_item_t{
      int code
      char* text
      bagpipe_macro_key_state_t value
  } 

  class bagpipe_macro_t{
      bagpipe_macro_item_t* macros
      uint16_t count
  }
  class bagpipe_macro_config_t{
      char* key
      bagpipe_macro_t* press
      bagpipe_macro_t* release 
      bagpipe_macro_t* modifier_press 
      bagpipe_macro_t* modifier_release  
      bool modifier 
  }
  bagpipe_macro_config_t --* bagpipe_macro_t
   bagpipe_macro_item_t *-- bagpipe_macro_t
  bagpipe_macro_item_t --* bagpipe_macro_key_state_t

```
[↥ TOP ↥](#diagrams)

# Emitter
An [emitter](source/emitter.c) logic implements the linux [uinput module](https://www.kernel.org/doc/html/v4.12/input/uinput.html) and send virtual keyboard codes.

```mermaid
classDiagram

    class key_value_t {
        <<enumeration>>
        KEY_VALUE_RELEASE = 0
        KEY_VALUE_PRESSED = 1
        KEY_VALUE_HOLD = 2
    }
    
    class emit_fn {
      void *ptr;
      void *call(int fd, const uint16_t type, const uint16_t code, const int32_t val);
    }
    class emitter_t {
        int fd
        emit_fn emit
        bool modifier_active
    }
    emitter_t --* emit_fn
  
```

call takes 

* `event type` from [input-event-codes.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h)

* `key code` from [input-event-codes.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h)
* `val` from `key_value_t` for the state

[↥ TOP ↥](#diagrams)

# Mapper

Mapper takes the `emitter` and `profile` and creates a mask (`bagpipe_button_mask_t`) to map off of.

The mask contains a list `bagpipe_macro_config_t` which can be passed into the emitter. 



```c
typedef struct bagpipe_button_mask
{
  union {
    uint8_t  uint8;
    uint16_t uint16;
    uint32_t uint32;
    uint64_t uint64;
  } bitmask;
  uint8_t mask_size;
  uint32_t INPUT_EVENT_CODE; // <linux/input-event-codes.h>
  char * INPUT_EVENT_TEXT;
  bagpipe_macro_config_t *macro_config;
} bagpipe_button_mask_t;

typedef union map_fn
{
    void *ptr;
    int (*call) (mapper_t *mapper, const bagpipe_button_mask_t *button, key_value_t value);
} map_fn;

typedef struct mapper{
    bagpipe_profile_t *profile;
    map_fn map;
    emitter_t *emitter;
} mapper_t;
```

when creating a mapper instance you can map to two functions 

```c
void map(mapper_t *mapper, const bagpipe_button_mask_t *button, key_value_t value);
void no_map(mapper_t *mapper, const bagpipe_button_mask_t *button, key_value_t value);
```

`map` will run the mapping logic
`no map` will just fire the default key value. This was added for future use of enabled profiles.



[↥ TOP ↥](#diagrams)
# Config

The objective of the config is to load a config from say yaml into the `bagpipe_profile_t` structure.

Buttons ins the config will be created to macros

```mermaid
classDiagram



  class bagpipe_profile_t{
    char* name
    char* description
    char* vendor    
    char* product
    uint16_t bustype_id
    uint16_t product_id
    uint16_t vendor_id
    uint16_t version_id
    bagpipe_macro_config_t buttons[BAGPIPE_MAX_MAP_COUNT]
    uint32_t button_count


    int bagpipe_profile_initialize(bagpipe_profile_t *item)
    void bagpipe_profile_delete(bagpipe_profile_t *item)
    void print_profile(bagpipe_profile_t *profile)
  } 


  class yaml_parser{
    int bagpipe_load_profile(bagpipe_profile_t *result, const char * file)
  }

  bagpipe_profile_map_item_t *-- bagpipe_profile_t  
  bagpipe_profile_t --|> yaml_parser
```
[↥ TOP ↥](#diagrams)
