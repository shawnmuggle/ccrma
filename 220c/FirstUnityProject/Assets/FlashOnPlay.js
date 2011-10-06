
var maxDistance : float = 100;

var targetColor : Color;

function Start() {
    targetColor = renderer.material.color;
    lastTime = 100000;
}

function FlashWhite () {
    //Debug.Log("HELO CUBE FLASH");
    renderer.material.color = Color(1 - targetColor.r, 1 - targetColor.g, 1- targetColor.b);
}

function Update() {
    renderer.material.color.r = 0.03 * (targetColor.r - renderer.material.color.r) + renderer.material.color.r;
    renderer.material.color.g = 0.03 * (targetColor.g - renderer.material.color.g) + renderer.material.color.g;
    renderer.material.color.b = 0.03 * (targetColor.b - renderer.material.color.b) + renderer.material.color.b;

    if (!audio.isPlaying && Vector3.Distance(transform.position, Camera.main.transform.position) < maxDistance) {
	FlashWhite();
	audio.Play();
    }

    // if (Vector3.Distance(transform.position, Camera.main.transform.position) > 100)
    // {
    // 	audio.volume = 0;
    // }

}