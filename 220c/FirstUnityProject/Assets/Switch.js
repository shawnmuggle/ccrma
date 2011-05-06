function Update () {
	if (Input.GetButtonDown("Jump"))
	{
		var newTarget = GameObject.FindWithTag("foobar").transform;
		GetComponent("Follow").target = newTarget;
		Debug.Log(GetComponent("Follow"));
	}
}