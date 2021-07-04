/**
 * @file	GLFW_ext.h
 * @author	SiS.
 *
 * @version 1.00.00B.
 *
 * OpenGL (Simple) Framework: OpenGL extensions header file.
 *
 */

#ifndef __GLFW_EXT__
#define __GLFW_EXT__

#ifndef __GLFW__
#error "Cannot include glfw_ext.h alone!"
#endif


//////////////////////////////////////////////////////////////////////////
//  #Include section.
//////////////////////////////////////////////////////////////////////////

// System include.

// OpenGL Headers.
#include <GL\glext.h>	// OpenGL extensions header.
#include <GL\wglext.h>	// OpenGL Windows extensions header.



//////////////////////////////////////////////////////////////////////////
//  OpenGL extensions.
//////////////////////////////////////////////////////////////////////////



#ifdef GL_VERSION_1_2

// void glBlendColor (GLclampf, GLclampf, GLclampf, GLclampf);
extern PFNGLBLENDCOLORPROC glBlendColor ;

// void glBlendEquation (GLenum);
extern PFNGLBLENDEQUATIONPROC glBlendEquation;

// void glDrawRangeElements (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *);
extern PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;

// void glColorTable (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
extern PFNGLCOLORTABLEPROC glColorTable;

// void glColorTableParameterfv (GLenum, GLenum, const GLfloat *);
extern PFNGLCOLORTABLEPARAMETERFVPROC glColorTableParameterfv;

// void glColorTableParameteriv (GLenum, GLenum, const GLint *);
extern PFNGLCOLORTABLEPARAMETERIVPROC glColorTableParameteriv;

// void glCopyColorTable (GLenum, GLenum, GLint, GLint, GLsizei);
extern PFNGLCOPYCOLORTABLEPROC glCopyColorTable;

// void glGetColorTable (GLenum, GLenum, GLenum, GLvoid *);
extern PFNGLGETCOLORTABLEPROC glGetColorTable;

// void glGetColorTableParameterfv (GLenum, GLenum, GLfloat *);
extern PFNGLGETCOLORTABLEPARAMETERFVPROC glGetColorTableParameterfv;

// void glGetColorTableParameteriv (GLenum, GLenum, GLint *);
extern PFNGLGETCOLORTABLEPARAMETERIVPROC glGetColorTableParameteriv;

// void glColorSubTable (GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
extern PFNGLCOLORSUBTABLEPROC glColorSubTable;

// void glCopyColorSubTable (GLenum, GLsizei, GLint, GLint, GLsizei);
extern PFNGLCOPYCOLORSUBTABLEPROC glCopyColorSubTable;

// void glConvolutionFilter1D (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
extern PFNGLCONVOLUTIONFILTER1DPROC glConvolutionFilter1D;

// void glConvolutionFilter2D (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
extern PFNGLCONVOLUTIONFILTER2DPROC glConvolutionFilter2D;

// void glConvolutionParameterf (GLenum, GLenum, GLfloat);
extern PFNGLCONVOLUTIONPARAMETERFPROC glConvolutionParameterf;

// void glConvolutionParameterfv (GLenum, GLenum, const GLfloat *);
extern PFNGLCONVOLUTIONPARAMETERFVPROC glConvolutionParameterfv;

// void glConvolutionParameteri (GLenum, GLenum, GLint);
extern PFNGLCONVOLUTIONPARAMETERIPROC glConvolutionParameteri;

// void glConvolutionParameteriv (GLenum, GLenum, const GLint *);
extern PFNGLCONVOLUTIONPARAMETERIVPROC glConvolutionParameteriv;

// void glCopyConvolutionFilter1D (GLenum, GLenum, GLint, GLint, GLsizei);
extern PFNGLCOPYCONVOLUTIONFILTER1DPROC glCopyConvolutionFilter1D;

