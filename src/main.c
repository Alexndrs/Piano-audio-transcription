/*
Compilation dans le fichier ~/Piano-audio-transcription:

  gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I include -L lib -lmingw32 -lSDL2main -lSDL2

pour généré un prog.exe sans fenetre de commande qui s'ouvre:
  gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lm

include GSL L- lib -lgslcblas -lgsl -lm -o bin/test.exe : 


gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I include -L lib -lgslcblas -lgsl -lmingw32 -lSDL2main -lSDL2 -lm

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


/*-----------Ecriture dans un fichier .txt ----------*/
/*
   Ces fonctions vont servir à tester nos autres fonctions en enregistrant des tableaux dans des fichier .txt
   qui seront lu en python pour pouvoir tracer des courbes.
*/

void float_tab_to_txt(float* tab, int nb_elem, char* nom_tableau) {
    FILE *fichier = fopen(nom_tableau, "w");
    
    for (int i = 0; i < nb_elem; i++) {
        fprintf(fichier, "%f\n", tab[i]);
    }
    
    fclose(fichier);
}

void short_tab_to_txt(short* tab, int nb_elem, char* nom_tableau) {
    FILE *fichier = fopen(nom_tableau, "w");
    
    for (int i = 0; i < nb_elem; i++) {
        if (tab[i] > 0)
            fprintf(fichier, "%hd\n", tab[i]);
        else fprintf(fichier, "%hd\n", 0);
    }
    
    fclose(fichier);
}

/*------------------  Fonction lect_audio.c  ----------------*/

struct wavfile //définit la structure de l entete d un wave
{
    char        id[4];          // doit contenir "RIFF"
    int         totallength;        // taille totale du fichier moins 8 bytes
    char        wavefmt[8];     // doit etre "WAVEfmt "
    int         format;             // 16 pour le format PCM
    short       pcm;              // 1 for PCM format
    short       channels;         // nombre de channels
    int         frequency;          // frequence d echantillonage
    int         bytes_per_second;   // nombre de bytes par secondes
    short       bytes_by_capture;
    short       bits_per_sample;  // nombre de bytes par echantillon
    char        data[4];        // doit contenir "data"
    int         bytes_in_data;      // nombre de bytes de la partie data
};



void nb_data_Fe(FILE *wav, int* p_nb_data, float* p_Fe){
    //creation et initialisation du header
    struct wavfile header;
    

    //Gestion des erreurs de lecture
    if ( wav == NULL ){
        printf("\nne peut pas ouvrir le fichier demande, verifier le nom\n");
        printf("ne pas oublier l'extention .wav\n");
        *p_nb_data = -1;
        return;
    }
    if ( fread(&header,sizeof(header),1,wav) < 1 ){
        printf("\nne peut pas lire le header\n");
        *p_nb_data = -1;
        return;
    }
    if ((header.id[0] != 'R')||(header.id[1] != 'I')||(header.id[2] != 'F')||(header.id[3] != 'F')){
        printf("\nerreur le fichier n'est pas un format wave valide\n");
        *p_nb_data = -1;
        return;
    }
    if (header.channels!=1){
        printf("\nerreur : le fichier n'est pas mono\n");
        *p_nb_data = -1;
        return;
    }

    //Si tout se passe bien...
    *p_nb_data = (header.bytes_in_data/header.bytes_by_capture);
    *p_Fe = header.frequency;
}



void rempli_tab(FILE *wav, float* tab_temps, short* tab_amplitudes){
    //creation et initialisation du header
    struct wavfile header;
    

    //Gestion des erreurs de lecture
    if ( wav == NULL ){
        printf("\nne peut pas ouvrir le fichier demande, verifier le nom\n");
        printf("ne pas oublier l'extention .wav\n");
        return ;
    }
    if ( fread(&header,sizeof(header),1,wav) < 1 ){
        printf("\nne peut pas lire le header\n");
        return ;
    }
    if ((header.id[0] != 'R')||(header.id[1] != 'I')||(header.id[2] != 'F')||(header.id[3] != 'F')){
        printf("\nerreur le fichier n'est pas un format wave valide\n");
        return ;
    }
    if (header.channels!=1){
        printf("\nerreur : le fichier n'est pas mono\n");
        return ;
    }

    //Remplissage du tableau
    int i=0;
    short value=0;
    while(fread(&value,(header.bits_per_sample)/8,1,wav)){
        //lecture des echantillons et enregistrement dans le tableau (en dB)
        tab_amplitudes[i] = 20*log10(fabs(value));
        if (tab_amplitudes[i] < 0)
            tab_amplitudes[i] = 0;
        tab_temps[i] = 1.*i/header.frequency;
        i++;
    }
}


