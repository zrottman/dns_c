dns : dns.o request.o
	cc -o dns dns.o request.o

dns.o : dns.c request.h
	cc -c dns.c

request.o : request.c request.h
	cc -c request.c

clean :
	rm dns dns.o request.o
