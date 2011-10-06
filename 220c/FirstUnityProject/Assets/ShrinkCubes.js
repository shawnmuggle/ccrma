
private var lightning : LineRenderer;

function Update () {
    if (Input.GetButtonDown("Fire1"))
    {
	var hit : RaycastHit;
	if (Physics.Raycast (Camera.main.transform.position, Camera.main.transform.forward, hit)) {
	    var shrinkScript = hit.transform.GetComponent("Shrink");
	    if (shrinkScript)
		shrinkScript.shrink();
	}
    }

    if (Input.GetButton("Fire1") || Input.GetButton("Fire3"))
    {
	if (lightning == null)
	{
	    lightning = gameObject.AddComponent(LineRenderer);
	    var player = GameObject.FindWithTag("Player");
	    if (player)
	    {
		player.SendMessage("setBeamOn");
	    }
	}
	lightning.material = new Material (Shader.Find("Particles/Additive"));
	var c3 : Color = Color.white;
	var c4 : Color = Color.white;
	lightning.SetColors(c3, c4);
	lightning.SetWidth(0.001,0.001);
	var numVertices = 100;
	lightning.SetVertexCount(numVertices);
	lightning.SetPosition(0, Camera.main.transform.position - Camera.main.transform.up * 0.02);
	for (var i = 1.0; i < numVertices; i++)
	{
	    var diff = (Camera.main.transform.position + Camera.main.transform.forward * 4) - (Camera.main.transform.position - Camera.main.transform.up * 0.02);
	    var percent = i / numVertices;
	    var point = Camera.main.transform.position - Camera.main.transform.up * 0.02 + diff * percent + Vector3(Random.value * 0.01 - 0.005, Random.value * 0.01 - 0.005, Random.value * 0.01 - 0.005);
	    lightning.SetPosition(i, point);
	}
    }

    if (Input.GetButton("Fire3"))
    {
	if (Physics.Raycast (Camera.main.transform.position, Camera.main.transform.forward, hit)) {
	    var shrinkAndDestroyScript = hit.transform.GetComponent("ShrinkAndDestroy");
	    if (shrinkAndDestroyScript)
		shrinkAndDestroyScript.shrink();
	}
    }

    if (Input.GetButtonUp("Fire3") || Input.GetButtonUp("Fire1"))
    {
	Destroy(lightning);
	lightning = null;
	player = GameObject.FindWithTag("Player");
	if (player)
	{
	    player.SendMessage("setBeamOff");
	}
    }
}