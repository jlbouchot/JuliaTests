CC=gcc

CFLAGS=-c -Wall -fPIC

julialib.so: juliaUtils.c tablesjulia.c treeBuilder.c cvtp.c
	$(CC) -shared -fPIC -o julialib.so juliaUtils.c treeBuilder.c cvtp.c

clean:
	rm *.so
