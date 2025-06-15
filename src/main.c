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
#include <stdbool.h>


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
} Jogador;

typedef struct Tijolo {
    Vector2 pos;
    Vector2 dim;
    Color cor;
} Tijolo;

/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/
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

/**
 * @brief Game entry point.
 */
int main( void ) {

    // local variables and initial user input

    // antialiasing
    SetConfigFlags( FLAG_MSAA_4X_HINT );

    // creates a new window 800 pixels wide and 450 pixels high
    InitWindow( 515, 600, "Breakout" );

    // init audio device only if your game uses sounds
    //InitAudioDevice();

    // FPS: frames per second
    SetTargetFPS( 60 );    

    // you must load game resources here
    bola = ( Bola ) {
        .pos = {
            .x = GetScreenWidth() / 2,
            .y = GetScreenHeight() / 2
        },
        .vel = {
            .x = 90,
            .y = 90
        },
        .raio = 15,
        .cor = BLUE,
    };

    jogador = ( Jogador ) {
                .dim = {
            .x = 75,
            .y = 10
        },
        .pos = {
            .x = GetScreenWidth() / 2,
            .y = GetScreenHeight() - 20,
        },

        .vel = 90,
        .cor = BLUE
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

    atualizarBola( &bola, GetFrameTime() );
    atualizarJogador( &jogador, GetFrameTime() );
}

void draw( void ) {

    BeginDrawing();
    ClearBackground( BLACK );
    
    desenharBola( &bola );
    desenharJogador( &jogador );
    desenharTijolos( &tijolos );

    EndDrawing();

}

void desenharBola( Bola *bola ) {

    DrawCircleV( bola->pos, bola->raio, bola->cor );
}

void atualizarBola( Bola *bola, float delta ) {

    bola->pos.x += bola->vel.x * delta;
    bola->pos.y += bola->vel.y * delta;

    if ( bola->pos.x - bola->raio <= 0 ) {
        bola->vel.x *= -1;
    }
    else if ( bola->pos.x + bola->raio >= GetScreenWidth() ) {
        bola->vel.x *= -1;
    }

   if ( bola->pos.y - bola->raio <= 0 ) {
        bola->vel.y *= -1;
    }
    else if ( bola->pos.y + bola->raio >= GetScreenHeight() ) {
        bola->vel.y *= -1;
    }

}

void desenharJogador( Jogador *jogador ) {

    DrawRectangleV( jogador->pos, jogador->dim, jogador->cor );
}

void atualizarJogador( Jogador *jogador, float delta ) {

    if (( IsKeyDown( KEY_A ) || IsKeyDown( KEY_LEFT ) ) && 
        jogador->pos.x - jogador->dim.x >= -50) {
        jogador->pos.x -= jogador->vel * delta;
    }
    
    if (( IsKeyDown( KEY_D) || IsKeyDown( KEY_RIGHT ) ) && 
    jogador->pos.x + jogador->dim.x <= GetScreenWidth() - 25 ) {
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

