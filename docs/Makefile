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


JULIALIBSRC=juliaUtils.c
JULIAOBJECTS=$(TREELIBSRC:.c=.o)

julialib.so: juliaUtils.c tablesjulia.c treeBuilder.c cvtp.c
	$(CC) -shared -fPIC -o julialib.so juliaUtils.c treeBuilder.c cvtp.c

TREELIBSRC=treeBuilder.c
TREEOBJECTS=$(TREELIBSRC:.c=.o)

treelib: $(TREEOBJECTS)
	$(CC) -shared -fPIC -o treelib.so $(TREEOBJECTS)

CVTPLIBSRC=cvtp.c
CVTPOBJECTS=$(CVTPLIBSRC:.c=.o)

cvtplib: $(CVTPOBJECTS)
	$(CC) -shared -fPIC -o cvtplib.so $(CVTPOBJECTS)
