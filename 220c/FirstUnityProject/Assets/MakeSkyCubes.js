var prefab : Transform;
var roomPrefab : Transform;

function Start () {

    Screen.showCursor = false;

    // Skycubes
    var numCubes = 4; // per row
    var cubeSpacing = 0.1;

    var bounds = transform.renderer.bounds;

    var cubeDimension = bounds.size.x / numCubes; // Assuming the plane is a square

    var newObject : Transform;

    for (var i = 0; i < numCubes; i++)
    {
	for (var j = 0; j < numCubes; j++)
	{
	    var position =  Vector3(bounds.min.x + cubeDimension * j + cubeDimension * 0.5, 
				    transform.position.y + cubeDimension * 0.5 + 0.5, 
				    bounds.min.z + cubeDimension * i + cubeDimension * 0.5);
	    var size = Vector3(cubeDimension * (1 - cubeSpacing), 
	    		       cubeDimension * (1 - cubeSpacing), 
	    		       cubeDimension * (1 - cubeSpacing));
	    
	    newObject = Instantiate(prefab, position, Quaternion.identity);
	    newObject.transform.localScale = size;

	    //Debug.Log(newObject.collider.bounds);

	    //newObject.collider.center = position;
	    //newObject.collider.size = size;
	    
	    //Debug.Log(newObject.collider.bounds);
	}
    }

    
    // Roomcubes
    var roomWidth = 10.0; // in cubes
    var roomHeight = 6.0; // in cubes

    var cubeSize = roomPrefab.renderer.bounds.size;

    for (i = 0; i < roomWidth; i++)
    {
	for (j = 1; j < roomHeight; j++)
	{
	    if (i != 0)
	    {
		var x = transform.position.x + (i - roomWidth * 0.5) * cubeSize.x;
		var z = transform.position.z - roomWidth * 0.5 * cubeSize.z;
		var y = transform.position.y + j * cubeSize.y;
		Instantiate(roomPrefab, Vector3(x, y, z), Quaternion.identity);
		
		z = transform.position.z + roomWidth * 0.5 * cubeSize.z;
		Instantiate(roomPrefab, Vector3(x, y, z), Quaternion.identity);
		
		x = transform.position.x - roomWidth * 0.5 * cubeSize.x;
		z = transform.position.z - (i - roomWidth * 0.5) * cubeSize.z;
		Instantiate(roomPrefab, Vector3(x, y, z), Quaternion.identity);
		
		x = transform.position.x + roomWidth * 0.5 * cubeSize.x;
		Instantiate(roomPrefab, Vector3(x, y, z), Quaternion.identity);
	    }
	}
    }

    for (i = 0; i < roomWidth; i++)
    {
	for (j = 0; j < roomWidth; j++)
	{
	    x = transform.position.x + (i - roomWidth * 0.5) * cubeSize.x;
	    z = transform.position.z - (j - roomWidth * 0.5) * cubeSize.z;
	    y = transform.position.y + roomHeight * cubeSize.y;
	    Instantiate(roomPrefab, Vector3(x, y, z), Quaternion.identity);
	}
    }
}