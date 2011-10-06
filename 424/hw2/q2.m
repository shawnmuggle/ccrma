fs = 44100;

% q2a

filename = 'bp_sp_3.wav';
s = wavread(filename);
s_channel_1 = s(:,1);
s_channel_1 = s_channel_1(39810:end);

figure(1);
s_channel_1_short = s_channel_1(1:0.3 * fs);
[stft, f, t] = spectrogram(s_channel_1_short, 256, 32);
imagesc(linspace(1, fs * 0.3, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
line(.12*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.14*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.175*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.205*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.265*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.29*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
title('First 300ms of bp sp 3');
xlabel('Time (s)');
ylabel('Frequency (Hz)');

figure(2);
s_channel_1_long = s_channel_1(1:2.5 * fs);
[stft, f, t] = spectrogram(s_channel_1_long, 256, 32);
imagesc(linspace(1, fs * 2.5, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
title('First 2.5s of bp sp 3');
xlabel('Time (s)');
ylabel('Frequency (Hz)');


filename = 'bp_sp_5.wav';
s = wavread(filename);
s_channel_1 = s(:,1);
s_channel_1 = s_channel_1(9800:end);

figure(3);
s_channel_1_short = s_channel_1(1:0.3 * fs);
[stft, f, t] = spectrogram(s_channel_1_short, 256, 32);
imagesc(linspace(1, fs * 0.3, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
line(.11*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.16*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.21*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.24*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.258*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.268*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.278*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(.295*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
title('First 300ms of bp sp 5');
xlabel('Time (s)');
ylabel('Frequency (Hz)');

figure(4);
s_channel_1_long = s_channel_1(1:2.5 * fs);
[stft, f, t] = spectrogram(s_channel_1_long, 256, 32);
imagesc(linspace(1, fs * 2.5, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
title('First 2.5s of bp sp 5');
xlabel('Time (s)');
ylabel('Frequency (Hz)');


% q2b

freq = 125;
max_freq = 8000;
bs = zeros(7, 5);
as = zeros(7, 5);
i = 2;
H = [];
normalized_freq = freq / 22050;
[lpb, lpa] = butter(4, sqrt(normalized_freq * (normalized_freq / 2)), 'low');
[H(1,:), w] = freqz(lpb, lpa, 512);
while (freq <= max_freq)
    normalized_freq = freq / 22050;
    low = sqrt(normalized_freq * (normalized_freq / 2));
    high = sqrt(normalized_freq * (normalized_freq * 2));
    [b, a] = butter(2, [low high]);
    bs(i,:) = b;
    as(i,:) = a;
    freq = freq * 2;
    i = i + 1;
    [H(i,:), w] = freqz(b, a, 512);
end
[hpb, hpa] = butter(4, sqrt(normalized_freq * (normalized_freq * 2)), 'high');
[H(i+1,:), w] = freqz(hpb, hpa, 512);

figure(5)
semilogx(w, db(abs(H)));
hold on
semilogx(w, db(sum(abs(H) .^ 2 )));
hold off
axis([10^-3 10^1 -200 10]);
title('Filter bank amplitudes + sum of energy');
xlabel('Frequency (rad)');
ylabel('Amplitude (db)');


% q2c

freq = 125;
max_freq = 8000;
bs = zeros(7, 5);
as = zeros(7, 5);
i = 1;
H = [];
normalized_freq = freq / 22050;
while (freq <= max_freq)
    normalized_freq = freq / 22050;
    low = sqrt(normalized_freq * (normalized_freq / 2));
    high = sqrt(normalized_freq * (normalized_freq * 2));
    [b, a] = butter(2, [low high]);
    bs(i,:) = b;
    as(i,:) = a;
    freq = freq * 2;
    i = i + 1;
    [H(i,:), w] = freqz(b, a, 512);
end


filename = 'bp_sp_3.wav';
s = wavread(filename);
s_channel_1 = s(:,1);
s_channel_1 = s_channel_1(39810:end);

t60s = zeros(1, 7);
for i = [1:7]
    s = filtfilt(bs(i,:), as(i,:), s_channel_1);
    
    N = round(100 * fs / 1000);
    w = hanning(N) / sum(hanning(N));
    avgs = sqrt(fftfilt(w, s.^2));
    
    figure(i + 5)
    plot(db(avgs));
    low = 10000;
    high = 43000;
    p = polyfit([low:high], db(avgs(low:high))', 1);
    hold on
    plot([low:high], p(1)*[low:high] + p(2), 'r');
    hold off
    title(sprintf('bp sp 3 filtered by bandpass %d', i));
    xlabel('Time (samp)');
    ylabel('Amplitude (dB)');
    
    
    t60s(i) = (-60 / p(1)) * (1 / fs);
end
t60s

filename = 'bp_sp_5.wav';
s = wavread(filename);
s_channel_1 = s(:,1);
s_channel_1 = s_channel_1(9800:end);

t60s = zeros(1, 7);
for i = [1:7]
    s = filtfilt(bs(i,:), as(i,:), s_channel_1);
    
    N = round(100 * fs / 1000);
    w = hanning(N) / sum(hanning(N));
    avgs = sqrt(fftfilt(w, s.^2));
    
    figure(i + 5 + 7)
    plot(db(avgs));
    low = 10000;
    high = 43000;
    p = polyfit([low:high], db(avgs(low:high))', 1);
    hold on
    plot([low:high], p(1)*[low:high] + p(2), 'r');
    hold off
    title(sprintf('bp sp 5 filtered by bandpass %d', i));
    xlabel('Time (samp)');
    ylabel('Amplitude (dB)');
    
    t60s(i) = (-60 / p(1)) * (1 / fs);
end

t60s


% q2d

% Band frequency : SNR
% 125 Hz : 20 dB
% 250 Hz : 30 dB
% 500 Hz : 37 dB
% 1000 Hz : 44 dB
% 2000 Hz : 50 dB
% 4000 Hz : 51 dB
% 8000 Hz : 52 dB