/*-------------  Fonction de fenetrage_hamming.c  -----------------*/

void fenetrage_hamming(short* tab_amplitude, short* amplitude_fenetree, int nb_data, float Fe, float t1, float t2)
{
    float Te = 1/Fe;
    for(int k=0; k < nb_data; k++)
    {
        float t = k*Te;
        if(t>t1 && t<t2)
        {
            amplitude_fenetree[k] = tab_amplitude[k]*(0.54 - 0.46 * cos(2*3.141592653589793*(t-t1)/(t2 - t1)));
        }
        else
        { 
            amplitude_fenetree[k] = 0;
        }
    }
}

/*-----------  Fonction de transformee_fourier.c  ---------------*/

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

void FourierTransform(short* tab_amplitudes_temporel, int nb_data, double* tab_amplitude_frequenciel){
  /*Transformée de Fourier d'un tableau d'amplitudes*/

  gsl_fft_complex_wavetable* wavetable;
  gsl_fft_complex_workspace* workspace;

  for (int i = 0; i < nb_data; i++) {
      REAL(tab_amplitude_frequenciel, i) = tab_amplitudes_temporel[i];
      IMAG(tab_amplitude_frequenciel, i) = 0.0;
  }

  wavetable = gsl_fft_complex_wavetable_alloc(nb_data);
  workspace = gsl_fft_complex_workspace_alloc(nb_data);

  gsl_fft_complex_forward(tab_amplitude_frequenciel, 1, nb_data, wavetable, workspace);


  gsl_fft_complex_wavetable_free(wavetable);
  gsl_fft_complex_workspace_free(workspace);
}


/*------------  fonction de freq_preponderante.c  -----------*/

float frequence_preponderante(double* tab_amplitude,int Fe,float seuil, int H, int len_tab)
{
    float fprep = -1;
    float amp_max = seuil;
    float f;
    float prod_spec;
    for(int k = 20*(242765./44100); k < (len_tab/H) ;k++) //C'est la première fréquence audible tel que (k/len_tab)*Fe > 20Hz
    {
        f = k*Fe/len_tab;
        prod_spec = 1;
        for (int i = 1; i <= H; i++)        // On indice de 1 à H compris pour prendre en compte le fondamental
        {
            double r = REAL(tab_amplitude,i*k);
            double im = IMAG(tab_amplitude,i*k);
            prod_spec *=(r*r + im*im);
        }

        if(log10(prod_spec) > amp_max)
        {
            amp_max = log10(prod_spec);
            fprep = f;
        }
    }
    if (fprep == -1){printf("Il ne s'est rien passé\n");}
    return (fprep); 
}

/*--------  fonction frequence_to_note  ----------*/

int frequency_to_note_number(float frequency) {
    float A4_frequency = 440.0; // A4 has a frequency of 440 Hz
    float C0_frequency = A4_frequency * pow(2.0, -4.75); // C0 has a frequency of about 16.35 Hz

    float half_steps_above_c0 = log2(frequency / C0_frequency) * 12.0;
    int note_number = round(half_steps_above_c0);

    return note_number;
}

/*--------  fonction piano_audio_to_piano_notes  ----------*/

