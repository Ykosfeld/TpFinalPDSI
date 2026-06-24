CFLAGS = -I/usr/include/allegro5 -I/usr/include
# Adicionamos as bibliotecas de primitivas (formas geométricas), fontes e ttf
LIBS = -L/usr/lib64 -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec  

all: chain

chain: chain.o
	gcc -o chain chain.o $(LIBS) -lm

chain.o: chain.c
	gcc $(CFLAGS) -c chain.c     

clean:
	rm -f chain.o chain