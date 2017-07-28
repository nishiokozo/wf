//#include <iostream>
//using namespace std;
#include <memory>
//#include <iostream>
#include <new>
#include <stdio.h>
//#include <memory.h>
#include <math.h>
//#include <assert.h>
#include <cassert>
//#include <unistd.h>
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


int		m_hdl_Offset;

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

	GLint		m_viewport[4];

//-----------------------------------------------------------------------------
INF_FBO( int width, int height, GLint val_MAX_MIN_FILTER )
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
 	GLint	vram_format = GL_RGB;
 	GLint	ram_format = GL_RGB;		// GL_RGBAは動作しない
 	GLint	ram_type = GL_UNSIGNED_BYTE;
 	GLvoid*	ram_pixels = 0;
 	glTexImage2D(
 		 GL_TEXTURE_2D
 		,0
 		,vram_format
 		,width
 		,height
 		,0
 		,ram_format
 		,ram_type
 		,ram_pixels
 	);
	glcheck();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, val_MAX_MIN_FILTER);
	glcheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, val_MAX_MIN_FILTER);
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
	glGetIntegerv( GL_VIEWPORT, m_viewport );
	glViewport(0, 0, m_w, m_h);
}
//-----------------------------------------------------------------------------
void	end()
//-----------------------------------------------------------------------------
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0 );
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
}

};

/*
		INF_TEX	infTex(
			  GL_RGBA
			, 2
			, 2
			, 0?GL_REPEAT:(1?GL_CLAMP_TO_EDGE:GL_MIRRORED_REPEAT)
			, 0?GL_REPEAT:(1?GL_CLAMP_TO_EDGE:GL_MIRRORED_REPEAT)
			, 1?GL_LINEAR:GL_NEAREST	//MAG
			, 1?GL_LINEAR:GL_NEAREST	//MIN
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
            
*/

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
	}
};

struct INF_ATTR
{
	char	name[16];
	void*	pOfs;
	int 	size;
	GLint 	hdl;
};
struct INF_UNIFORM
{
	char	name[16];
	int 	size;
	GLuint  hdl;
};

struct INF_MODEL
{
	GLenum	m_mode;
 	
	GLuint  m_hdlVert;
	GLuint  m_hdlIndex;
	GLuint  m_hdlprogram;
	GLuint  m_shaderMat;
	GLuint  m_shaderTex1;
	GLuint  m_shaderTex2;
	GLuint  m_shaderTex3;
	GLuint  m_shaderTex4;
	GLuint  m_shaderOfs1;
	GLuint  m_shaderOfs2;
	GLuint  m_shaderOfs3;
	GLuint  m_shaderOfs4;
	GLsizei	m_cntIndex;
	//
	GLsizei m_stride;
	GLvoid* m_ptrPos;
	GLvoid* m_ptrUv;
	INF_ATTR*		m_pAttr;
	int				m_cntAttr;
	INF_UNIFORM*	m_pUniform;
	int				m_cntUniform;
	//
	static const int	m_NUM_TEX = 4;
	GLuint		m_hdlTex[m_NUM_TEX];
	int			m_cntTex;

	INF_MODEL()
	{
		m_mode = 0;

		m_hdlVert = 0;
		m_hdlIndex = 0;
		m_hdlprogram = 0;
		m_shaderMat = 0;
		m_shaderTex1 = 0;
		m_shaderTex2 = 0;
		m_shaderTex3 = 0;
		m_shaderTex4 = 0;
		m_shaderOfs1 = 0;
		m_shaderOfs2 = 0;
		m_shaderOfs3 = 0;
		m_shaderOfs4 = 0;
		m_cntIndex = 0;
		//
		m_stride	= 0;
		m_ptrPos	= 0;
		m_ptrUv	= 0;
		m_pAttr = 0;
		m_cntAttr = 0;
		//
		m_hdlTex[0]= 0;
		m_cntTex = 0;
	}
};





