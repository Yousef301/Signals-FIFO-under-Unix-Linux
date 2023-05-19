CC = gcc
LDFLAGS = -lglut -lGLU -lGL -lm
OBJECTS = main child procChild Display
EXECUTABLE = main

.PHONY: all clean run

all: $(OBJECTS)

main: main.c
	$(CC) $< -o $@ $(LDFLAGS)

child: child.c
	$(CC) $< -o $@ $(LDFLAGS)

Display: Display.c
	$(CC) $< -o $@ $(LDFLAGS)

procChild: procChild.c
	$(CC) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJECTS)

run: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARGS)
