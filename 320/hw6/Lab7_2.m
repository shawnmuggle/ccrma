fs = 128;
dur_seconds = 1;
N = fs * dur_seconds;
T = 1 / fs;
n = [0:N - 1];
x = cos(2 * pi * 16.0625 * n * T);
w = nuttallwin(N)';
windowed_signal = w .* x;

subplot(2, 1, 1);
plot(windowed_signal);
title('Sinusoid x Nuttall Window');
xlabel('Samples');
ylabel('Amplitude');

zpf = 8;
windowed_zero_padded_signal = [windowed_signal zeros(1, N)];
subplot(2, 1, 2);
plotspec(windowed_zero_padded_signal);
title('Magnitude Spectra of Nuttall Windowed Sinusoid');
xlabel('Frequency');
ylabel('Magnitude');


% I chose to use the Nuttall window after researching different windows
% because I wanted to see its extreme tradeoff of mainlobe width for
% sidelobe height/falloff