// void glCopyConvolutionFilter2D (GLenum, GLenum, GLint, GLint, GLsizei, GLsizei);
extern PFNGLCOPYCONVOLUTIONFILTER2DPROC glCopyConvolutionFilter2D;

// void glGetConvolutionFilter (GLenum, GLenum, GLenum, GLvoid *);
extern PFNGLGETCONVOLUTIONFILTERPROC glGetConvolutionFilter;

// void glGetConvolutionParameterfv (GLenum, GLenum, GLfloat *);
extern PFNGLGETCONVOLUTIONPARAMETERFVPROC glGetConvolutionParameterfv;

// void glGetConvolutionParameteriv (GLenum, GLenum, GLint *);
extern PFNGLGETCONVOLUTIONPARAMETERIVPROC glGetConvolutionParameteriv;

// void glGetSeparableFilter (GLenum, GLenum, GLenum, GLvoid *, GLvoid *, GLvoid *);
extern PFNGLGETSEPARABLEFILTERPROC glGetSeparableFilter;

// void glSeparableFilter2D (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid *);
extern PFNGLSEPARABLEFILTER2DPROC glSeparableFilter2D;

// void glGetHistogram (GLenum, GLboolean, GLenum, GLenum, GLvoid *);
extern PFNGLGETHISTOGRAMPROC glGetHistogram;

// void glGetHistogramParameterfv (GLenum, GLenum, GLfloat *);
extern PFNGLGETHISTOGRAMPARAMETERFVPROC glGetHistogramParameterfv;

// void glGetHistogramParameteriv (GLenum, GLenum, GLint *);
extern PFNGLGETHISTOGRAMPARAMETERIVPROC glGetHistogramParameteriv;

// void glGetMinmax (GLenum, GLboolean, GLenum, GLenum, GLvoid *);
extern PFNGLGETMINMAXPROC glGetMinmax;

// void glGetMinmaxParameterfv (GLenum, GLenum, GLfloat *);
extern PFNGLGETMINMAXPARAMETERFVPROC glGetMinmaxParameterfv;

// void glGetMinmaxParameteriv (GLenum, GLenum, GLint *);
extern PFNGLGETMINMAXPARAMETERIVPROC glGetMinmaxParameteriv;

// void glHistogram (GLenum, GLsizei, GLenum, GLboolean);
extern PFNGLHISTOGRAMPROC glHistogram;
 
// void glMinmax (GLenum, GLenum, GLboolean);
extern PFNGLMINMAXPROC glMinmax;

// void glResetHistogram (GLenum);
extern PFNGLRESETHISTOGRAMPROC glResetHistogram;

// void glResetMinmax (GLenum);
extern PFNGLRESETMINMAXPROC glResetMinmax;

// void glTexImage3D (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
extern PFNGLTEXIMAGE3DPROC glTexImage3D;

// void glTexSubImage3D (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
extern PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;

// void glCopyTexSubImage3D (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
extern PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;

#endif // #ifdef GL_VERSION_1_2


//////////////////////////////////////////////////////////////////////////


#ifdef GL_VERSION_1_3

// void glActiveTexture (GLenum);
extern PFNGLACTIVETEXTUREPROC glActiveTexture;

// void glClientActiveTexture (GLenum);
extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;

// void glMultiTexCoord1d (GLenum, GLdouble);
extern PFNGLMULTITEXCOORD1DPROC glMultiTexCoord1d;

// void glMultiTexCoord1dv (GLenum, const GLdouble *);
extern PFNGLMULTITEXCOORD1DVPROC glMultiTexCoord1dv;

// void glMultiTexCoord1f (GLenum, GLfloat);
extern PFNGLMULTITEXCOORD1FPROC glMultiTexCoord1f;

// void glMultiTexCoord1fv (GLenum, const GLfloat *);
extern PFNGLMULTITEXCOORD1FVPROC glMultiTexCoord1fv;

// void glMultiTexCoord1i (GLenum, GLint);
extern PFNGLMULTITEXCOORD1IPROC glMultiTexCoord1i;

