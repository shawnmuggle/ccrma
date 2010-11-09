%% Clear all the data
clear all; close all; clc;

% Load the data
load data
 
% Plot
plot(violinist(1,:), violinist(2,:), 'x', 'MarkerSize', 14)
grid on; hold on;; axis([-.5 1.5 -.5 1.5])
text(violinist(1,1), violinist(2,1), '  v  1'); 
text(violinist(1,2), violinist(2,2), '  v  2');
text(violinist(1,3), violinist(2,3), '  v  3');

plot(mic(1,:), mic(2,:), 'ro', 'MarkerSize', 14)
legend('violinist', 'Mic')
text(mic(1,1), mic(2,1), '  mic  1'); 
text(mic(1,2), mic(2,2), '  mic  2');
text(mic(1,3), mic(2,3), '  mic  3');
title('Recording Configuration')
xlabel('X dimension ')
ylabel('Y dimension ')

%% Your Code here

hold on
for i=1:3,
    for j=1:3,
        %subplot(3, 3, (i - 1) * 3 + j);
        xc = xcorr(cleanSignals(:,j), mixedSignals(:,i));
        %plot(xc);
        [y, idx] = max(xc);
        disp(sprintf('index of max in xcorr for mixed signal %d and clean signal %d is %d', j, i, idx));
    end
end

% output:
% index of max in xcorr for mixed signal 1 and clean signal 1 is 132197
% index of max in xcorr for mixed signal 2 and clean signal 1 is 132254
% index of max in xcorr for mixed signal 3 and clean signal 1 is 132197
% index of max in xcorr for mixed signal 1 and clean signal 2 is 132203
% index of max in xcorr for mixed signal 2 and clean signal 2 is 132267
% index of max in xcorr for mixed signal 3 and clean signal 2 is 132167
% index of max in xcorr for mixed signal 1 and clean signal 3 is 132167
% index of max in xcorr for mixed signal 2 and clean signal 3 is 132267
% index of max in xcorr for mixed signal 3 and clean signal 3 is 132203
%
% In this case, the lag at which the cross-correlation of a clean signal
% and a mixed signal is maximized denotes the time it took for the sound
% from the violinist playing that clean signal to get to that microphone.
%
% mixed signal 1 receives clean signal 3 first, then clean signal 1, then
% clean signal 2
%
% mixed signal 2 receives clean signal 1, then clean signal 2 and 3
% simultaneously
%
% mixed signal 3 receives clean signal 2 first, then clean signal 1, then
% clean signal 3.

% We can therefore determine that mixed signal 1 comes from the microphone
% closest to the violinist playing clean signal 3, mixed signal 2 comes
% from the the middle microphone (closest to the middle violinist, who is 
% playing clean signal 1), and mixed signal 3 comes from the microphone
% closest to the violinist playing clean signal 2. Because the layout is
% symmetric, I can't say which of the outer violinists was playing clean
% signals 2 or 3.