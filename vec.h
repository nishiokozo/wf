// version 20170527 Metaball
// version 20170707 ray3
typedef	float	MAT3[3][3];
typedef	float	MAT4[4][4];

void	mat4_rotateX( float* m, float th );
void	mat4_rotateY( float* m, float th );
void	mat4_rotateZ( float* m, float th );


#define	pi	M_PI
class	ivec3
{
public:
	union
	{
		int	x;
		int	r;
		int	p1;
	};
	union
	{
		int	y;
		int	g;
		int	p2;
	};
	union
	{
		int	z;
		int	b;
		int	p3;
	};

};


typedef	struct
{
	float	x;	
	float	y;	
	float	z;	
	float	w;	
} vector;

class	vec2
{
public:

	union
	{
		float	x;	
		float	u;	
		float	s;	
		float	r;	
	};
	union
	{
		float	y;	
		float	v;	
		float	t;	
		float	g;	
	};

	vec2( const float ax, const float ay )
	{
		x = ax;
		y = ay;
	}
	vec2(){};

	vec2( float ax, float ay, float az )
	{
		x = ax;
		y = ay;
	}

	vec2& operator*=( const vec2& v ) 
	{
		x *= v.x;
		y *= v.y;
		
		return *this;
	}
	vec2& operator/=( const vec2& v ) 
	{
		x /= v.x;
		y /= v.y;
		
		return *this;
	}
	vec2& operator+=( const vec2& v ) 
	{
		x += v.x;
		y += v.y;
		
		return *this;
	}

	vec2& operator-=( const vec2& v ) 
	{
		x -= v.x;
		y -= v.y;
		
		return *this;
	}

	vec2 operator-() const
	{
		vec2	ret;
		ret.x = -x;
		ret.y = -y;

		return ret;
	}
	vec2 operator+() const
	{
		vec2	ret;
		ret.x = x;
		ret.y = y;

		return ret;
	}
	vec2 operator*( const float f ) const
	{
		vec2	ret;
		ret.x = x * f;
		ret.y = y * f;

		return ret;
	}

	vec2 operator/( const float f ) const
	{
		float a = 1.0f / f;
		vec2	ret;
		ret.x = x * a;
		ret.y = y * a;

		return ret;
	}

	vec2 operator*( const vec2& v ) const
	{
		vec2	ret;
		ret.x = x * v.x;
		ret.y = y * v.y;

		return ret;
	}

	vec2 operator-( const vec2& v ) const
	{
		vec2	ret;
		ret.x = x - v.x;
		ret.y = y - v.y;

		return ret;
	}

	vec2 operator+( const vec2& v ) const
	{
		vec2	ret;
		ret.x = x + v.x;
		ret.y = y + v.y;

		return ret;
	}

	friend	vec2 operator*( const float f, const vec2& v ) 
	{
		vec2	ret;
		ret.x = f * v.x;
		ret.y = f * v.y;
		
		return ret;
	}

	friend	vec2 operator+( const float f, const vec2& v ) 
	{
		vec2	ret;
		ret.x = f + v.x;
		ret.y = f + v.y;
		
		return ret;
	}


	void print()
	{
		printf("(vec2)%9.6f %9.6f\n", x, y );
	}

	void print( const char* str )
	{
		printf("%s%9.6f %9.6f", str, x, y );
	}

};


class vec3 : public vec2
{
//    friend class vec4;

public:
/*
	union
	{
		float	x;	
		float	r;	
	};
	union
	{
		float	y;	
		float	g;	
	};
*/
	union	{	float	z,b; };
	
	vec3(){};
	vec3( float a )
	{
		x = a;
		y = a;
		z = a;
	};

	vec3( float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	};
	vec3( const vec2& v, float _z )
	{
		x = v.x;
		y = v.y;
		z = _z;
	};

