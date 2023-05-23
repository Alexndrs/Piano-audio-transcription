# Piano-audio-transcription gp 10

Ce projet consiste à récupérer un fichier mono audio.wav.
Il détecte ensuite les notes joués.
Puis il affiche une fenêtre SDL les notes détectés, et il joue le fichier audio en simultané pour vérifier si les notes détectés sont les bonnes.
____
Librairies utilisés : 
- GSL
- SDL2

____
Le projet est donc décomposé en deux grandes fonction : 

> piano_audio_to_piano_note -> renvoie les notes détectés
> piano_notes_to_video -> affiche la fenêtre SDL et joue les notes détectés et l'audio



