% Mike Rotondo
% 424 HW 1
% Q2

% 2(a).
% See golay.m

% 2(b).
clf;
subplot(3,1,1);
plot(golay_response(0));
title('Length 1 Golay Code Impulse Response');
xlabel('Time (samples)');
ylabel('Amplitude');
axis([0 1024 -1 1]);

subplot(3,1,2);
plot(golay_response(5));
title('Length 32 Golay Code Impulse Response');
xlabel('Time (samples)');
ylabel('Amplitude');
axis([0 1024 -1 1]);

subplot(3,1,3);
plot(golay_response(10));
title('Length 1024 Golay Code Impulse Response');
xlabel('Time (samples)');
ylabel('Amplitude');
axis([0 1024 -1 1]);

% Saved as golay_impulse_responses.jpg

% 2(c).
resp1 = golay_response(0);
noise1 = resp1(1:120);
rms1 = sqrt(mean(noise1 .^ 2));
log_rms1 = 20 * log10(rms1)

resp2 = golay_response(5);
noise2 = resp2(1:120);
rms2 = sqrt(mean(noise2 .^ 2));
log_rms2 = 20 * log10(rms2)

resp3 = golay_response(10);
noise3 = resp3(1:120);
rms3 = sqrt(mean(noise3 .^ 2));
log_rms3 = 20 * log10(rms3)

% The respective SNRs for length 1, 32 and 1024 Golay codes are 
% approximately -42, -57, and -72 dB. These differ by about -15 dB.