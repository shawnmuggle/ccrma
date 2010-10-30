% Mike Rotondo - 320 Lab 5-1

fs = 128
T = 1 / fs
n = [0:fs];
x = sin(2 * pi * 16.0625 * n * T);

subplot(2, 1, 1);

plotspec(x, fs);
% Oh no, it's not right!
title('Non-Zero-Padded');

subplot(2, 1, 2);
zpf = 17;
x = [x zeros(1, length(x) * (zpf - 1))];
plotspec(x, fs);
title('Zero-Padded (zpf 17)');
