# Compilers
CC=gcc
VALAC=valac
PKGCONFIG=pkg-config

# Targets
GAMETARGET=halberd-game
EDITORTARGET=halberd-editor
LIBGAMETARGET=libhalberd-game
LIBEDITORTARGET=libhalberd-editor

# Filepaths
SRCPATH=src/
OBJPATH=obj/
LIBPATH=lib/
VAPIPATH=vapi/

# Dependency lists
GAMESRCS:=$(wildcard $(SRCPATH)$(GAMETARGET)/*.c)
EDITORSRCS:=$(wildcard $(SRCPATH)$(EDITORTARGET)/*.vala)
LIBGAMESRCS:=$(wildcard $(SRCPATH)$(LIBGAMETARGET)/*.c)
LIBEDITORSRCS:=$(wildcard $(SRCPATH)$(LIBEDITORTARGET)/*.c)

GAMEOBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(GAMESRCS))
EDITOROBJS:=$(patsubst $(SRCPATH)%.vala,$(OBJPATH)%.vala.o,$(EDITORSRCS))
LIBGAMEOBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(LIBGAMESRCS))
LIBEDITOROBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(LIBEDITORSRCS))

GAMEDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(GAMESRCS))
EDITORDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(EDITORSRCS))
LIBGAMEDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(LIBGAMESRCS))
LIBEDITORDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(LIBEDITORSRCS))

# Flags
VFLAGS=-c --vapidir=$(VAPIPATH) -X -I$(SRCPATH)$(LIBEDITORTARGET) -X -I$(SRCPATH)$(LIBGAMETARGET) --cc=$(CC)
VLIBS=--pkg=gtk+-3.0 --pkg=libhalberd-editor --pkg=libhalberd-game
CFLAGS=-g `$(PKGCONFIG) --cflags libxml-2.0 glew gl`
CLIBS=`$(PKGCONFIG) --libs libxml-2.0 glew gl lua`
GAMEFLAGS=-I$(SRCPATH)$(LIBGAMETARGET) `pkg-config --cflags glfw3 libpng`
GAMELIBS=`$(PKGCONFIG) --libs glfw3`
EDITORFLAGS=-I$(SRCPATH)$(LIBEDITORTARGET) -I$(SRCPATH)$(LIBGAMETARGET) -L$(LIBPATH) `$(PKGCONFIG) --cflags gtk+-3.0`
EDITORLIBS=`$(PKGCONFIG) --libs gtk+-3.0` $(LIBEDITORLIBS) $(LIBGAMELIBS)
LIBGAMEFLAGS=`$(PKGCONFIG) --cflags libpng`
LIBGAMELIBS=-lm `$(PKGCONFIG) --libs glfw3 libpng`
LIBEDITORFLAGS=-I$(SRCPATH)$(LIBGAMETARGET) 
LIBEDITORLIBS=

# Directory inits
$(shell mkdir -p $(OBJPATH))
$(shell mkdir -p $(LIBPATH))
$(shell mkdir -p $(VAPIPATH))
$(shell mkdir -p $(OBJPATH)$(EDITORTARGET))
$(shell mkdir -p $(OBJPATH)$(GAMETARGET))
$(shell mkdir -p $(OBJPATH)$(LIBGAMETARGET))
$(shell mkdir -p $(OBJPATH)$(LIBEDITORTARGET))
#-include $(LIBEDITORDEPS)
#-include $(LIBGAMEDEPS)
#-include $(LIBEDITORDEPS)

# Filetype rules
$(OBJPATH)$(GAMETARGET)/%.depend: $(SRCPATH)$(GAMETARGET)/%.c
	@echo -e "Building dependecies for \e[1;35m$<\e[0m..."
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $(GAMEFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJPATH)$(EDITORTARGET)/%.depend: $(SRCPATH)$(EDITORTARGET)/%.c
	@echo -e "Building dependecies for \e[1;35m$<\e[0m..."
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $(EDITORFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJPATH)$(LIBEDITORTARGET)/%.depend: $(SRCPATH)$(LIBEDITORTARGET)/%.c
	@echo -e "Building dependecies for \e[1;35m$<\e[0m..."
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $(LIBEDITORFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJPATH)$(LIBGAMETARGET)/%.depend: $(SRCPATH)$(LIBGAMETARGET)/%.c
	@echo -e "Building dependecies for \e[1;35m$<\e[0m..."
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $(LIBGAMEFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJPATH)$(GAMETARGET)/%.o: $(SRCPATH)$(GAMETARGET)/%.c
	@echo -e "Building \e[1;35m$<\e[0m..."
	$(CC) -c $< -o $@ $(CFLAGS) $(GAMEFLAGS)

$(OBJPATH)$(EDITORTARGET)/%.o: $(SRCPATH)$(EDITORTARGET)/%.c
	@echo -e "Building \e[1;35m$<\e[0m..."
	$(CC) -c $< -o $@ $(CFLAGS) $(EDITORFLAGS)

$(OBJPATH)$(LIBEDITORTARGET)/%.o: $(SRCPATH)$(LIBEDITORTARGET)/%.c
	@echo -e "Building \e[1;35m$<\e[0m..."
	$(CC) -c $< -o $@ $(CFLAGS) $(LIBEDITORFLAGS)

$(OBJPATH)$(LIBGAMETARGET)/%.o: $(SRCPATH)$(LIBGAMETARGET)/%.c
	@echo -e "Building \e[1;35m$<\e[0m..."
	$(CC) -c $< -o $@ $(CFLAGS) $(LIBGAMEFLAGS)

# Target rules
all: $(GAMETARGET) $(EDITORTARGET)

$(GAMETARGET): $(LIBGAMETARGET) $(GAMEDEPS) $(GAMESRCS) $(GAMEOBJS)
	@echo -e "Building \e[1;35m$<\e[0m..."
	@$(CC) -o $(GAMETARGET) $(GAMEOBJS) $(LIBPATH)$(LIBGAMETARGET).a $(CFLAGS) $(GAMEFLAGS) $(CLIBS) $(GAMELIBS) $(LIBGAMELIBS)
	@echo -e "Build Complete"

$(EDITORTARGET): $(LIBEDITORTARGET) $(EDITORSRCS) $(LIBGAMETARGET)
	@echo -e "Building \e[1;35m$<\e[0m..."
	$(VALAC) $(VFLAGS) $(EDITORSRCS) $(VLIBS)
	mv *.o $(OBJPATH)/$(EDITORTARGET)
	$(CC) -o $(EDITORTARGET) $(EDITOROBJS) $(LIBPATH)$(LIBEDITORTARGET).a $(LIBPATH)$(LIBGAMETARGET).a $(CFLAGS) $(EDITORFLAGS) $(CLIBS) $(EDITORLIBS)
	@echo -e "Build Complete"

$(LIBGAMETARGET): $(LIBGAMEDEPS) $(LIBGAMESRCS) $(LIBGAMEOBJS)
	@echo -e "Packaging \e[1;35m$<\e[0m..."
	@ar -rs $(LIBPATH)$(LIBGAMETARGET).a $(LIBGAMEOBJS)
	@echo -e "Build Complete"

$(LIBEDITORTARGET): $(LIBEDITORDEPS) $(LIBEDITORSRCS) $(LIBEDITOROBJS)
	@echo -e "Packaging \e[1;35m$<\e[0m..."
	@ar -rs $(LIBPATH)$(LIBEDITORTARGET).a $(LIBEDITOROBJS)
	@echo -e "Build Complete"

# Other rules
.PHONY: clean
clean:
	@echo -e "\e[0;31mCleaning up...\e[0m"
	$(RM) -r $(OBJPATH)
	$(RM) -r $(LIBPATH)
	$(RM) $(APP)
	@echo -e "\e[0;32mCleanup completed.\e[0m"
