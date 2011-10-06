
var prefab : Transform;

private var spawnCubes : boolean = false;
private var numCubesSpawned : int = 0;

function Update () {

    if (Input.GetKeyDown(KeyCode.BackQuote))
    {
	spawnCubes = true;

	var player = GameObject.FindWithTag("Player");
	if (player)
	{
	    player.SendMessage("stopOutside");
	}
    }

    if (spawnCubes && Random.value < 0.01)
    {
	var terrainSize = GetComponent(Terrain).terrainData.size;
	var pos = GetComponent(Terrain).GetPosition();

	var position =  Vector3(pos.x + Random.value * terrainSize.x, 
				pos.y - 300,
				pos.z + Random.value * terrainSize.z);

	var size = Vector3(500, 500, 500);
	
	var newObject = Instantiate(prefab, position, Quaternion.identity);
	newObject.transform.localScale = size;
	newObject.rigidbody.velocity = Vector3(0, Random.value * 50, 0);
	newObject.rigidbody.drag = 0;
	newObject.audio.maxDistance = 2000;
	Destroy(newObject.collider);


	var flashScript = newObject.GetComponent(FlashOnPlay);
	if (flashScript)
	    flashScript.maxDistance = 1000;

    }

}