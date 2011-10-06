var prefab : Transform;

var threshold : float;

private var createCubes : boolean = false;

function Update () {
    if (Input.GetKeyDown(KeyCode.Tab))
    {
	createCubes = true;
    }

    if (createCubes && Random.value < threshold)
    {
	side = Random.Range(0, 3);

	var position : Vector3;
	var velocity : Vector3;

	if (side == 0 || side == 1 || side == 2 || side == 3)
	{
	    // One of the sides (not top or bottom)
	    position = transform.position;
	    position.y = transform.position.y + renderer.bounds.extents.y + Random.value * 0.5;

	    if (side == 0 || side == 2)
	    {
		position.x += (Random.value * 2 - 1) * renderer.bounds.extents.x;
		if (side == 0)
		{
		    position.z += renderer.bounds.extents.z;
		    velocity = -Vector3.forward * (60 + Random.value * 20);
		}
		else
		{
		    position.z -= renderer.bounds.extents.z;
		    velocity = Vector3.forward * (60 + Random.value * 20);
		}
	    }
	    if (side == 1 || side == 3)
	    {
		position.z += (Random.value * 2 - 1) * renderer.bounds.extents.z;
		if (side == 1)
		{
		    position.x += renderer.bounds.extents.x;
		    velocity = Vector3.left * (60 + Random.value * 20);
		}
		else
		{
		    position.x -= renderer.bounds.extents.x;
		    velocity = -Vector3.left * (60 + Random.value * 200);
		}
	    }
	}

	var tinyCube = Instantiate(prefab, position, Quaternion.identity);
	tinyCube.rigidbody.AddForce(velocity);
    }
}