	vec3& operator*=( const vec3& v ) 
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		
		return *this;
	}

	vec3& operator/=( const vec3& v ) 
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		
		return *this;
	}
	vec3& operator+=( const vec3& v ) 
	{
		x += v.x;
		y += v.y;
		z += v.z;
		
		return *this;
	}

	vec3& operator-=( const vec3& v ) 
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		
		return *this;
	}

	vec3 operator-() const
	{
		vec3	ret;
		ret.x = -x;
		ret.y = -y;
		ret.z = -z;
		return ret;
	}
	vec3 operator+() const
	{
		vec3	ret;
		ret.x = x;
		ret.y = y;
		ret.z = z;
		return ret;
	}
	vec3 operator*( const float f ) const
	{
		vec3	ret;
		ret.x = x * f;
		ret.y = y * f;
		ret.z = z * f;
		return ret;
	}

	vec3 operator/( const float f ) const
	{
		float a = 1.0f / f;
		vec3	ret;
		ret.x = x * a;
		ret.y = y * a;
		ret.z = z * a;
		return ret;
	}

	vec3 operator*( const vec3& v ) const
	{
		vec3	ret;
		ret.x = x * v.x;
		ret.y = y * v.y;
		ret.z = z * v.z;
		return ret;
	}

	vec3 operator-( const vec3& v ) const
	{
		vec3	ret;
		ret.x = x - v.x;
		ret.y = y - v.y;
		ret.z = z - v.z;
		return ret;
	}

	vec3 operator+( const vec3& v ) const
	{
		vec3	ret;
		ret.x = x + v.x;
		ret.y = y + v.y;
		ret.z = z + v.z;
		return ret;
	}

	friend	vec3 operator*( const float f, const vec3& v ) 
	{
		vec3	ret;
		ret.x = f * v.x;
		ret.y = f * v.y;
		ret.z = f * v.z;
		
		return ret;
	}

	friend	vec3 operator+( const float f, const vec3& v ) 
	{
		vec3	ret;
		ret.x = f + v.x;
		ret.y = f + v.y;
		ret.z = f + v.z;
		
		return ret;
	}


	void print()
	{
		printf("(vec3)%9.6f %9.6f %9.6f\n", x, y, z );
	}

	void print( const char* str )
	{
		printf("%s<%9.6f %9.6f %9.6f>\n", str, x, y, z );
	}

};


#if 1
class vec4 : public vec3
{
public:
	union{		float	w,a;		};
#else
class vec4
{
public:
	union{		float	x,r;		};
	union{		float	y,g;		};
	union{		float	z,b;		};
	union{		float	w,a;		};
#endif

	vec4(){};
	vec4( float f )
	{
		x = f;
		y = f;
		z = f;
		w = f;
	};

	vec4( float ax, float ay, float az, float aw )
	{
		x = ax;
		y = ay;
		z = az;
		w = aw;
	};
	vec4( const vec3& v, float aw )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = aw;
	};
	vec4 operator+( const vec4& v )	const//	V + V
	{
		vec4	ret;
		ret.x = x + v.x;
		ret.y = y + v.y;
		ret.z = z + v.z;
		ret.w = w + v.w;
		return ret;
	}
	vec4 operator-( const vec4& v )	const //	V - V
	{
		vec4	ret;
		ret.x = x - v.x;
		ret.y = y - v.y;
		ret.z = z - v.z;
		ret.w = w - v.w;
		return ret;
	}
	vec4 operator*( const vec4& v )	const//	V * V
	{
		vec4	ret;
		ret.x = x * v.x;
		ret.y = y * v.y;
		ret.z = z * v.z;
		ret.w = w * v.w;
		return ret;
	}
	vec4 operator/( const vec4& v )	const//	V / V
	{
		vec4	ret;
		ret.x = x / v.x;
		ret.y = y / v.y;
		ret.z = z / v.z;
		ret.w = w / v.w;
		return ret;
	}

