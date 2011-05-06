function [rho] = min_impulse_response()
rhos = [0:0.0001:1];
maxes = zeros(1, 10001);
for i = [1:10001]
maxes(i) = max(abs(allpasses(64, rhos(i))));
end
[y, i] = min(maxes);
rho = i * 0.0001;