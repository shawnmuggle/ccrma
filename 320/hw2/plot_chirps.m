% Mike Rotondo / Lab 2-3-b

subplot(4, 1, 1);
plot(beat_sweep(1, 1, 100, [30 30], 44100, .1, 'bar'));
subplot(4, 1, 2);
plot(beat_sweep(1, 1, 300, [80 80], 44100, 0.06, 'bar'));
subplot(4, 1, 3);
plot(beat_sweep(1, 1, 1000, [300 300], 44100, 0.02, 'bar'));
subplot(4, 1, 4);
plot(beat_sweep(1, 1, 3000, [800 800], 44100, 0.01, 'bar'));