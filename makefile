PKGCONFIG=pkg-config
OS=GNU/Linux
CC=gcc
CFLAGS=-Isrc -Iincl -O3 -pipe -g -pg -Wall -Wno-unused-variable -Werror `$(PKGCONFIG) --cflags glew glfw3 libpng libxml-2.0 lua`
WINFLAGS=-m32 -Wl,-subsystem,windows -static-libgcc -DWINDOWS
LINUXFLAGS=
CLIBS=
WINLIBS=`$(PKGCONFIG) --libs glew ` -Wl,-Bstatic `$(PKGCONFIG) --static --libs gl glfw3 libpng libxml-2.0 lua`
LINUXLIBS=`$(PKGCONFIG) --libs --static glew glfw3 libpng libxml-2.0 lua`
SRCPATH=src/
OBJPATH=obj/

SRCS:=$(wildcard src/*.c)
EDITOR_SRCS:=$(wildcard src/editor/*.c) $(SRCS)
GAME_SRCS:=$(wildcard src/game/*.c) $(SRCS)

#OBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(wildcard $(SRCPATH)*.c))
EDITOR_OBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(EDITOR_SRCS))
GAME_OBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(GAME_SRCS))

#DEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(wildcard $(SRCPATH)*.c))
EDITOR_DEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(EDITOR_SRCS))
GAME_DEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(GAME_SRCS))

ifeq ($(OS), GNU/Linux)
OSFLAGS=$(LINUXFLAGS)
OSLIBS=$(LINUXLIBS)
else ifeq ($(OS), Windows)
OSFLAGS=$(WINFLAGS)
OSLIBS=$(WINLIBS)
CC=i686-w64-mingw32-gcc
APP := $(APP).exe
else
$(error Unknown OS selected for build output)
endif

$(OBJPATH)%.depend: $(SRCPATH)%.c
	@echo -e "Building dependecies for \e[1;35m$<\e[0m..."
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $(OSFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJPATH)%.o: $(SRCPATH)%.c
	@echo -e "Building \e[1;35m$<\e[0m..."
	@$(CC) -c $< -o $@ $(CFLAGS) $(OSFLAGS)


all: editor game

editor: $(EDITOR_DEPS) $(EDITOR_SRCS) $(EDITOR_OBJS)
	@echo -e "\e[0;33mBuilding  editor for \e[1;35m$(OS)\e[0;33m systems...\e[0m"
	@$(CC) -o editor $(EDITOR_OBJS) $(CFLAGS) $(OSFLAGS) $(CLIBS) $(OSLIBS)
	@echo -e "\e[0;32mBuild completed.\e[0m"

game: $(GAME_DEPS) $(GAME_SRCS) $(GAME_OBJS)
	@echo -e "\e[0;33mBuilding game for \e[1;35m$(OS)\e[0;33m systems...\e[0m"
	@$(CC) -o game $(GAME_OBJS) $(CFLAGS) $(OSFLAGS) $(CLIBS) $(OSLIBS)
	@echo -e "\e[0;32mBuild completed.\e[0m"

#TODO: rewrite this bit
release:
	@echo -e "\e[0;31mCopying relevant files...\e[0m"
	@mkdir -p bin
	@cp -r data bin
	@cp $(APP) bin
	@echo -e "\e[0;32mSuccess! You can find the new files in the bin folder.\e[0m"


#TODO: rewrite this bit
-include $(EDITOR_DEPS)
-include $(GAME_DEPS)
$(shell mkdir -p obj)

#TODO: rewrite this bit
.PHONY: clean
clean:
	@echo -e "\e[0;31mCleaning up...\e[0m"
	$(RM) $(EDITOR_OBJS)
	$(RM) $(GAME_OBJS)
	$(RM) $(EDITOR_DEPS)
	$(RM) $(GAME_DEPS)
	$(RM) $(APP)
	@echo -e "\e[0;32mCleanup completed.\e[0m"

