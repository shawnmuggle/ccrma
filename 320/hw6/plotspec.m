function plotspec (x, fs)
% function plotspec (x, fs)
% A function to quickly plot the spectrum of a time domain signal
%
% plotspec(x)
% when no sampling rate is specified, normalize frequency
%
% plotspec(x, fs)
% when a sampling rate is given, set the frequency axis in [Hz]
%
% Mike Rotondo / Lab 4-1

plot_title = 'Magnitude Spectrum';
y_axis_label = 'Magnitude (dB)';
x_axis_label = 'Frequency (Hz)';

if nargin == 1
  x_axis_label = 'Frequency';
  fs = 1.0;
end

x_axis = linspace(-fs / 2, fs / 2, length(x));

f = fft(x);
db_magnitude = 20 * log10(abs(f));
plot(x_axis, fftshift(db_magnitude));
title(plot_title);
xlabel(x_axis_label);
ylabel(y_axis_label);