// void glMultiTexCoord1iv (GLenum, const GLint *);
extern PFNGLMULTITEXCOORD1IVPROC glMultiTexCoord1iv;

// void glMultiTexCoord1s (GLenum, GLshort);
extern PFNGLMULTITEXCOORD1SPROC glMultiTexCoord1s;

// void glMultiTexCoord1sv (GLenum, const GLshort *);
extern PFNGLMULTITEXCOORD1SVPROC glMultiTexCoord1sv;

// void glMultiTexCoord2d (GLenum, GLdouble, GLdouble);
extern PFNGLMULTITEXCOORD2DPROC glMultiTexCoord2d;

// void glMultiTexCoord2dv (GLenum, const GLdouble *);
extern PFNGLMULTITEXCOORD2DVPROC glMultiTexCoord2dv;

// void glMultiTexCoord2f (GLenum, GLfloat, GLfloat);
extern PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;

// void glMultiTexCoord2fv (GLenum, const GLfloat *);
extern PFNGLMULTITEXCOORD2FVPROC glMultiTexCoord2fv;

// void glMultiTexCoord2i (GLenum, GLint, GLint);
extern PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i;

// void glMultiTexCoord2iv (GLenum, const GLint *);
extern PFNGLMULTITEXCOORD2IVPROC glMultiTexCoord2iv;

// void glMultiTexCoord2s (GLenum, GLshort, GLshort);
extern PFNGLMULTITEXCOORD2SPROC glMultiTexCoord2s;

// void glMultiTexCoord2sv (GLenum, const GLshort *);
extern PFNGLMULTITEXCOORD2SVPROC glMultiTexCoord2sv;

// void glMultiTexCoord3d (GLenum, GLdouble, GLdouble, GLdouble);
extern PFNGLMULTITEXCOORD3DPROC glMultiTexCoord3d;

// void glMultiTexCoord3dv (GLenum, const GLdouble *);
extern PFNGLMULTITEXCOORD3DVPROC glMultiTexCoord3dv;

// void glMultiTexCoord3f (GLenum, GLfloat, GLfloat, GLfloat);
extern PFNGLMULTITEXCOORD3FPROC glMultiTexCoord3f;

// void glMultiTexCoord3fv (GLenum, const GLfloat *);
extern PFNGLMULTITEXCOORD3FVPROC glMultiTexCoord3fv;

// void glMultiTexCoord3i (GLenum, GLint, GLint, GLint);
extern PFNGLMULTITEXCOORD3IPROC glMultiTexCoord3i;

// void glMultiTexCoord3iv (GLenum, const GLint *);
extern PFNGLMULTITEXCOORD3IVPROC glMultiTexCoord3iv;

// void glMultiTexCoord3s (GLenum, GLshort, GLshort, GLshort);
extern PFNGLMULTITEXCOORD3SPROC glMultiTexCoord3s;

// void glMultiTexCoord3sv (GLenum, const GLshort *);
extern PFNGLMULTITEXCOORD3SVPROC glMultiTexCoord3sv;

// void glMultiTexCoord4d (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
extern PFNGLMULTITEXCOORD4DPROC glMultiTexCoord4d;

// void glMultiTexCoord4dv (GLenum, const GLdouble *);
extern PFNGLMULTITEXCOORD4DVPROC glMultiTexCoord4dv;

// void glMultiTexCoord4f (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
extern PFNGLMULTITEXCOORD4FPROC glMultiTexCoord4f;

// void glMultiTexCoord4fv (GLenum, const GLfloat *);
extern PFNGLMULTITEXCOORD4FVPROC glMultiTexCoord4fv;

// void glMultiTexCoord4i (GLenum, GLint, GLint, GLint, GLint);
extern PFNGLMULTITEXCOORD4IPROC glMultiTexCoord4i;

// void glMultiTexCoord4iv (GLenum, const GLint *);
extern PFNGLMULTITEXCOORD4IVPROC glMultiTexCoord4iv;

