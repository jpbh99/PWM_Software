filename = 'TEK1.CSV';
X = csvread(filename);

s = X(:,2);
t = X(:,1);

Ts = mean(diff(t));
Fs = 1/Ts;
L = length(t);                                     
Fn = Fs/2;                                          
FX = fft(s)/L;                                     
Fv = linspace(0, 1, fix(L/2)+1)*Fn;                 
Iv = 1:length(Fv);                                  
figure(1);
semilogy(Fv, abs(FX(Iv))*2)
xlim([0 872])
grid
title('FFT')
xlabel('Frecuencia (Hz)')
ylabel('Amplitud')