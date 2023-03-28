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
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


void piano_notes_to_video(int* tab_temps,int* tab_notes,int nb_notes);

void SDL_ExitWithError(char* message);

int main(int argc, char*argv[])
{   

    //Exemple de tableau issue de la fonction audio_to
    int tab_temps[100];
    int tab_notes[100];
    srand(time(NULL));
    for (int i = 1; i <= 100; i++){
        tab_temps[i-1] = 100*i;
        tab_notes[i-1] = (rand() % 89);
    }
    piano_notes_to_video(tab_temps, tab_notes, 100);

    return 0;
}


void SDL_ExitWithError(char* message)
{
    SDL_Log("ERREUR : %s > %s\n",message,SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}


void update_renderer(SDL_Renderer *renderer, int t, int* tab_temps, int* tab_notes, int indice_derniere_note_en_cours,int animation_time,int nb_notes);

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
    //     SDL_ExitWithError ("Impossible de dessiner un point");
    
    /*----------------------------------------------------------------------------------*/



    /*Fonction utile : 
    
    SDL_GetTicks(); --> nb de milisecondes depuis SDL_init
    
    */


    /*------------------Gestion d'Evenements------------------*/
    int nb_note_deja_jouee = 0;
    int indice_derniere_note_en_cours = 0;
    int animation_time = 2000; //temps d'animation d'une note en ms
    SDL_bool program_launched = SDL_TRUE;
    int t = SDL_GetTicks();
    int tmax = tab_temps[nb_notes-1] + animation_time;
    
    while((program_launched) & (t<tmax))
    {   
        t = SDL_GetTicks(); 
        //Temps écoulé depuis le début (Depuis SDL_init) en ms

        if (tab_temps[indice_derniere_note_en_cours] < t - animation_time)
        { //Dans ce cas cette dernière note à finis son animation, la note suivante est la suivante à finir son animation
            indice_derniere_note_en_cours++;
        }

        update_renderer(renderer,t,tab_temps,tab_notes,indice_derniere_note_en_cours,animation_time,nb_notes);

        if ((nb_note_deja_jouee < nb_notes) & (tab_temps[nb_note_deja_jouee] < t))
        {
            printf("note %d au temps %d\n", tab_notes[nb_note_deja_jouee], t);
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


// Gestion de l'affichage graphique

void draw_faded_rectangle(SDL_Renderer *renderer,int x, int y,int width,int r,int g,int b, int r_bg, int g_bg, int b_bg);

void draw_rect(SDL_Renderer *renderer,int x, int y,int width,int height, int r,int g, int b);


void update_renderer(SDL_Renderer *renderer, int t, int* tab_temps, int* tab_notes, int indice_derniere_note_en_cours,int animation_time,int nb_notes)
{
    int largeur_note = WINDOW_WIDTH/88; //Il y'a 88 notes sur un piano
    int hauteur_note = WINDOW_HEIGHT/10;



    //On passe toutes les notes en blanche
        for(int j=0; j<89; j++)
        {   
            int x = largeur_note*j+largeur_note/2;
            draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note/2,largeur_note,hauteur_note,255,255,255);
        }
    
    //Notes en cours d'animation
    int i = indice_derniere_note_en_cours;
    while((tab_temps[i] < t)&(i < nb_notes))
    {   //La note i est en cours d'animation... et (t-tab_temps[i])/animation_time permet de situer où elle en est exactement dans son animation
        int x =  largeur_note*tab_notes[i]+largeur_note/2;
        int y = (WINDOW_HEIGHT-hauteur_note)*(t-tab_temps[i])/animation_time;

        //Plus tard on pourra changer la couleur (ici c'est du rouge), en fonction de la note joué par exemple.
        draw_faded_rectangle(renderer,x,y,largeur_note,255,0,0,0,0,0);

        if ((tab_temps[i] + animation_time - t) < 300){draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note/2,largeur_note,hauteur_note,255,200,200);}
        else {draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note/2,largeur_note,hauteur_note,255,255,255);}
        if (SDL_RenderDrawPoint(renderer, x , y) != 0)
            SDL_ExitWithError("Impossible de dessiner un point");
        i++;
    }
}


void draw_faded_rectangle(SDL_Renderer *renderer,int x, int y,int width, int r,int g, int b,int r_bg, int g_bg, int b_bg)
{
    /* Dessine un rectangle centré en x,y de largeur width, de hauteur Window_height/4 et faded en haut de couleur r,g,b*/
    int h = WINDOW_HEIGHT/4;
    for (int i = x-width/2; i < x+width/2;i++){
        for(int j=y-h/2; j<y+h/2;j++){
            if (SDL_SetRenderDrawColor(renderer,(2*(r-r_bg)*(j-y) + h*(r_bg + r))/(2*h),(2*(g-g_bg)*(j-y) + h*(g_bg + g))/(2*h),(2*(b-b_bg)*(j-y) + h*(b_bg + b))/(2*h),SDL_ALPHA_OPAQUE))
                SDL_ExitWithError("Impossible de changer la couleur pour le rendu");
            if (SDL_RenderDrawPoint(renderer, i , j) != 0)
                SDL_ExitWithError("Impossible de dessiner un point");

        }
    }

}


void draw_rect(SDL_Renderer *renderer,int x, int y,int width,int height, int r,int g, int b)
{
    /* Dessine un rectangle centré en x,y de largeur width, de hauteur height, de couleur r,g,b*/
    int h = WINDOW_HEIGHT/10;
    for (int i = x-width/2; i < x+width/2;i++){
        for(int j=y-h/2; j<y+h/2;j++){
            if (SDL_SetRenderDrawColor(renderer,r,g,b,SDL_ALPHA_OPAQUE))
                SDL_ExitWithError("Impossible de changer la couleur pour le rendu");
            if (SDL_RenderDrawPoint(renderer, i , j) != 0)
                SDL_ExitWithError("Impossible de dessiner un point");

        }
    }
}