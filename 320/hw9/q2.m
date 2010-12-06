cla;

nbins = 4096;
rho = [-0.5 0 0.5];
b = [rho 1];
a = [1 rho];
[h w] = freqz(b, a, 0:1/(nbins-1):pi);
group_delay = -diff(unwrap(angle(h)));

subplot(3, 1, 1);
plot(group_delay);

subplot(3, 1, 2);
group_delay2 = grpdelay(b, a, w);
plot(group_delay2);

% I did need to use unwrap.

subplot(3, 1, 3);
zplane(b, a);