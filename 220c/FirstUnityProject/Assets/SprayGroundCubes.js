
var prefab : Transform;

function Update () {
    if (Input.GetButton("Fire2"))
    {
	var cam = Camera.main.transform;
	var newObject = Instantiate(prefab, cam.position + cam.forward * 0.4 + Vector3(Random.value * 2 - 1, Random.value * 2 - 1, Random.value * 2 - 1) * 0.1, Random.rotation);
	newObject.transform.rigidbody.AddForce(cam.forward * 100 + Vector3((Random.value - 0.5) * 100, (Random.value - 0.5) * 100, (Random.value - 0.5) * 100));
	newObject.transform.localScale = Vector3(0.1 + Random.value * 0.1, 0.1 + Random.value * 0.1, 0.1 + Random.value * 0.1);

	var floor = GameObject.FindWithTag("floor");
	if (floor)
	{
	    var createFlyingCubesScript = floor.GetComponent(CreateFlyingCubes);
	    createFlyingCubesScript.threshold *= 0.9;
	}
    }
}