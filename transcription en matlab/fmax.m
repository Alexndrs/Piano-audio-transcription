function f_res = fmax(TF, H, tab_f, seuil)
% Cette fonction calcule f maximisant log(produit(|TF(if)|^2, 1<=i<=H) if
% TF est un tableau d'amplitudes
% H est le nombre de multiplicité dans le calcul
% tab_f est le tableau des fréquence : entre 0 et Fe = 1/Te avec un pas de
% Fe/nb_val

nb_val = size(tab_f,2);


%caclul du max qui doit dépasser un seuil
best = seuil;
f_res = -1;
for k = 1:(nb_val/H)
    f = tab_f(k);
    
    prod = 1;
    for i =1:H
        prod = prod * abs(TF(k*i))^2;
    end
%    if (mod(k,100) == 0)
%        log(prod)
%    end
    if (log(prod) > best)
        best = log(prod);
        f_res = f;
    end
end
end