// void glMultiTexCoord4s (GLenum, GLshort, GLshort, GLshort, GLshort);
extern PFNGLMULTITEXCOORD4SPROC glMultiTexCoord4s;

// void glMultiTexCoord4sv (GLenum, const GLshort *);
extern PFNGLMULTITEXCOORD4SVPROC glMultiTexCoord4sv;

// void glLoadTransposeMatrixf (const GLfloat *);
extern PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;

// void glLoadTransposeMatrixd (const GLdouble *);
extern PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd;

// void glMultTransposeMatrixf (const GLfloat *);
extern PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf;

// void glMultTransposeMatrixd (const GLdouble *);
extern PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd;

// void glSampleCoverage (GLclampf, GLboolean);
extern PFNGLSAMPLECOVERAGEPROC glSampleCoverage;

// void glCompressedTexImage3D (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;

// void glCompressedTexImage2D (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;

// void glCompressedTexImage1D (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *);
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;

// void glCompressedTexSubImage3D (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;

// void glCompressedTexSubImage2D (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;

// void glCompressedTexSubImage1D (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *);
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;

// void glGetCompressedTexImage (GLenum, GLint, GLvoid *);
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;

#endif // #ifdef GL_VERSION_1_3


//////////////////////////////////////////////////////////////////////////


#ifdef GL_VERSION_1_4

// void glBlendFuncSeparate (GLenum, GLenum, GLenum, GLenum);
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;

// void glFogCoordf (GLfloat);
extern PFNGLFOGCOORDFPROC glFogCoordf;

// void glFogCoordfv (const GLfloat *);
extern PFNGLFOGCOORDFVPROC glFogCoordfv;

// void glFogCoordd (GLdouble);
extern PFNGLFOGCOORDDPROC glFogCoordd;

// void glFogCoorddv (const GLdouble *);
extern PFNGLFOGCOORDDVPROC glFogCoorddv;

// void glFogCoordPointer (GLenum, GLsizei, const GLvoid *);
extern PFNGLFOGCOORDPOINTERPROC glFogCoordPointer;

// void glMultiDrawArrays (GLenum, GLint *, GLsizei *, GLsizei);
extern PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;

// void glMultiDrawElements (GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei);
extern PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;

// void glPointParameterf (GLenum, GLfloat);
extern PFNGLPOINTPARAMETERFPROC glPointParameterf;

// void glPointParameterfv (GLenum, const GLfloat *);
extern PFNGLPOINTPARAMETERFVPROC glPointParameterfv;

// void glPointParameteri (GLenum, GLint);
extern PFNGLPOINTPARAMETERIPROC glPointParameteri;

// void glPointParameteriv (GLenum, const GLint *);
extern PFNGLPOINTPARAMETERIVPROC glPointParameteriv;

// void glSecondaryColor3b (GLbyte, GLbyte, GLbyte);
extern PFNGLSECONDARYCOLOR3BPROC glSecondaryColor3b;

// void glSecondaryColor3bv (const GLbyte *);
extern PFNGLSECONDARYCOLOR3BVPROC glSecondaryColor3bv;

// void glSecondaryColor3d (GLdouble, GLdouble, GLdouble);
extern PFNGLSECONDARYCOLOR3DPROC glSecondaryColor3d;

// void glSecondaryColor3dv (const GLdouble *);
extern PFNGLSECONDARYCOLOR3DVPROC glSecondaryColor3dv;

// void glSecondaryColor3f (GLfloat, GLfloat, GLfloat);
extern PFNGLSECONDARYCOLOR3FPROC glSecondaryColor3f;

// void glSecondaryColor3fv (const GLfloat *);
extern PFNGLSECONDARYCOLOR3FVPROC glSecondaryColor3fv;

// void glSecondaryColor3i (GLint, GLint, GLint);
extern PFNGLSECONDARYCOLOR3IPROC glSecondaryColor3i;

 // void glSecondaryColor3iv (const GLint *);
