% Mike Rotondo - Lab 2-1

X = 2 * exp(j * pi / 4);
z0 = 0.9 * exp(j * pi / 8);
n = [-100:100];
n_small = [-38:38];
n_realsmall = [-3.8:.1:3.8];
tau = -1 / log(.9);
cla
figure(gcf)
subplot(2, 1, 1)
hold on
plot(n, real(X * z0 .^ n), '')
plot(n, imag(X * z0 .^ n), '')
plot(tau, 0, '--rs', 'MarkerSize', 10)
plot(6.91 * tau, 0, '--rs', 'MarkerSize', 10)
hold off
subplot(2, 1, 2)
hold on
plot(real(X * z0 .^ n_small), imag(X * z0 .^ n_small), '')
plot(tau * exp(j .* n_realsmall), '--rs', 'MarkerSize', 2)
plot(6.91 * tau * exp(j .* n_realsmall), '--rs', 'MarkerSize', 2)
hold off

