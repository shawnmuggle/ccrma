class LSystemNode
{
    LSystemNode@ prev, next;
    
    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		// CAN'T REUSE THIS NODE, GOTTA MAKE A NEW ONE
		// But ChucK doesn't have class introspection so we can't instantiate a new object of the same class as whatever this is.
		// So we have to write a custom expandInLSystem for each node subclass which at least just creates a new object of the same class and returns it.
    }
    
    fun void playInLSystem(LSystem sys)
    { }
}

class LSystem
{
    LSystemNode@ rootNode; // This is always just a blank head node
    
    LSystemNode root @=> rootNode;

	400 => float tempo;
	1::minute / tempo => dur beat;
	
	JCRev r => Gain g => dac;
	0.5 => g.gain;
	0.1 => r.mix;
	r @=> UGen@ chain;

    fun void advance()
    {
        rootNode.next @=> LSystemNode@ oldListIterator;

        LSystemNode newRoot @=> LSystemNode@ newListIterator;

        while (oldListIterator != null)
        {
            oldListIterator.expandInLSystem(this) @=> newListIterator.next; // Generator the new sub-list from the old node (expand)

			while (newListIterator.next != null)
            {
                newListIterator.next @=> newListIterator; // Advance the current node in the new list to the end
            }
            
            oldListIterator.next @=> oldListIterator; // Move to the next node in the old list.
        }
		
        newRoot @=> rootNode;
    }
    
    fun void addNode(LSystemNode@ node)
    {
        LSystemNode@ listIterator;
        rootNode @=> listIterator;
        while (listIterator.next != null)
        {
            listIterator.next @=> listIterator; // Advance the current node in the new list to the end
        }
        node @=> listIterator.next;
    }
    
    fun int numNodes()
    {
        0 => int i;
        LSystemNode@ listIterator;
        rootNode @=> listIterator;
        while (listIterator.next != null)
        {
            listIterator.next @=> listIterator; // Advance the current node in the new list to the end
            i++;
        }
        return i;
    }

	fun void loop(dur length)
	{
		Envelope e => chain;
		e @=> chain;
		length * 0.5 => e.duration;

		now + length * 0.5 => time half;
		e.keyOn();
		while (now < half)
		{
			this.play();
		}

		now + length * 0.5 => time end;
		e.keyOff();
		while (now < end)
		{
			this.play();
		}
	}
	
	fun void play()
	{
		rootNode @=> LSystemNode@ iterator;
		while (iterator != null)
		{
			iterator.playInLSystem(this);
			iterator.next @=> iterator;
		}
	}
}

class TenorNode extends LSystemNode
{
	Std.rand2(1, 4) => int i;
	"drums/tenor" + i + ".wav" => string t1_filename;

	Std.rand2(1, 4) => i;
	"drums/tenor" + i + ".wav" => string t2_filename;
	
	Std.rand2(1, 4) => i;
	"drums/tenor" + i + ".wav" => string t3_filename;
	
    fun void playInLSystem(LSystem sys)
    {
		SndBuf t1_buf => sys.chain;
		t1_filename => t1_buf.read;
		SndBuf t2_buf => sys.chain;
		t2_filename => t2_buf.read;
		SndBuf t3_buf => sys.chain;
		t3_filename => t3_buf.read;

		0 => t2_buf.rate;
		0 => t3_buf.rate;
        sys.beat * 0.5 => now;

		1 => t2_buf.rate;
        sys.beat * 0.5 => now;

		1 => t3_buf.rate;
		sys.beat => now;

		t1_buf =< sys.chain;
		t2_buf =< sys.chain;
		t3_buf =< sys.chain;
    }

    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		TenorNode n1;
		TenorNode n2 @=> n1.next;
		return n1;
	}	
}

class SnareRollNode extends LSystemNode
{
	"drums/snare_roll1.wav" => string roll_filename;
	"drums/snare_gok1.wav" => string gok_filename;

    fun void playInLSystem(LSystem sys)
    {
		SndBuf roll_buf => sys.chain;
		roll_filename => roll_buf.read;
		SndBuf gok_buf => sys.chain;
		gok_filename => gok_buf.read;
		0 => gok_buf.rate;

        sys.beat => now;
		1 => gok_buf.rate;
		0 => roll_buf.rate;
		2 * sys.beat => now;

		roll_buf =< sys.chain;
		gok_buf =< sys.chain;
    }

    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		SnareRollNode n1;
		TenorNode n2 @=> n1.next;
		return n1;
	}	
}

class Beat2Node extends LSystemNode
{
	Std.rand2(1, 2) => int i;
	"drums/snare" + i + ".wav" => string snare_filename;

	Std.rand2(1, 6) => i;
	"drums/bd" + i + ".wav" => string bd_filename;
	
    fun void playInLSystem(LSystem sys)
    {
		SndBuf bd_buf => sys.chain;
		bd_filename => bd_buf.read;
		SndBuf snare_buf => sys.chain;
		snare_filename => snare_buf.read;

        sys.beat => now;
		0 => bd_buf.pos;
		0 => snare_buf.pos;
        sys.beat / 2 => now;
		0 => snare_buf.pos;
        sys.beat / 2 => now;
		0 => bd_buf.pos;
        sys.beat / 2 => now;
		0 => snare_buf.pos;
        sys.beat / 2 => now;
		0 => bd_buf.pos;
		0 => snare_buf.pos;
		sys.beat => now;

		bd_buf =< sys.chain;
		snare_buf =< sys.chain;
    }

    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		Beat2Node n1;		
		return n1;
	}	
}

class CymbalNode extends LSystemNode
{
	Std.rand2(1, 4) => int i;
	"drums/cymbal" + i + ".wav" => string filename;

