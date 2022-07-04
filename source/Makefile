all:
	gcc -o threads threads.c -lpthread
	gcc -o nlocks nlocks.c -lpthread


clean:
	$(RM) threads
	$(RM) nlocks

run:
	./threads 12345
	./nlocks
