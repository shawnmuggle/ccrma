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

public class JitCustomEvents : MonoBehaviour {
	
  public Rigidbody Dog;
	
  private int stop_spawn;

  void Start() {
    //This is simply to prevent infinite loop of spawning and destruction of objects
    //Do one or the other
    stop_spawn = 0;	
  }

  // You can define here various custom functions that can be called from Max
  // These can affect any object
  public void run(GameObject target, int varA, float[] varB) {
    if (varA != 0) {
      switch (varA) {
				
	// Parse according to requested event ID
      case 1:
	changeColor(target, varB);
	break;
      case 2:
	changeMaterial(target, varB);
	break;
      case 3:
	raining_cats();
	break;
      case 4:
	raining_dogs();
	break;
      case 5:
	destroy_all_cats_n_dogs();
	break;
      }
    }
  }
	
  // Custom functions
  // In this case changeColor changes color of the object given RGB values
  // changeMaterial changes object's material
  void changeColor(GameObject tgt, float[] val) {
		
    // It is always a good idea to check sanity of the arguments
    if (val.Length == 3) {
			
      // Create color from arguments and apply it
      Color tmp = new Color(val[0]/255.0f, val[1]/255.0f, val[2]/255.0f);
      tgt.renderer.material.color = tmp;
    }
  }
	
  void changeMaterial(GameObject tgt, float[] val) {
		
    //Once again sanity check
    if (val.Length == 1) {
			
      if (val[0] == 1.0f) tgt.renderer.material.shader = Shader.Find ("Diffuse");
      else tgt.renderer.material.shader = Shader.Find ("Transparent/Diffuse");	
    }	
  }
	
  void raining_cats() {
    //Cats = spheres
    //this is a redundant implementation designed to link with custom_ball_script
    //written in Javascript. Ideally you would include its behavior in here. See
    //raining_dogs example for more info
    if (stop_spawn==0) SendMessage("Create_Cat");
    //this broadcasts the function to all assets associated with this gameObject
    //if no such asset is found, Unity3D will post errors to the Debug.Log	
  }
	
  void raining_dogs() {
    //Dogs = cubes
    //Here we do the call the right way, from this script, rather than broadcasting
    //a message to every asset that belongs to the main camera
    if (stop_spawn==0) {
      Vector3 pos = new Vector3((float)0.6568305, (float)4.370027, (float)2.624699);
      Rigidbody instance = (Rigidbody) Instantiate(Dog, pos, Quaternion.identity);
      instance.AddForce((float)(Random.value-0.5)*4, (float)(Random.value-0.5)*4, (float)(Random.value-0.5)*4);
      Color tmp = new Color((float)Random.value,(float)Random.value,(float)Random.value);
      instance.renderer.material.color = tmp;
    }
  }
	
  void destroy_all_cats_n_dogs() {
    //Disable spawning of new assets
    stop_spawn = 1;
    GameObject target;
    //First cats
    search_n_destroy("Cat");
    //Then dogs
    search_n_destroy("Dog");
    stop_spawn = 0;
  }
	
  void search_n_destroy(string tgt) {
    GameObject target = GameObject.FindWithTag(tgt);
    while (target != null) {
      target.tag = "Destroy";
      target = GameObject.FindWithTag(tgt);
    }
  }
}
