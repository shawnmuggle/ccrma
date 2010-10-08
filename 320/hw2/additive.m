% Mike Rotondo - Lab 2-2

function y = additive(f, Z, fs, dur, name)

% f: vector of freqs in Hz
% Z: vector of complex amplitudes A * exp(j * phi)
% fs: sampling rate in Hz
% dur: total duration of the signal in seconds
% name: name of the output audio file
% f and Z must be of the same length

assert(length(f) == length(Z), 'Got different length vectors of freqs and amps')

sampling_interval = 1 / fs;
t = [0:sampling_interval:dur];
phases_in_radians = (t' * f) .* (2 * pi) - (ones(length(t), 1) * angle(Z));
signals = ones(length(t), 1) * abs(Z) .* cos(phases_in_radians);
signal = sum(signals, 2);
y = signal

% scale the signal down so we don't clip, but if it's quiet (< 1)
% already, leave it alone.
scaling_factor = max(1, abs(max(abs(min(signal)), max(signal))))
% my signal seemed to clip slightly even with amplitudes of 1, so I
%   bring it down slightly with a .9
scaled_signal = .9 .* (signal / scaling_factor);

%plot(t, scaled_signal)
%sound(scaled_signal, fs)
wavwrite(scaled_signal, fs, name)
