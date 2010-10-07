X = 2 * exp(j * pi / 4);
z0 = 0.9 * exp(j * pi / 8);
n = [-20:.1:20];
plot(real(X * z0 .^ n), n)
plot(imag(X * z0 .^ n), n)