//-----------------------------------------------------------------------------
void	gl_setModel( INF_MODEL& m
//-----------------------------------------------------------------------------
	, GLfloat* 		tblVert
	, GLushort* 	tblIndex
	, const GLchar*	srcVert
	, const GLchar*	srcFlag
	, GLsizei 		cntIndex
	, GLenum 		mode
	, int 			sizeVert
	, const INF_ATTR 	pAttr[]
	, int 				cntAttr
)
{

	m.m_mode = mode;
 	m.m_cntIndex = cntIndex;
	
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
	}


	{//シェーダー内変数
			m.m_shaderMat	= glGetUniformLocation( m.m_hdlprogram, "Mat" );
			glcheck();
			m.m_shaderTex1	= glGetUniformLocation( m.m_hdlprogram, "Tex1" );
			glcheck();
			m.m_shaderTex2	= glGetUniformLocation( m.m_hdlprogram, "Tex2" );
			glcheck();
			m.m_shaderOfs1	= glGetUniformLocation( m.m_hdlprogram, "ofs1" );
			glcheck();
			m.m_shaderOfs2	= glGetUniformLocation( m.m_hdlprogram, "ofs2" );
			glcheck();
			m.m_shaderOfs3	= glGetUniformLocation( m.m_hdlprogram, "ofs3" );
			glcheck();
			m.m_shaderOfs4	= glGetUniformLocation( m.m_hdlprogram, "ofs4" );
			glcheck();

printf("Mat %x\n", m.m_shaderMat );
printf("Tex1 %x\n", m.m_shaderTex1 );
printf("Tex2 %x\n", m.m_shaderTex2 );
printf("ofs1 %x\n", m.m_shaderOfs1 );
printf("ofs2 %x\n", m.m_shaderOfs2 );
printf("ofs3 %x\n", m.m_shaderOfs3 );
printf("ofs4 %x\n", m.m_shaderOfs4 );
	}

	{// レンダリングパラメータ生成
		m.m_pAttr = (INF_ATTR*)malloc( sizeof(INF_ATTR[cntAttr]) );
	 	m.m_cntAttr = cntAttr;
	 	m.m_stride = 0;
		//
		for ( int i = 0 ; i < cntAttr ; i++ )
		{
			strcpy( m.m_pAttr[i].name, pAttr[i].name );

		 	m.m_pAttr[i].pOfs	= pAttr[i].pOfs;

		 	m.m_pAttr[i].size	= pAttr[i].size;

			m.m_pAttr[i].hdl	= glGetAttribLocation(m.m_hdlprogram, m.m_pAttr[i].name );
			glcheck();
			
			//--

		 	m.m_stride += pAttr[i].size*sizeof(GLfloat);
		}

	}


}
//-----------------------------------------------------------------------------
void	gl_drawModel( INF_MODEL& m, vect44& mat )
//-----------------------------------------------------------------------------
{
	glUseProgram( m.m_hdlprogram );

	// テクスチャ
//	if ( m.m_cntTex > 0 )
	{
		for ( int i = 0 ; i < m.m_cntTex ; i++ )
		{
			if ( i==0 ) 
			{
//				glUniform1i( m.m_shaderTex1, 0);
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, m.m_hdlTex[i] );
			}
			if ( i==1 ) 
			{
//				glUniform1i( m.m_shaderTex2, 1);
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, m.m_hdlTex[i] );
			}
		}
	}


	// 頂点
	glUniformMatrix4fv( m.m_shaderMat, 1, GL_FALSE, mat.GetArray() );

	{//ドットピッチ
		GLfloat		m_viewport[4];
		glGetFloatv( GL_VIEWPORT, m_viewport );
		glUniform2f(m.m_shaderOfs1, 1.0/m_viewport[2] , 1.0/m_viewport[3] );
	}

	glBindBuffer( GL_ARRAY_BUFFER,  m.m_hdlVert );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,  m.m_hdlIndex );

	for ( int i = 0 ; i < m.m_cntAttr ; i++ )
	{
		glEnableVertexAttribArray( m.m_pAttr[i].hdl );
		glVertexAttribPointer( 
			  m.m_pAttr[i].hdl
			, m.m_pAttr[i].size
			, GL_FLOAT
			, GL_FALSE
			, m.m_stride
			, m.m_pAttr[i].pOfs 
		);
	}
	glEnableVertexAttribArray(0);

	glDrawElements( m.m_mode, m.m_cntIndex, GL_UNSIGNED_SHORT, 0 );
}
//-----------------------------------------------------------------------------
void	gl_drawModelTex1( INF_MODEL& m, vect44& mat, GLuint hdlTex )
//-----------------------------------------------------------------------------
{
	m.m_cntTex=1;
	m.m_hdlTex[0]=hdlTex;
	gl_drawModel( m, mat );
}
//-----------------------------------------------------------------------------
void	gl_drawModelTex2( INF_MODEL& m, vect44& mat, GLuint hdlTex1, GLuint hdlTex2 )
//-----------------------------------------------------------------------------
{
	m.m_cntTex=2;
	m.m_hdlTex[0]=hdlTex1;
	m.m_hdlTex[1]=hdlTex2;
	gl_drawModel( m, mat );
}
//-----------------------------------------------------------------------------
void	gl_drawModelTex2b( INF_MODEL& m, vect44& mat, GLuint hdlTex1, GLuint hdlTex2 )
//-----------------------------------------------------------------------------
{
	glUseProgram( m.m_hdlprogram );
glcheck();

	glUniformMatrix4fv( m.m_shaderMat, 1, GL_FALSE, mat.GetArray() );
glcheck();

	glBindBuffer( GL_ARRAY_BUFFER,  m.m_hdlVert );
glcheck();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,  m.m_hdlIndex );
glcheck();

	for ( int i = 0 ; i < m.m_cntAttr ; i++ )
	{
		glEnableVertexAttribArray( m.m_pAttr[i].hdl );
		glVertexAttribPointer( 
			  m.m_pAttr[i].hdl
			, m.m_pAttr[i].size
			, GL_FLOAT
			, GL_FALSE
			, m.m_stride
			, m.m_pAttr[i].pOfs 
		);
glcheck();
	}
	glEnableVertexAttribArray(0);
