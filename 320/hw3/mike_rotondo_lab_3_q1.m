% Mike Rotondo / Lab3-Q1

fs = 44100;
num_partials = 6; % 6 is about as many partials as you need to get
                  % a reasonable-looking approximation
f_fund = 440;
n = [1:num_partials];
f = f_fund * n;

Z = j ./ n; % pi out of phase
flipper = repmat([1 0], 1, ceil(num_partials / 2));
square = additive(f, flipper(1:num_partials) .* Z, fs, 2, 'square');
subplot(3,1,1);
plot(square(1:ceil(fs/f_fund * 5)));

Z = (1 ./ (2 * (n .^ 2)));
tri = additive(f, flipper(1:num_partials) .* Z, fs, 2, 'triangle');
subplot(3,1,2);
plot(tri(1:ceil(fs/f_fund * 5)));

Z = -j ./ (2 * n); % -pi out of phase, for kicks
saw = additive(f, Z, fs, 2, 'sawtooth');
subplot(3,1,3);
plot(saw(1:ceil(fs/f_fund * 5)));

% square wave has the most high frequency content, so it's the
% harshest, with sawtooth a close second and triangle much less
% harsh. This is due to its inverse-square frequency components.
