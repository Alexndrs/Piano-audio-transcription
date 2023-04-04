%% LECTURE DU FICHIER AUDIO

[y,Fe] = audioread('do re mi fa so la.wav');

%y -> tableau des valeur du signal normalisés entre -1 et 1
%Fs -> fréquence d'échantillonage

sz = size(y);
L = sz(1); %L -> nombre de valeurs échantillonés

tab_t = [0:(1/Fe):((L-1)/Fe)]; %t -> tableau des temps d'échantillonage

t_gap = 0.2;
for k = 1:L/1000
    t = tab_t(1000*k);
    t1 = t - t_gap/2;
    t2 = t + t_gap/2;
    y_res = fenetrage_hamming(y,Fe,t1,t2);
    plot(tab_t,y_res);
    %ylim([-1.5 1.5])
    pause(0.1);
    %% CALCUL DE LA TRANSFORMEE DE FOURIER

    X = fft(y_res);

    %spectre sans le repliement (signal restreint de 0 à Fs/2)
    TF = X(1:floor(L/2)+1);  
    tab_f = Fe*(0:(L/2))/L;
    plot(tab_f,abs(TF));

    %% CALCUL DE FMAX
    H = 5;
    seuil = 30;
    f_res = fmax(TF, H, tab_f, seuil)
    title(['f : ',num2str(f_res),'/// t : ',num2str(t)]);
    xlim([0 1000])
    ylim([0 2000])
    pause(0.01);
end

