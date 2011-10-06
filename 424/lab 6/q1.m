% q1a

% I used a slow detector and a fast detector which released to the level of
% the slow detector to implement program dependence. For illustration, I
% output the actual levels of the detectors instead of the compressed
% signal.

s = wavread('program dependent filtstates.wav');
s1 = s(:, 1);
s2 = s(:, 2);
length_sec = length(s) / 44100.0;
t = linspace(0, 3, length(s));
subplot(2, 1, 1);
plot(t, s1);
axis([0 3 -1 1]);
title('Slow Detector');
xlabel('Time (s)');
ylabel('Detected Level (linear)');
subplot(2, 1, 2);
plot(t, s2);
axis([0 3 -1 1]);
title('Fast Detector (releasing to slow detector');
xlabel('Time (s)');
ylabel('Detected Level (linear)');
