function [response] = allpasses (n, rho, signal)
B = [rho 1];
A = [1 rho];
if nargin == 2
    signal = [1 zeros(1, 250)];
end
response = signal;
while (n > 0)
    response = filter(B, A, response);
    n = n - 1;
end
