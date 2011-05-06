using UnityEngine;
using System.Collections;
using System.Collections.Generic;

[AddComponentMenu("Scripts/OSCSender")]
public class OSCSender : MonoBehaviour
{

  private Osc oscHandler;

  public string remoteIp;
  public int sendToPort;
  public int listenerPort;

  ~OSCSender()
  {
    if (oscHandler != null)
      {            
	oscHandler.Cancel();
      }

    // speed up finalization
    oscHandler = null;
    System.GC.Collect();
  }

  void Update()
  {
  }

  void sendCollision(Transform hit)
  {
    OscMessage oscM = Osc.StringToOscMessage("/cube/collision " + transform.position.x + " " + transform.position.y + " " + transform.position.z);
    oscHandler.Send(oscM);
  }

  void OnDisable()
  {
    // close OSC UDP socket
    Debug.Log("closing OSC UDP socket in OnDisable");
    oscHandler.Cancel();
    oscHandler = null;
  }

  void Start()
  {        
    UDPPacketIO udp = GetComponent<UDPPacketIO>();
    udp.init(remoteIp, sendToPort, listenerPort);
        
    oscHandler = GetComponent<Osc>();
    oscHandler.init(udp);
        
    oscHandler.SetAddressHandler("/test_unity_in", Example);
  }

  public static void Example(OscMessage m)
  {
    Debug.Log("--------------> OSC example message received: ("+m+")");
  }



}
