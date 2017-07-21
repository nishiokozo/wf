//#include <iostream>
//using namespace std;
#include <memory>
//#include <iostream>
#include <new>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "bcm_host.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

typedef struct
{
   uint32_t width;
   uint32_t height;
// OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;

} INF_EGL;



//-----------------------------------------------------------------------------
static void init_egl(INF_EGL& inf)
//-----------------------------------------------------------------------------
{
	bcm_host_init();

   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_DEPTH_SIZE, 16,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };
   
   static const EGLint context_attributes[] = 
   {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   EGLConfig config;

   // get an EGL display connection
   inf.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(inf.display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(inf.display, NULL, NULL);
   assert(EGL_FALSE != result);
   
   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(inf.display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);
   
   // get an appropriate EGL frame buffer configuration
   result = eglBindAPI(EGL_OPENGL_ES_API);
   assert(EGL_FALSE != result);
   
   // create an EGL rendering context
   inf.context = eglCreateContext(inf.display, config, EGL_NO_CONTEXT, context_attributes);
   assert(inf.context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &inf.width, &inf.height);
   assert( success >= 0 );

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = inf.width;
   dst_rect.height = inf.height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = inf.width << 16;
   src_rect.height = inf.height << 16;        

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, DISPMANX_NO_ROTATE/*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = inf.width;
   nativewindow.height = inf.height;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   inf.surface = eglCreateWindowSurface( inf.display, config, &nativewindow, NULL );
   assert(inf.surface != EGL_NO_SURFACE);
   
   // connect the context to the surface
   result = eglMakeCurrent(inf.display, inf.surface, inf.surface, inf.context);
   assert(EGL_FALSE != result);
   
}

//---

#include	"vect.h"

//#define //glcheck()	if ( glGetError() )	{printf("info glGetError()= 0x%04x (%d) \n", glGetError(),__LINE__ );}
#define	glcheck()	assert(glGetError()==GL_NO_ERROR);

//=============================================================================
class	INF_FBO
//=============================================================================
{
public:
	GLuint	m_hdlFbo;
	GLuint	m_color_tex;
	GLuint	m_depth_tex;

	int		m_w;
	int		m_h;

//-----------------------------------------------------------------------------
INF_FBO( int width, int height )
//-----------------------------------------------------------------------------
{
	memset( this, 0, sizeof(*this));
	
	m_w = width;
	m_h = height;
#if  1
	glGenTextures(1, &m_color_tex);
	glcheck();
	glBindTexture(GL_TEXTURE_2D, m_color_tex);
	glcheck();
//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_SHORT_5_6_5, NULL);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,0);	//正常動作
//	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_SHORT_4_4_4_4,0); // 動作しない
	glcheck();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glcheck();

	glGenFramebuffers(1, &m_hdlFbo);
	glcheck();
	glBindFramebuffer(GL_FRAMEBUFFER, m_hdlFbo);
	glcheck();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_tex, 0);
	glcheck();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glcheck();
#else
	glGenTextures(1, &m_color_tex);
	glcheck();
	glBindTexture(GL_TEXTURE_2D, m_color_tex);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glcheck();
//	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,format,GL_UNSIGNED_SHORT_5_6_5,0);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,format,GL_UNSIGNED_SHORT_4_4_4_4,0);//エラー発生
	glcheck();

	glGenTextures(1, &m_depth_tex);
	glcheck();
	glBindTexture(GL_TEXTURE_2D, m_depth_tex);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glcheck();
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 2, 2, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glcheck();

	//-------------------------
	glGenFramebuffers(1, &m_hdlFbo);
	glcheck();
	glBindFramebuffer(GL_FRAMEBUFFER, m_hdlFbo);
	glcheck();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_tex, 0);
	glcheck();
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glcheck();
#endif

            GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
//                cout << status << endl; // this is not called
				 printf("\nFBO error %d\n", status );
            }
}
//-----------------------------------------------------------------------------
void	begin()
//-----------------------------------------------------------------------------
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_hdlFbo );
	glViewport(0, 0, m_w, m_h);
}
//-----------------------------------------------------------------------------
void	end()
//-----------------------------------------------------------------------------
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0 );
}

};


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
	GLuint m_hdlTex;

	INF_TEX( GLint format, GLsizei width, GLsizei height, GLint WRAP_S, GLint WRAP_T, GLint MAG_FILTER, GLint MIN_FILTER )
	{
		m_format		= format;
		m_width			= width;
		m_height		= height;
		m_WRAP_S	 	= WRAP_S;
		m_WRAP_T	 	= WRAP_T;
		m_MAG_FILTER	= MAG_FILTER;
		m_MIN_FILTER	= MIN_FILTER;
		//
		m_hdlTex=0;
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

		glGenTextures(1, &t.m_hdlTex); //生成数
		glcheck();
		glBindTexture(GL_TEXTURE_2D, t.m_hdlTex);
		glcheck();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//1, 2, 4, or 8
		glcheck();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, t.m_WRAP_S);
		glcheck();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t.m_WRAP_T);
		glcheck();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, t.m_MAG_FILTER);
		glcheck();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, t.m_MIN_FILTER);
		glcheck();
		glTexImage2D(GL_TEXTURE_2D, 0
			, t.m_format
			, t.m_width
			, t.m_height, 0
			, t.m_format, GL_UNSIGNED_BYTE
			, tex_pixels
		);
		glcheck();

	 	m.m_pTex = (INF_TEX*)malloc(sizeof(INF_TEX));
	 	*m.m_pTex = t;
	}
	
	{// 頂点バッファ登録
		glGenBuffers(1, &m.m_hdlVert);
		glcheck();
		glBindBuffer(GL_ARRAY_BUFFER, m.m_hdlVert);
		glcheck();
		glBufferData(GL_ARRAY_BUFFER, sizeVert, tblVert, GL_STATIC_DRAW);
		glcheck();
	}

	{// インデックスバッファ登録
		// index buffer
		glGenBuffers(1, &m.m_hdlIndex);
		glcheck();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.m_hdlIndex);
		glcheck();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*cntIndex, tblIndex, GL_STATIC_DRAW);
		glcheck();
	}

  	{// シェーダー登録
		m.m_hdlprogram = glCreateProgram();
			glcheck();
		{
			GLuint hdl = glCreateShader(GL_VERTEX_SHADER);
			glcheck();
			glShaderSource(hdl, 1, &srcVert, NULL);
			glcheck();
			glCompileShader(hdl);
			glcheck();
			glAttachShader(m.m_hdlprogram, hdl);
			glcheck();
			glDeleteShader(hdl);
			glcheck();
		}

		{
			GLuint hdl = glCreateShader(GL_FRAGMENT_SHADER);
			glcheck();
			glShaderSource(hdl, 1, &srcFlag, NULL);
			glcheck();
			glCompileShader(hdl);
			glcheck();
			glAttachShader(m.m_hdlprogram, hdl);
			glcheck();
			glDeleteShader(hdl);
			glcheck();
		}

		glLinkProgram(m.m_hdlprogram);
		m.m_shaderMat	= glGetUniformLocation( m.m_hdlprogram, "Mat" );
		glcheck();
		m.m_shaderTex	= glGetUniformLocation(m.m_hdlprogram,"Texture");
		glcheck();
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
			glcheck();
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
		glBindTexture( GL_TEXTURE_2D, m.m_pTex->m_hdlTex );
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
void	gl_drawModelTex( INF_MODEL& m, vect44& mat, GLuint hdlTex )
//-----------------------------------------------------------------------------
{
	glUseProgram( m.m_hdlprogram );

	// テクスチャ
//	if ( m.m_pTex )
	{
		glUniform1i( m.m_shaderTex, 0);
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, hdlTex );
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

// シェーダコンパイルのログバッファ 
#define MAX_SHADER_LOG_SIZE		(1024)
static	char s_logBuffer[MAX_SHADER_LOG_SIZE]; 
//-----------------------------------------------------------------------------
static void printShaderInfoLog(
//-----------------------------------------------------------------------------
	GLuint shader
)
{
	int logSize;
	int length;

	// ログの長さは、最後のNULL文字も含む 
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &logSize);
	glcheck();

	if (logSize > 1)
	{
		glGetShaderInfoLog(shader, MAX_SHADER_LOG_SIZE, &length, s_logBuffer);
		glcheck();
		printf( "printShaderInfoLog:%s\n", s_logBuffer );
	}
}
class INF_FILE
{
	FILE*	m_fp;
	int		m_len;
	bool	m_error;
	char*	m_pBuf;

	INF_FILE( const char* fn )
	{
		if ((m_fp = fopen(fn, "rb")) == NULL) 
		{
			m_error = true;
			return;
		}
		else
		{
			fseek(m_fp, 0, SEEK_END);
			m_len = (int)ftell(m_fp);
			fseek(m_fp, 0, SEEK_SET);

			m_pBuf = (char*)malloc( m_len );
			fread( m_pBuf, 1, m_len, m_fp);

			fclose( m_fp );
		
		}
	}

	~INF_FILE()
	{
		fclose( m_fp );
		free( m_pBuf );
	}

};
//-----------------------------------------------------------------------------
bool	file_malloc_Load( const char* fname, char** ppBuf, int* pLen )
//-----------------------------------------------------------------------------
{
	char*	fbuf = 0;
	int		flen = 0;
	{

		FILE*	fp = 0;
		if ((fp = fopen(fname, "rb")) == NULL)
		{
			printf( "File error [%s]\n", fname );
			return	false;
		}
		else
		{
			{
				fseek(fp, 0, SEEK_END);
				flen = (int)ftell(fp);
				fseek(fp, 0, SEEK_SET);
			}
			{
				fbuf = (char*)malloc( flen );
//				fbuf = new char( flen );
				fread(fbuf, 1, flen, fp);
			}
			fclose( fp );
		}
	}
	(*ppBuf) = fbuf;
	(*pLen) = flen;

	return	true;
}

//-----------------------------------------------------------------------------
bool shader_Compile(
//-----------------------------------------------------------------------------
	int		hdlProgram,
	int		type,
	const char*	fn
)
{
	GLint result;

	char*	pBuf = 0; 
	int		len = 0;

	file_malloc_Load( fn, &pBuf, &len );
	GLuint hdl = glCreateShader(type);
	glcheck();
	glShaderSource( hdl, 1, (const char **)&pBuf, &len);
	glcheck();
	glCompileShader( hdl );
	glcheck();
	glGetShaderiv( hdl, GL_COMPILE_STATUS, &result);
	glcheck();

	printShaderInfoLog( hdl );
	if (result == GL_FALSE)
	{
		fprintf(stderr, "Compile error at [%s]\n", fn);
	}

	glAttachShader( hdlProgram, hdl );
	glcheck();
	glDeleteShader( hdl );
	glcheck();

//printf("src:%s\n", pBuf);
	if ( pBuf ) {free( pBuf );pBuf = 0;}
	return	true;
}

//-----------------------------------------------------------------------------
int main( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	INF_EGL g_egl;
	// EGL 初期化	
	init_egl(g_egl);

#define	SIZE 512

	INF_FBO	fbo( SIZE, SIZE );
	

	INF_MODEL model_wf;
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
			  model_wf
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

	INF_MODEL model_tex;
	{
		INF_FORM pForm[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
			{"Uv"	, (void*)(12),	2},		//Uv
		};
		int cntForm = sizeof(pForm)/sizeof(INF_FORM);
		
		GLfloat tblVert[] = 
		{
		   -1,-1, 0 ,	 0, 0,
		    1,-1, 0 ,	 1, 0,
		   -1, 1, 0 ,	 0, 1,
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
			GL_RGBA,
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
			  model_tex
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
	glClearColor(0.15f, 0.25f, 0.35f, 1.0f);

	while(1) 
	{
		fbo.begin();

			glClearColor(0.15f, 0.25f, 0.35f,1.0f);
	        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


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
			gl_drawModel( model_wf, mat_wf );

		fbo.end();

		glClearColor(0.65f, 0.25f, 0.35f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_egl.width, g_egl.height);

		vect44 m;
		m.translate(2,1,9);
		vect44 mat_tex = m*matPers;

		gl_drawModelTex( model_tex, mat_tex, fbo.m_color_tex );



		//---
		eglSwapBuffers(g_egl.display, g_egl.surface);
	}
	return 0;
}
