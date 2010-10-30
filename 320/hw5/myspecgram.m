function myspecgram (x, fs, Nf)
% function myspecgram (x, fs, Nf)
% A function to plot the spectrogram of input signal
% using hann window and zpf = 8
%
% x: input signal (assume a row vector)
% fs: sampling rate of x
% Nf: frame size
%
% Mike Rotondo / Lab 5-2

zpf = 8;

fft_length = 2^nextpow2(zpf * Nf);

window = hanning(fft_length)';

X = [];
i = 0;
idx = 0;

while (idx < length(x))
    idx = i * Nf;
    upperbound = idx + Nf;
    subsignal = x(idx + 1:min(length(x) - 1, upperbound));
    padded_subsignal = [subsignal zeros(1, fft_length - length(subsignal))];
    windowed_subsignal = window .* padded_subsignal;
    X = [X 20 * log10(abs(fftshift(fft(windowed_subsignal))))'];
    i = i + 1;
end
colormap('jet');
imagesc(X);