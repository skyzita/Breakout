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
} Jogador;

typedef struct Tijolo {
    Vector2 pos;
    Vector2 dim;
    Color cor;
} Tijolo;

/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/

 // Variaveis para mostrar a mensagem de início de jogo 
 int iniciar = 0;
bool textoInicio = true;
bool textoRetorno = false;
bool textoGameOver = false;
 int vidaPerdida = 0;
int pontuacao = 0;

Bola bola;
Jogador jogador;
Tijolo tijolos; //Usei array bidimensional, pode me matar mesmo - Mari

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
/*Eu normalmente deixo o nome do ponteiro igual da variavel que ele ta apontando - Mari*/
void atualizarBola( Bola *bola, float delta );
void desenharJogador( Jogador *jogador );
void atualizarJogador( Jogador *jogador, float delta );
void desenharTijolos( Tijolo *tijolo );
void atualizarTijolos( Tijolo *tijolo, float delta );
void desenharUI(int vidas, int pontos);


void perderVida(int vidas);
void resetarPosicoes();

//Bool que testa a colisão com o jogador :3 - Ebi
bool checarColisao(Vector2 bolaPos, float raio, Rectangle rect);

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
            .x = 90,
            .y = 90
        },
        .raio = 15,
        .cor = BLUE,
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
        .vel = 90,
        .cor = BLUE,
        .vidas = 3
    };

    tijolos = ( Tijolo ) {
        .pos = {
            .x = 0,
            .y = 100
        },
        .dim = {
            .x = 60,
            .y = 20
        },
        .cor = RED
    };

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

    for(int i = 0; i < 2; i++){
        if(iniciar == 0 && IsKeyPressed(KEY_SPACE)){
            iniciar++;
            textoInicio = false;
            textoRetorno = false;
            textoGameOver = false;
        }
    }

    if(iniciar >= 1 &&
    textoRetorno == false &&
    textoGameOver == false)
    {
    atualizarBola( &bola, GetFrameTime() );
    atualizarJogador( &jogador, GetFrameTime() );
    }
}

void draw( void ) {

    BeginDrawing();
    ClearBackground( BLACK );
    
    
    desenharBola( &bola );
    desenharJogador( &jogador );
    desenharTijolos( &tijolos );
    desenharUI(jogador.vidas, pontuacao);

        if(textoGameOver == true){
        DrawText("GAME OVER", 100, 400, 40, WHITE);
    }


    if(textoInicio){
        DrawText("Pressione ESPAÇO para começar!", 100, 400 , 20, WHITE);
    }

    if(textoRetorno == true){
        DrawText("Pressione ESPAÇO para retornar!", 100, 400, 20, WHITE);
        }

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
    if ( xEsquerda <= 0 || 
        xDireita >= GetScreenWidth()) 
        {
        bola->vel.x *= -1;
    }

    //Colisões com Teto e Chão
   if ( ySul >= GetScreenHeight() || yNorte <= 0  )
    {
        bola->vel.y *= -1;

        if(ySul >= GetScreenHeight()){
            jogador.vidas--;

            if(jogador.vidas == 0){
                
                iniciar = 0;
                textoGameOver == true;
            }

            if(jogador.vidas > 0){
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
        jogador.dim.y};
  
        if(checarColisao(bola->pos, bola->raio, jogadorRect)) {
        bola->vel.y *= -1;
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

    if (( IsKeyDown( KEY_A ) || IsKeyDown( KEY_LEFT ) ) && 
        jogador->pos.x - jogador->dim.x >= 0) {
        jogador->pos.x -= jogador->vel * delta;
    }
    
    if (( IsKeyDown( KEY_D) || IsKeyDown( KEY_RIGHT ) ) && 
    jogador->pos.x + jogador->dim.x <= GetScreenWidth() ) {
        jogador->pos.x += jogador->vel * delta;
    }

}

void desenharTijolos( Tijolo *tijolo ) {

    for ( int i = 0; i < 8; i++ ) {
        if ( i == 1 ) {
            tijolo->cor = ORANGE; //se tiver uma forma de deixar isso menor, faça pfv - Mari
        }
        else if ( i == 2 ) {
            tijolo->cor = YELLOW;
        }
        else if ( i == 3 ) {
            tijolo->cor = GREEN;
        }
        else if ( i == 4 ) {
            tijolo->cor = SKYBLUE;
        }
        else if ( i == 5 ) {
            tijolo->cor = BLUE;
        }
        else if ( i == 6 ) {
            tijolo->cor = PURPLE;
        }
        else if ( i == 7 ) {
            tijolo->cor = VIOLET;
        } else {
            tijolo->cor = RED;
        }
        
        for ( int j = 0; j < 8; j++ ) {
            DrawRectangle( 
            ( tijolo->pos.x + tijolo->dim.x * j ) + 5 * j,
            ( tijolo->pos.y + tijolo->dim.y * i ) + 5 * i,
            tijolo->dim.x,
            tijolo->dim.y,
            tijolo->cor   );
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
    bola.vel = (Vector2){ 90, 90 };


    jogador.pos = (Vector2){ GetScreenWidth() / 2, GetScreenHeight() - 20 };

    iniciar = 0;
    textoRetorno = true;

}