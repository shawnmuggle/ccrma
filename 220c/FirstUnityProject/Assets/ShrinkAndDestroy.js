
private var shrinking = false;

function Update () {

    if (shrinking)
    {
	if ( transform.localScale.x > 0.01 )
	{
	    transform.localScale.x *= 0.9;
	    transform.renderer.material.color *= 1.002;
	}
	else if ( transform.localScale.z > 0.01 )
	{
	    transform.localScale.z *= 0.9;
	    transform.renderer.material.color *= 1.002;
	}
	else if ( transform.localScale.y > 0.01 )
	{
	    transform.localScale.y *= 0.9;
	    transform.renderer.material.color *= 1.002;
	}
	else
	{
	    var bigCubes = GameObject.FindGameObjectsWithTag ("bigcube");
	    for (var bigCube in bigCubes)
	    {
		var lowpass = bigCube.GetComponent(AudioLowPassFilter);
		lowpass.cutoffFrequency *= 1.05;
	    }
	    Destroy(gameObject);
	    var player = GameObject.FindWithTag("Player");
	    if (player)
	    {
		player.SendMessage("playCubeDeath");
	    }
	}
    }

}

function shrink()
{
    shrinking = true;
}