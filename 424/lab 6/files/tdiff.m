%tdiff.m                    generate input test wave for compression lab
fs=44100.0;			%set sampling rate
freq=800.0;			%frequency
highlev=1.0;
lowlev=0.1;			%low level at -20dB
hightime=0.0125;	%12.5 mS for burst signal
lowtime=1.0;		%1 second for recovery


burstnum=round(hightime*freq);  %number of cycles during burst

short=sin(linspace(0,burstnum*2*pi,round(burstnum/freq*fs)));
short=short(1:end-1); %leave off last zero

lownum=round(lowtime*freq);
long=sin(linspace(0,lownum*2*pi,round(lownum/freq*fs)));
long=long(1:end-1);

short2=[short short];
short4=[short2 short2];
short8=[short4 short4];
short16=[short8 short8];
short32=[short16 short16];
short64=[short32 short32];

tdiffsig=[highlev*short lowlev*long highlev*long lowlev*long];

plot(tdiffsig)

wavwrite(tdiffsig,44100,16,'tdiff.wav');
