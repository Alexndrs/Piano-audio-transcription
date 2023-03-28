
int nb_data( ??? )    
{
    /*---------------------ouverture du wave----------------------------------------*/
    FILE *wav = fopen(fichieraudio,"rb"); //ouverture du fichier wave
    struct wavfile header; //creation du header

    if ( wav == NULL )
    {
        printf("\nne peut pas ouvrir le fichier demande, verifier le nom\n");
        printf("ne pas oublier l'extention .wav\n");
        exit(1);
    }
    /*---------------------fin de ouverture du wave---------------------------------*/



    /*---------------------lecture de l entete et enregistrement dans header--------*/
    //initialise le header par l'entete du fichier wave
    //verifie que le fichier posséde un entete compatible
    if ( fread(&header,sizeof(header),1,wav) < 1 )
    {
        printf("\nne peut pas lire le header\n");
        exit(1);
    }
    if (    header.id[0] != 'R'|| header.id[1] != 'I'|| header.id[2] != 'F'|| header.id[3] != 'F' )
    {
        printf("\nerreur le fichier n'est pas un format wave valide\n");
        exit(1);
    }
    if (header.channels!=1)
    {
        printf("\nerreur : le fichier n'est pas mono\n");
        exit(1);
    }
    /*----------------fin de lecture de l entete et enregistrement dans header-------*/



    /*-------------------------determiner la taille des tableaux---------------------*/
    nbech=(header.bytes_in_data/header.bytes_by_capture);
    /*------------------fin de determiner la taille des tableaux---------------------*/

    return(nbech);
}