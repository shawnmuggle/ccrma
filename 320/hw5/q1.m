fs = 128
T = 1 / fs
n = [0:fs];
x = sin(2 * pi * 16.0625 * n * T);
plotspec(x, fs);
% Oh no, it's not right!

zpf = 17;
x = [x zeros(1, length(x) * (zpf - 1))];
plotspec(x, fs);