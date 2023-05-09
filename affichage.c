/*--------  fonctions pour la partie graphique  -----------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void SDL_ExitWithError(char* message)
{
    SDL_Log("ERREUR : %s > %s\n",message,SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
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



void update_renderer(SDL_Renderer *renderer, int t, int* tab_temps, int* tab_notes, int indice_derniere_note_en_cours,int animation_time,int nb_notes)
{
    int largeur_note_blanche = WINDOW_WIDTH/52; //Il y'a 52 touches blanches sur un piano
    int hauteur_note_blanche = WINDOW_HEIGHT/10;
    int largeur_note_noire = largeur_note_blanche/2; //les touches noires sont un peu plus petites que les blanches
    int hauteur_note_noire = 2/3*hauteur_note_blanche; //et plus courtes



    //On dessine toutes les notes blanche
        for(int j=0; j<52; j++)
        {   
            int x = largeur_note_blanche*j+largeur_note_blanche/2;
            draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_blanche,hauteur_note_blanche,255,255,255);
        }
    //on dessine toutes les notes noires
        int xj_noir = largeur_note_blanche; //Sur un piano il y a toujours une première touche noire seule au début
        draw_rect(renderer,xj_noir,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_noire,hauteur_note_noire,0,0,0);
        xj_noir = 2*largeur_note_blanche;
        int compteur = 1;   //dans un piano il y a soit deux bandes noires pour 3 blanches, soit 3 noires pour 4 blanches, on va traiter ces 2 cas
        int j = 0;
        while(j<36)   //se finit bien
        {
            if (compteur == 1)
            {
                for(int k = 1; k<3;++k)
                {
                    ++j;
                    xj_noir = xj_noir + largeur_note_blanche;
                    draw_rect(renderer,xj_noir,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_noire,hauteur_note_noire,0,0,0);
                }
                xj_noir += largeur_note_blanche;
                compteur = 2;
            }

            if (compteur == 2)
            {
                for(int k = 1; k<4;++k)
                {
                    ++j;
                    xj_noir = xj_noir + largeur_note_blanche;
                    draw_rect(renderer,xj_noir,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_noire,hauteur_note_noire,0,0,0);
                }
                xj_noir += largeur_note_blanche;
                compteur = 1;
            }
            
        }
    
    //Notes en cours d'animation
    int i = indice_derniere_note_en_cours;
    while((tab_temps[i] < t)&(i < nb_notes))
    {   //La note i est en cours d'animation... et (t-tab_temps[i])/animation_time permet de situer où elle en est exactement dans son animation
        if(tab_notes[i] == 2 || (tab_notes[i]-3)%12 == 2 || (tab_notes[i]-3)%12 == 4 || (tab_notes[i]-3)%12 == 7 || (tab_notes[i]-3)%12 == 9 || (tab_notes[i]-3)%12 == 11) //pour les notes noires
        {
            int x =  largeur_note_blanche/2*(tab_notes[i]+1+(tab_notes[i]-3)/12);
            int y = (WINDOW_HEIGHT-hauteur_note_blanche)*(t-tab_temps[i])/animation_time;

            //Plus tard on pourra changer la couleur (ici c'est du cyan), en fonction de la note joué par exemple.
            draw_faded_rectangle(renderer,x,y,largeur_note_blanche,0,255,255,0,0,0);

            if ((tab_temps[i] + animation_time - t) < 300){draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_blanche,hauteur_note_blanche,255,200,200);}
            else {draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_blanche,hauteur_note_blanche,255,255,255);}
            if (SDL_RenderDrawPoint(renderer, x , y) != 0)
            {
                SDL_ExitWithError("Impossible de dessiner un point");
            }
            i++;
        }
        else //pour les notes blanches
        {
            int x =  largeur_note_blanche/2*(tab_notes[i]+1+(tab_notes[i]-3)/12)+largeur_note_blanche/2;
            if(tab_notes[i]%12>5){x += largeur_note_blanche/2;}
            int y = (WINDOW_HEIGHT-hauteur_note_blanche)*(t-tab_temps[i])/animation_time;

            //Plus tard on pourra changer la couleur (ici c'est du rouge), en fonction de la note joué par exemple.
            draw_faded_rectangle(renderer,x,y,largeur_note_blanche,255,0,0,0,0,0);

            if ((tab_temps[i] + animation_time - t) < 300){draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_blanche,hauteur_note_blanche,255,200,200);}
            else {draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_blanche,hauteur_note_blanche,255,255,255);}
            if (SDL_RenderDrawPoint(renderer, x , y) != 0)
            {
                SDL_ExitWithError("Impossible de dessiner un point");
            }
            i++;
        }

    }
}



void piano_notes_to_video(int* tab_temps,int* tab_notes,int nb_notes, char* audio_name)
{
    printf("Debut Video\n");
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Lancement SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        SDL_ExitWithError("Initialisation SDL echouee");
        SDL_Quit();
        return;
    }

    // Creation fenêtre + rendu
    if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0){
        SDL_ExitWithError("Impossible de creer la fenetre et le rendu");
        SDL_Quit();
        return;
    }

    // Chargement de l'audio .WAV           
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;
    if(!SDL_LoadWAV(audio_name, &wavSpec, &wavBuffer, &wavLength))
    {
        printf("le son .WAV: '%s' n'a pas pu être chargé!\n"
                "SDL_Error: %s\n", audio_name, SDL_GetError());
        return;
    }

    // Ouverture audio device
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    if(!deviceId)
    {
        printf("L'audio device n'a pas pu être ouvert!\n"
                "SDL_Error: %s\n", SDL_GetError());
        SDL_FreeWAV(wavBuffer);
        return;
    }

    // Ajout à la file (playlist)
    if(SDL_QueueAudio(deviceId, wavBuffer, wavLength) < 0)
    {
        printf("L'audio n'a pas pu être ajouté à la playlist!\n"
                "SDL_Error: %s\n", SDL_GetError());
        SDL_CloseAudioDevice(deviceId);
        SDL_FreeWAV(wavBuffer);
        return;
    }
        

    /*-------------- Fonctions utiles en SDL:----------------------

    ####### Dessin d'un point coloré:

    if (SDL_SetRenderDrawColor(renderer,150,159,255, SDL_ALPHA_OPAQUE) !=0 )
        SDL_ExitWithError("Impossible de changer la couleur pour le rendu");
    
    if (SDL_RenderDrawPoint(renderer, 400, 300) != 0)
        SDL_ExitWithError("Impossible de dessiner un point");
    

    ####### Décompte du temps:

    SDL_GetTicks(); --> nb de milisecondes depuis SDL_init

    ----------------------------------------------------------------------------------*/


    /*------------------Gestion d'Evenements------------------*/



    int audio_has_started = 0;
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

        if (t > animation_time && (audio_has_started==0)){
            // Lance l'audio
             SDL_PauseAudioDevice(deviceId, 0);
             audio_has_started = 1;
        }

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

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


/*-------- Main  ----------*/


int main(int argc, char* argv[]){

    int* tab_temps_notes = malloc(sizeof(int)*1000);
    int* tab_notes = malloc(sizeof(int)*1000);
    int nb_notes_jouees;

    //Lecture et enregistrement des notes jouees
    piano_audio_to_piano_note("test_audio.wav", &tab_temps_notes, &tab_notes, &nb_notes_jouees);

    printf("nombre de notes jouees : %d\n",nb_notes_jouees);
    for(int i=0; i<nb_notes_jouees; i++){
        printf("%d,%d \n", tab_temps_notes[i], tab_notes[i]);
    }


    //Affichage graphique
    piano_notes_to_video(tab_temps_notes, tab_notes, nb_notes_jouees, "test_audio.wav");

    free(tab_temps_notes);
    free(tab_notes);
    return 0;
}


