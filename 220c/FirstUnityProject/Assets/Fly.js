
function Update () {
    if ( Input.GetKey("left shift") ) {
	constantForce.force = 8000 * Vector3.up;
    }
    else {
	constantForce.force = Vector3.zero;
    }
}