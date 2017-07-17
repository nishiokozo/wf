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

   GLuint g_tex;



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


struct INF_FORM
{
	char	name[16];
	void*	pOfs;
	int 	size;
	GLint 	shaderAttr;
};

struct INF_MODEL_CON
{
	GLenum	m_mode;
 	
	GLuint  m_hdlVert;
	GLuint  m_hdlIndex;
	GLuint  m_hdlprogram;
	GLuint  m_shaderMat;
	GLint  	m_hdlPos;
	GLint  	m_sizePos;
	GLsizei	m_cntIndex;

	INF_MODEL_CON()
	{
		m_mode = 0;

		m_hdlVert = 0;
		m_hdlIndex = 0;
		m_hdlprogram = 0;
		m_shaderMat = 0;
		m_hdlPos = 0;
		m_sizePos = 0;
		m_cntIndex = 0;
		
	}
};
struct INF_MODEL_TEX
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
	INF_FORM*	m_infForm;
	int			m_cntForm;

	INF_MODEL_TEX()
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
		m_infForm = 0;
		m_cntForm = 0;
	}
};




//-----------------------------------------------------------------------------
void	gl_setModelCon( INF_MODEL_CON& m, GLfloat* tblVert, GLushort* tblIndex, const GLchar *srcVert, const GLchar *srcFlag, GLsizei cntIndex, GLenum mode, int sizeVert )
//-----------------------------------------------------------------------------
{
	m.m_mode = mode;;
 	m.m_cntIndex = cntIndex;

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
		m.m_shaderMat = glGetUniformLocation( m.m_hdlprogram, "Mat" );
		m.m_hdlPos = glGetAttribLocation(m.m_hdlprogram, "Pos");
		m.m_sizePos = 3;//3:xyz

	}
}
//-----------------------------------------------------------------------------
void	gl_drawModelCon( INF_MODEL_CON& m, vect44& mat )
//-----------------------------------------------------------------------------
{
	glUseProgram( m.m_hdlprogram );

	glUniformMatrix4fv( m.m_shaderMat, 1, GL_FALSE, mat.GetArray() );

	glBindBuffer( GL_ARRAY_BUFFER,  m.m_hdlVert );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,  m.m_hdlIndex );

	glEnableVertexAttribArray( m.m_hdlPos );
	glVertexAttribPointer( m.m_hdlPos, m.m_sizePos, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	glEnableVertexAttribArray(0);

	glDrawElements( m.m_mode, m.m_cntIndex, GL_UNSIGNED_SHORT, 0 );
}

//-----------------------------------------------------------------------------
void	gl_setModelTex( INF_MODEL_TEX& m, GLfloat* tblVert, GLushort* tblIndex, const GLchar *srcVert, const GLchar *srcFlag
, GLsizei cntIndex, GLenum mode, int sizeVert, INF_FORM infForm[], int cntForm
, GLvoid* tex_pixels, GLint tex_format, GLsizei tex_width, GLsizei tex_height )
//-----------------------------------------------------------------------------
{
	m.m_mode = mode;
 	m.m_cntIndex = cntIndex;

	{//テクスチャ生成
		glGenTextures(1, &g_tex); //生成数
		glBindTexture(GL_TEXTURE_2D, g_tex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//1, 2, 4, or 8
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, tex_format, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_pixels);
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
	{// レンダリングパラメータ登録
		m.m_infForm = (INF_FORM*)malloc( sizeof(INF_FORM[cntForm]) );

	 	m.m_stride = 0;
	 	m.m_cntForm = cntForm;
		for ( int i = 0 ; i < cntForm ; i++ )
		{
		 	m.m_infForm[i].pOfs	= infForm[i].pOfs;

		 	m.m_infForm[i].size	= infForm[i].size;

			strcpy( m.m_infForm[i].name, infForm[i].name );

		 	m.m_stride += infForm[i].size*sizeof(GLfloat);
			
			//--
			
			m.m_infForm[i].shaderAttr	= glGetAttribLocation(m.m_hdlprogram, m.m_infForm[i].name );
		}

	}
}
//-----------------------------------------------------------------------------
void	gl_drawModelTex( INF_MODEL_TEX& m, vect44& mat )
//-----------------------------------------------------------------------------
{
	glUseProgram( m.m_hdlprogram );

	// テクスチャ
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, g_tex );


	// 頂点
	glUniformMatrix4fv( m.m_shaderMat, 1, GL_FALSE, mat.GetArray() );

	glBindBuffer( GL_ARRAY_BUFFER,  m.m_hdlVert );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,  m.m_hdlIndex );

	for ( int i = 0 ; i < m.m_cntForm ; i++ )
	{
		glEnableVertexAttribArray( m.m_infForm[i].shaderAttr );
		glVertexAttribPointer( 
			  m.m_infForm[i].shaderAttr
			, m.m_infForm[i].size
			, GL_FLOAT
			, GL_FALSE
			, m.m_stride
			, m.m_infForm[i].pOfs 
		);
	}
	glEnableVertexAttribArray(0);

	glDrawElements( m.m_mode, m.m_cntIndex, GL_UNSIGNED_SHORT, 0 );
}


//-----------------------------------------------------------------------------
int main ( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	// EGL 初期化	
	EGL_INF	g_egl;
	init_egl(&g_egl);




	INF_MODEL_CON wf;
	{
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

		gl_setModelCon( wf, tblVert, tblIndex, srcVert, srcFlag, cntIndex, GL_LINES, sizeof(tblVert) );
	}

	INF_MODEL_TEX tex;
	{
		INF_FORM infForm[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
			{"Uv"	, (void*)(12),	2},		//Uv
		};
		int cntForm = sizeof(infForm)/sizeof(INF_FORM);
		
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
			"varying vec2	vTex;							\n"
			"void main() 									\n"
			"{ 												\n"
			"	vTex = Uv;									\n"
			"	 gl_Position = Mat * vec4(Pos,1);			\n" // 左から掛ける
			"}								 				\n"
		;
		const GLchar *srcFlag =
			"precision mediump float;						\n"
			"varying	vec2 vTex;								\n"
			"uniform sampler2D uTexture;						\n"
			"void main()									\n"
			"{												\n"
			"	gl_FragColor = texture2D(uTexture,vec2(vTex.x,vTex.y));			\n"
			"}												\n"
		;

		GLint	tex_format=GL_RGB;
		GLsizei	tex_width=512;
		GLsizei	tex_height=512;
		GLvoid *tex_pixels=(char*)malloc( tex_width * tex_height * 3);
		{// テクスチャ生成
			//テクスチャデータ作成
			for ( int y = 0 ; y < tex_height ; y++ )
			{
				for ( int x = 0 ; x < tex_width ; x++ )
				{
					((char*)tex_pixels)[(y*tex_width+x)*3+0] =x/2;
					((char*)tex_pixels)[(y*tex_width+x)*3+1] =y/2;
					((char*)tex_pixels)[(y*tex_width+x)*3+2] =55;
				}
			}
		}			
            
		gl_setModelTex( 
			  tex
			, tblVert
			, tblIndex
			, srcVert
			, srcFlag
			, cntIndex
			, GL_TRIANGLE_STRIP
			, sizeof(tblVert)
			, infForm
			, cntForm
			, tex_pixels, tex_format, tex_width, tex_height 
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
		gl_drawModelCon( wf, mat_wf );


		vect44 m;
		m.translate(2,1,9);
		vect44 mat_tex = m*matPers;
		gl_drawModelTex( tex, mat_tex );



		//---
		eglSwapBuffers(g_egl.display, g_egl.surface);
	}
	return 0;
}
