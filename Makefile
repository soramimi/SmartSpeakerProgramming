


all:
	cd okaeri ; make
	cd tadaima ; make

clean:
	rm -f *.o
	cd packet ; make clean
	cd okaeri ; make clean
	cd tadaima ; make clean

run-okaeri:
	okaeri/okaeri

run-tadaima:
	tadaima/tadaima
