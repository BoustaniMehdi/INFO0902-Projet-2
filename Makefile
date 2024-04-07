OFILES1 = searchbylexicon.o Board.o List.o Set_HashTable.o
OFILES2 = searchbyboard.o Board.o List.o Set_HashTable.o
OFILES3 = searchbyboard.o Board.o List.o Set_BST.o
OFILES4 = searchbyboard.o Board.o List.o Set_RadixTrie.o
OFILES5 = test.o List.o Set_RadixTrie.o

TARGET1 = searchbylexicon
TARGET2 = searchbyboardhash
TARGET3 = searchbyboardbst
TARGET4 = searchbyboardradix
TARGET5 = test

LEXICON = english.txt

CC = gcc
CFLAGS = -g -Wall -Wextra -Wmissing-prototypes --pedantic -std=c99

.PHONY: all clean run

LDFLAGS = -lm

all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)
clean:
	rm -f $(OFILES1) $(OFILES2) $(OFILES3) $(OFILES4) $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)
run: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)
	./$(TARGET1) $(LEXICON) 150
	./$(TARGET2) $(LEXICON) 150
	./$(TARGET3) $(LEXICON) 150
	./$(TARGET4) $(LEXICON) 150
	./$(TARGET5)

$(TARGET1): $(OFILES1)
	$(CC) -o $(TARGET1) $(OFILES1) $(LDFLAGS)

$(TARGET2): $(OFILES2)
	$(CC) -o $(TARGET2) $(OFILES2) $(LDFLAGS)

$(TARGET3): $(OFILES3)
	$(CC) -o $(TARGET3) $(OFILES3) $(LDFLAGS)

$(TARGET4): $(OFILES4)
	$(CC) -o $(TARGET4) $(OFILES4) $(LDFLAGS)
	
$(TARGET5): $(OFILES5)
	$(CC) -o $(TARGET5) $(OFILES5) $(LDFLAGS)

Board.o: Board.c Board.h List.h Set.h
List.o: List.c List.h
Set_BST.o: Set_BST.c Set.h
Set_HashTable.o: Set_HashTable.c Set.h
Set_RadixTrie.o: Set_RadixTrie.c Set.h
searchbyboard.o: searchbyboard.c Board.h List.h Set.h
searchbylexicon.o: searchbylexicon.c Board.h List.h Set.h
test.o: Set_RadixTrie.c Set.h
leaks:
	valgrind --leak-check=full --show-leak-kinds=all -s ./test
