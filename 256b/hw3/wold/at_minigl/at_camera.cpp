#include "at_camera.h"
#include "math.h"
#include <iostream>

aiVector3D ATCamera::ViewDir;
aiVector3D ATCamera::RightVector;	
aiVector3D ATCamera::UpVector;
aiVector3D ATCamera::Position;

MGLfloat ATCamera::RotatedX = 0;
MGLfloat ATCamera::RotatedY = 0;
MGLfloat ATCamera::RotatedZ = 0;

aiVector3D ATCamera::cross(aiVector3D v1, aiVector3D v2)
{
	aiVector3D result = aiVector3D(0.0,0.0,0.0);
	
	result.x = v1.y*v2.z - v2.y*v1.z;
	result.y = v1.z*v2.x - v1.x*v2.z; 
	result.z = v1.x*v2.y - v1.y*v2.x;
	return result;
}

aiVector3D ATCamera::normalize(aiVector3D v)
{
	float norm = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	
	if(norm==0)
		return v;
	v = v/norm;
	return v;
}

/***************************************************************************************/

void ATCamera::init(aiVector3D position, aiVector3D view, aiVector3D up)
{
	// TODO: compute right vector as the cross product of the up and 
	
	//Init with standard OGL values:
	Position = position;//aiVector3D (0.0, 0.0,	0.0);
	ViewDir = view;//aiVector3D( 0.0, 0.0, -1.0);
	UpVector = up;//aiVector3D (0.0, 1.0, 0.0);
	
	
	
	RightVector = aiVector3D (1.0, 0.0, 0.0);
	
	
	//RightVector = cross(normalize(ViewDir), normalize(UpVector));

	//Only to be sure:
	RotatedX = RotatedY = RotatedZ = 0.0;
}

void ATCamera::move (aiVector3D Direction)
{
	Position = Position + Direction;
}

void ATCamera::rotateX (MGLfloat Angle)
{
	RotatedX += Angle;
	
	//Rotate viewdir around the right vector:
	ViewDir = normalize(ViewDir*cos(Angle*PIdiv180)
								+ UpVector*sin(Angle*PIdiv180));

	//now compute the new UpVector (by cross product)
	UpVector = cross(ViewDir, RightVector)*-1;

	
}

void ATCamera::rotateY (MGLfloat Angle)
{
	RotatedY += Angle;
	
	//Rotate viewdir around the up vector:
	ViewDir = normalize(ViewDir*cos(Angle*PIdiv180)
								- RightVector*sin(Angle*PIdiv180));

	//now compute the new RightVector (by cross product)
	RightVector = cross(ViewDir, UpVector);
}

void ATCamera::rotateZ (MGLfloat Angle)
{
	RotatedZ += Angle;
	
	//Rotate viewdir around the right vector:
	RightVector = normalize(RightVector*cos(Angle*PIdiv180)
								+ UpVector*sin(Angle*PIdiv180));

	//now compute the new UpVector (by cross product)
	UpVector = cross(ViewDir, RightVector)*-1;
}


void ATCamera::setRotationX ( MGLfloat Angle )
{
	MGLfloat diff = -(RotatedX - Angle);
	RotatedX = Angle;
	
	
	
	
	//Rotate viewdir around the right vector:
	ViewDir = normalize(ViewDir*cos((diff)*PIdiv180)
						+ UpVector*sin((diff)*PIdiv180));
	
	//now compute the new UpVector (by cross product)
	UpVector = cross(ViewDir, RightVector)*-1*-1;
	
	
}
void ATCamera::setRotationY ( MGLfloat Angle )
{	
	MGLfloat diff = (RotatedY - Angle);
	RotatedY = Angle;
	
	//Rotate viewdir around the up vector:
	ViewDir = normalize(ViewDir*cos(diff*PIdiv180)
						- RightVector*sin(diff*PIdiv180));
	
	//now compute the new RightVector (by cross product)
	RightVector = cross(ViewDir, -UpVector);
	
}
void ATCamera::setRotationZ ( MGLfloat Angle )
{
	MGLfloat diff = (RotatedZ - Angle);
	RotatedZ = Angle;
	
	//Rotate viewdir around the right vector:
	RightVector = normalize(RightVector*cos(diff*PIdiv180)
							+ UpVector*sin(diff*PIdiv180));
	
	//now compute the new UpVector (by cross product)
	UpVector = cross(ViewDir, RightVector)*-1*-1;
}




void ATCamera::render( void )
{
	//mglMatrixMode(MGL_MODELVIEW);
	//mglLoadIdentity();
	

	//The point at which the camera looks:
	aiVector3D ViewPoint = Position+ViewDir;

	//as we know the up vector, we can easily use gluLookAt:
	mglLookAt(	Position.x,Position.y,Position.z,
				ViewPoint.x,ViewPoint.y,ViewPoint.z,
				UpVector.x,UpVector.y,UpVector.z);

}

void ATCamera::moveForward( MGLfloat Distance )
{
	Position = Position + (ViewDir*-Distance);
}

void ATCamera::strafeRight ( MGLfloat Distance )
{
	Position = Position + (RightVector*Distance);
}

void ATCamera::moveUpward( MGLfloat Distance )
{
	Position = Position + (UpVector*Distance);
}