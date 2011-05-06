function [] = allpass_snr_compare ()
imp = [1 zeros(1, 1023)]';
imp_resp = hmeasure(imp);
imp_noise = imp_resp(1:100);
imp_rms = sqrt(mean(imp_noise .^ 2));
imp_log_rms = 20 * log10(imp_rms)

allpass_resp = allpass_measurement;
allpass_noise = allpass_resp(1:100);
allpass_rms = sqrt(mean(allpass_noise .^ 2));
allpass_log_rms = 20 * log10(allpass_rms)

allpass_log_rms - imp_log_rms