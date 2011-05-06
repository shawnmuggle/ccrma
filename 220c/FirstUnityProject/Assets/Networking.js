var remoteIP = "127.0.0.1";
var remotePort = 9876;

function Awake () {
    
    Debug.Log("HELO");
    Network.Connect(remoteIP, remotePort);
}

function OnConnectedToServer() {
    Debug.Log("CONNECTED OMG!?!?!");
}

function OnFailedToConnect() {
    Debug.Log("failed failed failed to conenect :(:(");
}
