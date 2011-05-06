% 1.a)
% These codes are 65536 samples long
[a b] = golay(16);
figure(1);
subplot(2, 1, 1);
plot(a(length(a) - 100:end));
xlabel('Time (samples)');
ylabel('Amplitude');
title('Golay code A excerpt, last 100 samples');
subplot(2, 1, 2);
plot(b(length(b) - 100:end));
xlabel('Time (samples)');
ylabel('Amplitude');
title('Golay code B exerpt, last 100 samples');

fs = 44100;
specs = [];
resps = [];
for index = [0:4]
    a_resp = wavread(sprintf('a_pos%d.wav', index));
    b_resp = wavread(sprintf('b_pos%d.wav', index));
    resp = golay_response(16, a_resp, b_resp);
    resps = [resps resp'];
    
    % 1.b)
    figure(2);
    subplot(5, 1, index + 1);
    spectrogram(resp);
    
    % 1.c)
    % The first plot line will give linear responses, the second will give
    % log magnitude. Each show only the first 1000 samples. The log
    % magnitude plots helped me see clearly where the impulse responses go
    % into the noise floor.
    figure(3);
    subplot(5, 1, index + 1);
    plot(resp(1:1000) + 0.2 * index);
    plot(20 * log10(abs(resp(1:1000))));

end

% 1.d)
% Jonathan suggested I window the impulse responses before ffting them so
% as to avoid feeding long periods of somewhat noisy silence to the fft, so
% that's what I do here. It cleans up the frequency responses considerably.
tf = fft(resps(1:2048,:),4096);
figure(4); semilogx([0:2048]/2048 * fs/2, 20 * log10(abs(tf(1:2049,:)))); grid;
xlabel('Frequency (Hz)');
ylabel('Amplitude (dB)');
title('Wah positions filter frequency responses');