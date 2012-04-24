CC         = g++
EXE	   = tagwalker
OBJ	   = main.o configuration.o tagwalker.o
LFLAGS 	   = -Wall -ltag
CFLAGS     = -Wall -c

all: $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o $(EXE)

%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) $<

debug: CFLAGS += -DDEBUG -g
debug: LFLAGS += -DDEBUG -g
debug: all

release: CFLAGS += -O
release: LFLAGS += -O
release: all

clean:
	rm *.o $(EXE)

install:
	cp $(EXE) "$(DESTDIR)/usr/bin"

install-man:
	cp ../man/$(EXE).1 "$(DESTDIR)/usr/share/man/man1"

install-man-gz:
	cp ../man/$(EXE).1.gz "$(DESTDIR)/usr/share/man/man1"
