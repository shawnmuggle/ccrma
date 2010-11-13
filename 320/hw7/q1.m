% Mike Rotondo / Lab 7-Question 1

n = [1:60];
x1 = 0.5 * cos(0.2 * pi * n);
x1 = [x1 zeros(1,40)];
b = [1 1];
a = [1];
y1 = filter(b, a, x1);
subplot(3, 1, 1);
plot(x1);
subplot(3, 1, 2);
plot(y1);

% e: acyclic
% f: no, we would've lost the last element of y1

x2 = 2 * cos(0.1 * pi * n);
x2 = [x2 zeros(1,40)];
y2 = filter(b, a, x2);
x3 = 0.5 * x1 + 2 * x2;
x3 = [x3 zeros(1,40)];
y3 = filter(b, a, x3);
subplot(3, 1, 3);
hold on;
plot(0.5 * y1, 'r');
plot(2 * y2, 'g');
plot(y3, 'b');