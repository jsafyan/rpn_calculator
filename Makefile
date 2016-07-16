all: pcalc

pcalc: pcalc.c
	clang -o pcalc pcalc.c

clean:
	rm -rf *.o

clobber: clean
	rm -rf pcalc