glcheck();

	glActiveTexture( GL_TEXTURE0 );
glcheck();
	glBindTexture( GL_TEXTURE_2D, hdlTex2 );
glcheck();

	glActiveTexture( GL_TEXTURE1 );
glcheck();
	glBindTexture( GL_TEXTURE_2D, hdlTex1 );
glcheck();
//	glUniform1i(glGetUniformLocation(m.m_hdlprogram, "Tex1"), 0);
	glUniform1i(m.m_shaderTex1, 0);
glcheck();
//	glUniform1i(glGetUniformLocation(m.m_hdlprogram, "Tex2"), 1);
	glUniform1i(m.m_shaderTex2, 1);
glcheck();
	{//ドットピッチ
		GLfloat		m_viewport[4];
		glGetFloatv( GL_VIEWPORT, m_viewport );
		glUniform2f(m.m_shaderOfs1, 1.0/m_viewport[2] , 1.0/m_viewport[3] );
	}
glcheck();

	glDrawElements( m.m_mode, m.m_cntIndex, GL_UNSIGNED_SHORT, 0 );
glcheck();
}
//-----------------------------------------------------------------------------
void	gl_drawModelBoid4( INF_MODEL& m, vect44& mat, GLuint hdlTex1 )
//-----------------------------------------------------------------------------
{
	glUseProgram( m.m_hdlprogram );
	glcheck();

	glUniformMatrix4fv( m.m_shaderMat, 1, GL_FALSE, mat.GetArray() );
	glcheck();

	glBindBuffer( GL_ARRAY_BUFFER,  m.m_hdlVert );
	glcheck();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,  m.m_hdlIndex );
	glcheck();

	for ( int i = 0 ; i < m.m_cntAttr ; i++ )
	{
		glEnableVertexAttribArray( m.m_pAttr[i].hdl );
		glVertexAttribPointer( 
			  m.m_pAttr[i].hdl
			, m.m_pAttr[i].size
			, GL_FLOAT
			, GL_FALSE
			, m.m_stride
			, m.m_pAttr[i].pOfs 
		);
	glcheck();
	}
	glEnableVertexAttribArray(0);
	glcheck();

	glActiveTexture( GL_TEXTURE0 );
	glcheck();
	glBindTexture( GL_TEXTURE_2D, hdlTex1 );
	glcheck();

	glActiveTexture( GL_TEXTURE1 );
	glcheck();
	glBindTexture( GL_TEXTURE_2D, hdlTex1 );
	glcheck();

	glActiveTexture( GL_TEXTURE2 );
	glcheck();
	glBindTexture( GL_TEXTURE_2D, hdlTex1 );
	glcheck();

	glActiveTexture( GL_TEXTURE3 );
	glcheck();
	glBindTexture( GL_TEXTURE_2D, hdlTex1 );
	glcheck();

