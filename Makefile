# CC = Compiler
CC=g++

# CFLAGS = CompilatorFLAGS
CFLAGS=-c

all: diff

diff: diff.o tex.o bin_tree.o mem_def.o
	$(CC) Obj/diff.o Obj/tex.o Obj/bin_tree.o Obj/mem_def.o -o diff

diff.o: differentiator.cpp
	$(CC) $(CFLAGS) differentiator.cpp -o Obj/diff.o

tex.o: tex.cpp
	$(CC) $(CFLAGS) tex.cpp -o Obj/tex.o

bin_tree.o: BinaryTree/bin_tree.cpp
	$(CC) $(CFLAGS) BinaryTree/bin_tree.cpp -o Obj/bin_tree.o

mem_def.o: MemoryDefender/memory_defender.cpp
	$(CC) $(CFLAGS) MemoryDefender/memory_defender.cpp -o Obj/mem_def.o

clean:
	rm -rf *.o diff