extern PFNGLSECONDARYCOLOR3IVPROC glSecondaryColor3iv;

// void glSecondaryColor3s (GLshort, GLshort, GLshort);
extern PFNGLSECONDARYCOLOR3SPROC glSecondaryColor3s;

// void glSecondaryColor3sv (const GLshort *);
extern PFNGLSECONDARYCOLOR3SVPROC glSecondaryColor3sv;

// void glSecondaryColor3ub (GLubyte, GLubyte, GLubyte);
extern PFNGLSECONDARYCOLOR3UBPROC glSecondaryColor3ub;

// void glSecondaryColor3ubv (const GLubyte *);
extern PFNGLSECONDARYCOLOR3UBVPROC glSecondaryColor3ubv;

// void glSecondaryColor3ui (GLuint, GLuint, GLuint);
extern PFNGLSECONDARYCOLOR3UIPROC glSecondaryColor3ui;

// void glSecondaryColor3uiv (const GLuint *);
extern PFNGLSECONDARYCOLOR3UIVPROC glSecondaryColor3uiv;

// void glSecondaryColor3us (GLushort, GLushort, GLushort);
extern PFNGLSECONDARYCOLOR3USPROC glSecondaryColor3us;

// void glSecondaryColor3usv (const GLushort *);
extern PFNGLSECONDARYCOLOR3USVPROC glSecondaryColor3usv;

// void glSecondaryColorPointer (GLint, GLenum, GLsizei, const GLvoid *);
extern PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer;

// void glWindowPos2d (GLdouble, GLdouble);
extern PFNGLWINDOWPOS2DPROC glWindowPos2d;

// void glWindowPos2dv (const GLdouble *);
extern PFNGLWINDOWPOS2DVPROC glWindowPos2dv;

// void glWindowPos2f (GLfloat, GLfloat);
extern PFNGLWINDOWPOS2FPROC glWindowPos2f;

// void glWindowPos2fv (const GLfloat *);
extern PFNGLWINDOWPOS2FVPROC glWindowPos2fv;

// void glWindowPos2i (GLint, GLint);
extern PFNGLWINDOWPOS2IPROC glWindowPos2i;

// void glWindowPos2iv (const GLint *);
extern PFNGLWINDOWPOS2IVPROC glWindowPos2iv;

// void glWindowPos2s (GLshort, GLshort);
extern PFNGLWINDOWPOS2SPROC glWindowPos2s;

// void glWindowPos2sv (const GLshort *);
extern PFNGLWINDOWPOS2SVPROC glWindowPos2sv;

// void glWindowPos3d (GLdouble, GLdouble, GLdouble);
extern PFNGLWINDOWPOS3DPROC glWindowPos3d;

// void glWindowPos3dv (const GLdouble *);
extern PFNGLWINDOWPOS3DVPROC glWindowPos3dv;

// void glWindowPos3f (GLfloat, GLfloat, GLfloat);
extern PFNGLWINDOWPOS3FPROC glWindowPos3f;

// void glWindowPos3fv (const GLfloat *);
extern PFNGLWINDOWPOS3FVPROC glWindowPos3fv;

// void glWindowPos3i (GLint, GLint, GLint);
extern PFNGLWINDOWPOS3IPROC glWindowPos3i;

// void glWindowPos3iv (const GLint *);
extern PFNGLWINDOWPOS3IVPROC glWindowPos3iv;

// void glWindowPos3s (GLshort, GLshort, GLshort);
extern PFNGLWINDOWPOS3SPROC glWindowPos3s;

 // void glWindowPos3sv (const GLshort *);
extern PFNGLWINDOWPOS3SVPROC glWindowPos3sv;

#endif // #ifdef GL_VERSION_1_4


//////////////////////////////////////////////////////////////////////////


#ifdef GL_VERSION_1_5

// void glGenQueries (GLsizei, GLuint *);
extern PFNGLGENQUERIESPROC glGenQueries;

