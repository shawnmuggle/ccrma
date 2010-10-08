% Mike Rotondo / Lab 2-3

function y_beat = beat_sweep(A, B, fc, f_delt, fs, dur, name)

% A: amplitude of the center frequency cosine
% B: amplitude of the sweeping frequency cosine
% fc: center frequency in Hz
% f_delt: [f1 f2] vector for frequency sweeping
% fs: sample rate in Hz
% dur: total duration of the signal in seconds
% name: name of the output audio file

sampling_interval = 1 / fs;
t = [0:sampling_interval:dur]';
phases_in_radians = (t * fc) .* 2 * pi;

ramp = t / max(t);
% !!! I had to talk to Prof Abel about a bug we were
% all seeing with this function, where if it swept from e.g. 99 to 101
% across 100 Hz, we saw asymmetric beat patterns. Once I added the
% /2 factor to my frequency, I saw a symmetric beat pattern for
% the same input. The /2 factor is from integrating a frequency
% which includes a t term, as explained to me by Prof Abel.
sweep_phases_in_radians = (t .* (fc + f_delt(1) + ramp * diff(f_delt/2))) .* 2 * pi;

signals = [(A * cos(phases_in_radians)) (B * cos(sweep_phases_in_radians))];
signal = sum(signals, 2);
y_beat = signal;

% stolen straight from my additive.m
% scale the signal down so we don't clip, but if it's quiet (< 1)
% already, leave it alone.
scaling_factor = max(1, abs(max(abs(min(signal)), max(signal))));
% my signal seemed to clip slightly even with amplitudes of 1, so I
%   bring it down slightly with a .9
scaled_signal = .9 .* (signal / scaling_factor);

%plot(t, scaled_signal)
%sound(scaled_signal, fs)
wavwrite(scaled_signal, fs, name)

% see plot_chirps.m for part b plots

% fc = 100Hz
% a: f2 = 50ish
% fc = 300Hz
% a: f2 = 120ish
% fc = 1000Hz
% a: f2 = 400
% fc = 3000Hz
% a: f2 = 300ish???