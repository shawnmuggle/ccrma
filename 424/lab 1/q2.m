% Get the 5 frequency responses by doing q1
q1
chopped_resps = resps(1:2048,:);

freqs = [440 603 1000 1333 2121];
gammas = [0.14 0.17 0.33 0.44 0.55];
Qs = [7 5 2.5 2.2 3];

for i = [0:4]
    tf = fft(chopped_resps(:,i+1),4096);
    figure(i + 5); semilogx([0:2048]/2048 * fs/2, 20 * log10(abs(tf(1:2049)))); grid;
    hold on
    wah_sim(gammas(i+1), freqs(i+1), Qs(i+1));
    hold off
    xlabel('Frequency (Hz)');
    ylabel('Amplitude (dB)');
    title('Wah positions filter frequency response vs wah approximation');
end