	friend	vec4 operator*( const float f, const vec4& v )	//	f * V
	{
		vec4	ret;
		ret.x = f * v.x;
		ret.y = f * v.y;
		ret.z = f * v.z;
		ret.w = f * v.w;
		
		return ret;
	}
/*

	vec4 operator*( const vec4& v ) const
	{
		vec4	ret;
		ret.x = x * v.x;
		ret.y = y * v.y;
		ret.z = z * v.z;
		ret.w = w * v.w;
		return ret;
	}
	vec4 operator=( const vec4& v )
	{
		x =  v.x;
		y =  v.y;
		z =  v.z;
		w =  v.w;
	}
	vec4& operator+=( const vec4& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		
		return *this;
	}
	vec4 operator*( const float f ) const
	{
		vec4	ret;
		ret.x = x * f;
		ret.y = y * f;
		ret.z = z * f;
		ret.w = w * f;
		return ret;
	}
	vec4 operator/( const float f ) const
	{
		vec4	ret;
		ret.x = x / f;
		ret.y = y / f;
		ret.z = z / f;
		ret.w = w / f;
		return ret;
	}


	friend	vec4 operator+( const float f, const vec4& v )
	{
		vec4	ret;
		ret.x = f + v.x;
		ret.y = f + v.y;
		ret.z = f + v.z;
		ret.w = f + v.w;
		
		return ret;
	}

	friend	vec4 operator-( const float f, const vec4& v )
	{
		vec4	ret;
		ret.x = f - v.x;
		ret.y = f - v.y;
		ret.z = f - v.z;
		ret.w = f - v.w;
		
		return ret;
	}

	friend	vec4 operator/( const float f, const vec4& v )
	{
		vec4	ret;
		ret.x = f / v.x;
		ret.y = f / v.y;
		ret.z = f / v.z;
		ret.w = f / v.w;
		
		return ret;
	}
*/


	void print()
	{
		printf("(vec4)%9.6f %9.6f %9.6f %9.6f\n", x, y, z, w );
	}

	void print( const char* str )
	{
		printf("%s<%9.6f %9.6f %9.6f %9.6f>\n", str, x, y, z, w );
	}

};

float	vec3_dotproduct( float a[3], float b[3] );

void	vec3_crossproduct( float v[3], float a[3], float b[3] );
vec3	       cross( const vec3& a, const vec3& b );

void	vec3_normalize( float v[3] );
void	vec3_normalize( vec3* v );
float	vec3_length( float v[3] );
float	vec3_length( vec3& v );

void	vec4_multiplyMatrix( float v[4], float m[16], float a[4] );

float	dot( const vec3& a, const vec3& b );

//float	dot( vec4& a, vec3& b );
//float	dot( vec3& a, vec4& b );
//float	dot( vec4& a, vec4& b );

vec3 refract( const vec3& a, const vec3& b, float n );
vec3 reflect( const vec3& I, const vec3& N );

//float	abs( float );
float	max( float a, float b );
vec3	max( float a, const vec3& b );
vec3	max( const vec3& b, float a );
float	min( float a, float b );
vec3	min( float a, const vec3& b );
vec3	min( const vec3& b, float a );

vec3	mix( float f, const vec3& a, const vec3& b );
float	mod( float a, float b );
float	length( const vec2& a );
float	length( const vec3& a );
float	length( const vec3& a, const vec3& b );
vec2	normalizXe( const vec2& a );
vec3	normalizXe( const vec3& a );
vec3	normalize( const vec3& a );


#define	RAD(n)	((n)*M_PI/180.0f)
#define	DEG(n)	((n)*180.0f/M_PI)
#define	M00	0
#define	M01	1
#define	M02	2
#define	M03	3
#define	M10	4
#define	M11	5
#define	M12	6
#define	M13	7
#define	M20	8
#define	M21	9
#define	M22	10
#define	M23	11
#define	M30	12
#define	M31	13
#define	M32	14
#define	M33	15

#define	rad(n)((n)*M_PI/180.0f)

