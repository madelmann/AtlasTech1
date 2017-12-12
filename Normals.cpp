#include "Normals.h"


CVector3 Normalize(CVector3 vVector)
{
	// Get the magnitude of our normal
	float magnitude = (float)sqrt( (vVector.x * vVector.x) + 
								   (vVector.y * vVector.y) + 
								   (vVector.z * vVector.z) );				

	// Now that we have the magnitude, we can divide our vector by that magnitude.
	// That will make our vector a total length of 1.  
	vVector = vVector / magnitude;		
	
	// Finally, return our normalized vector
	return vVector;										
}


void GetNormal(CVector3 v[3], CVector3 *normal)
{
    CVector3 a, b;

    // calculate the vectors A and B
    // note that v[3] is defined with counterclockwise winding in mind
    // a
    a.x = v[0].x - v[1].x;
    a.y = v[0].y - v[1].y;
    a.z = v[0].z - v[1].z;
    // b
    b.x = v[1].x - v[2].x;
    b.y = v[1].y - v[2].y;
    b.z = v[1].z - v[2].z;

    // calculate the cross product and place the resulting vector
    // into the address specified by vertex_t *normal

    normal->x = (a.z * b.y) - (a.y * b.z);
    normal->z = (a.y * b.x) - (a.x * b.y);
    normal->y = (a.x * b.z) - (a.z * b.x);

    // set vector to lengh 1
    //*normal = Normalize(*normal);
}