var targetColor : Color;

function FlashWhite () {

    Debug.Log("HELO CUBE FLASH");
    renderer.material.color = Color(1,1,1);
}

function Update() {
    renderer.material.color.r = 0.05 * (targetColor.r - renderer.material.color.r) + renderer.material.color.r;
    renderer.material.color.g = 0.05 * (targetColor.g - renderer.material.color.g) + renderer.material.color.g;
    renderer.material.color.b = 0.05 * (targetColor.b - renderer.material.color.b) + renderer.material.color.b;
}