//void	mat4_multiply( float* m, const float* a, const float* b );
void	mat4_multiply( float* m, const float* a, const float* y1 );

class	mat3
{
public:
	union
	{
		float	m_array[9];
		MAT3	m;
	};
	mat3()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;
	}

	mat3(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22
	)
	{
		m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
		m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
		m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
	}

	friend	vec3 operator*( const vec3& a, const mat3& m )
	{
		vec3 ret;
		ret.x = a.x*m.m[0][0] + a.y*m.m[1][0] + a.z*m.m[2][0];
		ret.y = a.x*m.m[0][1] + a.y*m.m[1][1] + a.z*m.m[2][1];
		ret.z = a.x*m.m[0][2] + a.y*m.m[1][2] + a.z*m.m[2][2];

		return ret;
	}

/*	friend	vec3& operator*=( vec3& v, mat3 m )
	{
		v = v * m;
	}
*/

	vec3 operator*( const vec3& a ) const
	{
		vec3 ret;
		ret.x = m[0][0]*a.x + m[0][1]*a.y + m[0][2]*a.z;
		ret.y = m[1][0]*a.x + m[1][1]*a.y + m[1][2]*a.z;
		ret.z = m[2][0]*a.x + m[2][1]*a.y + m[2][2]*a.z;
		return ret;
	}

	mat3 operator*( const mat3& m ) const
	{
		const MAT3& a = this->m;
		const MAT3& b = m.m;
		mat3 ret(
			a[0][0] * b[0][0] +  a[0][1] * b[1][0] +  a[0][2] * b[2][0],
			a[0][0] * b[0][1] +  a[0][1] * b[1][1] +  a[0][2] * b[2][1],
			a[0][0] * b[0][2] +  a[0][1] * b[1][2] +  a[0][2] * b[2][2],

			a[1][0] * b[0][0] +  a[1][1] * b[1][0] +  a[1][2] * b[2][0],
			a[1][0] * b[0][1] +  a[1][1] * b[1][1] +  a[1][2] * b[2][1],
			a[1][0] * b[0][2] +  a[1][1] * b[1][2] +  a[1][2] * b[2][2],

			a[2][0] * b[0][0] +  a[2][1] * b[1][0] +  a[2][2] * b[2][0],
			a[2][0] * b[0][1] +  a[2][1] * b[1][1] +  a[2][2] * b[2][1],
			a[2][0] * b[0][2] +  a[2][1] * b[1][2] +  a[2][2] * b[2][2]
		);
		return ret;
	}

	mat3& operator*=( const mat3& a ) 
	{
		*this = *this * a;
		
		return *this;
	}

	float* GetArray(){ return m_array; };

	void identity()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;
	}
	


	void rotXYZ( vec3 rot )
	{
		this->rotX( rot.x );
		this->rotY( rot.y );
		this->rotZ( rot.z );
	}
	void rotX( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat3(
			1,  0,  0,
			0,  c, -s,
			0,  s,  c
		);
	}
	void rotY( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat3(
			c,  0, -s,
			0,  1,  0,
			s,  0,  c
		);
	}
	void rotZ( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat3(
			c, -s,  0,
			s,	c,  0,
			0,  0,  1
		);
	}
