CC         = g++
EXE	   	   = tagwalker
OBJ		   = main.o configuration.o direntryhandle.o userinterface.o walker.o
LFLAGS 	   = `pkg-config --libs taglib` -lreadline
CFLAGS     = -c `pkg-config --cflags taglib` -I/usr/include/readline

$(EXE): CFLAGS += -O
$(EXE): LFLAGS += -O
$(EXE): $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o $(EXE)

debug: CFLAGS += -Wall -DDEBUG -g
debug: LFLAGS += -Wall -DDEBUG -g
debug: $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o $(EXE)

%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) $<

.PHONY: clean
clean:
	rm *.o $(EXE)

install:
	cp $(EXE) "$(DESTDIR)/usr/bin"

install-man:
	cp ../man/$(EXE).1 "$(DESTDIR)/usr/share/man/man1"

install-man-gz:
	cp ../man/$(EXE).1.gz "$(DESTDIR)/usr/share/man/man1"
