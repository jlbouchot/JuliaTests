CC=gcc 
 
CFLAGS=-c -Wall -fPIC
 
SOURCES=basecounter.c 
OBJECTS=$(SOURCES:.c=.o)
 
.c.o:
	$(CC) $(CFLAGS) $< -o $@ 
 
julialib: $(OBJECTS)
	$(CC) -shared -fPIC -o libcnt.so $(OBJECTS)
 
clean:
	rm *.o *.so
