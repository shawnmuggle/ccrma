f = 125;
max_f = 8000;
bs = zeros(7, 5);
as = zeros(7, 5);
i = 1;
H = [];
while (f <= max_f)
    normalized_f = f / 22050;
    low = sqrt(normalized_f * (normalized_f / 2));
    high = sqrt(normalized_f * (normalized_f * 2));
    [b, a] = butter(2, [low high]);
    bs(i,:) = b;
    as(i,:) = a;
    f = f * 2;
    i = i + 1;
    [H(i,:), w] = freqz(b, a, 512);
end

figure(3)
semilogx(w, db(abs(H)));
hold on
semilogx(w, db(sum(abs(H))));
hold off
axis([10^-3 10^1 -200 10]);