void piano_audio_to_piano_note(char* nom_fichier_audio, int* tab_temps_notes, int* tab_notes, int* nb_notes_jouees){

    FILE *wav = fopen(nom_fichier_audio,"rb");
    printf("\n nom_fichier : %s\n", nom_fichier_audio);
    int len_tab;
    float Fe;
    nb_data_Fe(wav, &len_tab, &Fe);
    printf("\n nombre d'echantillons et frequence : %d, %f\n\n",len_tab,Fe);
    if (len_tab == -1){
        printf("Erreur de lecture\n");
        tab_temps_notes = NULL;
        tab_notes = NULL;
        return ;
    }

    //  RECUPERATION DES TEMPS D'ECHANTILLONAGE ET DES AMPLITUDES ASSOCIEES
    float* tab_temps = malloc(sizeof(float) * len_tab);
    short* tab_amplitude = malloc(sizeof(short) * len_tab);
    if (tab_temps == NULL || tab_amplitude == NULL){
        printf("Données trop volumineuses\n");
        tab_temps_notes = NULL;
        tab_notes = NULL;
        return ;
    }
    wav = fopen(nom_fichier_audio,"rb");
    rempli_tab(wav, tab_temps, tab_amplitude);


    // Boucle de traitement : fenetrage, transformée de fourier, fréquence détectée
    float T_total = len_tab/Fe; // durée de l'enregistrement
    float tau = 0.1;       // pas de décalage temporel entre 2 fenêtres (en secondes)
    float t1 = 0;
    float t2 = t1 + tau;
    float* tab_frequence = malloc(sizeof(float)*len_tab);
    short* amplitude_fenetree = malloc(sizeof(short)*len_tab);
    double* amplitude_fourier = malloc(2*sizeof(double)*len_tab);          // La fonction Transformee_Fourier ne s'applique que sur des tableaux de double et ces tableaux sont composés de nombres complexes d'où le 2*sizeof(double)*len_tab.
    int i = 0;
    *nb_notes_jouees = 0;
    while (t2<T_total)
    {

        // Fenetrage
        fenetrage_hamming(tab_amplitude, amplitude_fenetree, len_tab, Fe, t1, t2);

        // Transformée de fourier
        FourierTransform(amplitude_fenetree, len_tab, amplitude_fourier);
        // Détection de la fréquence
        int H = 5; // nombre d'harmoniques qu'on étudie
        float seuil = 35; //seuil en dB pour l'étude des fréquences
        float fprep = frequence_preponderante(amplitude_fourier,Fe,seuil,H,len_tab);

        // Mise des notes dans le tableau
        printf("[t1,t2] = [%f,%f],  fprep = %f\n\n\n", t1,t2,fprep);
        if (fprep != -1){
            tab_notes[i] = frequency_to_note_number(fprep);
            tab_temps_notes[i] = floor(1000 * (t1 + t2)/2); //Tableau en milisecondes 
            i++;
            (*nb_notes_jouees)++;
        }

        t1 = t1 + tau;
        t2 = t2 + tau;
    }
    printf("Test1");
    free(tab_temps);
    printf("Test2");
    free(amplitude_fenetree);
    printf("Test3");
    free(amplitude_fourier);
    printf("Test4");
    free(tab_frequence);
    printf("Test5");
    free(tab_amplitude);
    printf("Test6");
}



/*--------  fonctions pour la partie graphique  -----------*/

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



