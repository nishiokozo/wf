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
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <math.h>
#include "bcm_host.h"
#include "vec.h"

typedef struct {
  EGLNativeWindowType  nativeWin;
  EGLDisplay  display;
  EGLContext  context;
  EGLSurface  surface;
  EGLint      majorVersion;
  EGLint      minorVersion;
  int         width;
  int         height;
} ScreenSettings;

const char vShaderSrc[] =
 "uniform	mat4	Mat;	  					\n"
  "attribute vec3  aPosition;\n"
//  "attribute vec2  aTex;     \n"
//  "varying   vec2  vTex;\n"
//  "uniform   float uFrame;\n"
  "void main()        \n"
  "{                  \n"
//  "  vTex = aTex;     \n"
//  "  gl_Position = vec4(aPosition.x - uFrame, aPosition.y + uFrame, 0, 1);\n"
//  "  gl_Position = vec4(aPosition.x , aPosition.y, 0, 1);\n"
  "   gl_Position = Mat * vec4(aPosition,1);   				\n"
  "}                  \n";

const char fShaderSrc[] =
  "precision mediump float;\n"\
//  "varying   vec2  vTex;\n"
  "void main()        \n"
  "{                  \n"
//  "  gl_FragColor = vec4(vTex.y, vTex.x, 0.5, 1.0);\n"
  "  gl_FragColor = vec4(1,1, 0.5, 1.0);\n"
  "}                  \n";

typedef struct {
  GLint   aPosition;
//  GLint   aTex;
//  GLint   uFrame;
} ShaderParams;

typedef struct {
    GLfloat x, y, z;
//    GLfloat u, v;
} VertexType;

VertexType vObj[] = {
  {.x = -0.5f, .y = -0.5f, .z = 0.0f},//, .u = 0.0f, .v = 1.0f},
  {.x =  0.5f, .y = -0.5f, .z = 0.0f},//, .u = 1.0f, .v = 1.0f},
  {.x =  0.0f, .y =  0.5f, .z = 0.0f},//, .u = 0.5f, .v = 0.0f},
};

unsigned short iObj[] = {
  0, 1, 2
};

ShaderParams    g_sp;
ScreenSettings  g_sc;

GLuint g_vbo;
GLuint g_ibo;
GLuint g_program;

EGLBoolean WinCreate(ScreenSettings *sc)
{
  uint32_t success = 0;
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
  if (success < 0) return EGL_FALSE;

  sc->width = width;
  sc->height = height;

  vc_dispmanx_rect_set(&dst_rect, 0, 0, sc->width, sc->height);
  vc_dispmanx_rect_set(&src_rect, 0, 0, sc->width << 16, sc->height << 16);

  dispman_display = vc_dispmanx_display_open(0);
  dispman_update = vc_dispmanx_update_start(0);
  dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display,
             0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, &alpha, 0, DISPMANX_NO_ROTATE);
  vc_dispmanx_update_submit_sync(dispman_update);
  nativewindow.element = dispman_element;
  nativewindow.width = width;
  nativewindow.height = height;
  sc->nativeWin = &nativewindow;
  return EGL_TRUE;
}

