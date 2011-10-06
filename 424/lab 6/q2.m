% This is the output signal when the compressor gets a linear ramp as its
% input signal
s = wavread('mystery sweep response.wav');
s = s(:, 1);
s = s(length(s) / 2:end);
length_sec = length(s) / 44100.0;
t = linspace(0, 1, length(s));
figure(1);
plot(t, s);

% 2a

% By observation: Threshold = .11 (-19.1721 dB)

% linear gain at input linear gain 0.8: .29
% linear gain at input linear gain 0.8978: 0.3072
% 20 * log10 (0.3072 / .29) =  0.5
% 1/rho = 0.5, rho = 2

% Ratio = 2

% 2b/2e

% ---
% Feedforward pseudocode: 

% globals lambda_t = 0, gamma_t = 1, linear_threshold = 0.11, ratio = 2
% for each sample s[i] in signal vector s:
% lambda_t = detect_level(s[i]);
% gamma_t = compute_gain(lambda_t);
% output[i] = modify_signal(s[i], gamma_t);

% detect_level(signal_element):
%   leaky_integrate(signal_element, lambda_t, time_constant)

% compute_gain(detected_linear_level):
%   if detected_linear_level > linear_threshold:
%     return gain which will reduce input level by ratio

% leaky_integrate(input, filtstate, time_constant)
%   amount = exp(-1.0 / time_constant);
%   filtstate = amount * filtstate + (1 - amount) * input;

% ---
% Feedback pseudocode:

% for each sample s[i] in signal vector s:
% output[i] = modify_signal(s[i], gamma_t);
% lambda_t = detect_level(output[i]);
% gamma_t = compute_gain(lambda_t);


% 2c

% This is the output signal when the compressor gets alternating 1s and 0s
% as its input signal
s = wavread('impulses response.wav');
s = s(:, 1);
length_sec = length(s) / 44100.0;
t = linspace(0, 1, length(s));
figure(2);
plot(t, s);
% output signal has amplitude of .3285

% This is the output signal when the compressor gets all 1s as its input
% signal
s = wavread('impulses response 2.wav');
s = s(:, 1);
length_sec = length(s) / 44100.0;
t = linspace(0, 1, length(s));
figure(3);
plot(t, s);
% output signal has amplitude of .3225

% I think these are close enough to be within a margin of error of equal,
% and so the compressor must use peak detection, because the signal with
% alternating values has the same detected level as the constant signal.


% 2d

% This is the output when the compressor gets a second of zeros, then a
% second of ones, then a second of zeros
s = wavread('attack release response.wav');
s = s(:, 1);
length_sec = length(s) / 44100.0;
t = linspace(0, 3, length(s));
figure(4);
plot(t, s);

% Attack time constant: 50 ms
% Release time constant: 0ms


% 2e
