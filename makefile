dns : main.o request.o
	cc -o dns main.o request.o

main.o : main.c request.h
	cc -c main.c

request.o : request.c request.h
	cc -c request.c

clean :
	rm dns main.o request.o