EGLBoolean SurfaceCreate(ScreenSettings *sc)
{
  EGLint attrib[] = {
    EGL_RED_SIZE,       8,
    EGL_GREEN_SIZE,     8,
    EGL_BLUE_SIZE,      8,
    EGL_ALPHA_SIZE,     8,
    EGL_DEPTH_SIZE,     24,
    EGL_NONE
  };
  EGLint context[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
  EGLint numConfigs;
  EGLConfig config;

  sc->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (sc->display == EGL_NO_DISPLAY) return EGL_FALSE;
  if (!eglInitialize(sc->display, &sc->majorVersion, &sc->minorVersion))
    return EGL_FALSE;
  if (!eglChooseConfig(sc->display, attrib, &config, 1, &numConfigs))
    return EGL_FALSE;
  sc->surface = eglCreateWindowSurface(sc->display, config, sc->nativeWin, NULL);
  if (sc->surface == EGL_NO_SURFACE) return EGL_FALSE;
  sc->context = eglCreateContext(sc->display, config, EGL_NO_CONTEXT, context);
  if (sc->context == EGL_NO_CONTEXT) return EGL_FALSE;
  if (!eglMakeCurrent(sc->display, sc->surface, sc->surface, sc->context))
      return EGL_FALSE;
  return EGL_TRUE;
}

GLuint LoadShader(GLenum type, const char *shaderSource)
{
  GLuint shader;
  GLint compiled;

  shader = glCreateShader(type);
  if (shader == 0) return 0;

  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) { // compile error
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 0) {
      char* infoLog = (char*)malloc(sizeof(char) * infoLen);
      glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
      printf("Error compiling shader:\n%s\n", infoLog);
      free(infoLog);
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

int InitShaders(GLuint *program, char const *vShSrc, char const *fShSrc)
{
  GLuint vShader;
  GLuint fShader;
  GLint  linked;
  GLuint prog;

  vShader = LoadShader(GL_VERTEX_SHADER, vShSrc);
  fShader = LoadShader(GL_FRAGMENT_SHADER, fShSrc);

  prog = glCreateProgram();
  if (prog == 0) return 0;

  glAttachShader(prog, vShader);
  glAttachShader(prog, fShader);
  glLinkProgram(prog);
  glGetProgramiv (prog, GL_LINK_STATUS, &linked);
  if (!linked) { // error
    GLint infoLen = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 0) {
      char* infoLog = (char*)malloc(sizeof(char) * infoLen);
      glGetProgramInfoLog(prog, infoLen, NULL, infoLog);
      printf("Error linking program:\n%s\n", infoLog);
      free ( infoLog );
    }
    glDeleteProgram (prog);
    return GL_FALSE;
  }
  glDeleteShader(vShader);
  glDeleteShader(fShader);

  *program = prog;
  return GL_TRUE;
}

void createBuffer()
{
  glGenBuffers(1, &g_vbo);
  // vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vObj), vObj, GL_STATIC_DRAW);

  // index buffer
  glGenBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iObj), iObj, GL_STATIC_DRAW);
}


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
void	matrix_identity( matrix m )
//-----------------------------------------------------------------------------
{
	m[M00] = 1.0f;	m[M01] = 0.0f;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] = 1.0f;	m[M12] = 0.0f;	m[M13] = 0.0f;
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
void matrix_perspective (matrix m, float fovy, float aspect, float zNear, float zFar)
//-----------------------------------------------------------------------------
{
        float f = 1.0f / tanf(fovy * (M_PI/360.0));

        m[0] = f / aspect;
        m[1] = 0.0;
        m[2] = 0.0;
        m[3] = 0.0;

        m[4] = 0.0;
        m[5] = f;
        m[6] = 0.0;
        m[7] = 0.0;

        m[8] = 0.0;
        m[9] = 0.0;
        m[10] = (zFar + zNear) / (zNear - zFar);
        m[11] = -1.0;

        m[12] = 0.0;
        m[13] = 0.0;
        m[14] = 2.0 * zFar * zNear / (zNear - zFar);
        m[15] = 0.0;
}


//-----------------------------------------------------------------------------
int main ( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	GLuint m_hdlMat;	  // シェーダーMatのハンドル

  unsigned int frames = 0;
  int res;

  bcm_host_init();
  res = WinCreate(&g_sc);
  if (!res) return 0;
  res = SurfaceCreate(&g_sc);
  if (!res) return 0;
  res = InitShaders(&g_program, vShaderSrc, fShaderSrc);
  if (!res) return 0;

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



  createBuffer();

		m_hdlMat = glGetUniformLocation( g_program, "Mat" );
  g_sp.aPosition = glGetAttribLocation(g_program, "aPosition");
//  g_sp.aTex = glGetAttribLocation(g_program, "aTex");
//  g_sp.uFrame  = glGetUniformLocation(g_program, "uFrame");

  glClearColor(0.0f, 0.3f, 0.0f, 0.5f);

  /* 600frame / 60fps = 10sec */ 
  while(frames < 600) 
  {
    glViewport(0, 0, g_sc.width, g_sc.height);
    glClear(GL_COLOR_BUFFER_BIT);

			// ワールド行列に対して回転をかける
			matrix_identity( m_matWorld );

			// カメラ( ビュー行列 )を設定
			{
				matrix_identity( m_matView );
				matrix_getTranslate( m_matView, 0.2, 0.2, 0.0 );
			}

			// ワールド行列×ビュー行列×射影行列をセット
			{
				matrix_multiply( m_matModel, m_matView, m_matWorld );

				matrix_identity( m_matProj );
				float pers = 27.5;
				float aspect = 1.33;
				float n = 0.1;
				float f = 100;
//				matrix_perspective( m_matProj, pers, aspect, n, f);

				matrix_multiply( m_matModel, m_matProj, m_matModel );


				glUniformMatrix4fv( m_hdlMat         , 1, GL_FALSE, m_matModel);

			}

//	void Draw ()
	{
	  glUseProgram(g_program);
	  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
	  glEnableVertexAttribArray(g_sp.aPosition);
	//  glEnableVertexAttribArray(g_sp.aTex);
	  glVertexAttribPointer(g_sp.aPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//  glVertexAttribPointer(g_sp.aTex, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
	  glEnableVertexAttribArray(0);
	  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
	}
    eglSwapBuffers(g_sc.display, g_sc.surface);
    frames++;
  }
  return 0;
}
