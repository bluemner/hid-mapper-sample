OUT:=bin
DEBUG_OUT:=$(OUT)/debug
RELESE_OUT:=$(OUT)/release

CPP_COMPILER:=g++ -std=c++17
C_COMPILER:=gcc -std=gnu17
SOURCE:=source/
TEST:=test/

LIBRARY_FLAGS:=-fPIC -shared
DEBUG_FLAGS:=-Wall -g -ggdb -fsanitize=address
DYNAMIC:= -rdynamic
HEADERS_DIR = include/
INC:=-I $(HEADERS_DIR)

# ADD Any Submodules to phone to build
.PHONY: libhid-descriptor

all: hidraw-linux-util libs

dir:
	mkdir -p $(OUT)
	mkdir -p $(DEBUG_OUT)
	mkdir -p $(RELESE_OUT)

build_c_release: $(SOURCE)/$(NAME).c | dir
	$(C_COMPILER) $(INC)  $(SOURCE)$(NAME).c $(OTHER) $(LIBS) $(EXTRA) -o $(RELESE_OUT)/$(NAME)

build_c_release_lib: $(SOURCE)/$(NAME).c | dir
	$(C_COMPILER) -shared $(DYNAMIC) $(LIBRARY_FLAGS) $(INC) $(SOURCE)$(NAME).c $(LIBS) $(EXTRA) -o $(RELESE_OUT)/$(LIB_NAME).so

build_c_debug: $(SOURCE)/$(NAME).c | dir
	$(C_COMPILER) $(DEBUG_FLAGS) $(INC)  $(SOURCE)$(NAME).c $(OTHER) $(LIBS) $(EXTRA) -o $(DEBUG_OUT)/$(NAME)

build_test_c_debug: $(TEST)/$(NAME).c | dir
	$(C_COMPILER) $(DEBUG_FLAGS) $(INC) $(TEST)$(NAME).c $(OTHER) $(LIBS) $(EXTRA) -o $(OUT)/$(NAME)

build_c_debug_lib: $(SOURCE)/$(NAME).c | dir
	$(C_COMPILER) $(DEBUG_FLAGS) -shared $(DYNAMIC) $(LIBRARY_FLAGS) $(INC) $(SOURCE)$(NAME).c $(OTHER) $(LIBS) $(EXTRA) -o $(OUT)/$(LIB_NAME).so


############################
libs:

test: test-macro test-keys

test-macro:
	make NAME=test-macro OTHER="$(SOURCE)macro.c $(SOURCE)keys.c" build_test_c_debug

test-keys:
	make NAME=test-keys OTHER="$(SOURCE)keys.c" build_test_c_debug

test-emitter:
	make NAME=test-emitter OTHER="$(SOURCE)emitter.c $(SOURCE)keys.c" build_test_c_debug

libhid-descriptor:
	cd libhid-descriptor && make libs
	cp libhid-descriptor/$(DEBUG_OUT)/*.so  $(DEBUG_OUT)/.
	cp libhid-descriptor/$(RELESE_OUT)/*.so  $(RELESE_OUT)/.

g600: g600-debug g600-release

G600_FILES:= $(SOURCE)macro.c $(SOURCE)keys.c $(SOURCE)mapper.c $(SOURCE)emitter.c $(SOURCE)config.c $(SOURCE)yaml-parser.c

g600-debug: dir libhid-descriptor
	make INC="$(INC) -I libhid-descriptor/include" NAME=g600-linux-driver LIBS="-lpthread -lyaml" OTHER="$(G600_FILES)"  build_c_debug
g600-release: dir libhid-descriptor
	make INC="$(INC) -I libhid-descriptor/include" NAME=g600-linux-driver LIBS="-lpthread -lyaml" OTHER="$(G600_FILES)"  build_c_release

exp: libhid-descriptor
	make INC="$(INC) -I libhid-descriptor/include" NAME=export build_c_debug