% Mike Rotondo HW 8 - Lab question 1

B = fir1(10, 0.5, 'low');
A = [1];
d = [1 zeros(1, 100)];
subplot(5, 1, 1);
plot(filter(B, A, d));
title('Impulse Response');
xlabel('Time (samples)');
ylabel('Amplitude');
axis([0 100 -0.5 0.5]);

n = [0:0.01:pi];
subplot(5, 1, 2);
plot(n, abs(freqz(B, A, length(n))));
title('Amplitude Response');
xlabel('Frequency (radians/sample)');
ylabel('Gain');
axis([0 pi 0 1]);

subplot(5, 1, 3);
plot(n, angle(freqz(B, A, length(n))));
title('Phase Response');
xlabel('Frequency (radians/sample)');
ylabel('Phase Shift');
axis([0 pi -5 5]);

subplot(5, 1, 4);
n = [-pi:2*pi/8192:pi];
n = n(1:8192);
size(n)
x = [randn(1, 4096) zeros(1, 4096)];
xar = abs(fft(x));
plot(n, fftshift(xar));
title('Unfiltered FFT');
xlabel('Frequency (radians/sample)');
ylabel('Magnitude');
axis([-pi pi 0 200]);

subplot(5, 1, 5);
y = filter(B, A, x);
yar = abs(fft(y));
plot(n, fftshift(yar));
title('Filtered FFT');
xlabel('Frequency (radians/sample)');
ylabel('Magnitude');
axis([-pi pi 0 200]);