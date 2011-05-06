% 1.a.1
counter = 1;
figure(1);
hold on
for i = [-4:0.5:2]
    if i == 0
        continue
    end
    wc = 2 * pi * 1000;
    num = [ 1 ];
    den = [ 1/wc^2 1/(wc * i) 1];
    sys = tf(num, den);
    bode(sys);
    title(sprintf('wc = 1000Hz, Q = %f', i));
end
hold off

figure(2);
hold on
for i = [-4:0.5:2]
    if i == 0
        continue
    end
    wc = 2 * pi * 1000;
    num = [ 1 ];
    den = [ 1/wc^2 1/(wc * i) 1];
    sys = tf(num, den);
    rlocus(sys);
    title(sprintf('wc = 1000Hz, Q = %f', i));
end
hold off

% 1.a.2
figure(3);
hold on
for i = [-2:0.5:2]
    Q = 2;
    wc = 1000 * 2^i;
    num = [ 1 ];
    den = [ 1/wc^2 1/(wc * Q) 1];
    sys = tf(num, den);
    bode(sys);
    title(sprintf('wc = %fHz, Q = 2', 1000 * 2^i));
end
hold off

figure(4);
hold on
for i = [-2:0.5:2]
    Q = 2;
    wc = 1000 * 2^i;
    num = [ 1 ];
    den = [ 1/wc^2 1/(wc * Q) 1];
    sys = tf(num, den);
    rlocus(sys);
    title(sprintf('wc = %fHz, Q = 2', 1000 * 2^i));
end
hold off