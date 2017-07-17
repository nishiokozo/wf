//#include <iostream>
//using namespace std;
#include <memory>
//#include <iostream>
#include <new>
#include <stdio.h>
#include <memory.h>
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
	{//ラズパイ描画コンテキストの初期化
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
	{//EGLの初期化
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
	printf("%dx%d \n", sc->width, sc->height);

}
//---

#include	"vect.h"


struct INF_TEX
{
	GLint	m_format;
	GLsizei	m_width;
	GLsizei	m_height;
	GLint 	m_WRAP_S	 ;
	GLint 	m_WRAP_T	 ;
	GLint 	m_MAG_FILTER ;
	GLint 	m_MIN_FILTER ;
	//
	GLuint m_tex;

	INF_TEX( GLint format, GLsizei width, GLsizei height, GLint WRAP_S, GLint WRAP_T, GLint MAG_FILTER, GLint MIN_FILTER )
	{
		m_format		= format;
		m_width		= width;
		m_height		= height;
		m_WRAP_S	 	= WRAP_S;
		m_WRAP_T	 	= WRAP_T;
		m_MAG_FILTER	= MAG_FILTER;
		m_MIN_FILTER	= MIN_FILTER;
		//
		m_tex=0;
	}
};

struct INF_FORM
{
	char	name[16];
	void*	pOfs;
	int 	size;
	GLint 	shaderAttr;
};

struct INF_MODEL
{
	GLenum	m_mode;
 	
	GLuint  m_hdlVert;
	GLuint  m_hdlIndex;
	GLuint  m_hdlprogram;
	GLuint  m_shaderMat;
	GLuint  m_shaderTex;	//Sampler2D
	GLsizei	m_cntIndex;
	//
	GLsizei m_stride;
	GLvoid* m_ptrPos;
	GLvoid* m_ptrUv;
	INF_FORM*	m_pForm;
	int			m_cntForm;
	INF_TEX*	m_pTex;

	INF_MODEL()
	{
		m_mode = 0;

		m_hdlVert = 0;
		m_hdlIndex = 0;
		m_hdlprogram = 0;
		m_shaderMat = 0;
		m_shaderTex = 0;
		m_cntIndex = 0;
		//
		m_stride	= 0;
		m_ptrPos	= 0;
		m_ptrUv	= 0;
		m_pForm = 0;
		m_cntForm = 0;
		m_pTex = 0;
	}
};





//-----------------------------------------------------------------------------
void	gl_setModel( INF_MODEL& m
//-----------------------------------------------------------------------------
	, GLfloat* tblVert
	, GLushort* tblIndex
	, const GLchar *srcVert
	, const GLchar *srcFlag
	, GLsizei cntIndex
	, GLenum mode
	, int sizeVert
	, INF_FORM pForm[]
	, int cntForm
	, INF_TEX	pTex[]
	, GLvoid *tex_pixels
)
{
	m.m_mode = mode;
 	m.m_cntIndex = cntIndex;
 	m.m_pTex = 0;

	if( pTex != 0 )
	{//テクスチャ生成
		INF_TEX&	t = pTex[0];

		glGenTextures(1, &t.m_tex); //生成数
		glBindTexture(GL_TEXTURE_2D, t.m_tex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//1, 2, 4, or 8
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, t.m_WRAP_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t.m_WRAP_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, t.m_MAG_FILTER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, t.m_MIN_FILTER);
		glTexImage2D(GL_TEXTURE_2D, 0
			, t.m_format
			, t.m_width
			, t.m_height, 0
			, t.m_format, GL_UNSIGNED_BYTE
			, tex_pixels
		);

	 	m.m_pTex = (INF_TEX*)malloc(sizeof(INF_TEX));
	 	*m.m_pTex = t;
	}
	
	{// 頂点バッファ登録
		glGenBuffers(1, &m.m_hdlVert);
		glBindBuffer(GL_ARRAY_BUFFER, m.m_hdlVert);
		glBufferData(GL_ARRAY_BUFFER, sizeVert, tblVert, GL_STATIC_DRAW);
	}

	{// インデックスバッファ登録
		// index buffer
		glGenBuffers(1, &m.m_hdlIndex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.m_hdlIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*cntIndex, tblIndex, GL_STATIC_DRAW);
	}

  	{// シェーダー登録
		m.m_hdlprogram = glCreateProgram();
		{
			GLuint hdl = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(hdl, 1, &srcVert, NULL);
			glCompileShader(hdl);
			glAttachShader(m.m_hdlprogram, hdl);
			glDeleteShader(hdl);
		}

		{
			GLuint hdl = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(hdl, 1, &srcFlag, NULL);
			glCompileShader(hdl);
			glAttachShader(m.m_hdlprogram, hdl);
			glDeleteShader(hdl);
		}

		glLinkProgram(m.m_hdlprogram);
		m.m_shaderMat	= glGetUniformLocation( m.m_hdlprogram, "Mat" );
	}
	{// レンダリングパラメータ生成
		m.m_pForm = (INF_FORM*)malloc( sizeof(INF_FORM[cntForm]) );

	 	m.m_stride = 0;
	 	m.m_cntForm = cntForm;
		for ( int i = 0 ; i < cntForm ; i++ )
		{
		 	m.m_pForm[i].pOfs	= pForm[i].pOfs;

		 	m.m_pForm[i].size	= pForm[i].size;

			strcpy( m.m_pForm[i].name, pForm[i].name );

		 	m.m_stride += pForm[i].size*sizeof(GLfloat);
			
			//--
			
			m.m_pForm[i].shaderAttr	= glGetAttribLocation(m.m_hdlprogram, m.m_pForm[i].name );
		}

	}
}
//-----------------------------------------------------------------------------
void	gl_drawModel( INF_MODEL& m, vect44& mat )
//-----------------------------------------------------------------------------
{
	glUseProgram( m.m_hdlprogram );

	// テクスチャ
	if ( m.m_pTex )
	{
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, m.m_pTex->m_tex );
	}


	// 頂点
	glUniformMatrix4fv( m.m_shaderMat, 1, GL_FALSE, mat.GetArray() );

	glBindBuffer( GL_ARRAY_BUFFER,  m.m_hdlVert );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,  m.m_hdlIndex );

	for ( int i = 0 ; i < m.m_cntForm ; i++ )
	{
		glEnableVertexAttribArray( m.m_pForm[i].shaderAttr );
		glVertexAttribPointer( 
			  m.m_pForm[i].shaderAttr
			, m.m_pForm[i].size
			, GL_FLOAT
			, GL_FALSE
			, m.m_stride
			, m.m_pForm[i].pOfs 
		);
	}
	glEnableVertexAttribArray(0);

	glDrawElements( m.m_mode, m.m_cntIndex, GL_UNSIGNED_SHORT, 0 );
}


