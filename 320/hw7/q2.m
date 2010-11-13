% Mike Rotondo / Lab 7-Question 2

n = [1:60];
x1 = cos(0.25 * pi * n);
x1 = [x1 zeros(1,40)];
b = [2 0 -1];
a = [1];
y1 = filter(b, a, x1);
x2 = circshift(x1, [0 3]);
y2 = filter(b, a, x2);
hold on;
plot(y1, 'r');
plot(y2, 'b');

% e: You would have to shift y1 by 3 to get y2, because the filter is
% time-invariant