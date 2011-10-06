
private var targetColor : Color;

function Start() {
    targetColor = renderer.material.color;
}

function FlashWhite () {
    //Debug.Log("HELO CUBE FLASH");
    renderer.material.color = Color(0.5,0.5,0.5);
}

function Update() {
    renderer.material.color.r = 0.03 * (targetColor.r - renderer.material.color.r) + renderer.material.color.r;
    renderer.material.color.g = 0.03 * (targetColor.g - renderer.material.color.g) + renderer.material.color.g;
    renderer.material.color.b = 0.03 * (targetColor.b - renderer.material.color.b) + renderer.material.color.b;

    if (Random.value < 0.005) {
	FlashWhite();
	if (!audio.isPlaying)
	{
	    audio.Play();
	}
    }
}