// void glDeleteQueries (GLsizei, const GLuint *);
extern PFNGLDELETEQUERIESPROC glDeleteQueries;

// GLboolean glIsQuery (GLuint);
extern PFNGLISQUERYPROC glIsQuery;

// void glBeginQuery (GLenum, GLuint);
extern PFNGLBEGINQUERYPROC glBeginQuery;

// void glEndQuery (GLenum);
extern PFNGLENDQUERYPROC glEndQuery;

// void glGetQueryiv (GLenum, GLenum, GLint *);
extern PFNGLGETQUERYIVPROC glGetQueryiv;

// void glGetQueryObjectiv (GLuint, GLenum, GLint *);
extern PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;

// void glGetQueryObjectuiv (GLuint, GLenum, GLuint *);
extern PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;

// void glBindBuffer (GLenum, GLuint);
extern PFNGLBINDBUFFERPROC glBindBuffer;

// void glDeleteBuffers (GLsizei, const GLuint *);
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;

// void glGenBuffers (GLsizei, GLuint *);
extern PFNGLGENBUFFERSPROC glGenBuffers;

// GLboolean glIsBuffer (GLuint);
extern PFNGLISBUFFERPROC glIsBuffer;

// void glBufferData (GLenum, GLsizeiptr, const GLvoid *, GLenum);
extern PFNGLBUFFERDATAPROC glBufferData;

// void glBufferSubData (GLenum, GLintptr, GLsizeiptr, const GLvoid *);
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;

// void glGetBufferSubData (GLenum, GLintptr, GLsizeiptr, GLvoid *);
extern PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;

// GLvoid* glMapBuffer (GLenum, GLenum);
extern PFNGLMAPBUFFERPROC glMapBuffer;

// GLboolean glUnmapBuffer (GLenum);
extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;

// void glGetBufferParameteriv (GLenum, GLenum, GLint *);
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;

// void glGetBufferPointerv (GLenum, GLenum, GLvoid* *);
extern PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;


#endif // #ifdef GL_VERSION_1_5


//////////////////////////////////////////////////////////////////////////


#ifdef GL_VERSION_2_0


// void glBlendEquationSeparate (GLenum, GLenum);
extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;

// void glDrawBuffers (GLsizei, const GLenum *);
extern PFNGLDRAWBUFFERSPROC glDrawBuffers;

// void glStencilOpSeparate (GLenum, GLenum, GLenum, GLenum);
extern PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;

// void glStencilFuncSeparate (GLenum, GLenum, GLint, GLuint);
extern PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;

// void glStencilMaskSeparate (GLenum, GLuint);
extern PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;

// void glAttachShader (GLuint, GLuint);
extern PFNGLATTACHSHADERPROC glAttachShader;

// void glBindAttribLocation (GLuint, GLuint, const GLchar *);
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;

// void glCompileShader (GLuint);
extern PFNGLCOMPILESHADERPROC glCompileShader;

// GLuint glCreateProgram (void);
extern PFNGLCREATEPROGRAMPROC glCreateProgram;

// GLuint glCreateShader (GLenum);
extern PFNGLCREATESHADERPROC glCreateShader;

// void glDeleteProgram (GLuint);
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;

// void glDeleteShader (GLuint);
extern PFNGLDELETESHADERPROC glDeleteShader;

// void glDetachShader (GLuint, GLuint);
extern PFNGLDETACHSHADERPROC glDetachShader;

// void glDisableVertexAttribArray (GLuint);
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

// void glEnableVertexAttribArray (GLuint);
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

// void glGetActiveAttrib (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;

// void glGetActiveUniform (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;

// void glGetAttachedShaders (GLuint, GLsizei, GLsizei *, GLuint *);
extern PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;

// GLint glGetAttribLocation (GLuint, const GLchar *);
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;

// void glGetProgramiv (GLuint, GLenum, GLint *);
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;

