
private var timeAccum : float = 0.0;
private var accumulate : boolean = false;
private var timeToFall = 10;
private var timeToDestruct = 5;
private var falling = false;
private var destructing = false;

function Update () {

    if (Input.GetKeyDown(KeyCode.LeftControl))
    {
	var player = GameObject.FindWithTag("Player");
	if (player)
	{
	    player.SendMessage("playExplosion");
	}
	accumulate = true;
	timeAccum = 0.0;


    }

    if (accumulate)
    {
	timeAccum += Time.deltaTime;

	if (timeAccum > 2.0)
	{
	    accumulate = false;
	    timeAccum = 0.0;

	    gameObject.AddComponent(Rigidbody);
	    rigidbody.AddTorque(Random.rotation.eulerAngles * 1);
	    falling = true;
	    RenderSettings.fog = false;
	    
	    player = GameObject.FindWithTag("Player");
	    player.transform.localScale = Vector3(1,1,1);
	    var motorScript = player.GetComponent(CharacterMotor);
	    motorScript.movement.maxForwardSpeed = 50.0; // TODO: These should be 10
	    motorScript.movement.maxSidewaysSpeed = 50.0;
	    motorScript.movement.maxBackwardsSpeed = 50.0;
	    motorScript.movement.maxGroundAcceleration = 30.0;
	    motorScript.movement.maxAirAcceleration = 20.0;
	    motorScript.movement.gravity = 20.0;
	    motorScript.movement.maxFallSpeed = 20.0;
	    motorScript.jumping.baseHeight = 1.0;
	    motorScript.jumping.extraHeight = 4.1;

	    var bigcubes = GameObject.FindGameObjectsWithTag ("bigcube");
	    for (var bigcube in bigcubes)
	    {
		var diff = bigcube.transform.position - Camera.main.transform.position;
		bigcube.rigidbody.AddForce(diff * 20);
	    }

	    var roomcubes = GameObject.FindGameObjectsWithTag ("roomcube");
	    for (var roomcube in roomcubes)
	    {
		diff = roomcube.transform.position - Camera.main.transform.position;
		roomcube.rigidbody.AddForce(diff * 20);
	    }

	}
    }
    
    if (falling)
    {
	timeAccum += Time.deltaTime;
	if (timeAccum > timeToFall)
	{
	    timeAccum = 0.0;
	    destructing = true;
	}
	
    }

    if (destructing)
    {
	timeAccum += Time.deltaTime;
	transform.localScale *= 0.99;
	if (timeAccum > timeToDestruct)
	{
	    Destroy(gameObject);
	    player = GameObject.FindWithTag("Player");
	    if (player)
	    {
		player.SendMessage("startOutside");
	    }
	}
    }
}