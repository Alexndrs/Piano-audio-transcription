/*
Compilation dans le fichier ~/Piano-audio-transcription:

  gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I include -L lib -lmingw32 -lSDL2main -lSDL2

pour généré un prog.exe sans fenetre de commande qui s'ouvre:
  gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I include -L lib -lmingw32 -lSDL2main -lSDL2 -mwindows -lm

*/

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


void piano_notes_to_video(int* tab_temps,int* tab_notes,int nb_notes);

void SDL_ExitWithError(char* message);

int main(int argc, char*argv[])
{   

    //Exemple de tableau issue de la fonction audio_to
    int tab_temps[] = {1, 2, 5};
    int tab_notes[] = {12, 5, 20};
    piano_notes_to_video(tab_temps, tab_notes, 3);

    return 0;
}


void SDL_ExitWithError(char* message)
{
    SDL_Log("ERREUR : %s > %s\n",message,SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}


void piano_notes_to_video(int* tab_temps,int* tab_notes,int nb_notes)
{
    
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Lancement SDL
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
        SDL_ExitWithError("Initialisation SDL");

    // Creation fenêtre + rendu
    if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0)
        SDL_ExitWithError("Impossible de creer la fenetre et le rendu");

    /*-------------- Exemple Dessin d'un point:----------------------*/

    // if (SDL_SetRenderDrawColor(renderer,150,159,255, SDL_ALPHA_OPAQUE) !=0 )
    //     SDL_ExitWithError("Impossible de changer la couleur pour le rendu");
    
    // if (SDL_RenderDrawPoint(renderer, 400, 300) != 0)
    //     SDL_ExitWithError("Impossible de dessiner un point");
    
    /*----------------------------------------------------------------------------------*/



    /*Fonction utile : 
    
    SDL_GetTicks(); --> nb de milisecondes depuis SDL_init
    
    */


    /*------------------Gestion d'Evenements------------------*/
    int nb_note_deja_jouee = 0;
    SDL_bool program_launched = SDL_TRUE;
    while((program_launched) & (nb_note_deja_jouee < nb_notes))
    {
        int t = SDL_GetTicks(); //Temps écoulé depuis le début (Depuis SDL_init) en ms
        //printf("ticks ecoules : %d\n",t);

        //printf("temps écoulé : %d ,  temps recherché : %d", )

        if (1000*tab_temps[nb_note_deja_jouee] < t)
        {
            //Animation pour la note : n = tab_notes[nb_note_deja_jouee]
            printf("note %d au temps %d\n", tab_notes[nb_note_deja_jouee], t/1000);
            nb_note_deja_jouee++;
        }


        SDL_Event event;
        while(SDL_PollEvent(&event)) 
        //capture tous les évènment exemple déplacement souris, appuie sur une touche ...
        {
            if (event.type == SDL_QUIT){program_launched=SDL_FALSE;}
        }
        SDL_RenderPresent(renderer);
    }
    
    /*--------------------------------------------------------*/


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}