// void glGetProgramInfoLog (GLuint, GLsizei, GLsizei *, GLchar *);
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

// void glGetShaderiv (GLuint, GLenum, GLint *);
extern PFNGLGETSHADERIVPROC glGetShaderiv;

// void glGetShaderInfoLog (GLuint, GLsizei, GLsizei *, GLchar *);
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

// void glGetShaderSource (GLuint, GLsizei, GLsizei *, GLchar *);
extern PFNGLGETSHADERSOURCEPROC glGetShaderSource;

// GLint glGetUniformLocation (GLuint, const GLchar *);
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;

// void glGetUniformfv (GLuint, GLint, GLfloat *);
extern PFNGLGETUNIFORMFVPROC glGetUniformfv;

// void glGetUniformiv (GLuint, GLint, GLint *);
extern PFNGLGETUNIFORMIVPROC glGetUniformiv;

// void glGetVertexAttribdv (GLuint, GLenum, GLdouble *);
extern PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;

// void glGetVertexAttribfv (GLuint, GLenum, GLfloat *);
extern PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;

// void glGetVertexAttribiv (GLuint, GLenum, GLint *);
extern PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;

// void glGetVertexAttribPointerv (GLuint, GLenum, GLvoid* *);
extern PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;

// GLboolean glIsProgram (GLuint);
extern PFNGLISPROGRAMPROC glIsProgram;

// GLboolean glIsShader (GLuint);
extern PFNGLISSHADERPROC glIsShader;

// void glLinkProgram (GLuint);
extern PFNGLLINKPROGRAMPROC glLinkProgram;

// void glShaderSource (GLuint, GLsizei, const GLchar* *, const GLint *);
extern PFNGLSHADERSOURCEPROC glShaderSource;

// void glUseProgram (GLuint);
extern PFNGLUSEPROGRAMPROC glUseProgram;

// void glUniform1f (GLint, GLfloat);
extern PFNGLUNIFORM1FPROC glUniform1f;

// void glUniform2f (GLint, GLfloat, GLfloat);
extern PFNGLUNIFORM2FPROC glUniform2f;

// void glUniform3f (GLint, GLfloat, GLfloat, GLfloat);
extern PFNGLUNIFORM3FPROC glUniform3f;

// void glUniform4f (GLint, GLfloat, GLfloat, GLfloat, GLfloat);
extern PFNGLUNIFORM4FPROC glUniform4f;

// void glUniform1i (GLint, GLint);
extern PFNGLUNIFORM1IPROC glUniform1i;

// void glUniform2i (GLint, GLint, GLint);
extern PFNGLUNIFORM2IPROC glUniform2i;

// void glUniform3i (GLint, GLint, GLint, GLint);
extern PFNGLUNIFORM3IPROC glUniform3i;

// void glUniform4i (GLint, GLint, GLint, GLint, GLint);
extern PFNGLUNIFORM4IPROC glUniform4i;

// void glUniform1fv (GLint, GLsizei, const GLfloat *);
extern PFNGLUNIFORM1FVPROC glUniform1fv;

// void glUniform2fv (GLint, GLsizei, const GLfloat *);
extern PFNGLUNIFORM2FVPROC glUniform2fv;

// void glUniform3fv (GLint, GLsizei, const GLfloat *);
extern PFNGLUNIFORM3FVPROC glUniform3fv;

// void glUniform4fv (GLint, GLsizei, const GLfloat *);
extern PFNGLUNIFORM4FVPROC glUniform4fv;

// void glUniform1iv (GLint, GLsizei, const GLint *);
extern PFNGLUNIFORM1IVPROC glUniform1iv;

// void glUniform2iv (GLint, GLsizei, const GLint *);
extern PFNGLUNIFORM2IVPROC glUniform2iv;

// void glUniform3iv (GLint, GLsizei, const GLint *);
extern PFNGLUNIFORM3IVPROC glUniform3iv;

