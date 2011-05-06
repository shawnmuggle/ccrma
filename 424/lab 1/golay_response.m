function [resp] = golay_response (n, a_resp, b_resp)
[a b] = golay(n);
%max_length = max(length(a_resp), length(b_resp));
max_length = 2^(n+2);
size([a_resp; zeros(2^nextpow2(max_length) - length(a_resp), 1)])

padded_a_resp = [a_resp; zeros(2^nextpow2(max_length) - length(a_resp), 1)];
padded_b_resp = [b_resp; zeros(2^nextpow2(max_length) - length(b_resp), 1)];
resp = fftfilt(flipud(a'), padded_a_resp') + fftfilt(flipud(b'), padded_b_resp');
resp = resp / (2.0 * 2^n);
resp = resp(2^n:end);