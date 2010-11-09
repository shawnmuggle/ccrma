// binaural listening requires headphones

// this version simulates the head being surrounded by 4 speakers
// and delivers tailored signals to each ear, as if coming from 
// the speakers

// this class inputs 4 channels and outputs 2

// plug headphones into dac.chan(1-2) and start it
// after it's started, other shreds can send their
// signals to Binaural.pssp[1-4] which are the
// "psuedo speakers" in quad formation

// impulse responses were made from each speaker to each ear
// using the nearfield quad setup in the ccrma ballroom with
// help from Jonathan Abel

public class Binaural
{
// number of synthesized point sources 
// which will be the number of speakers that produced impulse responses
4 => int nChans;

// psuedo speakers
static Gain @ pssp[nChans];
for (0=> int i; i<(nChans); i++) new Gain @=> pssp[i];

// function to convolve 1 input with 1 ear
fun void mixEar(int chan, int e)
{
  string ear;
  if (!e) "l" => ear; else "r" => ear;
  (chan+1) + ear => string name;

// impulse response location
  "/Users/mrotondo/Development/ccrma/220a/hw4/ch"+name+".wav" => string Yname;
  SndBuf Ybuf;
  Yname => Ybuf.read;
  Ybuf.gain(50.0);

// patch input to fft
  pssp[chan] => FFT X => blackhole;
// patch impulse response to fft
  Ybuf => FFT Y => blackhole;
// patch output to dac
  IFFT ifft => dac.chan(e);

// set FFT size
  1024 => int FFT_SIZE;
  if (FFT_SIZE < (Ybuf.samples())) <<<"need longer fft ", FFT_SIZE, Ybuf.samples()>>>;

  FFT_SIZE => X.size => Y.size;
// desired hop size
  FFT_SIZE / 4 => int HOP_SIZE;
// set window and window size
  Windowing.hann(FFT_SIZE ) => X.window;
  Windowing.rectangle(FFT_SIZE ) => Y.window;
  Windowing.hann(FFT_SIZE ) => ifft.window;
// use this to hold contents
  complex Z[FFT_SIZE/2];

// feed impulse response into fft buffer
  Ybuf.samples()::samp + now => time ir; // zero pad
  while( now < ir ) FFT_SIZE::samp => now;
  Ybuf =< Y =< blackhole;
// take ir's fft
  Y.upchuck();

  while( true )
  {
    // take incoming signal's fft
    X.upchuck();
    
    // multiply
    for( int i; i < X.size()/2; i++ )
        //Math.sqrt((Y.cval(i)$polar).mag) * X.cval(i) => Z[i]; // ask about this?
        2 * Y.cval(i) * X.cval(i) => Z[i];
    
    // take ifft
    ifft.transform( Z );
    
    // advance time
    HOP_SIZE::samp => now;
  }
 }

// mixing matrix 4x2
spork ~ mixEar(0,0);
spork ~ mixEar(0,1);

spork ~ mixEar(1,0);
spork ~ mixEar(1,1);

spork ~ mixEar(2,0);
spork ~ mixEar(2,1);

spork ~ mixEar(3,0);
spork ~ mixEar(3,1);
}

// instantiate and tweak gains
Binaural b;
b.pssp[0].gain(3.0);
b.pssp[1].gain(2.5);
b.pssp[2].gain(3.1);
b.pssp[3].gain(4.5);
// infinite stall
while(true){1::day=>now;};

