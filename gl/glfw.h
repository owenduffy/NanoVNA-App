/**
 * @file	GLFW.h
 * @author	SiS.
 *
 * @version 1.00.00B.
 *
 * OpenGL (Simple) Framework: Main header file.
 *
 */

#ifndef __GLFW__
#define __GLFW__


//////////////////////////////////////////////////////////////////////////
//  #Include section.
//////////////////////////////////////////////////////////////////////////

// System include.
#include <stdio.h>
#include <stdarg.h>

// Windows includes
#include <windows.h>


// OpenGL Headers.
#include <GL\gl.h>		// OpenGL manin header.
#include <GL\glu.h>		// GL utility library header.
#include <GL\glaux.h>	// GL auxiliary library header.
#include <GL\glext.h>	// OpenGL extensions header.
#include <GL\wglext.h>	// OpenGL Windows extensions header.

// Local Include
#include "glfw_int.h"
#include "glfw_ext.h"



//////////////////////////////////////////////////////////////////////////
//  Constants definition section.
//////////////////////////////////////////////////////////////////////////

/// OpenGL (Simple) Framework included preprocessor #define.
#define GLFW


// OpenGL versions (returned by GLFW_GetOpenGLVersion()
#define GLFW_GL_VER_1_0			1000	///< Version 1.0
#define GLFW_GL_VER_1_1			1100	///< Version 1.1
#define GLFW_GL_VER_1_2			1200	///< Version 1.2
#define GLFW_GL_VER_1_3			1300	///< Version 1.3
#define GLFW_GL_VER_1_4			1400	///< Version 1.4
#define GLFW_GL_VER_1_5			1500	///< Version 1.5
#define GLFW_GL_VER_2_0			2000	///< Version 2.0



//////////////////////////////////////////////////////////////////////////
//  Types & Data Structure definition definition section.
//////////////////////////////////////////////////////////////////////////

//
// Bits per Pixel enumerated type.
//
typedef enum
{
	GLFW_16BPP = 16,			///< 16 bits per pixel.
	GLFW_24BPP = 24,			///< 24 bits per pixel.
	GLFW_32BPP = 32				///< 32 bits per pixel.

} GLFW_BPP;


//
//  Application Window parameters data structure.
//
typedef struct
{
	INT			m_iPosX;		///< Window X position.
	INT			m_iPosY;		///< Window Y position.
	INT			m_iWidth;		///< Window width.
	INT			m_iHeight;		///< Window height.
	BOOL		n_bFullScreen;	///< Full screen flag.
	GLFW_BPP	m_iBpp;			///< Number of bits per pixel.
	GLFW_BPP	m_iZDepth;		///< Z-Buffer depth.
	LPCTSTR		m_szTitle;		///< Window title.
	WNDPROC		m_fpWindProc;	///< Messages handling procedure.

} GLWF_WINPAR;

// Pointer to one GLWF_WINPAR data structure.
typedef GLWF_WINPAR*	lpGLWF_WINPAR;


//
// Window context data structure.
//
typedef struct
{
	HWND 		m_hWin;			///< Handle of the window.
	HDC			m_hDC;			///< Handle to Device Context.
	HGLRC		m_hRC;			///< OpenGL rendering context.
	INT			m_iWidth;		///< Window width.	
	INT			m_iHeight;		///< Window height.
	INT			m_iPosX;		///< Window X position.
	INT			m_iPosY;		///< Window Y position.
	FLOAT		m_fAspectRatio;	///< Window aspect ratio.
	INT			m_iPixFormat;	///< Pixel Format.
	GLint		m_iFontLst;		///< Display list of the current font.

} GLFW_WCTXT;

// Pointer to one GLFW_WCTXT data structure.
typedef GLFW_WCTXT*	lpGLFW_WCTXT;



//////////////////////////////////////////////////////////////////////////
//  Exported Stuffs definition section.
//////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C" 
{
#endif // __cplusplus

// Library initialization functions:
extern BOOL			glfwInit ( void );


// Drawing Windows creation/destruction functions:
extern BOOL			glfwCreateWindow ( lpGLWF_WINPAR pParams, GLFW_WCTXT **ppWCntx );
extern BOOL			glfwDestroyWindow ( lpGLFW_WCTXT pWCntx, BOOL bCleanContext );
extern BOOL			glfwDestroyContext( lpGLFW_WCTXT pWCntx ); 
extern BOOL			glfwSetMsgProc ( WNDPROC fpWinProc, lpGLFW_WCTXT pWCntx );


// Windows context functions:
extern lpGLFW_WCTXT	glfwSetCurrWinContext ( lpGLFW_WCTXT pWCntx );
extern lpGLFW_WCTXT	glfwGetCurrWinContext ( void );
extern lpGLFW_WCTXT	glfwGetWinContext ( HWND hWindow );


// Extensions utility functions:
extern PVOID 		glfwGetExtension ( const PCHAR pExtNameGL );
extern BOOL			glfwQueryExtensionGL ( const PCHAR pExtNameGL );
extern BOOL			glfwQueryExtensionWGL ( const PCHAR pExtNameWGL );


// Text & Font functions:
extern void			glfwText2D ( lpGLFW_WCTXT pWCtxt, INT iPosX, INT iPosY, PCHAR szText );
extern void			glfwTextF2D ( lpGLFW_WCTXT pWCtxt, INT iPosX, INT iPosY, PCHAR szFormat, ...  );
extern void			glfwTextColor2D ( GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA );
extern void			glfwTextColor2Dv ( GLfloat* pColor );


// Utility functions:
extern const PCHAR 	glfwGetErrorString ( GLenum iError );
extern void			glfwDrawAxis ( GLfloat fLength );


// Miscellaneous functions:
extern DWORD		glfwGetVersion ( void );
extern PCHAR		glfwGetVersionString ( void );
extern DWORD		glfwGetOpenGLVersion ( void );


#ifdef __cplusplus
}
#endif // __cplusplus



//////////////////////////////////////////////////////////////////////////
//  END of Code.
//////////////////////////////////////////////////////////////////////////

#endif // __GLFW__
