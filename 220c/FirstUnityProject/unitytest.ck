OscRecv orec;
8000 => orec.port;
orec.listen();

orec.event("/cube/collide,f f f") @=> OscEvent collide_event; 

SinOsc i => Envelope env => dac;
400::ms => env.duration;

while ( true )
{ 
    collide_event => now; // wait for events to arrive.

    // grab the next message from the queue. 
    while( collide_event.nextMsg() != 0 )
    { 
		<<< "HIII" >>>;
		<<< collide_event.getFloat() >>>;
		1 => i.next;
	}

	1 => env.value;
	1 => env.keyOff;
}       