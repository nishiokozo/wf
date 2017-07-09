/* ------------------------------------------------------------------
 * triangel.c, Raspberry Pi, Open GL ES2 Sample
 * http://www.mztn.org/rpi/rpi10.html
 *
 * Copyright (C) 2012, Jun Mizutani All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 * ------------------------------------------------------------------ */

#include <stdio.h>
#include <memory.h>
#include <math.h>

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "bcm_host.h"

typedef struct 
{
	EGLNativeWindowType	nativeWin;
	EGLDisplay	display;
	EGLContext	context;
	EGLSurface	surface;
	EGLint		majorVersion;
	EGLint		minorVersion;
	int		 width;
	int		 height;
} ScreenSettings;


//-----------------------------------------------------------------------------
EGLBoolean ras_WinCreate(ScreenSettings *sc)
//-----------------------------------------------------------------------------
{
	int32_t success = 0;
	uint32_t width;
	uint32_t height;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;
	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	static EGL_DISPMANX_WINDOW_T nativewindow;
	VC_DISPMANX_ALPHA_T alpha = {DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 255, 0};

	success = graphics_get_display_size(0, &width, &height);
	if (success < 0) 
	{
		return EGL_FALSE;
	}
	
	sc->width = width;
	sc->height = height;

	vc_dispmanx_rect_set(&dst_rect, 0, 0, sc->width, sc->height);
	vc_dispmanx_rect_set(&src_rect, 0, 0, sc->width << 16, sc->height << 16);

	dispman_display = vc_dispmanx_display_open(0);
	dispman_update = vc_dispmanx_update_start(0);
	dispman_element = vc_dispmanx_element_add(
		dispman_update
		, dispman_display
		, 0
		, &dst_rect
		, 0
		, &src_rect
		, DISPMANX_PROTECTION_NONE
		, &alpha
		, 0
		, 0
	);
	vc_dispmanx_update_submit_sync(dispman_update);
	nativewindow.element = dispman_element;
	nativewindow.width = width;
	nativewindow.height = height;
	sc->nativeWin = &nativewindow;
	return EGL_TRUE;
}