//	glUniform1i(glGetUniformLocation(m.m_hdlprogram, "Tex1"), 0);
	glUniform1i(m.m_shaderTex1, 0);
	glcheck();
//	glUniform1i(glGetUniformLocation(m.m_hdlprogram, "Tex2"), 1);
//	glUniform1i(m.m_shaderTex2, 1);
//	glcheck();
	{//ドットピッチ
		GLfloat		m_viewport[4];
		glGetFloatv( GL_VIEWPORT, m_viewport );
		glcheck();
		float u = 1.0/m_viewport[2];
		float v = 1.0/m_viewport[3];
		glUniform2f(m.m_shaderOfs1, 0,0 );
		glcheck();
		glUniform2f(m.m_shaderOfs2, u,0 );
		glcheck();
		glUniform2f(m.m_shaderOfs3, 0,v );
		glcheck();
		glUniform2f(m.m_shaderOfs4, u,v );
		glcheck();
	}

	glDrawElements( m.m_mode, m.m_cntIndex, GL_UNSIGNED_SHORT, 0 );
	glcheck();
}

//-----------------------------------------------------------------------------
bool	file_malloc_Load( const char* fname, char** ppBuf )
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
				fread(fbuf, 1, flen, fp);
			}
			fclose( fp );
		}
	}
	(*ppBuf) = fbuf;

	return	true;
}