// void glUniform4iv (GLint, GLsizei, const GLint *);
extern PFNGLUNIFORM4IVPROC glUniform4iv;

// void glUniformMatrix2fv (GLint, GLsizei, GLboolean, const GLfloat *);
extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;

// void glUniformMatrix3fv (GLint, GLsizei, GLboolean, const GLfloat *);
extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;

// void glUniformMatrix4fv (GLint, GLsizei, GLboolean, const GLfloat *);
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

// void glValidateProgram (GLuint);
extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;

// void glVertexAttrib1d (GLuint, GLdouble);
extern PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;

// void glVertexAttrib1dv (GLuint, const GLdouble *);
extern PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;

// void glVertexAttrib1f (GLuint, GLfloat);
extern PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;

// void glVertexAttrib1fv (GLuint, const GLfloat *);
extern PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;

// void glVertexAttrib1s (GLuint, GLshort);
extern PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;

// void glVertexAttrib1sv (GLuint, const GLshort *);
extern PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;

// void glVertexAttrib2d (GLuint, GLdouble, GLdouble);
extern PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;

// void glVertexAttrib2dv (GLuint, const GLdouble *);
extern PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;

// void glVertexAttrib2f (GLuint, GLfloat, GLfloat);
extern PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;

// void glVertexAttrib2fv (GLuint, const GLfloat *);
extern PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;

// void glVertexAttrib2s (GLuint, GLshort, GLshort);
extern PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;

// void glVertexAttrib2sv (GLuint, const GLshort *);
extern PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;

// void glVertexAttrib3d (GLuint, GLdouble, GLdouble, GLdouble);
extern PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;

// void glVertexAttrib3dv (GLuint, const GLdouble *);
extern PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;

// void glVertexAttrib3f (GLuint, GLfloat, GLfloat, GLfloat);
extern PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;

// void glVertexAttrib3fv (GLuint, const GLfloat *);
extern PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;

// void glVertexAttrib3s (GLuint, GLshort, GLshort, GLshort);
extern PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;

// void glVertexAttrib3sv (GLuint, const GLshort *);
extern PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;

// void glVertexAttrib4Nbv (GLuint, const GLbyte *);
extern PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;

// void glVertexAttrib4Niv (GLuint, const GLint *);
extern PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;

// void glVertexAttrib4Nsv (GLuint, const GLshort *);
extern PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;

// void glVertexAttrib4Nub (GLuint, GLubyte, GLubyte, GLubyte, GLubyte);
extern PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;

// void glVertexAttrib4Nubv (GLuint, const GLubyte *);
extern PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;

// void glVertexAttrib4Nuiv (GLuint, const GLuint *);
extern PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;

// void glVertexAttrib4Nusv (GLuint, const GLushort *);
extern PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;

// void glVertexAttrib4bv (GLuint, const GLbyte *);
extern PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;

// void glVertexAttrib4d (GLuint, GLdouble, GLdouble, GLdouble, GLdouble);
extern PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;

// void glVertexAttrib4dv (GLuint, const GLdouble *);
extern PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;

// void glVertexAttrib4f (GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
extern PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;

// void glVertexAttrib4fv (GLuint, const GLfloat *);
extern PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;

// void glVertexAttrib4iv (GLuint, const GLint *);
extern PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;

// void glVertexAttrib4s (GLuint, GLshort, GLshort, GLshort, GLshort);
extern PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;

// void glVertexAttrib4sv (GLuint, const GLshort *);
extern PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;

// void glVertexAttrib4ubv (GLuint, const GLubyte *);
extern PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;

// void glVertexAttrib4uiv (GLuint, const GLuint *);
extern PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;

// void glVertexAttrib4usv (GLuint, const GLushort *);
extern PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;

// void glVertexAttribPointer (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;


#endif // #ifdef GL_VERSION_2_0




//////////////////////////////////////////////////////////////////////////
//  END of Code.
//////////////////////////////////////////////////////////////////////////

#endif // #ifndef __GLFW_EXT__