//-----------------------------------------------------------------------------
EGLBoolean ras_SurfaceCreate(ScreenSettings *sc)
//-----------------------------------------------------------------------------
{
	EGLint attrib[] = 
	{
		EGL_RED_SIZE,	8,
		EGL_GREEN_SIZE,	8,
		EGL_BLUE_SIZE,	8,
		EGL_ALPHA_SIZE,	8,
		EGL_DEPTH_SIZE,	24,
		EGL_NONE
	};
	EGLint context[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
	EGLint numConfigs;
	EGLConfig config;

	sc->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (sc->display == EGL_NO_DISPLAY) 
	{
		return EGL_FALSE;
	}
	
	if (!eglInitialize(sc->display, &sc->majorVersion, &sc->minorVersion))
	{
		return EGL_FALSE;
	}
	
	if (!eglChooseConfig(sc->display, attrib, &config, 1, &numConfigs))
	{
		return EGL_FALSE;
	}
	
	sc->surface = eglCreateWindowSurface(sc->display, config, sc->nativeWin, NULL);
	if (sc->surface == EGL_NO_SURFACE) 
	{
		return EGL_FALSE;
	}
	
	sc->context = eglCreateContext(sc->display, config, EGL_NO_CONTEXT, context);
	if (sc->context == EGL_NO_CONTEXT)
	{
		return EGL_FALSE;
	}

	if (!eglMakeCurrent(sc->display, sc->surface, sc->surface, sc->context))
	{
		return EGL_FALSE;
	}
	return EGL_TRUE;
}

//-----------------------------------------------------------------------------
GLuint LoadShader(GLenum type, const char *shaderSource)
//-----------------------------------------------------------------------------
{
	GLuint shader;
	GLint compiled;

	shader = glCreateShader(type);
	if (shader == 0) return 0;

	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) 
	{ // compile error
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 0) 
		{
			char* infoLog = malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			printf("Error compiling shader:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}



typedef struct 
{
	GLint	 m_Pos;
//	GLint	 m_Tex;
	GLint	 m_Frame;
} ShaderParams;

typedef struct 
{
	GLfloat x, y, z;
//	GLfloat u, v;
} VertexType;

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
typedef	float	matrix[16];

//-----------------------------------------------------------------------------
void	matrix_multiply( matrix m, matrix a, matrix b )
//-----------------------------------------------------------------------------
{

	float	a00 = a[M00];
	float	a01 = a[M01];
	float	a02 = a[M02];
	float	a03 = a[M03];
	float	a10 = a[M10];
	float	a11 = a[M11];
	float	a12 = a[M12];
	float	a13 = a[M13];
	float	a20 = a[M20];
	float	a21 = a[M21];
	float	a22 = a[M22];
	float	a23 = a[M23];
	float	a30 = a[M30];
	float	a31 = a[M31];
	float	a32 = a[M32];
	float	a33 = a[M33];

	float	b00 = b[M00];
	float	b01 = b[M01];
	float	b02 = b[M02];
	float	b03 = b[M03];
	float	b10 = b[M10];
	float	b11 = b[M11];
	float	b12 = b[M12];
	float	b13 = b[M13];
	float	b20 = b[M20];
	float	b21 = b[M21];
	float	b22 = b[M22];
	float	b23 = b[M23];
	float	b30 = b[M30];
	float	b31 = b[M31];
	float	b32 = b[M32];
	float	b33 = b[M33];

	m[M00] = a00 * b00 +  a01 * b10 +  a02 * b20 +  a03 * b30;
	m[M01] = a00 * b01 +  a01 * b11 +  a02 * b21 +  a03 * b31;
	m[M02] = a00 * b02 +  a01 * b12 +  a02 * b22 +  a03 * b32;
	m[M03] = a00 * b03 +  a01 * b13 +  a02 * b23 +  a03 * b33;

	m[M10] = a10 * b00 +  a11 * b10 +  a12 * b20 +  a13 * b30;
	m[M11] = a10 * b01 +  a11 * b11 +  a12 * b21 +  a13 * b31;
	m[M12] = a10 * b02 +  a11 * b12 +  a12 * b22 +  a13 * b32;
	m[M13] = a10 * b03 +  a11 * b13 +  a12 * b23 +  a13 * b33;

	m[M20] = a20 * b00 +  a21 * b10 +  a22 * b20 +  a23 * b30;
	m[M21] = a20 * b01 +  a21 * b11 +  a22 * b21 +  a23 * b31;
	m[M22] = a20 * b02 +  a21 * b12 +  a22 * b22 +  a23 * b32;
	m[M23] = a20 * b03 +  a21 * b13 +  a22 * b23 +  a23 * b33;

	m[M30] = a30 * b00 +  a31 * b10 +  a32 * b20 +  a33 * b30;
	m[M31] = a30 * b01 +  a31 * b11 +  a32 * b21 +  a33 * b31;
	m[M32] = a30 * b02 +  a31 * b12 +  a32 * b22 +  a33 * b32;
	m[M33] = a30 * b03 +  a31 * b13 +  a32 * b23 +  a33 * b33;
}
//-----------------------------------------------------------------------------
void	matrix_getTranslate( float* m, float x, float y, float z )
//-----------------------------------------------------------------------------
{
	m[M00] = 1.0f;	m[M01] = 0.0f;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] = 1.0f;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] = 1.0f;	m[M23] = 0.0f;
	m[M30] =   x;	m[M31] =   y;	m[M32] =   z;	m[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
void	matrix_getRotateX( matrix m, float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	m[M00] = 1.0f;	m[M01] = 0.0f;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] =   c;	m[M12] =  -s;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] =   s;	m[M22] =   c;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;
}

//-----------------------------------------------------------------------------
void	matrix_getRotateY( matrix m, float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	m[M00] =   c;	m[M01] = 0.0f;	m[M02] =  -s;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] = 1.0f;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] =   s;	m[M21] = 0.0f;	m[M22] =   c;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
void	matrix_getRotateZ( matrix m, float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	m[M00] =   c;	m[M01] =   -s;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] =   s;	m[M11] =    c;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] = 1.0f;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;
}

//-----------------------------------------------------------------------------
void	matrix_rotateX( matrix m, float th )
//-----------------------------------------------------------------------------
{
	matrix a;
	matrix_getRotateX(a,th);

	matrix_multiply( m, m, a );
}
//-----------------------------------------------------------------------------
void	matrix_rotateY( matrix m, float th )
//-----------------------------------------------------------------------------
{
	matrix a;
	matrix_getRotateY(a,th);

	matrix_multiply( m, m, a );
}
//-----------------------------------------------------------------------------
void	matrix_rotateZ( matrix m, float th )
//-----------------------------------------------------------------------------
{
	matrix a;
	matrix_getRotateZ(a,th);

	matrix_multiply( m, m, a );
}


