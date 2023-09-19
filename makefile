dns : main.o dns.o
	cc -o dns main.o dns.o

main.o : main.c dns.h
	cc -c main.c

dns.o : dns.c dns.h
	cc -c dns.c

clean :
	rm dns main.o dns.o
