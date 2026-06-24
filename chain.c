#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_ENEMIES 15
#define TIRO_INATIVO 0
#define TIRO_ATIVO 1
#define RAIO_TIRO 100
#define TEMPO_TIRO 2.0
#define BORDA_TIRO 4
#define SCORE_PENALTY 0.3

const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;
const int HERO_W = 30;
const int HERO_H = 40;


ALLEGRO_COLOR BKG_COLOR;
ALLEGRO_FONT *FONT_32;

//teste commit
//teste commit wsl

typedef struct Tiro {
	int x, y;
	float raio;
	int modo;
	float timer;
	ALLEGRO_COLOR cor;
} Tiro;

typedef struct Ship {
	int x, y;
	int vel;	
	ALLEGRO_COLOR cor;
	Tiro tiro;
} Ship;

typedef struct Hero {

	Ship ship;
	int dir_x;
	int dir_y;
	float score;
} Hero;


typedef struct Enemy {

	Ship ship;
	float raio;
	int active;

} Enemy;


void initTiro(Ship *s) {

	//printf("\nInit tiro!");

	s->tiro.x = s->x;
	s->tiro.y = s->y;
	s->tiro.raio = 3;
	s->tiro.cor = s->cor;
	s->tiro.timer = TEMPO_TIRO;
	s->tiro.modo = TIRO_INATIVO;

}

void initGlobals() {
	BKG_COLOR = al_map_rgb(10, 10, 10);
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
	FONT_32 = al_load_font("arial.ttf", 32, 1);   
}

void initHero(Hero *s) {

	s->score = 0;
	s->ship.cor = al_map_rgb(100 + rand()%156, 100 + rand()%156, 100 + rand()%156);
	s->ship.x = SCREEN_W/2;
	s->ship.y = SCREEN_H - HERO_H - 10;
	s->ship.vel = 2;
	s->dir_x = 0;
	s->dir_y = 0;
	initTiro(&s->ship);

}

void initEnemy(Enemy *s, int atraso) {
	s->raio = 10 + (rand() % (40 - 10 + 1));

	int x_min = s->raio;
	int x_max = SCREEN_W - s->raio;
	s->ship.x = x_min + (rand() % (x_max - x_min + 1));
	s->ship.y = -s->raio - atraso;
	s->ship.vel = 2 + (rand() % (5));
	s->ship.cor = al_map_rgb(100 + rand()%156, 100 + rand()%156, 100 + rand()%156);
	initTiro(&s->ship);
	s->active = true;
	
}

void drawScenario(Hero s) {

	char score_txt[5];

	al_clear_to_color(BKG_COLOR);


}

void drawHero(Hero s) {


	al_draw_filled_triangle(s.ship.x, s.ship.y, s.ship.x - HERO_W/2, s.ship.y + HERO_H, s.ship.x + HERO_W/2, s.ship.y + HERO_H, s.ship.cor);

	char score_txt[5];
	sprintf(score_txt, "%d", (int)s.score);
	//imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
	al_draw_text(FONT_32, s.ship.cor, 100, 20, 0, score_txt);	

	al_draw_circle(s.ship.tiro.x, s.ship.tiro.y, s.ship.tiro.raio, s.ship.tiro.cor, BORDA_TIRO);


}

void drawEnemy(Enemy s) {

	al_draw_filled_circle(s.ship.x, s.ship.y, s.raio, s.ship.cor);
	
}

void updateHero(Hero *s) {

	//atualizando a posiçao da nave
	s->ship.x += s->dir_x * s->ship.vel;

	s->ship.y += s->dir_y * s->ship.vel;
	
	//restrigindo a movimentaçao da nave para os limites da tela
	//eixo x
	if (s->ship.x < HERO_W/2) {
		s->ship.x = HERO_W/2;
	} else if (s->ship.x > SCREEN_W - HERO_W/2) {
		s->ship.x = SCREEN_W - HERO_W/2;
	}
	//eixo y
	if (s->ship.y < 0) {
		s->ship.y = 0;
	} else if (s->ship.y > SCREEN_H - HERO_H) {
		s->ship.y = SCREEN_H - HERO_H;
	}

	if(s->ship.tiro.modo != TIRO_ATIVO) {
		s->ship.tiro.x = s->ship.x;
		s->ship.tiro.y = s->ship.y;
	}
	else {
		if(s->ship.tiro.timer > 0)
			s->ship.tiro.timer -= 1.0/FPS;
		else
			initTiro(&s->ship);
	}

}

void updateEnemy(Enemy *s) {
	//atualizando a posicao do inimigo
	if (s->active) {
		s->ship.y += s->ship.vel;
		if (s->ship.y > SCREEN_H + s->raio) {
			initEnemy(s, 300);
		}
	}
	if(s->ship.tiro.modo != TIRO_ATIVO) {
		s->ship.tiro.x = s->ship.x;
		s->ship.tiro.y = s->ship.y;
	}
	else {
		if(s->ship.tiro.timer > 0) {
			s->ship.tiro.timer -= 1.0/FPS;
		} else {
			initTiro(&s->ship);
			if (!s->active) {
				initEnemy(s, 300);
			}
		}
	}
}


