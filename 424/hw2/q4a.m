fs = 44100;
ir = wavread('impulseresponse.wav');
ir_channel_1 = ir(:,1);
[stft, f, t] = spectrogram(ir_channel_1(1:fs * 3), 256, 32);
figure(1);
imagesc(linspace(1, fs * 3, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft))))); grid; axis('xy'); colorbar; colormap(jet);

ir = wavread('impulseresponsehigh.wav');
ir_channel_1 = ir(:,1);
[stft, f, t] = spectrogram(ir_channel_1(1:fs * 3), 256, 32);
figure(2);
imagesc(linspace(1, fs * 3, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft))))); grid; axis('xy'); colorbar; colormap(jet);
