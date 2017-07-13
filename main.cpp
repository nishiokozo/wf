#include <stdio.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <math.h>
#include "bcm_host.h"
#include "vect.h"
#include "EGL/eglext.h"


typedef struct 
{
	EGLNativeWindowType	nativeWin;
	EGLDisplay	display;
	EGLContext	context;
	EGLSurface	surface;
	EGLint			majorVersion;
	EGLint			minorVersion;
	int				 width;
	int				 height;
} ScreenSettings;


const char fShaderSrc[] =
	"precision mediump float;\n"
	"void main()				\n"
	"{									\n"
	"	gl_FragColor = vec4(1,1, 0.5, 1.0);\n"
	"}									\n";

//typedef struct {
//	GLint	 hdlPos;
//} ShaderParams;

typedef struct 
{
		GLfloat x, y, z;
} VERT_INF;



unsigned short tblIndex[] = {
	0, 1, 2
};


//ShaderParams		g_sp;
ScreenSettings	g_sc;

GLuint hdlVert;
GLuint hdlIndex;
GLuint hdlProgram;


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
		EGL_RED_SIZE,			 8,
		EGL_GREEN_SIZE,		 8,
		EGL_BLUE_SIZE,			8,
		EGL_ALPHA_SIZE,		 8,
		EGL_DEPTH_SIZE,		 24,
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
	GLuint vshader;
	GLint compiled;

	vshader = glCreateShader(type);
	if (vshader == 0) return 0;

	glShaderSource(vshader, 1, &shaderSource, NULL);
	glCompileShader(vshader);

	glGetShaderiv(vshader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) { // compile error
		GLint infoLen = 0;
		glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 0) {
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(vshader, infoLen, NULL, infoLog);
			printf("Error compiling vshader:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(vshader);
		return 0;
	}
	return vshader;
}

int InitShaders(GLuint *program, char const *vShSrc, char const *fShSrc)
{
	GLuint vShader;
	GLuint fShader;
	GLint	linked;
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



//-----------------------------------------------------------------------------
int main ( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	GLuint hdlMat;
	GLint hdlVecPos;

	unsigned int frames = 0;
	int res;

	vect44	matWorld;
	vect44	matPers;
	vect44	matView;
	vect44	matModel;



	

	bcm_host_init();

	res = WinCreate(&g_sc);
	if (!res) return 0;
	res = SurfaceCreate(&g_sc);
	if (!res) return 0;
	
// 行優先、列ｘ行で計算、ここまではOK。
// M・ｖで転置せずとも正しく計算される・・ここでやっぱりまたアレと思ってしまう。
// これについては結果がベクトルだから転置行列を使っても結果に影響を及ぼさないということ。
// 印象から自動で転置しているような思い込みをいつの間にかしてしまいがち。ここも注意。


// 
	// GLSLではマトリクス乗算は列ｘ行で計算されることに注意






	VERT_INF tblPos[] = 
	{
	  {.x = -0.433, .y = -0.25, .z = 0.0f},
	  {.x =  0.433, .y = -0.25, .z = 0.0f},
	  {.x =  0.0  , .y =  0.5 , .z = 0.0f}
	};
 
	char vshader[] = // GLSLは列ｘ行。なので左からマトリクスをかける
		"uniform mat4	Mat;					\n"
		"attribute vec3	Pos;					\n"
		"void main() 							\n"
		"{ 										\n"
		"	 gl_Position = Mat * vec4(Pos,1);	\n" // 左から掛ける
		"}								 		\n"
	;





	res = InitShaders(&hdlProgram, vshader, fShaderSrc);
	if (!res) return 0;


	//	createBuffer();
	{
		glGenBuffers(1, &hdlVert);
		// vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, hdlVert);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tblPos), tblPos, GL_STATIC_DRAW);

		// index buffer
		glGenBuffers(1, &hdlIndex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hdlIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tblIndex), tblIndex, GL_STATIC_DRAW);
	}

	hdlMat = glGetUniformLocation( hdlProgram, "Mat" );
	hdlVecPos = glGetAttribLocation(hdlProgram, "Pos");

	glClearColor(0.3f, 0.6f, 0.75f, 0.5f);

	/* 600frame / 60fps = 10sec */ 
	while(1) 
	{
		glViewport(0, 0, g_sc.width, g_sc.height);
		glClear(GL_COLOR_BUFFER_BIT);

			// ワールド行列に対して回転をかける
			matWorld.identity();

			// カメラ( ビュー行列 )を設定
			matView.identity();

			// 射影行列をセット
//			matPers.setPerspective( 27.5f, 1.33, 0.1f, 100.0f ); 

	//-----------------------------------------------------------------------------
	{
		float fovy	=	27.5; 
		float a=	1920.0f/1080.0f; 
		float zNear	=	0.1; 
		float zFar	=	100;

		float p = 1.0f / tanf(fovy *(M_PI/360.0));

		float	z2 = zFar;
		float	z1 = zNear;

		matPers =	vect44(
			p / a,		 0.0,	0.0,								 0.0,
			0.0,				p,			0.0,								 0.0,
			0.0,				0.0,		(z2+z1)				 /(z1-z2), 	-1.0,
			0.0,			0.0,	(2.0 * z2 * z1) /(z1-z2),	 0.0
		);

		matPers =	vect44(
			p / a, 		0.0,	0.0,		0.0,
			0.0,				p,			0.0,		0.0,
			0.0,				0.0,		1.0,	0.0,
			0.0,			0.0,	0.0,	 0.0
		);
		matPers =	vect44(
			1 / a, 		0.0,	0.0,		0.0,
			0.0,				1,	 	0.0,		0.0,
			0.0,				0.0,		0.0,	0.0,
			0.0,			0.0,	0.0,	1.0
		);
	}


			// モデル行列をセット
			matModel.identity();

static float rad =0;
rad += RAD(1);
			matModel.rotZ(rad);
			matModel.translate(-0.2 , 0.2, 0.2);

			
			vect44 mat = matModel*matPers;
//			vect44 mat = matModel;

			glUniformMatrix4fv( hdlMat				 , 1, GL_FALSE, mat.GetArray() );




//	void Draw ()
	{
		glUseProgram(hdlProgram);
		glBindBuffer(GL_ARRAY_BUFFER, hdlVert);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hdlIndex);
		glEnableVertexAttribArray(hdlVecPos);
	//	glEnableVertexAttribArray(g_sp.aTex);
		glVertexAttribPointer(hdlVecPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//	glVertexAttribPointer(g_sp.aTex, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
	}
		eglSwapBuffers(g_sc.display, g_sc.surface);
		frames++;
	}
	return 0;
}
