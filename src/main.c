/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Main function and logic for the game. Simplified template for game
 * development in C using Raylib (https://www.raylib.com/).
 * 
 * @copyright Copyright (c) 2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


/*---------------------------------------------
 * Library headers.
 *-------------------------------------------*/
#include "raylib/raylib.h"
//#include "raylib/raymath.h"

/*---------------------------------------------
 * Project headers.
 *-------------------------------------------*/


/*---------------------------------------------
 * Macros. 
 *-------------------------------------------*/


/*--------------------------------------------
 * Constants. 
 *------------------------------------------*/


/*---------------------------------------------
 * Custom types (enums, structs, unions, etc.)
 *-------------------------------------------*/

 typedef struct Bola {
    Vector2 pos;
    Vector2 vel;
    float raio;
    Color cor;  
} Bola;

typedef struct Jogador {
    Vector2 pos;
    Vector2 dim;
    Color cor;
    float vel;
    int vidas;
    int pontuacao;
} Jogador;

typedef struct Tijolo {
    Vector2 pos;
    Vector2 dim;
    Color cor;
    bool aparecendo;
} Tijolo;

typedef enum EstadoDoJogo {
    INICIO,
    JOGANDO,
    RETORNO,
    GAMEOVER,
    VITORIA
} EstadoDoJogo;

/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/

 // Variaveis para mostrar a mensagem de início de jogo 

Bola bola;
Jogador jogador;
Tijolo tijolos[8][8];
EstadoDoJogo estado = INICIO;
bool linhaAumentada[8] = { false };
int linhas = 8;

/*---------------------------------------------
 * Function prototypes. 
 *-------------------------------------------*/
/**
 * @brief Reads user input and updates the state of the game.
 */
void update( float delta );

/**
 * @brief Draws the state of the game.
 */
void draw( void );
void desenharBola( Bola *bola );
void atualizarBola( Bola *bola, float delta );
void desenharJogador( Jogador *jogador );
void atualizarJogador( Jogador *jogador, float delta );
void desenharTijolos( Tijolo tijolo[8][8] );
void desenharUI(int vidas, int pontos);
void desenharTextos( EstadoDoJogo estado );
void perderVida(int vidas);
void resetarPosicoes();

//Bool que testa a colisão com o jogador :3 
bool checarColisao(Vector2 bolaPos, float raio, Rectangle rect);

//Funcao que cria um array bidimensional que guarda as coords de cada bloco
void criarTijolos( Tijolo tijolo[8][8] );

//Funcao que checa se houve colisao bloco por bloco
void checarColisaoTijolo( Tijolo tijolo[8][8], Bola *bola );
void aumentarVel( Bola *bola, Tijolo tijolo[8][8] );

/**
 * @brief Game entry point.
 */
int main( void ) {

    // local variables and initial user input

    // antialiasing
    SetConfigFlags( FLAG_MSAA_4X_HINT );

    // creates a new window 515 pixels wide and 600 pixels high
    InitWindow( 515, 600, "Breakout" );

    // init audio device only if your game uses sounds
    //InitAudioDevice();

    // FPS: frames per second
    SetTargetFPS( 60 );    

    // you must load game resources here
    bola = ( Bola ) {
        .pos = {
            .x = GetScreenWidth() / 2,
            .y = GetScreenHeight() - 150
        },
        .vel = {
            .x = 100,
            .y = 100
        },
        .raio = 15,
        .cor = WHITE,
    };

    jogador = ( Jogador ) {
        .pos = {
            .x = GetScreenWidth() / 2,
            .y = GetScreenHeight() - 20,
        },
        .dim = {
            .x = 75,
            .y = 10
        },
        .vel = 150,
        .cor = WHITE,
        .vidas = 3,
        .pontuacao = 0
    };

    criarTijolos( tijolos );

    // game loop
    while ( !WindowShouldClose() ) {
        update( GetFrameTime() );
        draw();
    }

    // you should unload game resources here

    
    // close audio device only if your game uses sounds
    //CloseAudioDevice();
    CloseWindow();

    return 0;

}

void update( float delta ) {

    if ( estado == INICIO || estado == RETORNO ) {
        if ( IsKeyPressed( KEY_SPACE ) ) {
            estado = JOGANDO;
        }
    }

    if( estado == JOGANDO ) {
        atualizarBola( &bola, GetFrameTime() );
        atualizarJogador( &jogador, GetFrameTime() );
        checarColisaoTijolo( tijolos, &bola );
        aumentarVel( &bola, tijolos );
    }


}

void draw( void ) {

    BeginDrawing();
    ClearBackground( BLACK );
    
    desenharBola( &bola );
    desenharJogador( &jogador );
    desenharTijolos( tijolos );
    desenharUI(jogador.vidas, jogador.pontuacao);
    desenharTextos( estado );
        
    EndDrawing();

}

void desenharBola( Bola *bola ) {

    DrawCircleV( bola->pos, bola->raio, bola->cor );
}

void atualizarBola( Bola *bola, float delta ) {

    bola->pos.x += bola->vel.x * delta;
    bola->pos.y += bola->vel.y * delta;


    //Fazendo isso apenas para ficar mais legível :)
    //Direita e Esquerda
    float xDireita = bola->pos.x + bola->raio;
    float xEsquerda = bola->pos.x - bola->raio;

    //Norte e Sul
    float ySul = bola->pos.y + bola->raio;
    float yNorte = bola->pos.y - bola->raio;

    //Colisões com Paredes Laterais
    if ( xEsquerda <= 0 || xDireita >= GetScreenWidth()) {
        bola->vel.x *= -1;
    }

    //Colisões com Teto e Chão
   if ( ySul >= GetScreenHeight() || yNorte <= 0  ) {
        
        bola->vel.y *= -1;

        if(ySul >= GetScreenHeight()) {
            jogador.vidas--;

            if(jogador.vidas == 0) {
            
                estado = GAMEOVER;
            }

            if(jogador.vidas > 0) {
                resetarPosicoes();
            }
        }
    }
 
    Rectangle jogadorRect = {
        //Posição
        jogador.pos.x - jogador.dim.x / 2, 
        jogador.pos.y - jogador.dim.y / 2, 
        //Dimensões
        jogador.dim.x, 
        jogador.dim.y
    };
  
   if(checarColisao(bola->pos, bola->raio, jogadorRect)) {
        
    
        bola->vel.y *= -1;
        if(bola->pos.y <= jogadorRect.width / 2){
            bola->vel.x *= -1;
        }
        bola->pos.y = jogadorRect.y - bola->raio;
    }
}

void desenharJogador( Jogador *jogador ) {

    //Melhor usar o DrawRectangle normal pois ele centraliza do jeito que a gente quer :)
    DrawRectangle(
        jogador->pos.x - jogador->dim.x / 2,
        jogador->pos.y - jogador->dim.y /2,
        jogador->dim.x,
        jogador->dim.y,
        jogador->cor
    );
}

void atualizarJogador( Jogador *jogador, float delta ) {

    if (( IsKeyDown( KEY_A ) || IsKeyDown( KEY_LEFT ) ) 
    && jogador->pos.x - jogador->dim.x / 2 >= 0) {
        jogador->pos.x -= jogador->vel * delta;
    }
    
    if (( IsKeyDown( KEY_D) || IsKeyDown( KEY_RIGHT ) ) 
    && jogador->pos.x + jogador->dim.x / 2 <= GetScreenWidth() ) {
        jogador->pos.x += jogador->vel * delta;
    }

}

void desenharTijolos( Tijolo tijolo[8][8] ) {

    
    for ( int j = 0; j < 8; j++ ) {
        for ( int i = 0; i < 8; i++ ) {
            if ( tijolo[i][j].aparecendo == true ) {
                if ( j == 1 ) {
                    tijolo[i][j].cor = ORANGE; 
                }
                else if ( j == 2 ) {
                    tijolo[i][j].cor = YELLOW;
                }
                else if ( j == 3 ) {
                tijolo[i][j].cor = GREEN;
                }
                else if ( j == 4 ) {
                    tijolo[i][j].cor = SKYBLUE;
                }
                else if ( j == 5 ) {
                    tijolo[i][j].cor = BLUE;
                }
                else if ( j == 6 ) {
                    tijolo[i][j].cor = PURPLE;
                }
                else if ( j == 7 ) {
                    tijolo[i][j].cor = VIOLET;
                } else {
                    tijolo[i][j].cor = RED;
                }

                DrawRectangleV( tijolo[i][j].pos, tijolo[i][j].dim, tijolo[i][j].cor);
            }
        }
    }    
}

bool checarColisao(Vector2 bolaPos, float raio, Rectangle rect){
    float xMaisProximo = fmaxf(rect.x, fminf(bolaPos.x, rect.x + rect.width));
    float yMaisProximo = fmaxf(rect.y, fminf(bolaPos.y, rect.y + rect.height));

    float distanciaX = bolaPos.x - xMaisProximo;
    float distanciaY = bolaPos.y - yMaisProximo;

    float distanciaQuadrada = (distanciaX * distanciaX) + (distanciaY * distanciaY);

    return distanciaQuadrada < (raio * raio);
}


void desenharUI(int vidas, int pontos){
    char qtdVidas[50];
    char qtdPontos[100];
    sprintf(qtdVidas, "Vidas: %d", vidas);
    sprintf(qtdPontos,"Pontuação: %d", pontos);

    DrawText(qtdVidas, GetScreenWidth() / 8, GetScreenHeight() / 16, 15, WHITE);

    DrawText(qtdPontos, GetScreenWidth() - 150, GetScreenHeight() / 16, 15, WHITE);
}

void resetarPosicoes(){
    bola.pos = (Vector2){ GetScreenWidth() / 2, GetScreenHeight() - 150};
    if(bola.vel.x < 0){
        bola.vel.x *= -1;
        bola.vel.x *= 0.9;
    }
    if(bola.vel.y < 0){
        bola.vel.y *= -1;
        bola.vel.y *= 0.9;
    }
    //bola.vel = (Vector2){ 90, 90 };


    jogador.pos = (Vector2){ GetScreenWidth() / 2, GetScreenHeight() - 20 };

    estado = RETORNO;

}

void desenharTextos( EstadoDoJogo estado ) {

    if( estado == INICIO ) {
        DrawText( "Pressione ESPAÇO para começar!", 100, 400 , 20, WHITE );
        
    }

    if( estado == RETORNO ) {
        DrawText("Pressione ESPAÇO para retornar!", 100, 400, 20, WHITE);
    }

    if( estado == GAMEOVER ) {
        DrawText("GAME OVER", 140, 400, 40, WHITE);
        DrawText("Pressione R para reiniciar!", 130, 450, 20, WHITE);
    }
    

    if( estado == VITORIA ) {
        DrawText( "Você Venceu!!!", GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 20, 40, WHITE);
    }
}

void criarTijolos( Tijolo tijolo[8][8] ) {

    for ( int j = 0; j < 8; j++ ) {
        for ( int i = 0; i < 8; i++ ) {
            tijolo[i][j] = ( Tijolo ) {
                .pos = {
                    .x = ( 0 + 60 * i ) + 5 * i,
                    .y = ( 100 + 20 * j ) + 5 * j
                },
                .dim = {
                    .x = 60,
                    .y = 20
                },
                .cor = RED,
                .aparecendo = true
            };
        }
    }
}

void checarColisaoTijolo( Tijolo tijolo[8][8], Bola *bola ) {

    for ( int j = 0; j < 8; j++ ) {
        for ( int i = 0; i < 8; i++ ) {
            bool c1 = CheckCollisionCircleRec( 
            bola->pos, 
            bola->raio, 
            ( Rectangle ) {
                .x = tijolo[i][j].pos.x,
                .y = tijolo[i][j].pos.y,
                .width = tijolo[i][j].dim.x,
                .height = tijolo[i][j].dim.y
                }
            );

            if ( c1 && tijolo[i][j].aparecendo != false ) {
                tijolo[i][j].aparecendo = false;
                bola->vel.y *= -1;
                jogador.pontuacao += 100 + 100 * ( ( j - 7 ) * -1 );
        if ( bola->vel.x > 0 ) {
            bola->vel.x += 3.125;
        } else {
            bola->vel.x -= 3.125;
        }
        
        if ( bola->vel.y > 0 ) {
            bola->vel.y += 3.125;
        } else {
            bola->vel.y -= 3.125;
        }
       }
      }
     }
    }

void aumentarVel( Bola *bola, Tijolo tijolo[8][8] ) {

    bool linhaDestruida = false;
    int blocosDestruidos = 0;


    for ( int j = 0; j < 8; j++ ) {
        for ( int i = 0; i < 8; i++ ) {
            if ( tijolo[i][j].aparecendo == false ) {
                blocosDestruidos++;
            }
        }
        if ( blocosDestruidos == 8 && !linhaAumentada[j] ) {
            linhaDestruida = true;
            linhaAumentada[j] = true;
        } else {
            blocosDestruidos = 0;
        }
    }

    if ( linhaDestruida ) {
 /*
        if ( bola->vel.x > 0 ) {
            bola->vel.x += 25;
        } else {
            bola->vel.x -= 25;
        }
        
        if ( bola->vel.y > 0 ) {
            bola->vel.y += 25;
        } else {
            bola->vel.y -= 25;
        }
*/
        jogador.vel += 7.15;

        linhaDestruida = false;
               linhas--;

    }

            if(linhas <= 0){
            estado = VITORIA;
        }
}