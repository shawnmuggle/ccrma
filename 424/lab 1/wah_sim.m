function [transfer] = wah_sim(gamma, wc, Q)
fs = 44100;
b = [gamma/wc 0];
a = [1/wc^2 1/(wc * Q) 1];
%w = logspace(2 * pi * 1 / (fs/2), 2 * pi * 22050 / (fs/2), 2049);
w = logspace(1, log10(22050), 2049);
h = freqs(b, a, w);
mag = 20 * log10(abs(h));
semilogx(w, mag);