    fun void playInLSystem(LSystem sys)
    {
		SndBuf buf => sys.chain;
		filename => buf.read;
        sys.beat => now;

		if (Std.rand2f(0, 1) > 0.5)
		{
			sys.beat * 4 => now;
		}

		buf =< sys.chain;
    }
    
    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		CymbalNode n1;
		
		if (Std.rand2f(0, 1) > 0.8)
		{
			Beat2Node b @=> n1.next;
		}

		return n1;
	}	
}

class BassDrumNode extends LSystemNode
{
	Std.rand2(1, 6) => int i;
	"drums/bd" + i + ".wav" => string filename;

    fun void playInLSystem(LSystem sys)
    {
		SndBuf buf => sys.chain;
		filename => buf.read;
        sys.beat => now;

		buf =< sys.chain;
    }
    
    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		BassDrumNode bd1;
		BassDrumNode bd2 @=> bd1.next;

		bd2 @=> LSystemNode@ lastNode;
		if (Std.rand2f(0, 1) > 0.9)
		{
			CymbalNode c1 @=> lastNode.next;
			c1 @=> lastNode;
		}
		if (Std.rand2f(0, 1) > 0.7)
		{
			SnareRollNode sr1 @=> lastNode.next;
			sr1 @=> lastNode;
		}

		return bd1;
	}
}

class SpeedUpNode extends LSystemNode
{
    fun void playInLSystem(LSystem sys)
	{
		sys.beat / 2 => sys.beat;
		if (sys.beat < 1::minute / 1000) {
			sys.beat * 2 => sys.beat;
		}
	}

    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		SpeedUpNode s;
		return s;
	}
}

class SlowDownNode extends LSystemNode
{
    fun void playInLSystem(LSystem sys)
	{
		sys.beat * 2 => sys.beat;
		if (sys.beat > 1::minute / 100) {
			sys.beat / 2 => sys.beat;
		}
	}

    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		SlowDownNode s;
		return s;
	}
}

class SnareNode extends LSystemNode
{
	Std.rand2(1, 2) => int i;
	"drums/snare" + i + ".wav" => string filename;

    fun void playInLSystem(LSystem sys)
    {
		SndBuf buf => sys.chain;
		filename => buf.read;
        sys.beat => now;

		buf =< sys.chain;
    }
    
    fun LSystemNode@ expandInLSystem(LSystem sys)
    {
		SnareNode n1;
		BassDrumNode n2 @=> n1.next;
		SnareNode n3 @=> n2.next;
		n3 @=> LSystemNode@ lastNode;
		if (Std.rand2f(0, 1) > 0.7)
		{
			SnareNode n4 @=> lastNode.next;
			n4 @=> lastNode;
		}
		if (Std.rand2f(0, 1) > 0.95)
		{
			SpeedUpNode su @=> lastNode.next;
			su @=> lastNode;
		}
		if (Std.rand2f(0, 1) < 0.05)
		{
			SlowDownNode sd @=> lastNode.next;
			sd @=> lastNode;
		}

		return n1;
	}
}

LSystemNode@ nextNodes[10];
0 => int nextNodesIndex;

fun void newBand()
{
	LSystem sys;

	if (nextNodesIndex > 0)
	{
		for (0 => int i; i < nextNodesIndex; i++)
		{
			nextNodes[i] => sys.addNode;
		}
		0 => nextNodesIndex;
	}
	else
	{
		BassDrumNode n1 => sys.addNode;
		SnareNode n2 => sys.addNode;
		BassDrumNode n3 => sys.addNode;
	}
	
	spork ~ sys.loop(20::second);
	
	2::second => now;
	sys.advance();
	3::second => now;
	sys.advance();
	4::second => now;
	sys.advance();
	5::second => now;
	sys.advance();
	6::second => now;
}

fun void loopSound(string filename)
{
	SndBuf buf => Gain g => dac;
	0.5 => g.gain;
	filename => buf.read;
	0 => buf.pos;
	1 => buf.rate;
	
	while ( true )
	{
		buf.samples()::samp => now;
		0 => buf.pos;
	}
}

class CitySounds
{
	fun void playRain()
	{
		spork ~ loopSound("rain/rain1.wav");
		spork ~ loopSound("rain/rain2.wav");
		spork ~ loopSound("rain/rain3.wav");
		spork ~ loopSound("rain/rain4.wav");
	}

	fun void playCity()
	{
		spork ~ loopSound("city/city3.wav");
	}

	fun void playTraffic()
	{
		spork ~ loopSound("traffic/traffic1.wav");
	}
}

// the device number to open
0 => int deviceNum;

// instantiate a HidIn object
HidIn hi;
// structure to hold HID messages
HidMsg msg;

// open keyboard
if( !hi.openKeyboard( deviceNum ) ) me.exit();
// successful! print name of device
<<< "keyboard '", hi.name(), "' ready" >>>;

CitySounds city;

city.playRain();
city.playCity();

while (true)
{
	hi => now;

	while( hi.recv( msg ) )
	{
		if( msg.isButtonDown())
		{
			if (msg.which == 44)
			{
				spork ~ newBand();
			}

			if (msg.which == 5)
			{
				BassDrumNode b @=> nextNodes[nextNodesIndex++];
			}
			if (msg.which == 6)
			{
				CymbalNode c @=> nextNodes[nextNodesIndex++];
			}
			if (msg.which == 21)
			{
				SnareNode s @=> nextNodes[nextNodesIndex++];
			}
			if (msg.which == 22)
			{
				SnareRollNode r @=> nextNodes[nextNodesIndex++];
			}
			if (msg.which == 23)
			{
				TenorNode t @=> nextNodes[nextNodesIndex++];
			}
		}
	}	
}