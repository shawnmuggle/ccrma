var newObject : Transform;

protected var time_accum : float;

function Start() {
    time_accum = 0;
}

function FixedUpdate () {
    time_accum += Time.deltaTime;
    if (Input.GetButton("Fire1") && time_accum > 0.5) {
	var camera_transform = transform.Find("Main Camera").transform;
	var instance = Instantiate(newObject, transform.position + camera_transform.forward * 2.0, camera_transform.rotation);
	Debug.Log(instance);
	var color = Color(Random.value,Random.value,Random.value);
	var script : CollisionFlash = instance.GetComponent(CollisionFlash);
	Debug.Log(script);
	instance.renderer.material.color = color;
	script.targetColor = color;
	time_accum = 0;
    }
}