void piano_notes_to_video(int* tab_temps,int* tab_notes,int nb_notes, char* audio_name)
{
    
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


    // Lance l'audio
        SDL_PauseAudioDevice(deviceId, 0);


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

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


/*-------- Main  ----------*/


int main(int argc, char* argv[]){

    int* tab_temps_notes = malloc(sizeof(int)*100);
    int* tab_notes = malloc(sizeof(int)*100);
    int nb_notes_jouees;

    //Lecture et enregistrement des notes jouees
    piano_audio_to_piano_note("test_audio.wav", tab_temps_notes, tab_notes, &nb_notes_jouees);

    printf("nombre de notes jouees : %d\n",nb_notes_jouees);


    //Affichage graphique
    piano_notes_to_video(tab_temps_notes, tab_notes, nb_notes_jouees, "test_audio.wav");

    free(tab_temps_notes);
    free(tab_notes);
    return 0;
}




// // Define MAX and MIN macros
// #define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
// #define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// // Define screen dimensions
// #define SCREEN_WIDTH    800
// #define SCREEN_HEIGHT   600

// #define WAVES_SOUND "./test_audio.wav"

// int main(int argc, char* argv[])
// {

//     // Initialize SDL
//     if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
//     {
//         printf("SDL could not be initialized!\n"
//                "SDL_Error: %s\n", SDL_GetError());
//         return 0;
//     }


//     // Create window
//     SDL_Window *window = SDL_CreateWindow("SDL2 audio sample (Press SPACE to pause/play)",
//                                           SDL_WINDOWPOS_UNDEFINED,
//                                           SDL_WINDOWPOS_UNDEFINED,
//                                           SCREEN_WIDTH, SCREEN_HEIGHT,
//                                           SDL_WINDOW_SHOWN);
                                          
//     if(!window)
//     {
//         printf("Window could not be created!\n"
//                "SDL_Error: %s\n", SDL_GetError());
//     }
//     else
//     {
//         // Create renderer
//         SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
//         if(!renderer)
//         {
//             printf("Renderer could not be created!\n"
//                    "SDL_Error: %s\n", SDL_GetError());
//         }
//         else
//         {
//             // Load .WAV sound
//             SDL_AudioSpec wavSpec;
//             Uint32 wavLength;
//             Uint8 *wavBuffer;
//             if(!SDL_LoadWAV(WAVES_SOUND, &wavSpec, &wavBuffer, &wavLength))
//             {
//                 printf(".WAV sound '%s' could not be loaded!\n"
//                        "SDL_Error: %s\n", WAVES_SOUND, SDL_GetError());
//                 return 0;
//             }

//             // Open audio device
//             SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
//             if(!deviceId)
//             {
//                 printf("Audio device could not be opened!\n"
//                        "SDL_Error: %s\n", SDL_GetError());
//                 SDL_FreeWAV(wavBuffer);
//                 return 0;
//             }

//             // Queue audio (~ Add to playlist)
//             if(SDL_QueueAudio(deviceId, wavBuffer, wavLength) < 0)
//             {
//                 printf("Audio could not be queued!\n"
//                        "SDL_Error: %s\n", SDL_GetError());
//                 SDL_CloseAudioDevice(deviceId);
//                 SDL_FreeWAV(wavBuffer);
//                 return 0;
//             }

//             // Play audio
//             SDL_PauseAudioDevice(deviceId, 0);


//             // Event loop exit flag
//             int quit = 0;

//             // Event loop

//             while(!quit)
//             {
//                 SDL_Event e;

//                 // Wait indefinitely for the next available event
//                 SDL_WaitEvent(&e);

//                 // User requests quit
//                 if(e.type == SDL_QUIT)
//                 {
//                     quit = 1;
//                 }
                
//                 else if(e.type == SDL_KEYDOWN)
//                 {
//                     switch (e.key.keysym.sym)
//                     {
//                     case SDLK_SPACE:
//                         if(SDL_GetAudioDeviceStatus(deviceId) == SDL_AUDIO_PLAYING)
//                             SDL_PauseAudioDevice(deviceId, 1);
//                         else if(SDL_GetAudioDeviceStatus(deviceId) == SDL_AUDIO_PAUSED)
//                             SDL_PauseAudioDevice(deviceId, 0);
//                         break;
//                     }
//                 }

//                 // Initialize renderer color white for the background
//                 SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

//                 // Clear screen
//                 SDL_RenderClear(renderer);

//                 // // Set renderer color blue to draw the square
//                 // SDL_SetRenderDrawColor(renderer, 0x19, 0x71, 0xA9, 0xFF);

//                 // // Draw filled square
//                 // SDL_RenderFillRect(renderer, &squareRect);

//                 // Check pause status


//                 // Update screen
//                 SDL_RenderPresent(renderer);
//             }

//             // Clean up audio
//             SDL_CloseAudioDevice(deviceId);
//             SDL_FreeWAV(wavBuffer);

//             // Destroy renderer
//             SDL_DestroyRenderer(renderer);
//         }

//         // Destroy window
//         SDL_DestroyWindow(window);
//     }

//     // Quit SDL
//     SDL_Quit();

//     return 0;
// }