bool checkHeroColision(Hero *h, Enemy *e) {
	float proximoX = e->ship.x;
	float proximoY = e->ship.y;

	if (proximoX < h->ship.x) {
		proximoX = h->ship.x;
	} else if (proximoX > h->ship.x + HERO_W/2) {
		proximoX = h->ship.x + HERO_W/2;
	}

	if (proximoY < h->ship.y) {
		proximoY = h->ship.y;
	} else if (proximoY > h->ship.y + HERO_H/2) {
		proximoY = h->ship.y + HERO_H/2;
	}

	float dx = e->ship.x - proximoX;
	float dy = e->ship.y - proximoY;
	float distancia = sqrt((dx * dx) + (dy * dy));

	return distancia < e->raio;
}

bool checkTiroColision(Tiro *t, Enemy *e) {
	float dx = e->ship.x - t->x;
	float dy = e->ship.y - t->y;
	float distancia = sqrt((dx * dx) + (dy * dy));

	return distancia < (t->raio + e->raio);
}
 
int main(int argc, char **argv){

	srand(time(NULL));

	int i;
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}
   


	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());


	//----------------------jogo-------------------

	//inicializa globais
	initGlobals();

	//Cria o heroi
	Hero Hero;
	initHero(&Hero);


	//cria os inimigos:
	Enemy Inimigos[NUM_ENEMIES];
	for (int i = 0; i < NUM_ENEMIES; i++) {
		initEnemy(&Inimigos[i], i * 250);
	}

	//inicia o temporizador
	al_start_timer(timer);
	
	int playing = 1;
	bool redraw = true;

    while(playing) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER) {

            updateHero(&Hero);
			for (int i = 0; i < NUM_ENEMIES; i++) {
				updateEnemy(&Inimigos[i]);
				if (Inimigos[i].active) {
					if (checkHeroColision(&Hero, &Inimigos[i])) {
						playing = 0;
					}
					for (int j = 0; j < NUM_ENEMIES; j++) {
						if (Inimigos[j].ship.tiro.modo == TIRO_ATIVO && checkTiroColision(&Inimigos[j].ship.tiro, &Inimigos[i])) {
							Inimigos[i].active = false;
							Inimigos[i].ship.tiro.x = Inimigos[i].ship.x;
							Inimigos[i].ship.tiro.y = Inimigos[i].ship.y;
							Inimigos[i].ship.tiro.raio = Inimigos[i].raio;
							Inimigos[i].ship.tiro.modo = TIRO_ATIVO;
						}
					}
					if (Hero.ship.tiro.modo == TIRO_ATIVO) {
						if (checkTiroColision(&Hero.ship.tiro, &Inimigos[i])) {
							Inimigos[i].active = false;
							Inimigos[i].ship.tiro.x = Inimigos[i].ship.x;
							Inimigos[i].ship.tiro.y = Inimigos[i].ship.y;
							Inimigos[i].ship.tiro.raio = Inimigos[i].raio;
							Inimigos[i].ship.tiro.modo = TIRO_ATIVO;
						}
					}
				}
			}
            redraw = true;

            if(!playing) {
                al_rest(3);
            }
        }
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			//printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			switch(ev.keyboard.keycode) {
			//se a tecla for o W
				case ALLEGRO_KEY_W:
					Hero.dir_y--;
				break;

				case ALLEGRO_KEY_S:
					Hero.dir_y++;
				break;

				case ALLEGRO_KEY_A:
					Hero.dir_x--;
				break;

				case ALLEGRO_KEY_D:
					Hero.dir_x++;
				break;	

				case ALLEGRO_KEY_SPACE:
					if(Hero.ship.tiro.modo == TIRO_INATIVO) {
						Hero.ship.tiro.modo = TIRO_ATIVO;
						Hero.ship.tiro.raio = RAIO_TIRO;
					}
				break;
			}
		}			

		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			//imprime qual tecla foi
			//printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			switch(ev.keyboard.keycode) {
			//se a tecla for o W
				case ALLEGRO_KEY_W:
					Hero.dir_y++;
				break;

				case ALLEGRO_KEY_S:
					Hero.dir_y--;
				break;

				case ALLEGRO_KEY_A:
					Hero.dir_x++;
				break;

				case ALLEGRO_KEY_D:
					Hero.dir_x--;
				break;	
				/*
				case ALLEGRO_KEY_SPACE:
					if(Hero.tiro.modo == TIRO_HOLDING)
						Hero.tiro.modo = TIRO_ATIVO;
				break;		
				*/		
			}			

		}

		if(redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false; // Reseta a permissão

            // Desenha tudo na ordem correta
            drawScenario(Hero);
            drawHero(Hero);
			for (int i = 0; i < NUM_ENEMIES; i++) {
				if (Inimigos[i].active) {
					drawEnemy(Inimigos[i]);
				}
				if (Inimigos[i].ship.tiro.modo == TIRO_ATIVO) {
					al_draw_circle(Inimigos[i].ship.tiro.x, Inimigos[i].ship.tiro.y, Inimigos[i].ship.tiro.raio, Inimigos[i].ship.cor, BORDA_TIRO);
				}
			}

            // Joga tudo para a tela de uma vez só
            al_flip_display();
        }

	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	

		char my_text[100];
		al_clear_to_color(al_map_rgb(0,0,0));
	 	sprintf(my_text, "Pontuação: %d", (int)Hero.score);
		al_draw_text(FONT_32, al_map_rgb(220, 30, 0), SCREEN_W/3, SCREEN_H/2 + 50, 0, my_text);


		al_flip_display();
		al_rest(3);	
 
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
 
	return 0;
}