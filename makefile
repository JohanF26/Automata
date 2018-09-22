all:	ferreras_p1

ferreras_p1:	ferreras_p1.o
		g++ ferreras_p1.o -o ferreras_p1

ferreras_p1.o:	ferreras_p1.cpp
		g++ -c ferreras_p1.cpp

run:
		./ferreras_p1

clean:
		rm -f *.o ferreras_p1
