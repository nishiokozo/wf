#include <stdio.h>
#include <assert.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <math.h>
#include <bcm_host.h>
#include <EGL/eglext.h>


typedef struct 
{
	EGLNativeWindowType	nativeWin;
	EGLDisplay	display;
	EGLContext	context;
	EGLSurface	surface;
	EGLint		majorVersion;
	EGLint		minorVersion;
	int			width;
	int			height;
} EGL_INF;
//-----------------------------------------------------------------------------
void init_egl(EGL_INF *sc)
//-----------------------------------------------------------------------------
{
	bcm_host_init();
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

		assert(!(0 > graphics_get_display_size(0, &width, &height)));
		{
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
		}
		{
			EGLint attrib[] = 
			{
				EGL_RED_SIZE,		8,
				EGL_GREEN_SIZE,		8,
				EGL_BLUE_SIZE,		8,
				EGL_ALPHA_SIZE,		8,
				EGL_DEPTH_SIZE,		16,
				EGL_NONE
			};
			EGLint context[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
			EGLint numConfigs;
			EGLConfig config;

			sc->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			assert(!(sc->display == EGL_NO_DISPLAY));

			assert(eglInitialize(sc->display, &sc->majorVersion, &sc->minorVersion));

			assert(eglChooseConfig(sc->display, attrib, &config, 1, &numConfigs));

			sc->surface = eglCreateWindowSurface(sc->display, config, sc->nativeWin, NULL);
			assert(!(sc->surface == EGL_NO_SURFACE));

			sc->context = eglCreateContext(sc->display, config, EGL_NO_CONTEXT, context);
			assert(!(sc->context == EGL_NO_CONTEXT));

			assert(eglMakeCurrent(sc->display, sc->surface, sc->surface, sc->context));
		}
	}
	printf("%dx%d \n", sc->width, sc->height);

}
//---

#include	"vect.h"

GLuint g_hdlVert;
GLuint g_hdlIndex;
GLuint g_hdlprogram;
GLuint g_hdlMat;
GLint g_hdlVecPos;


//-----------------------------------------------------------------------------
int main ( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	// EGL 初期化	
	EGL_INF	g_egl;
	init_egl(&g_egl);


	{// 頂点バッファ登録
		GLfloat tblVert[] = 
		{
		   -1,-1,-1 ,
		    1,-1,-1 ,
		   -1, 1,-1 ,
		    1, 1,-1 ,
		   -1,-1, 1 ,
		    1,-1, 1 ,
		   -1, 1, 1 ,
		    1, 1, 1 ,
		};
		glGenBuffers(1, &g_hdlVert);
		glBindBuffer(GL_ARRAY_BUFFER, g_hdlVert);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tblVert), tblVert, GL_STATIC_DRAW);
	}

	{// インデックスバッファ登録
		unsigned short tblIndex[] = 
		{
			0,1,
			1,3,
			3,2,
			2,0,
			
			4,5,
			5,7,
			7,6,
			6,4,
			
			0,4,
			1,5,
			3,7,
			2,6,
		};
		// index buffer
		glGenBuffers(1, &g_hdlIndex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_hdlIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tblIndex), tblIndex, GL_STATIC_DRAW);
	}

 	int	cntVert = 8;
 	int	cntIndex = 24;
 
 
	{// シェーダー登録
		g_hdlprogram = glCreateProgram();
		{
			const GLchar *src = // GLSLは列ｘ行。なので左からマトリクスをかける
				"uniform mat4	Mat;					\n"
				"attribute vec3	Pos;					\n"
				"void main() 							\n"
				"{ 										\n"
				"	 gl_Position = Mat * vec4(Pos,1);	\n" // 左から掛ける
				"}								 		\n"
			;
			GLuint hdl = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(hdl, 1, &src, NULL);
			glCompileShader(hdl);
			glAttachShader(g_hdlprogram, hdl);
			glDeleteShader(hdl);
		}

		{
			const GLchar *src =
				"precision mediump float;				\n"
				"void main()							\n"
				"{										\n"
				"	gl_FragColor = vec4( 1,1, 1, 1.0);	\n"
				"}										\n"
			;
			GLuint hdl = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(hdl, 1, &src, NULL);
			glCompileShader(hdl);
			glAttachShader(g_hdlprogram, hdl);
			glDeleteShader(hdl);
		}

		glLinkProgram(g_hdlprogram);
		g_hdlMat = glGetUniformLocation( g_hdlprogram, "Mat" );
		g_hdlVecPos = glGetAttribLocation(g_hdlprogram, "Pos");

	}



	vect44	matWorld;
	vect44	matPers;
	vect44	matView;
	vect44	matModel;


	glClearColor(0.0f, 0.0f, 0.0f, 1.0);

	while(1) 
	{
		glViewport(0, 0, g_egl.width, g_egl.height);
		glClear(GL_COLOR_BUFFER_BIT);

		// ワールド行列に対して回転をかける
		matWorld.identity();

		// カメラ( ビュー行列 )を設定
		matView.identity();

		// 射影行列をセット
		matPers.setPerspective( 27.5f, 1920.0/1080.0 ); 


		// モデル行列をセット
		matModel.identity();

		static float rad =0;
		rad += RAD(1);
		matModel.rotX(rad);
		matModel.rotY(rad);
		matModel.translate(-0.2 , 0.2, 5 );

		vect44 mat = matModel*matPers;

		glUniformMatrix4fv( g_hdlMat				 , 1, GL_FALSE, mat.GetArray() );
		{
			glUseProgram(g_hdlprogram);
			glBindBuffer(GL_ARRAY_BUFFER, g_hdlVert);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_hdlIndex);
			glEnableVertexAttribArray(g_hdlVecPos);
			glVertexAttribPointer(g_hdlVecPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);
			glDrawElements(GL_LINES, cntIndex, GL_UNSIGNED_SHORT, 0);
		}

		//---
		eglSwapBuffers(g_egl.display, g_egl.surface);
	}
	return 0;
}
