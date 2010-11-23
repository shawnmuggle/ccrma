%filter: y(n) = 0.3024 x(n) − 0.3024 x(n − 2) + 1.749 y(n − 1) − 0.9244 y(n − 2)

b = [0.3024 0 -0.3024];
a = [1 -1.749 0.9244];

[x fs nbits opts] = wavread('jobs');
%zero pad x so that we get the "release" part of the filtered output
%sound(x, fs);
x = [x; zeros(length(x), 1)];

for N=[128 1024 8092],

    % difference equation:
    yd = zeros(length(x), 1);
    tic;
    for i=[1:length(x)],
        yd(i) = 0.3024 * x(i);
        if i > 1
            yd(i) = yd(i) + 1.749 * yd(i - 1);
        end
        if i > 2
            yd(i) = yd(i) - 0.3024 * x(i - 2) - 0.9244 * yd(i - 2);
        end
    end
    td = toc;
    disp(sprintf('diff eq took %f seconds for N=%d', td, N));
    %sound(yd, fs);
 
    % convolution with impulse response:
    h = filter(b,a,[1; zeros(N-1,1)]);
    tic;
    yc = conv(h, x);
    tc = toc;
    disp(sprintf('conv took %f seconds for N=%d', tc, N));
    %sound(yc, fs);
    
    % filtering via TDFII form:
    tic;
    yf = filter(b, a, x);
    tf = toc;
    disp(sprintf('filter took %f seconds for N=%d', tf, N));
    %sound(yf, fs);
    
    % filtering using FFTs of length N
    tic;
    yfft = fftfilt(h, x, N);
    tfft = toc;
    disp(sprintf('fftfilt took %f seconds for N=%d', tfft, N));
    %sound(yfft, fs);
end

%diff eq took 0.147026 seconds for N=128
%conv took 0.013168 seconds for N=128
%filter took 0.018941 seconds for N=128
%fftfilt took 4.875833 seconds for N=128

%diff eq took 0.194299 seconds for N=1024
%conv took 0.102301 seconds for N=1024
%filter took 0.025203 seconds for N=1024
%fftfilt took 14.821652 seconds for N=1024

%diff eq took 0.193024 seconds for N=8092
%conv took 0.774891 seconds for N=8092
%filter took 0.025200 seconds for N=8092
%fftfilt took 0.868636 seconds for N=8092