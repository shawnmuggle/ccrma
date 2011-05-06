% Mike Rotondo
% 424 HW 1
% Q3

% 3(a).
% See allpasses.m
clf;
allpassed_impulse = allpasses(64, 0.5);
plot(allpassed_impulse);
title('Impulse response of 64 cascaded allpass filters');
xlabel('Time (samples)');
ylabel('Amplitude');

% Saved as allpass_impulse_response.jpg

% 3(b).
% See min_impulse_response.m
rho = min_impulse_response()
% The optimal rho is approximately 0.2842

% 3(c).
% See allpass_measurement.m
allpass_measurement();

% 3(d).
% See allpass_snr_compare.m
allpass_snr_compare()

% The SNR of a unit pulse is -40 dB, while the Allpass filtered impulse
% gives an SNR of approximately -52 dB. This is a difference of -12 dB. In
% terms of the Golay code SNRs, the allpass is better than the 1-length
% code, but worse than the 1024-length and even the 32-length.