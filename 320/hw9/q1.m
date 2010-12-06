cla;

[b_butt, a_butt] = butter(2, 1/3/2);
% coefficients:
% b = 0.0495    0.0990    0.0495
% a = 1.0000   -1.2796    0.4776

[b_cheb, a_cheb] = cheby1(2, 0.5, 1/3/2);
% coefficients
% b =  0.0689    0.1379    0.0689
% a = 1.0000   -1.1955    0.4875

[h_butt w_butt] = freqz(b_butt, a_butt, 4096);
[h_cheb w_cheb] = freqz(b_cheb, a_cheb, 4096);
subplot(2, 3, 1);
hold on
plot([0:1/4095:1], abs(h_butt));
plot([0:1/4095:1], abs(h_cheb), 'r');
title('Magnitude Responses (cutoff = fs/3)');
xlabel('Normalized Frequency (1.0 = fs/2)');
ylabel('Magnitude');
subplot(2, 3, 4);
hold on
plot([0:1/4095:1], w_butt);
plot([0:1/4095:1], w_cheb, 'r');
title('Phase Responses (cutoff = fs/3)');
xlabel('Normalized Frequency (1.0 = fs/2)');
ylabel('Phase Offset (Radians)');

% The chebyshev filter has more ripple (in the magnitude response) but a
% faster falloff in both the magnitude and phase responses.



[b_butt, a_butt] = butter(2, 1/6/2);
% coefficients:
% b = 0.0144    0.0288    0.0144
% a = 1.0000   -1.6330    0.6906

[b_cheb, a_cheb] = cheby1(2, 0.5, 1/6/2);
% coefficients
% b = 0.0204    0.0409    0.0204
% a = 1.0000   -1.6042    0.6908

[h_butt w_butt] = freqz(b_butt, a_butt, 4096);
[h_cheb w_cheb] = freqz(b_cheb, a_cheb, 4096);
subplot(2, 3, 2);
hold on
plot([0:1/4095:1], abs(h_butt));
plot([0:1/4095:1], abs(h_cheb), 'r');
title('Magnitude Responses (cutoff = fs/6)');
xlabel('Normalized Frequency (1.0 = fs/2)');
ylabel('Magnitude');
subplot(2, 3, 5);
hold on
plot([0:1/4095:1], w_butt);
plot([0:1/4095:1], w_cheb, 'r');
title('Phase Responses (cutoff = fs/6)');
xlabel('Normalized Frequency (1.0 = fs/2)');
ylabel('Phase Offset');


[b_butt_band, a_butt_band] = butter(2, [1/6/2 1/3/2]);
% b = 0.0144         0   -0.0288         0    0.0144
% a = 1.0000   -3.3854    4.5190   -2.8088    0.6906

[b_cheb_band, a_cheb_band] = cheby1(2, 0.5, [1/6/2 1/3/2]);
% b_cheb_band = 0.0204         0   -0.0409         0    0.0204
% a_cheb_band = 1.0000   -3.3586    4.4654   -2.7823    0.6908

[h_butt_band w_butt_band] = freqz(b_butt_band, a_butt_band, 4096);
[h_cheb_band w_cheb_band] = freqz(b_cheb_band, a_cheb_band, 4096);
subplot(2, 3, 3);
hold on;
plot([0:1/4095:1], abs(h_butt_band));
plot([0:1/4095:1], abs(h_cheb_band), 'r');
title('Magnitude Responses (cutoffs = fs/6, fs/3)');
xlabel('Normalized Frequency (1.0 = fs/2)');
ylabel('Magnitude');
subplot(2, 3, 6);
hold on;
plot([0:1/4095:1], w_butt_band);
plot([0:1/4095:1], w_cheb_band, 'r');
title('Phase Responses (cutoffs = fs/6, fs/3)');
xlabel('Normalized Frequency (1.0 = fs/2)');
ylabel('Phase Offset');
