window_length = 128;
zpf = 8;

w1 = boxcar(window_length)';
subplot(2, 3, 1);
plot(w1);
title('Boxcar Window');
xlabel('Time');
ylabel('Amplitude');
w1_padded = [w1 zeros(1, window_length * (zpf - 1))];
subplot(2, 3, 4);
plotspec(w1_padded);

w1 = bartlett(window_length)';
subplot(2, 3, 2);
plot(w1);
title('Bartlett Window');
xlabel('Time');
ylabel('Amplitude');
w1_padded = [w1 zeros(1, window_length * (zpf - 1))];
subplot(2, 3, 5);
plotspec(w1_padded);

w1 = hann(window_length)';
subplot(2, 3, 3);
plot(w1);
title('Hanning Window');
xlabel('Time');
ylabel('Amplitude');
w1_padded = [w1 zeros(1, window_length * (zpf - 1))];
subplot(2, 3, 6);
plotspec(w1_padded);

% best to worst
% peak magnitude: boxcar, bartlett, hanning
% main lobe width: boxcar, bartlett, hanning
% relative height of first sidelobe: hanning, bartlett, boxcar
% falloff rate: hanning, bartlett, boxcar

% main lobe width affects frequency resolution, and boxcar is best at that
% because it takes the greatest amount of the original signal into
% consideration.

% higher sidelobes or a slower falloff rate means there will be increased
% spectral leakage from discontinuities introduced at the edge of the
% sampled region. The Hanning window has the best falloff and lowest
% sidelobes because it smooths out the discontinuities most dramatically.