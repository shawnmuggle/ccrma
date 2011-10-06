fs = 44100;

s = wavread('ss_src.wav');
r = wavread('ss_sp_1.wav');
r = r(:,1);

ir = real(ifft(fft(r(1:length(s)), 2^22)./fft(s, 2^22)));
ir = ir / max(ir);

% Sweep response
subplot(3,1,1);
[stft, f, t] = spectrogram(r(1:fs * 50), 256, 32);
imagesc(linspace(1, fs * 50, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
title('Sine sweep signal');
xlabel('Time (s)');
ylabel('Frequency (Hz)');

% Impulse response
subplot(3,1,2);
[stft, f, t] = spectrogram(ir(1:fs * 5), 256, 32);
imagesc(linspace(1, fs * 5, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
title('First 5 seconds of impulse response');
xlabel('Time (s)');
ylabel('Frequency (Hz)');

% First 250ms of impulse response
subplot(3,1,3);
[stft, f, t] = spectrogram(ir(1:fs * 0.25), 256);
imagesc(linspace(1, fs * 0.25, length(t)) / fs, (fs / 2) * f / pi, 20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
line(0.032*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(0.0417*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(0.0774*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(0.125*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(0.1845*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
title('First 250 milliseconds of impulse response');
xlabel('Time (s)');
ylabel('Frequency (Hz)');
% Why is the sweep signal zero padded?

% (I discussed this with Chris Carlson) The deconvolving version of the
% sweep signal that is listed in the problem set (c =
% real(ifft(1./fft(s)));) is zero padded so that it can slide over the
% entirety of the sweep response while "deconvolving" the sine signal out
% to get the impulse response.

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

t60s = zeros(1, 7);
for i = [1:7]
    s = filtfilt(bs(i,:), as(i,:), ir);
    
    N = round(100 * fs / 1000);
    w = hanning(N) / sum(hanning(N));
    avgs = sqrt(fftfilt(w, s.^2));
    
    figure(i + 1)
    plot(db(avgs)); % These IRs all have a weird dip in the middle.
    low = 10000;
    high = 43000;
    p = polyfit([low:high], db(avgs(low:high))', 1);
    hold on
    plot([low:high], p(1)*[low:high] + p(2), 'r');
    hold off
    title(sprintf('sine swee response filtered by bandpass %d', i));
    xlabel('Time (samp)');
    ylabel('Amplitude (dB)');
    
    
    t60s(i) = (-60 / p(1)) * (1 / fs);
end
t60s

% Band frequency : SNR
% 125 Hz : 44 dB
% 250 Hz : 51 dB
% 500 Hz : 59 dB
% 1000 Hz : 61 dB
% 2000 Hz : 64 dB
% 4000 Hz : 68 dB
% 8000 Hz : 71 dB

% These SNRs are way better than with balloon pops. The extrapolated T60s
% are similar to those obtained from balloon pops.