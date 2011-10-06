
// pitchVariance should be <= 1
var pitchVariance : float;
var pitchBaseline : float;

function Start () {
    audio.pitch = pitchBaseline + (Random.value * 2 - 1) * pitchVariance;

    //Debug.Log(audio.pitch);
}