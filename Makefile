
NAME=HRA2016
NAMECLI=$(NAME)-cli 

ZIPNAME=xpospi88.zip
ZIPFILES = src/* examples/* Makefile README.txt doxy.config 

CC=g++
CFLAGS=-Wall -Wextra --std=c++11 -pedantic
LFLAGS=-lboost_system -lboost_filesystem
LFLAGS += $(shell command -v g++-4.8 >/dev/null 2>&1 && echo -n "-static-libstdc++")

BASEFILES = src/game.cpp src/player.cpp


all: $(NAMECLI) gui


gui:
	qmake-qt5 src/src.pro -o gui-compiled/Makefile  
	$(MAKE) CXX=$(CC) -C gui-compiled -o $(NAME)
	rm -rf gui-compiled/

$(NAME):src/main-gui.cpp
	$(CC) $(CFLAGS) src/main-gui.cpp -o $(NAME) $(BASEFILES) $(LFLAGS) 


$(NAMECLI): src/main-cli.cpp
	$(CC) $(CFLAGS) src/main-cli.cpp -o $(NAMECLI) $(BASEFILES) src/game-cli.cpp src/console.cpp $(LFLAGS)

cli: $(NAMECLI)
	@./HRA2016-cli

doxygen:
	@doxygen doxy.config

run: all
	@./HRA2016 && ./HRA2016-cli 

pack:
	@zip $(ZIPNAME) $(ZIPFILES) 


clean:
	rm -f $(NAME) $(NAMECLI)
