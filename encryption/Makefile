Para = $(shell libgcrypt-config --cflags --libs)

all : panthercrypt.c pantherdec.c

	gcc -o encrypt panthercrypt.c ${Para}
	gcc -o decrypt pantherdec.c ${Para}