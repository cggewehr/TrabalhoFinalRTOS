CFLAGS= -pthread -lrt -O2
TARGETS= main.c
make: $(TARGETS)
	gcc $(TARGETS) -o Cliente $(CFLAGS)
clean:
	rm -f Cliente *.o
