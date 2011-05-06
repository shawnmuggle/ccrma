function [impulse_response] = allpass_measurement()

rho = min_impulse_response();
allpassed_impulse_response = allpasses(64, rho);

max_value = max(abs(allpassed_impulse_response));
allpassed_impulse_response = allpassed_impulse_response / max_value;

allpass_system_response = hmeasure(allpassed_impulse_response);

impulse_response = flipud(allpasses(64, rho, flipud(allpass_system_response)));

impulse_response = fliplr(impulse_response(1:end));

max_value = max(abs(impulse_response));
impulse_response = impulse_response / max_value;