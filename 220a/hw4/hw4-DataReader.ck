// type in a data file's full pathname as argument to this file and it'll play the data file
// in miniAudicle the argument goes into the text field just above this document
// this file can only be shredded once since it declares a public class below

// any agrument supplied?
if (me.args()) 
{ 
  DataReader data;
  data.setDataSource(me.arg(0)); // file name is assumed to be first argument
  data.start();
  data.scaledVal();

  PulseOsc s => BPF f => dac;
  1 => f.Q;
  40 => s.freq;

  VoicForm v => dac;
  VoicForm v2 => dac;
  VoicForm v3 => dac;
  0.2 => v.pitchSweepRate;
  150 => v.freq;
  1.0 => v.voiced;
  0.0 => v.unVoiced;
  0.4 => v.loudness;

  0.5 => v2.pitchSweepRate;
  200 => v2.freq;
  0.7 => v2.voiced;
  0.3 => v2.unVoiced;
  0.4 => v2.loudness;

  1.0 => v3.pitchSweepRate;
  300 => v3.freq;
  0.9 => v3.gain;
  0.3 => v3.voiced;
  0.7 => v3.unVoiced;
  0.4 => v3.loudness;

  StifKarp b => Pan2 p => dac;
  1000 => b.freq;
  0.2 => b.gain;

  0 => float prevVal;
  while (!data.isFinished())
  {
    data.scaledVal() => float val; // next data point, scaled in 0.0 - 1.0 range
    val - prevVal => float delta;
    s.gain(val);
    s.freq(Std.mtof(15.0 + delta*20.0));
    f.freq(Std.mtof(10.0 + val*100.0));

    if (delta > 0.53)
    {
	(delta * 64) $ int => int phonemeNum;
	phonemeNum => v.phonemeNum;
	phonemeNum => v2.phonemeNum;
	phonemeNum => v3.phonemeNum;

    	1.0 => v.speak;
    	1.0 => v2.speak;
    	1.0 => v3.speak;
	500::ms => now;
	1.0 => v.quiet;
	1.0 => v2.quiet;
	1.0 => v3.quiet;
    }

    val * val => b.stretch;
    b.freq() => float freq;
    val *=> freq;
    freq => b.freq;
    if (b.freq() < 1000)
    {
	1000 => b.freq;
    }    

    val => p.pan;
    1.0 => b.noteOn;
    30::ms => now;
  }
}

// argument or not, define the DataReader
// public class so only shred once
// reads time series file and serves up values
public class DataReader { 
  "data.dat" => string inFilename;
  999999999999.9 => float bigNum;
  bigNum => float minVal;
  -bigNum => float maxVal;
  float range;
  float scale;
  0 => int cnt;
  false => int cleaned;
  fun float min() { return minVal; }
  fun float max() { return maxVal; }
  fun float rng() { return range; }
  fun float scl() { return scale; }
  fun int count() { return cnt; }

  // set data file
  fun void setDataSource( string f )
  {
      f => inFilename; // full path
  }

  // file pointer
  FileIO @ finp;
  new FileIO @=> finp;
   
  1 => int DC; // number of parallel time series
    
  // current time series' value
  float vals[];
  new float[DC] @=> vals;
  
  // flag end of file
  1 => int finished;
  fun int isFinished()
  {
    return finished;
  }
  
  // open file, close first if already opened
  fun void openData( )
  {
    if( finp.good() ) finp.close();
    0 => finished;
    finp.open( inFilename, FileIO.READ );
    <<<inFilename>>>;
    if( !finp.good() )cherr <= "can't open file: " <= inFilename <= " for reading..." <= IO.newline();
  }

  // run through all data points to find statistics
  fun void calibrate( )
  {
    bigNum => minVal;
    -bigNum => maxVal;
    openData();
    while(next()) 
    { 
      if (vals[0] > maxVal) vals[0] => maxVal;
      if (vals[0] < minVal) vals[0] => minVal;
      cnt++; 
    }
    <<<cnt, "data points">>>;
    max()-min() => range;
    1.0 / rng() => scale;
  }

  // hack that will check if input string is actually a number, maybe not foolproof
  fun int isNumber (string s)
  {
    if (s=="0") return true;
    if (s=="0.0") return true;
    if (s=="0.00") return true;
    if (s=="0.000") return true;
    if (s=="0.0000") return true;
    if (s=="0.00000") return true;
    return !(Std.atof(s)==0.0000);
  }

  // read a line, if we can
  fun int next()
  {
    if (!finp.good()) {<<<"file not open">>>; return false;}
    if (isFinished()) {
      // <<<"finished file already">>>; 
      return false;}
    [ -1.0] @=> vals; // start vals with non-vals    
    string s; // must declare the read string first, can't be inline...
    finp => s; // read a string up to first space or newline
    if (finp.eof()) 
    {
      //<<<"encountered EOF">>>; 
      1 => finished; 
      return false;
    }
    finp.readLine() => string rest;
    if (isNumber(s))
      Std.atof(s) => vals[0]; // should be good to go
    else <<< "non-number string found in data, line ", cnt, " = ", s+rest>>>;
    return true;
  }

  // calibrate and cue up at start 
  fun void start( )
  {
    if (!cnt) calibrate();
    openData();
  }

  // return one datum 
  fun float val( )
  {
    next(); 
    return vals[0];
  }

  // scaled version 
  fun float scaledVal( )
  {
    val() - min() => float tmp; 
    return scl() * tmp;
  }

}

