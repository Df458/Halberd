# Compilers
CC=gcc
VALAC=valac
PKGCONFIG=pkg-config
OS=Linux

# Targets
GAMETARGET=halberd-game
EDITORTARGET=halberd-editor
LIBGAMETARGET=libhalberd-game
LIBEDITORTARGET=libhalberd-editor
COMMONTARGET=libhalberd-common

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
COMMONSRCS:=$(wildcard $(SRCPATH)$(COMMONTARGET)/*.c)

GAMEOBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(GAMESRCS))
EDITOROBJS:=$(patsubst $(SRCPATH)%.vala,$(OBJPATH)%.vala.o,$(EDITORSRCS))
LIBGAMEOBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(LIBGAMESRCS))
LIBEDITOROBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(LIBEDITORSRCS))
COMMONOBJS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.o,$(COMMONSRCS))

GAMEDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(GAMESRCS))
EDITORDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(EDITORSRCS))
LIBGAMEDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(LIBGAMESRCS))
LIBEDITORDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(LIBEDITORSRCS))
COMMONDEPS:=$(patsubst $(SRCPATH)%.c,$(OBJPATH)%.depend,$(COMMONSRCS))

# Flags
VFLAGS=-c --vapidir=$(VAPIPATH) -X -I$(SRCPATH)$(LIBEDITORTARGET) -X -I$(SRCPATH)$(LIBGAMETARGET) -X -I$(SRCPATH)$(COMMONTARGET) -X -I/usr/include/dfgame/common/ --cc=$(CC)
VLIBS=--pkg=glib-2.0 --pkg=gtk+-3.0 --pkg=gee-0.8 --pkg=libxml-2.0 --pkg=dflib --pkg=dfgame-editor-frontend --pkg dfgame-editor --pkg dfgame-game --pkg=$(LIBEDITORTARGET) --pkg=$(LIBGAMETARGET) --pkg=$(COMMONTARGET)
CFLAGS=-g -Wall -Werror -Wno-unused-variable -Wno-traditional -Wno-pedantic -I$(SRCPATH)$(COMMONTARGET) `$(PKGCONFIG) --cflags libxml-2.0 glew gl dfgame-common`
CLIBS=`$(PKGCONFIG) --libs libxml-2.0 glew gl lua dfgame-common`
GAMEFLAGS=-I$(SRCPATH)$(LIBGAMETARGET) `pkg-config --cflags glfw3 libpng dfgame-game-front`
GAMELIBS=`$(PKGCONFIG) --libs glfw3 dfgame-game-front`
EDITORFLAGS=-I$(SRCPATH)$(LIBEDITORTARGET) -I$(SRCPATH)$(LIBGAMETARGET) -L$(LIBPATH) `$(PKGCONFIG) --cflags glib-2.0 gtk+-3.0 gee-0.8 dfgame-editor-front`
EDITORLIBS=-ldflib `$(PKGCONFIG) --libs glib-2.0 gtk+-3.0 gee-0.8 dfgame-editor-front` $(LIBEDITORLIBS) $(LIBGAMELIBS)
LIBGAMEFLAGS=`$(PKGCONFIG) --cflags libpng dfgame-game`
LIBGAMELIBS=-lm `$(PKGCONFIG) --libs glfw3 libpng dfgame-game`
LIBEDITORFLAGS=`$(PKGCONFIG) --cflags dfgame-editor`
LIBEDITORLIBS=`$(PKGCONFIG) --libs dfgame-editor`

# Directory inits
$(shell mkdir -p $(OBJPATH))
$(shell mkdir -p $(LIBPATH))
$(shell mkdir -p $(VAPIPATH))
$(shell mkdir -p $(OBJPATH)$(EDITORTARGET))
$(shell mkdir -p $(OBJPATH)$(GAMETARGET))
$(shell mkdir -p $(OBJPATH)$(LIBGAMETARGET))
$(shell mkdir -p $(OBJPATH)$(LIBEDITORTARGET))
$(shell mkdir -p $(OBJPATH)$(COMMONTARGET))
#-include $(LIBEDITORDEPS)
#-include $(LIBGAMEDEPS)
#-include $(LIBEDITORDEPS)

ifeq ($(OS), Linux)
#OSFLAGS=$(LINUXFLAGS)
#OSLIBS=$(LINUXLIBS)
else ifeq ($(OS), Windows)
#OSFLAGS=$(WINFLAGS)
#OSLIBS=$(WINLIBS)
CC=i686-w64-mingw32-gcc
PKGCONFIG=i686-w64-mingw32-pkg-config
else
$(error Unknown OS selected for output)
endif

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

$(OBJPATH)$(COMMONTARGET)/%.depend: $(SRCPATH)$(COMMONTARGET)/%.c
	@echo -e "Building dependecies for \e[1;35m$<\e[0m..."
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
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

$(OBJPATH)$(COMMONTARGET)/%.o: $(SRCPATH)$(COMMONTARGET)/%.c
	@echo -e "Building \e[1;35m$<\e[0m..."
	$(CC) -c $< -o $@ $(CFLAGS) $(LIBGAMEFLAGS)

# Target rules
all: $(GAMETARGET) $(EDITORTARGET) tags

$(GAMETARGET): $(LIBGAMETARGET) $(GAMEDEPS) $(GAMESRCS) $(GAMEOBJS)
	@echo -e "Building \e[1;35m$@\e[0m..."
	$(CC) -o $(GAMETARGET) $(GAMEOBJS) $(LIBPATH)$(LIBGAMETARGET).a $(LIBPATH)$(COMMONTARGET).a $(CFLAGS) $(GAMEFLAGS) $(CLIBS) $(GAMELIBS) $(LIBGAMELIBS)
	@echo -e "Build Complete"

# $(EDITORSRCS) compiles the files. This should be changed.
$(EDITORTARGET): $(LIBEDITORTARGET) $(EDITORSRCS) $(LIBGAMETARGET)
	@echo -e "Building \e[1;35m$@\e[0m..."
	$(VALAC) $(VFLAGS) $(EDITORSRCS) $(VLIBS)
	mv *.o $(OBJPATH)/$(EDITORTARGET)
	$(CC) -o $(EDITORTARGET) $(EDITOROBJS) $(LIBPATH)$(LIBEDITORTARGET).a $(LIBPATH)$(LIBGAMETARGET).a $(LIBPATH)$(COMMONTARGET).a $(CFLAGS) $(EDITORFLAGS) $(CLIBS) $(EDITORLIBS)
	@echo -e "Build Complete"

$(LIBGAMETARGET): $(COMMONTARGET) $(LIBGAMEDEPS) $(LIBGAMESRCS) $(LIBGAMEOBJS)
	@echo -e "Packaging \e[1;35m$@\e[0m..."
	@ar -rs $(LIBPATH)$(LIBGAMETARGET).a $(LIBGAMEOBJS)
	@echo -e "Build Complete"

$(LIBEDITORTARGET): $(COMMONTARGET) $(LIBEDITORDEPS) $(LIBEDITORSRCS) $(LIBEDITOROBJS)
	@echo -e "Packaging \e[1;35m$@\e[0m..."
	@ar -rs $(LIBPATH)$(LIBEDITORTARGET).a $(LIBEDITOROBJS)
	@echo -e "Build Complete"

$(COMMONTARGET): $(COMMONDEPS) $(COMMONSRCS) $(COMMONOBJS)
	@echo -e "Packaging \e[1;35m$@\e[0m..."
	@ar -rs $(LIBPATH)$(COMMONTARGET).a $(COMMONOBJS)
	@echo -e "Build Complete"

# Other rules
settings: data/org.df458.halberd.gschema.xml
	cp -f data/org.df458.halberd.gschema.xml /usr/share/glib-2.0/schemas
	glib-compile-schemas /usr/share/glib-2.0/schemas

tags: $(COMMONSRCS) $(LIBEDITORSRCS) $(LIBGAMESRCS) $(EDITORSRCS) $(GAMESRCS)
	ctags -R

install: settings

.PHONY: clean
clean:
	@echo -e "\e[0;31mCleaning up...\e[0m"
	$(RM) -r $(OBJPATH)
	$(RM) -r $(LIBPATH)
	$(RM) $(APP)
	@echo -e "\e[0;32mCleanup completed.\e[0m"
