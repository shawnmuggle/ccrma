
var secondsToLive : float;

private var secondsLived : float = 0;

function Update () {

    secondsLived += Time.deltaTime;
    if (secondsLived > secondsToLive)
	Destroy(gameObject);
}