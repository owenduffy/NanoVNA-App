
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#include <math.h>

#pragma hdrstop

#include "Graphs.h"
#include "DataUnit.h"
#include "Unit1.h"
#include "CommsUnit.h"
#include "LCMatch.h"
#include "spline.h"
#include "Calibration.h"

#pragma package(smart_init)

#define GL_CLAMP_TO_EDGE 0x812F

#define AUTO_PEAK_HOLD_COUNT  60	// 3 seconds

// custom dashed line patterns
const float grid_line_pattern1[] = {2.0f, 2.0f};
const float grid_line_pattern2[] = {1.0f, 1.0f};

const float rad_2_deg = 180.0 / M_PI;
const float deg_2_rad = M_PI / 180.0;

#define NON_LIN_GAMMA

#ifdef USE_OPENGL
String __fastcall glErrorStr(GLenum err)
{
	switch (err)
	{
		case GL_NO_ERROR				:	return "";
		case GL_INVALID_ENUM			:	return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE		:	return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION	:	return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW		:	return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW		:	return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY		:	return "GL_OUT_OF_MEMORY";
		default							:	return "GL_UNKNOWN_0x" + IntToHex((int)err, sizeof(unsigned int) / 2);
	}
}

bool WGLExtensionSupported(const char *extension_name)
{
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");
	return (_wglGetExtensionsStringEXT == NULL) ? false : (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL) ? false : true;
}
#endif

void __fastcall swapBitmapRedBlue(Graphics::TBitmap *bm)
{	// swap the red and blue pixels over

	if (bm == NULL)
		return;

	if (bm->PixelFormat == pf24bit)
	{
		for (int y = 0; y < bm->Height; y++)
		{
			uint8_t *p = (uint8_t *)bm->ScanLine[y];
			for (int x = 0, w = 0; x < bm->Width; x++, w += 3)
			{
				uint8_t *q = p + w;
				const uint8_t pix = q[0];
				q[0] = q[2];
				q[2] = pix;
			}
		}
	}
	else
	if (bm->PixelFormat == pf32bit)
	{
		for (int y = 0; y < bm->Height; y++)
		{
			uint8_t *p = (uint8_t *)bm->ScanLine[y];
			for (int x = 0, w = 0; x < bm->Width; x++, w += 4)
			{
				uint8_t *q = p + w;
				const uint8_t pix = q[0];
				q[0] = q[2];
				q[2] = pix;
			}
		}
	}
}

// returns the highest number closest to v, which is a power of 2
// assumes 32 bit ints
inline int __fastcall next_pow2(int value)
{
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;
	return value;
}

double __fastcall nonLinear(double value, double min, double max, double gamma)
{	// return 0.0 to 'max - min' (assuming value is between min and max)

	if (gamma <= GAMMA_MIN)
		return value - min;	// linear
/*
	if (max < min)
	{	// swap
		const float tmp = max;
		max = min;
		min = tmp;
	}
*/
	const double range = fabs(max - min);

	if (value < min)
		value = min;

	#ifdef NON_LIN_GAMMA
		const double level = (level >= 0.0) ? pow((value - min) / range, 1.0 / gamma) * range : 0.0; 	// non-linear scale

		// reverse
		//value = min + (pow(level / range, gamma) * range);
	#else

//		double level = value - min;
//		const double scale = (10.0 / range) * gamma;
//		const double scale = (1.0 / range) * gamma * 10;
//		level = (level >= 0.0) ? min + log(1.0 + level * scale) * range / log(1.0 + range * scale) : 0.0;

//		double level = value - min;
//		level = (level >= 0.0) ? min + log(1.0 + level * gamma) * range / log(1.0 + range * gamma) : 0.0;


		const double scale = (gamma - GAMMA_MIN) / (GAMMA_MAX - GAMMA_MIN);	// 0 to 1
		const double mid   = min + (range * scale);
//		const double level = min + (log(1.0 + (value - min)) / mid) * max;
		const double level = min + (log(1.0 + (value - min)) / mid) / (log(1.0 + range) / range) * range;

		//const double level = min + mid * exp(max * value);
		//const double value = log((level - min) / mid) / max;
	#endif

	return level;
}

void __fastcall convolve(	const float x[],	// input vector
					const float h[],	// impulse response (or second input vector)
							float y[],	// output vetor (result of convolution)
					const short L)		// vector size
{
	for (int n = 0; n < L; n++)
	{
		float sum = x[0] * h[n];
		for (int i = 1; i <= n; i++)
			sum += x[i] * h[n - i];
		y[n] = sum;
	}
}

__fastcall CGraphs::CGraphs()
{
	#ifdef USE_OPENGL
		m_gl.win_control         = NULL;
		m_gl.hdc                 = NULL;
		m_gl.hrc                 = NULL;
		m_gl.rebuild_fonts       = true;
	#endif

	m_gdi_plus_token = 0;
	Gdiplus::GdiplusStartup(&m_gdi_plus_token, &m_gdi_plus_startup_input, NULL);
	m_gdi_plus = NULL;

	m_pb    = NULL;
	m_form  = NULL;
	m_panel = NULL;

	m_lpf_coeff = 0.0f;

	m_mouse.marker_mem           = -1;
	m_mouse.marker_index         = -1;
	m_mouse.graph                = -1;
	m_mouse.x                    = -1;
	m_mouse.y                    = -1;
	m_mouse.Hz                   = -1;
	m_mouse.cpx                  = complexf (0);
	m_mouse.point_mem            = -1;
	m_mouse.point_index          = -1;
	m_mouse.time_mem             = -1;
	m_mouse.time_index           = -1;
	m_mouse.graph_type_select    = -1;
	m_mouse.graph_scale_pos      = -1;
	m_mouse.graph_v_scale        = -1;
	m_mouse.graph_h_scale        = -1;
	m_mouse.graph_scale_pos_drag = -1;
	m_mouse.graph_v_scale_drag   = -1;
	m_mouse.graph_h_scale_drag   = -1;
	m_mouse.graph_drag           = -1;
	m_mouse.down_graph           = -1;
	m_mouse.down_Hz              = -1;
	m_mouse.down_max             =  0;
	m_mouse.down_min             =  0;
	m_mouse.down_val             =  0;
	m_mouse.down_x               = -1;
	m_mouse.down_y               = -1;
	m_mouse.down_marker          = -1;

	m_graph_bm = new Graphics::TBitmap();
	if (m_graph_bm)
	{
		m_graph_bm->Monochrome   = false;
		m_graph_bm->Transparent  = false;
		m_graph_bm->PixelFormat  = pf32bit;
	}

	for (int graph = 0; graph < MAX_GRAPHS; graph++)
	{
		m_grid[graph].graph_type = GRAPH_TYPE_SIZE;	// none
		m_grid[graph].bitmap = new Graphics::TBitmap();
		if (m_grid[graph].bitmap)
		{
			m_grid[graph].bitmap->Monochrome   = false;
			m_grid[graph].bitmap->Transparent  = false;
			m_grid[graph].bitmap->PixelFormat  = pf32bit;
		}
		#ifdef USE_OPENGL
			m_grid[graph].bitmap_texture = 0;
//			m_grid[graph].bitmap_pbo = 0;
		#endif

		m_max[graph].channel = -1;
		m_max[graph].index   = -1;
		m_max[graph].value   = 0;

		m_min[graph].channel = -1;
		m_min[graph].index   = -1;
		m_min[graph].value   = 0;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			m_max_visible_index[graph][mem] = -1;
			m_min_visible_index[graph][mem] = -1;
		}
	}
}

__fastcall CGraphs::~CGraphs()
{
	for (int graph = 0; graph < MAX_GRAPHS; graph++)
	{
		if (m_grid[graph].bitmap)
			delete m_grid[graph].bitmap;
		m_grid[graph].bitmap = NULL;

		#ifdef USE_OPENGL
			if (m_grid[graph].bitmap_texture != 0)
				glDeleteTextures(1, &m_grid[graph].bitmap_texture);
			m_grid[graph].bitmap_texture = 0;
/*
			if (m_grid[graph].bitmap_pbo != 0)
				glDeleteBuffers(1, &m_grid[graph].bitmap_pbo);
			m_grid[graph].bitmap_pbo = 0;
*/
		#endif
	}

	if (m_graph_bm != NULL)
		delete m_graph_bm;
	m_graph_bm = NULL;

	if (m_gdi_plus)
		delete m_gdi_plus;
	m_gdi_plus = NULL;

	if (m_gdi_plus_token != 0)
		Gdiplus::GdiplusShutdown(m_gdi_plus_token);
	m_gdi_plus_token = 0;

	#ifdef USE_OPENGL
		if (m_gl.hdc)
		{
			for (int i = 0; i < (int)m_gl.font.size(); i++)
			{
				if (m_gl.font[i].bitmap != NULL)
					delete m_gl.font[i].bitmap;
				m_gl.font[i].bitmap = NULL;

				if (m_gl.font[i].font != NULL)
					delete m_gl.font[i].font;
				m_gl.font[i].font = NULL;

				if (m_gl.font[i].base != 0)
					glDeleteLists(m_gl.font[i].base, 256 - 32);
				m_gl.font[i].base = 0;
			}
			m_gl.font.resize(0);

			glMatrixMode(GL_PROJECTION);
			//glPopMatrix;
			glMatrixMode(GL_MODELVIEW);
			//glPopMatrix;

			wglMakeCurrent(m_gl.hdc, NULL);
			if (m_gl.hrc)
				wglDeleteContext(m_gl.hrc);
			::ReleaseDC(m_gl.win_control->Handle, m_gl.hdc);
			m_gl.hrc = NULL;
			m_gl.hdc = NULL;
		}
	#endif
}

// **********************************

#ifdef USE_OPENGL

void __fastcall CGraphs::glInit(TWinControl *win_control)
{
//	GLenum err;

	if (win_control == NULL || m_gl.win_control != NULL)
		return;

	const bool double_buffer = true;

	const int width  = win_control->ClientWidth;
	const int height = win_control->ClientHeight;

	// ************

	m_gl.win_control = win_control;
	m_gl.hdc = ::GetDC(m_gl.win_control->Handle);
	if (!m_gl.hdc)
		return;

	const HDC hOldDC   = wglGetCurrentDC();
	const HGLRC hOldRC = wglGetCurrentContext();

	memset(&m_gl.pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	m_gl.pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	m_gl.pfd.nVersion   = 1;
	m_gl.pfd.dwFlags    = (double_buffer) ? PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER : PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	m_gl.pfd.iPixelType = PFD_TYPE_RGBA;	// PFD_TYPE_RGBA or PFD_TYPE_COLORINDEX
	m_gl.pfd.cColorBits = 32;	// 32 or 24
//	m_gl.pfd.cDepthBits = 16;
	m_gl.pfd.cDepthBits = 32;
	m_gl.pfd.iLayerType = PFD_MAIN_PLANE;	// PFD_MAIN_PLANE, PFD_OVERLAY_PLANE or PFD_UNDERLAY_PLANE
	m_gl.pixel_format   = ChoosePixelFormat(m_gl.hdc, &m_gl.pfd);
	if (m_gl.pixel_format != NULL && m_gl.pixel_format != 0)
		SetPixelFormat(m_gl.hdc, m_gl.pixel_format, &m_gl.pfd);

	m_gl.hrc = wglCreateContext(m_gl.hdc);	// create an openGL rendering context
//	gl_err = glGetError();

	wglMakeCurrent(m_gl.hdc, m_gl.hrc);	// set current rendering context
//	gl_err = glGetError();

//	HGLRC rc = wglGetCurrentContext();

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluOrtho2D(0, width, height, 0);
	glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);	// 0,0 bottom left
//	glOrtho(0.0f, width, height, 0.0f,  0.0f, 1.0f);	// 0,0 top left
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	glLoadMatrixf((const GLfloat*)&basematrix);

	if (double_buffer)
	{
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}
	else
	{
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);
	}

	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_POLYGON_STIPPLE);
	glDisable(GL_LINE_STIPPLE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);	// we are only working in 2D (for now)
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_NORMALIZE);
//	glDisable(GL_MULTISAMPLE);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_BLEND);

	glShadeModel(GL_SMOOTH);                // Enable smooth shading
//	glShadeModel(GL_FLAT);

	glPixelZoom(1.0, 1.0);					// Don’t scale the image

	glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);

	glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glPixelTransferi(GL_MAP_STENCIL, GL_FALSE);

	glPixelTransferi(GL_MAP_COLOR, GL_FALSE);

   // set pixel range 0.0 to 1.0
	glPixelTransferi(GL_RED_BIAS, 0);	glPixelTransferi(GL_RED_SCALE, 1);
	glPixelTransferi(GL_GREEN_BIAS, 0);	glPixelTransferi(GL_GREEN_SCALE, 1);
	glPixelTransferi(GL_BLUE_BIAS, 0);	glPixelTransferi(GL_BLUE_SCALE, 1);
	glPixelTransferi(GL_ALPHA_BIAS, 0);	glPixelTransferi(GL_ALPHA_SCALE, 1);

//	glDisable(GL_CONVOLUTION_1D_EXT);
//	glDisable(GL_CONVOLUTION_2D_EXT);
//	glDisable(GL_SEPARABLE_2D_EXT);

//	glDisable(GL_HISTOGRAM_EXT);
//	glDisable(GL_MINMAX_EXT);

//	glDisable(GL_TEXTURE_3D_EXT);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

//	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		// Set anti-aliasing
//	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);	// Set anti-aliasing

//	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Set anti-aliasing
//	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);	// Set anti-aliasing

//	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);	// Set anti-aliasing
//	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);	// Set anti-aliasing

	glEnable(GL_BLEND);
//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// *********
	// texture stuff

	// select modulate to mix texture with color for shading
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// when texture area is small, bi-linear filter the closest mipmap
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// when texture area is large, bi-linear filter the original
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// the texture ends at the edges (clamp)
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// ************
	// turn the vsync off/on

//	wglSwapIntervalEXT(0);

//	if (WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT    = (PFNWGLSWAPINTERVALEXTPROC   ) wglGetProcAddress("wglSwapIntervalEXT");
		PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");
		if (wglSwapIntervalEXT != NULL)
			wglSwapIntervalEXT(0);	// no wait .. turn the vsync off
//			wglSwapIntervalEXT(1);	// wait .. turn the vsync on
//			wglSwapIntervalEXT(-1);	// wait .. turn the automatic vsync on
	}

	typedef int (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
//	typedef int (__stdcall *PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapInterval = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapInterval");
	if (wglSwapInterval == NULL)
		wglSwapInterval = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if (wglSwapInterval != NULL)
		wglSwapInterval(0);	// no wait .. turn the vsync off
//		wglSwapInterval(1);	// wait .. turn the vsync on

	// ************

	{
		GLrgba col = colourToGLcolour(settings.m_colours.background);
		glClearColor((GLfloat)col.r / 255, (GLfloat)col.g / 255, (GLfloat)col.b / 255, 1.0);	// background colour R,G,B,A
	}

	if (double_buffer)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// if we in 3D mode
		//glClear(GL_COLOR_BUFFER_BIT);						// if we in 2D mode
		glFlush();
		SwapBuffers(m_gl.hdc);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// if we in 3D mode
		//glClear(GL_COLOR_BUFFER_BIT);						// if we in 2D mode
		glFlush();
		SwapBuffers(m_gl.hdc);
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// if we in 3D mode
		//glClear(GL_COLOR_BUFFER_BIT);						// if we in 2D mode
		glFlush();
	}

	// Build Our Bitmap Fonts
	glRebuildFonts();

	wglMakeCurrent(hOldDC, hOldRC);
}

void __fastcall CGraphs::glRebuildFonts()
{
	for (int i = 0; i < (int)m_gl.font.size(); i++)
	{
		if (m_gl.font[i].bitmap != NULL)
			delete m_gl.font[i].bitmap;
		m_gl.font[i].bitmap = NULL;

		if (m_gl.font[i].font != NULL)
			delete m_gl.font[i].font;
		m_gl.font[i].font = NULL;

		if (m_gl.font[i].base != 0)
			glDeleteLists(m_gl.font[i].base, 256 - 32);
		m_gl.font[i].base = 0;
	}
	m_gl.font.resize(0);

	if (m_gl.hdc == NULL)
		return;

	const HDC hOldDC   = wglGetCurrentDC();
	const HGLRC hOldRC = wglGetCurrentContext();

	wglMakeCurrent(m_gl.hdc, m_gl.hrc);	// set current rendering context

	glBuildFont(m_gl.hdc, "Consolas", 8, false);
	glBuildFont(m_gl.hdc, "Consolas", 8, true);

	wglMakeCurrent(hOldDC, hOldRC);

	m_gl.rebuild_fonts = false;
}

int __fastcall CGraphs::glBuildFont(HDC hdc, String name, int size, bool bold)
{	// Build Our Bitmap Font
	glFont gl_font;

	if (hdc == 0 || name.IsEmpty() || size <= 0)
		return -1;

	TFont *m_font = settings.graphFont;
	if (!m_font)
		return -1;

	TFont *font = new TFont();
	if (!font)
		return -1;

	font->Name    = m_font->Name;
	font->Charset = m_font->Charset;
	font->Color   = m_font->Color;
	font->Size    = m_font->Size;
	font->Style   = m_font->Style;
	font->Pitch   = m_font->Pitch;
	font->Quality = m_font->Quality;

	if (!bold)
		font->Style = font->Style >> fsBold;  // make unbold
	else
		font->Style = font->Style << fsBold;  // make bold

	Graphics::TBitmap *bitmap = new Graphics::TBitmap();
	if (!bitmap)
	{
		delete font;
		return -1;
	}

	bitmap->Monochrome   = false;
	bitmap->Transparent  = false;
	bitmap->PixelFormat  = pf32bit;
	bitmap->Width        = 256;
	bitmap->Height       = 64;
	bitmap->Canvas->Font = font;

	LOGFONT lf;
	::GetObject(bitmap->Canvas->Font->Handle, sizeof(LOGFONT), &lf);
	lf.lfEscapement  = 0;
	lf.lfOrientation = lf.lfEscapement;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	const HFONT lf_handle = CreateFontIndirect(&lf);
//	const HFONT old_handle = ::SelectObject(bitmap->Canvas->Handle, lf_handle);
	::SetGraphicsMode(bitmap->Canvas->Handle, GM_ADVANCED);

	HGDIOBJ original = ::SelectObject(hdc, GetStockObject(DEVICE_DEFAULT_FONT));	// save current font

	HFONT oldfont = (HFONT)SelectObject(hdc, lf_handle);       // Selects the font we want
	if (oldfont == 0)
	{
		if (original != 0)
			::SelectObject(hdc, original);
		::DeleteObject(lf_handle);
		delete font;
		return -1;
	}

	gl_font.base_start = 32;

	GLuint list = glGenLists(256 - gl_font.base_start);
	if (list == 0)
	{
		if (original != 0)
			::SelectObject(hdc, original);
		::DeleteObject(lf_handle);
		delete font;
		return -1;
	}

	// Build '256-32' characters starting at character 32
	if (!wglUseFontBitmaps(hdc, gl_font.base_start, 255 - gl_font.base_start, list))	// 2D text
//	if (!wglUseFontOutlines(hdc, gl_font.base_start, 255 - gl_font.base_start, list, 0.0f, 0.1f, WGL_FONT_POLYGONS, gl_font.gmf))   // 3D text
	{
		if (original != 0)
			::SelectObject(hdc, original);
		::DeleteObject(lf_handle);
		delete font;
		return -1;
	}

	::SelectObject(hdc, oldfont);				// Selects the font we want
	::DeleteObject(lf_handle);

	gl_font.bitmap     = bitmap;
	gl_font.font       = font;
	gl_font.height     = bitmap->Canvas->TextHeight("Hg_|");
	gl_font.base       = list;

	m_gl.font.push_back(gl_font);

	return m_gl.font.size();
}

int __fastcall CGraphs::glTextWidth(int font_index, String s)
{
	if (font_index < 0 || font_index >= (int)m_gl.font.size())
		return 0;

	if (m_gl.font[font_index].base == 0 || m_gl.font[font_index].bitmap == NULL)
		return 0;

//	int length = 0;
//	for (unsigned int i = 0; i < s.Length(); i++)
////		length += m_gl.font.gmf[s[i]].gmfCellIncX;
//		length += m_gl.font.gmf[s[i]].gmfBlackBoxX;

	return m_gl.font[font_index].bitmap->Canvas->TextWidth(s);
}

int __fastcall CGraphs::glTextHeight(int font_index, String s)
{
	if (font_index < 0 || font_index >= (int)m_gl.font.size())
		return 0;

	if (m_gl.font[font_index].base == 0 || m_gl.font[font_index].bitmap == NULL)
		return 0;

//	int length = 0;
//	for (unsigned int i = 0; i < s.Length(); i++)
////		length += m_gl.font.gmf[s[i]].gmfCellIncY;
//		length += m_gl.font.gmf[s[i]].gmfBlackBoxY;

	return m_gl.font[font_index].bitmap->Canvas->TextHeight(s);
}

void CGraphs::glPrint(int font_index, GLint x, GLint y, const char *fmt, ...)
{
	if (font_index < 0 || font_index >= (int)m_gl.font.size())
		return;

	const GLuint base       = m_gl.font[font_index].base;
	const GLuint base_start = m_gl.font[font_index].base_start;
	if (base == 0 || fmt == NULL)
		return;

	if (fmt == NULL)
		return;

	va_list ap;
	char tmp;

	va_start(ap, fmt);
		int buf_size = vsnprintf(&tmp, 0, fmt, ap);
	va_end(ap);

	if (buf_size == 0)
		return;

	if (buf_size == -1)
		buf_size = 512;

	char *buf = new char [buf_size + 1];
	if (buf == NULL)
		return;

	va_start(ap, fmt);
		vsnprintf_s(buf, buf_size + 1, fmt, ap);
	va_end(ap);

	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);

	glPushAttrib(GL_LIST_BIT);
		glRasterPos2i(x, y);				// Position the text on the screen
//		glTranslated(x, y, 0);				// Position the text (0,0 - Bottom left)
		glListBase(base - base_start);
		glCallLists(buf_size, GL_UNSIGNED_BYTE, buf);	// Draws the display list text
	glPopAttrib();

	glShadeModel(GL_FLAT);

	delete buf;
}

void __fastcall CGraphs::glTextOut(int font_index, GLint x, GLint y, String s)
{
	if (font_index < 0 || font_index >= (int)m_gl.font.size())
		return;

	const GLuint base       = m_gl.font[font_index].base;
	const GLuint base_start = m_gl.font[font_index].base_start;
	if (base == 0)
		return;

	y += 3 + (m_gl.font[font_index].height / 2);

	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);

	glPushAttrib(GL_LIST_BIT);
		glRasterPos2i(x, y);				// Position the text on the screen
//		glTranslated(x, y, 0);				// Position the text (0,0 - Bottom left)
		glListBase(base - base_start);
		glCallLists(s.Length(), GL_UNSIGNED_BYTE, AnsiString(s).c_str());	// Draws the display list text
	glPopAttrib();

	glShadeModel(GL_FLAT);
}

void __fastcall CGraphs::glCircle(const float x, const float y, const float radius, const float deg_start, const float deg_end, const int segments, const bool fill)
{
	if (radius > 0.0f && segments > 1)
	{
		const float deg_step = (deg_end - deg_start) / segments;
		if (!fill)
		{	// empty circle
			glBegin(GL_LINE_LOOP);
				for (int i = 0; i < segments; i++)
				{
					const float phi  = (deg_start + (deg_step * i)) * deg_2_rad;
					const GLfloat vx = (GLfloat)(x + (cosf(phi) * radius));
					const GLfloat vy = (GLfloat)(y - (sinf(phi) * radius));
					glVertex2f(vx, vy);
				}
			glEnd();
		}
		else
		{	// filled circle
			//glShadeModel(GL_SMOOTH);
			glShadeModel(GL_FLAT);
			glBegin(GL_TRIANGLE_FAN);
				glVertex2f(x, y);	// center
				for (int i = 0; i <= segments; i++)
				{
					const float phi  = (deg_start + (deg_step * i)) * deg_2_rad;
					const GLfloat vx = (GLfloat)(x + (cosf(phi) * radius));
					const GLfloat vy = (GLfloat)(y - (sinf(phi) * radius));
					glVertex2f(vx, vy);
				}
			glEnd();
			//glShadeModel(GL_FLAT);
		}
	}
}

void __fastcall CGraphs::glResize(const int width, const int height)
{
	GLenum err;

	if (!m_gl.hdc || m_gl.hrc == NULL || width <= 0 || height <= 0)
		return;

	const HDC hOldDC   = wglGetCurrentDC();
	const HGLRC hOldRC = wglGetCurrentContext();

	wglMakeCurrent(m_gl.hdc, m_gl.hrc);

	glFlush();

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, width, height);
//	err = glGetError();
//	if (err != GL_NO_ERROR)
//	{
//		String s = glErrorStr(err);
//	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

//	gluOrtho2D(0, width, height, 0);
	glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);	// 0,0 bottom left
//	glOrtho(0.0f, width, height, 0.0f,  0.0f, 1.0f);	// 0,0 top left
//	err = glGetError();
//	if (err != GL_NO_ERROR)
//	{
//		String s = glErrorStr(err);
//	}
//
//	const GLfloat nRange = 200.0f;
//	if (width <= height)
//		glOrtho(-nRange, nRange, -nRange * height / width, nRange * height / width, -nRange, nRange);
//	else
//		glOrtho(-nRange * width / height, nRange * width / height, -nRange, nRange, -nRange, nRange);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	glFinish();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// if we in 3D mode
//	glClear(GL_COLOR_BUFFER_BIT);						// if we in 2D mode
	glFlush();
	SwapBuffers(m_gl.hdc);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// if we in 3D mode
//	glClear(GL_COLOR_BUFFER_BIT);						// if we in 2D mode
	glFlush();
	SwapBuffers(m_gl.hdc);

	wglMakeCurrent(hOldDC, hOldRC);
}

GLuint __fastcall CGraphs::createTexture(Graphics::TBitmap *bitmap, bool pixel_perfect)
{
	if (!bitmap)
		return 0;
	if (bitmap->Width <= 0 || bitmap->Height <= 0)
		return 0;

	Graphics::TBitmap *bm = new Graphics::TBitmap();
	if (bm == NULL)
		return 0;

//	const int size = (next_pow2(bitmap->Width) > next_pow2(bitmap->Height)) ? next_pow2(bitmap->Width) : next_pow2(bitmap->Height);

	bm->Monochrome  = false;
	bm->Transparent = false;
	bm->PixelFormat = pf24bit;
	bm->Width       = next_pow2(bitmap->Width);
	bm->Height      = next_pow2(bitmap->Height);

	// resize the bitmap
	Gdiplus::Bitmap p_bitmap(bitmap->Handle, bitmap->Palette);
	Gdiplus::Graphics p_bm(bm->Canvas->Handle);
	p_bm.SetSmoothingMode(Gdiplus::SmoothingModeDefault);
	//p_bm.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);	// SmoothingModeAntiAlias
	//p_bm.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	p_bm.DrawImage(&p_bitmap, 0, 0, bm->Width, bm->Height);

	// need to swap the red and blue pixels
//	swapBitmapRedBlue(bm);

	const bool GL_TEXTURE_2D_wasEnabled = glIsEnabled(GL_TEXTURE_2D);
	GLint oldTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);

//	glEnable(GL_POLYGON_SMOOTH);
//	glShadeModel(GL_SMOOTH);
//	glDisable(GL_POLYGON_SMOOTH);
//	glShadeModel(GL_FLAT);

	glEnable(GL_TEXTURE_2D);

	// allocate a texture name
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	if (texture_id == 0)
	{
		delete bm;
		return 0;
	}

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// select the texture
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// when texture area is small, bi-linear filter the closest mipmap
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// when texture area is large, bi-linear filter the original
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (pixel_perfect)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// the texture ends at the edges (clamp)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// build the texture mipmaps
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, bm->Width, bm->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, bm->ScanLine[bm->Height - 1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR_EXT, bm->Width, bm->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, bm->ScanLine[bm->Height - 1]);

/*
	Gdiplus::Rect rect(0, 0, p_bitmap->GetWidth(), p_bitmap->GetHeight());
	Gdiplus::BitmapData data;
	p_bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &data);
	//p_bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, data.Width, data.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.Scan0);
	p_bitmap->UnlockBits(&data);
*/
//	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, oldTexture);

	if (!GL_TEXTURE_2D_wasEnabled)
		glDisable(GL_TEXTURE_2D);

	delete bm;

	return texture_id;
}

void __fastcall CGraphs::renderTexture(const TRect rect, const GLuint texture_id)
{
	if (rect.IsEmpty() || texture_id == 0)
		return;

	const GLint x1 = rect.left;
	const GLint y1 = rect.top;
	const GLint x2 = x1 + rect.Width();
	const GLint y2 = y1 + rect.Height();

	const bool GL_TEXTURE_2D_wasEnabled = glIsEnabled(GL_TEXTURE_2D);

//	glEnable(GL_POLYGON_SMOOTH);
//	glShadeModel(GL_SMOOTH);
//	glDisable(GL_POLYGON_SMOOTH);
//	glShadeModel(GL_FLAT);

	GLint oldTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);

	if (!GL_TEXTURE_2D_wasEnabled)
		glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex2i(x1, y1);	// top left corner
		glTexCoord2f(1, 1); glVertex2i(x2, y1);	// top right corner
		glTexCoord2f(1, 0); glVertex2i(x2, y2);	// bottom right corner
		glTexCoord2f(0, 0); glVertex2i(x1, y2);	// bottom left corner
	glEnd();

	glBindTexture(GL_TEXTURE_2D, oldTexture);

	if (!GL_TEXTURE_2D_wasEnabled)
		glDisable(GL_TEXTURE_2D);
}

Graphics::TBitmap * __fastcall CGraphs::glCopyScreen()
{
	GLenum err;

	#ifndef USE_OPENGL
		return NULL;
	#endif

	if (!m_gl.hdc || !m_gl.hrc || !m_gl.win_control)
		return NULL;

	const int view_width  = m_gl.win_control->ClientWidth;
	const int view_height = m_gl.win_control->ClientHeight;
	if (view_width <= 0 || view_height <= 0)
		return NULL;

	const HDC hOldDC   = wglGetCurrentDC();
	const HGLRC hOldRC = wglGetCurrentContext();

	if (!wglMakeCurrent(m_gl.hdc, m_gl.hrc))
		return NULL;

	Graphics::TBitmap *bm = new Graphics::TBitmap();
	if (!bm)
	{
		wglMakeCurrent(hOldDC, hOldRC);
		return NULL;
	}

	bm->Monochrome  = false;
	bm->Transparent = false;
	bm->PixelFormat = pf24bit;
	bm->Width       = view_width;
	bm->Height      = view_height;

//	glReadBuffer(GL_BACK);
	glReadBuffer(GL_FRONT);

	try
	{
		glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
		//glReadPixels(0, 0, view_width, view_height, GL_RGB, GL_UNSIGNED_BYTE, bm->ScanLine[view_height - 1]);
		glReadPixels(0, 0, view_width, view_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm->ScanLine[view_height - 1]);
		err = glGetError();
	}
	catch (Exception &exception)
	{
		//String s = exception.ToString();
		wglMakeCurrent(hOldDC, hOldRC);
		delete bm;
		return NULL;
	}

	wglMakeCurrent(hOldDC, hOldRC);

	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		String s = glErrorStr(err);
		delete bm;
		return NULL;
	}

	// need to swap the red and blue pixels
//	swapBitmapRedBlue(bm);

	return bm;
}

#endif

// **********************************

int __fastcall CGraphs::numberOfGraphs()
{
	switch (settings.graphArrangement)
	{
		default:
		case GRAPH_ARRANGE_1:
			return 1;
		case GRAPH_ARRANGE_1L_1R:
		case GRAPH_ARRANGE_1T_1B:
			return 2;
		case GRAPH_ARRANGE_1T_2B:
		case GRAPH_ARRANGE_2L_1R:
		case GRAPH_ARRANGE_2T_1B:
		case GRAPH_ARRANGE_1L_2R:
		case GRAPH_ARRANGE_3D:
		case GRAPH_ARRANGE_3A:
			return 3;
		case GRAPH_ARRANGE_4S:
		case GRAPH_ARRANGE_4A:
		case GRAPH_ARRANGE_4D:
		case GRAPH_ARRANGE_3T_1B:
		case GRAPH_ARRANGE_3L_1R:
		case GRAPH_ARRANGE_1T_3B:
		case GRAPH_ARRANGE_1L_3R:
		case GRAPH_ARRANGE_1T_1M_2B:
		case GRAPH_ARRANGE_1T_2M_1B:
		case GRAPH_ARRANGE_2T_1M_1B:
		case GRAPH_ARRANGE_1L_1M_2R:
		case GRAPH_ARRANGE_1L_2M_1R:
		case GRAPH_ARRANGE_2L_1M_1R:
			return 4;
	}
}

bool __fastcall CGraphs::isFrequencyGraph(const int graph_type)
{
	switch (graph_type)
	{
		case GRAPH_TYPE_LOGMAG_S11:
		case GRAPH_TYPE_LOGMAG_S21:
		case GRAPH_TYPE_LOGMAG_S11S21:
		case GRAPH_TYPE_LINMAG_S11:
		case GRAPH_TYPE_LINMAG_S21:
		case GRAPH_TYPE_LINMAG_S11S21:
		case GRAPH_TYPE_PHASE_S11:
		case GRAPH_TYPE_PHASE_S21:
		case GRAPH_TYPE_PHASE_S11S21:
		case GRAPH_TYPE_PHASE_UNWRAP_S11:
		case GRAPH_TYPE_PHASE_UNWRAP_S21:
		case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
		case GRAPH_TYPE_VSWR_S11:
		case GRAPH_TYPE_IMPEDANCE_S11:
		case GRAPH_TYPE_SERIES_RJX_S11:
		case GRAPH_TYPE_PARALLEL_RJX_S11:
		case GRAPH_TYPE_GJB_S11:
		case GRAPH_TYPE_SERIES_RESISTANCE_S11:
		case GRAPH_TYPE_SERIES_REACTANCE_S11:
		case GRAPH_TYPE_QUALITY_FACTOR_S11:
		case GRAPH_TYPE_GROUP_DELAY_S11:
		case GRAPH_TYPE_GROUP_DELAY_S21:
		case GRAPH_TYPE_GROUP_DELAY_S11S21:
		case GRAPH_TYPE_REAL_IMAG_S11:
		case GRAPH_TYPE_REAL_IMAG_S21:
		case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
		case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
		case GRAPH_TYPE_COAX_LOSS_S11:
		case GRAPH_TYPE_CAL_LOGMAG:
			return true;
		default:
			break;
	}
	return false;
}

bool __fastcall CGraphs::isSmithGraph(const int graph_type)
{
	switch (graph_type)
	{
		case GRAPH_TYPE_SMITH_S11:
		case GRAPH_TYPE_SMITH_S21:
			return true;
		default:
			break;
	}
	return false;
}

bool __fastcall CGraphs::isAdmittanceGraph(const int graph_type)
{
	switch (graph_type)
	{
		case GRAPH_TYPE_ADMITTANCE_S11:
		case GRAPH_TYPE_ADMITTANCE_S21:
			return true;
		default:
			break;
	}
	return false;
}

bool __fastcall CGraphs::isPolarGraph(const int graph_type)
{
	switch (graph_type)
	{
		case GRAPH_TYPE_POLAR_S11:
		case GRAPH_TYPE_POLAR_S21:
			return true;
		default:
			break;
	}
	return false;
}

bool __fastcall CGraphs::isTDRGraph(const int graph_type)
{
	switch (graph_type)
	{
		case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LIN_BP_S11:
		case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LOG_BP_S11:
		case GRAPH_TYPE_TDR_IMPEDANCE_S11:
			return true;
		default:
			break;
	}
	return false;
}

bool __fastcall CGraphs::isTimeGraph(const int graph_type)
{
	switch (graph_type)
	{
		case GRAPH_TYPE_PHASE_VECTOR_S11:
		case GRAPH_TYPE_PHASE_VECTOR_S21:
			return true;
		default:
			break;
	}
	return false;
}

void __fastcall CGraphs::computeVisibleIndexRange(const int graph, const int graph_type)
{
	// compute the on-screen visible range of the trace

	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		m_max_visible_index[graph][mem] = -1;
		m_min_visible_index[graph][mem] = -1;
	}

	if (data_unit.m_freq_start_Hz <= 0 || data_unit.m_freq_stop_Hz <= 0 || data_unit.m_freq_center_Hz <= 0 || data_unit.m_freq_span_Hz <= 0)
		return;

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		if (!settings.memoryEnable[mem])
			continue;

		const int size = data_unit.freqArraySize(mem);
		if (size < 4)
			continue;

		if (!isFrequencyGraph(graph_type))
		{
			m_max_visible_index[graph][mem] = 0;
			m_min_visible_index[graph][mem] = size - 1;
			continue;
		}

		int min_index = -1;
		int max_index = -1;

		for (int i = 0; i < size; i++)
		{
			const int64_t Hz = data_unit.m_point_mem[mem][i].Hz;
			if (Hz < data_unit.m_freq_start_Hz)
				continue;
			if (Hz > data_unit.m_freq_stop_Hz)
				break;
			if (min_index < 0)
				min_index = i;
			max_index = i;
		}

		// go one point either side of visible
		if (max_index >= 0)
			if (++max_index > (size - 1))
				max_index = size - 1;
		if (min_index >= 0)
			if (--min_index < 0)
				min_index = 0;

		m_max_visible_index[graph][mem] = max_index;
		m_min_visible_index[graph][mem] = min_index;
	}
}

void __fastcall CGraphs::traceLPF(const int graph, const int graph_type, const float level, const int interations)
{
	// LPF the graph traces

	(void)graph_type;	// stop compiler warning

	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	if (interations <= 0 || level <= 0.0f)
		return;

	const float coeff     = level * 0.95f;
	const float coeff_inv = 1.0f - coeff;

	if (coeff <= 0.0f || coeff >= 1.0f)
		return;

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		if (settings.memoryEnable[mem])
		{
			for (int chan = 0; chan < MAX_CHANNELS; chan++)
			{
				const int size = m_levels[mem][chan].size();
				if (size > 1)
				{
					// run the curve trace through a simple IIR low pass filter, forward then reverse (so as not to shift the curve sidewards)
					for (int k = 0; k < interations; k++)
					{
						float lpf;

						lpf = m_levels[mem][chan][0];
						for (int i = 1; i < size; i++)
						{
							lpf = (coeff * lpf) + (coeff_inv * m_levels[mem][chan][i]);
							m_levels[mem][chan][i] = lpf;
						}

						//lpf = m_levels[mem][chan][size - 1];
						for (int i = size - 2; i >= 0; i--)
						{
							lpf = (coeff * lpf) + (coeff_inv * m_levels[mem][chan][i]);
							m_levels[mem][chan][i] = lpf;
						}
					}
				}
			}
		}
	}
}

void __fastcall CGraphs::traceSmooth(const int graph, const int graph_type)
{
	// smooth/filter the graph traces

	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	const int median_level = settings.medianFilterLevel;
	const int smooth_level = settings.curveSmoothingLevel;

	//const int mem = 0;                            // only the live memory
	for (int mem = 0; mem < MAX_MEMORIES; mem++)    // all memories
	{
		if (settings.memoryEnable[mem])
		{
			for (int chan = 0; chan < MAX_CHANNELS; chan++)
			{
				if (!m_levels[mem][chan].empty())
				{
					median_filter.process(m_levels[mem][chan], 1 << (median_level - 1));   // variable width

					smoothing_filter.process(m_levels[mem][chan], 1 << (smooth_level - 1), 1, true);     // boxcar
//					smoothing_filter.process(m_levels[mem][chan], 1 << (smooth_level - 1), 1, false);    // variable iterations .. seems to produce nicer than variable width
					//smoothing_filter.process(m_levels[mem][chan], 1, 1 << (smooth_level - 1), false);  // variable width
				}
			}
		}
	}

	//	traceLPF(graph, graph_type, m_lpf_coeff);
}

void __fastcall CGraphs::computeTraceMinMax(const int graph, const int graph_type)
{
	// find the min/max levels in the graph points

	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	const bool all_points = !isFrequencyGraph(graph_type);

	int    max_chan  = -1;
	int    max_index = -1;
	double max_value = -1;

	int    min_chan  = -1;
	int    min_index = -1;
	double min_value = 0;

	for (int m = 0; m < MAX_MEMORIES; m++)
	{
		const int max_vis_index = m_max_visible_index[graph][m];
		const int min_vis_index = m_min_visible_index[graph][m];

		for (int c = 0; c < MAX_CHANNELS; c++)
		{
//			for (auto const &level : m_levels[m][c])
			for (int i = 0; i < (int)m_levels[m][c].size(); i++)
			{
				if (all_points || (i >= min_vis_index && i <= max_vis_index))
				{	// only interested if the point is on screen
					const double level = m_levels[m][c][i];
					if (max_index < 0 || max_value < level)
					{
						max_chan  = c;
						max_index = i;
						max_value = level;
					}
					if (min_index < 0 || min_value > level)
					{
						min_chan  = c;
						min_index = i;
						min_value = level;
					}
				}
			}
		}
	}

	m_max[graph].channel = max_chan;
	m_max[graph].index   = max_index;
	m_max[graph].value   = max_value;

	m_min[graph].channel = min_chan;
	m_min[graph].index   = min_index;
	m_min[graph].value   = min_value;
}

void __fastcall CGraphs::computeCalTraceMinMax(const int graph, const int graph_type)
{
	// find the min/max levels in the graph points

	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	const bool all_points = !isFrequencyGraph(graph_type);

	int    max_chan  = -1;
	int    max_index = -1;
	double max_value = -1;

	int    min_chan  = -1;
	int    min_index = -1;
	double min_value = 0;

	const int mem = 0;
	{
		const int max_vis_index = m_max_visible_index[graph][mem];
		const int min_vis_index = m_min_visible_index[graph][mem];

		for (int trace = 0; trace < MAX_CAL_TRACES; trace++)
		{
//			for (auto const &level : m_levels[mem][trace])
			for (int i = 0; i < (int)m_levels[mem][trace].size(); i++)
			{
				if (all_points || (i >= min_vis_index && i <= max_vis_index))
				{	// only interested if the point is on screen
					const double level = m_levels[mem][trace][i];
					if (max_index < 0 || max_value < level)
					{
						max_chan  = trace;
						max_index = i;
						max_value = level;
					}
					if (min_index < 0 || min_value > level)
					{
						min_chan  = trace;
						min_index = i;
						min_value = level;
					}
				}
			}
		}
	}

	m_max[graph].channel = max_chan;
	m_max[graph].index   = max_index;
	m_max[graph].value   = max_value;

	m_min[graph].channel = min_chan;
	m_min[graph].index   = min_index;
	m_min[graph].value   = min_value;
}

void __fastcall CGraphs::computeGraphMinMax(const int graph, const int graph_type, double &max_levels, double &min_levels)
{
	// decide on the graph scale to use

	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	t_graph_setting *gs = &settings.m_graph_setting[graph_type];

	// fetch the min/max auto setting
	const bool auto_max = (gs->auto_max && m_max[graph].index >= 0) ? true : false;
	const bool auto_min = (gs->auto_min && m_min[graph].index >= 0) ? true : false;

	double max_lev = gs->max;
	double min_lev = gs->min;

	const bool smith_mode = isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);

	if (!smith_mode)
	{
		// fetch the max measured value
		if (auto_max)
		{
			if (settings.autoScalePeakHold)
			{
				if (gs->max <= m_max[graph].value)
				{	// do a peak hold to stop the graph jumping up and down
					gs->max = m_max[graph].value;
					gs->auto_max_hold_count = AUTO_PEAK_HOLD_COUNT;
				}
				else
				if (gs->auto_max_hold_count > 0)
				{
					// if it's still within 10% of the max then maintain the hold
					const double range = fabs(gs->max - gs->min);
					if (m_max[graph].value >= (gs->max - (range * 0.1)))
						gs->auto_max_hold_count = AUTO_PEAK_HOLD_COUNT;
					else
						gs->auto_max_hold_count--;
				}
				else
				{
					const double range = fabs(gs->max - gs->min);
					gs->max -= range * 0.01;
					if (gs->max <= m_max[graph].value)
					{
						gs->max = m_max[graph].value;
						gs->auto_max_hold_count = AUTO_PEAK_HOLD_COUNT;
					}
				}
				max_lev = gs->max;
			}
			else
			{
				max_lev = m_max[graph].value;
				gs->auto_max_hold_count = 0;
			}
		}

		// fetch the min measured value
		if (auto_min)
		{
			if (settings.autoScalePeakHold)
			{
				if (gs->min >= m_min[graph].value)
				{	// do a peak hold to stop the graph jumping up and down
					gs->min = m_min[graph].value;
					gs->auto_min_hold_count = AUTO_PEAK_HOLD_COUNT;
				}
				else
				if (gs->auto_min_hold_count > 0)
				{
					// if it's still within 10% of the min then maintain the hold
					const double range = fabs(gs->max - gs->min);
					if (m_min[graph].value <= (gs->min + (range * 0.1)))
						gs->auto_min_hold_count = AUTO_PEAK_HOLD_COUNT;
					else
						gs->auto_min_hold_count--;
				}
				else
				{
					const double range = fabs(gs->max - gs->min);
					gs->min += range * 0.01;
					if (gs->min >= m_min[graph].value)
					{
						gs->min = m_min[graph].value;
						gs->auto_min_hold_count = AUTO_PEAK_HOLD_COUNT;
					}
				}
				min_lev = gs->min;
			}
			else
			{
				min_lev = m_min[graph].value;
				gs->auto_min_hold_count = 0;
			}
		}
	}
	else
	{	// smith mode
		// fetch the max measured value
		if (auto_max)
		{
			if (settings.autoScalePeakHold)
			{
				if (gs->max <= m_max[graph].value)
				{	// do a peak hold to stop the graph jumping up and down
					gs->max = m_max[graph].value;
					gs->auto_max_hold_count = AUTO_PEAK_HOLD_COUNT;
				}
				else
				if (gs->auto_max_hold_count > 0)
				{
					// if it's still within 10% of the max then maintain the hold
					const double range = fabs(gs->max - gs->min);
					if (m_max[graph].value >= (gs->max - (range * 0.1)))
						gs->auto_max_hold_count = AUTO_PEAK_HOLD_COUNT;
					else
						gs->auto_max_hold_count--;
				}
				else
				{
					const double range = fabs(gs->max - gs->min);
					gs->max -= range * 0.01;
					if (gs->max <= m_max[graph].value)
					{
						gs->max = m_max[graph].value;
						gs->auto_max_hold_count = AUTO_PEAK_HOLD_COUNT;
					}
				}
				max_lev = gs->max;
			}
			else
			{
				max_lev = m_max[graph].value;
				gs->auto_max_hold_count = 0;
			}
		}
	}

	// sanity check
	settings.clipGraphMinMax(graph_type, max_lev, min_lev);

	// apply some headroom to the min/max values
	if (auto_min || auto_max)
	{
		const double head_room = fabs(max_lev - min_lev) * 0.05;	// 5% headroom
		if (auto_max)
			max_lev += head_room;
		if (auto_min)
			min_lev -= head_room;

		// sanity check
		settings.clipGraphMinMax(graph_type, max_lev, min_lev);
	}

	max_levels = max_lev;
	min_levels = min_lev;
}

float __fastcall CGraphs::freqToX(const int graph, const int graph_type, const int64_t freq)
{
	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return -1;

	if (data_unit.m_freq_center_Hz <= 0 || data_unit.m_freq_span_Hz <= 0)
		return -1;

	if (!isFrequencyGraph(graph_type))
		return -1;

	const float x_scale = (float)m_graph_pos[graph].gw / data_unit.m_freq_span_Hz;
	return m_graph_pos[graph].gx + ((float)m_graph_pos[graph].gw * 0.5f) + ((freq - data_unit.m_freq_center_Hz) * x_scale);
}

int64_t __fastcall CGraphs::xyToFreq(const int graph, const int graph_type, int x, int y)
{
	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return -1;

	if (data_unit.m_freq_center_Hz <= 0 || data_unit.m_freq_span_Hz <= 0)
		return -1;

	if (!isFrequencyGraph(graph_type))
		return -1;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (x < gx || x > (gx + gw) || y < gy || y > (gy + gh))
		return -1;

	const double f = ((double)(x - gx) / gw) - 0.5;	// -0.5 to +0.5
	return I64ROUND(data_unit.m_freq_center_Hz + (data_unit.m_freq_span_Hz * f));
}

bool __fastcall CGraphs::nearestPoint(const int graph, const int graph_type, const int x, const int y, const int max_pixel_dist, int &graph_num, int &mem, int &channel, int &index, int64_t &Hz, double &secs)
{	// return the nearest graph line point to the supplied X/Y graph coord

	graph_num = -1;
	index     = -1;
	mem       = -1;
	channel   = -1;
	index     = -1;
	Hz        = -1;
	secs      = -1;

	if (graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return false;

	float point_dist  = -1.0f;
	int point_graph   = -1;
	int point_mem     = -1;
	int point_chan    = -1;
	int point_index   = -1;

	const bool freq_mode  = isFrequencyGraph(graph_type);
	const bool smith_mode = isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);
	const bool tdr_mode   = isTDRGraph(graph_type);

	if (graph_type == GRAPH_TYPE_CAL_LOGMAG)
	{
		const int g = graph;
		const int m = 0;
		{
			const int size = calibration_module.m_calibration.point.size();
			if (size > 0)
			{
				for (int c = 0; c < MAX_CAL_TRACES; c++)
				{
					for (int i = 0; i < (int)m_line_points[g][m][c].size(); i++)
					{
						const float dx = m_line_points[g][m][c][i].x - x;
						const float dy = m_line_points[g][m][c][i].y - y;
						const float dp = (dx * dx) + (dy * dy);
						if (point_index < 0 || point_dist > dp)
						{	// found a closer line point
							point_dist  = dp;
							point_graph = -2;	// calibration graph
							point_mem   = m;
							point_chan  = c;
							point_index = i;
						}
					}
				}
			}
		}
	}
	else
	{	// memories
		const int g = graph;
		{
			for (int m = 0; m < MAX_MEMORIES; m++)
			{
				const int size = data_unit.m_point_filt[m].size();
				if (settings.memoryEnable[m] && size > 0)
				{
					for (int c = 0; c < MAX_CHANNELS; c++)
					{
						for (int i = 0; i < (int)m_line_points[g][m][c].size(); i++)
						{
							const float dx = m_line_points[g][m][c][i].x - x;
							const float dy = m_line_points[g][m][c][i].y - y;
							const float dp = (dx * dx) + (dy * dy);
							if (point_index < 0 || point_dist > dp)
							{	// found a closer line point
								point_dist  = dp;
								point_graph = g;
								point_mem   = m;
								point_chan  = c;
								point_index = i;
							}
						}
					}
				}
			}
		}
	}

	if (point_graph == -1 || point_mem < 0 || point_chan < 0 || point_index < 0 || point_dist < 0.0f)
		return false;

	if (max_pixel_dist >= 0)
	{
		point_dist = sqrtf(point_dist);   // distance now in screen pixels
		if (point_dist > max_pixel_dist)  // within 'max_pixel_dist' of the nearest sweep point ?
			return false;                  // no
	}

	graph_num = point_graph;
	mem       = point_mem;
	channel   = point_chan;
	index     = point_index;
	Hz        = 0;
	secs      = 0;

	if (freq_mode || smith_mode)
	{
		if (graph_type == GRAPH_TYPE_CAL_LOGMAG)
		{
			Hz = calibration_module.m_calibration.point[point_index].HzCal;
		}
		else
		{
			Hz = data_unit.m_point_filt[point_mem][point_index].Hz;
		}
	}
	else
	if (tdr_mode)
	{
		const int size = m_fft[point_graph][point_mem].size() / 2;
		if (size > 1)
		{
			const double mem_freq_step = data_unit.freq_step(point_mem);
			const double mem_max_time  = data_unit.max_time(mem_freq_step);
			secs = (mem_max_time * point_index) / (size - 1);
		}
	}

	return true;
}

uint32_t __fastcall CGraphs::colourToRGB(TColor colour, uint8_t &red, uint8_t &grn, uint8_t &blu)
{
	const uint32_t col = Graphics::ColorToRGB(colour);
	red = (col >>  0) & 0xff;
	grn = (col >>  8) & 0xff;
	blu = (col >> 16) & 0xff;
	return col;
}

#ifdef USE_OPENGL
GLrgba __fastcall CGraphs::colourToGLcolour(TColor colour)
{
	GLrgba c;
	const uint32_t col = Graphics::ColorToRGB(colour);
	c.r = (col >>  0) & 0xff;
	c.g = (col >>  8) & 0xff;
	c.b = (col >> 16) & 0xff;
	c.a = 255;
	return c;
}
#endif

void __fastcall CGraphs::drawMouseMarker(const int x, const int y, const int size)
{
	TColor outline_colour = settings.m_colours.mouse_marker;
	TColor fill_colour    = settings.markerFill ? settings.m_colours.mouse_marker : settings.m_colours.background;

	// draw a little marker triangle

	#ifndef USE_OPENGL
		TPoint points[4];
		points[0].x = x - 1;    points[0].y = y - 2;
		points[1].x = x - size; points[1].y = y - (size * 2);
		points[2].x = x + size; points[2].y = y - (size * 2);
		points[3].x = x + 1;    points[3].y = y - 2;
		m_graph_bm->Canvas->Pen->Style   = psSolid;
		m_graph_bm->Canvas->Pen->Width   = 1;
		m_graph_bm->Canvas->Pen->Color   = outline_colour;
		m_graph_bm->Canvas->Brush->Color = fill_colour;
		m_graph_bm->Canvas->Polygon(&points[0], ARRAY_SIZE(points) - 1);
	#else
		GLrgba ocol = colourToGLcolour(outline_colour);
		GLrgba fcol = colourToGLcolour(fill_colour);

		// inside
		glShadeModel(GL_FLAT);
		//glShadeModel(GL_SMOOTH);
		glColor3ub(fcol.r, fcol.g, fcol.b);
		glBegin(GL_TRIANGLES);
			glVertex2f(x, y - 2);
			glVertex2f(x - size, y - (size * 2));
			glVertex2f(x + size, y - (size * 2));
		glEnd();

		// outline
		//glDisable(GL_LINE_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(1);
		glColor3ub(ocol.r, ocol.g, ocol.b);
		glBegin(GL_LINE_LOOP);
			glVertex2f(x, y - 2);
			glVertex2f(x - size, y - (size * 2));
			glVertex2f(x + size, y - (size * 2));
		glEnd();
	#endif
}

void __fastcall CGraphs::drawMarker(const int graph, const int x, const int y, const bool upsidedown_marker, const bool highlighted, const bool selected, String s1, String s2, String s3, String s4, String s5, String s6)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;

		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold

		const int th = m_graph_bm->Canvas->TextHeight("Hq|");

		m_graph_bm->Canvas->Pen->Style   = psSolid;
		m_graph_bm->Canvas->Brush->Style = bsSolid;
	#else
		const int th = m_gl.font[1].height;
	#endif

	int marker_size = MARKER_SIZE;
//	if (selected)
//		marker_size += 1;
	if (highlighted)
		marker_size += 1;

	int s_num = 0;
	if (!s1.IsEmpty()) s_num++;
	if (!s2.IsEmpty()) s_num++;
	if (!s3.IsEmpty()) s_num++;
	if (!s4.IsEmpty()) s_num++;
	if (!s5.IsEmpty()) s_num++;
	if (!s6.IsEmpty()) s_num++;
//	if (s_num <= 0)
//		return;

	#ifndef USE_OPENGL
		const int text_width1 = m_graph_bm->Canvas->TextWidth(s1);
		const int text_width2 = m_graph_bm->Canvas->TextWidth(s2);
		const int text_width3 = m_graph_bm->Canvas->TextWidth(s3);
		const int text_width4 = m_graph_bm->Canvas->TextWidth(s4);
		const int text_width5 = m_graph_bm->Canvas->TextWidth(s5);
		const int text_width6 = m_graph_bm->Canvas->TextWidth(s6);
	#else
		const int text_width1 = glTextWidth(1, s1);
		const int text_width2 = glTextWidth(1, s2);
		const int text_width3 = glTextWidth(1, s3);
		const int text_width4 = glTextWidth(1, s4);
		const int text_width5 = glTextWidth(1, s5);
		const int text_width6 = glTextWidth(1, s6);
	#endif
	int tw = text_width1;
	if (tw < text_width2)
		 tw = text_width2;
	if (tw < text_width3)
		 tw = text_width3;
	if (tw < text_width4)
		 tw = text_width4;
	if (tw < text_width5)
		 tw = text_width5;
	if (tw < text_width6)
		 tw = text_width6;

	int tx = (x >= gx + (gw / 2)) ? x + (marker_size + 1) : x - (marker_size + 6) - tw;
	if (tx < gx)
		tx = x + (marker_size + 1);
	else
	if (tx + tw + 6 > gx + gw)
		tx = x - (tw + 6 + marker_size);

	int ty;
	if (!upsidedown_marker)
		ty = y - 2 - (th * s_num);
	else
		ty = y + 2;
	if (ty > gy + gh - (th * s_num))
		ty =  gy + gh - (th * s_num);
	if (ty < gy)
		 ty = gy;
	else
	if (ty > gy + gh - (th * s_num))
		 ty = gy + gh - (th * s_num);

	const TRect rect = TRect(tx, ty, tx + tw + 6, ty + 2 + (th * s_num));

	ty++;

	if (y >= gy && y <= (gy + gh))
	{	// marker

		//TColor outline_colour = settings.m_colours.marker;
		TColor outline_colour = (selected) ? settings.m_colours.marker_selected : settings.m_colours.marker;
		TColor fill_colour    = (selected) ? settings.m_colours.marker_selected : (settings.markerFill || highlighted) ? settings.m_colours.marker : settings.m_colours.background;

		#ifndef USE_OPENGL
			TPoint points[4];
			if (!upsidedown_marker)
			{
				points[0].x = x - 1;           points[0].y = y - 2;
				points[1].x = x - marker_size; points[1].y = y - (marker_size * 2);
				points[2].x = x + marker_size; points[2].y = y - (marker_size * 2);
				points[3].x = x + 1;           points[3].y = y - 2;
			}
			else
			{
				points[0].x = x - 1;           points[0].y = y + 2;
				points[1].x = x - marker_size; points[1].y = y + (marker_size * 2);
				points[2].x = x + marker_size; points[2].y = y + (marker_size * 2);
				points[3].x = x + 1;           points[3].y = y + 2;
			}
			m_graph_bm->Canvas->Pen->Style   = psSolid;
			m_graph_bm->Canvas->Pen->Width   = 1;
			m_graph_bm->Canvas->Pen->Color   = outline_colour;
			m_graph_bm->Canvas->Brush->Color = fill_colour;
			m_graph_bm->Canvas->Polygon(&points[0], ARRAY_SIZE(points) - 1);

//			m_graph_bm->Canvas->Pen->Color   = TColor(RGB(0, 128, 0));
//			m_graph_bm->Canvas->Brush->Style = bsClear;
			m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
			m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;

//			m_graph_bm->Canvas->Pen->Color = settings.m_colours.grid;
//			m_graph_bm->Canvas->Rectangle(rect);

			if (!s1.IsEmpty())
			{
				m_graph_bm->Canvas->TextOut(tx + 3, ty, s1);
				ty += th;
			}
			if (!s2.IsEmpty())
			{
				m_graph_bm->Canvas->TextOut(tx + 3, ty, s2);
				ty += th;
			}
			if (!s3.IsEmpty())
			{
				m_graph_bm->Canvas->TextOut(tx + 3, ty, s3);
				ty += th;
			}
			if (!s4.IsEmpty())
			{
				m_graph_bm->Canvas->TextOut(tx + 3, ty, s4);
				ty += th;
			}
			if (!s5.IsEmpty())
			{
				m_graph_bm->Canvas->TextOut(tx + 3, ty, s5);
				ty += th;
			}
			if (!s6.IsEmpty())
			{
				m_graph_bm->Canvas->TextOut(tx + 3, ty, s6);
				//ty += th;
			}

//			m_graph_bm->Canvas->Brush->Color = settings.m_colours.grid;
//			m_graph_bm->Canvas->FrameRect(rect);

		#else
			GLrgba ocol = colourToGLcolour(outline_colour);
			GLrgba fcol = colourToGLcolour(fill_colour);

			// inside
			glShadeModel(GL_FLAT);
			//glShadeModel(GL_SMOOTH);
			glColor3ub(fcol.r, fcol.g, fcol.b);
			glBegin(GL_TRIANGLES);
				glVertex2f(x, y - 2);
				glVertex2f(x - marker_size, y - (marker_size * 2));
				glVertex2f(x + marker_size, y - (marker_size * 2));
			glEnd();

			// outline
			//glDisable(GL_LINE_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1);
			glColor3ub(ocol.r, ocol.g, ocol.b);
			glBegin(GL_LINE_LOOP);
				glVertex2f(x, y - 2);
				glVertex2f(x - marker_size, y - (marker_size * 2));
				glVertex2f(x + marker_size, y - (marker_size * 2));
			glEnd();

			if (!s1.IsEmpty())
			{
				glTextOut(1, tx + 3, ty, s1);
				ty += th;
			}
			if (!s2.IsEmpty())
			{
				glTextOut(1, tx + 3, ty, s2);
				ty += th;
			}
			if (!s3.IsEmpty())
			{
				glTextOut(1, tx + 3, ty, s3);
				ty += th;
			}
			if (!s4.IsEmpty())
			{
				glTextOut(1, tx + 3, ty, s4);
				ty += th;
			}
			if (!s5.IsEmpty())
			{
				glTextOut(1, tx + 3, ty, s5);
				ty += th;
			}
			if (!s6.IsEmpty())
			{
				glTextOut(1, tx + 3, ty, s6);
				//ty += th;
			}
		#endif
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawMarkersOnGraph(const int graph, const int mem, const int channel, String units_str, String id)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (mem < 0 || mem >= MAX_MEMORIES)
		return;

	if (channel < 0 || channel >= MAX_CHANNELS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	const std::vector <float> &levels = m_levels[mem][channel];

	const int size = data_unit.freqArraySize(mem);
	if (size <= 0)
		return;

	if (!Form1)
		return;

	const bool show_marker_text = settings.showMarkerText;

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		const int fsize = m_graph_bm->Canvas->Font->Size;

		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#else
		const int th = m_gl.font[1].height;
	#endif

	const int m_index = m_mouse.marker_index;
	const int mi      = Form1->selectedMarker();
	const int mi_nd   = Form1->nonDeltaMarker();

	int index_nd = -1;
	if (mi_nd >= 0)
	{
		const t_marker_freq marker = settings.m_markers_freq[mi_nd];
		index_nd = data_unit.indexFreq(marker.Hz, mem);
	}

	const int gb = gy + gh - th;

	String s = "Delta marker X, mem-X         ";
	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsClear;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
		const int tx = gx + gw - m_graph_bm->Canvas->TextWidth(s);
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#else
		const GLint tx = gx + gw - glTextWidth(1, s);
	#endif

	int y = gy + 10;

	for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
	{
		const t_marker_freq marker = settings.m_markers_freq[i];

		if (!show_marker_text && i != mi && i != m_index)
			continue;

		const int index = data_unit.indexFreq(marker.Hz, mem);
		if (index < 0 || index >= size)
			continue;

		const int64_t Hz = data_unit.m_point_filt[mem][index].Hz;

		if (index < 0 || index >= (int)levels.size())
			continue;

		const float level = levels[index];

		String s[3];
		if (marker.type == MARKER_TYPE_DELTA)
			s[0].printf(L" Delta marker %d", 1 + i);
		else
			s[0].printf(L" Marker %d", 1 + i);
		if (mem <= 0)
			s[0] += ", live";
		else
			s[0] += ", memory " + IntToStr(mem);

		if (marker.type != MARKER_TYPE_DELTA || index_nd < 0 || index_nd >= (int)data_unit.m_point_filt[mem].size())
		{
			s[1] =          " Freq    " + common.freqToStr1(Hz, true, true, 6, false) + "Hz";
			if (!units_str.Trim().IsEmpty())
				s[2].printf(L" %s    %#.4g%s ", id.c_str(), level, units_str.c_str());
			else
				s[2].printf(L" %s    %#.4g ", id.c_str(), level);
		}
		else
		{
			const double Hz_nd   = data_unit.m_point_filt[mem][index_nd].Hz;
			const float level_nd = levels[index_nd];
			s[1] =          " Freq    " + common.freqToStr1(Hz - Hz_nd, true, true, 6, true) + "Hz";
			if (!units_str.Trim().IsEmpty())
				s[2].printf(L" %s    %+#.4g%s ", id.c_str(), level - level_nd, units_str.c_str());
			else
				s[2].printf(L" %s    %+#.4g ", id.c_str(), level - level_nd);
		}

		#ifndef USE_OPENGL
			if (i == mi || i == m_index)
				m_graph_bm->Canvas->Font->Size = fsize + 2;

			if (i == mi || i == m_index)
				m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
			else
				m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;	// remove bold

			if (i == mi)	// selected marker
				m_graph_bm->Canvas->Font->Color = settings.m_colours.marker_selected;
			else
				m_graph_bm->Canvas->Font->Color = settings.m_colours.font;

			for (int k = 0; k < 3; k++)
			{
				if (!s[k].Trim().IsEmpty())
				{
					int th;
					if (i != mi && k <= 0)
					{	// make the first line bold
						TFontStyles fs = m_graph_bm->Canvas->Font->Style;
						m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
						th = m_graph_bm->Canvas->TextHeight("Hq|");
						m_graph_bm->Canvas->TextOut(tx, y, s[k]);
						m_graph_bm->Canvas->Font->Style = fs;
					}
					else
					{
						th = m_graph_bm->Canvas->TextHeight("Hq|");
						m_graph_bm->Canvas->TextOut(tx, y, s[k]);
					}
					y += th;
					if (y > gb)
						break;
				}
			}

			if (m_graph_bm->Canvas->Font->Size != fsize)
				m_graph_bm->Canvas->Font->Size = fsize;

			y += m_graph_bm->Canvas->TextHeight("Hq|") / 2;
		#else
			int bold = (i == mi || i == m_index) ? 1 : 0;

			GLrgba col = colourToGLcolour((i == mi) ? settings.m_colours.marker_selected : settings.m_colours.font);
			glColor3ub(col.r, col.g, col.b);

			for (int k = 0; k < 3; k++)
			{
				if (!s[k].Trim().IsEmpty())
				{
					const int b = (i != mi && k <= 0) ? 1 : bold;	// always make the first line bold
					glTextOut(b, tx, y, s[k]);
					y += m_gl.font[b].height;
					if (y > gb)
						break;
				}
			}

			y += m_gl.font[bold].height / 2;
		#endif

		if (y >= gb)
			break;
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsSolid;

		if (m_graph_bm->Canvas->Font->Size != fsize)
			m_graph_bm->Canvas->Font->Size = fsize;

		m_graph_bm->Canvas->Font = font;
   #endif
}

void __fastcall CGraphs::drawMarkersOnGraph(const int graph, const int mem, const int channel1, const int channel2, String units_str, String id1, String id2)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (mem < 0 || mem >= MAX_MEMORIES)
		return;

	if (channel1 < 0 || channel1 >= MAX_CHANNELS)
		return;

	if (channel2 < 0 || channel2 >= MAX_CHANNELS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	const std::vector <float> &levels1 = m_levels[mem][channel1];
	const std::vector <float> &levels2 = m_levels[mem][channel2];

	const int size = data_unit.freqArraySize(mem);
	if (size <= 0)
		return;

	if (!Form1)
		return;

	const bool show_marker_text = settings.showMarkerText;

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		const int fsize = m_graph_bm->Canvas->Font->Size;
	#else
	#endif

	const int m_index = m_mouse.marker_index;
	const int mi      = Form1->selectedMarker();
	const int mi_nd   = Form1->nonDeltaMarker();

	int index_nd = -1;
	if (mi_nd >= 0)
	{
		const t_marker_freq marker = settings.m_markers_freq[mi_nd];
		index_nd = data_unit.indexFreq(marker.Hz, mem);
	}

	String s = "Delta marker X, mem-X         ";
	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsClear;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
		const int tx = gx + gw - m_graph_bm->Canvas->TextWidth(s);
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#else
		const GLint tx = gx + gw - glTextWidth(1, s);
	#endif

	const int gb = gy + gh - 5;

	int y = gy + 10;

	for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
	{
		const t_marker_freq marker = settings.m_markers_freq[i];

		if (!show_marker_text && i != mi && i != m_index)
			continue;

		const int index = data_unit.indexFreq(marker.Hz, mem);
		if (index < 0 || index >= size || index >= (int)data_unit.m_point_filt[mem].size())
			continue;

		const double Hz = data_unit.m_point_filt[mem][index].Hz;

		if (index < 0 || index >= (int)levels1.size() || index >= (int)levels2.size())
			continue;

		const float level1 = levels1[index];
		const float level2 = levels2[index];

		String s[4];

		if (marker.type == MARKER_TYPE_DELTA)
			s[0].printf(L" Delta marker %d", 1 + i);
		else
			s[0].printf(L" Marker %d", 1 + i);
		if (mem <= 0)
			s[0] += ", live";
		else
			s[0] += ", memory " + IntToStr(mem);

		if (marker.type != MARKER_TYPE_DELTA || index_nd < 0 || index_nd >= (int)data_unit.m_point_filt[mem].size())
		{
			s[1] = " Freq    " + common.freqToStr1(Hz, true, true, 6, false) + "Hz";
			if (!units_str.Trim().IsEmpty())
			{
				s[2].printf(L" %s   %#.4g%s ", id1.c_str(), level1, units_str.c_str());
				s[3].printf(L" %s   %#.4g%s ", id2.c_str(), level2, units_str.c_str());
			}
			else
			{
				s[2].printf(L" %s   %#.4g ", id1.c_str(), level1);
				s[3].printf(L" %s   %#.4g ", id2.c_str(), level2);
			}
		}
		else
		{
			const double Hz_nd    = data_unit.m_point_filt[mem][index_nd].Hz;
			const float level1_nd = levels1[index_nd];
			const float level2_nd = levels2[index_nd];
			s[1] = " Freq    " + common.freqToStr1(Hz - Hz_nd, true, true, 6, true) + "Hz";
			if (!units_str.Trim().IsEmpty())
			{
				s[2].printf(L" %s    %+#.4g%s ", id1.c_str(), level1 - level1_nd, units_str.c_str());
				s[3].printf(L" %s    %+#.4g%s ", id2.c_str(), level2 - level2_nd, units_str.c_str());
			}
			else
			{
				s[2].printf(L" %s    %+#.4g ", id1.c_str(), level1 - level1_nd);
				s[3].printf(L" %s    %+#.4g ", id2.c_str(), level2 - level2_nd);
			}
		}

		#ifndef USE_OPENGL
			if (i == mi || i == m_index)
				m_graph_bm->Canvas->Font->Size = fsize + 2;

			if (i == mi || i == m_index)
				m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
			else
				m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;	// remove bold

			if (i == mi)	// selected marker
				m_graph_bm->Canvas->Font->Color = settings.m_colours.marker_selected;
			else
				m_graph_bm->Canvas->Font->Color = settings.m_colours.font;

			for (unsigned int k = 0; k < ARRAY_SIZE(s); k++)
			{
				int th;
				if (i != mi && k <= 0)
				{	// make the first line bold
					TFontStyles fs = m_graph_bm->Canvas->Font->Style;
					m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
					th = m_graph_bm->Canvas->TextHeight("Hq|");
					m_graph_bm->Canvas->TextOut(tx, y, s[k]);
					m_graph_bm->Canvas->Font->Style = fs;
				}
				else
				{
					m_graph_bm->Canvas->TextOut(tx, y, s[k]);
					th = m_graph_bm->Canvas->TextHeight("Hq|");
				}
				y += th;
				if (y > gb)
					break;
			}

			if (m_graph_bm->Canvas->Font->Size != fsize)
				m_graph_bm->Canvas->Font->Size = fsize;

			y += m_graph_bm->Canvas->TextHeight("Hq|") / 2;
		#else
			int bold = (i == mi || i == m_index) ? 1 : 0;

			GLrgba col = colourToGLcolour((i == mi) ? settings.m_colours.marker_selected : settings.m_colours.font);
			glColor3ub(col.r, col.g, col.b);

			for (unsigned int k = 0; k < ARRAY_SIZE(s); k++)
			{
				if (!s[k].Trim().IsEmpty())
				{
					const int b = (i != mi && k <= 0) ? 1 : bold;	// always make the first line bold
					glTextOut(b, tx, y, s[k]);
					y += m_gl.font[b].height;
					if (y > gb)
						break;
				}
			}

			y += m_gl.font[bold].height / 2;
		#endif

		if (y >= gb)
			break;
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsSolid;

		if (m_graph_bm->Canvas->Font->Size != fsize)
			m_graph_bm->Canvas->Font->Size = fsize;

		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawMarkers(const int graph, const int mem, const int channel, const float level_min, const float level_max, AnsiString units_str, const float gamma, const bool draw_v_line)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (mem < 0 || mem >= MAX_MEMORIES)
		return;

	if (channel < 0 || channel >= MAX_CHANNELS)
		return;

	if (data_unit.m_freq_start_Hz <= 0 || data_unit.m_freq_stop_Hz <= 0 || data_unit.m_freq_span_Hz <= 0)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	const std::vector <float> &levels = m_levels[mem][channel];

	const double level_range = fabs(level_max - level_min);

	const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
	const double y_scale = (double)gh / level_range;

	if (!Form1)
		return;

	uint8_t line_red, line_grn, line_blu;
	colourToRGB(settings.m_colours.marker_line, line_red, line_grn, line_blu);
	uint8_t font_red, font_grn, font_blu;
	colourToRGB(settings.m_colours.font, font_red, font_grn, font_blu);
	uint8_t marker_red, marker_grn, marker_blu;
	colourToRGB(settings.m_colours.marker, marker_red, marker_grn, marker_blu);

	#ifndef USE_OPENGL
		Gdiplus::Pen line_pen(Gdiplus::Color(255, line_red, line_grn, line_blu), 1);
		line_pen.SetAlignment(Gdiplus::PenAlignmentCenter);
		line_pen.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));

		TFont *font = m_graph_bm->Canvas->Font;

		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		m_graph_bm->Canvas->Font->Color = settings.m_colours.font;

		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
	#endif

	const bool draw_marker_value = !settings.showMarkersOnGraph;
	const bool show_marker_text  = settings.showMarkerText;

	const int m_index           = m_mouse.marker_index;
	const int selected_index    = Form1->selectedMarker();
	const int highlighted_index = (m_index != selected_index) ? m_index : -1;

	int index_nd = Form1->nonDeltaMarker();
	if (index_nd >= 0)
		index_nd = data_unit.indexFreq(settings.m_markers_freq[index_nd].Hz, mem);

	// draw the vertical lines first so that they sit behind all markers
	if (draw_v_line)
	{
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Pen->Color   = settings.m_colours.marker_line;
			m_graph_bm->Canvas->Pen->Width   = 1;
			m_graph_bm->Canvas->Brush->Style = bsClear;
		#else
		#endif

		for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
		{
			if (i == highlighted_index || i == selected_index)
				continue;   // don't yet draw the highlighted and or selected marker line/text

			const t_marker_freq marker = settings.m_markers_freq[i];

			const int index = data_unit.indexFreq(marker.Hz, mem);
			const int64_t Hz = (index >= 0 && index < (int)levels.size() && index < (int)data_unit.m_point_filt[mem].size()) ? data_unit.m_point_filt[mem][index].Hz : marker.Hz;

			const int x = gx + IROUND(((double)gw / 2) + ((Hz - data_unit.m_freq_center_Hz) * x_scale));

			if (x >= gx && x <= (gx + gw))
			{
				// marker vertical line
				if (draw_marker_value)
				{
					#ifndef USE_OPENGL
						if (m_gdi_plus)
							m_gdi_plus->DrawLine(&line_pen, x, gy, x, gy + gh);
					#else
						glLineStipple(1, 0xcccc);
						glEnable(GL_LINE_STIPPLE);
						glDisable(GL_LINE_SMOOTH);
						//glEnable(GL_LINE_SMOOTH);
						glLineWidth(1.0f);
						glColor3ub(line_red, line_grn, line_blu);
						//glBegin(GL_LINE_STRIP);
						glBegin(GL_LINES);
							glVertex2i(x, gy);
							glVertex2i(x, gy + gh);
						glEnd();
						glDisable(GL_LINE_STIPPLE);
					#endif
				}

				if (draw_marker_value)
				{	// marker text
					String s[2];

					if (marker.type == MARKER_TYPE_DELTA)
						s[0].printf(L" D%d ", 1 + i);
					else
						s[0].printf(L" %d ", 1 + i);

					if (marker.type != MARKER_TYPE_DELTA || index_nd < 0 || index_nd >= (int)data_unit.m_point_filt[mem].size())
					{
						//s[1] = common.freqToStrMHz(Hz);
						s[1] = " " + common.freqToStr1(Hz, true, true, 6, false) + "Hz";
					}
					else
					{
						const double Hz_nd = data_unit.m_point_filt[mem][index_nd].Hz;
						s[1] = " " + common.freqToStr1(Hz - Hz_nd, true, true, 6, true) + "Hz";
					}

					#ifndef USE_OPENGL
						const int tw1 = m_graph_bm->Canvas->TextWidth(s[0]);
						const int tw2 = m_graph_bm->Canvas->TextWidth(s[1]);
						m_graph_bm->Canvas->TextOut(x - (tw1 / 2), gy -      5 - th, s[0]);
						m_graph_bm->Canvas->TextOut(x - (tw2 / 2), gy + gh + 5 + th, s[1]);
					#else
						const int tw1 = glTextWidth(0, s[0]);
						const int tw2 = glTextWidth(0, s[1]);
						glColor3ub(font_red, font_grn, font_blu);
						glTextOut(0, x - (tw1 / 2), gy -      5 - th, s[0]);
						glTextOut(0, x - (tw2 / 2), gy + gh + 5 + th, s[1]);
					#endif
				}
				else
				{
					//String s;
					//s.printf(L" %d ", 1 + i);
					#ifndef USE_OPENGL
						//const int tw1 = m_graph_bm->Canvas->TextWidth(s);
						//m_graph_bm->Canvas->TextOut(x - (tw1 / 2), gy + gh + 5 + th, s);
					#else
						//const int tw1 = glTextWidth(0, s);
						//glColor3ub(font_red, font_grn, font_blu);
						//glTextOut(0, x - (tw1 / 2), gy + gh + 5 + th, s);
					#endif
				}
			}
		}

		if (highlighted_index >= 0)
		{	// draw the highlighted marker line/text after the other ones
			const t_marker_freq marker = settings.m_markers_freq[highlighted_index];

			const int index = data_unit.indexFreq(marker.Hz, mem);
			const int64_t Hz = (index >= 0 && index < (int)levels.size() && index < (int)data_unit.m_point_filt[mem].size()) ? data_unit.m_point_filt[mem][index].Hz : marker.Hz;

			const int x = gx + IROUND(((double)gw / 2) + ((Hz - data_unit.m_freq_center_Hz) * (double)x_scale));
			if (x >= gx && x <= (gx + gw))
			{
				// marker vertical line
				if (draw_marker_value)
				{
					#ifndef USE_OPENGL
						if (m_gdi_plus)
							m_gdi_plus->DrawLine(&line_pen, x, gy, x, gy + gh);
					#else
						glLineStipple(1, 0xcccc);
						glEnable(GL_LINE_STIPPLE);
						glDisable(GL_LINE_SMOOTH);
						//glEnable(GL_LINE_SMOOTH);
						glLineWidth(1.0f);
						glColor3ub(line_red, line_grn, line_blu);
						//glBegin(GL_LINE_STRIP);
						glBegin(GL_LINES);
							glVertex2i(x, gy);
							glVertex2i(x, gy + gh);
						glEnd();
						glDisable(GL_LINE_STIPPLE);
					#endif
				}

				if (draw_marker_value)
				{
					String s[2];

					if (marker.type == MARKER_TYPE_DELTA)
						s[0].printf(L" D%d ", 1 + highlighted_index);
					else
						s[0].printf(L" %d ", 1 + highlighted_index);

					if (marker.type != MARKER_TYPE_DELTA || index_nd < 0 || index_nd >= (int)data_unit.m_point_filt[mem].size())
					{
						//s[1] = common.freqToStrMHz(Hz);
						s[1] = " " + common.freqToStr1(Hz, true, true, 6, false) + "Hz";
					}
					else
					{
						const double Hz_nd = data_unit.m_point_filt[mem][index_nd].Hz;
						s[1] = " " + common.freqToStr1(Hz - Hz_nd, true, true, 6, true) + "Hz";
					}

					#ifndef USE_OPENGL
						m_graph_bm->Canvas->Font->Color = settings.m_colours.marker;
						m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;   // make bold
						const int tw1 = m_graph_bm->Canvas->TextWidth(s[0]);
						const int tw2 = m_graph_bm->Canvas->TextWidth(s[1]);
						m_graph_bm->Canvas->TextOut(x - (tw1 / 2), gy -      5 - th, s[0]);
						m_graph_bm->Canvas->TextOut(x - (tw2 / 2), gy + gh + 5 + th, s[1]);
						m_graph_bm->Canvas->Font = font;
					#else
						const int tw1 = glTextWidth(1, s[0]);
						const int tw2 = glTextWidth(1, s[1]);
						//glColor3ub(font_red, font_grn, font_blu);
						glColor3ub(marker_red, marker_grn, marker_blu);
						glTextOut(1, x - (tw1 / 2), gy -      5 - th, s[0]);
						glTextOut(1, x - (tw2 / 2), gy + gh + 5 + th, s[1]);
					#endif
				}
				else
				{
					//String s1;
					//s1.printf(L" %d ", 1 + highlighted_index);
					#ifndef USE_OPENGL
						//const int tw1 = m_graph_bm->Canvas->TextWidth(s1);
						//m_graph_bm->Canvas->TextOut(x - (tw1 / 2), gy + gh + 1 + (th / 2), s1);
					#else
						//const int tw1 = glTextWidth(0, s1);
						//glColor3ub(font_red, font_grn, font_blu);
						//glTextOut(0, x - (tw1 / 2), gy + gh + 1 + (th / 2), s1);
					#endif
				}
			}
		}

		if (selected_index >= 0)
		{	// draw the selected marker line/text after the other ones
			const t_marker_freq marker = settings.m_markers_freq[selected_index];

			const int index = data_unit.indexFreq(marker.Hz, mem);
			const int64_t Hz = (index >= 0 && index < (int)levels.size() && index < (int)data_unit.m_point_filt[mem].size()) ? data_unit.m_point_filt[mem][index].Hz : marker.Hz;

			const int x = gx + IROUND(((double)gw / 2) + ((Hz - data_unit.m_freq_center_Hz) * x_scale));
			if (x >= gx && x <= (gx + gw))
			{
				// marker vertical line
				if (draw_marker_value)
				{
					#ifndef USE_OPENGL
						if (m_gdi_plus)
							m_gdi_plus->DrawLine(&line_pen, x, gy, x, gy + gh);
					#else
						glLineStipple(1, 0xcccc);
						glEnable(GL_LINE_STIPPLE);
						glDisable(GL_LINE_SMOOTH);
						//glEnable(GL_LINE_SMOOTH);
						glLineWidth(1.0f);
						glColor3ub(line_red, line_grn, line_blu);
						//glBegin(GL_LINE_STRIP);
						glBegin(GL_LINES);
							glVertex2i(x, gy);
							glVertex2i(x, gy + gh);
						glEnd();
						glDisable(GL_LINE_STIPPLE);
					#endif
				}

				if (draw_marker_value)
				{
					String s[2];

					if (marker.type == MARKER_TYPE_DELTA)
						s[0].printf(L" D%d ", 1 + selected_index);
					else
						s[0].printf(L" %d ", 1 + selected_index);

					if (marker.type != MARKER_TYPE_DELTA || index_nd < 0 || index_nd >= (int)data_unit.m_point_filt[mem].size())
					{
						//s[1] = common.freqToStrMHz(Hz);
						s[1] = " " + common.freqToStr1(Hz, true, true, 6, false) + "Hz";
					}
					else
					{
						const double Hz_nd = data_unit.m_point_filt[mem][index_nd].Hz;
						s[1] = " " + common.freqToStr1(Hz - Hz_nd, true, true, 6, true) + "Hz";
					}

					#ifndef USE_OPENGL
						//m_graph_bm->Canvas->Font->Color = settings.m_colours.marker_selected;
						m_graph_bm->Canvas->Font->Color = settings.m_colours.font;
						m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;   // make bold
						const int tw1 = m_graph_bm->Canvas->TextWidth(s[0]);
						const int tw2 = m_graph_bm->Canvas->TextWidth(s[1]);
						m_graph_bm->Canvas->TextOut(x - (tw1 / 2), gy -      5 - th, s[0]);
						m_graph_bm->Canvas->TextOut(x - (tw2 / 2), gy + gh + 5 + th, s[1]);
						m_graph_bm->Canvas->Font = font;
					#else
						const int tw1 = glTextWidth(1, s[0]);
						const int tw2 = glTextWidth(1, s[1]);
						glColor3ub(font_red, font_grn, font_blu);
						glTextOut(1, x - (tw1 / 2), gy -      5 - th, s[0]);
						glTextOut(1, x - (tw2 / 2), gy + gh + 5 + th, s[1]);
					#endif
				}
				else
				{
					//String s1;
					//s1.printf(" %d ", 1 + selected_index);
					#ifndef USE_OPENGL
						//const int tw1 = m_graph_bm->Canvas->TextWidth(s1);
						//m_graph_bm->Canvas->TextOut(x - (tw1 / 2), gy + gh + 1 + (th / 2), s1);
					#else
						//const int tw1 = glTextWidth(0, s1);
						//glColor3ub(font_red, font_grn, font_blu);
						//glTextOut(0, x - (tw1 / 2), gy + gh + 1 + (th / 2), s1);
					#endif
				}
			}
		}

		m_graph_bm->Canvas->Brush->Style = bsSolid;
	}

	for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
	{
		if (i == highlighted_index || i == selected_index)
			continue;	// draw the highlighted/selected marker last

		const t_marker_freq marker = settings.m_markers_freq[i];

		int64_t Hz = marker.Hz;
		float level = 0;

		const int index = data_unit.indexFreq(Hz, mem);
		if (index >= 0 && index < (int)levels.size() && index < (int)data_unit.m_point_filt[mem].size())
		{
			Hz = data_unit.m_point_filt[mem][index].Hz;
			level = levels[index];
		}

		const int x = gx + IROUND(((double)gw / 2) + ((Hz - data_unit.m_freq_center_Hz) * x_scale));
		if (x < gx || x > (gx + gw))
			continue;

		int y = 0;
		bool up = false;

		if (index >= 0 && index < (int)levels.size())
		{
//			if (gamma <= GAMMA_MIN)
//			{
//				y = gy - IROUND((level - level_max) * y_scale);
//			}
//			else
			{
				const double value = nonLinear(level, level_min, level_max, gamma);
				y = (gy + gh) - IROUND(value * y_scale);
			}

			if (y < gy)
			{
				y = gy;
				up = true;	// up-side-down marker
			}
			else
			if (y > (gy + gh))
				y = gy + gh;
		}

		String s;

		const bool dm = (marker.type == MARKER_TYPE_DELTA && index_nd >= 0 && index_nd < (int)levels.size()) ? true : false;

		if (draw_marker_value && index >= 0)
		{
			if (show_marker_text)
			{
				if (!dm)
				{
					//s.printf(L"%#.6g", level);
					s = common.valueToStr(level, false, true, "%#.5g");
				}
				else
				{
					const float level_nd = levels[index_nd];
					//s.printf(L"%+#.6g", level - level_nd);
					s = common.valueToStr(level - level_nd, false, true, "%+#.5g");
				}
				if (!units_str.IsEmpty())
					s += units_str;
			}
		}
		else
		if (!dm)
			s.printf(L"%d", 1 + i);
		else
			s.printf(L"D%d", 1 + i);

		drawMarker(graph, x, y, up, false, false, "", "", s);
	}

	if (selected_index >= 0)
	{	// draw the selected marker after the others so that it's on top

		const t_marker_freq marker = settings.m_markers_freq[selected_index];

		const int index = data_unit.indexFreq(marker.Hz, mem);
		if (index >= 0 && index < (int)levels.size())
		{
			int64_t Hz = marker.Hz;
			float level = 0;

			const int index = data_unit.indexFreq(Hz, mem);
			if (index >= 0 && index < (int)levels.size() && index < (int)data_unit.m_point_filt[mem].size())
			{
				Hz = data_unit.m_point_filt[mem][index].Hz;
				level = levels[index];
			}

			const int x = gx + IROUND(((double)gw / 2) + ((Hz - data_unit.m_freq_center_Hz) * (double)x_scale));
			if (x >= gx && x <= (gx + gw))
			{
				if (index >= 0 && index < (int)levels.size())
				{
					int y;

					const bool highlighted = (selected_index == m_index) ? true : false;

//					if (gamma <= GAMMA_MIN)
//					{
//						y = gy - IROUND((level - level_max) * y_scale);
//					}
//					else
					{
						const double value = nonLinear(level, level_min, level_max, gamma);
						y = (gy + gh) - IROUND(value * y_scale);
					}

					bool up = false;

					if (y < gy)
					{
						y = gy;
						up = true;	// up-side-down marker
					}
					else
					if (y > (gy + gh))
						y = gy + gh;

					String s;

					if (y >= gy && y <= (gy + gh))
					{
						const bool dm = (marker.type == MARKER_TYPE_DELTA && index_nd >= 0 && index_nd < (int)levels.size()) ? true : false;
						if (draw_marker_value && index >= 0)
						{
//							if (show_marker_text)
							{
								if (!dm)
								{
									//s.printf(L"%#.6g", level);
									s = common.valueToStr(level, false, true, "%#.5g");
								}
								else
								{
									const float level_nd = levels[index_nd];
									//s.printf(L"%+#.6g", level - level_nd);
									s = common.valueToStr(level - level_nd, false, true, "%+#.5g");
								}
								if (!units_str.IsEmpty())
									s += units_str;
							}
						}
						else
						if (!dm)
							s.printf(L"%d", 1 + selected_index);
						else
							s.printf(L"D%d", 1 + selected_index);
					}

					drawMarker(graph, x, y, up, highlighted, true, "", "", s);
				}
			}
		}
	}

	if (highlighted_index >= 0)
	{	// draw the highlighted marker last so that's on top

		const t_marker_freq marker = settings.m_markers_freq[highlighted_index];

		const int index = data_unit.indexFreq(marker.Hz, mem);
		if (index >= 0 && index < (int)levels.size())
		{
			int64_t Hz = marker.Hz;
			float level = 0;

			const int index = data_unit.indexFreq(Hz, mem);
			if (index >= 0 && index < (int)levels.size() && index < (int)data_unit.m_point_filt[mem].size())
			{
				Hz = data_unit.m_point_filt[mem][index].Hz;
				level = levels[index];
			}

			const int x = gx + IROUND(((double)gw / 2) + ((Hz - data_unit.m_freq_center_Hz) * x_scale));
			if (x >= gx && x <= (gx + gw))
			{
				int y = 0;

				if (index >= 0 && index < (int)levels.size())
				{
//					if (gamma <= GAMMA_MIN)
//					{
//						y = gy - IROUND((level - level_max) * y_scale);
//					}
//					else
					{
						const double value = nonLinear(level, level_min, level_max, gamma);
						y = (gy + gh) - IROUND(value * y_scale);
					}

					bool up = false;

					if (y < gy)
					{
						y = gy;
						up = true;	// up-side-down marker
					}
					else
					if (y > (gy + gh))
						y = gy + gh;

					String s;

					if (y >= gy && y <= (gy + gh))
					{
						const bool dm = (marker.type == MARKER_TYPE_DELTA && index_nd >= 0 && index_nd < (int)levels.size()) ? true : false;
						if (draw_marker_value && index >= 0)
						{
//							if (show_marker_text)
							{
								if (!dm)
								{
									//s.printf(L"%#.6g", level);
									s = common.valueToStr(level, false, true, "%#.5g");
								}
								else
								{
									const float level_nd = levels[index_nd];
									//s.printf(L"%+#.6g", level - level_nd);
									s = common.valueToStr(level - level_nd, false, true, "%+#.5g");
								}
								if (!units_str.IsEmpty())
									s += units_str;
							}
						}
						else
						if (!dm)
							s.printf(L"%d", 1 + highlighted_index);
						else
							s.printf(L"D%d", 1 + highlighted_index);
					}

					drawMarker(graph, x, y, up, true, false, "", "", s);
				}
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
   #endif
}

void __fastcall CGraphs::drawMouse(const int graph, const int graph_type, const double level_min, const double level_max, const double gamma)
{
	AnsiString s;

	if (graph < 0 || graph >= MAX_GRAPHS)
		return;
	if (graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;
	if (m_mouse.Hz <= 0)
		return;
	if (m_mouse.graph_v_scale_drag >= 0 || m_mouse.graph_h_scale_drag >= 0)
		return;
	if (data_unit.m_freq_center_Hz <= 0 || data_unit.m_freq_span_Hz <= 0)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
	#endif

	const double level_range = fabs(level_max - level_min);

	const int channels = (graph_type != GRAPH_TYPE_CAL_LOGMAG) ? MAX_CHANNELS : MAX_CAL_TRACES;

	const int64_t Hz = I64ROUND(m_mouse.Hz);

	const int m_graph = m_mouse.graph;
//	const int m_mem   = (graph_type != GRAPH_TYPE_CAL_LOGMAG) ? m_mouse.point_mem : 0;
	const int m_mem   = m_mouse.point_mem;
	const int m_index = m_mouse.point_index;

	const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
	const double y_scale = (double)gh / level_range;

	const int x = gx + IROUND(((double)gw / 2) + ((double)(Hz - data_unit.m_freq_center_Hz) * x_scale));

	if (m_mouse.graph == graph)
	{	// vertical mouse line
		uint8_t line_red, line_grn, line_blu;
		colourToRGB(settings.m_colours.mouse_line, line_red, line_grn, line_blu);
		#ifndef USE_OPENGL
			if (m_gdi_plus)
			{
				Gdiplus::Pen pen(Gdiplus::Color(255, line_red, line_grn, line_blu), 1);
				pen.SetAlignment(Gdiplus::PenAlignmentCenter);
				pen.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));

				m_gdi_plus->DrawLine(&pen, x, gy, x, gy + gh);
			}
		#else
			glLineStipple(1, 0xcccc);
			glEnable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
			//glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			glColor3ub(line_red, line_grn, line_blu);
			//glBegin(GL_LINE_STRIP);
			glBegin(GL_LINES);
				glVertex2i(x, gy);
				glVertex2i(x, gy + gh);
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		#endif
	}

	// first draw the levels where the mouse isn't
	if (graph_type != GRAPH_TYPE_CAL_LOGMAG)
	{	// memories
		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			if (!settings.memoryEnable[mem])
				continue;

			const int index = data_unit.indexFreq(Hz, mem);
			if (index < 0)
				continue;

			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				const std::vector <float> &levels = m_levels[mem][channel];

				if (index < 0 || index >= (int)levels.size())
					continue;

				const double level = levels[index];

				const double value = nonLinear(level, level_min, level_max, gamma);

				int y = (gy + gh) - IROUND(value * y_scale);
				if (y < gy) y = gy;
				else
				if (y > (gy + gh)) y = gy + gh;

				if (x >= gx && x <= (gx + gw))
				{	// mouse marker
/*
					if (m_mouse.graph == graph && m_mem >= 0 && m_index >= 0)
					{	// horizontal mouse line
						m_graph_bm->Canvas->Brush->Style = bsClear;
						m_graph_bm->Canvas->Pen->Style   = psDot;
						m_graph_bm->Canvas->Pen->Color   = settings.m_colours.mouse_line;
						m_graph_bm->Canvas->Pen->Width   = 1;
						m_graph_bm->Canvas->MoveTo(gx, y);
						m_graph_bm->Canvas->LineTo(gx + gw, y);
						m_graph_bm->Canvas->Pen->Style   = psSolid;
						m_graph_bm->Canvas->Brush->Style = bsSolid;
					}
*/
					drawMouseMarker(x, y, MARKER_SIZE);
				}
			}
		}
	}
	else
	{	// calibrations
		const int mem = 0;
		const int index = data_unit.indexFreq(Hz, -2);
		if (index >= 0)
		{
			for (int channel = 0; channel < MAX_CAL_TRACES; channel++)
			{
				const std::vector <float> &levels = m_levels[mem][channel];

				if (index < 0 || index >= (int)levels.size())
					continue;

				const double level = levels[index];

				const double value = nonLinear(level, level_min, level_max, gamma);

				int y = (gy + gh) - IROUND(value * y_scale);
				if (y < gy) y = gy;
				else
				if (y > (gy + gh)) y = gy + gh;

				if (x >= gx && x <= (gx + gw))
				{	// mouse marker
					drawMouseMarker(x, y, MARKER_SIZE);
				}
         }
		}
	}

	// mouse text
	if (m_graph == graph)
	{
		String s1;
		String s2;

		if (graph_type != GRAPH_TYPE_CAL_LOGMAG)
		{	// memories
			if (m_index >= 0)
			{
				if (m_mem >= 0)
				{
					const int channel = 0;
					const std::vector <float> &levels = m_levels[m_mem][channel];
					if (m_mem <= 0)
						s1.printf(L"   live %d/%u   ", 1 + m_index, levels.size());
						//s1.printf(L"   %d live %d/%u   ", m_graph, 1 + m_index, levels.size());
					else
						s1.printf(L"   mem-%d %d/%u   ", m_mem, 1 + m_index, levels.size());
						//s1.printf(L"   %d mem-%d %d/%u   ", m_graph, m_mem, 1 + m_index, levels.size());
				}
				else
				{
					const int size = data_unit.freqArraySize(-1);
					if (size > 0)
						s1.printf(L"   %d/%d   ", 1 + m_index, size);
						//s1.printf(L"   %d %d/%d   ", m_graph, 1 + m_index, size);
					else
						s1.printf(L"   %d   ", 1 + m_index);
						//s1.printf(L"   %d %d   ", m_graph, 1 + m_index);
				}
			}

			s2 = " " + common.freqToStr1(Hz, true, true, 6, false) + "Hz ";

			#ifndef USE_OPENGL
				m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style << fsBold;   // bold text
				m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
				m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
				const int tw1 = m_graph_bm->Canvas->TextWidth(s1);
				const int tw2 = m_graph_bm->Canvas->TextWidth(s2);
			#else
				const int tw1 = glTextWidth(1, s1);
				const int tw2 = glTextWidth(1, s2);
			#endif

			if (tw1 > 0 || tw2 > 0)
			{
				//const int border_width = settings.borderWidth;
				//const int ty = gy - 5 - (th * 2);   // at top of graph
				const int ty = gy + gh + 5;           // at bottom of graph
				int tx1 = x - (tw1 / 2);
				int tx2 = x - (tw2 / 2);
				#ifndef USE_OPENGL
					// index text
					if (tw1 > 0)
						m_graph_bm->Canvas->TextOut(tx1, ty, s1);
					// frequency text
					if (tw2 > 0)
						m_graph_bm->Canvas->TextOut(tx2, ty + th, s2);
				#else
					//m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
					uint8_t font_red, font_grn, font_blu;
					colourToRGB(settings.m_colours.font, font_red, font_grn, font_blu);
					glColor3ub(font_red, font_grn, font_blu);
					// index text
					if (tw1 > 0)
						glTextOut(1, tx1, ty, s1);
					// frequency text
					if (tw2 > 0)
						glTextOut(1, tx2, ty + th, s2);
				#endif
			}
		}
		else
		{	// calibrations
			if (m_index >= 0)
			{
				const int channel = 0;
				const std::vector <float> &levels = m_levels[m_mem][channel];
				if (m_mem <= 0)
					s1.printf(L"   live %d/%u   ", 1 + m_index, levels.size());
				else
					s1.printf(L"   mem-%d %d/%u   ", m_mem, 1 + m_index, levels.size());
			}

			s2 = " " + common.freqToStr1(Hz, true, true, 6, false) + "Hz ";

			#ifndef USE_OPENGL
				m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style << fsBold;   // bold text
				m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
				m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
				const int tw1 = m_graph_bm->Canvas->TextWidth(s1);
				const int tw2 = m_graph_bm->Canvas->TextWidth(s2);
			#else
				const int tw1 = glTextWidth(1, s1);
				const int tw2 = glTextWidth(1, s2);
			#endif

			if (tw1 > 0 || tw2 > 0)
			{
				//const int ty = gy - 5 - (th * 2);   // at top of graph
				const int ty = gy + gh + 5;           // at bottom of graph
				int tx1 = x - (tw1 / 2);
				int tx2 = x - (tw2 / 2);
				#ifndef USE_OPENGL
					// index text
					if (tw1 > 0)
						m_graph_bm->Canvas->TextOut(tx1, ty, s1);
					// frequency text
					if (tw2 > 0)
						m_graph_bm->Canvas->TextOut(tx2, ty + th, s2);
				#else
					uint8_t font_red, font_grn, font_blu;
					colourToRGB(settings.m_colours.font, font_red, font_grn, font_blu);
					glColor3ub(font_red, font_grn, font_blu);
					// index text
					if (tw1 > 0)
						m_graph_bm->Canvas->TextOut(tx1, ty, s1);
					// frequency text
					if (tw2 > 0)
						m_graph_bm->Canvas->TextOut(tx2, ty + th, s2);
				#endif
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
   #endif
}

void __fastcall CGraphs::drawMaxMarkers(const int graph, const int graph_type, const double level_min, const double level_max, const double gamma, String units, String id1, String id2)
{
	if (graph < 0 || graph >= MAX_GRAPHS)
		return;

//	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

//	const bool freq_mode  = isFrequencyGraph(graph_type);
//	const bool smith_mode = isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);
//	const bool tdr_mode   = isTDRGraph(graph_type);

	const double level_range = fabs(level_max - level_min);

//	const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
	const double y_scale = (double)gh / level_range;

	for (int m = 0; m < MAX_MEMORIES; m++)
	{
		if (settings.memoryEnable[m])
		{
			for (int c = 0; c < MAX_CHANNELS; c++)
			{
				float value = 0;
				int index = -1;
				for (unsigned int i = 0; i < m_levels[m][c].size(); i++)
				{
					const float level = m_levels[m][c][i];
					if (index < 0 || value < level)
					{
						value   = level;
						index   = i;
					}
				}

				if (index >= 0 && index < (int)data_unit.m_point_filt[m].size())
				{
					const int64_t Hz  = data_unit.m_point_filt[m][index].Hz;
					const float level = m_levels[m][c][index];

					String s[3];

					s[0] = (c <= 0) ? id1 : id2;
					s[0] += " max";
					s[0] = s[0].Trim();

					s[1] = common.freqToStr1(Hz, true, true, 6, false) + "Hz";

					//if (smith_mode)
						s[2] = common.valueToStr(level, true, true, "%#.6g") + units;

					const int x = IROUND(freqToX(graph, graph_type, Hz));

					int y;
//					if (gamma <= GAMMA_MIN)
//					{
//						y = gy - IROUND((level - level_max) * y_scale);
//					}
//					else
					{
						const double value = nonLinear(level, level_min, level_max, gamma);
						y = (gy + gh) - IROUND(value * y_scale);
					}

					bool up = false;

					if (y < gy)
					{
						y = gy;
						up = true;	// up-side-down marker
					}
					else
					if (y > (gy + gh))
						y = gy + gh;

					drawMarker(graph, x, y, up, false, false, s[0], s[1], s[2]);
				}
			}
		}
	}
}

void __fastcall CGraphs::drawMinMarkers(const int graph, const int graph_type, const double level_min, const double level_max, const double gamma, String units, String id1, String id2)
{
	if (graph < 0 || graph >= MAX_GRAPHS)
		return;

//	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

//	const bool freq_mode  = isFrequencyGraph(graph_type);
//	const bool smith_mode = isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);
//	const bool tdr_mode   = isTDRGraph(graph_type);

	const double level_range = fabs(level_max - level_min);

//	const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
	const double y_scale = (double)gh / level_range;

	for (int m = 0; m < MAX_MEMORIES; m++)
	{
		if (settings.memoryEnable[m])
		{
			for (int c = 0; c < MAX_CHANNELS; c++)
			{
				float value = 0;
				int index = -1;
				for (unsigned int i = 0; i < m_levels[m][c].size(); i++)
				{
					const float level = m_levels[m][c][i];
					if (index < 0 || value > level)
					{
						value   = level;
						index   = i;
					}
				}

				if (index >= 0 && index < (int)data_unit.m_point_filt[m].size())
				{
					const int64_t Hz  = data_unit.m_point_filt[m][index].Hz;
					const float level = m_levels[m][c][index];

					String s[3];

					s[0] = (c <= 0) ? id1 : id2;
					s[0] += " min";
					s[0] = s[0].Trim();

					s[1] = common.freqToStr1(Hz, true, true, 6, false) + "Hz";

					//if (smith_mode)
						s[2] = common.valueToStr(level, true, true, "%#.6g") + units;

					const int x = IROUND(freqToX(graph, graph_type, Hz));

					int y;
//					if (gamma <= GAMMA_MIN)
//					{
//						y = gy - IROUND((level - level_max) * y_scale);
//					}
//					else
					{
						const double value = nonLinear(level, level_min, level_max, gamma);
						y = (gy + gh) - IROUND(value * y_scale);
					}

					bool up = true;

					if (y < gy)
					{
						y = gy;
					}
					else
					if (y > (gy + gh))
					{
						y = gy + gh;
						up = false;
					}

					drawMarker(graph, x, y, up, false, false, s[0], s[1], s[2]);
				}
			}
		}
	}
}

void __fastcall CGraphs::drawMarkersSmithPolar(const int graph, const int mem, const int channel, std::vector <t_pointf> &line_points, bool admittance, bool polar)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (mem < 0 || mem >= MAX_MEMORIES)
		return;

	const int size = data_unit.freqArraySize(mem);
	if (size <= 0)
		return;

	if (!Form1)
		return;

	const bool draw_marker_value = !settings.showMarkersOnGraph;
	const bool show_marker_text  = settings.showMarkerText;

	const float ref_impedance = 50;

	const int m_index = m_mouse.marker_index;
	const int mi      = Form1->selectedMarker();
	const int mi_nd   = Form1->nonDeltaMarker();

	int index_nd = -1;
	if (mi_nd >= 0)
		index_nd = data_unit.indexFreq(settings.m_markers_freq[mi_nd].Hz, mem);

	for (unsigned int i = 0; i < settings.m_markers_freq.size(); i++)
	{
		if ((int)i == mi)
			continue;	// skip the selected marker .. we draw it last

		const t_marker_freq marker = settings.m_markers_freq[i];

		const int index = data_unit.indexFreq(marker.Hz, mem);
		if (index >= 0 && index < size && index < (int)data_unit.m_point_filt[mem].size())
		{
			const bool highlighted = ((int)i == m_index) ? true : false;

			const int x = line_points[index].x;
			const int y = line_points[index].y;

			const bool dm = (marker.type == MARKER_TYPE_DELTA && index_nd >= 0 && index_nd < size && index_nd < (int)data_unit.m_point_filt[mem].size()) ? true : false;

			const double Hz_nd = dm ? data_unit.m_point_filt[mem][index_nd].Hz : 0;

			const double Hz = data_unit.m_point_filt[mem][index].Hz;
			complexf c = data_unit.m_point_filt[mem][index].sParam[channel];

			if (dm)
				c -= data_unit.m_point_filt[mem][index_nd].sParam[channel];

			const complexf imp   = data_unit.impedance(c, ref_impedance);
			const complexf imp_p = data_unit.serialToParallel(imp);
			const float mag      = data_unit.magnitude(c);
			const float phase    = data_unit.phase(c);
			//const float phase  = (c.real != 0.0f) ? atan2f(c.imag(), c.real) : 0.0f;
			const float cap      = data_unit.impedanceToCapacitance(imp, Hz);
			const float ind      = data_unit.impedanceToInductance(imp, Hz);
			const float cap_p    = data_unit.impedanceToCapacitance(imp_p, Hz);
			const float ind_p    = data_unit.impedanceToInductance(imp_p, Hz);

			const float res    = imp.real();
			const float res_j  = ABS(imp.imag());

			const float resp   = imp_p.real();
			const float resp_j = ABS(imp_p.imag());

			String res_str   = common.valueToStr(res, false, true);
			String resj_str  = common.valueToStr(res_j, false, true);

			String resp_str  = common.valueToStr(resp, false, true);
			String respj_str = common.valueToStr(resp_j, false, true);

			String cap_str   = common.valueToStr(cap, false, true) + "F";
			String capp_str  = common.valueToStr(cap_p, false, true) + "F";

			String ind_str   = common.valueToStr(ind, false, true) + "H";
			String indp_str  = common.valueToStr(ind_p, false, true) + "H";

			String s[7];
			int j = 0;

			if ((show_marker_text || highlighted) && draw_marker_value)
			{	// more marker details
				if (marker.type == MARKER_TYPE_DELTA)
					s[j++].printf(L" Delta marker %u ", 1 + i);
				else
					s[j++].printf(L" Marker  %u ", 1 + i);

				s[j++] =          " Freq    " + common.freqToStr1(Hz - Hz_nd, true, true, 6, dm) + "Hz";

				//s[j++] =          " re im     " + common.valueToStr(c.real(), false, true, "", true) + " " + common.valueToStr(c.imag(), false, true, "", true);

				if (polar)
				{	// polar
					//s[j++].printf(L" Rs jX    %#.4g %cj%#.4g ", imp.real(), (imp.imag() < 0) ? '-' : '+', fabsf(imp.imag()));
					s[j++] =       " Rs jX    " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
					s[j++].printf(L" Rs L/C   %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
					s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}
				else
				if (admittance)
				{	// admittance
					//s[j++].printf(L" Rp+jX    %#.4g %cj%#.4g ", imp_p.real(), (imp_p.imag() < 0) ? '-' : '+', fabsf(imp_p.imag()));
					s[j++] =       " Rp jX    " + resp_str + " " + ((imp_p.imag() >= 0) ? "+j" : "-j") + respj_str;
					s[j++].printf(L" Rp L/C   %#.4g %s ", imp_p.real(), ((imp_p.imag() < 0) ? capp_str : indp_str).c_str());
					s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}
				else
				{	// smith
					//s[j++].printf(L" Rs jX    %#.4g %cj%#.4g ", imp.real(), (imp.imag() < 0) ? '-' : '+', fabsf(imp.imag()));
					s[j++] =       " Rs jX    " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
					s[j++].printf(L" Rs L/C   %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
					s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}
			}
			else
			{	// just marker number
				if (marker.type == MARKER_TYPE_DELTA)
					s[j++].printf(L"D%u", 1 + i);
				else
					s[j++].printf(L"%u", 1 + i);
			}

			drawMarker(graph, x, y, false, highlighted, false, s[0], s[1], s[2], s[3], s[4], s[5]);
		}
	}

	if (mi >= 0)
	{	// draw the selected marker
		const t_marker_freq marker = settings.m_markers_freq[mi];
		const int index = data_unit.indexFreq(marker.Hz, mem);
		if (index >= 0 && index < size && index < (int)data_unit.m_point_filt[mem].size())
		{
			const bool highlighted = (mi == m_index) ? true : false;

			const int x = line_points[index].x;
			const int y = line_points[index].y;

			const bool dm = (marker.type == MARKER_TYPE_DELTA && index_nd >= 0 && index_nd < size && index_nd < (int)data_unit.m_point_filt[mem].size()) ? true : false;

			const double Hz_nd = dm ? data_unit.m_point_filt[mem][index_nd].Hz : 0;

			const double Hz = data_unit.m_point_filt[mem][index].Hz;
			complexf c = data_unit.m_point_filt[mem][index].sParam[channel];

			if (dm)
				c -= data_unit.m_point_filt[mem][index_nd].sParam[channel];

			const complexf imp   = data_unit.impedance(c, ref_impedance);
			const complexf imp_p = data_unit.serialToParallel(imp);
			const float mag      = data_unit.magnitude(c);
			const float phase    = data_unit.phase(c);
			const float cap      = data_unit.impedanceToCapacitance(imp, Hz);
			const float ind      = data_unit.impedanceToInductance(imp, Hz);
			const float cap_p    = data_unit.impedanceToCapacitance(imp_p, Hz);
			const float ind_p    = data_unit.impedanceToInductance(imp_p, Hz);

			const float res    = imp.real();
			const float res_j  = ABS(imp.imag());

			const float resp   = imp_p.real();
			const float resp_j = ABS(imp_p.imag());

			String res_str   = common.valueToStr(res, false, true);
			String resj_str  = common.valueToStr(res_j, false, true);

			String resp_str  = common.valueToStr(resp, false, true);
			String respj_str = common.valueToStr(resp_j, false, true);

			String cap_str   = common.valueToStr(cap, false, true) + "F";
			String capp_str  = common.valueToStr(cap_p, false, true) + "F";

			String ind_str   = common.valueToStr(ind, false, true) + "H";
			String indp_str  = common.valueToStr(ind_p, false, true) + "H";

			String s[7];
			int j = 0;

			if ((show_marker_text || highlighted) && draw_marker_value)
			{	// more marker details
				if (marker.type == MARKER_TYPE_DELTA)
					s[j++].printf(L" Delta marker   %u ", 1 + mi);
				else
					s[j++].printf(L" Marker  %u ", 1 + mi);

				s[j++] =          " Freq    " + common.freqToStr1(Hz - Hz_nd, true, true, 6, dm) + "Hz";

				//s[j++] =          " re im     " + common.valueToStr(c.real(), false, true, "", true) + " " + common.valueToStr(c.imag(), false, true, "", true);

				if (polar)
				{	// polar
					//s[j++].printf(L" Rs jX    %#.4g %cj%#.4g ", imp.real(), (imp.imag() < 0) ? '-' : '+', fabsf(imp.imag()));
					s[j++] =       " Rs jX    " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
					s[j++].printf(L" Rs L/C   %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
					s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}
				else
				if (admittance)
				{	// admittance
					//s[j++].printf(L" Rp jX    %#.4g %cj%#.4g ", imp_p.real(), (imp_p.imag() < 0) ? '-' : '+', fabsf(imp_p.imag()));
					s[j++] =       " Rp jX    " + resp_str + " " + ((imp_p.imag() >= 0) ? "+j" : "-j") + respj_str;
					s[j++].printf(L" Rp L/C   %#.4g %s ", imp_p.real(), ((imp_p.imag() < 0) ? capp_str : indp_str).c_str());
					s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}
				else
				{	// smith
					//s[j++].printf(L" Rs jX    %#.4g %cj%#.4g ", imp.real(), (imp.imag() < 0) ? '-' : '+', fabsf(imp.imag()));
					s[j++] =       " Rs jX    " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
					s[j++].printf(L" Rs L/C   %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
					s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}
			}
			else
			{	// just marker number
				if (marker.type == MARKER_TYPE_DELTA)
					s[j++].printf(L"D%u", 1 + mi);
				else
					s[j++].printf(L"%u", 1 + mi);
			}

			drawMarker(graph, x, y, false, highlighted, true, s[0], s[1], s[2], s[3], s[4], s[5]);
		}
	}
}

void __fastcall CGraphs::drawMarkersOnSmithPolarGraph(const int graph, const int mem, const int channel, bool admittance, bool polar)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (mem < 0 || mem >= MAX_MEMORIES)
		return;

	const int   gx = m_graph_pos[graph].gx;
	const int   gy = m_graph_pos[graph].gy;
	const int   gw = m_graph_pos[graph].gw;
	const int   gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	const int size = data_unit.freqArraySize(mem);
	if (size <= 0)
		return;

	if (!Form1)
		return;

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
		const int fsize = m_graph_bm->Canvas->Font->Size;
		m_graph_bm->Canvas->Brush->Style = bsClear;
	#else
	#endif

	const bool show_marker_text = settings.showMarkerText;

	const int m_index = m_mouse.marker_index;
	const int mi      = Form1->selectedMarker();
	const int mi_nd   = Form1->nonDeltaMarker();

	int index_nd = -1;
	if (mi_nd >= 0)
		index_nd = data_unit.indexFreq(settings.m_markers_freq[mi_nd].Hz, mem);

	const int gb = gy + gh - 5;

	String s = "                         ";
	#ifndef USE_OPENGL
		const int tx = gx + gw - m_graph_bm->Canvas->TextWidth(s);
		int y = gy;
	#else
		const int tx = gx + gw - glTextWidth(1, s);
		int y = gy;
	#endif

	const float ref_impedance = 50;

	for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
	{
		const t_marker_freq marker = settings.m_markers_freq[i];

		if (!show_marker_text && i != mi && i != m_index)
			continue;	// don't show the marker text

		const int index = data_unit.indexFreq(marker.Hz, mem);
		if (index < 0 || index >= (int)data_unit.m_point_filt[mem].size())
			continue;

		const bool dm = (marker.type == MARKER_TYPE_DELTA && index_nd >= 0 && index_nd < size && index_nd < (int)data_unit.m_point_filt[mem].size()) ? true : false;

		const double Hz_nd = dm ? data_unit.m_point_filt[mem][index_nd].Hz : 0;

		const double Hz = data_unit.m_point_filt[mem][index].Hz;
		complexf c = data_unit.m_point_filt[mem][index].sParam[channel];

		if (dm)
			c -= data_unit.m_point_filt[mem][index_nd].sParam[channel];

		const complexf imp   = data_unit.impedance(c, ref_impedance);
		const complexf imp_p = data_unit.serialToParallel(imp);
		const float mag      = data_unit.magnitude(c);
		const float phase    = data_unit.phase(c);
		const float cap      = data_unit.impedanceToCapacitance(imp, Hz);
		const float ind      = data_unit.impedanceToInductance(imp, Hz);
		const float cap_p    = data_unit.impedanceToCapacitance(imp_p, Hz);
		const float ind_p    = data_unit.impedanceToInductance(imp_p, Hz);

		const float res    = imp.real();
		const float res_j  = ABS(imp.imag());

		const float resp   = imp_p.real();
		const float resp_j = ABS(imp_p.imag());

		String res_str   = common.valueToStr(res, false, true);
		String resj_str  = common.valueToStr(res_j, false, true);

		String resp_str  = common.valueToStr(resp, false, true);
		String respj_str = common.valueToStr(resp_j, false, true);

		String cap_str   = common.valueToStr(cap, false, true) + "F";
		String capp_str  = common.valueToStr(cap_p, false, true) + "F";

		String ind_str   = common.valueToStr(ind, false, true) + "H";
		String indp_str  = common.valueToStr(ind_p, false, true) + "H";

		String s[7];
		int j = 0;

		if (marker.type == MARKER_TYPE_DELTA)
			s[j].printf(L" Delta marker %u ", 1 + i);
		else
			s[j].printf(L" Marker %u ", 1 + i);
		if (mem <= 0)
			s[j++] += ", live";
		else
			s[j++] += ", memory " + IntToStr(mem);
		s[j++] =          " Freq    " + common.freqToStr1(Hz - Hz_nd, true, true, 6, dm) + "Hz";
		s[j++] =          " re im   " + common.valueToStr(c.real(), false, true, "", true) + " " + common.valueToStr(c.imag(), false, true, "", true);
		if (polar)
		{	// polar
			//s[j++].printf(L" Rs jX    %#.4g %cj%#.4g ", imp.real(), (imp.imag() < 0) ? '-' : '+', fabsf(imp.imag()));
			s[j++] =       " Rs jX    " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
			s[j++].printf(L" Rs L/C   %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
			s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
			s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp), false, true);
		}
		else
		if (admittance)
		{	// admittance
			//s[j++].printf(L" Rp jX    %#.4g %cj%#.4g ", imp_p.real(), (imp_p.imag() < 0) ? '-' : '+', fabsf(imp_p.imag()));
			s[j++] =       " Rp jX    " + resp_str + " " + ((imp_p.imag() >= 0) ? "+j" : "-j") + respj_str;
			s[j++].printf(L" Rp L/C   %#.4g %s ", imp_p.real(), ((imp_p.imag() < 0) ? capp_str : indp_str).c_str());
			s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
			s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp_p), false, true);
		}
		else
		{	// smith
			//s[j++].printf(L" Rs jX    %#.4g %cj%#.4g ", imp.real(), (imp.imag() < 0) ? '-' : '+', fabsf(imp.imag()));
			s[j++] =       " Rs jX    " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
			s[j++].printf(L" Rs L/C   %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
			s[j++].printf(L" Mag Ang  %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
			s[j++] =       " Imp      " + common.valueToStr(data_unit.magnitude(imp), false, true);
		}

		#ifndef USE_OPENGL
			if (i == mi || i == m_index)
				m_graph_bm->Canvas->Font->Size = fsize + 2;

			if (i == mi || i == m_index)
				m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
			else
				m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;	// remove bold

			if (i == mi)	// selected marker
				m_graph_bm->Canvas->Font->Color = settings.m_colours.marker_selected;
			else
				m_graph_bm->Canvas->Font->Color = settings.m_colours.font;
		#else
			const int bold = (i == mi || i == m_index) ? 1 : 0;
			GLrgba font_col = colourToGLcolour((i == mi) ? settings.m_colours.marker_selected : settings.m_colours.font);
		#endif

		for (unsigned int k = 0; k < ARRAY_SIZE(s); k++)
		{
			if (!s[k].Trim().IsEmpty())
			{
				#ifndef USE_OPENGL
					int th;
					if (i != mi && k <= 0)
					{	// make the first line bold
						TFontStyles fstyle = m_graph_bm->Canvas->Font->Style;
						m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
						th = m_graph_bm->Canvas->TextHeight("Hq|");
						m_graph_bm->Canvas->TextOut(tx, y, s[k]);
						m_graph_bm->Canvas->Font->Style = fstyle;
					}
					else
					{
						th = m_graph_bm->Canvas->TextHeight("Hq|");
						m_graph_bm->Canvas->TextOut(tx, y, s[k]);
					}
					y += th;
				#else
					const int b = (i != mi && k <= 0) ? 1 : 0;	// always make the first line bold
					const int th = m_gl.font[b].height;
					glColor3ub(font_col.r, font_col.g, font_col.b);
					glTextOut(b, tx, y, s[k]);
					y += th;
				#endif
				if (y > gb)
					break;
			}
		}

		#ifndef USE_OPENGL
			if (m_graph_bm->Canvas->Font->Size != fsize)
				m_graph_bm->Canvas->Font->Size = fsize;
			y += m_graph_bm->Canvas->TextHeight("Hq|") / 2;
		#else
			y += m_gl.font[0].height / 2;
		#endif

		if (y >= gb)
			break;
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsSolid;
		if (m_graph_bm->Canvas->Font->Size != fsize)
			m_graph_bm->Canvas->Font->Size = fsize;
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawMouseSmithPolar(const int graph, const int graph_type, const int mem, const int channel, std::vector <t_pointf> &line_points, bool admittance, bool polar)
{
	String s[7];

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
	{
		if (m_pb)
			if (m_pb->Hint != "")
				m_pb->Hint = "";
		return;
	}

	const int m_graph  = m_mouse.graph;
	const int m_mem    = m_mouse.point_mem;
	const int m_index  = m_mouse.point_index;
	const int64_t m_Hz = m_mouse.Hz;
	const float m_mag  = m_mouse.mag;

	const t_graph_setting *gs = &settings.m_graph_setting[graph_type];

//	const float cx = m_graph_pos[graph].cx;
//	const float cy = m_graph_pos[graph].cy;
	const float gr = m_graph_pos[graph].cr;

	if (m_graph < 0 || mem < 0 || mem >= MAX_MEMORIES || gr <= 0)
	{
		if (m_pb)
			if (m_pb->Hint != "")
				m_pb->Hint = "";
		return;
	}

	const float ref_impedance = 50.0f;

	int str_count = 0;
	//int tx = -1;
	//int ty = -1;
	int tx = 0;
	int ty = 0;

	if (m_index < 0)
	{	// not a trace sample point - draw mouse cursor position data

		if (m_mag >= 0.0f && m_mag <= gs->max)
		{	// inside the circle chart
			if (graph == m_graph && mem == 0)
			{
				// choose a frequency and I/Q to use for the following computations
				const double Hz  = (data_unit.m_freq_cw_Hz > 0) ? data_unit.m_freq_cw_Hz : data_unit.m_freq_center_Hz;
				const complexf c = m_mouse.cpx;

				const complexf imp   = data_unit.impedance(c, ref_impedance);
				const complexf imp_p = data_unit.serialToParallel(imp);
				const float mag      = data_unit.magnitude(c);
				const float phase    = data_unit.phase(c);
				const float cap      = data_unit.impedanceToCapacitance(imp, Hz);
				const float ind      = data_unit.impedanceToInductance(imp, Hz);
				const float cap_p    = data_unit.impedanceToCapacitance(imp_p, Hz);
				const float ind_p    = data_unit.impedanceToInductance(imp_p, Hz);

				const float res    = imp.real();
				const float res_j  = ABS(imp.imag());

				const float resp   = imp_p.real();
				const float resp_j = ABS(imp_p.imag());

				String res_str   = common.valueToStr(res, false, true);
				String resj_str  = common.valueToStr(res_j, false, true);

				String resp_str  = common.valueToStr(resp, false, true);
				String respj_str = common.valueToStr(resp_j, false, true);

				String cap_str   = common.valueToStr(cap, false, true) + "F";
				String capp_str  = common.valueToStr(cap_p, false, true) + "F";

				String ind_str   = common.valueToStr(ind, false, true) + "H";
				String indp_str  = common.valueToStr(ind_p, false, true) + "H";

				s[str_count++] =          " Mouse ";
				s[str_count++] =          " Freq (CW) " + common.freqToStr1(Hz, true, true, 6, false) + "Hz";
				s[str_count++] =          " re im     " + common.valueToStr(c.real(), false, true, "", true) + " " + common.valueToStr(c.imag(), false, true, "", true);
				if (polar)
				{	// polar
					s[str_count++] =       " Rs jX     " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
					s[str_count++].printf(L" Rs L/C    %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
					s[str_count++].printf(L" Mag Ang   %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
				}
				else
				if (admittance)
				{  // admittance
					s[str_count++] =       " Rp jX     " + resp_str + " " + ((imp_p.imag() >= 0) ? "+j" : "-j") + respj_str;
					s[str_count++].printf(L" Rp L/C    %#.4g %s ", imp_p.real(), ((imp_p.imag() < 0) ? capp_str : indp_str).c_str());
					s[str_count++].printf(L" Mag Ang   %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[str_count++] =       " Imp       " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}
				else
				{  // smith
					s[str_count++] =       " Rs jX     " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
					s[str_count++].printf(L" Rs L/C    %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
					s[str_count++].printf(L" Mag Ang   %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[str_count++] =       " Imp       " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}

				if (tx < 0)
					tx = m_mouse.x;
				if (ty < 0)
					ty = m_mouse.y;
			}
		}
		else
		{
			if (m_pb)
				if (m_pb->Hint != "")
					m_pb->Hint = "";
		}
	}
	else
	if (settings.memoryEnable[mem] && m_Hz > 0)
	{
		int index = data_unit.indexFreq(m_Hz, mem);
		if (index < 0)
			index = m_index;

		if (index >= 0 && index < (int)data_unit.m_point_filt[mem].size())
		{
			const int x = line_points[index].x;
			const int y = line_points[index].y;

			// mouse marker
			if (m_mem < 0 || mem == m_mem)
				drawMouseMarker(x, y, MARKER_SIZE);

			// draw the text
			if (graph == m_graph && mem == m_mem)
			{
				// sweep point value
				const int64_t Hz = data_unit.m_point_filt[mem][index].Hz;
				const complexf c = data_unit.m_point_filt[mem][index].sParam[channel];

				const complexf imp   = data_unit.impedance(c, ref_impedance);
				const complexf imp_p = data_unit.serialToParallel(imp);
				const float mag      = data_unit.magnitude(c);
				const float phase    = data_unit.phase(c);
				const float cap      = data_unit.impedanceToCapacitance(imp, Hz);
				const float ind      = data_unit.impedanceToInductance(imp, Hz);
				const float cap_p    = data_unit.impedanceToCapacitance(imp_p, Hz);
				const float ind_p    = data_unit.impedanceToInductance(imp_p, Hz);

				const float res    = imp.real();
				const float res_j  = ABS(imp.imag());

				const float resp   = imp_p.real();
				const float resp_j = ABS(imp_p.imag());

				String res_str   = common.valueToStr(res, false, true);
				String resj_str  = common.valueToStr(res_j, false, true);

				String resp_str  = common.valueToStr(resp, false, true);
				String respj_str = common.valueToStr(resp_j, false, true);

				String cap_str   = common.valueToStr(cap, false, true) + "F";
				String capp_str  = common.valueToStr(cap_p, false, true) + "F";

				String ind_str   = common.valueToStr(ind, false, true) + "H";
				String indp_str  = common.valueToStr(ind_p, false, true) + "H";

				if (mem <= 0)
					s[str_count++].printf(L" Point %d/%u, live", 1 + index, line_points.size());
				else
					s[str_count++].printf(L" Point %d/%u, mem %d", 1 + index, line_points.size(), mem);
				s[str_count++] =          " Freq      " + common.freqToStr1(Hz, true, true, 6, false) + "Hz";
				s[str_count++] =          " re im     " + common.valueToStr(c.real(), false, true, "", true) + " " + common.valueToStr(c.imag(), false, true, "", true);
				if (polar)
				{  // polar
					s[str_count++] =       " Rs jX     " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
					s[str_count++].printf(L" Rs L/C    %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
					s[str_count++].printf(L" Mag Ang   %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
				}
				else
				if (admittance)
				{	// admittance
					s[str_count++] =       " Rp jX     " + resp_str + " " + ((imp_p.imag() >= 0) ? "+j" : "-j") + respj_str;
					s[str_count++].printf(L" Rp L/C    %#.4g %s ", imp_p.real(), ((imp_p.imag() < 0) ? capp_str : indp_str).c_str());
					s[str_count++].printf(L" Mag Ang   %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[str_count++] =       " Imp       " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}
				else
				{	// smith
					s[str_count++] =       " Rs jX     " + res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
					s[str_count++].printf(L" Rs L/C    %#.4g %s ", imp.real(), ((imp.imag() < 0) ? cap_str : ind_str).c_str());
					s[str_count++].printf(L" Mag Ang   %0.4f %#.4g\xb0 ", mag, phase * rad_2_deg);
					s[str_count++] =       " Imp       " + common.valueToStr(data_unit.magnitude(imp), false, true);
				}

				if (tx < 0)
					tx = x;
				if (ty < 0)
					ty = y;
			}
		}
		else
		{
			if (m_pb)
				if (m_pb->Hint != "")
					m_pb->Hint = "";
		}
	}

	// draw the details
/*
	if (m_pb)
	{
		if (str_count > 0)
		{
			String s2;
			for (int k = 0; k < str_count; k++)
				s2 += (k <= 0) ? s[k] : String("\n") + s[k];
			if (m_pb->Hint != s2)
				m_pb->Hint = s2;
		}
	}
*/
	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");

		m_graph_bm->Canvas->Brush->Style = bsClear;
		//m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
		m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
		m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style >> fsBold;	// remove bold
	#else
		GLrgba font_col = colourToGLcolour(settings.m_colours.font);
		const int th = m_gl.font[0].height;
	#endif

	{
		int max_w = 0;
		int max_h = 0;
		for (int k = 0; k < str_count; k++)
		{
			#ifndef USE_OPENGL
				if (k == 0)
				{
					TFontStyles fs = m_graph_bm->Canvas->Font->Style;
					m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;	// make bold
					const int w = m_graph_bm->Canvas->TextWidth(s[k]);
					if (max_w < w)
						max_w = w;
					max_h += th;
					m_graph_bm->Canvas->Font->Style = fs;
				}
				else
				{
					const int w = m_graph_bm->Canvas->TextWidth(s[k]);
					if (max_w < w)
						max_w = w;
					max_h += th;
				}
			#else
				const int w = glTextWidth((k == 0) ? 1 : 0, s[k]);	// bold/not
				if (max_w < w)
					max_w = w;
				max_h += th;
			#endif
		}

		if (max_w > 0 && max_h > 0)
		{
			#ifndef USE_OPENGL
				if (tx <= 0)
					tx = m_graph_pos[graph].x;
				else
					tx += 40;

				if (ty <= 0)
					ty = m_graph_pos[graph].y + m_graph_pos[graph].h - max_h - th;
				else
					ty -= max_h + 40;

				if (tx < 3)
					tx = 3;
				else
				if (tx > (m_graph_bm->Width - 3 - max_w))
					tx = m_graph_bm->Width - 3 - max_w;

				if (ty < 3)
					ty = 3;
				else
				if (ty > (m_graph_bm->Height - 3 - max_h))
					ty = m_graph_bm->Height - 3 - max_h;

				if (m_graph_bm->Canvas->Brush->Style != bsClear)
				{	// clear the text background
					m_graph_bm->Canvas->Pen->Style = psSolid;
					m_graph_bm->Canvas->Pen->Color = settings.m_colours.border;
					m_graph_bm->Canvas->Rectangle(TRect(tx - 2, ty - 2, tx + max_w + 2, ty + max_h + 2));
				}
			#else
				if (tx <= 0)
					tx = m_graph_pos[graph].x;
				else
					tx += 40;

				if (ty <= 0)
					ty = m_graph_pos[graph].y + m_graph_pos[graph].h - max_h - th;
				else
					ty -= max_h + 40;

				if (tx < 3)
					tx = 3;
				else
				if (tx > (m_gl.win_control->ClientWidth - 3 - max_w))
					tx = m_gl.win_control->ClientWidth - 3 - max_w;

				if (ty < 3)
					ty = 3;
				else
				if (ty > (m_gl.win_control->ClientHeight - 3 - max_h))
					ty = m_gl.win_control->ClientHeight - 3 - max_h;
			#endif
		}
	}

	for (int k = 0; k < str_count; k++)
	{
		#ifndef USE_OPENGL
			if (k == 0)
			{
				TFontStyles fs = m_graph_bm->Canvas->Font->Style;
				m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;	// make bold
				m_graph_bm->Canvas->TextOut(tx, ty, s[k]);
				m_graph_bm->Canvas->Font->Style = fs;
			}
			else
			{
				m_graph_bm->Canvas->TextOut(tx, ty, s[k]);
			}
			ty += th;
		#else
			const int bold = (k == 0) ? 1 : 0;
			glColor3ub(font_col.r, font_col.g, font_col.b);
			glTextOut(bold, tx, ty, s[k]);
			ty += th;
		#endif
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

double __fastcall CGraphs::computeGraticuleScale(const double min_spacing, const double scale, double &step, int &mult_index)
{
	// range of table values must be >=1.0 and <10
	// we move the stepped spacing up according to this table
	const double mult[] = {1.0, 2.0, 2.5, 5.0};
	//const double mult[] = {1.0, 2.5, 5.0};
	//const int mult[] = {1, 2, 5};

	if (step <= 0.0)
		step = 1.0;

	if (mult_index < 0)
	{
		mult_index = 0;
	}
	else
	if (mult_index >= (int)ARRAY_SIZE(mult))
	{  // next magnitude
		mult_index = 0;
		step *= 10;
	}

	while ((step * mult[mult_index] * scale) < min_spacing)
	{
		if (++mult_index >= (int)ARRAY_SIZE(mult))
		{  // next magnitude
			mult_index = 0;
			step *= 10;
		}
	}
/*
	uint64_t s = 1;
	while (s < step)
		s *= 10;
	step = s;
*/
	return step * mult[mult_index]; // the graticule/grid line spacing to use
}

void __fastcall CGraphs::drawFreqLines(const int graph, const int graph_type)
{	// draw frequency scale

	if (data_unit.m_freq_span_Hz <= 0)
		return;

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	const int alpha = GRID_ALPHA;
	const int freq_bands_alpha = 32;

	uint8_t red, grn, blu;
	colourToRGB(settings.m_colours.background, red, grn, blu);
//	const uint8_t alpha   = 32;
	const uint8_t level   = ((uint16_t)red + (uint16_t)grn + (uint16_t)blu) / 3;
	const uint8_t grey    = (level >= 128) ? 0 : 255;

	const bool show_freq_bands = settings.showFrequencyBands;

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		//const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		//const int th = m_gl.font[0].height;
	#endif

	const double range_Hz = ABS(data_unit.m_freq_stop_Hz - data_unit.m_freq_start_Hz);

	const double x_scale = (double)gw / range_Hz;

	// compute and save a suitable graticule/grid scale to show on screen
	std::vector < std::pair <int, String> > lines;
	{
		#if 1

			const int min_str_spacing = 10;
			int mult_index = -1;
			double step_size = 1.0;
			do
			{
				const double Hz_step = computeGraticuleScale(min_str_spacing, x_scale, step_size, ++mult_index);
				int x2 = 0;
				double Hz = floor((double)data_unit.m_freq_start_Hz / Hz_step) * Hz_step;
				while (Hz <= data_unit.m_freq_stop_Hz)
				{
					const int x = IROUND((Hz - data_unit.m_freq_start_Hz) * x_scale);
					if (Hz >= data_unit.m_freq_start_Hz && Hz <= data_unit.m_freq_stop_Hz && x >= 0 && x <= gw)
					{
						const String s = common.freqToStr1(Hz, true, false, 6, false);
						#ifndef USE_OPENGL
							const int tw2 = m_graph_bm->Canvas->TextWidth(s) / 2;
						#else
							const int tw2 = glTextWidth(0, s) / 2;
						#endif
						if (!lines.empty() && (x - tw2) < (x2 + min_str_spacing))
						{	// to close together - go round again and try the next scale up
							lines.resize(0);
							break;
						}
						x2 = x + tw2;
						lines.push_back(std::make_pair(gx + x, s));
					}
					Hz += Hz_step;
				}
			} while (lines.empty());

		#else

			int mult_index   = 0;
			double step_size = 1.0;
			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth("000.000MHz");
			#else
				const int tw = glTextWidth(0, "000.000MHz");
			#endif
			step_size = computeGraticuleScale(tw + min_str_spacing, x_scale, step_size, mult_index);
			if (step_size > 0.0)
			{
				double Hz = floor((double)data_unit.m_freq_start_Hz / step_size) * step_size;
				while (Hz <= data_unit.m_freq_stop_Hz)
				{
					const int x = IROUND((Hz - data_unit.m_freq_start_Hz) * x_scale);
					if (Hz >= data_unit.m_freq_start_Hz && Hz <= data_unit.m_freq_stop_Hz && x >= 0 && x <= gw)
					{
						const String s = common.freqToStr1(Hz, true, false, 6, false);
						lines.push_back(std::make_pair(gx + x, s));
					}
					Hz += step_size;
				}
			}

		#endif
	}

	if (graph == m_mouse.graph_h_scale && m_mouse.graph_h_scale >= 0 && m_mouse.graph_scale_pos >= 0)
	{	// mouse is over the horizontal graph scale
		TRect rect;

		#ifndef USE_OPENGL
			const Gdiplus::SmoothingMode sm = m_gdi_plus->GetSmoothingMode();
			m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		#else
		#endif

		#if 1
			rect.Left   = gx + (gw * (m_mouse.graph_scale_pos + 0)) / 3;
			rect.Right  = gx + (gw * (m_mouse.graph_scale_pos + 1)) / 3;
			rect.Top    = gy + gh;
			rect.Bottom = gy + gh + 30;
		#else
			switch (m_mouse.graph_scale_pos)
			{
				case 0:
					rect.Left   = gx + (gw * 0) / 3;
					rect.Right  = gx + (gw * 2) / 3;
					rect.Top    = gy + gh;
					rect.Bottom = gy + gh + 30;
					break;
				case 1:
					rect.Left   = gx + (gw * (m_mouse.graph_scale_pos + 0)) / 3;
					rect.Right  = gx + (gw * (m_mouse.graph_scale_pos + 1)) / 3;
					rect.Top    = gy + gh;
					rect.Bottom = gy + gh + 30;
					break;
				case 2:
					rect.Left   = gx + (gw * 1) / 3;
					rect.Right  = gx + (gw * 3) / 3;
					rect.Top    = gy + gh;
					rect.Bottom = gy + gh + 30;
					break;
			}
		#endif

		#ifndef USE_OPENGL
			Gdiplus::Point points[] = {
				Gdiplus::Point(rect.Left,  rect.Top),
				Gdiplus::Point(rect.Right, rect.Top),
				Gdiplus::Point(rect.Right, rect.Bottom),
				Gdiplus::Point(rect.Left,  rect.Bottom)};

			Gdiplus::GraphicsPath path;
			path.AddLines(points, ARRAY_SIZE(points));

			Gdiplus::PathGradientBrush brush(&path);

			#if 0
				brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() / 2), rect.Top));
			#else
				switch (m_mouse.graph_scale_pos)
				{
					case 0:
						brush.SetCenterPoint(Gdiplus::Point(rect.Left + 10, rect.Top));
						break;
					case 1:
						brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 0.5f), rect.Top));
						break;
					case 2:
						brush.SetCenterPoint(Gdiplus::Point(rect.Right - 10, rect.Top));
						break;
				}
			#endif
			brush.SetCenterColor(Gdiplus::Color(48, grey, grey, grey));

			//Gdiplus::Color outside_colours[] = {
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey)};
			//int outside_colours_count = ARRAY_SIZE(outside_colours);
			//brush.SetSurroundColors(outside_colours, &outside_colours_count);
			Gdiplus::Color outside_colour = Gdiplus::Color(0, grey, grey, grey);
			int outside_colour_count = 1;
			brush.SetSurroundColors(&outside_colour, &outside_colour_count);

			m_gdi_plus->FillPath(&brush, &path);

			brush.SetFocusScales(0.25f, 0.25f);

			m_gdi_plus->FillPath(&brush, &path);

			m_gdi_plus->SetSmoothingMode(sm);
		#else
			GLrgba ocol(grey, grey, grey,  0);
			GLrgba icol(grey, grey, grey, 64);
			//glShadeModel(GL_FLAT);
			glShadeModel(GL_SMOOTH);
			glBegin(GL_TRIANGLE_FAN);
				glColor4ub(icol.r, icol.g, icol.b, icol.a);
				glVertex2i(rect.Left + (rect.Width() / 2), rect.Top + (rect.Height() / 2));	// center
				glColor4ub(ocol.r, ocol.g, ocol.b, ocol.a);
				glVertex2i(rect.Left, rect.Top);												// top left
				glVertex2i(rect.Left + rect.Width(), rect.Top);							// top right
				glVertex2i(rect.Left + rect.Width(), rect.Top + rect.Height());	// bottom right
				glVertex2i(rect.Left, rect.Top + rect.Height());						// bottom left
				glVertex2i(rect.Left, rect.Top);												// top left
			glEnd();
			glShadeModel(GL_FLAT);
		#endif
	}

	if (show_freq_bands)
	{
		const int64_t start_Hz = data_unit.m_freq_start_Hz;
		const int64_t stop_Hz  = data_unit.m_freq_stop_Hz;

		TRect rect;
		rect.Top    = gy;
		rect.Bottom = gy + gh;

		#ifndef USE_OPENGL
			//m_graph_bm->Canvas->Brush->Color = TColor(RGB(240, 240, 240));
			//m_graph_bm->Canvas->Brush->Style = bsSolid;
			const Gdiplus::SmoothingMode sm = m_gdi_plus->GetSmoothingMode();
			m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
			Gdiplus::SolidBrush brush(Gdiplus::Color(freq_bands_alpha, grey, grey, grey));	// ARGB
		#else
			glColor4ub(grey, grey, grey, freq_bands_alpha);
			glEnable(GL_POLYGON_SMOOTH);
			glShadeModel(GL_SMOOTH);					// Enable smooth shading
			glBegin(GL_QUADS);
		#endif

		for (unsigned int i = 0; i < settings.m_freq_band.size(); i++)
		{
			const t_freq_band fb = settings.m_freq_band[i];
			if (fb.enabled && fb.low_Hz <= stop_Hz && fb.high_Hz >= start_Hz)
			{	// draw the frequency band background
				rect.Left  = gx + (int)floor(((double)gw / 2) + ((fb.low_Hz  - data_unit.m_freq_center_Hz) * x_scale));
				rect.Right = gx + (int)ceil( ((double)gw / 2) + ((fb.high_Hz - data_unit.m_freq_center_Hz) * x_scale));
				if (rect.Left < gx)
					 rect.Left = gx;
				if (rect.Right > (gx + gw))
					 rect.Right = gx + gw;
				#ifndef USE_OPENGL
					//m_graph_bm->Canvas->FillRect(rect);
					m_gdi_plus->FillRectangle(&brush, rect.left, rect.top, rect.Width(), rect.Height());
				#else
					glVertex2i(rect.left, rect.top);
					glVertex2i(rect.left + rect.Width(), rect.top);
					glVertex2i(rect.left + rect.Width(), rect.top + rect.Height());
					glVertex2i(rect.left, rect.top + rect.Height());
				#endif
			}
		}

		#ifndef USE_OPENGL
			m_gdi_plus->SetSmoothingMode(sm);
		#else
			glEnd();
			glShadeModel(GL_FLAT);
			glDisable(GL_POLYGON_SMOOTH);
		#endif
	}

	if (!lines.empty())
	{	// draw the graticule/grid lines

		uint8_t red, grn, blu;
		colourToRGB(settings.m_colours.grid, red, grn, blu);

		#ifndef USE_OPENGL
			const Gdiplus::SmoothingMode sm = m_gdi_plus->GetSmoothingMode();
			m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);		// fast

			Gdiplus::Pen pen(Gdiplus::Color(alpha, red, grn, blu), 1);
			pen.SetAlignment(Gdiplus::PenAlignmentCenter);
			//pen.SetDashStyle(Gdiplus::DashStyleSolid);
//			pen.SetDashStyle(Gdiplus::DashStyleDash);
			pen.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));

			{	// left side
				const int x = gx;
				m_gdi_plus->DrawLine(&pen, x, gy, x, gy + gh);
			}

			for (unsigned int i = 0; i < lines.size(); i++)
			{
				//const int x = lines[i].pos;
				const int x = lines[i].first;
				if (x > gx && x < (gx + gw))
					m_gdi_plus->DrawLine(&pen, x, gy, x, gy + gh);
			}

			{	// right side
				const int x = gx + gw;
				m_gdi_plus->DrawLine(&pen, x, gy, x, gy + gh);
			}

			m_gdi_plus->SetSmoothingMode(sm);
		#else
			glLineStipple(1, 0xcccc);
			glEnable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
			//glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			glColor4ub(red, grn, blu, alpha);

			//glBegin(GL_LINE_STRIP);
			glBegin(GL_LINES);

			{	// left side
				const int x = gx;
				glVertex2i(x, gy);
				glVertex2i(x, gy + gh);
			}

			for (unsigned int i = 0; i < lines.size(); i++)
			{
				//const int x = lines[i].pos;
				const int x = lines[i].first;
				if (x > gx && x < (gx + gw))
				{
					glVertex2i(x, gy);
					glVertex2i(x, gy + gh);
				}
			}

			{	// right side
				const int x = gx + gw;
				glVertex2i(x, gy);
				glVertex2i(x, gy + gh);
			}

			glEnd();
			glDisable(GL_LINE_STIPPLE);

		#endif
	}

	{	// draw the frequency text scale
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Brush->Style = bsClear;
			m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
			for (unsigned int i = 0; i < lines.size(); i++)
			{
				const int x = lines[i].first;
				String s = lines[i].second;
				m_graph_bm->Canvas->TextOut(x - (m_graph_bm->Canvas->TextWidth(s) / 2), gy + gh + 5, s);
			}
		#else
			GLrgba font_col = colourToGLcolour(settings.m_colours.font);
			glColor3ub(font_col.r, font_col.g, font_col.b);
			for (unsigned int i = 0; i < lines.size(); i++)
			{
				const int x = lines[i].first;
				String s = lines[i].second;
				glTextOut(0, x - (glTextWidth(0, s) / 2), gy + gh + 5, s);
			}
		#endif
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawTimeDistLines(const int graph, const int mem)
{	// draw time/dist scale

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	const int alpha = GRID_ALPHA;

	#ifndef USE_OPENGL
		const Gdiplus::SmoothingMode sm = m_gdi_plus->GetSmoothingMode();
		m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);		// fast
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);		// high quality
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

		TFont *font = m_graph_bm->Canvas->Font;

		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold

		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
	#endif

	const double freq_step = data_unit.freq_step(mem);
	const double max_time  = data_unit.max_time(freq_step);
	const double max_dist  = data_unit.max_dist(freq_step, data_unit.m_velocity_factor);

	if (freq_step <= 0.0 || max_time <= 0.0 || max_dist <= 0.0)
		return;

	const float time_scale = (float)gw / max_time;

	double time_step = max_time / 10;

	{	// make non-fraction steps
		int64_t int_scale = 1;
		while (int_scale > 0 && int_scale < (1.0 / max_time))
			int_scale *= 10;
		int_scale *= 10;
		time_step = floor(time_step * int_scale) / int_scale;
	}

	std::vector < std::pair <int, double> > lines;
	for (double tim = 0; tim <= max_time + (time_step / 2); tim += time_step)
	{
		const int x = gx + IROUND(tim * time_scale);
		if (x >= gx && x <= (gx + gw))
			lines.push_back(std::make_pair(x, tim));
	}

	{	// draw the grid lines
		uint8_t red, grn, blu;
		colourToRGB(settings.m_colours.grid, red, grn, blu);

		#ifndef USE_OPENGL
			Gdiplus::Pen pen(Gdiplus::Color(alpha, red, grn, blu), 1);
			pen.SetAlignment(Gdiplus::PenAlignmentCenter);
			//pen.SetDashStyle(Gdiplus::DashStyleSolid);
			//pen.SetDashStyle(Gdiplus::DashStyleDash);
			pen.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));

			for (unsigned int i = 0; i < lines.size(); i++)
			{
				const int x = lines[i].first;
				m_gdi_plus->DrawLine(&pen, x, gy, x, gy + gh);
			}
		#else
			glLineStipple(1, 0xcccc);
			glEnable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
			//glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			glColor4ub(red, grn, blu, alpha);
			//glBegin(GL_LINE_STRIP);
			glBegin(GL_LINES);
				for (unsigned int i = 0; i < lines.size(); i++)
				{
					const int x = lines[i].first;
					glVertex2i(x, gy);
					glVertex2i(x, gy + gh);
				}
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		#endif
	}

	const int min_str_spacing = 5;

	int x1 = -min_str_spacing;
	int x2 = -min_str_spacing;

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
		m_graph_bm->Canvas->Brush->Style = bsClear;
	#else
		GLrgba font_col = colourToGLcolour(settings.m_colours.font);
		glColor3ub(font_col.r, font_col.g, font_col.b);
	#endif

	for (unsigned int i = 0; i < lines.size(); i++)
	{
		String s1;
		String s2;

		const int x      = lines[i].first;
		const double tim = lines[i].second;

		s1 = common.secsToStr(tim);
		#ifndef USE_OPENGL
			const int tw1 = m_graph_bm->Canvas->TextWidth(s1);
		#else
			const int tw1 = glTextWidth(0, s1);
		#endif
		const int tx1 = x - (tw1 / 2);
		const int ty1 = gy - th - 5;

		s2 = common.distToStr((tim * max_dist) / max_time);
		#ifndef USE_OPENGL
			const int tw2 = m_graph_bm->Canvas->TextWidth(s2);
		#else
			const int tw2 = glTextWidth(0, s2);
		#endif
		const int tx2 = x - (tw2 / 2);
		const int ty2 = gy + gh + 5;

		if (tx1 >= (x1 + min_str_spacing))
		{	// time
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->TextOut(tx1, ty1, s1);
			#else
				glTextOut(0, tx1, ty1, s1);
			#endif
			x1 = tx1 + tw1;
		}

		if (tx2 >= (x2 + min_str_spacing))
		{	// dist
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->TextOut(tx2, ty2, s2);
			#else
				glTextOut(0, tx2, ty2, s2);
			#endif
			x2 = tx2 + tw2;
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
		m_gdi_plus->SetSmoothingMode(sm);
	#endif
}

void __fastcall CGraphs::drawNoneLinMagLines(const int graph, const bool left_side, const double level_min, const double level_max, double gamma, const double initial_step_size, String fmt, String units, const bool is_vswr)
{	// draw non-linear magnitude scale

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (gamma < GAMMA_MIN)
		gamma = GAMMA_MIN;

	const double level_range = fabs(level_max - level_min);
	if (level_range <= 0.0)
		return;

	const int alpha = GRID_ALPHA;

	uint8_t back_red, back_grn, back_blu;
	colourToRGB(settings.m_colours.background, back_red, back_grn, back_blu);
	const uint8_t level = ((uint16_t)back_red + (uint16_t)back_grn + (uint16_t)back_blu) / 3;
	const uint8_t grey  = (level >= 128) ? 0 : 255;

	#ifndef USE_OPENGL
		const Gdiplus::SmoothingMode sm = m_gdi_plus->GetSmoothingMode();
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		m_graph_bm->Canvas->Brush->Style = bsClear;
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
	#endif

	const double y_scale = (double)gh / level_range;

	int vswr2_y = 0;
	if (is_vswr)
	{	// compute the 2:1 vswr line position
		const double level = nonLinear(2.0, level_min, level_max, gamma);
		vswr2_y = (gy + gh) - IROUND(level * y_scale);
	}

	std::vector < std::pair <double, String> > lines;

	// compute and save a suitable graticule/grid scale to show on screen
	if (gamma <= GAMMA_MIN)
	{	// linear scale
		const int min_str_spacing = th + 1;
		int mult_index = 0;
		double step_size = initial_step_size;
		step_size = computeGraticuleScale(min_str_spacing, y_scale, step_size, mult_index);

		if (is_vswr && level_min == 1.0)
		{
			const double level = 1.0;
			const double y = (level_max - level) * y_scale;
			String s = "1" + units;
			lines.push_back(std::make_pair(gy + y, s));
		}

		double level = floor(level_min / step_size) * step_size;
		while (level <= level_max)
		{
			const double y = (level_max - level) * y_scale;
			if (level >= level_min && level <= level_max)
			{
				if (y >= 0 && y <= gh)
				{
					String s;
					if (fmt.IsEmpty())
					{
						s = common.valueToStr(level, false, true, "%#.6f");
					}
					else
					{
						s.printf(fmt.c_str(), level);
						s = common.trimTrailingZeros(s);
					}
					s += units;
					lines.push_back(std::make_pair(gy + y, s));
				}
			}
			level += step_size;
		}
	}
	else
	{	// non-linear scale
		const int min_str_spacing = th * 1;
		double step_size = initial_step_size;
		double y_prev = 0.0;
		double level = floor((double)level_min / step_size) * step_size;

		while (true)
		{
			if (level >= level_min)
			{
				const double value = nonLinear(level, level_min, level_max, gamma);
				const double y = (gy + gh) - (value * y_scale);
				if (y < gy)
					break;
				if (lines.empty() || y <= (y_prev - min_str_spacing))
				{
					String s;
					String f = fmt;
					if (f.IsEmpty())
					{
						f = "%#.6f";
						if (is_vswr)
						{
							f = "%#.0f";
							if (level < 2.0) f = "%#.5f";
							else
							if (level < 10.0) f = "%#.2f";
							else
							if (level < 100.0) f = "%#.1f";
						}
						else
						{
							if (level < 0.001) f = "%#.7f";
							else
							if (level < 0.01) f = "%#.6f";
							else
							if (level < 0.1) f = "%#.5f";
							else
							if (level < 1.0) f = "%#.4f";
							else
							if (level < 10.0) f = "%#.3f";
							else
							if (level < 100.0) f = "%#.2f";
						}
						s = common.valueToStr(level, false, true, f);
					}
					else
						s.printf(f.c_str(), level);
					s = common.trimTrailingZeros(s) + units;
					lines.push_back(std::make_pair(y, s));
					y_prev = y;
				}
			}
			level += step_size;
			if ((level - level_min) >= (step_size * 10))
			{	// next magnitude
				step_size *= 10;
				level = level_min + step_size;
			}
		}
	}

	if (graph == m_mouse.graph_v_scale && m_mouse.graph_v_scale >= 0 && m_mouse.graph_scale_pos >= 0)
	{	// mouse is over the vertical graph scale
		TRect rect;

		#ifndef USE_OPENGL
			m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
			//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		#endif

		#if 1
			//rect.Left   = 0;
			rect.Left   = gx - 50;
			rect.Right  = gx;
			//rect.Right  = gx - 5;
			rect.Top    = gy + (gh * (m_mouse.graph_scale_pos + 0)) / 3;
			rect.Bottom = gy + (gh * (m_mouse.graph_scale_pos + 1)) / 3;
		#else
			switch (m_mouse.graph_scale_pos)
			{
				case 0:
					rect.Left   = gx - 50;
					rect.Right  = gx;
					rect.Top    = gy;
					rect.Bottom = gy + ((gh * 2) / 3);
					break;
				case 1:
					rect.Left   = gx - 50;
					rect.Right  = gx;
					rect.Top    = gy + (gh * (m_mouse.graph_scale_pos + 0)) / 3;
					rect.Bottom = gy + (gh * (m_mouse.graph_scale_pos + 1)) / 3;
					break;
				case 2:
					rect.Left   = gx - 50;
					rect.Right  = gx;
					rect.Top    = gy + ((gh * 1) / 3);
					rect.Bottom = gy + gh;
					break;
			}
		#endif

		#ifndef USE_OPENGL
			Gdiplus::Point points[] = {
				Gdiplus::Point(rect.Left,  rect.Top),
				Gdiplus::Point(rect.Right, rect.Top),
				Gdiplus::Point(rect.Right, rect.Bottom),
				Gdiplus::Point(rect.Left,  rect.Bottom)};

			Gdiplus::GraphicsPath path;
			path.AddLines(points, ARRAY_SIZE(points));

			Gdiplus::PathGradientBrush brush(&path);

			#if 0
				brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 1.0f), rect.Top + (rect.Height() / 2)));
			#else
				switch (m_mouse.graph_scale_pos)
				{
					case 0:
						brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 1.0f), rect.Top + 10));
						break;
					case 1:
						brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 1.0f), (rect.Top + rect.Bottom) / 2));
						break;
					case 2:
						brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 1.0f), rect.Bottom - 10));
						break;
				}
			#endif

			brush.SetCenterColor(Gdiplus::Color(48, grey, grey, grey));

			//Gdiplus::Color outside_colours[] = {
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey)};
			//int outside_colours_count = ARRAY_SIZE(outside_colours);
			//brush.SetSurroundColors(outside_colours, &outside_colours_count);
			Gdiplus::Color outside_colour = Gdiplus::Color(0, grey, grey, grey);
			int outside_colour_count = 1;
			brush.SetSurroundColors(&outside_colour, &outside_colour_count);

			m_gdi_plus->FillPath(&brush, &path);

			brush.SetFocusScales(0.25f, 0.25f);

			m_gdi_plus->FillPath(&brush, &path);

			m_gdi_plus->SetSmoothingMode(sm);
		#else
			GLrgba ocol(grey, grey, grey,  0);
			GLrgba icol(grey, grey, grey, 48);
			//glShadeModel(GL_FLAT);
			glShadeModel(GL_SMOOTH);
			glBegin(GL_TRIANGLE_FAN);
				glColor4ub(icol.r, icol.g, icol.b, icol.a);
				glVertex2i(rect.Left + (rect.Width() / 2), rect.Top + (rect.Height() / 2));	// center
				glColor4ub(ocol.r, ocol.g, ocol.b, ocol.a);
				glVertex2i(rect.Left, rect.Top);												// top left
				glVertex2i(rect.Left + rect.Width(), rect.Top);							// top right
				glVertex2i(rect.Left + rect.Width(), rect.Top + rect.Height());	// bottom right
				glVertex2i(rect.Left, rect.Top + rect.Height());						// bottom left
				glVertex2i(rect.Left, rect.Top);												// top left
			glEnd();
			glShadeModel(GL_FLAT);
		#endif
	}

	uint8_t grid_red, grid_grn, grid_blu;
	colourToRGB(settings.m_colours.grid, grid_red, grid_grn, grid_blu);

	if (!lines.empty())
	{	// draw the graticule/grid lines

		#ifndef USE_OPENGL
			m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
			Gdiplus::Pen pen(Gdiplus::Color(alpha, grid_red, grid_grn, grid_blu), 1);
			pen.SetAlignment(Gdiplus::PenAlignmentCenter);
			//pen.SetDashStyle(Gdiplus::DashStyleSolid);
			//pen.SetDashStyle(Gdiplus::DashStyleDash);
			pen.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));
		#else
			glLineStipple(1, 0xcccc);
			glEnable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
			//glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			glColor4ub(grid_red, grid_grn, grid_blu, alpha);
			//glBegin(GL_LINE_STRIP);
			glBegin(GL_LINES);
		#endif

		{	// top line
			const int y = gy;
			#ifndef USE_OPENGL
				m_gdi_plus->DrawLine(&pen, gx, y, gx + gw, y);
			#else
				glVertex2i(gx, y);
				glVertex2i(gx + gw, y);
			#endif
		}

		for (unsigned int i = 0; i < lines.size(); i++)
		{
			const int y = IROUND(lines[i].first);
			if (y > gy && y < (gy + gh))
			{
				#ifndef USE_OPENGL
					m_gdi_plus->DrawLine(&pen, gx, y, gx + gw, y);
				#else
					glVertex2i(gx, y);
					glVertex2i(gx + gw, y);
				#endif
			}
		}

		{	// bottom line
			const int y = gy + gh;
			#ifndef USE_OPENGL
				m_gdi_plus->DrawLine(&pen, gx, y, gx + gw, y);
			#else
				glVertex2i(gx, y);
				glVertex2i(gx + gw, y);
			#endif
		}

		#ifndef USE_OPENGL
			m_gdi_plus->SetSmoothingMode(sm);
		#else
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		#endif
	}

	// draw the graticule text
	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsClear;
		m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
	#else
		GLrgba font_col = colourToGLcolour(settings.m_colours.font);
		glColor3ub(font_col.r, font_col.g, font_col.b);
	#endif

	if (left_side)
	{
		for (unsigned int i = 0; i < lines.size(); i++)
		{
			String s = lines[i].second;
			#ifndef USE_OPENGL
				const int x = gx - 5 - m_graph_bm->Canvas->TextWidth(s);
			#else
				const int x = gx - 5 - glTextWidth(0, s);
			#endif
			const int y = IROUND(lines[i].first - (th / 2));
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->TextOut(x, y, s);
			#else
				glTextOut(0, x, y, s);
			#endif
		}
	}
	else
	{
		for (unsigned int i = 0; i < lines.size(); i++)
		{
			String s = lines[i].second;
			const int x = gx + gw + 5;
			const int y = IROUND(lines[i].first - (th / 2));
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->TextOut(x, y, s);
			#else
				glTextOut(0, x, y, s);
			#endif
		}
	}

	// draw the VSWR 2:1 line
	if (is_vswr && vswr2_y >= gy && vswr2_y <= (gy + gh))
	{
		uint8_t red, grn, blu;
		colourToRGB(settings.m_colours.vswr2_line, red, grn, blu);

		#ifndef USE_OPENGL
			m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);

			Gdiplus::Pen pen(Gdiplus::Color(alpha, red, grn, blu), 1);
			pen.SetAlignment(Gdiplus::PenAlignmentCenter);
			pen.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));

			m_gdi_plus->DrawLine(&pen, gx, vswr2_y, gx + gw, vswr2_y);

			m_gdi_plus->SetSmoothingMode(sm);
		#else
			glLineStipple(1, 0xaaaa);
			glEnable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
			//glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			glColor4ub(red, grn, blu, alpha);
			//glBegin(GL_LINE_STRIP);
			glBegin(GL_LINES);
				glVertex2i(gx, vswr2_y);
				glVertex2i(gx + gw, vswr2_y);
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		#endif
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawMagLines(const int graph, const bool left_side, const double level_min, const double level_max, const double initial_step_size, String fmt, String units)
{	// draw linear magnitude scale

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	const double level_range = fabs(level_max - level_min);
	if (level_range <= 0.0)
		return;

	const int alpha = GRID_ALPHA;

	#ifndef USE_OPENGL
		const Gdiplus::SmoothingMode sm = m_gdi_plus->GetSmoothingMode();
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
	#endif

	fmt = fmt.Trim();

	uint8_t back_red, back_grn, back_blu;
	colourToRGB(settings.m_colours.background, back_red, back_grn, back_blu);
	const uint8_t level = ((uint16_t)back_red + (uint16_t)back_grn + (uint16_t)back_blu) / 3;
	const uint8_t grey  = (level >= 128) ? 0 : 255;

	const double y_scale = (double)gh / level_range;

	// compute and save a suitable graticule/grid scale to show on screen
	std::vector < std::pair <double, String> > lines;
	{
		#if 0

			const int min_str_spacing = th + 10;
			const int th2 = th / 2;
			int mult_index = -1;
			double step_size = initial_step_size;
			do
			{
				const double level_step = computeGraticuleScale(th + min_str_spacing, y_scale, step_size, ++mult_index);
				double y2 = 0;
				double level = floor(level_min / level_step) * level_step;
				while (level <= level_max)
				{
					const double y = (level_max - level) * y_scale;
					if (level >= level_min && level <= level_max && y >= 0 && y <= gh)
					{
						if (!lines.empty() && (y + th2) > (y2 + min_str_spacing))
						{	// to close together - go round again and try the next scale up
							lines.resize(0);
							break;
						}
						y2 = y - th2;

						String s;
						if (fmt.IsEmpty())
						{
							s = common.valueToStr(level, false, true, "%#.6f");
						}
						else
						{
							s.printf(fmt.c_str(), level);
							if (s.Pos(common.decimalPoint()) > 0)
							{	// remove trailing zero's and unneeded decimal points
								while (!s.IsEmpty() && s[s.Length()] == '0')
									s = s.SubString(1, s.Length() - 1).Trim();
								if (!s.IsEmpty() && s[s.Length()] == common.decimalPoint())
								s = s.SubString(1, s.Length() - 1).Trim();
							}
						}
						s += units;
						lines.push_back(std::make_pair(gy + y, s));
					}
					level += level_step;
				}
			} while (lines.empty());

		#else

			const int min_str_spacing = th + 10;
			int mult_index = 0;
			double step_size = initial_step_size;
			step_size = computeGraticuleScale(min_str_spacing, y_scale, step_size, mult_index);
			double level = floor(level_min / step_size) * step_size;
			while (level <= level_max)
			{
				const double y = (level_max - level) * y_scale;
				if (level >= level_min && level <= level_max && y >= 0 && y <= gh)
				{
					String s;
					if (fmt.IsEmpty())
					{
						s = common.valueToStr(level, false, true, "%#.6f");
					}
					else
					{
						s.printf(fmt.c_str(), level);
						if (s.Pos(common.decimalPoint()) > 0)
						{	// remove trailing zero's and unneeded decimal points
							while (!s.IsEmpty() && s[s.Length()] == '0')
								s = s.SubString(1, s.Length() - 1).Trim();
							if (!s.IsEmpty() && s[s.Length()] == common.decimalPoint())
							s = s.SubString(1, s.Length() - 1).Trim();
						}
					}
					s += units;
					lines.push_back(std::make_pair(gy + y, s));
				}
				level += step_size;
			}

		#endif
	}

	if (graph == m_mouse.graph_v_scale && m_mouse.graph_v_scale >= 0 && m_mouse.graph_scale_pos >= 0)
	{	// mouse is over the vertical graph scale
		TRect rect;

		#ifndef USE_OPENGL
			m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
			//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		#endif

		#if 1
			//rect.Left   = 0;
			rect.Left   = gx - 50;
			rect.Right  = gx;
			//rect.Right  = gx - 5;
			rect.Top    = gy + (gh * (m_mouse.graph_scale_pos + 0)) / 3;
			rect.Bottom = gy + (gh * (m_mouse.graph_scale_pos + 1)) / 3;
		#else
			switch (m_mouse.graph_scale_pos)
			{
				case 0:
					rect.Left   = gx - 50;
					rect.Right  = gx;
					rect.Top    = gy;
					rect.Bottom = gy + ((gh * 2) / 3);
					break;
				case 1:
					rect.Left   = gx - 50;
					rect.Right  = gx;
					rect.Top    = gy + (gh * (m_mouse.graph_scale_pos + 0)) / 3;
					rect.Bottom = gy + (gh * (m_mouse.graph_scale_pos + 1)) / 3;
					break;
				case 2:
					rect.Left   = gx - 50;
					rect.Right  = gx;
					rect.Top    = gy + ((gh * 1) / 3);
					rect.Bottom = gy + gh;
					break;
			}
		#endif

		#ifndef USE_OPENGL
			Gdiplus::Point points[] = {
				Gdiplus::Point(rect.Left,  rect.Top),
				Gdiplus::Point(rect.Right, rect.Top),
				Gdiplus::Point(rect.Right, rect.Bottom),
				Gdiplus::Point(rect.Left,  rect.Bottom)};

			Gdiplus::GraphicsPath path;
			path.AddLines(points, ARRAY_SIZE(points));

			Gdiplus::PathGradientBrush brush(&path);

			#if 0
				brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 1.0f), rect.Top + (rect.Height() / 2)));
			#else
				switch (m_mouse.graph_scale_pos)
				{
					case 0:
						brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 1.0f), rect.Top + 10));
						break;
					case 1:
						brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 1.0f), (rect.Top + rect.Bottom) / 2));
						break;
					case 2:
						brush.SetCenterPoint(Gdiplus::Point(rect.Left + (rect.Width() * 1.0f), rect.Bottom - 10));
						break;
				}
			#endif

			brush.SetCenterColor(Gdiplus::Color(48, grey, grey, grey));

			//Gdiplus::Color outside_colours[] = {
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey),
			//	Gdiplus::Color(16, grey, grey, grey)};
			//int outside_colours_count = ARRAY_SIZE(outside_colours);
			//brush.SetSurroundColors(outside_colours, &outside_colours_count);
			Gdiplus::Color outside_colour = Gdiplus::Color(0, grey, grey, grey);
			int outside_colour_count = 1;
			brush.SetSurroundColors(&outside_colour, &outside_colour_count);

			m_gdi_plus->FillPath(&brush, &path);

			brush.SetFocusScales(0.25f, 0.25f);

			m_gdi_plus->FillPath(&brush, &path);

			m_gdi_plus->SetSmoothingMode(sm);
		#else
			GLrgba ocol(grey, grey, grey,  0);
			GLrgba icol(grey, grey, grey, 48);
			//glShadeModel(GL_FLAT);
			glShadeModel(GL_SMOOTH);
			glBegin(GL_TRIANGLE_FAN);
				glColor4ub(icol.r, icol.g, icol.b, icol.a);
				glVertex2i(rect.Left + (rect.Width() / 2), rect.Top + (rect.Height() / 2));	// center
				glColor4ub(ocol.r, ocol.g, ocol.b, ocol.a);
				glVertex2i(rect.Left, rect.Top);												// top left
				glVertex2i(rect.Left + rect.Width(), rect.Top);							// top right
				glVertex2i(rect.Left + rect.Width(), rect.Top + rect.Height());	// bottom right
				glVertex2i(rect.Left, rect.Top + rect.Height());						// bottom left
				glVertex2i(rect.Left, rect.Top);												// top left
			glEnd();
			glShadeModel(GL_FLAT);
		#endif
	}

	if (!lines.empty())
	{	// draw the graticule/grid lines

		uint8_t grid_red, grid_grn, grid_blu;
		colourToRGB(settings.m_colours.grid, grid_red, grid_grn, grid_blu);

		#ifndef USE_OPENGL
			m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
			Gdiplus::Pen pen(Gdiplus::Color(alpha, grid_red, grid_grn, grid_blu), 1);
			pen.SetAlignment(Gdiplus::PenAlignmentCenter);
			//pen.SetDashStyle(Gdiplus::DashStyleSolid);
			//pen.SetDashStyle(Gdiplus::DashStyleDash);
			pen.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));
		#else
			glLineStipple(1, 0xcccc);
			glEnable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
			//glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			glColor4ub(grid_red, grid_grn, grid_blu, alpha);
			//glBegin(GL_LINE_STRIP);
			glBegin(GL_LINES);
		#endif

		{	// top line
			const float y = gy;
			#ifndef USE_OPENGL
				m_gdi_plus->DrawLine(&pen, (float)gx, y, (float)(gx + gw), y);
			#else
				glVertex2i(gx, y);
				glVertex2i(gx + gw, y);
			#endif
		}

		for (unsigned int i = 0; i < lines.size(); i++)
		{
			const float y = lines[i].first;
			if (y > gy && y < (gy + gh))
			{
				#ifndef USE_OPENGL
					m_gdi_plus->DrawLine(&pen, (float)gx, y, (float)(gx + gw), y);
				#else
					glVertex2i(gx, y);
					glVertex2i(gx + gw, y);
				#endif
		  }
		}

		{	// bottom line
			const float y = gy + gh;
			#ifndef USE_OPENGL
				m_gdi_plus->DrawLine(&pen, (float)gx, y, (float)(gx + gw), y);
			#else
				glVertex2i(gx, y);
				glVertex2i(gx + gw, y);
			#endif
		}

		#ifndef USE_OPENGL
			m_gdi_plus->SetSmoothingMode(sm);
		#else
			glEnd();
			glDisable(GL_LINE_STIPPLE);
		#endif
	}

	// draw the graticule text

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsClear;
		m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
	#else
		GLrgba font_col = colourToGLcolour(settings.m_colours.font);
		glColor3ub(font_col.r, font_col.g, font_col.b);
	#endif

	if (left_side)
	{
		for (unsigned int i = 0; i < lines.size(); i++)
		{
			String s = lines[i].second;
			const float y = lines[i].first - (th / 2);
			#ifndef USE_OPENGL
				const float x = gx - 5 - m_graph_bm->Canvas->TextWidth(s);
				m_graph_bm->Canvas->TextOut(IROUND(x), IROUND(y), s);
			#else
				const float x = gx - 5 - glTextWidth(0, s);
				glTextOut(0, IROUND(x), IROUND(y), s);
			#endif
		}
	}
	else
	{
		for (unsigned int i = 0; i < lines.size(); i++)
		{
			const float x = gx + gw + 5;
			const float y = lines[i].first - (th / 2);
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->TextOut(IROUND(x), IROUND(y), lines[i].second);
			#else
				glTextOut(0, IROUND(x), IROUND(y), lines[i].second);
			#endif
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawSmithChart(const int graph, const int graph_type, const bool admittance, const float max_level)
{
	String s;
	uint32_t colour;
	uint8_t red;
	uint8_t grn;
	uint8_t blu;

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].x;
	const int gw = m_graph_pos[graph].w;
	const int gy = m_graph_pos[graph].y;
	const int gh = m_graph_pos[graph].h;
	const int cx = m_graph_pos[graph].cx;
	const int cy = m_graph_pos[graph].cy;
	const int gr = m_graph_pos[graph].cr;

	if (gw <= 10 || gh <= 10 || gr <= 10)
		return;

	const int mx = m_mouse.x;
	const int my = m_mouse.y;

	Graphics::TBitmap *bitmap = m_grid[graph].bitmap;
	if (bitmap == NULL)
		return;

	const bool redraw = (   bitmap->Width  != gw
								|| bitmap->Height != gh
								|| m_grid[graph].graph_type   != graph_type
								|| m_grid[graph].grid_colour  != settings.m_colours.grid
								|| m_grid[graph].back_colour  != settings.m_colours.background
								|| m_grid[graph].vswr2_colour != settings.m_colours.vswr2_line
								|| m_grid[graph].both_scales  != settings.smithBothScales
								#ifdef USE_OPENGL
									//|| m_grid[graph].gl_texture == 0
								#endif
								) ? true : false;

	const int alpha = GRID_ALPHA / 1.5;

	#ifndef USE_OPENGL_2
		if (redraw)
		{
			m_grid[graph].graph_type   = graph_type;
			m_grid[graph].grid_colour  = settings.m_colours.grid;
			m_grid[graph].back_colour  = settings.m_colours.background;
			m_grid[graph].vswr2_colour = settings.m_colours.vswr2_line;
			m_grid[graph].both_scales  = settings.smithBothScales;

			bitmap->Width  = gw;
			bitmap->Height = gh;
		}

		Gdiplus::Graphics gdi_plus(bitmap->Canvas->Handle);

		if (redraw)
		{
			// background
			bitmap->Canvas->Brush->Color = settings.m_colours.background;
			bitmap->Canvas->Brush->Style = bsSolid;
			bitmap->Canvas->FillRect(TRect(0, 0, bitmap->Width, bitmap->Height));

			bitmap->Canvas->Pen->Style   = psSolid;
			//bitmap->Canvas->Brush->Color = settings.m_colours.background;
			bitmap->Canvas->Brush->Style = bsClear;

			//gdi_plus.SetSmoothingMode(Gdiplus::SmoothingModeNone);
			//gdi_plus.SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
			//gdi_plus.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			gdi_plus.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

			gdi_plus.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			//gdi_plus.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

			//gdi_plus.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			//gdi_plus.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			gdi_plus.SetCompositingQuality(Gdiplus::CompositingQualityGammaCorrected);
			//gdi_plus.SetCompositingQuality(Gdiplus::CompositingQualityAssumeLinear);

			//gdi_plus.SetPixelOffsetMode(Gdiplus::PixelOffsetModeInvalid);
			//gdi_plus.SetPixelOffsetMode(Gdiplus::PixelOffsetModeDefault);
			//gdi_plus.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighSpeed);
			//gdi_plus.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
			gdi_plus.SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);
			//gdi_plus.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		}

		colourToRGB(settings.m_colours.grid, red, grn, blu);

		Gdiplus::Pen grid_pen1(Gdiplus::Color(alpha, red, grn, blu), 1);
		grid_pen1.SetAlignment(Gdiplus::PenAlignmentCenter);
		grid_pen1.SetDashStyle(Gdiplus::DashStyleSolid);
		//grid_pen1.SetDashStyle(Gdiplus::DashStyleDash);
		//grid_pen1.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));

		Gdiplus::Pen grid_pen2(Gdiplus::Color(alpha / 2, red, grn, blu), 1);
		grid_pen2.SetAlignment(Gdiplus::PenAlignmentCenter);
		//grid_pen2.SetDashStyle(Gdiplus::DashStyleSolid);
		//grid_pen2.SetDashStyle(Gdiplus::DashStyleDash);
		grid_pen2.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));

		colourToRGB(settings.m_colours.vswr2_line, red, grn, blu);
		Gdiplus::Pen vswr2_pen(Gdiplus::Color(alpha, red, grn, blu), 1);
		vswr2_pen.SetAlignment(Gdiplus::PenAlignmentCenter);
		//vswr2_pen.SetDashStyle(Gdiplus::DashStyleSolid);
		//vswr2_pen.SetDashStyle(Gdiplus::DashStyleDash);
		vswr2_pen.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));

	#else

	#endif

	const int num_segments = 16;

	const int num_intermediates = settings.smithBothScales ? 0 : 2;
	const float circle_r[] = {0.25, 0.5, 0.75, 1.0, 1.0 / 0.75, 1.0 / 0.5, 1.0 / 0.25};

	struct {
		float x;
		float impedance;
	} constant_resistance_circles[12];

	{
		ZeroMemory(&constant_resistance_circles, sizeof(constant_resistance_circles));
		const int num_points = ARRAY_SIZE(constant_resistance_circles);
		const float ref_imp = 50.0f;	// center of smith
		for (int k = 0; k < num_points; k++)
		{
			// left (-1.0) to right (+1.0) across the smith chart
			const float x = -1.0f + (2.0f * k) / num_points;
			if (x >= -1.0f && x < 1.0f)
			{
				const float d = (1.0f - x) * (1.0f - x);
				constant_resistance_circles[k].x = x;
				constant_resistance_circles[k].impedance = (d != 0.0f) ? (ref_imp * (1.0f + x) * (1.0f - x)) / d : 0.0f;
			}
		}
	}

	if (redraw)
	{
		#ifdef USE_OPENGL_2
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			//glLineStipple(1, 0xcccc);
			glLineStipple(1, 0xaaaa);
			//glEnable(GL_LINE_STIPPLE);
		#endif

		if (!admittance || settings.smithBothScales)
		{	// smith chart constant resistance circles
			#ifdef USE_OPENGL_2
				colourToRGB(settings.m_colours.grid, red, grn, blu);
			#endif
			for (int k = 0; k < (int)ARRAY_SIZE(constant_resistance_circles); k++)
			{
				#ifndef USE_OPENGL_2
					const int xx = (cx - gx) + IROUND((gr + (gr * constant_resistance_circles[k].x)) / 2);
					const int yy = (cy - gy);
					const int rr = ((cx - gx) + gr) - xx;
					if (k & 1)
					{
						if (!settings.smithBothScales)
							gdi_plus.DrawEllipse(&grid_pen2, xx - rr, yy - rr, rr * 2, rr * 2);	// faint circle
					}
					else
						gdi_plus.DrawEllipse(&grid_pen1, xx - rr, yy - rr, rr * 2, rr * 2);	// main circle
				#else
					const int xx = cx + IROUND((gr + (gr * constant_resistance_circles[k].x)) / 2);
					const int yy = cy;
					const int rr = (cx + gr) - xx;
					glColor4ub(red, grn, blu, (k & 1) ? alpha / 2 : alpha);
					glCircle(xx, yy, rr, 0, 360, num_segments * (rr / 8), false);
				#endif
			}
		}

		if (admittance || settings.smithBothScales)
		{	// admittance chart constant conductance circles
			#ifdef USE_OPENGL_2
				colourToRGB(settings.m_colours.grid, red, grn, blu);
			#endif
			for (int k = 0; k < (int)ARRAY_SIZE(constant_resistance_circles); k++)
			{
				#ifndef USE_OPENGL_2
					const int xx = (cx - gx) - IROUND((gr + (gr * constant_resistance_circles[k].x)) / 2);
					const int yy = (cy - gy);
					const int rr = xx - ((cx - gx) - gr);
					if (k & 1)
					{
						if (!settings.smithBothScales)
							gdi_plus.DrawEllipse(&grid_pen2, xx - rr, yy - rr, rr * 2, rr * 2);	// faint circle
					}
					else
						gdi_plus.DrawEllipse(&grid_pen1, xx - rr, yy - rr, rr * 2, rr * 2);	// main circle
				#else
					const int xx = cx - IROUND((gr + (gr * constant_resistance_circles[k].x)) / 2);
					const int yy = cy;
					const int rr = xx - (cx - gr);
					glColor4ub(red, grn, blu, (k & 1) ? alpha / 2 : alpha);
					glCircle(xx, yy, rr, 0, 360, num_segments * (rr / 8), false);
				#endif
			}
		}

		#ifdef USE_OPENGL_2
			glDisable(GL_LINE_STIPPLE);
		#endif

		#ifndef USE_OPENGL_2
			{	// stop any drawing outside of the smith circle
				Gdiplus::GraphicsPath path;
				path.AddEllipse((cx - gx) - gr, (cy - gy) - gr, gr * 2, gr * 2);
				Gdiplus::Region region(&path);
				//gdi_plus.DrawPath(&grid_pen1, &path);
				gdi_plus.SetClip(&region);
			}
		#else
			// the stencil pattern will mask off areas we don't want to draw in, so outside the smith circle

			glEnable(GL_STENCIL_TEST);

			GLint stencil_bits;
			glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
			GLint max_stencil = (1u << stencil_bits) - 1;

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_FALSE);	// disable drawing to depth buffer
			glStencilFunc(GL_NEVER, 1, ~0);
			//glStencilFunc(GL_NEVER, max_stencil, ~0);
			glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

			// draw stencil pattern
			glStencilMask(0xFF);			// enable writing to the stencil buffer
			//glClear(GL_STENCIL_BUFFER_BIT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				glColor3ub(255, 255, 255);
				glDisable(GL_LINE_SMOOTH);
				//glEnable(GL_LINE_SMOOTH);
				glLineWidth(1.0f);
				glCircle(cx, cy, 1 + gr, 0, 360, num_segments * (gr / 8), true);
				glDisable(GL_LINE_SMOOTH);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			//glDepthMask(GL_TRUE);
			glStencilMask(0);				// disable writing to the stencil buffer
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glStencilFunc(GL_EQUAL, 1, ~0);
			//glStencilFunc(GL_EQUAL, max_stencil, ~0);

		#endif

		#ifdef USE_OPENGL_2
			//glEnable(GL_LINE_STIPPLE);
		#endif

		if (!admittance || settings.smithBothScales)
		{	// reactance circles
			#ifdef USE_OPENGL_2
				colourToRGB(settings.m_colours.grid, red, grn, blu);
			#endif
			for (unsigned int i = 0; i < ARRAY_SIZE(circle_r); i++)
			{
				const float r0 = (i < 1) ? 0.0f : circle_r[i - 1];
				const float r1 = circle_r[i];
				if (r1 > 0.0f)
				{
					const int radius = IROUND(gr * (1.0f / r1));
					for (int k = 1; k <= num_intermediates; k++)
					{
						const float r2 = r0 + (((r1 - r0) * k) / (num_intermediates + 1));
						if (r2 > 0.0f)
						{
							#ifndef USE_OPENGL_2
								const int x = (cx - gx) + gr;
								const int r = IROUND(gr * (1.0f / r2));
								gdi_plus.DrawEllipse(&grid_pen2, x - r, (cy - gy) - (r * 2), r * 2, r * 2);	// upper circle
								gdi_plus.DrawEllipse(&grid_pen2, x - r, (cy - gy) + (r * 0), r * 2, r * 2);	// lower circle
							#else
								const int x = cx + gr;
								const int r = IROUND(gr * (1.0f / r2));
								glColor4ub(red, grn, blu, alpha / 2);
								glCircle(x, cy - r, r, 0, 360, num_segments * (r / 8), false);	// upper circle
								glCircle(x, cy + r, r, 0, 360, num_segments * (r / 8), false);	// lower circle
							#endif
						}
					}

					{
						#ifndef USE_OPENGL_2
							const int x = (cx - gx) + gr;
							gdi_plus.DrawEllipse(&grid_pen1, x - radius, (cy - gy) - (radius * 2), radius * 2, radius * 2);	// upper circle
							gdi_plus.DrawEllipse(&grid_pen1, x - radius, (cy - gy) + (radius * 0), radius * 2, radius * 2);	// lower circle
						#else
							const int x = cx + gr;
							glColor4ub(red, grn, blu, alpha);
							glCircle(x, cy - radius, radius, 0, 360, num_segments * (radius / 8), false);	// upper circle
							glCircle(x, cy + radius, radius, 0, 360, num_segments * (radius / 8), false);	// lower circle
						#endif
					}

				}
			}
		}

		if (admittance || settings.smithBothScales)
		{	// susceptance circles
			#ifdef USE_OPENGL_2
				colourToRGB(settings.m_colours.grid, red, grn, blu);
			#endif
			for (unsigned int i = 0; i < ARRAY_SIZE(circle_r); i++)
			{
				const float r0 = (i < 1) ? 0.0f : circle_r[i - 1];
				const float r1 = circle_r[i];
				if (r1 > 0.0f)
				{
					const int radius = IROUND(gr * (1.0f / r1));
					for (int k = 1; k <= num_intermediates; k++)
					{
						const float r2 = r0 + (((r1 - r0) * k) / (num_intermediates + 1));
						if (r2 > 0.0f)
						{
							#ifndef USE_OPENGL_2
								const int x = (cx - gx) - gr;
								const int r = IROUND(gr * (1.0f / r2));
								gdi_plus.DrawEllipse(&grid_pen2, x - r, (cy - gy) - (r * 2), r * 2, r * 2);	// upper circle
								gdi_plus.DrawEllipse(&grid_pen2, x - r, (cy - gy) + (r * 0), r * 2, r * 2);	// lower circle
							#else
								const int x = cx - gr;
								const int r = IROUND(gr * (1.0f / r2));
								glColor4ub(red, grn, blu, alpha / 2);
								glCircle(x, cy - r, r, 0, 360, num_segments * (r / 8), false);	// upper circle
								glCircle(x, cy + r, r, 0, 360, num_segments * (r / 8), false);	// lower circle
							#endif
						}
					}
					{
						#ifndef USE_OPENGL_2
							const int x = (cx - gx) - gr;
							gdi_plus.DrawEllipse(&grid_pen1, x - radius, (cy - gy) - (radius * 2), radius * 2, radius * 2);	// upper circle
							gdi_plus.DrawEllipse(&grid_pen1, x - radius, (cy - gy) + (radius * 0), radius * 2, radius * 2);	// lower circle
						#else
							const int x = cx - gr;
							glColor4ub(red, grn, blu, alpha);
							glCircle(x, cy - radius, radius, 0, 360, num_segments * (radius / 8), false);	// upper circle
							glCircle(x, cy + radius, radius, 0, 360, num_segments * (radius / 8), false);	// lower circle
						#endif
					}
				}
			}
		}

		#ifndef USE_OPENGL_2
			gdi_plus.ResetClip();
		#else
			glDisable(GL_STENCIL_TEST);
		#endif

		{	// draw the vswr2 center red circle
			const int r = gr / 3;
			#ifndef USE_OPENGL_2
				gdi_plus.DrawEllipse(&vswr2_pen, (cx - gx) - r, (cy - gy) - r, r * 2, r * 2);
			#else
				colourToRGB(settings.m_colours.vswr2_line, red, grn, blu);
				glColor4ub(red, grn, blu, alpha);
				glCircle(cx, cy, r, 0, 360, num_segments * (r / 8), false);
			#endif
		}

		if (!admittance)
		{  // draw the 45 deg lines
			const int x = IROUND(cosf(45.0f * deg_2_rad) * gr);
			const int y = IROUND(sinf(45.0f * deg_2_rad) * gr);
			#ifndef USE_OPENGL_2
				gdi_plus.DrawLine(&grid_pen1, cx - gx, cy - gy, (cx - gx) + x, (cy - gy) - y);
				gdi_plus.DrawLine(&grid_pen1, cx - gx, cy - gy, (cx - gx) + x, (cy - gy) + y);
			#else
				colourToRGB(settings.m_colours.grid, red, grn, blu);
				glColor4ub(red, grn, blu, alpha);
				glBegin(GL_LINES);
					glVertex2i(cx, cy); glVertex2i(cx + x, cy - y);
					glVertex2i(cx, cy); glVertex2i(cx + x, cy + y);
				glEnd();
			#endif
		}
		/*
		{
			const int r = gr / 2;
			#ifndef USE_OPENGL_2
				// draw unity conductance circle .. left hand circle
				gdi_plus.DrawEllipse(&grid_pen2, (cx - gx) - (r * 2), (cy - gy) - r, r * 2, r * 2);
				// draw unity resistance circle .. right hand circle
				gdi_plus.DrawEllipse(&grid_pen2, cx - gx, (cy - gy) - r, r * 2, r * 2);
			#else
				colourToRGB(settings.m_colours.grid, red, grn, blu);
				glColor4ub(red, grn, blu, alpha);
				glCircle(cx - r, cy, r, 0, 360, num_segments * (r / 8), false);	// draw unity conductance circle .. left hand circle
				glCircle(cx + r, cy, r, 0, 360, num_segments * (r / 8), false);	// draw unity resistance circle ... right hand circle
			#endif
		}
		*/

		// center horizontal line
		#ifndef USE_OPENGL_2
			gdi_plus.DrawLine(&grid_pen1, (cx - gx) - gr, cy - gy, (cx - gx) + gr, cy - gy);
		#else
			colourToRGB(settings.m_colours.grid, red, grn, blu);
			glColor4ub(red, grn, blu, alpha);
			glDisable(GL_LINE_SMOOTH);
			glBegin(GL_LINES);
				glVertex2i(cx - gr, cy);
				glVertex2i(cx + gr, cy);
			glEnd();
		#endif

		#ifdef USE_OPENGL_2
			glDisable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
		#endif

		#ifdef USE_OPENGL
			if (m_grid[graph].bitmap_texture != 0)
				glDeleteTextures(1, &m_grid[graph].bitmap_texture);
			m_grid[graph].bitmap_texture = 0;
/*
			if (m_grid[graph].bitmap_pbo != 0)
				glDeleteBuffers(1, &m_grid[graph].bitmap_pbo);
			m_grid[graph].bitmap_pbo = 0;
*/
//			GLuint fbo = 0;
//			glGenFramebuffers(1, &fbo);

			#if 0

				int image_size = bitmap->Width * bitmap->Height;
				if (image_size > 0)
				{
					if (bitmap->PixelFormat == pf24bit) image_size *= 3;
					else
					if (bitmap->PixelFormat == pf32bit) image_size *= 4;
					glGenBuffers(1, &m_grid[graph].bitmap_pbo);
					if (m_grid[graph].bitmap_pbo != 0)
					{
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);	// unbind
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_grid[graph].bitmap_pbo);
						glBufferData(GL_PIXEL_UNPACK_BUFFER, image_size, NULL, GL_STREAM_DRAW);
						void *mapped_buffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
						if (mapped_buffer)
						{	// write data into the mapped buffer, possibly in another thread.
							memcpy(mapped_buffer, bitmap->ScanLine[bitmap->Height - 1], image_size);
							glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
						}
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);	// unbind
					}
				}
			#else
				#if 0
					//glEnable(GL_POLYGON_SMOOTH);
					//glShadeModel(GL_SMOOTH);
					glDisable(GL_POLYGON_SMOOTH);
					glShadeModel(GL_FLAT);
					glDisable(GL_BLEND);
					m_grid[graph].bitmap_texture = createTexture(bitmap, false);
					glEnable(GL_BLEND);
					glDisable(GL_POLYGON_SMOOTH);
					glShadeModel(GL_FLAT);
				#endif
			#endif
		#endif
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->CopyMode = cmSrcCopy;
		m_graph_bm->Canvas->Draw(gx, gy, bitmap);
	#else
		glDisable(GL_BLEND);
/*
		if (m_grid[graph].bitmap_pbo != 0)
		{
			glEnable(GL_POLYGON_SMOOTH);
			glShadeModel(GL_SMOOTH);
			//glDisable(GL_POLYGON_SMOOTH);
			//glShadeModel(GL_FLAT);
			glColor3ub(255, 255, 255);
			glBindTexture(GL_TEXTURE_2D, m_grid[graph].bitmap_pbo);
			glBegin(GL_QUADS);
				glTexCoord2i(0, 1);	glVertex2i(gx, gy);
				glTexCoord2i(1, 1);	glVertex2i(gx + gw, gy);
				glTexCoord2i(1, 0);	glVertex2i(gx + gw, gy + gh);
				glTexCoord2i(0, 0);	glVertex2i(gx, gy + gh);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_POLYGON_SMOOTH);
			glShadeModel(GL_FLAT);
		}
		else
*/		if (m_grid[graph].bitmap_texture != 0)
		{
			//glEnable(GL_POLYGON_SMOOTH);
			//glShadeModel(GL_SMOOTH);
			glDisable(GL_POLYGON_SMOOTH);
			glShadeModel(GL_FLAT);
			glDisable(GL_BLEND);
			glColor3ub(255, 255, 255);
			renderTexture(TRect(gx, gy, gx + gw, gy + gh), m_grid[graph].bitmap_texture);
			glEnable(GL_BLEND);
			//glDisable(GL_POLYGON_SMOOTH);
			//glShadeModel(GL_FLAT);
		}
		else
		{	// slower DrawPixels method
			GLint saved_rp[2];	// x and y
			GLboolean valid_rp = FALSE;
			glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &valid_rp);
			if (valid_rp)
				glGetIntegerv(GL_CURRENT_RASTER_POSITION, saved_rp);
			glRasterPos2i(gx, gy + gh);
			glDisable(GL_BLEND);
			//glShadeModel(GL_SMOOTH);
			glShadeModel(GL_FLAT);
			glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
			//glDrawPixels(bitmap->Width, bitmap->Height, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->ScanLine[bitmap->Height - 1]);
			glDrawPixels(bitmap->Width, bitmap->Height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap->ScanLine[bitmap->Height - 1]);
			glEnable(GL_BLEND);
			if (valid_rp)
				glRasterPos2i(saved_rp[0], saved_rp[1]);
		}

		glEnable(GL_BLEND);
	#endif

	// ********************
	// overlay some text

	#ifndef USE_OPENGL
		TFont *font                      = m_graph_bm->Canvas->Font;
		if (settings.graphFont != NULL)
			m_graph_bm->Canvas->Font      = settings.graphFont;

		m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
		m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		m_graph_bm->Canvas->Brush->Style = bsClear;

		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		GLrgba font_col = colourToGLcolour(settings.m_colours.font);
		glColor3ub(font_col.r, font_col.g, font_col.b);

		const int th = m_gl.font[0].height;
	#endif

	{
		int yt = gy + 5;
		if (yt < (cy - gr - 5 - th))
			yt = cy - gr - 5 - th;
		int yb = gy + gh - 5 - th;
		if (yb > (cy + gr + 5))
			yb = cy + gr + 5;

		if (!admittance)
		{
			{
				s = "Short";
				#ifndef USE_OPENGL
					const int tw = m_graph_bm->Canvas->TextWidth(s);
				#else
					const int tw = glTextWidth(0, s);
				#endif
				int x = cx - gr - 5 - tw;
				int y = cy - (th / 2);

				if (x < gx)
				{
					x = gx;
					y = cy + 5;
				}
				else
				if (x > (gx + gw - 5 - tw))
				{
					x = gx + gw - 5 - tw;
					y = cy + 5;
				}
				if (y < yt) y = yt;
				else
				if (y > yb) y = yb;
				if (x <= (cx - gr - tw) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
				{	// only draw the text if the mouse is far enough away
					#ifndef USE_OPENGL
						m_graph_bm->Canvas->TextOut(x, y, s);
					#else
						glTextOut(0, x, y, s);
					#endif
				}
			}

			const int num = ARRAY_SIZE(constant_resistance_circles);
			for (int k = 1; k < num; k++)
			{
				if ((k & 1) == 0)
				{
					const float imp = constant_resistance_circles[k].impedance;
					s.printf(L"%0.2f", imp);
					s = common.trimTrailingZeros(s);
					#ifndef USE_OPENGL
						const int tw = m_graph_bm->Canvas->TextWidth(s);
					#else
						const int tw = glTextWidth(0, s);
					#endif
					int x = cx + IROUND(gr * constant_resistance_circles[k].x) - (tw / 2);
					int y = cy + 5;

					if (x < gx)
					{
						x = gx;
						//y = cy + 5;
					}
					else
					if (x > (gx + gw - 5 - tw))
					{
						x = gx + gw - 5 - tw;
						//y = cy + 5;
					}
					if (y < yt) y = yt;
					else
					if (y > yb) y = yb;
					if (mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
					{	// only draw the text if the mouse is far enough away
						#ifndef USE_OPENGL
							m_graph_bm->Canvas->TextOut(x, y, s);
						#else
							glTextOut(0, x, y, s);
						#endif
					}
				}
			}

			{
				s = "Open";
				#ifndef USE_OPENGL
					const int tw = m_graph_bm->Canvas->TextWidth(s);
				#else
					const int tw = glTextWidth(0, s);
				#endif
				int x = cx + gr + 5;
				int y = cy - (th / 2);

				if (x < gx)
				{
					x = gx;
					y = cy + 5;
				}
				else
				if (x > (gx + gw - 5 - tw))
				{
					x = gx + gw - 5 - tw;
					y = cy + 5;
				}
				if (y < yt) y = yt;
				else
				if (y > yb) y = yb;
				if (x >= (cx + gr) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
				{	// only draw the text if the mouse is far enough away
					#ifndef USE_OPENGL
						m_graph_bm->Canvas->TextOut(x, y, s);
					#else
						glTextOut(0, x, y, s);
					#endif
				}
			}
		}
		else
		{
			const int num = ARRAY_SIZE(constant_resistance_circles);
			for (int k = 0; k < num; k++)
			{
				if ((k & 1) == 0)
				{
					const float imp = constant_resistance_circles[k].impedance;
					s.printf(L"%0.2f", imp);
					s = common.trimTrailingZeros(s);
					#ifndef USE_OPENGL
						const int tw = m_graph_bm->Canvas->TextWidth(s);
					#else
						const int tw = glTextWidth(0, s);
					#endif
					int x = cx - IROUND(gr * constant_resistance_circles[k].x) - (tw / 2);
					int y = cy + 5;

					if (x < gx)
					{
						x = gx;
						//y = cy + 5;
					}
					else
					if (x > (gx + gw - 5 - tw))
					{
						x = gx + gw - 5 - tw;
						//y = cy + 5;
					}
					if (y < yt) y = yt;
					else
					if (y > yb) y = yb;
					if (mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
					{	// only draw the text if the mouse is far enough away
						#ifndef USE_OPENGL
							m_graph_bm->Canvas->TextOut(x, y, s);
						#else
							glTextOut(0, x, y, s);
						#endif
					}
				}
			}
		}

		{
			s = "Inductive";
			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth(s);
			#else
				const int tw = glTextWidth(0, s);
			#endif
			int x = cx - (tw / 2);
			int y = yt;

			if (x < gx)
			{
				x = gx;
				//y = cy + 5;
			}
			else
			if (x > (gx + gw - 5 - tw))
			{
				x = gx + gw - 5 - tw;
				//y = cy + 5;
			}
			if (y < yt) y = yt;
			else
			if (y > yb) y = yb;
			if (y <= (cy - gr - th) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
			{	// only draw the text if the mouse is far enough away
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->TextOut(x, y, s);
				#else
					glTextOut(0, x, y, s);
				#endif
			}
		}
		{
			s = "Capacitive";
			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth(s);
			#else
				const int tw = glTextWidth(0, s);
			#endif
			int x = cx - (tw / 2);
			int y = yb;

			if (x < gx)
			{
				x = gx;
				//y = cy + 5;
			}
			else
			if (x > (gx + gw - 5 - tw))
			{
				x = gx + gw - 5 - tw;
				//y = cy + 5;
			}
			if (y < yt) y = yt;
			else
			if (y > yb) y = yb;
			if (y >= (cy + gr) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
			{	// only draw the text if the mouse is far enough away
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->TextOut(x, y, s);
				#else
					glTextOut(0, x, y, s);
				#endif
			}
		}
	}

	// ********************

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawPolarChart(const int graph, const int graph_type, const float max_level)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].x;
	const int gy = m_graph_pos[graph].y;
	const int gw = m_graph_pos[graph].w;
	const int gh = m_graph_pos[graph].h;
	const int cx = m_graph_pos[graph].cx;
	const int cy = m_graph_pos[graph].cy;
	const int gr = m_graph_pos[graph].cr;

	if (gw <= 10 || gh <= 10 || gr <= 10)
		return;

	const int mx = m_mouse.x;
	const int my = m_mouse.y;
/*
	Graphics::TBitmap *bitmap = m_grid[graph].bitmap;
	if (bitmap == NULL)
		return;

	const bool redraw = (   bitmap->Width  != gw
								|| bitmap->Height != gh
								|| m_grid[graph].graph_type   != graph_type
								|| m_grid[graph].grid_colour  != settings.m_colours.grid
								|| m_grid[graph].back_colour  != settings.m_colours.background
								|| m_grid[graph].vswr2_colour != settings.m_colours.vswr2_line
								#ifdef USE_OPENGL
									//|| m_grid[graph].gl_texture == 0
								#endif
								) ? true : false;
*/
	const int alpha = GRID_ALPHA;

	#ifndef USE_OPENGL
		const Gdiplus::SmoothingMode sm = m_gdi_plus->GetSmoothingMode();
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);		// fast
		m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);		// high quality
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
	#endif

	uint32_t colour;
	uint8_t red;
	uint8_t grn;
	uint8_t blu;

	colourToRGB(settings.m_colours.grid, red, grn, blu);

	#ifndef USE_OPENGL
		Gdiplus::Pen grid_pen1(Gdiplus::Color(alpha, red, grn, blu), 1);
		grid_pen1.SetAlignment(Gdiplus::PenAlignmentCenter);
		grid_pen1.SetDashStyle(Gdiplus::DashStyleSolid);
		//grid_pen1.SetDashStyle(Gdiplus::DashStyleDash);
		//grid_pen1.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));

		Gdiplus::Pen grid_pen2(Gdiplus::Color(alpha / 2, red, grn, blu), 1);
		grid_pen2.SetAlignment(Gdiplus::PenAlignmentCenter);
		grid_pen2.SetDashStyle(Gdiplus::DashStyleSolid);
		//grid_pen2.SetDashStyle(Gdiplus::DashStyleDash);
		//grid_pen2.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));

		colourToRGB(settings.m_colours.vswr2_line, red, grn, blu);
		Gdiplus::Pen vswr2_pen(Gdiplus::Color(alpha, red, grn, blu), 1);
		vswr2_pen.SetAlignment(Gdiplus::PenAlignmentCenter);
		//vswr2_pen.SetDashStyle(Gdiplus::DashStyleSolid);
//		vswr2_pen.SetDashStyle(Gdiplus::DashStyleDash);
		vswr2_pen.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));
	#else
	#endif

	const int num_lines = 360 / (45.0f / 4);

	std::vector < std::pair <double, String> > scale;
	if (max_level > 0.0f)
	{
		String s = "0.00001";
		#ifndef USE_OPENGL
			const int tw = m_graph_bm->Canvas->TextWidth(s);
		#else
			const int tw = glTextWidth(0, s);
		#endif
		const int min_str_spacing = (tw / 2) + 5;

		const double x_scale = (double)gr / max_level;

		int mult_index   = 0;
		double step_size = 0.000001;
		step_size = computeGraticuleScale(min_str_spacing, x_scale, step_size, mult_index);

		double level = 0.0;
		while (level <= max_level)
		{
			const double r = level * x_scale;
			if (level >= 0.0 && level <= max_level && r >= 0 && r <= gr)
			{
				s.printf(L"%#.6f", level);
				if (s.Pos(common.decimalPoint()) > 0)
				{	// remove trailing zero's and unneeded decimal points
					while (!s.IsEmpty() && s[s.Length()] == '0')
						s = s.SubString(1, s.Length() - 1).Trim();
					if (!s.IsEmpty() && s[s.Length()] == common.decimalPoint())
					s = s.SubString(1, s.Length() - 1).Trim();
				}
				scale.push_back(std::make_pair(r, s));
			}
			level += step_size;
		}
	}

	{
		#ifdef USE_OPENGL
			//glDisable(GL_LINE_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			//glLineStipple(1, 0xcccc);
			//glEnable(GL_LINE_STIPPLE);
			colourToRGB(settings.m_colours.grid, red, grn, blu);
			glBegin(GL_LINES);
		#endif
		for (int i = 0; i <= num_lines; i++)
		{
			const int x = IROUND(cos((2 * M_PI * i) / num_lines) * gr);
			const int y = IROUND(sin((2 * M_PI * i) / num_lines) * gr);
			#ifndef USE_OPENGL
				if ((i & 1) == 0)
					m_gdi_plus->DrawLine(&grid_pen1, cx, cy, cx + x, cy - y);
				else
					m_gdi_plus->DrawLine(&grid_pen2, cx, cy, cx + x, cy - y); // fainter intermediate circle
			#else
				glColor4ub(red, grn, blu, (i & 1) ? alpha / 2 : alpha);
				glVertex2i(cx, cy);
				glVertex2i(cx + x, cy - y);
			#endif
		}
		#ifdef USE_OPENGL
			glEnd();
			//glDisable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
		#endif
	}

	{	// circles
		#ifdef USE_OPENGL
			colourToRGB(settings.m_colours.grid, red, grn, blu);
			//glDisable(GL_LINE_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
		#endif

		for (unsigned int i = 1; i < scale.size(); i++)
		{
			const int r = IROUND(scale[i].first);
			#ifndef USE_OPENGL
				if ((i & 1) == 0)
					m_gdi_plus->DrawEllipse(&grid_pen1, cx - r, cy - r, r * 2, r * 2);
				else
					m_gdi_plus->DrawEllipse(&grid_pen2, cx - r, cy - r, r * 2, r * 2); // fainter intermediate circle
			#else
				glColor4ub(red, grn, blu, (i & 1) ? alpha / 2 : alpha);
				glCircle(cx, cy, r, 0, 360, num_lines * (r / 8), false);
			#endif
		}

		{	// outter circle
			const int r = gr;
			#ifndef USE_OPENGL
				m_gdi_plus->DrawEllipse(&grid_pen1, cx - r, cy - r, r * 2, r * 2);
			#else
				glColor4ub(red, grn, blu, alpha);
				glCircle(cx, cy, r, 0, 360, num_lines * (r / 8), false);
			#endif
		}

		#ifdef USE_OPENGL
			glEnd();
			//glDisable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
		#endif
	}

/*
	{	// draw the vswr2 center red circle
		const int r = gr / 3;
		#ifndef USE_OPENGL
			m_gdi_plus->DrawEllipse(&vswr2_pen, cx - r, cy - r, r * 2, r * 2);
		#else
			colourToRGB(settings.m_colours.vswr2_line, red, grn, blu);
			glColor4ub(red, grn, blu, alpha);
			//glDisable(GL_LINE_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			glLineStipple(1, 0xcccc);
			glEnable(GL_LINE_STIPPLE);
			glCircle(cx, cy, r, 0, 360, num_lines * 4, false);
			glDisable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
		#endif
	}
*/
	{	// draw some scale text
		String s;

		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font->Color = settings.m_colours.font;
			m_graph_bm->Canvas->Brush->Style = bsClear;
		#else
			GLrgba font_col = colourToGLcolour(settings.m_colours.font);
			glColor3ub(font_col.r, font_col.g, font_col.b);
		#endif

		int yt = gy + 5;
		int yb = gy + gh - 5 - th;

		//for (float angle = 0.0f; angle < 360.0f; angle += 45.0f / 2)
		for (float angle = 0.0f; angle < 360.0f; angle += 45.0f)
		{
			int x = cx + IROUND(cosf(angle * deg_2_rad) * gr);
			int y = cy - IROUND(sinf(angle * deg_2_rad) * gr);

			s.printf(L"%0.1f", angle);
			s = common.trimTrailingZeros(s) + "\xb0";

			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth(s);
			#else
				const int tw = glTextWidth(0, s);
			#endif

			if (angle == 90.0f || angle == 270.0f)
				x -= tw / 2;
			else
			if (angle > 90.0f && angle < 270.0f)
				x -= tw + 5;
			else
				x += 5;

			if (angle == 90.0f)
				y -= th + 5;
			else
			if (angle == 270.0f)
				y += 5;
			else
			if (angle == 0.0f || angle == 180.0f)
				y -= th / 2;
			else
			if (angle > 0.0f && angle < 180.0f)
				y -= (th / 2) + 5;
			else
				y = y - (th / 2) + 5;

			if (x < gx)
				x = gx;
			else
			if (x > (gx + gw - 5 - tw))
				x = gx + gw - 5 - tw;
			if (y < yt) y = yt;
			else
			if (y > yb) y = yb;
			if (x >= (cx + gr) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
			{	// only draw the text if the mouse is far enough away
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->TextOut(x, y, s);
				#else
					glTextOut(0, x, y, s);
				#endif
			}
		}

		// circle scale
		for (unsigned int i = 1; i < scale.size(); i++)
		{
			const int r = IROUND(scale[i].first);
			String s = scale[i].second;

			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth(s);
				const int y = (i & 1) ? cy + 5 : cy - 5 - th;
				const int x = cx + r - (tw / 2);
			#else
				const int tw = glTextWidth(0, s);
				const int y = (i & 1) ? cy + 5 : cy - 5 - th;
				const int x = cx + r - (tw / 2);
			#endif
			if (y >= (cy + gr) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
			{	// only draw the text if the mouse is far enough away
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->TextOut(x, y, s);
				#else
					glTextOut(0, x, y, s);
				#endif
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsSolid;
		m_graph_bm->Canvas->Font = font;
		m_gdi_plus->SetSmoothingMode(sm);
	#endif
}

void __fastcall CGraphs::drawPhaseVectorChart(const int graph, const int graph_type)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].x;
	const int gy = m_graph_pos[graph].y;
	const int gw = m_graph_pos[graph].w;
	const int gh = m_graph_pos[graph].h;
	const int cx = m_graph_pos[graph].cx;
	const int cy = m_graph_pos[graph].cy;
	const int gr = m_graph_pos[graph].cr;

	if (gw <= 10 || gh <= 10 || gr <= 10)
		return;

	const int mx = m_mouse.x;
	const int my = m_mouse.y;

	const int alpha = GRID_ALPHA;

	#ifndef USE_OPENGL
		const Gdiplus::SmoothingMode sm = m_gdi_plus->GetSmoothingMode();
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);		// fast
		m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);		// high quality
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
	#endif

	uint32_t colour;
	uint8_t red;
	uint8_t grn;
	uint8_t blu;

	colourToRGB(settings.m_colours.grid, red, grn, blu);

	#ifndef USE_OPENGL
		Gdiplus::Pen grid_pen1(Gdiplus::Color(alpha, red, grn, blu), 1);
		grid_pen1.SetAlignment(Gdiplus::PenAlignmentCenter);
		grid_pen1.SetDashStyle(Gdiplus::DashStyleSolid);
		//grid_pen1.SetDashStyle(Gdiplus::DashStyleDash);
		grid_pen1.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));

		Gdiplus::Pen grid_pen2(Gdiplus::Color(alpha / 2, red, grn, blu), 1);
		grid_pen2.SetAlignment(Gdiplus::PenAlignmentCenter);
		//grid_pen2.SetDashStyle(Gdiplus::DashStyleSolid);
		//grid_pen2.SetDashStyle(Gdiplus::DashStyleDash);
		grid_pen2.SetDashPattern(grid_line_pattern1, ARRAY_SIZE(grid_line_pattern1));
	#else
	#endif

	const int num_circles = 12;
	const int num_lines   = 24;

	{
		#ifdef USE_OPENGL
			//glDisable(GL_LINE_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
			//glLineStipple(1, 0xcccc);
			//glEnable(GL_LINE_STIPPLE);
			colourToRGB(settings.m_colours.grid, red, grn, blu);
			glBegin(GL_LINES);
		#endif
		for (int i = 0; i <= num_lines; i++)
		{
			const int x = IROUND(cos((2 * M_PI * i) / num_lines) * gr);
			const int y = IROUND(sin((2 * M_PI * i) / num_lines) * gr);
			#ifndef USE_OPENGL
				if ((i & 1) == 0)
					m_gdi_plus->DrawLine(&grid_pen1, cx, cy, cx + x, cy - y);
				else
					m_gdi_plus->DrawLine(&grid_pen2, cx, cy, cx + x, cy - y); // fainter intermediate circle
			#else
				glColor4ub(red, grn, blu, (i & 1) ? alpha / 2 : alpha);
				glVertex2i(cx, cy);
				glVertex2i(cx + x, cy - y);
			#endif
		}
		#ifdef USE_OPENGL
			glEnd();
			//glDisable(GL_LINE_STIPPLE);
			glDisable(GL_LINE_SMOOTH);
		#endif
	}

	{
		for (int i = 1; i <= num_circles; i++)
		{
			const int r = (gr * i) / num_circles;
			#ifndef USE_OPENGL
				if ((i & 1) == 0)
					m_gdi_plus->DrawEllipse(&grid_pen1, cx - r, cy - r, r * 2, r * 2);
				else
					m_gdi_plus->DrawEllipse(&grid_pen2, cx - r, cy - r, r * 2, r * 2); // fainter intermediate circle
			#else
				colourToRGB(settings.m_colours.grid, red, grn, blu);
				glColor4ub(red, grn, blu, (i & 1) ? alpha / 2 : alpha);
				//glDisable(GL_LINE_SMOOTH);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(1.0f);
				glCircle(cx, cy, r, 0, 360, num_lines * (r / 8), false);
				glDisable(GL_LINE_SMOOTH);
			#endif
		}
	}

	{	// draw some scale text
		String s;

		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font->Color = settings.m_colours.font;
			m_graph_bm->Canvas->Brush->Style = bsClear;
		#else
			GLrgba font_col = colourToGLcolour(settings.m_colours.font);
			glColor3ub(font_col.r, font_col.g, font_col.b);
		#endif

		int yt = gy + 5;
		if (yt < (cy - gr - 5 - th))
			yt = cy - gr - 5 - th;
		int yb = gy + gh - 5 - th;
		if (yb > (cy + gr + 5))
			yb = cy + gr + 5;

		{
			s = "0\xb0";
			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth(s);
			#else
				const int tw = glTextWidth(0, s);
			#endif
			int y = cy - (th / 2);
			int x = cx + gr + 5;
			if (x > (gx + gw - 5 - tw))
			{
				x = gx + gw - 5 - tw;
				y = cy + 5;
			}

			if (y < yt) y = yt;
			else
			if (y > yb) y = yb;
			if (x >= (cx + gr) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
			{	// only draw the text if the mouse is far enough away
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->TextOut(x, y, s);
				#else
					glTextOut(0, x, y, s);
				#endif
			}
		}

		{
			s = "180\xb0";
			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth(s);
			#else
				const int tw = glTextWidth(0, s);
			#endif
			int y = cy - (th / 2);
			int x = cx - gr - 5 - tw;
			if (x < gx)
			{
				x = gx;
				y = cy + 5;
			}

			if (y < yt) y = yt;
			else
			if (y > yb) y = yb;
			if (x <= (cx - gr - tw) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
			{	// only draw the text if the mouse is far enough away
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->TextOut(x, y, s);
				#else
					glTextOut(0, x, y, s);
				#endif
			}
		}

		{
			s = "90\xb0";
			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth(s);
			#else
				const int tw = glTextWidth(0, s);
			#endif
			const int x = cx - (tw / 2);
			int y = yt;

			if (y < yt) y = yt;
			else
			if (y > yb) y = yb;
			if (y <= (cy - gr - th) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
			{	// only draw the text if the mouse is far enough away
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->TextOut(x, y, s);
				#else
					glTextOut(0, x, y, s);
				#endif
			}
		}

		{
			s = "270\xb0";
			#ifndef USE_OPENGL
				const int tw = m_graph_bm->Canvas->TextWidth(s);
			#else
				const int tw = glTextWidth(0, s);
			#endif
			const int x = cx - (tw / 2);
			int y = yb;

			if (y < yt) y = yt;
			else
			if (y > yb) y = yb;
			if (y >= (cy + gr) || mx <= (x - 5) || mx >= (x + tw + 5) || my <= (y - 5) || my >= (y + th + 5))
			{	// only draw the text if the mouse is far enough away
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->TextOut(x, y, s);
				#else
					glTextOut(0, x, y, s);
				#endif
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Brush->Style = bsSolid;
		m_graph_bm->Canvas->Font = font;
		m_gdi_plus->SetSmoothingMode(sm);
	#endif
}

void __fastcall CGraphs::drawLCMatching(const int graph, const int graph_type, const int mem)
{
	// do some S11 L/C matching

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (mem < 0 || mem >= MAX_MEMORIES)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;
	//const int cx = m_graph_pos[graph].cx;
	//const int cy = m_graph_pos[graph].cy;
	const int gr = m_graph_pos[graph].cr;

	if (gw <= 10 || gh <= 10 || gr <= 10)
		return;

	if (!Form1)
		return;

	if (!settings.lcMatchingEnable)
		return;

	const int size = data_unit.freqArraySize(mem);
	if (size <= 0)
		return;

	if (!Form1)
		return;

//	const bool freq_mode  = isFrequencyGraph(graph_type);
	const bool smith_mode = isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);
//	const bool tdr_mode   = isTDRGraph(graph_type);

	const complexf cpx = m_mouse.cpx;
	const float mag = m_mouse.mag;

	const float ref_impedance = 50;

	int index = -1;

	if (m_mouse.point_index >= 0)
	{
		index = m_mouse.point_index;
	}
	else
	if ((!smith_mode || mag < 0 || mag > 1) && !settings.m_markers_freq.empty())
	{	// find the closest spot sweep frequency to the markers frequency

		index = Form1->selectedMarker();
		index = data_unit.indexFreq(settings.m_markers_freq[index].Hz, mem);
	}

	double Hz = 0;
	complexf c;

	if (index >= 0 && index < size)
	{
		Hz = data_unit.m_point_filt[mem][index].Hz;
		c  = data_unit.m_point_filt[mem][index].s11;
	}
	else
	if (mag >= 0 && mag <= 1)
	{	// use the free moving mouse position and the sweep cw frequency
		//Hz = data_unit.m_freq_center_Hz;
		Hz = data_unit.m_freq_cw_Hz;
		c  = cpx;
	}

	if (Hz <= 0)
		return;

	const complexf imp = data_unit.impedance(c, ref_impedance);

	String s;

	t_lc_match lc_matches[4];

//	const uint32_t fHz = CFG_GetParam(CFG_PARAM_MEAS_F);

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
		m_graph_bm->Canvas->Brush->Style = bsClear;
		m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
	#else
		const int th = m_gl.font[0].height;
		GLrgba font_col = colourToGLcolour(settings.m_colours.font);
		glColor3ub(font_col.r, font_col.g, font_col.b);
	#endif

	const int x = smith_mode ? gx - 40 : gx + 20;
	int y = gy + 10;

	s = " LC Match ";
	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
		m_graph_bm->Canvas->TextOut(x, y, s);
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // make unbold
	#else
		glTextOut(1, x, y, s);
	#endif

	s.printf(L" Source %#.3f ", ref_impedance);
	y += th;
	#ifndef USE_OPENGL
		m_graph_bm->Canvas->TextOut(x, y, s);
	#else
		glTextOut(0, x, y, s);
	#endif

	s.printf(L" Load   %#.4f %cj%#.4f ", imp.real(), (imp.imag() < 0) ? '-' : '+', fabsf(imp.imag()));
	y += th;
	#ifndef USE_OPENGL
		m_graph_bm->Canvas->TextOut(x, y, s);
	#else
		glTextOut(0, x, y, s);
	#endif

	s =       " Freq   " + common.freqToStr1(Hz, true, true, 6, false) + "Hz";
	if (Hz == data_unit.m_freq_cw_Hz)
      s += " CW";
	y += th;
	#ifndef USE_OPENGL
		m_graph_bm->Canvas->TextOut(x, y, s);
	#else
		glTextOut(0, x, y, s);
	#endif

//	const int matches = LCMatch::calc(c, lc_matches, ref_impedance);
	const int matches = LCMatch::calc(imp, lc_matches, ref_impedance);
	if (matches < 0)
	{
		s = " No LC match for this load ";
		y += th;
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
			m_graph_bm->Canvas->TextOut(x, y, s);
			m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // make unbold
		#else
			glTextOut(1, x, y, s);
		#endif
	}
	else
	if (matches == 0)
	{
		s = " No LC match needed ";
		y += th;
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
			m_graph_bm->Canvas->TextOut(x, y, s);
			m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // make unbold
		#else
			glTextOut(1, x, y, s);
		#endif
	}
	else
	{
		s = "  src shunt     series     load shunt ";
		y += th;
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
			m_graph_bm->Canvas->TextOut(x, y, s);
			m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // make unbold
		#else
			glTextOut(1, x, y, s);
		#endif

		for (int i = 0; i < matches; i++)
		{
			AnsiString s;
			char str[3][12];

			LCMatch::x_str(Hz, lc_matches[i].xpl, str[0]);
			LCMatch::x_str(Hz, lc_matches[i].xps, str[1]);
			LCMatch::x_str(Hz, lc_matches[i].xs,  str[2]);

			s.printf(" %s  %s  %s ", str[1], str[2], str[0]);
			y += th;
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->TextOut(x, y, s);
			#else
				glTextOut(0, x, y, s);
			#endif
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawPoints(const int graph, const int graph_type, std::vector <t_pointf> &points_f, bool draw_spline, int line_width, bool show_points, TColor line_colour)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	int gx = m_graph_pos[graph].gx;
	int gy = m_graph_pos[graph].gy;
	int gw = m_graph_pos[graph].gw;
	int gh = m_graph_pos[graph].gh;

	int gxw = gx + gw;
	int gyh = gy + gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (points_f.size() < 4)	// spline needs at least 4 points to work
		return;

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Lock();

		const Gdiplus::SmoothingMode sm      = m_gdi_plus->GetSmoothingMode();
		const Gdiplus::CompositingMode cm    = m_gdi_plus->GetCompositingMode();
		const Gdiplus::CompositingQuality cq = m_gdi_plus->GetCompositingQuality();

		//m_gdi_plus->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
		m_gdi_plus->SetCompositingMode(Gdiplus::CompositingModeSourceOver);

		if (settings.lineAlpha >= 255)
		{	// no transparency
			//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityGammaCorrected);
			//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityAssumeLinear);
		}
			else
		{
			//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityGammaCorrected);
			//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityAssumeLinear);
		}

		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	#else
	#endif

//	m_gdi_plus->SetClip(Gdiplus::Rect(gx - (line_width / 2), gy - (line_width / 2), gw + line_width, gh + line_width));

//	m_graph_bm->Canvas->Pen->Style = psSolid;
//	m_graph_bm->Canvas->Pen->Width = line_width;
//	m_graph_bm->Canvas->Pen->Color = line_colour;

//	const bool freq_mode  = isFrequencyGraph(graph_type);
	const bool smith_mode = isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);
//	const bool tdr_mode   = isTDRGraph(graph_type);

	const bool clip_trace = settings.clipTraces || smith_mode;

	if (smith_mode)
	{	// use the full graph area as the clipping rectangle
		gx = m_graph_pos[graph].x;
		gy = m_graph_pos[graph].y;
		gw = m_graph_pos[graph].w;
		gh = m_graph_pos[graph].h;

		gxw = gx + gw - 1;
		gyh = gy + gh - 1;
	}

	const int gxc  = gx  - (gw * 5);
	const int gxwc = gxw + (gw * 5);
	const int gyc  = gy  - (gh * 5);
	const int gyhc = gyh + (gh * 5);

	m_gdi_points.resize(0);

	if (draw_spline)
	{
		#if 1
			const int num_spline_points = points_f.size() * 8;	// add 8 more interpolated points between each pair of control points
			const float step = 1.0f / (num_spline_points - 1);

			CRSpline2 spline;

			// add the spline control points
			for (unsigned int i = 0; i < points_f.size(); i++)
			{
				float x = points_f[i].x;
				float y = points_f[i].y;
				if (clip_trace)
				{	// don't leave the graph drawing area
					if (x <  gx) x = gx;
					else
					if (x > gxw) x = gxw;
					if (y <  gy) y = gy;
					else
					if (y > gyh) y = gyh;
				}
				else
				{
					if (x <  gxc) x = gxc;
					else
					if (x > gxwc) x = gxwc;
					if (y <  gyc) y = gyc;
					else
					if (y > gyhc) y = gyhc;
				}
				spline.addPoint(x, y);
			}

			// now create the trace line within spline points
			for (int i = 0; i < num_spline_points; i++)
			{
				const vec2 v = spline.interpolatedPoint(step * i);
				float x = v.x;
				float y = v.y;
				if (clip_trace)
				{	// don't leave the graph drawing area
					if (x <  gx) x = gx;
					else
					if (x > gxw) x = gxw;
					if (y <  gy) y = gy;
					else
					if (y > gyh) y = gyh;
				}
				else
				{
					if (x <  gxc) x = gxc;
					else
					if (x > gxwc) x = gxwc;
					if (y <  gyc) y = gyc;
					else
					if (y > gyhc) y = gyhc;
				}
				Gdiplus::PointF point(x, y);
				m_gdi_points.push_back(point);
			}
		#else
			const int num_spline_points = points_f.size() * 8;	// add 8 more interpolated points between each pair of control points
			MonotCubicInterpolator mci;
			for (unsigned int i = 0; i < points_f.size(); i++)
			{
				float x = points_f[i].x;
				float y = points_f[i].y;
				if (clip_trace)
				{	// don't leave the graph drawing area
					if (x <  gx) x = gx;
					else
					if (x > gxw) x = gxw;
					if (y <  gy) y = gy;
					else
					if (y > gyh) y = gyh;
				}
				else
				{
					if (x <  gxc) x = gxc;
					else
					if (x > gxwc) x = gxwc;
					if (y <  gyc) y = gyc;
					else
					if (y > gyhc) y = gyhc;
				}
				mci.addPair(x, y, (i < ((int)points_f.size() - 1)) ? false : true);
			}
			const float step = (points_f[points_f.size() - 1].x - points_f[0].x) / (num_spline_points - 1);
			for (int i = 0; i < num_spline_points; i++)
			{
				float x = points_f[0].x + (step * i);	// this needs fixing for the smith chart
				float y = mci.evaluate(x);
				if (clip_trace)
				{	// don't leave the graph drawing area
					if (x <  gx) x = gx;
					else
					if (x > gxw) x = gxw;
					if (y <  gy) y = gy;
					else
					if (y > gyh) y = gyh;
				}
				else
				{
					if (x <  gxc) x = gxc;
					else
					if (x > gxwc) x = gxwc;
					if (y <  gyc) y = gyc;
					else
					if (y > gyhc) y = gyhc;
				}
				Gdiplus::PointF point(x, y);
				m_gdi_points.push_back(point);
			}
		#endif
	}
	else
	{	// draw non-spline curve
		for (unsigned int i = 0; i < points_f.size(); i++)
		{
			float x = points_f[i].x;
			float y = points_f[i].y;
			if (clip_trace)
			{	// don't leave the graph drawing area
				if (x <  gx) x = gx;
				else
				if (x > gxw) x = gxw;
				if (y <  gy) y = gy;
				else
				if (y > gyh) y = gyh;
			}
			else
			{
				if (x <  gxc) x = gxc;
				else
				if (x > gxwc) x = gxwc;
				if (y <  gyc) y = gyc;
				else
				if (y > gyhc) y = gyhc;
			}
			Gdiplus::PointF point(x, y);
			m_gdi_points.push_back(point);
		}
	}

	if (m_gdi_points.size() > 1)
	{
		uint8_t red, grn, blu;
		colourToRGB(line_colour, red, grn, blu);
		const int alpha = settings.lineAlpha;

		#ifndef USE_OPENGL
			Gdiplus::Pen pen(Gdiplus::Color(alpha, red, grn, blu), line_width);
			pen.SetAlignment(Gdiplus::PenAlignmentCenter);
			pen.SetDashStyle(Gdiplus::DashStyleSolid);
			m_gdi_plus->DrawLines(&pen, &m_gdi_points[0], m_gdi_points.size());
		#else
			glColor4ub(red, grn, blu, alpha);
			//glDisable(GL_LINE_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(line_width);
			glBegin(GL_LINE_STRIP);
				glVertex2f(m_gdi_points[0].X, m_gdi_points[0].Y);
				for (unsigned int i = 1; i < m_gdi_points.size(); i++)
					glVertex2f(m_gdi_points[i].X, m_gdi_points[i].Y);
			glEnd();
			glDisable(GL_LINE_SMOOTH);
		#endif
	}

	if (show_points)
	{	// plot the graph points
		int point_size = line_width;
		if (point_size < 2) point_size = 2;
		else
		if (point_size > 3) point_size = 3;
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Pen->Width   = 1;
			m_graph_bm->Canvas->Pen->Color   = settings.m_colours.point;
			m_graph_bm->Canvas->Brush->Color = (point_size <= 2) ? settings.m_colours.point : settings.m_colours.background;
		#else
			GLrgba col = colourToGLcolour(settings.m_colours.point);
			glColor3ub(col.r, col.g, col.b);
			//glDisable(GL_LINE_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.0f);
		#endif
		for (unsigned int i = 0; i < points_f.size(); i++)
		{
			const int x = IROUND(points_f[i].x);
			const int y = IROUND(points_f[i].y);
			if (x >= gx && x <= gx + gw && y >= gy && y <= gy + gh)
			{
				#ifndef USE_OPENGL
					m_graph_bm->Canvas->Ellipse(x - point_size, y - point_size, x + point_size, y + point_size);
				#else
					glCircle(x, y, point_size, 0, 360, 6, true);
				#endif
			}
		}
		#ifdef USE_OPENGL
			glDisable(GL_LINE_SMOOTH);
		#endif
	}

	// restore
	#ifndef USE_OPENGL
		//	m_gdi_plus->ResetClip();
		m_gdi_plus->SetCompositingQuality(cq);
		m_gdi_plus->SetCompositingMode(cm);
		m_gdi_plus->SetSmoothingMode(sm);

		m_graph_bm->Canvas->Unlock();
	#else

	#endif
}

void __fastcall CGraphs::drawGraphLines(const int graph, const int graph_type)
{
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int x  = m_graph_pos[graph].x;
	const int y  = m_graph_pos[graph].y;
	const int w  = m_graph_pos[graph].w;
	const int h  = m_graph_pos[graph].h;
	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;
//	const int cx = m_graph_pos[graph].cx;
//	const int cy = m_graph_pos[graph].cy;
//	const int gr = m_graph_pos[graph].cr;

//	if (gw <= 10 || gh <= 10 || gr <= 10)
	if (gw <= 10 || gh <= 10)
		return;

	const int line_width = settings.lineWidth;

//	const bool freq_mode  = isFrequencyGraph(graph_type);
	const bool smith_mode = isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);
//	const bool tdr_mode   = isTDRGraph(graph_type);

	#ifndef USE_OPENGL
		Gdiplus::Status status = Gdiplus::GenericError;
		//HRGN clip_region = NULL;
		if (m_gdi_plus)
		{
			if (!smith_mode)
				status = m_gdi_plus->SetClip(Gdiplus::Rect(gx - (line_width / 2), gy - (line_width / 2), gw + line_width, gh + line_width));
			else
				status = m_gdi_plus->SetClip(Gdiplus::Rect(x - (line_width / 2), y - (line_width / 2), w + line_width, h + line_width));
		}
		//	if (!smith_mode)
		//		clip_region = ::CreateRectRgn(gx - 1, gy - 1, gx + gw + 1, gy + gh + 1);
		//	else
		//		clip_region = ::CreateRectRgn(x - 1, y - 1, x + w + 1, y + h + 1);
		//	if (clip_region)
		//		::SelectClipRgn(m_graph_bm->Canvas->Handle, clip_region);
	#else
		//const int view_width  = m_gl.win_control->ClientWidth;
		const int view_height = m_gl.win_control->ClientHeight;
		GLint scissor_box[4];
		glGetIntegerv(GL_SCISSOR_BOX, scissor_box);
//		GLboolean SCISSOR_was_enabled = glIsEnabled(GL_SCISSOR_TEST);
		if (!smith_mode)
			glScissor(gx - (line_width / 2), view_height - (gy + gh - (line_width / 2)), gw + line_width, gh + line_width);
		else
			glScissor( x - (line_width / 2), view_height - ( y +  h - (line_width / 2)),  w + line_width,  h + line_width);
		glEnable(GL_SCISSOR_TEST);

	#endif

	if (graph_type == GRAPH_TYPE_CAL_LOGMAG)
	{
		const int mem = 0;
		for (int trace = 0; trace < MAX_CAL_TRACES; trace++)
		{
			if (m_line_points[graph][mem][trace].size() >= 3)
			{
				bool draw_spline = settings.splineEnable;
				if (!smith_mode && draw_spline)
				{
					const int x_dist = ABS(m_line_points[graph][mem][trace][1].x - m_line_points[graph][mem][trace][0].x);
					draw_spline = (x_dist >= 3) ? true : false;
				}
				drawPoints(graph, graph_type, m_line_points[graph][mem][trace], draw_spline, line_width, settings.showPoints, settings.m_colours.line[mem][trace]);
			}
		}
	}
	else
	{
		for (int mem = MAX_MEMORIES - 1; mem >= 0; mem--)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (m_line_points[graph][mem][channel].size() >= 3)
				{
					bool draw_spline = settings.splineEnable;
					if (!smith_mode && draw_spline)
					{
						const int x_dist = ABS(m_line_points[graph][mem][channel][1].x - m_line_points[graph][mem][channel][0].x);
						draw_spline = (x_dist >= 3) ? true : false;
					}
					drawPoints(graph, graph_type, m_line_points[graph][mem][channel], draw_spline, line_width, settings.showPoints, settings.m_colours.line[mem][channel]);
				}
			}
		}
	}

	#ifndef USE_OPENGL
		if (m_gdi_plus && status == Gdiplus::Ok)
			m_gdi_plus->ResetClip();
		//	if (clip_region != NULL)
		//	{
		//		::DeleteObject(clip_region);
		//		::SelectClipRgn(m_graph_bm->Canvas->Handle, NULL);
		//	}
	#else
		glScissor(scissor_box[0], scissor_box[1], scissor_box[2], scissor_box[3]);
//		if (!SCISSOR_was_enabled)
			glDisable(GL_SCISSOR_TEST);
	#endif
}

void __fastcall CGraphs::drawDetails(const int graph, const int graph_type, String title)
{
	if (graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (!m_graph_bm)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	//const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

//	const int64_t Hz = m_mouse.Hz;
//	int mem          = m_mouse.point_mem;
//	const int index  = m_mouse.point_index;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	title = title.Trim();

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style >> fsBold;     // remove bold
		m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
		m_graph_bm->Canvas->Brush->Style = bsClear;
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
	#endif

	// show the auto-max/auto-min indicators
	if (gs && graph_type != GRAPH_TYPE_CAL_LOGMAG)
	{	// auto min/max triangle indicators on the top and bottom graph lines
		TColor line_colour = settings.m_colours.font;
		TColor fill_colour = settings.m_colours.font;

		const int size = 4;
		const int x = gx + 1 + ((1 + size) / 2);

		if (gs->auto_max)
		{
			#ifndef USE_OPENGL
				TPoint points[4];
				points[0].x = x - size; points[0].y = gy - 2;
				points[1].x = x;        points[1].y = gy - 2 - (size * 2);
				points[2].x = x;        points[2].y = gy - 2 - (size * 2);
				points[3].x = x + size; points[3].y = gy - 2;
				m_graph_bm->Canvas->Pen->Style   = psSolid;
				m_graph_bm->Canvas->Pen->Width   = 1;
				m_graph_bm->Canvas->Pen->Color   = line_colour;
				m_graph_bm->Canvas->Brush->Color = fill_colour;
				m_graph_bm->Canvas->Polygon(&points[0], ARRAY_SIZE(points) - 1);
			#else
				GLrgba ocol = colourToGLcolour(line_colour);
				GLrgba fcol = colourToGLcolour(fill_colour);

				// inside
				glShadeModel(GL_FLAT);
				//glShadeModel(GL_SMOOTH);
				glColor3ub(fcol.r, fcol.g, fcol.b);
				glBegin(GL_TRIANGLES);
					glVertex2i(x - size, gy - 2);
					glVertex2i(x + size, gy - 2);
					glVertex2i(x,        gy - (size * 2));
				glEnd();

				// outline
				//glDisable(GL_LINE_SMOOTH);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(1);
				glColor3ub(ocol.r, ocol.g, ocol.b);
				glBegin(GL_LINE_LOOP);
					glVertex2i(x - size, gy - 2);
					glVertex2i(x + size, gy - 2);
					glVertex2i(x,        gy - (size * 2));
				glEnd();
				glDisable(GL_LINE_SMOOTH);

			#endif
		}

		if (gs->auto_min)
		{
			#ifndef USE_OPENGL
				TPoint points[4];
				points[0].x = x - size; points[0].y = gy + gh + 0;
				points[1].x = x;        points[1].y = gy + gh + 0 + (size * 2);
				points[2].x = x;        points[2].y = gy + gh + 0 + (size * 2);
				points[3].x = x + size; points[3].y = gy + gh + 0;
				m_graph_bm->Canvas->Pen->Style   = psSolid;
				m_graph_bm->Canvas->Pen->Width   = 1;
				m_graph_bm->Canvas->Pen->Color   = line_colour;
				m_graph_bm->Canvas->Brush->Color = fill_colour;
				m_graph_bm->Canvas->Polygon(&points[0], ARRAY_SIZE(points) - 1);
			#else
				GLrgba ocol = colourToGLcolour(line_colour);
				GLrgba fcol = colourToGLcolour(fill_colour);

				// inside
				glShadeModel(GL_FLAT);
				//glShadeModel(GL_SMOOTH);
				glColor3ub(fcol.r, fcol.g, fcol.b);
				glBegin(GL_TRIANGLES);
					glVertex2i(x - size, gy + gh + 1);
					glVertex2i(x + size, gy + gh + 1);
					glVertex2i(x,        gy + gh + (size * 2));
				glEnd();

				// outline
				//glDisable(GL_LINE_SMOOTH);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(1);
				glColor3ub(ocol.r, ocol.g, ocol.b);
				glBegin(GL_LINE_LOOP);
					glVertex2i(x - size, gy + gh + 1);
					glVertex2i(x + size, gy + gh + 1);
					glVertex2i(x,        gy + gh + (size * 2));
				glEnd();
				glDisable(GL_LINE_SMOOTH);
			#endif
		}

		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Brush->Style = bsClear;
		#endif
	}

	int x = m_graph_pos[graph].x + 5;
//	int y = gy - 5 - (int)(th * 1.5f);
	int y = gy - 5 - (th * 2);

	{	// graph title
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style << fsBold;     // make bold
			m_graph_bm->Canvas->TextOut(x, y, title);
			//x += m_graph_bm->Canvas->TextWidth(title);
			m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style >> fsBold;     // remove bold
		#else
			GLrgba font_col = colourToGLcolour(settings.m_colours.font);
			glColor3ub(font_col.r, font_col.g, font_col.b);
			glTextOut(1, x, y, title);
			//x += glTextWidth(1, title);
		#endif
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawDetails(const int graph, const int graph_type, const uint8_t chan_mask, const int mem, const int index, String title, String units, String s[], String s_value[], String min_max_fmt)
{
	if (graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (!m_graph_bm)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	//const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	const int64_t Hz = m_mouse.Hz;
//	int mem          = m_mouse.point_mem;
//	const int index  = m_mouse.point_index;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	title = title.Trim();

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style >> fsBold;     // remove bold
		m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
		m_graph_bm->Canvas->Brush->Style = bsClear;
		const int th = m_graph_bm->Canvas->TextHeight("Hq|");
	#else
		const int th = m_gl.font[0].height;
		GLrgba font_col = colourToGLcolour(settings.m_colours.font);
	#endif

	// show the auto-max/auto-min indicators
	if (gs && graph_type != GRAPH_TYPE_CAL_LOGMAG)
	{	// auto min/max triangle indicators on the top and bottom graph lines
		TColor line_colour = settings.m_colours.font;
		TColor fill_colour = settings.m_colours.font;

		const int size = 4;
		const int x = gx + 1 + ((1 + size) / 2);

		if (gs->auto_max)
		{
			#ifndef USE_OPENGL
				TPoint points[4];
				points[0].x = x - size; points[0].y = gy - 2;
				points[1].x = x;        points[1].y = gy - 2 - (size * 2);
				points[2].x = x;        points[2].y = gy - 2 - (size * 2);
				points[3].x = x + size; points[3].y = gy - 2;
				m_graph_bm->Canvas->Pen->Style   = psSolid;
				m_graph_bm->Canvas->Pen->Width   = 1;
				m_graph_bm->Canvas->Pen->Color   = line_colour;
				m_graph_bm->Canvas->Brush->Color = fill_colour;
				m_graph_bm->Canvas->Polygon(&points[0], ARRAY_SIZE(points) - 1);
			#else
				GLrgba ocol = colourToGLcolour(line_colour);
				GLrgba fcol = colourToGLcolour(fill_colour);

				// inside
				glShadeModel(GL_FLAT);
				//glShadeModel(GL_SMOOTH);
				glColor3ub(fcol.r, fcol.g, fcol.b);
				glBegin(GL_TRIANGLES);
					glVertex2i(x - size, gy - 2);
					glVertex2i(x + size, gy - 2);
					glVertex2i(x,        gy - (size * 2));
				glEnd();

				// outline
				//glDisable(GL_LINE_SMOOTH);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(1);
				glColor3ub(ocol.r, ocol.g, ocol.b);
				glBegin(GL_LINE_LOOP);
					glVertex2i(x - size, gy - 2);
					glVertex2i(x + size, gy - 2);
					glVertex2i(x,        gy - (size * 2));
				glEnd();
				glDisable(GL_LINE_SMOOTH);
			#endif
		}

		if (gs->auto_min)
		{
			#ifndef USE_OPENGL
				TPoint points[4];
				points[0].x = x - size; points[0].y = gy + gh + 0;
				points[1].x = x;        points[1].y = gy + gh + 0 + (size * 2);
				points[2].x = x;        points[2].y = gy + gh + 0 + (size * 2);
				points[3].x = x + size; points[3].y = gy + gh + 0;
				m_graph_bm->Canvas->Pen->Style   = psSolid;
				m_graph_bm->Canvas->Pen->Width   = 1;
				m_graph_bm->Canvas->Pen->Color   = line_colour;
				m_graph_bm->Canvas->Brush->Color = fill_colour;
				m_graph_bm->Canvas->Polygon(&points[0], ARRAY_SIZE(points) - 1);
			#else
				GLrgba ocol = colourToGLcolour(line_colour);
				GLrgba fcol = colourToGLcolour(fill_colour);

				// inside
				glShadeModel(GL_FLAT);
				//glShadeModel(GL_SMOOTH);
				glColor3ub(fcol.r, fcol.g, fcol.b);
				glBegin(GL_TRIANGLES);
					glVertex2i(x - size, gy + gh + 1);
					glVertex2i(x + size, gy + gh + 1);
					glVertex2i(x,        gy + gh + (size * 2));
				glEnd();

				// outline
				//glDisable(GL_LINE_SMOOTH);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(1);
				glColor3ub(ocol.r, ocol.g, ocol.b);
				glBegin(GL_LINE_LOOP);
					glVertex2i(x - size, gy + gh + 1);
					glVertex2i(x + size, gy + gh + 1);
					glVertex2i(x,        gy + gh + (size * 2));
				glEnd();
				glDisable(GL_LINE_SMOOTH);
			#endif
		}

		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Brush->Style = bsClear;
		#endif
	}

	int x = m_graph_pos[graph].x + 5;
	int y = gy - 5 - (th * 2);

	// graph title
	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;     // make bold
		m_graph_bm->Canvas->TextOut(x, y, title);
		x += m_graph_bm->Canvas->TextWidth(title);
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;     // remove bold
	#else
		glColor3ub(font_col.r, font_col.g, font_col.b);
		glTextOut(1, x, y, title);
		x += glTextWidth(1, title);
	#endif

	if (gs && gs->show_min_max && graph_type != GRAPH_TYPE_CAL_LOGMAG)
	{
		if (m_max[graph].index >= 0 || m_min[graph].index >= 0)
		{
			//m_max[graph].channel
			//m_max[graph].index
			//m_max[graph].value

			//m_min[graph].channel
			//m_min[graph].index
			//m_min[graph].value

			String max_s = "max";
			String min_s = "min";

			min_max_fmt = min_max_fmt.Trim();
			if (min_max_fmt.IsEmpty())
				min_max_fmt = "max %#.8f";

			if (m_max[graph].index >= 0)
			{
				max_s.printf(min_max_fmt.c_str(), m_max[graph].value);
				max_s = common.trimTrailingZeros(max_s);	// remove any trailing decimal point
				max_s = "max " + max_s + " " + units;
				max_s = max_s.Trim();
			}
			if (m_min[graph].index >= 0)
			{
				min_s.printf(min_max_fmt.c_str(), m_min[graph].value);
				min_s = common.trimTrailingZeros(min_s);	// remove any trailing decimal point
				min_s = "min " + min_s + " " + units;
				min_s = min_s.Trim();
			}

			//max_s = (gs->auto_max) ? "auto " + max_s : "     " + max_s;
			//min_s = (gs->auto_min) ? "auto " + min_s : "     " + min_s;

			#ifndef USE_OPENGL
				const int max_tw = m_graph_bm->Canvas->TextWidth(max_s);
				const int min_tw = m_graph_bm->Canvas->TextWidth(min_s);
				x += m_graph_bm->Canvas->TextWidth("   ");
				m_graph_bm->Canvas->TextOut(x, y +  0, max_s);
				m_graph_bm->Canvas->TextOut(x, y + th, min_s);
				x += (max_tw > min_tw) ? max_tw : min_tw;
			#else
				glColor3ub(font_col.r, font_col.g, font_col.b);
				const int max_tw = glTextWidth(0, max_s);
				const int min_tw = glTextWidth(0, min_s);
				x += glTextWidth(0, "   ");
				glTextOut(0, x, y +  0, max_s);
				glTextOut(0, x, y + th, min_s);
				x += (max_tw > min_tw) ? max_tw : min_tw;
			#endif
		}
	}

	if (graph_type == GRAPH_TYPE_CAL_LOGMAG)
	{	// calibration graph

		// draw the text inside the graph area because all 5 lines won't fit where it normally goes
		x = gx + 20;
		y = gy + 20;

		if (mem >= 0)
		{
			String mouse_s[MAX_CAL_TRACES];

			if (Hz > 0)
			{
				for (int channel = 0; channel < MAX_CAL_TRACES; channel++)
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
						mouse_s[channel] = s[channel] + " " + s_value[channel];
			}

			for (int channel = 0; channel < MAX_CAL_TRACES; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					#ifndef USE_OPENGL
						m_graph_bm->Canvas->Font->Color = settings.m_colours.line[mem][channel];
						m_graph_bm->Canvas->TextOut(x, y, mouse_s[channel]);
					#else
						GLrgba font_col = colourToGLcolour(settings.m_colours.line[mem][channel]);
						glColor3ub(font_col.r, font_col.g, font_col.b);
						glTextOut(0, x, y, mouse_s[channel]);
					#endif
					y += th;
				}
			}
		}
	}
	else
	{	// memories graph
		#ifndef USE_OPENGL
			x += m_graph_bm->Canvas->TextWidth("   ");
		#else
			x += glTextWidth(0, "   ");
		#endif
//		if (chan_mask == 3)
//			y -= th / 2;

		if (mem >= 0)
		{
			String mouse_s[MAX_CHANNELS];

			if (settings.memoryEnable[mem])
			{
				if (Hz > 0)
				{
					for (int channel = 0; channel < MAX_CHANNELS; channel++)
					{
						if (index >= 0 && index < (int)m_levels[mem][channel].size())
						{
							if (mem <= 0)
								mouse_s[channel] = s[channel] + " live " + s_value[channel];
							else
								mouse_s[channel] = s[channel] + " mem-" + IntToStr(mem) + " " + s_value[channel];
						}
					}
				}
				else
				{
					for (int channel = 0; channel < MAX_CHANNELS; channel++)
					{
						if (mem <= 0)
							mouse_s[channel] = s[channel] + " live ";
						else
							mouse_s[channel] = s[channel] + " mem-" + IntToStr(mem);
					}
				}
			}

			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					#ifndef USE_OPENGL
						m_graph_bm->Canvas->Font->Color = settings.m_colours.line[mem][channel];
						m_graph_bm->Canvas->TextOut(x, y, mouse_s[channel]);
					#else
						GLrgba font_col = colourToGLcolour(settings.m_colours.line[mem][channel]);
						glColor3ub(font_col.r, font_col.g, font_col.b);
						glTextOut(0, x, y, mouse_s[channel]);
					#endif
					y += th;
				}
			}
		}
		else
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					int tx = x;
					for (int m = 0; m < MAX_MEMORIES; m++)
					{
						if (settings.memoryEnable[m])
						{
							String s2;
							s2.printf(L" mem-%d", m);
							s2 = s[channel] + s2;
							#ifndef USE_OPENGL
								m_graph_bm->Canvas->Font->Color = settings.m_colours.line[m][channel];
								m_graph_bm->Canvas->TextOut(tx, y, s2);
								tx += m_graph_bm->Canvas->TextWidth(s2 + " ");
							#else
								GLrgba font_col = colourToGLcolour(settings.m_colours.line[mem][channel]);
								glColor3ub(font_col.r, font_col.g, font_col.b);
								glTextOut(0, tx, y, s2);
								tx += glTextWidth(0, s2 + " ");
							#endif
						}
					}
					y += th;
				}
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif
}

void __fastcall CGraphs::drawLogMagCalibrations(const int graph, const int graph_type, const uint8_t trace_mask, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
//		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		for (int channel = 0; channel < MAX_CAL_TRACES; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	const int mem = 0;

	// ***************
	// compute the log mag

	const int size = calibration_module.m_calibration.point.size();
	if (size > 0)
	{
		for (int trace = 0; trace < MAX_CAL_TRACES; trace++)
		{
			if (trace_mask & (1u << trace))
			{
				m_levels[mem][trace].resize(size);
				for (int i = 0; i < size; i++)
				{
					const complexf sparam = calibration_module.m_calibration.point[i].sParam[trace];
					float level = data_unit.gain10(sparam);
					m_levels[mem][trace][i] = level;
				}
			}
		}
	}

	// ***************
	// curve smoothing

	const int median_level = settings.medianFilterLevel;
	const int smooth_level = settings.curveSmoothingLevel;

	for (int trace = 0; trace < MAX_CAL_TRACES; trace++)
	{
		median_filter.process(m_levels[mem][trace], 1 << (median_level - 1));   // variable width
		smoothing_filter.process(m_levels[mem][trace], 1 << (smooth_level - 1), 1, true);     // boxcar
//		smoothing_filter.process(m_levels[mem][trace], 1 << (smooth_level - 1), 1, false);    // variable iterations .. seems to produce nicer than variable width
		//smoothing_filter.process(m_levels[mem][trace], 1, 1 << (smooth_level - 1), false);  // variable width
	}

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeCalTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.4f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

		//const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float y_scale = (double)gh / range_levels;

		for (int trace = 0; trace < MAX_CAL_TRACES; trace++)
		{
			const int size = m_levels[mem][trace].size();
			if (size > 0)
			{
				m_line_points[graph][mem][trace].resize(size);
				for (int i = 0; i < size; i++)
				{
					const int64_t Hz  = calibration_module.m_calibration.point[i].HzCal;
					const float level = m_levels[mem][trace][i] - (float)min_levels;
					m_line_points[graph][mem][trace][i].x = freqToX(graph, graph_type, Hz);
					m_line_points[graph][mem][trace][i].y = (gy + gh) - (level * y_scale);
				}
		  }
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	String units = "dB";
	String s[MAX_CAL_TRACES] = {"Open ", "Short", "Load ", "Isoln", "Thru "};

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CAL_TRACES];

		int64_t Hz = m_mouse.Hz;
//		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem >= 0)
		{
			for (int trace = 0; trace < MAX_CAL_TRACES; trace++)
			{
				if (index >= 0 && index < (int)m_levels[mem][trace].size())
				{
					String s2;
					s2.printf(L"%#.4f", m_levels[mem][trace][index]);
					s2 = common.trimTrailingZeros(s2) + " " + units;
					s_value[trace] = s2.Trim();
				}
			}
		}

		String title = String("Freq Log Mag " + units).Trim();
		if (trace_mask & 0x01)
			title += " Open";
		if (trace_mask & 0x02)
			title += " Short";
		if (trace_mask & 0x04)
			title += " Load";
		if (trace_mask & 0x08)
			title += " Isoln";
		if (trace_mask & 0x10)
			title += " Thru";
		drawDetails(graph, graph_type, trace_mask, mem, index, title, units, s, s_value, "%#.4f");
	}
}

void __fastcall CGraphs::drawLogMagS11S21(const int graph, const int graph_type, const uint8_t chan_mask, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// compute the log mag

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					m_levels[mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						float level = data_unit.gain10(data_unit.m_point_filt[mem][i].sParam[channel]);
						if (normalise)
							level -= data_unit.gain10(data_unit.m_point_norm[i].sParam[channel]);
						m_levels[mem][channel][i] = level;
					}
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.4f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

		//const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float y_scale = (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][channel][i] - (float)min_levels;
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - (level * y_scale);
					}
			  }
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	// draw the markers
	if (gs && gs->show_markers)
	{
		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			for (int c = 0; c < MAX_CHANNELS; c++)
			{
				if (chan_mask & (1u << c))
				{
					drawMarkers(graph, m, c, min_levels, max_levels, " dB", 1.0f, draw_v_line);
					draw_v_line = false;
				}
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				String units      = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? " dB" : " dBm";
				String line_type1 = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 " : "line ";
				String line_type2 = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S21 " : "line ";
				switch (chan_mask & 3)
				{
					case 1: drawMarkersOnGraph(graph, m, 0, units, line_type1); break;
					case 2: drawMarkersOnGraph(graph, m, 1, units, line_type2); break;
					case 3: drawMarkersOnGraph(graph, m, 0, 1, units, line_type1, line_type2); break;
					default: break;
				}
				draw_on_graph = false;
			}
		}
	}

	String units = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "dB" : "dBm";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11" : "line";
	s[1] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S21" : "line";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						String s2;
						s2.printf(L"%#.4f", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = String("Freq Log Mag " + units).Trim();
		if (chan_mask & 1)
			title += " S11";
		if (chan_mask & 2)
			title += " S21";
		drawDetails(graph, graph_type, chan_mask, mem, index, title, units, s, s_value, "%#.4f");
	}
}

void __fastcall CGraphs::drawLinMagS11S21(const int graph, const int graph_type, const uint8_t chan_mask, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the lin mag

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					m_levels[mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						float level = data_unit.magnitude(data_unit.m_point_filt[mem][i].sParam[channel]);
						if (normalise)
							level -= data_unit.magnitude(data_unit.m_point_norm[i].sParam[channel]);
						m_levels[mem][channel][i] = level;
					}
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	const double gamma = settings.m_graph_setting[graph_type].gamma;

	if (gamma <= GAMMA_MIN)
		drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.6f", "");
	else
		drawNoneLinMagLines(graph, true, min_levels, max_levels, gamma, 0.00001, "%#.6f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale = (double)gh / range_levels;

		const double pow_x_scale = 1.0 / range_levels;
		const double pow_y = (gamma >= 1.0) ? 1.0 / gamma : 1.0;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz = data_unit.m_point_filt[mem][i].Hz;
						double level = (double)m_levels[mem][channel][i] - min_levels;
						if (gamma > GAMMA_MIN)
						{	// non-linear scale
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? min_levels + pow(level * pow_x_scale, pow_y) * range_levels : 0.0;
								// reverse
								//level = min_levels + (pow((level - min_levels) * pow_x_scale, gamma) * range_levels);
							#else
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - ((float)level * y_scale);
					}
			  }
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#endif

	// draw the markers
	if (gs && gs->show_markers)
	{
		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					drawMarkers(graph, m, channel, min_levels, max_levels, "", gamma, draw_v_line);
					draw_v_line = false;
				}
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				String line_type1 = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 " : "line ";
				String line_type2 = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S21 " : "line ";
				switch (chan_mask & 3)
				{
					case 1: drawMarkersOnGraph(graph, m, 0, "", line_type1); break;
					case 2: drawMarkersOnGraph(graph, m, 1, "", line_type2); break;
					case 3: drawMarkersOnGraph(graph, m, 0, 1, "", line_type1, line_type2); break;
					default: break;
				}
				draw_on_graph = false;
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif

	String units = "";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11" : "line";
	s[1] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S21" : "line";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, gamma, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, gamma, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, gamma);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						String s2;
						s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = String("Freq Linear Mag " + units).Trim();
		if (chan_mask & 1)
			title += " S11";
		if (chan_mask & 2)
			title += " S21";
		drawDetails(graph, graph_type, chan_mask, mem, index, title, units, s, s_value, "%#.8f");
	}
}

void __fastcall CGraphs::drawPhaseS11S21(const int graph, const int graph_type, const uint8_t chan_mask, const bool unwrap, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the phase

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					m_levels[mem][channel].resize(size);
					// compute the phase (in degrees)
					for (int i = 0; i < size; i++)
					{
						float level = data_unit.phase(data_unit.m_point_filt[mem][i].sParam[channel]);
						if (normalise)
							level -= data_unit.phase(data_unit.m_point_norm[i].sParam[channel]);
						m_levels[mem][channel][i] = level * rad_2_deg;
					}

					// phase unwrap (in degrees)
					if (unwrap)
						common.phaseUnwrapDeg(&m_levels[mem][channel][0], size);
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	drawMagLines(graph, true, min_levels, max_levels, 0.0001, "%#.5f", "\xb0");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale = (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][channel][i] - (float)min_levels;
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - (level * y_scale);
					}
			  }
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#endif

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					drawMarkers(graph, m, channel, min_levels, max_levels, "\xb0", 1.0f, draw_v_line);
					draw_v_line = false;
				}
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				String units      = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "\xb0" : "\xb0";
				String line_type1 = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 " : "line ";
				String line_type2 = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S21 " : "line ";
				switch (chan_mask & 3)
				{
					case 1: drawMarkersOnGraph(graph, m, 0,    units, line_type1); break;
					case 2: drawMarkersOnGraph(graph, m, 1,    units, line_type2); break;
					case 3: drawMarkersOnGraph(graph, m, 0, 1, units, line_type1, line_type2); break;
					default: break;
				}
				draw_on_graph = false;
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif

	String units = "\xb0";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11" : "line";
	s[1] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S21" : "line";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						//String s2 = common.valueToStr(m_levels[mem][channel][index], true, false, "%#.6g") + units;
						String s2;
						s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = (!unwrap) ? "Freq Phase" : "Freq Phase Unwrap";
		if (chan_mask & 1)
			title += " S11";
		if (chan_mask & 2)
			title += " S21";
		drawDetails(graph, graph_type, chan_mask, mem, index, title, units, s, s_value, "%#.7f");
	}
}

void __fastcall CGraphs::drawVSWRReturnLossS11(const int graph, const int graph_type, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

//	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the VSWR

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int channel = 0;
			{
				for (int i = 0; i < size; i++)
				{
					float level = data_unit.VSWR(data_unit.m_point_filt[mem][i].sParam[channel]);
					if (normalise)
						level -= data_unit.VSWR(data_unit.m_point_norm[i].sParam[channel]);
					m_levels[mem][channel].push_back(level);
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	const double gamma = settings.m_graph_setting[graph_type].gamma;

	drawNoneLinMagLines(graph, true, min_levels, max_levels, gamma, 0.00001, "", "", true);

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

		//const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale = (double)gh / range_levels;

		const double pow_x_scale = 1.0 / range_levels;
		const double pow_y = (gamma >= 1.0) ? 1.0 / gamma : 1.0;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz = data_unit.m_point_filt[mem][i].Hz;
						double level = (double)m_levels[mem][channel][i] - min_levels;
						if (gamma > GAMMA_MIN)
						{	// non-linear scale
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? pow(level * pow_x_scale, pow_y) * range_levels : 0.0;
								// reverse
								//level = min_levels + (pow(level * pow_x_scale, gamma) * range_levels);
							#else
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - ((float)level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#endif

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int channel = 0;
			{
				drawMarkers(graph, m, channel, min_levels, max_levels, "", gamma, draw_v_line);
				draw_v_line = false;
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
					drawMarkersOnGraph(graph, m, channel, "", "S11  ");
				else
					drawMarkersOnGraph(graph, m, channel, "", "Line ");
				draw_on_graph = false;
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif

	String units = "";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11" : "line";
	s[1] = "";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, gamma, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, gamma, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, gamma);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						//String s2 = common.valueToStr(m_levels[mem][channel][index], true, false, "%#.6g") + units;
						String s2;
						s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = "Freq VSWR S11";
		drawDetails(graph, graph_type, 1, mem, index, title, units, s, s_value, "%#.6f");
	}
}

void __fastcall CGraphs::drawImpedanceS11(const int graph, const int graph_type, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the impedance

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int channel = 0;
			{
				m_levels[mem][channel].resize(size);
				for (int i = 0; i < size; i++)
				{
					complexf c = data_unit.m_point_filt[mem][i].sParam[channel];
					complexf z = data_unit.impedance(c, 50);
					float Zs   = data_unit.magnitude(z);
					if (normalise)
					{
						c   = data_unit.m_point_norm[i].sParam[channel];
						z   = data_unit.impedance(c, 50);
						Zs -= data_unit.magnitude(z);
					}
					m_levels[mem][channel][i] = Zs;
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	const double gamma = settings.m_graph_setting[graph_type].gamma;

	if (gamma <= GAMMA_MIN)
		drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.6f", "");
	else
		drawNoneLinMagLines(graph, true, min_levels, max_levels, gamma, 0.00001, "%#.6f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale = (double)gh / range_levels;

		const double pow_x_scale = 1.0 / range_levels;
		const double pow_y = (gamma >= 1.0) ? 1.0 / gamma : 1.0;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz = data_unit.m_point_filt[mem][i].Hz;
						double level = (double)m_levels[mem][channel][i] - min_levels;
						if (gamma > GAMMA_MIN)
						{	// non-linear scale
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? pow(level * pow_x_scale, pow_y) * range_levels : 0.0;
								// reverse
								//level = min_levels + (pow(level * pow_x_scale, gamma) * range_levels);
							#else
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - ((float)level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#endif

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int channel = 0;
			{
				drawMarkers(graph, m, channel, min_levels, max_levels, " Ohm", gamma, draw_v_line);
				draw_v_line = false;
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
					drawMarkersOnGraph(graph, m, channel, " Ohm", "S11  ");
				else
					drawMarkersOnGraph(graph, m, channel, " Ohm", "Line ");
				draw_on_graph = false;
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif

	String units = "";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 Zs" : "line";
	s[1] = "";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, gamma, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, gamma, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, gamma);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						String s2 = common.valueToStr(m_levels[mem][channel][index], true, false, "%#.6g") + units;
						//String s2;
						//s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						//s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = "Freq Impedance S11";
		drawDetails(graph, graph_type, 1, mem, index, title, units, s, s_value, "%#.6f");
	}
}

void __fastcall CGraphs::drawQualityFactorS11(const int graph, const int graph_type, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the values

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int channel = 0;
			{
				m_levels[mem][channel].resize(size);
				for (int i = 0; i < size; i++)
				{
					float level = data_unit.qualityFactor(data_unit.m_point_filt[mem][i].sParam[channel], 50);
					if (normalise)
						level -= data_unit.qualityFactor(data_unit.m_point_norm[i].sParam[channel], 50);
					m_levels[mem][channel][i] = level;
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	const double gamma = settings.m_graph_setting[graph_type].gamma;

	if (gamma <= GAMMA_MIN)
		drawMagLines(graph, true, min_levels, max_levels, 0.0001, "%#.5f", "");
	else
		drawNoneLinMagLines(graph, true, min_levels, max_levels, gamma, 0.0001, "%#.5f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale =  (double)gh / range_levels;

		const double pow_x_scale = 1.0 / range_levels;
		const double pow_y = (gamma >= 1.0) ? 1.0 / gamma : 1.0;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			const int channel = 0;
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz = data_unit.m_point_filt[mem][i].Hz;
						double level = (double)m_levels[mem][channel][i] - min_levels;
						if (gamma > GAMMA_MIN)
						{	// non-linear scale
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? pow(level * pow_x_scale, pow_y) * range_levels : 0.0;
								// reverse
								//level = min_levels + (pow(level * pow_x_scale, gamma) * range_levels);
							#else
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - ((float)level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int channel = 0;

			{
				drawMarkers(graph, m, channel, min_levels, max_levels, "", 1.0f, draw_v_line);
				draw_v_line = false;
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				String line_type = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11  " : "line ";
				drawMarkersOnGraph(graph, m, channel, " Q", line_type);
				draw_on_graph = false;
			}
		}
	}

	String units = "";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11" : "line";
	s[1] = "";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, gamma, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, gamma, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, gamma);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						//String s2 = common.valueToStr(m_levels[mem][channel][index], true, false, "%#.6g") + units;
						String s2;
						s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = "Freq Quality Factor S11";
		drawDetails(graph, graph_type, 1, mem, index, title, units, s, s_value, "%#.6f");
	}
}

void __fastcall CGraphs::drawQCS11(const int graph, const int graph_type, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the values

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
/*
			{	// quality factor
				const int channel = 0;
				{
					m_levels[mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						float level = data_unit.QualityFactor(data_unit.m_point_filt[mem][i].sParam[channel], 50);
						if (normalise)
							level -= data_unit.QualityFactor(data_unit.m_point_norm[i].sParam[channel], 50);
						m_levels[mem][channel][i] = level;
					}
				}
			}
*/
			{	// capacitance
				const int channel = 0;
//				const int channel = 1;
				{
					const float ref_impedance = 50.0f;
					m_levels[mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const double Hz = data_unit.m_point_filt[mem][i].Hz;
						complexf c = data_unit.m_point_filt[mem][i].sParam[channel];
						if (normalise)
							c -= data_unit.m_point_norm[i].sParam[channel];
						const complexf imp     = data_unit.impedance(c, ref_impedance);
						//const complexf imp_p = data_unit.serialToParallel(imp);
						const float cap        = data_unit.impedanceToCapacitance(imp, Hz);
						//const float ind      = data_unit.impedanceToInductance(imp, Hz);
						//const float cap_p    = data_unit.impedanceToCapacitance(imp_p, Hz);
						//const float ind_p    = data_unit.impedanceToInductance(imp_p, Hz);
						m_levels[mem][channel][i] = cap;
					}
				}
			}

		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	//drawMagLines(graph, true,  min_levels, max_levels, 1e-13, "%0.5f", "");
	//drawMagLines(graph, false, min_levels, max_levels, 1e-13, "%0.5f", "F");

	drawMagLines(graph, true,  min_levels, max_levels, 1e-13, "", "F");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale = (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			const int channel = 0;
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][channel][i] - (float)min_levels;
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - (level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int channel = 0;

			{
				drawMarkers(graph, m, channel, min_levels, max_levels, "", 1.0f, draw_v_line);
				draw_v_line = false;
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				String line_type = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11  " : "line ";
				drawMarkersOnGraph(graph, m, channel, " Cap", line_type);
				draw_on_graph = false;
			}
		}
	}

	String units = "";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11" : "line";
	s[1] = "";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						//String s2 = common.valueToStr(m_levels[mem][channel][index], true, false, "%#.6g") + units;
						String s2;
						s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = "Freq Series Capacitance S11";
		//drawDetails(graph, graph_type, 1, mem, index, "Q+C", units, s, s_value, "%#.6f");
		drawDetails(graph, graph_type, 1, mem, index, title, units, s, s_value, "%#.6f");
	}
}

void __fastcall CGraphs::drawQLS11(const int graph, const int graph_type, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the values

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
/*
			{	// quality factor
				const int channel = 0;
				{
					m_levels[mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						float level = data_unit.QualityFactor(data_unit.m_point_filt[mem][i].sParam[channel], 50);
						if (normalise)
							level -= data_unit.QualityFactor(data_unit.m_point_norm[i].sParam[channel], 50);
						m_levels[mem][channel][i] = level;
					}
				}
			}
*/
			{	// inductance
				const int channel = 0;
//				const int channel = 1;
				{
					const float ref_impedance = 50.0f;
					m_levels[mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const double Hz = data_unit.m_point_filt[mem][i].Hz;
						complexf c = data_unit.m_point_filt[mem][i].sParam[channel];
						if (normalise)
							c -= data_unit.m_point_norm[i].sParam[channel];
						const complexf imp     = data_unit.impedance(c, ref_impedance);
						//const complexf imp_p = data_unit.serialToParallel(imp);
						//const float cap      = data_unit.impedanceToCapacitance(imp, Hz);
						const float ind        = data_unit.impedanceToInductance(imp, Hz);
						//const float cap_p    = data_unit.impedanceToCapacitance(imp_p, Hz);
						//const float ind_p    = data_unit.impedanceToInductance(imp_p, Hz);
						m_levels[mem][channel][i] = ind;
					}
				}
			}

		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	//drawMagLines(graph, true,  min_levels, max_levels, 1e-13, "%0.5f", "");
	//drawMagLines(graph, false, min_levels, max_levels, 1e-13, "%0.5f", "H");

	drawMagLines(graph, true,  min_levels, max_levels, 1e-13, "", "H");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale =  (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			const int channel = 0;
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][channel][i] - (float)min_levels;
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - (level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int channel = 0;

			{
				drawMarkers(graph, m, channel, min_levels, max_levels, "", 1.0f, draw_v_line);
				draw_v_line = false;
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				String line_type = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11  " : "line ";
				drawMarkersOnGraph(graph, m, channel, " Ind", line_type);
				draw_on_graph = false;
			}
		}
	}

	String units = "";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11" : "line";
	s[1] = "";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						//String s2 = common.valueToStr(m_levels[mem][channel][index], true, false, "%#.6g") + units;
						String s2;
						s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = "Freq Series Inductance S11";
		//drawDetails(graph, graph_type, 1, mem, index, "Q+L", units, s, s_value, "%#.6f");
		drawDetails(graph, graph_type, 1, mem, index, title, units, s, s_value, "%#.6f");
	}
}

void __fastcall CGraphs::drawGroupDelayS11S21(const int graph, const int graph_type, const uint8_t chan_mask, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the group delay

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 1 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			std::vector <float> phase(size);
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					// compute the phase (in degrees)
					m_levels[mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						complexf sparam = data_unit.m_point_filt[mem][i].sParam[channel];
						float level = data_unit.phase(sparam);
						if (normalise)
						{
							sparam = data_unit.m_point_norm[i].sParam[channel];
							level -= data_unit.phase(sparam);
						}
						phase[i] = level * rad_2_deg;
					}

					// phase unwrap (in degrees)
					common.phaseUnwrapDeg(&phase[0], size);

					// compute the group delay (in ns)
					for (int i = 0; i < (size - 1); i++)
					{
						const int64_t delta_freq  = data_unit.m_point_filt[mem][i + 1].Hz - data_unit.m_point_filt[mem][i + 0].Hz;
						const float delta_phase   = phase[i + 1] - phase[i + 0];
						const float group_delay   = delta_phase / (360 * delta_freq);
						m_levels[mem][channel][i] = group_delay * -1e9f;	// convert to nano-seconds
					}
					m_levels[mem][channel][size - 1] = m_levels[mem][channel][size - 2];
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	drawMagLines(graph, true, min_levels, max_levels, 0.0001, "%#.5f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale = (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][channel][i] - (float)min_levels;
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - (level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#endif

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				if (chan_mask & (1u << channel))
				{
					drawMarkers(graph, m, channel, min_levels, max_levels, " ns", 1.0f, draw_v_line);
					draw_v_line = false;
				}
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				String units      = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? " ns" : " ns";
				String line_type1 = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 " : "line ";
				String line_type2 = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S21 " : "line ";
				switch (chan_mask & 3)
				{
					case 1: drawMarkersOnGraph(graph, m, 0,    units, line_type1); break;
					case 2: drawMarkersOnGraph(graph, m, 1,    units, line_type2); break;
					case 3: drawMarkersOnGraph(graph, m, 0, 1, units, line_type1, line_type2); break;
					default: break;
				}
				draw_on_graph = false;
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif

	String units = "";
	String s[MAX_CHANNELS];
	s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11" : "line";
	s[1] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S21" : "line";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						String s2 = common.valueToStr(m_levels[mem][channel][index], true, false, "%#.6g") + units;
						//String s2;
						//s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						//s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = "Freq Group Delay ns";
		if (chan_mask & 1)
			title += " S11";
		if (chan_mask & 2)
			title += " S21";
		drawDetails(graph, graph_type, chan_mask, mem, index, title, units, s, s_value, "%#.6f");
	}
}

void __fastcall CGraphs::drawSeriesRJX(const int graph, const int graph_type, const int mask, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the Rs

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int chan = 0;
			{
				if (mask & 1)
					m_levels[mem][chan + 0].resize(size);
				if (mask & 2)
					m_levels[mem][chan + 1].resize(size);
				for (int i = 0; i < size; i++)
				{
					complexf sparam = data_unit.m_point_filt[mem][i].sParam[chan];
					complexf z = data_unit.impedance(sparam, 50);
					if (normalise)
					{
						sparam = data_unit.m_point_norm[i].sParam[chan];
						z -= data_unit.impedance(sparam, 50);
					}
					if (mask & 1)
						m_levels[mem][chan + 0][i] = z.real();
					if (mask & 2)
						m_levels[mem][chan + 1][i] = z.imag();
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.6f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale = (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int chan = 0; chan < MAX_CHANNELS; chan++)
			{
				const int size = m_levels[mem][chan].size();
				if (size > 0)
				{
					m_line_points[graph][mem][chan].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][chan][i] - (float)min_levels;
						m_line_points[graph][mem][chan][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][chan][i].y = (gy + gh) - (level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#endif

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			for (int chan = 0; chan < MAX_CHANNELS; chan++)
			{
				drawMarkers(graph, m, chan, min_levels, max_levels, " Ohm", 1.0f, draw_v_line);
				draw_v_line = false;
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				switch (mask & 3)
				{
					default:
						break;
					case 1:
						drawMarkersOnGraph(graph, m, 0, " Ohm", "Rs   ");
						draw_on_graph = false;
						break;
					case 2:
						drawMarkersOnGraph(graph, m, 1, " Ohm", "Xs   ");
						draw_on_graph = false;
						break;
					case 3:
						drawMarkersOnGraph(graph, m, 0, 1, " Ohm", "Rs   ", "Xs   ");
						draw_on_graph = false;
						break;
				}
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif

	String units = "";
	String s[MAX_CHANNELS];
	if (mask & 1)
		s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 Rs" : "line";
	if (mask & 2)
		s[1] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 Xs" : "line";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int chan = 0; chan < MAX_CHANNELS; chan++)
				{
					if (index >= 0 && index < (int)m_levels[mem][chan].size())
					{
						String s2 = common.valueToStr(m_levels[mem][chan][index], true, false, "%#.6g") + units;
						s_value[chan] = s2.Trim();
					}
				}
			}
		}

		String title;
		switch (mask & 3)
		{
			default: break;
			case 1: title = "Freq Rs S11"; break;
			case 2: title = "Freq Xs S11"; break;
			case 3: title = "Freq Rs+jX S11"; break;
		}
		drawDetails(graph, graph_type, mask, mem, index, title, units, s, s_value, "%#.6f");
	}
}

void __fastcall CGraphs::drawParallelRJX(const int graph, const int graph_type, const int mask, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the Rp

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int chan = 0;
			{
				if (mask & 1)
					m_levels[mem][chan + 0].resize(size);
				if (mask & 2)
					m_levels[mem][chan + 1].resize(size);
				if (mask & 4)
					m_levels[mem][chan + 0].resize(size);
				if (mask & 8)
					m_levels[mem][chan + 1].resize(size);
				for (int i = 0; i < size; i++)
				{
					complexf sparam = data_unit.m_point_filt[mem][i].sParam[chan];
					complexf z      = data_unit.impedance(sparam, 50);
					complexf zp     = data_unit.serialToParallel(z);
					if (normalise)
					{
						sparam = data_unit.m_point_norm[i].sParam[chan];
						z      = data_unit.impedance(sparam, 50);
						zp    -= data_unit.serialToParallel(z);
					}
					if (mask & 1)
						m_levels[mem][chan + 0][i] = zp.real();
					if (mask & 2)
						m_levels[mem][chan + 1][i] = zp.imag();
					if (mask & 4)
						m_levels[mem][chan + 0][i] = 1/zp.real();
					if (mask & 8)
						m_levels[mem][chan + 1][i] = -1/zp.imag();
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.6f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float  y_scale = (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int chan = 0; chan < MAX_CHANNELS; chan++)
			{
				const int size = m_levels[mem][chan].size();
				if (size > 0)
				{
					m_line_points[graph][mem][chan].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][chan][i] - (float)min_levels;
						m_line_points[graph][mem][chan][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][chan][i].y = (gy + gh) - (level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			for (int chan = 0; chan < MAX_CHANNELS; chan++)
			{
				if (mask & 0xc)
				  drawMarkers(graph, m, chan, min_levels, max_levels, " S", 1.0f, draw_v_line);
        else
				  drawMarkers(graph, m, chan, min_levels, max_levels, " Ohm", 1.0f, draw_v_line);
				draw_v_line = false;
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				switch (mask & 15)
				{
					default:
						break;
					case 1:
						drawMarkersOnGraph(graph, m, 0, " Ohm", "Rp   ");
						draw_on_graph = false;
						break;
					case 2:
						drawMarkersOnGraph(graph, m, 1, " Ohm", "Xp   ");
						draw_on_graph = false;
						break;
					case 3:
						drawMarkersOnGraph(graph, m, 0, 1, " Ohm", "Rp   ", "Xp   ");
						draw_on_graph = false;
						break;
					case 4:
						drawMarkersOnGraph(graph, m, 0, " S", "G   ");
						draw_on_graph = false;
						break;
					case 8:
						drawMarkersOnGraph(graph, m, 1, " S", "B   ");
						draw_on_graph = false;
						break;
					case 12:
						drawMarkersOnGraph(graph, m, 0, 1, " S", "G    ", "B    ");
						draw_on_graph = false;
						break;
				}
			}
		}
	}

	String units = "";
	String s[MAX_CHANNELS];
	if (mask & 1)
		s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 Rp" : "line";
	if (mask & 2)
		s[1] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 Xp" : "line";
	if (mask & 4)
		s[0] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 G" : "line";
	if (mask & 8)
		s[1] = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "S11 B" : "line";

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int chan = 0; chan < MAX_CHANNELS; chan++)
				{
					if (index >= 0 && index < (int)m_levels[mem][chan].size())
					{
						String s2 = common.valueToStr(m_levels[mem][chan][index], true, false, "%#.6g") + units;
						s_value[chan] = s2.Trim();
					}
				}
			}
		}

		String title;
		switch (mask & 15)
		{
			default: break;
			case 1: title = "Freq Rp S11"; break;
			case 2: title = "Freq Xp S11"; break;
			case 3: title = "Freq Rp||jXp S11"; break;
			case 4: title = "Freq G S11"; break;
			case 8: title = "Freq B S11"; break;
			case 12: title = "Freq G+jB S11"; break;
		}
//owen
		drawDetails(graph, graph_type, mask&0xc ? (mask&0xc)>>2 : mask, mem, index, title, units, s, s_value, "%#.6f");
	}
}

void __fastcall CGraphs::drawRealImag(const int graph, const int graph_type, const int channel, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	//const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int chan = 0;
			{
				m_levels[mem][chan + 0].resize(size);
				m_levels[mem][chan + 1].resize(size);
				for (int i = 0; i < size; i++)
				{
					complexf sparam = data_unit.m_point_filt[mem][i].sParam[channel];
					if (normalise)
						sparam -= data_unit.m_point_norm[i].sParam[channel];
					m_levels[mem][chan + 0][i] = sparam.real();
					m_levels[mem][chan + 1][i] = sparam.imag();
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawFreqLines(graph, graph_type);

	drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.6f", "");

	{	// compute the graph points
		const double range_levels = fabs(max_levels - min_levels);

//		const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float y_scale  = (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int channel = 0; channel < 2; channel++)
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][channel][i] - (float)min_levels;
						m_line_points[graph][mem][channel][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][channel][i].y = (gy + gh) - (level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	#ifndef USE_OPENGL
		TFont *font = m_graph_bm->Canvas->Font;
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#endif

	if (gs && gs->show_markers)
	{	// draw the markers
		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			for (int channel = 0; channel < 2; channel++)
			{
				drawMarkers(graph, m, channel, min_levels, max_levels, "", 1.0f, draw_v_line);
				draw_v_line = false;
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				drawMarkersOnGraph(graph, m, 0, 1, "", "Real ", "Imag ");
				draw_on_graph = false;
			}
		}
	}

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font = font;
	#endif

	String units = "";
	String s[MAX_CHANNELS];
	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
	{  // VNA
		s[0] = (channel <= 0) ? "S11 Real" : "S21 Real";
		s[1] = (channel <= 0) ? "S11 Imag" : "S21 Imag";
	}
	else
	{	// TinySA
		s[0] = "Real";
		s[1] = "Imag";
	}

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						//String s2 = common.valueToStr(m_levels[mem][channel][index], true, false, "%#.6g") + units;
						String s2;
						s2.printf(L"%#.4g", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = "Freq S-Params";
		if (channel == 0)
			title += " S11";
		if (channel == 1)
			title += " S21";
		drawDetails(graph, graph_type, 3, mem, index, title, units, s, s_value, "%#.7f");
	}
}

void __fastcall CGraphs::drawCoaxLossS11(const int graph, const int graph_type, const bool show_marker_text)
{
	// make sure any previous trace is cleared
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	// sanity check some supplied details
	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	// fetch the graph position and size - so we know where the graph area is to draw in
	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	// sanity check the sweep/scan frequencies
	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	// some graph settings we need
	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the log magnitude from the received VNA s-params

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);	// number of spot frequencies
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int chan = 0;	// S11 = chan 0 (S21 = chan 1)
			{
				m_levels[mem][chan].resize(size);
				for (int i = 0; i < size; i++)
				{
					complexf sparam = data_unit.m_point_filt[mem][i].sParam[chan];
					float level     = data_unit.gain10(sparam);
					if (normalise)
					{
						sparam = data_unit.m_point_norm[i].sParam[chan];
						level -= data_unit.gain10(sparam);
					}
					m_levels[mem][chan][i] = level;
				}
			}
		}
	}

	// ***************
	// curve smoothing

	// apply a notch to the S11 data to remove the phasing/ringing
	for (int mem = 0, chan = 0; mem < MAX_MEMORIES; mem++)
	{
		if (settings.memoryEnable[mem])
		{
			const float peak_index = data_unit.m_fft_peak_index[graph][mem];
			const int fft_size     = data_unit.m_fft_mag[graph][mem].size();

			if (peak_index > 0 && fft_size > 0)
			{
				// This is where we remove the "ringing" from the S11 trace to produce a clean'ish S11 coax loss trace.
				//
				// We do this by detecting the fundamental frequency of the "ringing" component (thee TDR FFT tells us this) to
				// create a notch filter that we then simply apply to the S11 trace - this almostly completely removes the
				// "ringing" from the trace.
				//
				// The resultant trace is then identical whether it has an open or short on the far end of the coax (or at
				// least with the tests I've so far done) - something that others seem to try and achieve with other
				// more complicated methods that normally involves having to do both the open and the short measurements
				// in order to get a result.

				const int size = m_levels[mem][chan].size();

				if (size > 0)
				{
					// use the FFT result to choose the notch frequency (the ringing frequency)
					//
					// 'fft_size' is the size of the TDR FFT output
					// 'peak_index' is the index (0 to fft_size-1) of the main peak level in the TDR FFT output
					//
					// we use those two values to create the notch filter that removes the 'ringing' from the S11
					// trace whilst leaving all other frequencies (above and below) intack

					biquad_filter.makeNotch(fft_size, peak_index, 0.3f);
					//biquad_filter.makeNotch(fft_size, peak_index, 0.25f);

					// notch out the phasing/ringing - forwards then backwards so as not to shift the waveform sidewards

					std::vector <float> levels(size);

					// forward notch filter
					biquad_filter.prime(m_levels[mem][chan][0]);		// prime the filter
					for (int i = 0; i < size; i++)
						levels[i] = biquad_filter.process(m_levels[mem][chan][i]);	// notch the S11 trace

					// backwards notch filter
					biquad_filter.prime(m_levels[mem][chan][size - 1]);		// prime the filter
					for (int i = size - 1; i >= 0; i--)
						m_levels[mem][chan][i] = biquad_filter.process(m_levels[mem][chan][i]);	// notch the S11 trace

					// combine the forward and backwards outputs of the notch filter - this gives us the final coax loss trace
					for (int i = 0; i < size; i++)
						m_levels[mem][chan][i] = (m_levels[mem][chan][i] + levels[i]) * 0.5f;

				}

				//traceLPF(graph, graph_type, 0.75f, 10);
			}
		}
	}

	// divide by two because the S11 loss is there and back, we only only want the one-way loss
	for (int mem = 0, chan = 0; mem < MAX_MEMORIES; mem++)
		for (int i = 0; i < (int)m_levels[mem][chan].size(); i++)
			m_levels[mem][chan][i] *= 0.5f;

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	// draw the frequency grid/scale
	drawFreqLines(graph, graph_type);

	// draw the magnitude grid/scale
	drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.4f", "");

	{	// compute the trace points
		const double range_levels = fabs(max_levels - min_levels);

		//	const double x_scale = (double)gw / data_unit.m_freq_span_Hz;
		const float y_scale = (double)gh / range_levels;

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			for (int chan = 0; chan < MAX_CHANNELS; chan++)
			{
				const int size = m_levels[mem][chan].size();
				if (size > 0)
				{
					m_line_points[graph][mem][chan].resize(size);
					for (int i = 0; i < size; i++)
					{
						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float level = m_levels[mem][chan][i] - (float)min_levels;
						m_line_points[graph][mem][chan][i].x = freqToX(graph, graph_type, Hz);
						m_line_points[graph][mem][chan][i].y = (gy + gh) - (level * y_scale);
					}
			  }
			}
		}
	}

	// draw the trace
	drawGraphLines(graph, graph_type);

	// ***************

	// draw the markers
	if (gs && gs->show_markers)
	{
		bool draw_v_line   = true;
		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			for (int c = 0; c < MAX_CHANNELS; c++)
			{
				const int size = m_levels[m][c].size();
				if (size > 0)
				{
					drawMarkers(graph, m, c, min_levels, max_levels, " dB", 1.0f, draw_v_line);
					draw_v_line = false;
				}
			}

			if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
			{
				String units     = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? " dB" : " dBm";
				String line_type = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? "loss" : "line ";
				drawMarkersOnGraph(graph, m, 0, units, line_type);
				draw_on_graph = false;
			}
		}
	}

	String units = "dB";
	String s[MAX_CHANNELS];

	if (gs && gs->show_max_marker)
		drawMaxMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);
	if (gs && gs->show_min_marker)
		drawMinMarkers(graph, graph_type, min_levels, max_levels, 1.0f, units, s[0], s[1]);

	if (m_mouse.Hz > 0)
		drawMouse(graph, graph_type, min_levels, max_levels, 1.0f);

	{	// draw the graph title and mouse details
		String s_value[MAX_CHANNELS];

		int64_t Hz = m_mouse.Hz;
		int mem    = m_mouse.point_mem;
		int index  = m_mouse.point_index;

		if (mem < 0)
		{
			mem = data_unit.firstUsedMem();
			if (mem >= 0 && index >= 0 && Hz < 0)
				Hz = data_unit.indexFreq(index, mem);
		}

		if (mem >= 0)
		{
			if (settings.memoryEnable[mem] && Hz > 0)
			{
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
				{
					if (index >= 0 && index < (int)m_levels[mem][channel].size())
					{
						String s2;
						s2.printf(L"%#.4f", m_levels[mem][channel][index]);
						s2 = common.trimTrailingZeros(s2) + " " + units;
						s_value[channel] = s2.Trim();
					}
				}
			}
		}

		String title = String("Freq Coax Loss S11 " + units).Trim();

		drawDetails(graph, graph_type, 1, mem, index, title, units, s, s_value, "%#.4f");
	}

	{	// draw the coax open/short message onto the graph
		String s = " Far end of coax must be open or short ";
		const int x = gx + 5;
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // make bold
			//m_graph_bm->Canvas->Brush->Style = bsClear;
			m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
			m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;
			const int y = gy + gh - 1 - m_graph_bm->Canvas->TextHeight("Hq|");
			m_graph_bm->Canvas->TextOut(x, y, s);
			m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style << fsBold;  // remove bold
		#else
			GLrgba back_col = colourToGLcolour(settings.m_colours.background);
			GLrgba font_col = colourToGLcolour(settings.m_colours.font);
			glColor3ub(font_col.r, font_col.g, font_col.b);
			const int y = gy + gh - 1 - m_gl.font[1].height;
			glTextOut(1, x, y, s);
		#endif
	}
}

void __fastcall CGraphs::drawSmithS11S21(const int graph, const int graph_type, const bool admittance, const int s_channel, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	//const int gw = m_graph_pos[graph].gw;
	//const int gh = m_graph_pos[graph].gh;
	const int cx = m_graph_pos[graph].cx;
	const int cy = m_graph_pos[graph].cy;
	const int gr = m_graph_pos[graph].cr;

	if (gr <= 0)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the Rp

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int channel = 0;
			{
				m_levels[mem][channel + 0].resize(size);
				m_levels[mem][channel + 1].resize(size);
				for (int i = 0; i < size; i++)
				{
					complexf sparam = data_unit.m_point_filt[mem][i].sParam[s_channel];
					if (normalise)
						sparam -= data_unit.m_point_norm[i].sParam[s_channel];
					m_levels[mem][channel + 0][i] = sparam.real();
					m_levels[mem][channel + 1][i] = sparam.imag();
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	{	// find the min/max levels in the graph points
		int    max_chan  = -1;
		int    max_index = -1;
		double max_value = -1;

		int    min_chan  = -1;
		int    min_index = -1;
		double min_value = 0;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int c = 0;
			{
//				for (auto const &level : m_levels[m][c])
				for (int i = 0; i < (int)m_levels[m][c].size(); i++)
				{
					const complexf cpx(m_levels[m][c + 0][i], m_levels[m][c + 1][i]);
					const float mag = data_unit.magnitude(cpx);

					if (max_index < 0 || max_value < mag)
					{
						max_chan  = c;
						max_index = i;
						max_value = mag;
					}
				}
			}
		}

		m_max[graph].channel = max_chan;
		m_max[graph].index   = max_index;
		m_max[graph].value   = max_value;

		m_min[graph].channel = min_chan;
		m_min[graph].index   = min_index;
		m_min[graph].value   = min_value;
	}

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawSmithChart(graph, graph_type, admittance, max_levels);

	{	// compute the graph points
		const float scale = (max_levels > 0) ? gr / max_levels : gr;

		// calculate the graph points
		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			const int channel = 0;
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
//						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float re = m_levels[mem][channel + 0][i];
						const float im = m_levels[mem][channel + 1][i];
						const float x = re * scale;
						const float y = im * scale;
						m_line_points[graph][mem][channel][i].x = cx + x;
						m_line_points[graph][mem][channel][i].y = cy - y;
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			if (settings.memoryEnable[m])
			{
				const int channel = 0;
				drawMarkersSmithPolar(graph, m, s_channel, m_line_points[graph][m][channel], admittance, false);

				if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
				{
					drawMarkersOnSmithPolarGraph(graph, m, s_channel, admittance, false);
					draw_on_graph = false;
				}
			}
		}
	}

	// draw the mouse position
	if (m_mouse.marker_index < 0)
	{
		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int chan = 0;
			drawMouseSmithPolar(graph, graph_type, m, s_channel, m_line_points[graph][m][chan], admittance, false);
		}
	}

	{  // draw the title
		#ifndef USE_OPENGL
			TFont *font = m_graph_bm->Canvas->Font;
			m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style << fsBold;   // go bold
			m_graph_bm->Canvas->Brush->Style = bsClear;
			const int th = m_graph_bm->Canvas->TextHeight("Hq|");
		#else
			const int th = m_gl.font[1].height;
		#endif
		{
			String title = admittance ? "Freq Admittance" : "Freq Smith";
			title += (s_channel <= 0) ? " S11" : " S21";
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->Font->Color = settings.m_colours.font;
				m_graph_bm->Canvas->TextOut(gx - 40, gy - 5 - (th * 2), title);
			#else
				GLrgba font_col = colourToGLcolour(settings.m_colours.font);
				glColor3ub(font_col.r, font_col.g, font_col.b);
				glTextOut(1, gx - 40, gy - 5 - (th * 2), title);
			#endif
		}
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font = font;
		#endif
	}
}

void __fastcall CGraphs::drawPolarS11S21(const int graph, const int graph_type, const int s_channel, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	//const int gw = m_graph_pos[graph].gw;
	//const int gh = m_graph_pos[graph].gh;
	const int cx = m_graph_pos[graph].cx;
	const int cy = m_graph_pos[graph].cy;
	const int gr = m_graph_pos[graph].cr;

	if (gr <= 0)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the Rp

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const bool normalise = ((int)data_unit.m_point_norm.size() == size && settings.normalisationEnabled) ? true : false;
			const int channel = 0;
			{
				m_levels[mem][channel + 0].resize(size);
				m_levels[mem][channel + 1].resize(size);
				for (int i = 0; i < size; i++)
				{
					complexf sparam = data_unit.m_point_filt[mem][i].sParam[s_channel];
					if (normalise)
						sparam -= data_unit.m_point_norm[i].sParam[s_channel];
					m_levels[mem][channel + 0][i] = sparam.real();
					m_levels[mem][channel + 1][i] = sparam.imag();
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	{	// find the min/max levels in the graph points
		int    max_chan  = -1;
		int    max_index = -1;
		double max_value = -1;

		int    min_chan  = -1;
		int    min_index = -1;
		double min_value = 0;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int c = 0;
			{
//				for (auto const &level : m_levels[m][c])
				for (int i = 0; i < (int)m_levels[m][c].size(); i++)
				{
					const complexf cpx(m_levels[m][c + 0][i], m_levels[m][c + 1][i]);
					const float mag = data_unit.magnitude(cpx);

					if (max_index < 0 || max_value < mag)
					{
						max_chan  = c;
						max_index = i;
						max_value = mag;
					}
				}
			}
		}

		m_max[graph].channel = max_chan;
		m_max[graph].index   = max_index;
		m_max[graph].value   = max_value;

		m_min[graph].channel = min_chan;
		m_min[graph].index   = min_index;
		m_min[graph].value   = min_value;
	}

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawPolarChart(graph, graph_type, max_levels);

	{	// compute the graph points
		const float scale = (max_levels > 0) ? gr / max_levels : gr;

		// calculate the graph points
		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			 const int channel = 0;
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
//						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float re = m_levels[mem][channel + 0][i];
						const float im = m_levels[mem][channel + 1][i];
						const float x = re * scale;
						const float y = im * scale;
						m_line_points[graph][mem][channel][i].x = cx + x;
						m_line_points[graph][mem][channel][i].y = cy - y;
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (gs && gs->show_markers)
	{	// draw the markers

		bool draw_on_graph = true;

		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			if (settings.memoryEnable[m])
			{
				const int channel = 0;
				drawMarkersSmithPolar(graph, m, s_channel, m_line_points[graph][m][channel], false, false);

				if (show_marker_text && settings.showMarkersOnGraph && settings.memoryEnable[m] && draw_on_graph)
				{
					drawMarkersOnSmithPolarGraph(graph, m, s_channel, false, false);
					draw_on_graph = false;
				}
			}
		}
	}

	// draw the mouse position
	if (m_mouse.marker_index < 0)
	{
		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int channel = 0;
			drawMouseSmithPolar(graph, graph_type, m, s_channel, m_line_points[graph][m][channel], false, true);
		}
	}

	{
		#ifndef USE_OPENGL
			TFont *font                      = m_graph_bm->Canvas->Font;
			m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style << fsBold;   // go bold
			m_graph_bm->Canvas->Brush->Style = bsClear;
			const int th = m_graph_bm->Canvas->TextHeight("Hq|");
		#else
			const int th = m_gl.font[1].height;
		#endif
		{
			String title = "Freq Polar";
			title += (s_channel <= 0) ? " S11" : " S21";
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->Font->Color = settings.m_colours.font;
				m_graph_bm->Canvas->TextOut(gx - 40, gy - 5 - (th * 2), title);
			#else
				GLrgba font_col = colourToGLcolour(settings.m_colours.font);
				glColor3ub(font_col.r, font_col.g, font_col.b);
				glTextOut(1, gx - 40, gy - 5 - (th * 2), title);
			#endif
		}
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font = font;
		#endif
	}
}

void __fastcall CGraphs::drawPhaseVectorS11S21(const int graph, const int graph_type, const int s_channel, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	//const int gw = m_graph_pos[graph].gw;
	//const int gh = m_graph_pos[graph].gh;
	const int cx = m_graph_pos[graph].cx;
	const int cy = m_graph_pos[graph].cy;
	const int gr = m_graph_pos[graph].cr;

	if (gr <= 0)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the phase mag

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size > 0 && settings.memoryEnable[mem])
		{
			const int channel = 0;
			{
				m_levels[mem][channel + 0].resize(size);
				m_levels[mem][channel + 1].resize(size);
				for (int i = 0; i < size; i++)
				{
					const complexf sparam = data_unit.m_point_filt[mem][i].sParam[s_channel];
					m_levels[mem][channel + 0][i] = sparam.real();
					m_levels[mem][channel + 1][i] = sparam.imag();
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************

	drawPhaseVectorChart(graph, graph_type);

	{	// compute the graph points
		const float scale = gr;

		// calculate the graph points
		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			 const int channel = 0;
			{
				const int size = m_levels[mem][channel].size();
				if (size > 0)
				{
					m_line_points[graph][mem][channel].resize(size);
					for (int i = 0; i < size; i++)
					{
//						const int64_t Hz  = data_unit.m_point_filt[mem][i].Hz;
						const float re = m_levels[mem][channel + 0][i];
						const float im = m_levels[mem][channel + 1][i];
						const float x = re * scale;
						const float y = im * scale;
						m_line_points[graph][mem][channel][i].x = cx + x;
						m_line_points[graph][mem][channel][i].y = cy - y;
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	// draw the mouse position
	if (m_mouse.marker_index < 0)
	{
		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			const int channel = 0;
			drawMouseSmithPolar(graph, graph_type, m, s_channel, m_line_points[graph][m][channel], false, true);
		}
	}

	{
		#ifndef USE_OPENGL
			TFont *font                      = m_graph_bm->Canvas->Font;
			m_graph_bm->Canvas->Font->Style  = m_graph_bm->Canvas->Font->Style << fsBold;   // go bold
			m_graph_bm->Canvas->Brush->Style = bsClear;
			const int th = m_graph_bm->Canvas->TextHeight("Hq|");
		#else
			const int th = m_gl.font[1].height;
		#endif
		{
			String title = "Freq Phase Vector";
			title += (s_channel <= 0) ? " S11" : " S21";
			#ifndef USE_OPENGL
				m_graph_bm->Canvas->Font->Color = settings.m_colours.font;
				m_graph_bm->Canvas->TextOut(gx - 40, gy - 5 - (th * 2), title);
			#else
				GLrgba font_col = colourToGLcolour(settings.m_colours.font);
				glColor3ub(font_col.r, font_col.g, font_col.b);
				glTextOut(1, gx - 40, gy - 5 - (th * 2), title);
			#endif
		}
		#ifndef USE_OPENGL
			m_graph_bm->Canvas->Font = font;
		#endif
	}
}

void __fastcall CGraphs::drawTDRLinear(const int graph, const int graph_type, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the TDR magnitude

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		if (settings.memoryEnable[mem])
		{
			const int size = m_fft[graph][mem].size() / 2;
			if (size > 0)
			{
				const int chan = 0;
				if ((int)data_unit.m_fft_mag[graph][mem].size() >= size)
				{
					m_levels[mem][chan].resize(size);
					for (int i = 0; i < size; i++)
						m_levels[mem][chan][i] = data_unit.m_fft_mag[graph][mem][i];
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

//	drawTimeDistLines(graph, first_mem);
	drawTimeDistLines(graph, -1);

	const double gamma = settings.m_graph_setting[graph_type].gamma;

	if (gamma <= GAMMA_MIN)
		drawMagLines(graph, true, min_levels, max_levels, 0.00001, "%#.6f", "");
	else
		drawNoneLinMagLines(graph, true, min_levels, max_levels, gamma, 0.00001, "%#.6f", "");

//	const double freq_step = data_unit.freq_step(first_mem);
	const double freq_step = data_unit.freq_step(-1);
	const double max_time  = data_unit.max_time(freq_step);
	const double max_dist  = data_unit.max_dist(freq_step, data_unit.m_velocity_factor);

	const double range_levels = fabs(max_levels - min_levels);

	const double y_scale = (double)gh / range_levels;

	// calculate the graph points
	if (freq_step > 0.0 && max_time > 0.0 && max_dist > 0.0)
	{
		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			const double mem_freq_step = data_unit.freq_step(mem);
			const double mem_max_time  = data_unit.max_time(mem_freq_step);
			const int chan = 0;
			{
				const int size = m_levels[mem][chan].size();
				if (size > 1 && mem_max_time > 0.0)
				{
					const float x_scale = gw * (mem_max_time / max_time) / (size - 1);
					m_line_points[graph][mem][chan].resize(size);
					for (int i = 0; i < size; i++)
					{
						double level = (double)m_levels[mem][chan][i] - min_levels;
						if (gamma > GAMMA_MIN)
						{
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? pow(level / range_levels, 1.0 / gamma) * range_levels : 0.0; 	// non-linear scale
							#else
								//const double scale = (10.0 / range_levels) * gamma;
//								const double scale = (1.0 / range_levels) * gamma * 10;
//								level = (level >= 0.0) ? min_levels + log(1.0 + level * scale) * range_levels / log(1.0 + range_levels * scale) : 0.0;
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}
						m_line_points[graph][mem][chan][i].x = gx + (i * x_scale);
						m_line_points[graph][mem][chan][i].y = (gy + gh) - (float)(level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (max_time > 0.0)
	{
		const int mx = gx + IROUND((gw * m_mouse.secs) / max_time);

		bool tdr_graph = isTDRGraph(graph_type);
		if (m_mouse.graph != graph)
		{	// mouse is on another graph so find out what type of graph that is
			const int graph_type = settings.graphType[m_mouse.graph];
			tdr_graph = isTDRGraph(graph_type);
		}

		if (tdr_graph && mx >= gx && mx <= (gx + gw) && m_mouse.y >= gy && m_mouse.y <= (gy + gh))
		{
			String s;

			#ifndef USE_OPENGL
				const int th = m_graph_bm->Canvas->TextHeight("Hq|");
			#else
				const int th = m_gl.font[0].height;
			#endif

			const double dist = max_dist * (m_mouse.secs / max_time);

			if (graph == m_mouse.graph)
			{
				// vertical mouse line
				if (m_gdi_plus)
				{
					uint8_t red, grn, blu;
					colourToRGB(settings.m_colours.mouse_line, red, grn, blu);

					#ifndef USE_OPENGL
						Gdiplus::Pen pen(Gdiplus::Color(255, red, grn, blu), 1);
						pen.SetAlignment(Gdiplus::PenAlignmentCenter);
						pen.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));

						m_gdi_plus->DrawLine(&pen, mx, gy, mx, gy + gh);
					#else
						glLineStipple(1, 0xcccc);
						glEnable(GL_LINE_STIPPLE);
						glDisable(GL_LINE_SMOOTH);
						//glEnable(GL_LINE_SMOOTH);
						glLineWidth(1.0f);
						glColor3ub(red, grn, blu);
						//glBegin(GL_LINE_STRIP);
						glBegin(GL_LINES);
							glVertex2i(mx, gy);
							glVertex2i(mx, gy + gh);
						glEnd();
						glDisable(GL_LINE_STIPPLE);
					#endif
				}

				#ifndef USE_OPENGL
					//m_graph_bm->Canvas->Brush->Style = bsClear;
					m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
					m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;

					s = common.secsToStr(m_mouse.secs, false);
					m_graph_bm->Canvas->TextOut(mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy - 2 - (th * 2), s);

					s.printf(L"%s %#.3fft", common.distToStr(dist, false).c_str(), dist * (float)METERS_TO_FEET);
					m_graph_bm->Canvas->TextOut(mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy + gh + 3 + th, s);
				#else
					GLrgba back_col = colourToGLcolour(settings.m_colours.background);
					GLrgba font_col = colourToGLcolour(settings.m_colours.font);

					glColor3ub(font_col.r, font_col.g, font_col.b);

					s = common.secsToStr(m_mouse.secs, false);
					glTextOut(0, mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy - 2 - (th * 2), s);

					s.printf(L"%s %#.3fft", common.distToStr(dist, false).c_str(), dist * (float)METERS_TO_FEET);
					glTextOut(0, mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy + gh + 3 + th, s);
				#endif
			}

			// draw the marker(s)

			if (m_mouse.point_mem >= 0 && m_mouse.point_index >= 0)
			{	// mouse if near a point

				const int mem   = m_mouse.point_mem;
				const int chan  = 0;
				const int index = m_mouse.point_index;

				const int size = m_levels[mem][chan].size();

				if (index >= 0 && index < size && size > 1)
				{
					String s[4];

					const double mem_freq_step = data_unit.freq_step(mem);
					const double mem_max_time  = data_unit.max_time(mem_freq_step);
					const double mem_max_dist  = data_unit.max_dist(mem_freq_step, data_unit.m_velocity_factor);

					if (mem_max_time > 0.0 && mem_max_dist > 0.0)
					{
						const double secs = m_mouse.secs;
						const double dist = mem_max_dist * (m_mouse.secs / mem_max_time);

						float level = m_levels[mem][chan][index];

						if (gs && gs->show_markers && graph == m_mouse.graph)
						{
							if (mem <= 0)
								s[0].printf(L"index %d/%d, live", 1 + index, size);
							else
								s[0].printf(L"index %d/%d, mem-%d", 1 + index, size, mem);
							s[1].printf(   L"mag   %#.4f", level);
							s[2].printf(   L"time  %s", common.secsToStr(secs).c_str());
							s[3].printf(   L"dist  %s %#.3fft", common.distToStr(dist).c_str(), dist * (float)METERS_TO_FEET);
						}

						if (gamma > GAMMA_MIN)
						{	// non-linear scale
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? pow(level / range_levels, 1.0 / gamma) * range_levels : 0.0;
							#else
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}

						int x = gx + IROUND(gw * index * mem_max_time / (max_time * (size - 1)));
						int y = gy - IROUND((level - max_levels) * y_scale);

						bool up = false;

						if (y < gy)
						{
							y = gy;
							up = true;	// up-side-down marker
						}
						else
						if (y > (gy + gh))
							y = gy + gh;

						drawMarker(graph, x, y, up, false, false, s[0], s[1], s[2], s[3]);
					}
				}
			}
			else
			{	// mouse is not near a point

				const int chan = 0;
				for (int mem = 0; mem < MAX_MEMORIES; mem++)
				{
					const int size = m_levels[mem][chan].size();
					if (size > 1)
					{
						const double mem_freq_step = data_unit.freq_step(mem);
						const double mem_max_time  = data_unit.max_time(mem_freq_step);
						const double mem_max_dist  = data_unit.max_dist(mem_freq_step, data_unit.m_velocity_factor);

						if (mem_max_time > 0.0 && mem_max_dist > 0.0)
						{
							//const double secs = m_mouse.secs;
							//const double dist = mem_max_dist * (m_mouse.secs / mem_max_time);

							const int index = (size - 1) * (m_mouse.secs / mem_max_time);

							if (index >= 0 && index < size)
							{
								String s[2];

								float level = m_levels[mem][chan][index];

								if (gs && gs->show_markers)
								{
									if (mem <= 0)
										s[0].printf(L"index %d/%d, live", 1 + index, size);
									else
										s[0].printf(L"index %d/%d, mem-%d", 1 + index, size, mem);
									s[1].printf(   L"mag   %#.4f", level);
								}

								if (gamma > GAMMA_MIN)
								{	// non-linear scale
									#ifdef NON_LIN_GAMMA
										level = (level >= 0.0) ? pow(level / range_levels, 1.0 / gamma) * range_levels : 0.0;
									#else
										level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
									#endif
								}

								int x = gx + IROUND(gw * index * mem_max_time / (max_time * (size - 1)));
								int y = gy - IROUND((level - max_levels) * y_scale);

								bool up = false;

								if (y < gy)
								{
									y = gy;
									up = true;	// up-side-down marker
								}
								else
								if (y > (gy + gh))
									y = gy + gh;

								drawMarker(graph, x, y, up, false, false, s[0], s[1]);
							}
						}
					}
				}
			}
		}
		else
		{	// show the peak(s)
			for (int mem = 0; mem < MAX_MEMORIES; mem++)
			{
				const int channel = 0;

				const double mem_freq_step = data_unit.freq_step(mem);
				const double mem_max_time  = data_unit.max_time(mem_freq_step);
				const double mem_max_dist  = data_unit.max_dist(mem_freq_step, data_unit.m_velocity_factor);

				const int size = m_levels[mem][channel].size();
				if (size > 1)
				{
					const float time_scale = gw * (mem_max_time / max_time) / (size - 1);

					const float p_index_f = data_unit.m_fft_peak_index[graph][mem];
					const int p_index     = IROUND(p_index_f);

					if (p_index >= 0 && p_index < size)
					{	// draw peak value marker
						String s[4];

						const float n = p_index_f / (size - 1);
						float level = m_levels[mem][channel][p_index];

						if (gs && gs->show_markers)
						{
							if (mem <= 0)
								s[0].printf(L"index %d/%d, live", 1 + p_index, size);
							else
								s[0].printf(L"index %d/%d, mem-%d", 1 + p_index, size, mem);
							s[1].printf(   L"mag   %#.4f", level);
							s[2].printf(   L"time  %s", common.secsToStr(n * mem_max_time).c_str());
							s[3].printf(   L"dist  %s %#.3fft", common.distToStr(n * mem_max_dist).c_str(), n * mem_max_dist * (float)METERS_TO_FEET);
						}

						if (gamma > GAMMA_MIN)
						{	// non-linear scale
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? pow(level / range_levels, 1.0 / gamma) * range_levels : 0.0;
							#else
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}

						int x = gx + IROUND(p_index * time_scale);
						int y = gy - IROUND((level - max_levels) * y_scale);

						bool up = false;

						if (y < gy)
						{
							y = gy;
							up = true;	// up-side-down marker
						}
						else
						if (y > (gy + gh))
							y = gy + gh;

						drawMarker(graph, x, y, up, false, false, s[0], s[1], s[2], s[3]);
					}
				}
			}
		}
	}

	{
		String title = (graph_type == GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11) ? "Time Lin LP Mag S11" : "Time Lin BP Mag S11";
		drawDetails(graph, graph_type, title);
	}
}

void __fastcall CGraphs::drawTDRLog(const int graph, const int graph_type, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the TDR

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		if (settings.memoryEnable[mem])
		{
			const int size = m_fft[graph][mem].size() / 2;
			if (size > 0)
			{
				if ((int)data_unit.m_fft_dB10[graph][mem].size() >= size)
				{
					const int chan = 0;
					m_levels[mem][chan].resize(size);
					for (int i = 0; i < size; i++)
						m_levels[mem][chan][i] = data_unit.m_fft_dB10[graph][mem][i];
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

//	drawTimeDistLines(graph, first_mem);
	drawTimeDistLines(graph, -1);

	drawMagLines(graph, true, min_levels, max_levels, 0.0001, "%#.4f", "");

//	const double freq_step = data_unit.freq_step(first_mem);
	const double freq_step = data_unit.freq_step(-1);
	const double max_time  = data_unit.max_time(freq_step);
	const double max_dist  = data_unit.max_dist(freq_step, data_unit.m_velocity_factor);

	const double range_levels = fabs(max_levels - min_levels);

	const float y_scale = (double)gh / range_levels;

	// calculate the graph points
	if (freq_step > 0.0 && max_time > 0.0 && max_dist > 0.0)
	{
		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			const double mem_freq_step = data_unit.freq_step(mem);
			const double mem_max_time  = data_unit.max_time(mem_freq_step);
			const int chan = 0;
			{
				const int size = m_levels[mem][chan].size();
				if (size > 1)
				{
					const float x_scale = gw * (mem_max_time / max_time) / (size - 1);
					m_line_points[graph][mem][chan].resize(size);
					for (int i = 0; i < size; i++)
					{
						const float level = m_levels[mem][chan][i] - (float)min_levels;
						m_line_points[graph][mem][chan][i].x = gx + (i * x_scale);
						m_line_points[graph][mem][chan][i].y = (gy + gh) - (level * y_scale);
					}
				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (max_time > 0.0)
	{
		const int mx = gx + IROUND((gw * m_mouse.secs) / max_time);

		bool tdr_graph = isTDRGraph(graph_type);
		if (m_mouse.graph != graph)
		{	// mouse is on another graph so find out what type of graph that is
			const int graph_type = settings.graphType[m_mouse.graph];
			tdr_graph = isTDRGraph(graph_type);
		}

		if (tdr_graph && mx >= gx && mx <= (gx + gw) && m_mouse.y >= gy && m_mouse.y <= (gy + gh))
      {
         String s;

         const int th = m_graph_bm->Canvas->TextHeight("Hq|");

			const double dist = max_dist * (m_mouse.secs / max_time);

			if (graph == m_mouse.graph)
			{
				// vertical mouse line
				if (m_gdi_plus)
				{
					uint8_t red, grn, blu;
					colourToRGB(settings.m_colours.mouse_line, red, grn, blu);

					#ifndef USE_OPENGL
						Gdiplus::Pen pen(Gdiplus::Color(255, red, grn, blu), 1);
						pen.SetAlignment(Gdiplus::PenAlignmentCenter);
						pen.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));
						m_gdi_plus->DrawLine(&pen, mx, gy, mx, gy + gh);
					#else
						glLineStipple(1, 0xcccc);
						glEnable(GL_LINE_STIPPLE);
						glDisable(GL_LINE_SMOOTH);
						//glEnable(GL_LINE_SMOOTH);
						glLineWidth(1.0f);
						glColor3ub(red, grn, blu);
						//glBegin(GL_LINE_STRIP);
						glBegin(GL_LINES);
							glVertex2i(mx, gy);
							glVertex2i(mx, gy + gh);
						glEnd();
						glDisable(GL_LINE_STIPPLE);
					#endif
				}

				#ifndef USE_OPENGL
					//m_graph_bm->Canvas->Brush->Style = bsClear;
					m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
					m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;

					s = common.secsToStr(m_mouse.secs, false);
					m_graph_bm->Canvas->TextOut(mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy - 2 - (th * 2), s);

					s.printf(L"%s %#.3fft", common.distToStr(dist, false).c_str(), dist * (float)METERS_TO_FEET);
					m_graph_bm->Canvas->TextOut(mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy + gh + 3 + th, s);
				#else
					GLrgba back_col = colourToGLcolour(settings.m_colours.background);
					GLrgba font_col = colourToGLcolour(settings.m_colours.font);

					glColor3ub(font_col.r, font_col.g, font_col.b);

					s = common.secsToStr(m_mouse.secs, false);
					glTextOut(0, mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy - 2 - (th * 2), s);

					s.printf(L"%s %#.3fft", common.distToStr(dist, false).c_str(), dist * (float)METERS_TO_FEET);
					glTextOut(0, mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy + gh + 3 + th, s);
				#endif
			}

			// draw the marker(s)

			if (m_mouse.point_mem >= 0 && m_mouse.point_index >= 0)
			{	// mouse is near a point

				const int mem   = m_mouse.point_mem;
				const int chan  = 0;
				const int index = m_mouse.point_index;

				const int size = m_levels[mem][chan].size();

				if (index >= 0 && index < size && size > 1)
				{
					String s[4];

					const double mem_freq_step = data_unit.freq_step(mem);
					const double mem_max_time  = data_unit.max_time(mem_freq_step);
					const double mem_max_dist  = data_unit.max_dist(mem_freq_step, data_unit.m_velocity_factor);

					if (mem_max_time > 0.0 && mem_max_dist > 0.0)
					{
						const double secs = m_mouse.secs;
						const double dist = mem_max_dist * (m_mouse.secs / mem_max_time);

						const float level = m_levels[mem][chan][index];

						if (gs && gs->show_markers && graph == m_mouse.graph)
						{
							if (mem <= 0)
								s[0].printf(L"index %d/%d, live", 1 + index, size);
							else
								s[0].printf(L"index %d/%d, mem-%d", 1 + index, size, mem);
							s[1].printf(L"mag   %#.3f dB", level);
							s[2].printf(   L"time  %s", common.secsToStr(secs).c_str());
							s[3].printf(   L"dist  %s %#.3fft", common.distToStr(dist).c_str(), dist * (float)METERS_TO_FEET);
						}

						int x = gx + IROUND(gw * index * mem_max_time / (max_time * (size - 1)));
						int y = gy - IROUND((level - max_levels) * y_scale);

						bool up = false;

						if (y < gy)
						{
							y = gy;
							up = true;	// up-side-down marker
						}
						else
						if (y > (gy + gh))
							y = gy + gh;

						drawMarker(graph, x, y, up, false, false, s[0], s[1], s[2], s[3]);
					}
				}
			}
			else
			{	// mouse is not near a point

				const int chan = 0;
				for (int mem = 0; mem < MAX_MEMORIES; mem++)
				{
					const int size = m_levels[mem][chan].size();
					if (size > 1)
					{
						const double mem_freq_step = data_unit.freq_step(mem);
						const double mem_max_time  = data_unit.max_time(mem_freq_step);
						const double mem_max_dist  = data_unit.max_dist(mem_freq_step, data_unit.m_velocity_factor);

						if (mem_max_time > 0.0 && mem_max_dist > 0.0)
						{
							//const double secs = m_mouse.secs;
							//const double dist = mem_max_dist * (m_mouse.secs / mem_max_time);

							const int index = (size - 1) * (m_mouse.secs / mem_max_time);

							if (index >= 0 && index < size)
							{
								String s[2];

								const float level = m_levels[mem][chan][index];

								if (gs && gs->show_markers)
								{
									if (mem <= 0)
										s[0].printf(L"index %d/%d, live", 1 + index, size);
									else
										s[0].printf(L"index %d/%d, mem-%d", 1 + index, size, mem);
									s[1].printf(L"mag   %#.3f dB", level);
								}

								int x = gx + IROUND(gw * index * mem_max_time / (max_time * (size - 1)));
								int y = gy - IROUND((level - max_levels) * y_scale);

								bool up = false;

								if (y < gy)
								{
									y = gy;
									up = true;	// up-side-down marker
								}
								else
								if (y > (gy + gh))
									y = gy + gh;

								drawMarker(graph, x, y, up, false, false, s[0], s[1]);
							}
						}
					}
				}
			}
		}
		else
		{
			for (int mem = 0; mem < MAX_MEMORIES; mem++)
			{
				const int channel = 0;

				const double mem_freq_step = data_unit.freq_step(mem);
				const double mem_max_time  = data_unit.max_time(mem_freq_step);
				const double mem_max_dist  = data_unit.max_dist(mem_freq_step, data_unit.m_velocity_factor);

				const int size = m_levels[mem][channel].size();
				if (size > 1 && mem_max_time > 0.0 && mem_max_dist > 0.0)
				{
					const float time_scale = gw * (mem_max_time / max_time) / (size - 1);

					const float p_index_f = data_unit.m_fft_peak_index[graph][mem];
					const int p_index     = IROUND(p_index_f);

					if (p_index >= 0 && p_index < size)
					{	// draw peak value marker
						String s[4];

						const float n     = p_index_f / (size - 1);
						const float level = m_levels[mem][channel][p_index];

						if (gs && gs->show_markers)
						{
							if (mem <= 0)
								s[0].printf(L"index %d/%d, live", 1 + p_index, size);
							else
								s[0].printf(L"index %d/%d, mem-%d", 1 + p_index, size, mem);
							s[1].printf(L"mag   %#.3f dB", level);
							s[2].printf(L"time  %s", common.secsToStr(n * mem_max_time).c_str());
							s[3].printf(L"dist  %s %#.3fft", common.distToStr(n * mem_max_dist).c_str(), n * mem_max_dist * (float)METERS_TO_FEET);
						}

						int x = gx + IROUND(p_index * time_scale);
						int y = gy - IROUND((level - max_levels) * y_scale);

						bool up = false;

						if (y < gy)
						{
							y = gy;
							up = true;	// up-side-down marker
						}
						else
						if (y > (gy + gh))
							y = gy + gh;

						drawMarker(graph, x, y, up, false, false, s[0], s[1], s[2], s[3]);
					}
				}
			}
		}
	}

	{
		String title = (graph_type == GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11) ? "Time Log LP Mag S11" : "Time Log BP Mag S11";
		drawDetails(graph, graph_type, title);
	}
}

void __fastcall CGraphs::drawTDRImpedance(const int graph, const int graph_type, const bool show_marker_text)
{
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_levels[mem][channel].resize(0);
			m_line_points[graph][mem][channel].resize(0);
		}
	}

	if (m_graph_bm == NULL || graph < 0 || graph >= MAX_GRAPHS || graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	const int gx = m_graph_pos[graph].gx;
	const int gy = m_graph_pos[graph].gy;
	const int gw = m_graph_pos[graph].gw;
	const int gh = m_graph_pos[graph].gh;

	if (gw <= 10 || gh <= 10)
		return;

	if (data_unit.m_freq_start_Hz == 0 || data_unit.m_freq_stop_Hz == 0 || data_unit.m_freq_center_Hz == 0 || data_unit.m_freq_span_Hz == 0)
		return;

	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

	// ***************
	// calculate the TDR impedance

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		if (settings.memoryEnable[mem])
		{
			const int chan = 0;
			const int size = m_fft[graph][mem].size() / 2;
			if (size > 0)
			{
				if ((int)data_unit.m_fft_buffer[graph][mem].size() >= size)
				{
					m_levels[mem][chan + 0].resize(size);
					m_levels[mem][chan + 1].resize(size);
					for (int i = 0; i < size; i++)
					{
						const complexf imp = data_unit.impedance(data_unit.m_fft_buffer[graph][mem][i], 50.0f);
						m_levels[mem][chan + 0][i] = imp.real();
						m_levels[mem][chan + 1][i] = imp.imag();
					}
				}
			}
		}
	}

	// ***************
	// curve smoothing

	traceSmooth(graph, graph_type);

	// ***************
	// decide the graph vertical range

	// find the min/max levels in the graph points
	computeTraceMinMax(graph, graph_type);

	// decide on the graph scale to use
	double max_levels;
	double min_levels;
	computeGraphMinMax(graph, graph_type, max_levels, min_levels);

	// ***************

	drawTimeDistLines(graph, -1);

	const double gamma = settings.m_graph_setting[graph_type].gamma;

	if (gamma <= GAMMA_MIN)
		drawMagLines(graph, true, min_levels, max_levels, 0.0001, "%#.5f", "");
	else
		drawNoneLinMagLines(graph, true, min_levels, max_levels, gamma, 0.0001, "%#.5f", "");

//	const double freq_step = data_unit.freq_step(first_mem);
	const double freq_step = data_unit.freq_step(-1);
	const double max_time  = data_unit.max_time(freq_step);
	const double max_dist  = data_unit.max_dist(freq_step, data_unit.m_velocity_factor);

	const double range_levels = fabs(max_levels - min_levels);

	const float y_scale = (double)gh / range_levels;

	// calculate the graph points
	if (freq_step > 0.0 && max_time > 0.0 && max_dist > 0.0)
	{
		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			const double mem_freq_step = data_unit.freq_step(mem);
			const double mem_max_time  = data_unit.max_time(mem_freq_step);
			const int chan = 0;
			{
				const int size = m_levels[mem][chan].size();
				if (size > 1)
				{
					const float x_scale = gw * (mem_max_time / max_time) / (size - 1);

					m_line_points[graph][mem][chan].resize(size);
					for (int i = 0; i < size; i++)
					{
						const complexf imp(m_levels[mem][chan + 0][i], m_levels[mem][chan + 1][i]);
						double level = (double)data_unit.magnitude(imp) - min_levels;
						if (gamma > GAMMA_MIN)
						{	// non-linear scale
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? pow(level / range_levels, 1.0 / gamma) * range_levels : 0.0;
							#else
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}
						m_line_points[graph][mem][chan][i].x = gx + (i * x_scale);
						m_line_points[graph][mem][chan][i].y = (gy + gh) - ((float)level * y_scale);
					}

				}
			}
		}
	}

	drawGraphLines(graph, graph_type);

	// ***************

	if (max_time > 0.0)
	{
		const int mx = gx + IROUND((gw * m_mouse.secs) / max_time);

		bool tdr_graph = isTDRGraph(graph_type);
		if (m_mouse.graph != graph)
		{	// mouse is on another graph so find out what type of graph that is
			const int graph_type = settings.graphType[m_mouse.graph];
			tdr_graph = isTDRGraph(graph_type);
		}

		if (tdr_graph && mx >= gx && mx <= (gx + gw) && m_mouse.y >= gy && m_mouse.y <= (gy + gh))
		{
			String s;

			#ifndef USE_OPENGL
				const int th = m_graph_bm->Canvas->TextHeight("Hq|");
			#else
				const int th = m_gl.font[0].height;
			#endif

			const double dist = max_dist * (m_mouse.secs / max_time);

			if (graph == m_mouse.graph)
			{
				// vertical mouse line
				if (m_gdi_plus)
				{
					uint8_t red, grn, blu;
					colourToRGB(settings.m_colours.mouse_line, red, grn, blu);

					#ifndef USE_OPENGL
						Gdiplus::Pen pen(Gdiplus::Color(255, red, grn, blu), 1);
						pen.SetAlignment(Gdiplus::PenAlignmentCenter);
						pen.SetDashPattern(grid_line_pattern2, ARRAY_SIZE(grid_line_pattern2));
						m_gdi_plus->DrawLine(&pen, mx, gy, mx, gy + gh);
					#else
						glLineStipple(1, 0xcccc);
						glEnable(GL_LINE_STIPPLE);
						glDisable(GL_LINE_SMOOTH);
						//glEnable(GL_LINE_SMOOTH);
						glLineWidth(1.0f);
						glColor3ub(red, grn, blu);
						//glBegin(GL_LINE_STRIP);
						glBegin(GL_LINES);
							glVertex2i(mx, gy);
							glVertex2i(mx, gy + gh);
						glEnd();
						glDisable(GL_LINE_STIPPLE);
					#endif
				}

				#ifndef USE_OPENGL
					//m_graph_bm->Canvas->Brush->Style = bsClear;
					m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
					m_graph_bm->Canvas->Font->Color  = settings.m_colours.font;

					s = common.secsToStr(m_mouse.secs, false);
					m_graph_bm->Canvas->TextOut(mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy - 2 - (th * 2), s);

					s.printf(L"%s %#.3fft", common.distToStr(dist, false).c_str(), dist * (float)METERS_TO_FEET);
					m_graph_bm->Canvas->TextOut(mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy + gh + 3 + th, s);
				#else
					GLrgba back_col = colourToGLcolour(settings.m_colours.background);
					GLrgba font_col = colourToGLcolour(settings.m_colours.font);

					glColor3ub(font_col.r, font_col.g, font_col.b);

					s = common.secsToStr(m_mouse.secs, false);
					glTextOut(0, mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy - 2 - (th * 2), s);

					s.printf(L"%s %#.3fft", common.distToStr(dist, false).c_str(), dist * (float)METERS_TO_FEET);
					glTextOut(0, mx - (m_graph_bm->Canvas->TextWidth(s) / 2), gy + gh + 3 + th, s);
				#endif
			}

			// draw the marker(s)

			if (m_mouse.point_mem >= 0 && m_mouse.point_index >= 0)
			{	// mouse is near a point

				const int mem   = m_mouse.point_mem;
				const int chan  = 0;
				const int index = m_mouse.point_index;

            const int size = m_levels[mem][chan].size();

				if (index >= 0 && index < size && size > 1)
				{
					String s[4];

					const double mem_freq_step = data_unit.freq_step(mem);
					const double mem_max_time  = data_unit.max_time(mem_freq_step);
					const double mem_max_dist  = data_unit.max_dist(mem_freq_step, data_unit.m_velocity_factor);

					if (mem_max_time > 0.0 && mem_max_dist > 0.0)
					{
						const double secs = m_mouse.secs;
						const double dist = mem_max_dist * (m_mouse.secs / mem_max_time);

						const complexf imp(m_levels[mem][chan + 0][index], m_levels[mem][chan + 1][index]);
						float level = data_unit.magnitude(imp);

						if (gs && gs->show_markers && graph == m_mouse.graph)
						{
							if (mem <= 0)
								s[0].printf(L"index %d/%d, live", 1 + index, size);
							else
								s[0].printf(L"index %d/%d, mem-%d", 1 + index, size, mem);
//	   			   	s[1] =          "R+jX  " + common.valueToStr(imp.real(), false, true) + " " + ((imp.imag() >= 0) ? "+j" : "-j") + common.valueToStr(fabs(imp.imag()), false, true);
							s[1] =          "Imp   " + common.valueToStr(level, false, true);
							s[2].printf(   L"time  %s", common.secsToStr(secs).c_str());
							s[3].printf(   L"dist  %s %#.3fft", common.distToStr(dist).c_str(), dist * (float)METERS_TO_FEET);
						}

						if (gamma > GAMMA_MIN)
						{	// non-linear scale
							#ifdef NON_LIN_GAMMA
								level = (level >= 0.0) ? pow(level / range_levels, 1.0 / gamma) * range_levels : 0.0;
							#else
								level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
							#endif
						}

						const int x = gx + IROUND(gw * index * mem_max_time / (max_time * (size - 1)));
						int y = gy - IROUND((level - max_levels) * y_scale);

						bool up = false;

						if (y < gy)
						{
							y = gy;
							up = true;	// up-side-down marker
						}
						else
						if (y > (gy + gh))
							y = gy + gh;

						drawMarker(graph, x, y, up, false, false, s[0], s[1], s[2], s[3]);
					}
				}
			}
			else
			{	// mouse is not near a point

				const int chan = 0;
				for (int mem = 0; mem < MAX_MEMORIES; mem++)
				{
					const int size = m_levels[mem][chan].size();
					if (size > 1)
					{
						const double mem_freq_step = data_unit.freq_step(mem);
						const double mem_max_time  = data_unit.max_time(mem_freq_step);
						const double mem_max_dist  = data_unit.max_dist(mem_freq_step, data_unit.m_velocity_factor);

						if (mem_max_time > 0.0 && mem_max_dist > 0.0)
						{
							//const double secs = m_mouse.secs;
							//const double dist = mem_max_dist * (m_mouse.secs / mem_max_time);

							const int index = (size - 1) * (m_mouse.secs / mem_max_time);

							if (index >= 0 && index < size)
							{
								String s[2];

								const complexf imp(m_levels[mem][chan + 0][index], m_levels[mem][chan + 1][index]);
								float level = data_unit.magnitude(imp);

								if (gs && gs->show_markers)
								{
									if (mem <= 0)
										s[0].printf(L"index %d/%d, live", 1 + index, size);
									else
										s[0].printf(L"index %d/%d, mem-%d", 1 + index, size, mem);
//	   				   		s[1] =          "R+jX  " + common.valueToStr(imp.real(), false, true) + " " + ((imp.imag() >= 0) ? "+j" : "-j") + common.valueToStr(fabs(imp.imag()), false, true);
									s[1] =          "Imp   " + common.valueToStr(level, false, true);
								}

								if (gamma > GAMMA_MIN)
								{	// non-linear scale
									#ifdef NON_LIN_GAMMA
										level = (level >= 0.0) ? pow(level / range_levels, 1.0 / gamma) * range_levels : 0.0;
									#else
										level = (level >= 0.0) ? min_levels + log(1.0 + level * gamma) * range_levels / log(1.0 + range_levels * gamma) : 0.0;
									#endif
								}

								int x = gx + IROUND(gw * index * mem_max_time / (max_time * (size - 1)));
								int y = gy - IROUND((level - max_levels) * y_scale);

								bool up = false;

								if (y < gy)
								{
									y = gy;
									up = true;	// up-side-down marker
								}
								else
								if (y > (gy + gh))
									y = gy + gh;

								drawMarker(graph, x, y, up, false, false, s[0], s[1]);
							}
						}
					}
				}
			}
		}
	}

	{
		String title = "Time Impedance S11";
		drawDetails(graph, graph_type, title);
	}
}

void __fastcall CGraphs::updateFFT(const int graph, const int graph_type, const int mem)
{
	if (graph < 0 || graph >= MAX_GRAPHS)
		return;

	if (mem < 0 || mem >= MAX_MEMORIES)
		return;

	const int size = data_unit.m_point_filt[mem].size();

	if ((!isTDRGraph(graph_type) && graph_type != GRAPH_TYPE_COAX_LOSS_S11) || size < 4)
	{
		data_unit.m_fft_buffer[graph][mem].resize(0);
		data_unit.m_fft_window[graph][mem].resize(0);
		data_unit.m_fft_pwr[graph][mem].resize(0);
		data_unit.m_fft_mag[graph][mem].resize(0);
		data_unit.m_fft_dB10[graph][mem].resize(0);
		data_unit.m_fft_peak_index[graph][mem] = -1;
		data_unit.m_fft_peak_mag[graph][mem] = 0.0f;
		return;
	}

	// *************
	// inverse FFT

	const int channel = 0;

	int m_domain_mode = TD_NONE;
	switch (graph_type)
	{
		default:
		case GRAPH_TYPE_TDR_LIN_BP_S11:
		case GRAPH_TYPE_TDR_LOG_BP_S11:
			m_domain_mode = TD_BANDPASS;
			break;
		case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
			m_domain_mode = TD_LOWPASS_IMPULSE;
			break;
		case GRAPH_TYPE_TDR_IMPEDANCE_S11:
			m_domain_mode = TD_LOWPASS_STEP;
			break;
	}

	int offset      = 0;
	int window_size = size;
	switch (m_domain_mode)
	{
		case TD_NONE:
		case TD_BANDPASS:
//			offset      = size;
//			window_size = size * 2;
			break;
		case TD_LOWPASS_IMPULSE:
			offset      = size;
			window_size = size * 2;
			break;
		case TD_LOWPASS_STEP:
			offset      = size;
			window_size = size * 2;
			break;
	}

	if ((int)data_unit.m_fft_window[graph][mem].size() != window_size)
	{
		// ensure power of 2 and a minimum size of 'FFT_SIZE' and 2 * points
//		const int size2 = ((size * 2) <= FFT_SIZE) ? FFT_SIZE : size * 2;
		const int size2 = ((size * 1) <= FFT_SIZE) ? FFT_SIZE : size * 1;
		int fft_size = next_pow2(size2);

		m_fft[graph][mem].init(fft_size, FFT_WINDOW_NONE, true);

		fft_size = m_fft[graph][mem].size();

		data_unit.m_fft_buffer[graph][mem].resize(fft_size);

		const bool scale_window = (m_domain_mode != TD_LOWPASS_STEP) ? true : false;
		m_fft[graph][mem].createWindow(window_size, FFT_WINDOW_KAISER, scale_window, data_unit.m_fft_window[graph][mem], settings.tdrWindow);
	}

	// zero
	for (int i = 0; i < m_fft[graph][mem].size(); i++)
		data_unit.m_fft_buffer[graph][mem][i] = complexf(0);

	{	// fetch the s-params
		//float scale = (m_domain_mode == TD_BANDPASS) ? 2 : 1;
		float scale = (window_size <= size) ? 2 : 1;
		scale /= m_fft[graph][mem].size();
		for (int i = 0; i < size; i++)
		{
			const float window = data_unit.m_fft_window[graph][mem][i + offset] * scale;
			data_unit.m_fft_buffer[graph][mem][i] = data_unit.m_point_filt[mem][i].sParam[channel] * window;
		}
	}

//	if (m_domain_mode == TD_LOWPASS_IMPULSE || m_domain_mode == TD_LOWPASS_STEP)
	if (window_size > size)
	{	// conjugate
		for (int i = 1; i < m_fft[graph][mem].size() / 2; i++)
		{
			const complexf c = data_unit.m_fft_buffer[graph][mem][i];
			data_unit.m_fft_buffer[graph][mem][m_fft[graph][mem].size() - i] = complexf(c.real(), -c.imag());
		}
	}

	// inverse FFT
	m_fft[graph][mem].transform(&data_unit.m_fft_buffer[graph][mem][0], &data_unit.m_fft_buffer[graph][mem][0], false);

	// convolve
	if (m_domain_mode == TD_LOWPASS_STEP)
	{
		for (int i = 1; i < m_fft[graph][mem].size(); i++)
			data_unit.m_fft_buffer[graph][mem][i] += data_unit.m_fft_buffer[graph][mem][i - 1];
	}

	// pre-compute some results for later use
	if (graph_type != GRAPH_TYPE_TDR_IMPEDANCE_S11)
	{
		const int size = data_unit.m_fft_buffer[graph][mem].size();

		data_unit.m_fft_pwr[graph][mem].resize(size);
		data_unit.m_fft_mag[graph][mem].resize(size);
		data_unit.m_fft_dB10[graph][mem].resize(size);

		for (int i = 0; i < size; i++)
		{
			const complexf c = data_unit.m_fft_buffer[graph][mem][i];
			const float pwr  = (c.real() * c.real()) + (c.imag() * c.imag());
			const float mag  = sqrtf(pwr);
			const float dB10 = (pwr > 0.0f) ? 10.0f * log10f(pwr) : 0.0f;
			data_unit.m_fft_pwr[graph][mem][i]  = pwr;
			data_unit.m_fft_mag[graph][mem][i]  = mag;
			data_unit.m_fft_dB10[graph][mem][i] = dB10;
		}

		// find the peak value
		float p_index = -1;
		float p_value = 0.0f;
		float m[3];
		m[0] = m[1] = m[2] = data_unit.m_fft_mag[graph][mem][0];
		//p_value = m[2];
		for (int i = 0; i < size / 2; i++)
		{
			m[0] = m[1];
			m[1] = m[2];
			m[2] = data_unit.m_fft_mag[graph][mem][i + 1];
			if (m[1] >= p_value && m[1] > m[0] && m[1] > m[2])
			{
            if (i > 0)
            {
               p_index = i;
					p_value = m[1];
				}
			}
		}

		// Hyperbolic interpolation
		// This fine tunes the detected peak position using a little interpolation across 3 FFT bins
		if (p_index > 0 && p_index < (size - 1))
		{
			const float y1 = data_unit.m_fft_mag[graph][mem][p_index - 1];
			const float y2 = data_unit.m_fft_mag[graph][mem][p_index + 0];
			const float y3 = data_unit.m_fft_mag[graph][mem][p_index + 1];
			float d = y1 - (2 * y2) + y3;
			d = (d != 0.0f) ? 0.5f * (y1 - y3) / d : 0.0f;
			p_index        += d;
			//const int32_t delta_Hz = abs((int64_t)frequencies[idx + 0] - frequencies[idx + 1]);
		}

		data_unit.m_fft_peak_index[graph][mem] = p_index;
		data_unit.m_fft_peak_mag[graph][mem]   = p_value;
	}
	else
	{
		data_unit.m_fft_pwr[graph][mem].resize(0);
		data_unit.m_fft_mag[graph][mem].resize(0);
		data_unit.m_fft_dB10[graph][mem].resize(0);
		data_unit.m_fft_peak_index[graph][mem] = -1;
		data_unit.m_fft_peak_mag[graph][mem] = 0.0f;
	}
}

void __fastcall CGraphs::fetchSamples(int history_position)
{
	const int mem = 0;	// live memory

	if (history_position < -((int)MAX_HISTORY - 1))
		history_position = -((int)MAX_HISTORY - 1);
	if (history_position > 0)
		history_position = 0;

	int size = data_unit.freqArraySize(-1);

	if (size <= 0)
	{
		size = data_unit.m_point_incoming.size();
		if (size > 0)
		{
			// show the live incoming points
			data_unit.m_point_mem[mem].resize(size);
			for (int i = 0; i < size; i++)
				data_unit.m_point_mem[mem][i] = data_unit.m_point_incoming[i];

			// apply our own calibration correction (only to the live trace)
			if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
				if (settings.calibrationSelection == CAL_SELECT_APP)
					calibration_module.correct(data_unit.m_point_mem[mem]);
		}
		return;
	}

	data_unit.m_point_mem[mem].resize(size);
	for (int i = 0; i < size; i++)
	{
		data_unit.m_point_mem[mem][i].Hz = data_unit.m_point[i].Hz;
		for (int k = 0; k < MAX_CHANNELS; k++)
			data_unit.m_point_mem[mem][i].sParam[k] = complexf(0);
	}

	// number of frames to average
	const int average_size = 1u << settings.timeAverageLevel;

	// number of history frames so far
	int hist_frames = data_unit.m_history_frames;
	if (hist_frames > (int)MAX_HISTORY)
		hist_frames = (int)MAX_HISTORY;
	if (hist_frames < 0)
		hist_frames = 0;

	// number of available history frames
	int hist_size = (average_size < hist_frames) ? average_size : hist_frames;

	// decide how far back in history we will be
	int hist_pos = history_position;
	if (hist_pos < -(hist_frames - 1))
		hist_pos = -(hist_frames - 1);
	if (hist_pos > 0)
		hist_pos = 0;

	// how many history frames we will be averaging
	hist_size += hist_pos;
	if (hist_size < 0)
		hist_size = 0;

	// newest history frame index we will be displaying
	int hist_index = (hist_frames > 0) ? data_unit.m_history_index - 1 : 0;	// point to the last frame received
	hist_index += hist_pos;
	if (hist_index >= (int)MAX_HISTORY)
		hist_index -= (int)MAX_HISTORY;
	if (hist_index < 0)
		hist_index += (int)MAX_HISTORY;

	#define TIME_AVERAGE_IQ

	if (average_size > 1 && hist_size > 0)
	{	// compute the moving average

		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			for (int i = 0; i < size; i++)
			{
				int count = 0;

				#ifdef TIME_AVERAGE_IQ
					complexf avg;
				#else
					float avg_mag   = 0;
					float avg_phase = 0;
				#endif

				// add the current incoming points - this lets the user see the new points as they arrive (scans across the graphs)
				if (hist_pos == 0 && i < (int)data_unit.m_point_incoming.size())
				{
					if (data_unit.m_point_incoming[i].Hz == data_unit.m_point[i].Hz)
					{
						const complexf pointf = data_unit.m_point_incoming[i].sParam[channel];
						const complexf point(pointf.real(), pointf.imag());

						#ifdef TIME_AVERAGE_IQ
							avg += point;
						#else
							const float mag   = SQR(point.real()) + SQR(point.imag());
							const float phase = atan2f(point.imag(), point.real());
							avg_mag   += mag;
							avg_phase += phase;
						#endif

						count++;
					}
				}

				{	// average the history of scans

					const int h_size = hist_size - count;
					int h_index = hist_index;

					for (int k = 0; k < h_size; k++)
					{
						//const complexf cpx = data_unit.m_point[i].hist[h_index].sParam[channel];
						complexf cpx;
						switch (channel)
						{
							case 0: cpx = data_unit.m_point[i].hist[h_index].s11; break;
							case 1: cpx = data_unit.m_point[i].hist[h_index].s21; break;
							case 2: cpx = data_unit.m_point[i].hist[h_index].s12; break;
							case 3: cpx = data_unit.m_point[i].hist[h_index].s22; break;
						}

						#ifdef TIME_AVERAGE_IQ
							avg += cpx;
						#else
							const float mag   = SQR(cpx.real()) + SQR(cpx.imag());
							const float phase = atan2f(cpx.imag(), cpx.real());
							avg_mag   += mag;
							avg_phase += phase;
						#endif

						count++;
						if (--h_index < 0)
							h_index += (int)MAX_HISTORY;
					}
				}

				if (count > 1)
				{
					#ifdef TIME_AVERAGE_IQ
						avg /= count;
					#else
						avg_mag   = sqrtf(avg_mag / count);
						avg_phase = avg_phase / count;
					#endif
				}

				#ifndef TIME_AVERAGE_IQ
					const complexf avg(cosf(avg_phase) * avg_mag, sinf(avg_phase) * avg_mag);
				#endif

				// save the averaged s-param
				data_unit.m_point_mem[mem][i].sParam[channel] = complexf (avg.real(), avg.imag());
			}
		}
	}
	else
	{	// non-averaged
		for (int i = 0; i < size; i++)
		{
			for (int channel = 0; channel < MAX_CHANNELS; channel++)
			{
				data_unit.m_point_mem[mem][i].Hz = data_unit.m_point[i].Hz;

				complexf point = data_unit.m_point[i].hist[hist_index].sParam[channel];

				// replace with the current incoming points - this lets the user see the new points as they arrive (scans across the graphs)
				if (hist_pos == 0 && i < (int)data_unit.m_point_incoming.size())
				{
					if (data_unit.m_point_incoming[i].Hz == data_unit.m_point_mem[mem][i].Hz)
						point = data_unit.m_point_incoming[i].sParam[channel];
				}

				data_unit.m_point_mem[mem][i].sParam[channel] = point;
			}
		}
	}

	// apply our own calibration correction (only to the live trace)
	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
		if (settings.calibrationSelection == CAL_SELECT_APP)
			calibration_module.correct(data_unit.m_point_mem[mem]);
}

void __fastcall CGraphs::setGraphPos(const int graph, const int x, const int y, const int w, const int h, const int double_v_scale)
{
	if (graph < 0 || graph >= MAX_GRAPHS)
		return;

	const int left_border  = m_graph_bm->Canvas->TextWidth(" -0.00000 ");
	const int right_border = 20;
	const int upper_border = 5 + (m_graph_bm->Canvas->TextHeight("Hq|") * 2) + 2;
	const int lower_border = 5 + (m_graph_bm->Canvas->TextHeight("Hq|") * 2) + 2;

	m_graph_pos[graph].x = x;
	m_graph_pos[graph].y = y;
	m_graph_pos[graph].w = w;
	m_graph_pos[graph].h = h;

	m_graph_pos[graph].gx = x + left_border;
	m_graph_pos[graph].gy = y + upper_border;
	m_graph_pos[graph].gw = double_v_scale ? w - left_border - left_border : w - left_border - right_border;
	m_graph_pos[graph].gh = h - upper_border - lower_border;

	// set the smith chart position and radius
	if (m_graph_pos[graph].gw > 0 && m_graph_pos[graph].gh > 0)
	{
		const int w1 = m_graph_pos[graph].gw + left_border  + right_border;
		const int h1 = m_graph_pos[graph].gh + upper_border + lower_border;
		m_graph_pos[graph].cx = x + (w1 / 2);
		m_graph_pos[graph].cy = y + (h1 / 2);
		m_graph_pos[graph].cr  = ((h - 10) < (w - 10)) ? (h - 10) / 2 : (w - 10) / 2;
	}
	else
	{
		m_graph_pos[graph].cx = 0;
		m_graph_pos[graph].cy = 0;
		m_graph_pos[graph].cr = 0;

	}
}

void __fastcall CGraphs::arrangeGraph(const int graph_type1, const int graph_type2, const int graph_type3, const int graph_type4, const int num_graphs)
{
//	const int graph_type[MAX_GRAPHS] = {graph_type1, graph_type2, graph_type3, graph_type4};

	#ifndef USE_OPENGL
		m_graph_bm->Canvas->Font->Style = m_graph_bm->Canvas->Font->Style >> fsBold;  // remove bold
	#endif

	for (int graph = 0; graph < MAX_GRAPHS; graph++)
		setGraphPos(graph, 0, 0, 0, 0, false);

	const TColor window_colour     = TStyleManager::ActiveStyle->GetStyleColor(scWindow);
	const TColor background_colour = settings.m_colours.background;

	const int border_width = (background_colour != window_colour) ? settings.borderWidth : 0;

	const int gx = border_width;
	const int gy = border_width;
	#ifndef USE_OPENGL
		const int gw = m_graph_bm->Width  - (border_width * 2);
		const int gh = m_graph_bm->Height - (border_width * 2);
	#else
		const int gw = m_gl.win_control->ClientWidth  - (border_width * 2);
		const int gh = m_gl.win_control->ClientHeight - (border_width * 2);
	#endif

	const t_graph_arrange graph_arrange = (num_graphs > 1) ? settings.graphArrangement : GRAPH_ARRANGE_1;

	switch (graph_arrange)
	{
		default:
		case GRAPH_ARRANGE_1:			// 1 graph
			{
				const int graph = 0;
				//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
				const bool double_v_scale = false;
				setGraphPos(graph, gx, gy, gw, gh, double_v_scale);
			}
			break;

		case GRAPH_ARRANGE_1L_1R:		// 1 left 1 right
			{
				int x = gx;
				int y = gy;
				const int w = gw / 2;
				const int h = gh;
				for (int graph = 0; graph < 2; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					setGraphPos(graph, x, y, w, h, double_v_scale);
					x += w;
				}
			}
			break;

		case GRAPH_ARRANGE_1T_1B:		// 1 top 1 bottom
			{
				int x = gx;
				int y = gy;
				const int w = gw;
				const int h = gh / 2;
				for (int graph = 0; graph < 2; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					setGraphPos(graph, x, y, w, h, double_v_scale);
					y += h;
				}
			}
			break;

		case GRAPH_ARRANGE_1T_2B:		// 1 top 2 bottom
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 3; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
							{
								const int w = gw;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
							}
							break;
						case 1:
						case 2:
							{
								const int w = gw  / 2;
								const int h = gh / 2;
								setGraphPos(graph, x, y + h, w, h, double_v_scale);
								x += w;
							}
							break;
					}
				}
			}
			break;

		case GRAPH_ARRANGE_2L_1R:		// 2 left 1 right
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 3; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
						case 2:
							{
								const int w = gw  / 2;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
							}
							break;
						case 1:
							{
								const int w = gw / 2;
								const int h = gh;
								setGraphPos(graph, x + w, y, w, h, double_v_scale);
								y += gh / 2;
							}
							break;
					}
            }
			}
			break;

		case GRAPH_ARRANGE_2T_1B:		// 2 top 1 bottom
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 3; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
						case 1:
							{
								const int w = gw  / 2;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
						case 2:
							{
								const int w = gw;
								const int h = gh / 2;
								setGraphPos(graph, gx, y + h, w, h, double_v_scale);
							}
							break;
					}
            }
			}
			break;

		case GRAPH_ARRANGE_1L_2R:		// 1 left 2 right
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 3; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
							{
								const int w = gw / 2;
								const int h = gh;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
						case 1:
						case 2:
							{
								const int w = gw / 2;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
					}
            }
			}
			break;

		case GRAPH_ARRANGE_3D:			// 3 down
			{
				int x = gx;
				int y = gy;
				const int w = gw;
				const int h = gh / 3;
				for (int graph = 0; graph < 3; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					setGraphPos(graph, x, y, w, h, double_v_scale);
					y += h;
				}
			}
			break;

		case GRAPH_ARRANGE_3A:			// 3 across
			{
				int x = gx;
				int y = gy;
				const int w = gw / 3;
				const int h = gh;
				for (int graph = 0; graph < 3; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					setGraphPos(graph, x, y, w, h, double_v_scale);
					x += w;
				}
			}
			break;

		case GRAPH_ARRANGE_4S:			// 4 graphs in a square
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					const int w = gw  / 2;
					const int h = gh / 2;
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					if (graph == 2)
					{
						x  = gx;
						y += h;
					}
					setGraphPos(graph, x, y, w, h, double_v_scale);
					x += w;
				}
			}
			break;

		case GRAPH_ARRANGE_4A:			// 4 across
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					const int w = gw / 4;
					const int h = gh;
					setGraphPos(graph, x, y, w, h, double_v_scale);
					x += w;
				}
         }
			break;

		case GRAPH_ARRANGE_4D:			// 4 down
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					const int w = gw;
					const int h = gh / 4;
					setGraphPos(graph, x, y, w, h, double_v_scale);
					y += h;
				}
         }
			break;

		case GRAPH_ARRANGE_3T_1B:		// 3 top 1 bottom
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
						case 1:
						case 2:
							{
								const int w = gw  / 3;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
						case 3:
							{
								const int w = gw;
								const int h = gh / 2;
								x = gx;
								y = gy + h;
								setGraphPos(graph, x, y, w, h, double_v_scale);
							}
							break;
					}
				}
			}
			break;

		case GRAPH_ARRANGE_3L_1R:		// 3 left 1 right
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
						case 1:
						case 2:
							{
								const int w = gw / 2;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
						case 3:
							{
								const int w = gw / 2;
								const int h = gh;
								setGraphPos(graph, x + w, gy, w, h, double_v_scale);
							}
							break;
               }
				}
			}
			break;

		case GRAPH_ARRANGE_1T_3B:		// 1 top 3 bottom
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
							{
								const int w = gw;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
						case 1:
						case 2:
						case 3:
							{
								const int w = gw  / 3;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
					}
				}
			}
			break;

		case GRAPH_ARRANGE_1L_3R:		// 1 left 3 right
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
							{
								const int w = gw  / 2;
								const int h = gh;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
						case 1:
						case 2:
						case 3:
							{
								const int w = gw  / 2;
								const int h = gh / 3;
					 			setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
               }
				}
			}
			break;

		case GRAPH_ARRANGE_1T_1M_2B:	// 1 top 1 middle 2 bottom
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
						case 1:
							{
								const int w = gw;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
						case 2:
						case 3:
							{
								const int w = gw  / 2;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
               }
				}
			}
			break;

		case GRAPH_ARRANGE_1T_2M_1B:	// 1 top 2 middle 1 bottom
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
							{
								const int w = gw;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
						case 1:
							{
								const int w = gw  / 2;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
						case 2:
							{
								const int w = gw  / 2;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x  = gx;
								y += h;
							}
							break;
						case 3:
							{
								const int w = gw;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
							}
							break;
               }
				}
			}
			break;

		case GRAPH_ARRANGE_2T_1M_1B:	// 1 top 1 middle 1 bottom
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
							{
								const int w = gw  / 2;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
						case 1:
							{
								const int w = gw  / 2;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x  = gx;
								y += h;
							}
							break;
						case 2:
						case 3:
							{
								const int w = gw;
								const int h = gh / 3;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
               }
				}
			}
			break;

		case GRAPH_ARRANGE_1L_1M_2R:	// 1 left   1 middle   2 right
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
						case 1:
							{
								const int w = gw  / 3;
								const int h = gh;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
						case 2:
						case 3:
							{
								const int w = gw  / 3;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
               }
				}
			}
			break;

		case GRAPH_ARRANGE_1L_2M_1R:	// 1 left   2 middle   1 right
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
							{
								const int w = gw  / 3;
								const int h = gh;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
						case 1:
							{
								const int w = gw  / 3;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
						case 2:
							{
								const int w = gw  / 3;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
								y  = gy;
							}
							break;
						case 3:
							{
								const int w = gw / 3;
								const int h = gh;
								setGraphPos(graph, x, y, w, h, double_v_scale);
							}
							break;
					}
				}
			}
			break;

		case GRAPH_ARRANGE_2L_1M_1R:	// 2 left   1 middle   1 right
			{
				int x = gx;
				int y = gy;
				for (int graph = 0; graph < 4; graph++)
				{
					//const bool double_v_scale = (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_CAP_S11) || (graph_type[graph] == GRAPH_TYPE_QUALITY_FACTOR_IND_S11);
					const bool double_v_scale = false;
					switch (graph)
					{
						case 0:
							{
								const int w = gw / 3;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								y += h;
							}
							break;
						case 1:
							{
								const int w = gw / 3;
								const int h = gh / 2;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
								y  = gy;
							}
							break;
						case 2:
						case 3:
							{
								const int w = gw / 3;
								const int h = gh;
								setGraphPos(graph, x, y, w, h, double_v_scale);
								x += w;
							}
							break;
					}
				}
			}
			break;
	}
}

void __fastcall CGraphs::drawGraph(const int graph, const int graph_type, const bool show_marker_text)
{
	#ifndef USE_OPENGL
		if (m_pb == NULL)
			return;
	#endif

	switch (graph_type)
	{
		case GRAPH_TYPE_LOGMAG_S11:             drawLogMagS11S21(graph, graph_type, 0x01, show_marker_text);     break;
		case GRAPH_TYPE_LOGMAG_S21:             drawLogMagS11S21(graph, graph_type, 0x02, show_marker_text);     break;
		case GRAPH_TYPE_LOGMAG_S11S21:          drawLogMagS11S21(graph, graph_type, 0x03, show_marker_text);     break;
//		case GRAPH_TYPE_LOGMAG_S11S21S21S22:    drawLogMagS11S21(graph, graph_type, 0x0f, show_marker_text);     break;
		case GRAPH_TYPE_LINMAG_S11:             drawLinMagS11S21(graph, graph_type, 1, show_marker_text);        break;
		case GRAPH_TYPE_LINMAG_S21:             drawLinMagS11S21(graph, graph_type, 2, show_marker_text);        break;
		case GRAPH_TYPE_LINMAG_S11S21:          drawLinMagS11S21(graph, graph_type, 3, show_marker_text);        break;
		case GRAPH_TYPE_PHASE_S11:              drawPhaseS11S21(graph, graph_type, 1, false, show_marker_text);  break;
		case GRAPH_TYPE_PHASE_S21:              drawPhaseS11S21(graph, graph_type, 2, false, show_marker_text);  break;
		case GRAPH_TYPE_PHASE_S11S21:           drawPhaseS11S21(graph, graph_type, 3, false, show_marker_text);  break;
		case GRAPH_TYPE_PHASE_UNWRAP_S11:       drawPhaseS11S21(graph, graph_type, 1, true, show_marker_text);   break;
		case GRAPH_TYPE_PHASE_UNWRAP_S21:       drawPhaseS11S21(graph, graph_type, 2, true, show_marker_text);   break;
		case GRAPH_TYPE_PHASE_UNWRAP_S11S21:    drawPhaseS11S21(graph, graph_type, 3, true, show_marker_text);   break;
		case GRAPH_TYPE_VSWR_S11:               drawVSWRReturnLossS11(graph, graph_type, show_marker_text);      break;
		case GRAPH_TYPE_IMPEDANCE_S11:          drawImpedanceS11(graph, graph_type, show_marker_text);           break;
		case GRAPH_TYPE_SERIES_RJX_S11:         drawSeriesRJX(graph, graph_type, 3, show_marker_text);           break;
		case GRAPH_TYPE_PARALLEL_RJX_S11:       drawParallelRJX(graph, graph_type, 3, show_marker_text);         break;
		case GRAPH_TYPE_SERIES_RESISTANCE_S11:  drawSeriesRJX(graph, graph_type, 1, show_marker_text);           break;
		case GRAPH_TYPE_SERIES_REACTANCE_S11:   drawSeriesRJX(graph, graph_type, 2, show_marker_text);           break;
		case GRAPH_TYPE_QUALITY_FACTOR_S11:     drawQualityFactorS11(graph, graph_type, show_marker_text);       break;
		case GRAPH_TYPE_SMITH_S11:              drawSmithS11S21(graph, graph_type, false, 0, show_marker_text);  break;
		case GRAPH_TYPE_SMITH_S21:              drawSmithS11S21(graph, graph_type, false, 1, show_marker_text);  break;
		case GRAPH_TYPE_ADMITTANCE_S11:         drawSmithS11S21(graph, graph_type, true, 0, show_marker_text);   break;
		case GRAPH_TYPE_ADMITTANCE_S21:         drawSmithS11S21(graph, graph_type, true, 1, show_marker_text);   break;
		case GRAPH_TYPE_POLAR_S11:              drawPolarS11S21(graph, graph_type, 0, show_marker_text);         break;
		case GRAPH_TYPE_POLAR_S21:              drawPolarS11S21(graph, graph_type, 1, show_marker_text);         break;
		case GRAPH_TYPE_GROUP_DELAY_S11:        drawGroupDelayS11S21(graph, graph_type, 1, show_marker_text);    break;
		case GRAPH_TYPE_GROUP_DELAY_S21:        drawGroupDelayS11S21(graph, graph_type, 2, show_marker_text);    break;
		case GRAPH_TYPE_GROUP_DELAY_S11S21:     drawGroupDelayS11S21(graph, graph_type, 3, show_marker_text);    break;
		case GRAPH_TYPE_REAL_IMAG_S11:          drawRealImag(graph, graph_type, 0, show_marker_text);            break;
		case GRAPH_TYPE_REAL_IMAG_S21:          drawRealImag(graph, graph_type, 1, show_marker_text);            break;
		case GRAPH_TYPE_SERIES_CAPACITANCE_S11: drawQCS11(graph, graph_type, show_marker_text);                  break;
		case GRAPH_TYPE_SERIES_INDUCTANCE_S11:  drawQLS11(graph, graph_type, show_marker_text);                  break;
		case GRAPH_TYPE_COAX_LOSS_S11:          drawCoaxLossS11(graph, graph_type, show_marker_text);            break;
		case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11: drawTDRLinear(graph, graph_type, show_marker_text);              break;
		case GRAPH_TYPE_TDR_LIN_BP_S11:         drawTDRLinear(graph, graph_type, show_marker_text);              break;
		case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11: drawTDRLog(graph, graph_type, show_marker_text);                 break;
		case GRAPH_TYPE_TDR_LOG_BP_S11:         drawTDRLog(graph, graph_type, show_marker_text);                 break;
		case GRAPH_TYPE_TDR_IMPEDANCE_S11:      drawTDRImpedance(graph, graph_type, show_marker_text);           break;
		case GRAPH_TYPE_CAL_LOGMAG:             drawLogMagCalibrations(graph, graph_type, 31, show_marker_text); break;
		case GRAPH_TYPE_PHASE_VECTOR_S11:       drawPhaseVectorS11S21(graph, graph_type, 0, show_marker_text);   break;
		case GRAPH_TYPE_PHASE_VECTOR_S21:       drawPhaseVectorS11S21(graph, graph_type, 1, show_marker_text);   break;
		case GRAPH_TYPE_GJB_S11:                drawParallelRJX(graph, graph_type, 12, show_marker_text);        break;
		default: break;
	}
}

void __fastcall CGraphs::drawBorder()
{
	// gradient fill the paintbox edge

	uint8_t wred, wgrn, wblu;
	uint8_t bred, bgrn, bblu;
	const uint32_t window_colour     = colourToRGB(TStyleManager::ActiveStyle->GetStyleColor(scWindow), wred, wgrn, wblu);
	const uint32_t background_colour = colourToRGB(settings.m_colours.background, bred, bgrn, bblu);

	const int border_width = settings.borderWidth;

	if (background_colour != window_colour && border_width > 0)
	{
		#ifndef USE_OPENGL
			Gdiplus::Color outside_colour = Gdiplus::Color(255, wred, wgrn, wblu);
			int outside_colour_count = 1;

			{	// left edge
				Gdiplus::Point points[] = {
					Gdiplus::Point(0, 0),
					Gdiplus::Point(border_width, border_width),
					Gdiplus::Point(border_width, m_graph_bm->Height - border_width),
					Gdiplus::Point(0, m_graph_bm->Height)};
				Gdiplus::GraphicsPath path;
				path.AddLines(points, ARRAY_SIZE(points));
				Gdiplus::PathGradientBrush brush(&path);
				brush.SetCenterPoint(Gdiplus::Point(border_width, m_graph_bm->Height / 2));
				brush.SetCenterColor(Gdiplus::Color(255, bred, bgrn, bblu));
				brush.SetSurroundColors(&outside_colour, &outside_colour_count);
				brush.SetFocusScales(0.0f, 1.0f);
				m_gdi_plus->FillPath(&brush, &path);
			}

			{	// right edge
				Gdiplus::Point points[] = {
					Gdiplus::Point(m_graph_bm->Width - border_width, border_width),
					Gdiplus::Point(m_graph_bm->Width, 0),
					Gdiplus::Point(m_graph_bm->Width, m_graph_bm->Height),
					Gdiplus::Point(m_graph_bm->Width - border_width, m_graph_bm->Height - border_width)};
				Gdiplus::GraphicsPath path;
				path.AddLines(points, ARRAY_SIZE(points));
				Gdiplus::PathGradientBrush brush(&path);
				brush.SetCenterPoint(Gdiplus::Point(m_graph_bm->Width - border_width, m_graph_bm->Height / 2));
				brush.SetCenterColor(Gdiplus::Color(255, bred, bgrn, bblu));
				brush.SetSurroundColors(&outside_colour, &outside_colour_count);
				brush.SetFocusScales(0.0f, 1.0f);
				m_gdi_plus->FillPath(&brush, &path);
			}

			{	// top edge
				Gdiplus::Point points[] = {
					Gdiplus::Point(0, 0),
					Gdiplus::Point(m_graph_bm->Width, 0),
					Gdiplus::Point(m_graph_bm->Width - border_width, border_width),
					Gdiplus::Point(border_width, border_width)};
				Gdiplus::GraphicsPath path;
				path.AddLines(points, ARRAY_SIZE(points));
				Gdiplus::PathGradientBrush brush(&path);
				brush.SetCenterPoint(Gdiplus::Point(m_graph_bm->Width / 2, border_width));
				brush.SetCenterColor(Gdiplus::Color(255, bred, bgrn, bblu));
				brush.SetSurroundColors(&outside_colour, &outside_colour_count);
				brush.SetFocusScales(1.0f, 0.0f);
				m_gdi_plus->FillPath(&brush, &path);
			}

			{	// bottom edge
				Gdiplus::Point points[] = {
					Gdiplus::Point(0, m_graph_bm->Height),
					Gdiplus::Point(m_graph_bm->Width, m_graph_bm->Height),
					Gdiplus::Point(m_graph_bm->Width - border_width, m_graph_bm->Height - border_width),
					Gdiplus::Point(border_width, m_graph_bm->Height - border_width)};
				Gdiplus::GraphicsPath path;
				path.AddLines(points, ARRAY_SIZE(points));
				Gdiplus::PathGradientBrush brush(&path);
				brush.SetCenterPoint(Gdiplus::Point(m_graph_bm->Width / 2, m_graph_bm->Height - border_width));
				brush.SetCenterColor(Gdiplus::Color(255, bred, bgrn, bblu));
				brush.SetSurroundColors(&outside_colour, &outside_colour_count);
				brush.SetFocusScales(1.0f, 0.0f);
				m_gdi_plus->FillPath(&brush, &path);
			}
		#else
			const int view_width  = m_gl.win_control->ClientWidth;
			const int view_height = m_gl.win_control->ClientHeight;

			glEnable(GL_POLYGON_SMOOTH);
			glShadeModel(GL_SMOOTH);					// Enable smooth shading
			glBegin(GL_QUADS);
				// top edge
				glColor3ub(wred, wgrn, wblu);
				glVertex2f(0, 0);
				glVertex2f(view_width, 0);
				glColor3ub(bred, bgrn, bblu);
				glVertex2f(view_width - border_width, border_width);
				glVertex2f(border_width, border_width);

				// right edge
				glColor3ub(wred, wgrn, wblu);
				glVertex2f(view_width, 0);
				glVertex2f(view_width, view_height);
				glColor3ub(bred, bgrn, bblu);
				glVertex2f(view_width - border_width, view_height - border_width);
				glVertex2f(view_width - border_width, border_width);

				// bottom edge
				glColor3ub(wred, wgrn, wblu);
				glVertex2f(0, view_height);
				glVertex2f(view_width, view_height);
				glColor3ub(bred, bgrn, bblu);
				glVertex2f(view_width - border_width, view_height - border_width);
				glVertex2f(border_width, view_height - border_width);

				// left edge
				glColor3ub(wred, wgrn, wblu);
				glVertex2f(0, 0);
				glVertex2f(0, view_height);
				glColor3ub(bred, bgrn, bblu);
				glVertex2f(border_width, view_height - border_width);
				glVertex2f(border_width, border_width);
			glEnd();
			glShadeModel(GL_FLAT);
			glDisable(GL_POLYGON_SMOOTH);
		#endif
	}
}

void __fastcall CGraphs::resetAutoMinMaxCounters()
{
	for (int i = 0; i < GRAPH_TYPE_SIZE; i++)
	{
		settings.m_graph_setting[i].auto_max_hold_count = 0;
		settings.m_graph_setting[i].auto_min_hold_count = 0;
	}
}

#ifndef USE_OPENGL

void __fastcall CGraphs::onPaint(TObject *Sender,
											const bool show_marker_text,
											const int graph_type1,
											const int graph_type2,
											const int graph_type3,
											const int graph_type4,
											const int num_graphs,
											const int history_position)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	const int graph_type[MAX_GRAPHS] = {graph_type1, graph_type2, graph_type3, graph_type4};

	m_pb = pb;

	for (int graph = 0; graph < MAX_GRAPHS; graph++)
	{
		m_max[graph].channel = -1;
		m_max[graph].index   = -1;
		m_max[graph].value   = 0;

		m_min[graph].channel = -1;
		m_min[graph].index   = -1;
		m_min[graph].value   = 0;

		m_graph_pos[graph].gx  = 0;
		m_graph_pos[graph].gy  = 0;
		m_graph_pos[graph].gw  = 0;
		m_graph_pos[graph].gh  = 0;

		m_graph_pos[graph].cx = 0;
		m_graph_pos[graph].cy = 0;
		m_graph_pos[graph].cr = 0;
	}

	if (m_graph_bm == NULL)
	{
		pb->Canvas->Brush->Color = settings.m_colours.background;
		pb->Canvas->Brush->Style = bsSolid;
		pb->Canvas->FillRect(pb->Canvas->ClipRect);
		return;
	}

	if (m_graph_bm->Width != pb->Width || m_graph_bm->Height != pb->Height)
	{
		m_graph_bm->Width  = pb->Width;
		m_graph_bm->Height = pb->Height;
	}

	TFont *font              = m_graph_bm->Canvas->Font;
	m_graph_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : pb->Canvas->Font;

	// background colour
	m_graph_bm->Canvas->Pen->Width   = 1;
	m_graph_bm->Canvas->Pen->Style   = psSolid;
	m_graph_bm->Canvas->Pen->Color   = settings.m_colours.border;
	m_graph_bm->Canvas->Brush->Color = settings.m_colours.background;
	m_graph_bm->Canvas->Brush->Style = bsSolid;
	//m_graph_bm->Canvas->Rectangle(m_graph_bm->Canvas->ClipRect);	// with outer rectangle
	m_graph_bm->Canvas->FillRect(m_graph_bm->Canvas->ClipRect);		// without outer rectangle

	// computer the graph locations and size
	arrangeGraph(graph_type[0], graph_type[1], graph_type[2], graph_type[3], num_graphs);
/*
	if (!data_unit.validFrequencySettings())
	{	// nothing to plot
		pb->Canvas->CopyMode = cmSrcCopy;
		pb->Canvas->Draw(0, 0, m_graph_bm);
		return;
	}
*/
	if (m_gdi_plus)
		delete m_gdi_plus;
	m_gdi_plus = new Gdiplus::Graphics(m_graph_bm->Canvas->Handle);
	if (m_gdi_plus)
	{
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeNone);
		m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		//m_gdi_plus->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		m_gdi_plus->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		//m_gdi_plus->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

		m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
		//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
		//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityGammaCorrected);
		//m_gdi_plus->SetCompositingQuality(Gdiplus::CompositingQualityAssumeLinear);

		//m_gdi_plus->SetPixelOffsetMode(Gdiplus::PixelOffsetModeInvalid);
		//m_gdi_plus->SetPixelOffsetMode(Gdiplus::PixelOffsetModeDefault);
		//m_gdi_plus->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighSpeed);
		//m_gdi_plus->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
		m_gdi_plus->SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);
		//m_gdi_plus->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

	}

	fetchSamples(history_position);

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
		data_unit.m_point_filt[mem] = data_unit.m_point_mem[mem];

	// apply the S21 gain offset
	if (settings.s21OffsetdB != 0.0f && data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
	{
		const int mem     = 0;
		const int size    = data_unit.m_point_filt[mem].size();
		const float gain  = powf(10.0f, settings.s21OffsetdB / 20.0f);
		for (int i = 0; i < size; i++)
			data_unit.m_point_filt[mem][i].s21 *= gain;
	}

	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
	{
		// apply the e-delay (port extension) to the live memory
		if (settings.eDelaySecs != 0)
		{
			const int mem = 0;
			const int size = data_unit.m_point_filt[mem].size();
			const double phi = 2.0 * M_PI * settings.eDelaySecs;
			for (int i = 0; i < size; i++)
			{
				const double w = phi * (double)data_unit.m_point_filt[mem][i].Hz;
				const complexf ed(cosf(w), sinf(w));
				for (int channel = 0; channel < MAX_CHANNELS; channel++)
					data_unit.m_point_filt[mem][i].sParam[channel] *= ed;
			}
		}

		// calculate the inverse FFT for each channel
		for (int mem = 0; mem < MAX_MEMORIES; mem++)
		{
			const int size = data_unit.freqArraySize(mem);
			if (size > 0 && settings.memoryEnable[mem])
			{
				for (int i = 0; i < MAX_GRAPHS; i++)
					if (isTDRGraph(graph_type[i]) || graph_type[i] == GRAPH_TYPE_COAX_LOSS_S11)
						updateFFT(i, graph_type[i], mem);
			}
		}
	}

	// ***********

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_INFO_PANEL, 0, 0);

	for (int i = 0; i < num_graphs; i++)
	{
		// find out which part of the data is visible on screen
		computeVisibleIndexRange(i, graph_type[i]);

		drawGraph(i, graph_type[i], show_marker_text);
	}

	if (Form1 && data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
	{
		if (pb == Form1->GraphPaintBox)
		{	// main form graph (not a separate window graph)

			// first enabled memory that contains data
			const int mem = data_unit.firstUsedMem(true, 0);

			if (mem >= 0)
			{
				int smith_graph = -1;
				for (int graph = 0; graph < num_graphs; graph++)
				{
					if (isSmithGraph(graph_type[graph]) || isAdmittanceGraph(graph_type[graph]) || isPolarGraph(graph_type[graph]))
					{
						smith_graph = graph;
						break;
					}
				}

				if (smith_graph < 0)
					drawLCMatching(0, graph_type[0], mem);
				else
					drawLCMatching(smith_graph, graph_type[smith_graph], mem);
			}
		}
	}

	// ***********
	// draw the new image onto the paintbox

	drawBorder();

	pb->Canvas->CopyMode = cmSrcCopy;
	pb->Canvas->Draw(0, 0, m_graph_bm);

	m_graph_bm->Canvas->Font = font;

	if (m_gdi_plus)
		delete m_gdi_plus;
	m_gdi_plus = NULL;
}

#else

void __fastcall CGraphs::glRenderScene(TObject *Sender, const bool show_marker_text, const int graph_type1, const int graph_type2, const int graph_type3, const int graph_type4, const int num_graphs, const int history_position)
{
	GLenum err;

	if (!Form1)
		return;
	if (Form1->WindowState == wsMinimized)
		return;

	if (!m_gl.hdc || !m_gl.hrc || !m_gl.win_control || !m_graph_bm)
		return;

	m_pb    = NULL;
	m_form  = dynamic_cast<TForm *>(Sender);
	m_panel = dynamic_cast<TPanel *>(Sender);

	const int view_width  = m_gl.win_control->ClientWidth;
	const int view_height = m_gl.win_control->ClientHeight;

	const int graph_type[MAX_GRAPHS] = {graph_type1, graph_type2, graph_type3, graph_type4};

	for (int graph = 0; graph < MAX_GRAPHS; graph++)
	{
		m_max[graph].channel = -1;
		m_max[graph].index   = -1;
		m_max[graph].value   = 0;

		m_min[graph].channel = -1;
		m_min[graph].index   = -1;
		m_min[graph].value   = 0;

		m_graph_pos[graph].gx  = 0;
		m_graph_pos[graph].gy  = 0;
		m_graph_pos[graph].gw  = 0;
		m_graph_pos[graph].gh  = 0;

		m_graph_pos[graph].cx = 0;
		m_graph_pos[graph].cy = 0;
		m_graph_pos[graph].cr = 0;
	}

	if (m_graph_bm->Width != view_width || m_graph_bm->Height != view_height)
	{
		m_graph_bm->Width  = view_width;
		m_graph_bm->Height = view_height;
	}

	if (m_panel) m_graph_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : m_panel->Font;
	if (m_form)  m_graph_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : m_form->Font;
	//if (m_panel) m_graph_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : m_gl.win_control->Font;

	bool main_form = false;
	if (Form1)
	{
		if (m_panel) main_form = (m_panel == Form1->GLPanel) ? true : false;
		if (m_form)  main_form = (m_form  == Form1) ? true : false;
	}

	// computer the graph locations and size
	arrangeGraph(graph_type[0], graph_type[1], graph_type[2], graph_type[3], num_graphs);

//	if (data_unit.validFrequencySettings())
	{
		fetchSamples(history_position);

		for (int mem = 0; mem < MAX_MEMORIES; mem++)
			data_unit.m_point_filt[mem] = data_unit.m_point_mem[mem];

		// apply the S21 gain offset
		if (settings.s21OffsetdB != 0.0f && data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
		{
			const int mem     = 0;
			const int size    = data_unit.m_point_filt[mem].size();
			const float gain  = powf(10.0f, settings.s21OffsetdB / 20.0f);
			for (int i = 0; i < size; i++)
				data_unit.m_point_filt[mem][i].s21 *= gain;
		}

		if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
		{
			// apply the e-delay (port extension) to the live memory
			if (settings.eDelaySecs != 0)
			{
				const int mem = 0;
				const int size = data_unit.m_point_filt[mem].size();
				const double phi = 2.0 * M_PI * settings.eDelaySecs;
				for (int i = 0; i < size; i++)
				{
					const double w = phi * (double)data_unit.m_point_filt[mem][i].Hz;
					const complexf ed(cosf(w), sinf(w));
					for (int channel = 0; channel < MAX_CHANNELS; channel++)
						data_unit.m_point_filt[mem][i].sParam[channel] *= ed;
				}
			}

			// calculate the inverse FFT for each channel
			for (int mem = 0; mem < MAX_MEMORIES; mem++)
			{
				const int size = data_unit.freqArraySize(mem);
				if (size > 0 && settings.memoryEnable[mem])
				{
					for (int i = 0; i < MAX_GRAPHS; i++)
						if (isTDRGraph(graph_type[i]) || graph_type[i] == GRAPH_TYPE_COAX_LOSS_S11)
							updateFFT(i, graph_type[i], mem);
				}
			}
		}

		//	glResize(view_width, view_height);

		const HDC hOldDC   = wglGetCurrentDC();
		const HGLRC hOldRC = wglGetCurrentContext();

		if (wglMakeCurrent(m_gl.hdc, m_gl.hrc))
		{
			glLoadIdentity();
			err = glGetError();
			if (err != GL_NO_ERROR)
			{
				String s = glErrorStr(err);
			}

			GLrgba back_col = colourToGLcolour(settings.m_colours.background);
			glClearColor((float)back_col.r / 255, (float)back_col.g / 255, (float)back_col.b / 255, 1);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// if we in 3D mode
			//glClear(GL_COLOR_BUFFER_BIT);									// if we in 2D mode

			glFlush();

			if (m_gl.rebuild_fonts)
				glRebuildFonts();
/*
			// draw background quad
			//glEnable(GL_POLYGON_SMOOTH);
			//glShadeModel(GL_SMOOTH);					// Enable smooth shading
			glDisable(GL_POLYGON_SMOOTH);
			glShadeModel(GL_FLAT);
			glBegin(GL_QUADS);
				glColor3ub(back_col.r, back_col.g, back_col.b);
				glVertex2f(0, 0);
				glVertex2f(view_width, 0);
				glVertex2f(view_width, view_height);
				glVertex2f(0, view_height);
			glEnd();
*/
			for (int i = 0; i < num_graphs; i++)
			{
				// find out which part of the data is visible on screen
				computeVisibleIndexRange(i, graph_type[i]);
				drawGraph(i, graph_type[i], show_marker_text);
			}

			if (Form1 && data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
			{
				if (main_form)
				{	// main form graph (not a separate window graph)

					// first enabled memory that contains data
					const int mem = data_unit.firstUsedMem(true, 0);

					if (mem >= 0)
					{
						int smith_graph = -1;
						for (int graph = 0; graph < num_graphs; graph++)
						{
							if (isSmithGraph(graph_type[graph]) || isAdmittanceGraph(graph_type[graph]) || isPolarGraph(graph_type[graph]))
							{
								smith_graph = graph;
								break;
							}
						}
						if (smith_graph < 0)
							drawLCMatching(0, graph_type[0], mem);
						else
							drawLCMatching(smith_graph, graph_type[smith_graph], mem);
					}
				}
			}
/*
			{
				GLYPHMETRICSFLOAT gmf[256];

				glCullFace(GL_BACK);
				glEnable(GL_CULL_FACE);

				GLuint base = glGenLists(256);

				// create display lists for glyphs 0 through 255 with 0.1 extrusion and default deviation.
				// The display list numbering starts at 1000 (it could be any number)
				wglUseFontOutlines(m_gl.hdc, 0, 255, base, 0.0f, 0.1f, WGL_FONT_POLYGONS, gmf);

				glShadeModel(GL_SMOOTH);                // Enable Smooth Shading

				// Draw the characters in a string
				int x = 100;
				int y = 100;
				String s = "Hello Windows OpenGL World";
				glColor3ub(255, 255, 255);
				glPushAttrib(GL_LIST_BIT);
					glRasterPos2i(x, y);						// Position the text on the screen
					//glTranslated(x, y, 0);				// Position the text (0,0 - Bottom left)
					glListBase(base); 						// Indicates the start of display lists for the glyphs
					glCallLists(s.Length(), GL_UNSIGNED_BYTE, AnsiString(s).c_str());	// Draws the display list text
				glPopAttrib();

				glDeleteLists(base, 256);
			}
*/
			drawBorder();

			SwapBuffers(m_gl.hdc);

			//	glMatrixMode(GL_PROJECTION);
			//	glPopMatrix;
			//	glMatrixMode(GL_MODELVIEW);
			//	glPopMatrix;

			wglMakeCurrent(hOldDC, hOldRC);

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_INFO_PANEL, 0, 0);
		}
	}
}

#endif

