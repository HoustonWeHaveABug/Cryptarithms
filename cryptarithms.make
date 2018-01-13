CRYPTARITHMS_C_FLAGS=-O2 -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

cryptarithms: cryptarithms.o
	gcc -o cryptarithms cryptarithms.o

cryptarithms.o: cryptarithms.c cryptarithms.make
	gcc -c ${CRYPTARITHMS_C_FLAGS} -o cryptarithms.o cryptarithms.c

clean:
	rm -f cryptarithms cryptarithms.o
