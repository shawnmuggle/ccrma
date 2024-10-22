function [s_channel_1] = q2a (filename)

if nargin == 0
    filename = 'bp_sp_3.wav';
end

s = wavread(filename);

fs = 44100;
s_channel_1 = s(:,1);

s_channel_1 = s_channel_1(39810:end);

figure(1);
s_channel_1_short = s_channel_1(1:0.3 * fs);
stft = spectrogram(s_channel_1_short, 256, 32);
imagesc(20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
line(23*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(28*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(34*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(41*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(51*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);
line(57*[1 1], get(gca,'YLim'), 'Color', 'w', 'LineStyle', ':', 'LineWidth', 2);

figure(2);
s_channel_1_long = s_channel_1(1:2.5 * fs);
stft = spectrogram(s_channel_1_long, 256, 32);
imagesc(20*log10(abs(stft)/max(max(abs(stft)))), [-80 0]); grid; axis('xy'); colorbar; colormap(jet);
