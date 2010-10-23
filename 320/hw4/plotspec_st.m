function plotspec_st (x, fs, time, Nf)
% function plotspec_st (x, fs, time, Nf)
% A function to quickly plot the spectrum of a signal
% segment centered at a given time
%
% x: input signal (assume a row vector)
% fs: sampling rate of x
% time: the time at which you want to see the spectrum
% Nf: frame (or slice) size
%
% Mike Rotondo / Lab 4-2
start_sample = int32(time * fs - (Nf * fs)/2)
short_signal = x(start_sample:start_sample + (Nf * fs) - 1);
window = hann(Nf * fs);

size(window)
size(short_signal)

windowed_signal = short_signal .* window';
plotspec(windowed_signal, fs);