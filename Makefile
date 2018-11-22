default: tc


tc: tc.c
	gcc -pthread -o tc tc.c
	
run: tc
	./tc
	
clean:
	-rm -f tc
