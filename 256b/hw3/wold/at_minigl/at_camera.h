 
#ifndef __AT_CAMERA_H__
#define __AT_CAMERA_H__


#include "assimp.h"
#include "at_minigl.h"

//#include <gl\glut.h>		// Need to include it here because the GL* types are required
#define PI 3.1415926535897932384626433832795
#define PIdiv180 (PI/180.0)

 
class ATCamera
{
	
public:
	
	
	static void init(aiVector3D position, aiVector3D view, aiVector3D up);		 
	static void render ( void );	 
							 

	static void move ( aiVector3D Direction );
	static void rotateX ( MGLfloat Angle );
	static void rotateY ( MGLfloat Angle );
	static void rotateZ ( MGLfloat Angle );
	
	static void setRotationX ( MGLfloat Angle );
	static void setRotationY ( MGLfloat Angle );
	static void setRotationZ ( MGLfloat Angle );

	static void moveForward ( MGLfloat Distance );
	static void moveUpward ( MGLfloat Distance );
	static void strafeRight ( MGLfloat Distance );
	
	static aiVector3D cross(aiVector3D v1, aiVector3D v2);
	static aiVector3D normalize(aiVector3D v);
	
private:
	
	ATCamera()
	{
		
	}
	
	static aiVector3D ViewDir;
	static aiVector3D RightVector;	
	static aiVector3D UpVector;
	static aiVector3D Position;
	
	static MGLfloat RotatedX, RotatedY, RotatedZ;	


};


#endif