/*
	vec3 RotateX( float f )
	{
//		MAT3& m = this->m;
		float	c = cos(f);
		float	s = sin(f);

		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	
		m[1][0] = 0.0f;	m[1][1] =   c;	m[1][2] =  -s;	
		m[2][0] = 0.0f;	m[2][1] =   s;	m[2][2] =   c;	
	}
	void rotY( float f )
	{
//		MAT3& m = this->m;
		float	c = cos(f);
		float	s = sin(f);

		m[0][0] =    c;	m[0][1] = 0.0f;	m[0][2] =  -s;	
		m[1][0] = 0.0f;	m[1][1] = 0.0f;	m[1][2] = 0.0f;	
		m[2][0] =    s;	m[2][1] = 0.0f;	m[2][2] =   c;	
	}
	void rotZ( float f )
	{
//		MAT3& m = this->m;
		float	c = cos(f);
		float	s = sin(f);

		m[0][0] =    c;	m[0][1] =   -s;	m[0][2] = 0.0f;	
		m[1][0] =    s;	m[1][1] =    c;	m[1][2] = 0.0f;	
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	
	}
*/

	void scale( float x, float y, float z )
	{
		m[0][0] *= x;
		m[1][1] *= y;
		m[2][2] *= z;
	}
	void scale( vec3 scale )
	{
		m[0][0] *= scale.x;
		m[1][1] *= scale.y;
		m[2][2] *= scale.z;
	}

	void print()
	{
		printf("mat3:\n");
		printf("%9.6f %9.6f %9.6f \n", m[0][0], m[0][1], m[0][2] );
		printf("%9.6f %9.6f %9.6f \n", m[1][0], m[1][1], m[1][2] );
		printf("%9.6f %9.6f %9.6f \n", m[2][0], m[2][1], m[2][2] );
	}
	void print( const char* str )
	{
		printf("mat3:%s\n", str);
		printf("%9.6f %9.6f %9.6f \n", m[0][0], m[0][1], m[0][2] );
		printf("%9.6f %9.6f %9.6f \n", m[1][0], m[1][1], m[1][2] );
		printf("%9.6f %9.6f %9.6f \n", m[2][0], m[2][1], m[2][2] );
	}

};

//---
class	mat4
{
public:
	union
	{
		float	m_array[16];
		MAT4	m;
	};
	mat4()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}

	mat4(
		float m00, float m01, float m02, float m03, 
		float m10, float m11, float m12, float m13, 
		float m20, float m21, float m22, float m23, 
		float m30, float m31, float m32, float m33 
	)
	{
		m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;	m[0][3] = m03;
		m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;	m[1][3] = m13;
		m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;	m[2][3] = m23;
		m[3][0] = m30;	m[3][1] = m31;	m[3][2] = m32;	m[3][3] = m33;
	}

	friend	vec4 operator*( const vec4& a, const mat4& m )
	{
		vec4 ret;
		ret.x = a.x*m.m[0][0] + a.y*m.m[1][0] + a.z*m.m[2][0] + a.w*m.m[3][0] ;
		ret.y = a.x*m.m[0][1] + a.y*m.m[1][1] + a.z*m.m[2][1] + a.w*m.m[3][1] ;
		ret.z = a.x*m.m[0][2] + a.y*m.m[1][2] + a.z*m.m[2][2] + a.w*m.m[3][2] ;
		ret.w = a.x*m.m[0][3] + a.y*m.m[1][3] + a.z*m.m[2][3] + a.w*m.m[3][3] ;

		return ret;
	}

	vec4 operator*( const vec4& a ) const
	{
		vec4 ret;
		ret.x = m[0][0]*a.x + m[0][1]*a.y + m[0][2]*a.z + m[0][3]*a.w ;
		ret.y = m[1][0]*a.x + m[1][1]*a.y + m[1][2]*a.z + m[1][3]*a.w ;
		ret.z = m[2][0]*a.x + m[2][1]*a.y + m[2][2]*a.z + m[2][3]*a.w ;
		ret.w = m[3][0]*a.x + m[3][1]*a.y + m[3][2]*a.z + m[3][3]*a.w ;
		return ret;
	}

	mat4 operator*( const mat4& m ) const
	{
		const MAT4& a = this->m;
		const MAT4& b = m.m;

		mat4 ret(
			a[0][0] * b[0][0] +  a[0][1] * b[1][0] +  a[0][2] * b[2][0] +  a[0][3] * b[3][0],
			a[0][0] * b[0][1] +  a[0][1] * b[1][1] +  a[0][2] * b[2][1] +  a[0][3] * b[3][1],
			a[0][0] * b[0][2] +  a[0][1] * b[1][2] +  a[0][2] * b[2][2] +  a[0][3] * b[3][2],
			a[0][0] * b[0][3] +  a[0][1] * b[1][3] +  a[0][2] * b[2][3] +  a[0][3] * b[3][3],

			a[1][0] * b[0][0] +  a[1][1] * b[1][0] +  a[1][2] * b[2][0] +  a[1][3] * b[3][0],
			a[1][0] * b[0][1] +  a[1][1] * b[1][1] +  a[1][2] * b[2][1] +  a[1][3] * b[3][1],
			a[1][0] * b[0][2] +  a[1][1] * b[1][2] +  a[1][2] * b[2][2] +  a[1][3] * b[3][2],
			a[1][0] * b[0][3] +  a[1][1] * b[1][3] +  a[1][2] * b[2][3] +  a[1][3] * b[3][3],

			a[2][0] * b[0][0] +  a[2][1] * b[1][0] +  a[2][2] * b[2][0] +  a[2][3] * b[3][0],
			a[2][0] * b[0][1] +  a[2][1] * b[1][1] +  a[2][2] * b[2][1] +  a[2][3] * b[3][1],
			a[2][0] * b[0][2] +  a[2][1] * b[1][2] +  a[2][2] * b[2][2] +  a[2][3] * b[3][2],
			a[2][0] * b[0][3] +  a[2][1] * b[1][3] +  a[2][2] * b[2][3] +  a[2][3] * b[3][3],

			a[3][0] * b[0][0] +  a[3][1] * b[1][0] +  a[3][2] * b[2][0] +  a[3][3] * b[3][0],
			a[3][0] * b[0][1] +  a[3][1] * b[1][1] +  a[3][2] * b[2][1] +  a[3][3] * b[3][1],
			a[3][0] * b[0][2] +  a[3][1] * b[1][2] +  a[3][2] * b[2][2] +  a[3][3] * b[3][2],
			a[3][0] * b[0][3] +  a[3][1] * b[1][3] +  a[3][2] * b[2][3] +  a[3][3] * b[3][3]
		);

		return	ret;
	}

