CFLAGS= -pthread -lrt -O2
TARGETS= CarroPkg.h CarroPkg.c main.c simThreads.c simThreads.h
make: $(TARGETS)
	gcc $(TARGETS) -o Simulador $(CFLAGS)
clean:
	rm -f Simulador *.o
