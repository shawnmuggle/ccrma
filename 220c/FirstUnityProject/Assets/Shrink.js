
private var shrinking = false;

function Update () {

    if (shrinking)
    {
	var hpf = GetComponent(AudioHighPassFilter);
	hpf.cutoffFrequency *= 1.005;

	var flashScript = GetComponent(FlashOnPlay);

	if ( transform.localScale.x > 0.3 )
	{
	    transform.localScale.x *= 0.99;
	    flashScript.targetColor *= 1.003;
	}
	else if ( transform.localScale.z > 0.3 )
	{
	    transform.localScale.z *= 0.99;
	    flashScript.targetColor *= 1.003;
	}
	else if ( transform.localScale.y > 0.3 )
	{
	    transform.localScale.y *= 0.99;
	    flashScript.targetColor *= 1.003;
	}
	else
	{
	    Debug.Log(flashScript.targetColor);
	    rigidbody.useGravity = true;
	    rigidbody.AddTorque(Vector3(Random.value, Random.value, Random.value));
	    shrinking = false;
	}
    }

}

function shrink()
{
    shrinking = true;
}