s = wavread('test01_44100.wav');
length_sec = length(s) / 44100.0;
t = linspace(0, length_sec, length(s));
figure(1);
plot(t, s);
axis([0 5 -1 1]);

s = wavread('compressor test.wav');
length_sec = length(s) / 44100.0;
t = linspace(0, length_sec, length(s));
figure(2);
plot(t, s);
axis([0 5 -1 1]);

% The attack in the compressed version starts at 1 sec and lasts until
% approximately 1.06 seconds (attack time constant set to 10ms). The
% release starts at 3 sec and lasts until approximately 3.6 seconds
% (release time constant set to 100ms). These match up with the attack and
% release times shown in the graph in the reader which uses the same time
% constants.

% q2c
% The different attacks were noticeably different, and I could definitely
% hear the thuddiness of the longer attacks (~10ms). However, the release
% changes were much more noticeable: the 100ms range of release times made
% drums sound like they had reverb on them, and coupled with a 10ms attack,
% they sounded really up front and poppy. When I turned down the release
% all the way, and turned up the input gain and lowered the threshold, I
% got an incredible (but pretty bad) popping sound out of the drums.

s = wavread('rms compressor values.wav');
length_sec = length(s) / 44100.0;
t = linspace(0, length_sec, length(s));
figure(3);
plot(t, s);
axis([0 5 -1 1]);

s = wavread('rms compressor output.wav');
length_sec = length(s) / 44100.0;
t = linspace(0, length_sec, length(s));
figure(4);
plot(t, s);
axis([0 5 -1 1]);