#include "Tools.h"


CScreen::CScreen()
{
	Depth = 32;
	Height = 480;
	Middle.x = 320;
	Middle.y = 240;
	Width = 640;
}

CScreen::~CScreen()
{
}

CVector3::CVector3( void )
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

CVector3::CVector3( float x_, float y_, float z_ )
{
    x = x_;
    y = y_;
    z = z_;
}

CVector3::~CVector3( void )
{
}

void CVector3::set( float x_, float y_, float z_ )
{
    x = x_;
    y = y_;
    z = z_;
}

float CVector3::length( void )
{
    return (float)sqrt( x * x + y * y + z * z );
}

void CVector3::normalize( void )
{
	float fLength = length();

	x = x / fLength;
	y = y / fLength;
	z = z / fLength;
}

//
// Operators...
//

CVector3 CVector3::operator + ( const CVector3 &other )
{
    CVector3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x + other.x;
    vResult.y = y + other.y;
    vResult.z = z + other.z;

    return vResult;
}

CVector3 CVector3::operator + ( void ) const
{
    return *this;
}

CVector3 CVector3::operator - ( const CVector3 &other )
{
    CVector3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x - other.x;
    vResult.y = y - other.y;
    vResult.z = z - other.z;

    return vResult;
}

CVector3 CVector3::operator - ( void ) const
{
    CVector3 vResult(-x, -y, -z);

    return vResult;
}

CVector3 CVector3::operator * ( const CVector3 &other )
{
    CVector3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x * other.x;
    vResult.y = y * other.y;
    vResult.z = z * other.z;

    return vResult;
}

CVector3 CVector3::operator * ( const float scalar )
{
    CVector3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x * scalar;
    vResult.y = y * scalar;
    vResult.z = z * scalar;

    return vResult;
}

CVector3 operator * ( const float scalar, const CVector3 &other )
{
    CVector3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = other.x * scalar;
    vResult.y = other.y * scalar;
    vResult.z = other.z * scalar;

    return vResult;
}

CVector3 CVector3::operator / ( const CVector3 &other )
{
    CVector3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x / other.x;
    vResult.y = y / other.y;
    vResult.z = z / other.z;

    return vResult;
}

CVector3 CVector3::operator / ( const float scalar )
{
    CVector3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x / scalar;
    vResult.y = y / scalar;
    vResult.z = z / scalar;

    return vResult;
}

CVector3& CVector3::operator = ( const CVector3 &other )
{
	x = other.x;
	y = other.y;
	z = other.z;

    return *this;
}

CVector3& CVector3::operator += ( const CVector3 &other )
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

CVector3& CVector3::operator -= ( const CVector3 &other )
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

bool CVector3::operator == (const CVector3 &other)
{
	return (this->x == other.x) && (this->y == other.y) && (this->z == other.z);
}

bool CVector3::operator != (const CVector3 &other)
{
	return (this->x != other.x) || (this->y != other.y) || (this->z != other.z);
}

//
// Static utility methods...
//

float CVector3::distance( const CVector3 &v1,  const CVector3 &v2  )
{
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	float dz = v1.z - v2.z;

	return (float)sqrt( dx * dx + dy * dy + dz * dz );
}

float CVector3::dotProduct( const CVector3 &v1,  const CVector3 &v2 )
{
	return( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z  );
}

CVector3 CVector3::crossProduct( const CVector3 &v1,  const CVector3 &v2 )
{
	CVector3 vCrossProduct;

    vCrossProduct.x = v1.y * v2.z - v1.z * v2.y;
    vCrossProduct.y = v1.z * v2.x - v1.x * v2.z;
    vCrossProduct.z = v1.x * v2.y - v1.y * v2.x;

	return vCrossProduct;
}


char *ExtractFileExt(char *Filename)
{
	if(!Filename)
		return "";

    unsigned int d = strlen(Filename);

	if(d == 0)
	    return "";

    while(Filename[d] != '.' && d > 0)
    {
	    d--;
    }

    if(d <= 0)
	    return Filename;

	d++;

    char buffer[255] = "";
	sprintf(buffer, "%s", Filename);

    Filename = (char *)malloc(strlen(Filename) - d);

    for(unsigned int i = d, j = 0; i < strlen(Filename); i += 1, j += 1)
    {
	    Filename[j] = buffer[i];
    }
    Filename[strlen(Filename) - d + 1] = '\0';

    return Filename;
}

char *ExtractFilename(char *Filename)
{
	if(strcmp(Filename, "") == 0)
		return "";

	int d = strlen(Filename);
	while(Filename[d] != '\\' && d > 0)
	{
		d--;
	}

	if(d <= 0)
		return Filename;

	d++;

	char *buffer = "";
	buffer = (char *)malloc(strlen(Filename) - d);

	for(unsigned int i = d; i < strlen(Filename); i += 1)
	{
		buffer[i - d] = Filename[i];
	}
	buffer[strlen(buffer) + 1] = '\0';

	return buffer;
}

void readline(FILE *f, char *string)
{
    do
    {
	    fgets(string, 255, f);
    } while ((string[0] == '/') ||  (string[0] == '\n'));

    return;
}

void readstr(FILE *f, char *string)
{
    do
    {
	    fgets(string, 255, f);
    } while (/*(string[0] == '/') ||*/ (string[0] == '\n'));

    return;
}

char *RemoveFileExt(char *Filename)
{
    if(strcmp(Filename, "") == 0)
	    return "";

    int d = strlen(Filename);
    while(Filename[d] != '.' && d > 0)
    {
	    d--;
    }

    if(d <= 0)
	    return Filename;

    char *buffer = "";
    buffer = (char *)malloc(d);
    strncpy(buffer, Filename, d);

    Filename = (char *)malloc(d);
    strncpy(Filename, buffer, d);

    for(int i = 0; i < d; i += 1)
    {
	    Filename[i] = buffer[i];
    }
    Filename[d] = '\0';

    return Filename;
}

bool StringCompare(std::string s1, std::string s2)
{
	if(s1.size() != s2.size())
		return false;

	if(s1 != s2)
		return false;

	return true;
}

bool StringCompareI(const std::string & s1, const std::string& s2)
{
	if(s1.size() != s2.size())
		return false;

	std::string::const_iterator it1 = s1.begin();
	std::string::const_iterator it2 = s2.begin();

	while( (it1 != s1.end()) && (it2 != s2.end()) )
	{
		if(::toupper(*it1) != ::toupper(*it2)) // letters differ?
			return false;

		++it1;
		++it2;
	}

	return true;

}
