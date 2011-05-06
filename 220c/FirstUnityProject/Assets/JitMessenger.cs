// mu (myu) Max-Unity Interoperability Toolkit
// Ivica Ico Bukvic <ico@vt.edu> <http://ico.bukvic.net>
// Ji-Sun Kim <hideaway@vt.edu>
// Keith Wooldridge <kawoold@vt.edu>
// With thanks to Denis Gracanin

// Virginia Tech Department of Music
// DISIS Interactive Sound & Intermedia Studio
// Collaborative for Creative Technologies in the Arts and Design

// Copyright DISIS 2008.
// mu is distributed under the GPL license v3 (http://www.gnu.org/licenses/gpl.html)

using UnityEngine;
using System.Collections;

public class JitMessenger : MonoBehaviour {

  public GameObject send;
  private JitSend writer;
  private bool valid = false;

  // Use this for initialization
  void Start () {
    if (send == null) send = GameObject.Find("Main Camera");
    if (send != null) writer = (JitSend)send.GetComponent("JitSend");
    if (writer != null) valid = true;
  }
	
  // Update is called once per frame
  // This is where you should forward info as needed
  void Update () {

    if (valid) {
			
      //It is recommended to use this script with rigid bodies as that allows for
      //easy monitoring of object's activity and consequently its need to send state data
      if (!rigidbody.IsSleeping()) {
	float[] tmp = {transform.eulerAngles.x, transform.eulerAngles.y, transform.eulerAngles.z};
	write('r', tmp);
      }
    }
  }
	
  private void write(char method, float[] var) {
    string toWrite = name + " " + method;
    for(int i = 0; i < var.Length; i++){
      toWrite += " " + var[i];
    }
    toWrite += ";\n";
    writer.write(toWrite);
  }
}
