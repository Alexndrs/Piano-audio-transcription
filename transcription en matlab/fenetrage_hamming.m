function y_res = fenetrage_hamming(y,Fe,t1,t2)
%retourner le signal y_res = y * h où h est la fenêtre de hamming entre t1
%et t2

y_res = y;
nb_val = size(y,1);
Te = 1/Fe;

for k=1:nb_val
    t = k*Te;
    if ((t<t1)||(t>t2))
        y_res(k) = 0;
    else
        y_res(k) = y(k) * (0.54 - 0.46 * cos(2*pi*(t-t1)/(t2 - t1)));
    end
end


