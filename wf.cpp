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

	mat4	matWorld;
	mat4	matPers;
	mat4	matView;
	mat4	matModel;
	
	

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
			matWorld.identity();

			// カメラ( ビュー行列 )を設定
			matView.identity();

			// 射影行列をセット
//			mat4_perspective( matPers,   27.5f, 1.33, 0.1f, 100.0f ); 

			// モデル行列をセット
			matModel.identity();
			
			mat4 mat = matModel;

			glUniformMatrix4fv( m_hdlMat         , 1, GL_FALSE, mat.GetArray() );

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
