function q2c

findT60s('bp_sp_3.wav')
%pause
%findT60s('bp_sp_4.wav')

function t60s = findT60s (filename)
fs = 44100;
s_channel_1 = q2a(filename);
q2b
t60s = zeros(1, 7);
for i = [1:7]
    s = filtfilt(bs(i,:), as(i,:), s_channel_1);
    
%    s = s / max(s);
    
    N = round(100 * fs / 1000);
    w = hanning(N) / sum(hanning(N));
    avgs = sqrt(fftfilt(w, s.^2));
    
    figure(i)
    plot(db(avgs));
    low = 10000;
    high = 43000;
    p = polyfit([low:high], db(avgs(low:high))', 1);
    hold on
    plot([low:high], p(1)*[low:high] + p(2), 'r');
    hold off
    
    t60s(i) = (-60 / p(1)) * (1 / fs);
end