//-----------------------------------------------------------------------------
int main( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	INF_EGL infEgl;
	// EGL 初期化	
	init_egl(infEgl);
	printf("%dx%d\n", infEgl.width, infEgl.height );

	{
        GLint   vert_vectors = 0;
        GLint   frag_vectors = 0;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &vert_vectors);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &frag_vectors);
		printf("vector  %d,%d\n", vert_vectors, frag_vectors );
	}        
	{
        GLint   textureUnits = 0;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
		printf("textureUnits  %d\n", textureUnits );
    }
    {
	   printf( "%s\n", glGetString(GL_VENDOR) );
	   printf( "%s\n", glGetString(GL_RENDERER) );
	   printf( "%s\n", glGetString(GL_VERSION) );
	   printf( "%s\n", glGetString(GL_EXTENSIONS) );
    }
	 printf( "\n--start--\n" );

        
	INF_MODEL model_wf;
	{
		INF_ATTR infAttr[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
		};
		int cntAttr = sizeof(infAttr)/sizeof(INF_ATTR);

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
			"uniform mat4	Mat;												\n"
			"attribute vec3	Pos;												\n"
			"void main() 														\n"
			"{ 																	\n"
			"	 gl_Position = Mat * vec4(Pos,1);								\n" // 左から掛ける
			"}								 									\n"
		;
		const GLchar *srcFlag =
			"void main()														\n"
			"{																	\n"
			"	vec4	col = vec4( 0.0, 1.0, 0.0, 1.0 );						\n"
//			"	gl_FragColor = vec4( 1,1, 1, 1.0);								\n"
			"	col = min(vec4(1.0,1.0,1.0,1.0), col);						\n"//輝度を倍
			"																	\n"
			"	gl_FragColor = col;												\n"
			"}																	\n"
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
			, infAttr
			, cntAttr
		);

	}

	INF_MODEL model_tex;
	{
		INF_ATTR infAttr[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
			{"Uv"	, (void*)(12),	2},		//Uv
		};
		int cntAttr = sizeof(infAttr)/sizeof(INF_ATTR);

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
			"uniform mat4	Mat;												\n"
			"attribute vec3	Pos;												\n"
			"attribute vec2	Uv;													\n"
			"varying vec2	uv;													\n"
			"uniform vec2	ofs1;												\n"
			"void main() 														\n"
			"{ 																	\n"
			"	uv.x = Uv.x-ofs1.x*0.0;													\n"
			"	uv.y = Uv.y-ofs1.y*0.0;													\n"
			"	 gl_Position = Mat * vec4(Pos,1);								\n" // 左から掛ける
			"}								 									\n"
		;
		const GLchar *srcFlag =
			"precision mediump float;											\n"
			"varying	vec2 uv;												\n"
			"uniform sampler2D uTexture;										\n"
			"void main()														\n"
			"{																	\n"
			"	gl_FragColor = texture2D(uTexture,vec2(uv.x,uv.y));				\n"
			"}																	\n"
		;
		gl_setModel( 
			  model_tex
			, tblVert
			, tblIndex
			, srcVert
			, srcFlag
			, cntIndex
			, GL_TRIANGLE_STRIP
			, sizeof(tblVert)
			, infAttr
			, cntAttr
		);
	}

	INF_MODEL model_add;
	{
		INF_ATTR infAttr[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
			{"Uv"	, (void*)(12),	2},		//Uv
		};
		int cntAttr = sizeof(infAttr)/sizeof(INF_ATTR);

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
			"uniform mat4	Mat;												\n"
			"attribute vec3	Pos;												\n"
			"attribute vec2	Uv;													\n"
			"varying vec2	uv1;													\n"
			"varying vec2	uv2;													\n"
			"uniform vec2	ofs1;												\n"
			"void main() 														\n"
			"{ 																	\n"
			"	uv1 = Uv;														\n"
			"	uv2 = Uv+ofs1;														\n"
			"	 gl_Position = Mat * vec4(Pos,1);								\n" // 左から掛ける
			"}								 									\n"
		;
		const GLchar *srcFlag =
			"precision mediump float;											\n"
			"varying	vec2 uv1;												\n"
			"varying	vec2 uv2;												\n"
			"uniform sampler2D Tex1;										\n"
			"uniform sampler2D Tex2;										\n"
			"void main()														\n"
			"{																	\n"
			"	gl_FragColor = ( texture2D(Tex1,vec2(uv1.x,uv1.y))				\n"
			"					+texture2D(Tex2,vec2(uv2.x,uv2.y)));				\n"
			"}																																							\n"
		;
		gl_setModel( 
			  model_add
			, tblVert
			, tblIndex
			, srcVert
			, srcFlag
			, cntIndex
			, GL_TRIANGLE_STRIP
			, sizeof(tblVert)
			, infAttr
			, cntAttr
		);
	}
	INF_MODEL model_boid4;
	{
		INF_ATTR infAttr[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
			{"Uv"	, (void*)(12),	2},		//Uv
		};
		int cntAttr = sizeof(infAttr)/sizeof(INF_ATTR);

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
			"uniform mat4	Mat;												\n"
			"attribute vec3	Pos;												\n"
			"attribute vec2	Uv;													\n"
			"varying vec2	uv1;													\n"
			"varying vec2	uv2;													\n"
			"varying vec2	uv3;													\n"
			"varying vec2	uv4;													\n"
			"uniform vec2	ofs1;												\n"
			"uniform vec2	ofs2;												\n"
			"uniform vec2	ofs3;												\n"
			"uniform vec2	ofs4;												\n"
			"void main() 														\n"
			"{ 																	\n"
			"	uv1 = Uv+ofs1;														\n"
			"	uv2 = Uv+ofs2;														\n"
			"	uv3 = Uv+ofs3;														\n"
			"	uv4 = Uv+ofs4;														\n"
			"	 gl_Position = Mat * vec4(Pos,1);								\n" // 左から掛ける
			"}								 									\n"
		;
		const GLchar *srcFlag =
			"precision mediump float;											\n"
			"varying	vec2 uv1;												\n"
			"varying	vec2 uv2;												\n"
			"varying	vec2 uv3;												\n"
			"varying	vec2 uv4;												\n"
			"uniform sampler2D Tex1;										\n"
			"uniform sampler2D Tex2;										\n"
			"void main()														\n"
			"{																	\n"
			"	gl_FragColor = ( texture2D(Tex1,vec2(uv1.x,uv1.y))				\n"
			"					+texture2D(Tex2,vec2(uv2.x,uv2.y)));				\n"
			"}																																							\n"
		;
		gl_setModel( 
			  model_boid4
			, tblVert
			, tblIndex
			, srcVert
			, srcFlag
			, cntIndex
			, GL_TRIANGLE_STRIP
			, sizeof(tblVert)
			, infAttr
			, cntAttr
		);
	}
	
	INF_MODEL model_gaus_v;
	{
		INF_ATTR infAttr[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
			{"Uv"	, (void*)(12),	2},		//Uv
		};
		int cntAttr = sizeof(infAttr)/sizeof(INF_ATTR);

	
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

		const GLchar *srcVert =
			"uniform mat4 Mat;													\n"
			"attribute vec3 Pos;												\n"
			"attribute vec2 Uv;													\n"
			"varying	vec2 uv;												\n"
			"void main(void)													\n"
			"{																	\n"
			"	uv = Uv;														\n"
			"	gl_Position = Mat * vec4(Pos,1);								\n"
			"}																																							\n"
		;
		const GLchar *srcFlag = 
			"uniform sampler2D	Tex;											\n"
			"varying	vec2	uv;												\n"
			"uniform vec2	ofs1;												\n"
			"																	\n"
			"void main (void)													\n"
			"{																	\n"
			"	vec4	col = vec4( 0.0, 0.0, 0.0, 1.0 );						\n"
			"	float a = 2.0;													\n"
			"																	\n"
			"	col  = a*texture2D( Tex, uv ) * 0.133176 ;						\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*1.0 ) ) * 0.125979;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*1.0 ) ) * 0.125979;	\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*2.0 ) ) * 0.106639;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*2.0 ) ) * 0.106639;	\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*3.0 ) ) * 0.080775;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*3.0 ) ) * 0.080775;	\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*4.0 ) ) * 0.054750;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*4.0 ) ) * 0.054750;	\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*5.0 ) ) * 0.033208;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*5.0 ) ) * 0.033208;	\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*6.0 ) ) * 0.018023;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*6.0 ) ) * 0.018023;	\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*7.0 ) ) * 0.008753;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*7.0 ) ) * 0.008753;	\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*8.0 ) ) * 0.003804;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*8.0 ) ) * 0.003804;	\n"
			"																	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0,  ofs1.y*9.0 ) ) * 0.001479;	\n"
			"	col += a*texture2D(Tex, uv + vec2(  0, -ofs1.y*9.0 ) ) * 0.001479;	\n"
			"																	\n"
			"	col = min(vec4(1.0,1.0,1.0,1.0), col);						\n"//輝度を倍
			"																	\n"
			"	gl_FragColor = col;												\n"
			"}																	\n"
		;
		gl_setModel( 
			  model_gaus_v
			, tblVert
			, tblIndex
			, srcVert
			, srcFlag
			, cntIndex
			, GL_TRIANGLE_STRIP
			, sizeof(tblVert)
			, infAttr
			, cntAttr
		);
	}
	INF_MODEL model_gaus_h;
	{
		INF_ATTR infAttr[] = 
		{
			{"Pos"	, (void*)(0),	3},		//Pos
			{"Uv"	, (void*)(12),	2},		//Uv
		};
		int cntAttr = sizeof(infAttr)/sizeof(INF_ATTR);
		
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

		const GLchar *srcVert =
			"uniform mat4 Mat;													\n"
			"attribute vec3 Pos;												\n"
			"attribute vec2 Uv;													\n"
			"varying	vec2 uv;												\n"
			"uniform vec2	ofs1;												\n"
			"void main(void)													\n"
			"{																	\n"
			"	uv = Uv-vec2(ofs1.x*2.0,0);														\n"
			"	gl_Position = Mat * vec4(Pos,1);								\n"
			"}																																							\n"
		;
		const GLchar *srcFlag =
			"uniform sampler2D	Tex;																																						\n"
			"varying vec2	uv;												\n"
			"uniform vec2	ofs1;												\n"
			"																	\n"
			"void main (void)													\n"
			"{																	\n"
			"	vec4	col = vec4( 0.0, 0.0, 0.0, 1.0 );						\n"
			"																	\n"
			"																	\n"
			"	col  = texture2D( Tex, uv ) * 0.133176 ;						\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*1.0, 0 ) ) * 0.125979;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*1.0, 0 ) ) * 0.125979;	\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*2.0, 0 ) ) * 0.106639;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*2.0, 0 ) ) * 0.106639;	\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*3.0, 0 ) ) * 0.080775;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*3.0, 0 ) ) * 0.080775;	\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*4.0, 0 ) ) * 0.054750;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*4.0, 0 ) ) * 0.054750;	\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*5.0, 0 ) ) * 0.033208;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*5.0, 0 ) ) * 0.033208;	\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*6.0, 0 ) ) * 0.018023;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*6.0, 0 ) ) * 0.018023;	\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*7.0, 0 ) ) * 0.008753;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*7.0, 0 ) ) * 0.008753;	\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*8.0, 0 ) ) * 0.003804;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*8.0, 0 ) ) * 0.003804;	\n"
			"																	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*9.0, 0 ) ) * 0.001479;	\n"
			"	col += texture2D(Tex, uv + vec2(  ofs1.x*9.0, 0 ) ) * 0.001479;	\n"
			"																	\n"
			"	col = min(vec4(1.0,1.0,1.0,1.0), col);							\n"//輝度を倍
			"																	\n"
			"	gl_FragColor = col;												\n"
			"}																	\n"
		;
		gl_setModel( 
			  model_gaus_h
			, tblVert
			, tblIndex
			, srcVert
			, srcFlag
			, cntIndex
			, GL_TRIANGLE_STRIP
			, sizeof(tblVert)
			, infAttr
			, cntAttr
		);
	}
	

	INF_FBO	fbo1a( 1280, 720, GL_LINEAR );
	INF_FBO	fbo1b( 1280, 720, GL_LINEAR );
	INF_FBO	fbo2a( 1280/2, 720/2, GL_LINEAR );
	INF_FBO	fbo2b( 1280/2, 720/2, GL_LINEAR );
	INF_FBO	fbo4a( 1280/4, 720/4, GL_LINEAR );
	INF_FBO	fbo4b( 1280/4, 720/4, GL_LINEAR );
	INF_FBO	fbo8a( 1280/8, 720/8, GL_LINEAR );
	INF_FBO	fbo8b( 1280/8, 720/8, GL_LINEAR );
	

	vect44	matPers;
	vect44	matModel;


	// GL環境設定
	glEnable(GL_CULL_FACE);
	glClearColor(0.15f, 0.25f, 0.35f, 1.0f);

	vect44 mIdent;	

	while(1) 
	{
		glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, infEgl.width, infEgl.height);


			static float rad =0;
			rad += RAD(1/2.0);
