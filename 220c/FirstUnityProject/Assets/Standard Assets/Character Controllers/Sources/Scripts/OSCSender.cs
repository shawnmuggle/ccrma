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

  void sendVelocity(float velocity)
  {
    OscMessage oscM = Osc.StringToOscMessage("/setNoiseFreq " + velocity);
    oscHandler.Send(oscM);
  }

  void setBeamOn()
  {
    OscMessage oscM = Osc.StringToOscMessage("/setBeamOn");
    oscHandler.Send(oscM);
  }
  void setBeamOff()
  {
    OscMessage oscM = Osc.StringToOscMessage("/setBeamOff");
    oscHandler.Send(oscM);
  }

  void playCubeDeath()
  {
    OscMessage oscM = Osc.StringToOscMessage("/playCubeDeath");
    oscHandler.Send(oscM);
  }

  void playExplosion()
  {
    OscMessage oscM = Osc.StringToOscMessage("/playExplosion");
    oscHandler.Send(oscM);
  }

  void startOutside()
  {
    OscMessage oscM = Osc.StringToOscMessage("/startOutside");
    oscHandler.Send(oscM);
  }
  void stopOutside()
  {
    OscMessage oscM = Osc.StringToOscMessage("/stopOutside");
    oscHandler.Send(oscM);
  }

  void startJumping()
  {
    OscMessage oscM = Osc.StringToOscMessage("/startJump");
    oscHandler.Send(oscM);
  }

  void gameOver()
  {
    OscMessage oscM = Osc.StringToOscMessage("/gameOver");
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
