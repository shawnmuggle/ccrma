% 1a

width = 25;
length = 40;
height = 10;

volume = width * length * height;

plywood_area = width * length + width * length * 0.5 + .1 * (length * height * 2 + width * height * 2);
glass_area = .1 * (length * height * 2 + width * height * 2);
marble_area = .8 * (length * height * 2 + width * height * 2) + width * length * 0.5;

total_area = plywood_area + glass_area + marble_area;

% rows: 250Hz, 1000Hz, 4000Hz
% columns: plywood, glass, marble
absorption_rates = [.22 .25 .01;
                    .22 .12 .01;
                    .11 .04 .02;];

effective_areas_250 = [plywood_area glass_area marble_area] .* absorption_rates(1,:);
effective_areas_1000 = [plywood_area glass_area marble_area] .* absorption_rates(2,:);
effective_areas_4000 = [plywood_area glass_area marble_area] .* absorption_rates(3,:);

gc = (-2 * log(.001)) / .161;

t60_250_s = -2 * log(.001) * volume / (gc * sum(effective_areas_250));
t60_1000_s = -2 * log(.001) * volume / (gc * sum(effective_areas_1000));
t60_4000_s = -2 * log(.001) * volume / (gc * sum(effective_areas_4000));

t60_250_a = 110; % This seems so wrong!
t60_1000_a = 30;
t60_4000_a = 7;

t60_250 = (1/t60_250_s + 1/t60_250_s)^-1
t60_1000 = (1/t60_1000_s + 1/t60_1000_s)^-1
t60_4000 = (1/t60_4000_s + 1/t60_4000_s)^-1


% 1b

width = width / 2;
length = length / 2;
height = height / 2;

volume = width * length * height;

plywood_area = width * length + width * length * 0.5 + .1 * (length * height * 2 + width * height * 2);
glass_area = .1 * (length * height * 2 + width * height * 2);
marble_area = .8 * (length * height * 2 + width * height * 2) + width * length * 0.5;

total_area = plywood_area + glass_area + marble_area;

% rows: 250Hz, 1000Hz, 4000Hz
% columns: plywood, glass, marble
absorption_rates = [.22 .25 .01;
                    .22 .12 .01;
                    .11 .04 .02;];

effective_areas_250 = [plywood_area glass_area marble_area] .* absorption_rates(1,:);
effective_areas_1000 = [plywood_area glass_area marble_area] .* absorption_rates(2,:);
effective_areas_4000 = [plywood_area glass_area marble_area] .* absorption_rates(3,:);

gc = (-2 * log(.001)) / .161;

t60_250_s = -2 * log(.001) * volume / (gc * sum(effective_areas_250));
t60_1000_s = -2 * log(.001) * volume / (gc * sum(effective_areas_1000));
t60_4000_s = -2 * log(.001) * volume / (gc * sum(effective_areas_4000));

t60_250_a = 110; % This seems so wrong!
t60_1000_a = 30;
t60_4000_a = 7;

t60_250 = (1/t60_250_s + 1/t60_250_s)^-1
t60_1000 = (1/t60_1000_s + 1/t60_1000_s)^-1
t60_4000 = (1/t60_4000_s + 1/t60_4000_s)^-1

% surface areas halfved => T60s are halved!!


% 1c

width = 25;
length = 40;
height = 10;

volume = width * length * height;

plywood_area = width * length + .1 * (length * height * 2 + width * height * 2);
glass_area = .1 * (length * height * 2 + width * height * 2);
marble_area = .8 * (length * height * 2 + width * height * 2);
carpet_area = width * length;

total_area = plywood_area + glass_area + marble_area + carpet_area;

% rows: 250Hz, 1000Hz, 4000Hz
% columns: plywood, glass, marble
absorption_rates = [.22 .25 .01 .06;
                    .22 .12 .01 .37;
                    .11 .04 .02 .65;];

effective_areas_250 = [plywood_area glass_area marble_area carpet_area] .* absorption_rates(1,:);
effective_areas_1000 = [plywood_area glass_area marble_area carpet_area] .* absorption_rates(2,:);
effective_areas_4000 = [plywood_area glass_area marble_area carpet_area] .* absorption_rates(3,:);

gc = (-2 * log(.001)) / .161;

t60_250_s = -2 * log(.001) * volume / (gc * sum(effective_areas_250));
t60_1000_s = -2 * log(.001) * volume / (gc * sum(effective_areas_1000));
t60_4000_s = -2 * log(.001) * volume / (gc * sum(effective_areas_4000));

t60_250_a = 110; % This seems so wrong!
t60_1000_a = 30;
t60_4000_a = 7;

t60_250 = (1/t60_250_s + 1/t60_250_s)^-1
t60_1000 = (1/t60_1000_s + 1/t60_1000_s)^-1
t60_4000 = (1/t60_4000_s + 1/t60_4000_s)^-1

% Now the T60s go down at higher frequencies instead of going up!