//		rad = 2;

		fbo1a.begin();

	        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			// 射影行列をセット
			matPers.setPerspective( 27.5f, 1920.0/1080.0 ); 

			// モデル行列をセット
			matModel.identity();

			matModel.rotX(rad/3);
			matModel.rotY(-rad);
			matModel.translate(2, 0, 6 );
			vect44 mat_wf = matModel*matPers;
			gl_drawModel( model_wf, mat_wf );

#if 1
			// 太線
			fbo1b.begin();	gl_drawModelBoid4( model_boid4, mIdent, fbo1a.m_color_tex );

			// 1/8
			fbo2a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo1b.m_color_tex );
			fbo4a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo2a.m_color_tex );
			fbo8a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo4a.m_color_tex );

			fbo8b.begin();	gl_drawModelTex1( model_gaus_v, mIdent, fbo8a.m_color_tex );
			fbo8a.begin();	gl_drawModelTex1( model_gaus_h, mIdent, fbo8b.m_color_tex );

		fbo1a.end();

		gl_drawModelTex2b( model_add, mIdent, fbo8a.m_color_tex, fbo1a.m_color_tex );
//		gl_drawModelTex1( model_add, mIdent, fbo8a.m_color_tex );

#endif
#if 0
			// 1/8
			fbo2a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo1a.m_color_tex );
			fbo4a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo2a.m_color_tex );
			fbo8a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo4a.m_color_tex );

			fbo8b.begin();	gl_drawModelTex1( model_gaus_v, mIdent, fbo8a.m_color_tex );
			fbo8a.begin();	gl_drawModelTex1( model_gaus_h, mIdent, fbo8b.m_color_tex );

		fbo1a.end();

		gl_drawModelTex2b( model_add, mIdent, fbo8a.m_color_tex, fbo1a.m_color_tex );
