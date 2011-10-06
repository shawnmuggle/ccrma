fs = 44100;
s = wavread('aliased_sweep.wav');
s1 = s(:,1);

figure(1);
[stft, f, t] = spectrogram(s1, 256, 32);
imagesc(linspace(1, length(s1) / fs, length(t)), (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
title('Spectrogram of aliased sweep');
xlabel('Time (s)');
ylabel('Frequency (Hz)');

% There are aliased frequencies present throughout this recording which
% represent the shifted replicas of the transform in frequency space. They
% are audible as fast up/down sweeps playing alongside the original sweep.


figure(2);
[b, a] = cheby2(12, 70, (1/8.0) * (2 - 18 / 22));
freqz(b, a, 256);
sos = tf2sos(b, a);


% SOFT CLIPPING
s = wavread('softclip_unaliased_sweep.wav');
s1 = s(:,1);

figure(3);
[stft, f, t] = spectrogram(s1, 256, 32);
imagesc(linspace(1, length(s1) / fs, length(t)), (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
title('Spectrogram of soft-clipped unaliased sweep');
xlabel('Time (s)');
ylabel('Frequency (Hz)');


% HARD CLIPPING
s = wavread('hardclip_unaliased_sweep.wav');
s1 = s(:,1);

figure(4);
[stft, f, t] = spectrogram(s1, 256, 32);
imagesc(linspace(1, length(s1) / fs, length(t)), (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
title('Spectrogram of hard-clipped unaliased sweep');
xlabel('Time (s)');
ylabel('Frequency (Hz)');

% Hard clipping basically seems to make the harmonics and aliasing more
% loud/present, though I still couldn't hear the aliasing. The harmonics
% added a bit of fuzziness at the beginning in both cases.