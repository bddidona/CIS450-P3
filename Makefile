default: tc

tc.o: tc.c
	gcc -c tc.c -o tc.o

tc: tc.o
	gcc tc.o -o tc

clean:
	-rm -f tc.o
	-rm -f tc