//		gl_drawModelTex1( model_add, mIdent, fbo8a.m_color_tex );
#endif
#if 0
			// 1/4
			fbo2a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo1a.m_color_tex );
			fbo4a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo2a.m_color_tex );

			fbo4b.begin();	gl_drawModelTex1( model_gaus_v, mIdent, fbo4a.m_color_tex );
			fbo4a.begin();	gl_drawModelTex1( model_gaus_h, mIdent, fbo4b.m_color_tex );

		fbo1a.end();

		gl_drawModelTex2b( model_add, mIdent, fbo4a.m_color_tex, fbo1a.m_color_tex );
//		gl_drawModelTex1( model_add, mIdent, fbo8a.m_color_tex );
#endif
#if 0
			// 1/2
			fbo2a.begin();	gl_drawModelTex1( model_tex, mIdent, fbo1a.m_color_tex );
			fbo2b.begin();	gl_drawModelTex1( model_gaus_v, mIdent, fbo2a.m_color_tex );
			fbo2a.begin();	gl_drawModelTex1( model_gaus_h, mIdent, fbo2b.m_color_tex );

		fbo1a.end();

//		gl_drawModelTex2b( model_add, mIdent, fbo2a.m_color_tex, fbo1a.m_color_tex );
		gl_drawModelTex1( model_add, mIdent, fbo2a.m_color_tex );


#endif

		{
			matModel.identity();

//			static float rad =0;
//			rad += RAD(1/2.0);
//		rad = -2;
			matModel.rotX(rad/3);
			matModel.rotY(rad);
			matModel.translate(-2, 0, 6 );
			vect44 mat_wf = matModel*matPers;
			gl_drawModel( model_wf, mat_wf );
		}

		//---
		eglSwapBuffers(infEgl.display, infEgl.surface);
	}
	return 0;
}
