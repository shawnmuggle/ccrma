
private var alphaFadeValue : float = 0;

var blackTexture : Texture;

function Update () {

    if (Input.GetKeyDown(KeyCode.Alpha1))
    {
	transform.SendMessage("gameOver");
    }

    alphaFadeValue += Mathf.Clamp01(Time.deltaTime / 5);
    
    GUI.color.a = alphaFadeValue;
    Debug.Log(blackTexture);
    GUI.DrawTexture( Rect(0, 0, Screen.width, Screen.height ), blackTexture );

}