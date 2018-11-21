default: tc


tc: tc.c
	gcc -pthread -o tc tc.c

clean:
	-rm -f tc.o
	-rm -f tc
