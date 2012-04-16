CC         = g++
EXE		   = twalk
OBJ		   = main.o behavioursettings.o
LFLAGS 	   = -Wall -ltag
CFLAGS     = -Wall -c

all: $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o $(EXE)

%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) $<

debug: CFLAGS += -DDEBUG -g
debug: all

release: CFLAGS += -O
release: all

clean:
	rm *.o $(EXE)

install:
	cp $(EXE) "$(DESTDIR)/usr/bin"