//-----------------------------------------------------------------------------
int main( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	// EGL 初期化	
	EGL_INF	g_egl;
	init_egl(&g_egl);




	INF_MODEL wf;
	{
		INF_FORM pForm[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
		};
		int cntForm = sizeof(pForm)/sizeof(INF_FORM);

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
		GLushort tblIndex[] = 
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
		int cntIndex = sizeof(tblIndex)/sizeof(GLushort);

		const GLchar *srcVert = // GLSLは列ｘ行。なので左からマトリクスをかける
			"uniform mat4	Mat;							\n"
			"attribute vec3	Pos;							\n"
			"void main() 									\n"
			"{ 												\n"
			"	 gl_Position = Mat * vec4(Pos,1);			\n" // 左から掛ける
			"}								 				\n"
		;
		const GLchar *srcFlag =
			"precision mediump float;						\n"
			"void main()									\n"
			"{												\n"
			"	gl_FragColor = vec4( 1,1, 1, 1.0);			\n"
			"}												\n"
		;

		gl_setModel( 
			  wf
			, tblVert
			, tblIndex
			, srcVert
			, srcFlag
			, cntIndex
			, GL_LINES
			, sizeof(tblVert)
			, pForm
			, cntForm
			, 0,0
		);

	}

	INF_MODEL tex;
	{
		INF_FORM pForm[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
			{"Uv"	, (void*)(12),	2},		//Uv
		};
		int cntForm = sizeof(pForm)/sizeof(INF_FORM);
		
		GLfloat tblVert[] = 
		{
		   -1,-1, 0 ,	-1,-1,
		    1,-1, 0 ,	 1,-1,
		   -1, 1, 0 ,	-1, 1,
		    1, 1, 0 ,	 1, 1,
		};
		GLushort tblIndex[] = 
		{
			0,1,2,3,
		};
		int cntIndex = sizeof(tblIndex)/sizeof(GLushort);
		
		const GLchar *srcVert = // GLSLは列ｘ行。なので左からマトリクスをかける
			"uniform mat4	Mat;							\n"
			"attribute vec3	Pos;							\n"
			"attribute vec2	Uv;								\n"
			"varying vec2	vUv;							\n"
			"void main() 									\n"
			"{ 												\n"
			"	vUv = Uv;									\n"
			"	 gl_Position = Mat * vec4(Pos,1);			\n" // 左から掛ける
			"}								 				\n"
		;
		const GLchar *srcFlag =
			"precision mediump float;						\n"
			"varying	vec2 vUv;							\n"
			"uniform sampler2D uTexture;					\n"
			"void main()									\n"
			"{												\n"
			"	gl_FragColor = texture2D(uTexture,vec2(vUv.x,vUv.y));			\n"
			"}												\n"
		;

		INF_TEX	infTex(
			GL_RGB,
			2,
			2,
			0?GL_REPEAT:(1?GL_CLAMP_TO_EDGE:GL_MIRRORED_REPEAT),
			0?GL_REPEAT:(1?GL_CLAMP_TO_EDGE:GL_MIRRORED_REPEAT),
			0?GL_LINEAR:GL_NEAREST,
			0?GL_LINEAR:GL_NEAREST
		);
		GLvoid*	tex_pixels=0;
		{// テクスチャ生成
			//テクスチャデータ作成
			tex_pixels=(GLvoid*)malloc( infTex.m_width * infTex.m_height * 3);

			for ( int y = 0 ; y < infTex.m_height ; y++ )
			{
				for ( int x = 0 ; x < infTex.m_width ; x++ )
				{
					((char*)tex_pixels)[(y*infTex.m_width+x)*3+0] =x*100;
					((char*)tex_pixels)[(y*infTex.m_width+x)*3+1] =y*100;
					((char*)tex_pixels)[(y*infTex.m_width+x)*3+2] =55;
				}
			}
		}			
            
		gl_setModel( 
			  tex
			, tblVert
			, tblIndex
			, srcVert
			, srcFlag
			, cntIndex
			, GL_TRIANGLE_STRIP
			, sizeof(tblVert)
			, pForm
			, cntForm
			, &infTex, tex_pixels
		);
	}



	vect44	matPers;
	vect44	matModel;


	// GL環境設定
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);

	while(1) 
	{
		glViewport(0, 0, g_egl.width, g_egl.height);
		glClear(GL_COLOR_BUFFER_BIT);



		// 射影行列をセット
		matPers.setPerspective( 27.5f, 1920.0/1080.0 ); 

		// モデル行列をセット
		matModel.identity();

		static float rad =0;
		rad += RAD(1);
		matModel.rotX(rad/3);
		matModel.rotY(rad);
		matModel.translate(-0.2 , 0.2, 5 );
		vect44 mat_wf = matModel*matPers;
		gl_drawModel( wf, mat_wf );


		vect44 m;
		m.translate(2,1,9);
		vect44 mat_tex = m*matPers;
		gl_drawModel( tex, mat_tex );



		//---
		eglSwapBuffers(g_egl.display, g_egl.surface);
	}
	return 0;
}