//-----------------------------------------------------------------------------
int main ( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	unsigned int frames = 0;
	int res;

	ScreenSettings	g_sc;
	ShaderParams	g_sp;

	GLuint g_vbo;
	GLuint g_ibo;
	GLuint shader_program;
	GLuint m_hdlMat;	  // シェーダーMatのハンドル


	float m_matWorld[16];			// ワールド行列
	float m_matView[16];
	float m_matProj[16];
	float m_matModel[16];


float m_matIdentity[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};


	bcm_host_init();
	res = ras_WinCreate(&g_sc);
	if (!res) return 0;
	res = ras_SurfaceCreate(&g_sc);
	if (!res) return 0;


	{
		shader_program = glCreateProgram();
		if (shader_program == 0) return 0;
	
		{
			const char src[] =
				"uniform	mat4	Mat;	  					\n"
				"attribute	vec3	Pos;						\n"
				"void main()									\n"
				"{												\n"
//			  "   gl_Position = Mat * vec4(Pos,1);   				\n"
			  "   gl_Position = vec4( Pos.x, Pos.y,0,1);   				\n"
				"} 												\n"
			;
			GLuint sdr = LoadShader(GL_VERTEX_SHADER, src);
			glAttachShader(shader_program, sdr);
			glDeleteShader(sdr);
		}

		{
			const char src[] =
//				"precision mediump float;						\n"
				"void main()									\n"
				"{				 								\n"
				"	gl_FragColor = vec4(1,1, 0.5, 1.0);			\n"
				"}												\n"
			;
			GLuint sdr = LoadShader(GL_FRAGMENT_SHADER, src);
			glAttachShader(shader_program, sdr);
			glDeleteShader(sdr);
		}
		
		glLinkProgram(shader_program);

		{
			GLint	hdl;
			glGetProgramiv (shader_program, GL_LINK_STATUS, &hdl);
			if (!hdl) 
			{ // error
				{
					GLint infoLen = 0;
					glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &infoLen);
					if (infoLen > 0) 
					{
						char* infoLog = malloc(sizeof(char) * infoLen);
						glGetProgramInfoLog(shader_program, infoLen, NULL, infoLog);
						printf("Error linking program:\n%s\n", infoLog);
						free ( infoLog );
					}
				}
				glDeleteProgram (shader_program);
				return 0;
			}
		}

	}

//	createBuffer();
	{
		{// vertex buffer
			glGenBuffers(1, &g_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
			{
				VertexType tbl[] = 
				{
					{.x = -0.5f, .y = -0.5f, .z = 0.0f},
					{.x =  0.5f, .y = -0.5f, .z = 0.0f},
					{.x =  0.0f, .y =  0.5f, .z = 0.0f},
				};
				glBufferData(GL_ARRAY_BUFFER, sizeof(tbl), tbl, GL_STATIC_DRAW);
			}
		}

		{// index buffer
			glGenBuffers(1, &g_ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
			{
				unsigned int tbl[] = 
				{
					0, 1,2,
//					1, 2,
//					2, 0,
				};
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tbl), tbl, GL_STATIC_DRAW);
			}
		}
	}


		// ハンドル( ポインタ )の取得
		m_hdlMat = glGetUniformLocation( shader_program, "Mat" );
	g_sp.m_Pos = glGetAttribLocation(shader_program, "Pos");

	glClearColor(0.0f, 0.6f, 0.75f, 1.0f);

	while(1) 
	{
		glViewport(0, 0, g_sc.width, g_sc.height);
		glClear(GL_COLOR_BUFFER_BIT);
	//	Draw();

			// ワールド行列に対して回転をかける
//			Matrix.setIdentityM( m_matWorld, 0 );  // 単位行列でリセット
			memcpy( m_matWorld, m_matIdentity, sizeof(float[16]) );
//			Matrix.rotateM( m_matWorld, 0, rot, 0.0f, 1.0f, 1.0f );  // 回転行列
			matrix_rotateX( m_matWorld, 1.0 );
			matrix_rotateY( m_matWorld, 1.0 );

			// カメラ( ビュー行列 )を設定
			{

//				Matrix.setIdentityM( m_matView, 0 );
	//			m_matView = m_matIdentity;
				memcpy( m_matView, m_matIdentity, sizeof(float[16]) );
//				Matrix.translateM( m_matView, 0, 0.0f, 0.0f, -4.0f );
				matrix_getTranslate( m_matView, 0, 0, -4 );
			}

			// ワールド行列×ビュー行列×射影行列をセット
			{
//				Matrix.multiplyMM( m_matModel, 0, m_matView, 0, m_matWorld, 0 );
				matrix_multiply( m_matModel, m_matView, m_matWorld );
//				Matrix.multiplyMM( m_matModel, 0, m_matProj, 0, m_matModel, 0 );
				matrix_multiply( m_matModel, m_matProj, m_matModel );
//				glUniformMatrix4fv(dif_hdl_mat_camera, 1, GL_FALSE, mat );

				memcpy( m_matModel, m_matIdentity, sizeof(float[16]) );



				glUniformMatrix4fv( m_hdlMat         , 1, GL_FALSE, m_matModel);

			}

		{
			glUseProgram( shader_program );
			glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
			glEnableVertexAttribArray(g_sp.m_Pos);
////			glVertexAttribPointer(g_sp.m_Pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
			glVertexAttribPointer(g_sp.m_Pos, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
//			glVertexAttribPointer(g_sp.m_Pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);
//			glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		}
		eglSwapBuffers(g_sc.display, g_sc.surface);
	}
	return 0;
}
