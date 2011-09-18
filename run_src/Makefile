TARGET=judge
CC=g++
#MACROS=-DDEBUG=1
OBJS=judge.o misc.o

.PHONY: clean tags

$(TARGET): $(OBJS)
	$(CC) -O2 -o $@ $^

judge.o: judge.cpp
	$(CC) $(MACROS) -O2 -c -o $@ $^

misc.o: misc.cpp
	$(CC) $(MACROS) -O2 -c -o $@ $^

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)

tags:
	ctags -R
