N = 1024; % window length
w = (.42 - .5*cos(2*pi*(0:N-1)/(N-1)) + .08*cos(4*pi*(0:N-1)/(N-1)));
% Blackman window

fs = 8192;
t = [0:N-1];
x = 0.6 * cos(0.25 * 2 * pi + (t .* (1000 * 2 * pi)) ./ fs);

size(w)
size(x)

wx = x .* w;

subplot(3,2,1);
plot(w);
title('window');
xlabel('sample');
ylabel('magnitude');

subplot(3,2,3);
plot(x);
title('signal');
xlabel('sample');
ylabel('magnitude');

subplot(3,2,5);
plot(wx);
title('windowed signal');
xlabel('sample');
ylabel('magnitude');

s = fft(x, 4*N);
subplot(3,2,2);
plot(20 * log10(abs(fftshift(s ./ max(abs(s))))));
title('spectrum of 1000 Hz sinusoid, windowed with blackman');
ylim([-80 0]);
xlabel('frequency bin');
ylabel('log magnitude');

w2 = boxcar(N);
x2 = 0.6 * cos(0.25 * 2 * pi + (t .* (1023 * 2 * pi)) ./ fs);
x3 = 0.6 * cos(0.25 * 2 * pi + (t .* (1024 * 2 * pi)) ./ fs);

s = fft(x2, 4*N);
subplot(3,2,4);
plot(20 * log10(abs(fftshift(s ./ max(abs(s))))));
title('spectrum of 1023 Hz sinusoid, windowed with boxcar');
ylim([-80 0]);
xlabel('frequency bin');
ylabel('log magnitude');

s = fft(3, 4*N);
subplot(3,2,6);
plot(20 * log10(abs(fftshift(s ./ max(abs(s))))));
title('spectrum of 1024 Hz sinusoid, windowed with boxcar');
xlabel('frequency bin');
ylabel('log magnitude');