/*
	mat4& operator=( const mat4& a ) 
	{
		*this = a;
		
		return *this;
	}
*/
	mat4& operator*=( const mat4& a ) 
	{
//		mat4_multiply( m_array, m_array, a.m_array );
		*this = *this * a;
		
		return *this;
	}

	float* GetArray(){ return m_array; };

	void identity()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}
	
	void SetTranslate( const vec3& pos )
	{
		m[3][0] = pos.x;
		m[3][1] = pos.y;
		m[3][2] = pos.z;
	}
	void SetTranslate( float x, float y, float z )
	{
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
	}
/*
	void Invers( const mat4& m )
	{
		mat4_invers( *this, m );
	}
*/
/*
	vec3 GetVecX()
	{
		return vec3( m[0][0], m[0][1], m[0][2] );
	}
	vec3 GetVecY()
	{
		return vec3( m[1][0], m[1][1], m[1][2] );
	}
	vec3 GetVecZ()
	{
		return vec3( m[2][0], m[2][1], m[2][2] );
	}
	vec3 GetVecT()
	{
		return vec3( m[3][0], m[3][1], m[3][2] );
	}
	void setVecX( vec3 v )
	{
		m[0][0] = v.x; 
		m[0][1] = v.y; 
		m[0][2] = v.z;
	}
	void setVecY( vec3 v )
	{
		m[1][0] = v.x; 
		m[1][1] = v.y; 
		m[1][2] = v.z;
	}
	void setVecZ( vec3 v )
	{
		m[2][0] = v.x; 
		m[2][1] = v.y; 
		m[2][2] = v.z;
	}
	void setVecT( vec3 v )
	{
		m[3][0] = v.x; 
		m[3][1] = v.y; 
		m[3][2] = v.z;
	}
*/

	//-----------------------------------------------------------------------------
	void setPerspective( float fovy, float aspect, float zNear, float zFar)
	//-----------------------------------------------------------------------------
	{
		float p = 1.0f / tanf(fovy *(M_PI/360.0));

		float	z2 = zFar;
		float	z1 = zNear;

		*this =  mat4(
			p / aspect, 0.0,	0.0,        				 0.0,
			0.0,        p,    	0.0,        				 0.0,
			0.0,        0.0,  	(z2+z1)         /(z1-z2), 	-1.0,
			0.0,    	0.0,	(2.0 * z2 * z1) /(z1-z2),   0.0
		);
	}

	//-----------------------------------------------------------------------------
	void lookAt( vec3 pos, vec3 at, vec3 up )
	//-----------------------------------------------------------------------------
	{
		vec3	z = normalizXe(at-pos);
		vec3	x = cross( up, z );
		vec3	y = cross( z, x );
	
		m[0][0] =x.x;	m[0][1] =x.y;	m[0][2] =x.z;	m[0][3] = 0.0f;
		m[1][0] =y.x;	m[1][1] =y.y;	m[1][2] =y.z;	m[1][3] = 0.0f;
		m[2][0] =z.x;	m[2][1] =z.y;	m[2][2] =z.z;	m[2][3] = 0.0f;
		m[3][0] =pos.x;	m[3][1] =pos.y;	m[3][2] =pos.z;	m[3][3] = 1.0f;
	}
	//-----------------------------------------------------------------------------
	void lookFor( vec3 pos, vec3 z, vec3 up )
	//-----------------------------------------------------------------------------
	{
		vec3	x = cross( up, z );
		vec3	y = cross( z, x );
	
		m[0][0] =x.x;	m[0][1] =x.y;	m[0][2] =x.z;	m[0][3] = 0.0f;
		m[1][0] =y.x;	m[1][1] =y.y;	m[1][2] =y.z;	m[1][3] = 0.0f;
		m[2][0] =z.x;	m[2][1] =z.y;	m[2][2] =z.z;	m[2][3] = 0.0f;
		m[3][0] =pos.x;	m[3][1] =pos.y;	m[3][2] =pos.z;	m[3][3] = 1.0f;
	}
	//-----------------------------------------------------------------------------
	void rotXYZ( vec3 rot )
	//-----------------------------------------------------------------------------
	{
		this->rotX( rot.x );
		this->rotY( rot.y );
		this->rotZ( rot.z );
	}
	//-----------------------------------------------------------------------------
	void rotX( float f)
	//-----------------------------------------------------------------------------
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat4(
			1,  0,  0,  0,
			0,  c, -s,  0,
			0,  s,  c,  0,
			0,  0,  0,  1
		);
	}
	//-----------------------------------------------------------------------------
	void rotY( float f)
	//-----------------------------------------------------------------------------
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat4(
			c,  0, -s,  0,
			0,  1,  0,  0,
			s,  0,  c,  0,
			0,  0,  0,  1
		);
	}
	//-----------------------------------------------------------------------------
	void rotZ( float f)
	//-----------------------------------------------------------------------------
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat4(
			c, -s,  0,  0,
			s,	c,  0,  0,
			0,  0,  1,  0,
			0,  0,  0,  1
		);
	}
	//-----------------------------------------------------------------------------
	void setPos( float x, float y, float z )
	//-----------------------------------------------------------------------------
	{
		m[0][3] = x;
		m[1][3] = y;
		m[2][3] = z;
	}
	//-----------------------------------------------------------------------------
	void trans( vec3 vec )
	//-----------------------------------------------------------------------------
	{
		m[3][0] += vec.x;
		m[3][1] += vec.y;
		m[3][2] += vec.z;
	}
	//-----------------------------------------------------------------------------
	void trans( float x, float y, float z )
	//-----------------------------------------------------------------------------
	{
		m[3][0] += x;
		m[3][1] += y;
		m[3][2] += z;
	}
	
	//-----------------------------------------------------------------------------
	void scale( float x, float y, float z )
	//-----------------------------------------------------------------------------
	{
		m[0][0] *= x;
		m[1][1] *= y;
		m[2][2] *= z;
	}
	//-----------------------------------------------------------------------------
	void scale( vec3 scale )
	//-----------------------------------------------------------------------------
	{
		m[0][0] *= scale.x;
		m[1][1] *= scale.y;
		m[2][2] *= scale.z;
	}

	//-----------------------------------------------------------------------------
	void print()
	//-----------------------------------------------------------------------------
	{
		printf("mat4:\n");
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[0][0], m[0][1], m[0][2], m[0][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[1][0], m[1][1], m[1][2], m[1][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[2][0], m[2][1], m[2][2], m[2][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[3][0], m[3][1], m[3][2], m[3][3] );
	}
	//-----------------------------------------------------------------------------
	void print( const char* str )
	//-----------------------------------------------------------------------------
	{
		printf("mat4:%s\n", str);
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[0][0], m[0][1], m[0][2], m[0][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[1][0], m[1][1], m[1][2], m[1][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[2][0], m[2][1], m[2][2], m[2][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[3][0], m[3][1], m[3][2], m[3][3] );
	}

};


void	mat4_Frustum( mat4& m, float l, float r, float b, float t, float n, float f );
void	mat4_Ortho( float* m, float l, float r, float b, float t, float n, float f );

int	mat4_print( mat4& m );
int	mat4_print( float* m );
void	mat4_frustum( float* m, float l, float r, float b, float t, float n, float f );
void	mat4_translate( mat4& m, float x, float y, float z );
void	mat4_translate( float* m, float x, float y, float z );
void	mat4_translate( float* m, vec3 vec );
void	mat4_translate( mat4& m, vec3 vec );
void	mat4_rotateX( mat4& m, float th );
void	mat4_rotateY( mat4& m, float th );
void	mat4_rotateZ( mat4& m, float th );
void	mat4_scaling( mat4& m, float sx, float sy, float sz  );
void	mat4_scaling( float* m, float sx, float sy, float sz  );
void	mat4_scaling( float* m, const vec3& vecScale  );
void	mat4_scaling( mat4& m, const vec3& vecScale  );
void	mat4_transpose( float* m, float a[16] );

void	mat4_copy( float* m, float a[16] );

void 	mat4_invers( mat4& a, const mat4& b );
//void	mat4_invers( float* out, float* in );
void	mat4_identity( float* m );
void	mat4_identity( mat4& m );

void	mat4_getTranslate( float* m, float x, float y, float z );
void	mat4_getRotateX( float* m, float th );
void	mat4_getRotateY( float* m, float th );
void	mat4_getRotateZ( float* m, float th );
void	mat4_getScale( float* m, float sx, float sy, float sz  );

vec4	mul( const vec4& a, const mat4& m );

float*	mat4_identity();

void 	mat4_glFrustumf( float* m,  float left, float right, float bottom, float top, float near, float far);
void 	mat4_perspective( mat4& m, float fovy, float aspect, float zNear, float zFar);
void 	mat4_perspective(float* m, float fovy, float aspect, float zNear, float zFar);
void 	mat4_invers( float* m, const float* in );
void	mat4_add( float* m, float* a, float* b );
void	mat4_sub( float* m, float* a, float* b );
void	mat4_div( float* m, float f );

// for raytrace
void mat4_ray_perspective(float* m, float fovy, float aspect );
void	mat4_otrho( float* m, float l, float r, float b, float t, float n, float f );

mat4	mat4_GetTranslate( float x, float y, float z );
mat4	mat4_GetRotateX( float th );
mat4	mat4_GetRotateY( float th );
mat4	mat4_GetRotateZ( float th );
