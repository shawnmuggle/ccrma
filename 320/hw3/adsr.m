function [env, envlog] = adsr (amps, durs, fs)
% function env = adsr (amps, durs, fs)
% amps: vector of amplitude values at each transition point [0~1] (1x4)
% durs: vector of durations of each period [s] (1x4)
% fs: sampling rate in Hz
% Mike Rotondo / Lab3-Q2

durs_samps = durs * fs;
env = [linspace(0, amps(1), durs_samps(1)) ...
       linspace(amps(1), amps(2), durs_samps(2)) ...
       linspace(amps(2), amps(3), durs_samps(3)) ...
       linspace(amps(3), amps(4), durs_samps(4))];

% I talked with Jonathan and he suggested that I make a log
% envelope to see the difference between the two. I also plotted
% both of them on a decibel scale to see how we would perceive
% their relative loudnesses
logamps = log10(max(amps, 0.001));
envlog = [logspace(log10(0.001), logamps(1), durs_samps(1)) ...
	  logspace(logamps(1), logamps(2), durs_samps(2)) ...
	  logspace(logamps(2), logamps(3), durs_samps(3)) ...
	  logspace(logamps(3), logamps(4), durs_samps(4))];
