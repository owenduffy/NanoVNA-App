
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef GraphsH
#define GraphsH

//#define USE_OPENGL

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <Vcl.Graphics.hpp>

#ifdef USE_OPENGL
	#define GL_GLEXT_PROTOTYPES
	#define WGL_WGLEXT_PROTOTYPES
	//#pragma link "glut.lib"
	#include <windows.h>
//	#include <gl/glew.h>
	#include <gl/gl.h>
	#include <gl/glext.h>
	#include <gl/glu.h>
	#include <gl/glaux.h>
	#include <gl/wglext.h>
#endif

// GDI+ system
//#include <Unknwn.h>
//#include <windows.h>
#include <algorithm>
//using std::min;
//using std::max;
#pragma option push
	#pragma warn -8022
	#include <gdiplus.h>
//	#pragma link "gdiplus.lib"
//	#pragma comment (lib, "gdiplus.lib")
//	using namespace Gdiplus;
#pragma option pop

#include <vector>

#include "types.h"
#include "settings.h"
#include "common.h"
#include "median_filter.h"
#include "smoothing_filter.h"
#include "biquad_filter.h"
#include "tfft.h"

// ****************************************************

#ifdef USE_OPENGL

#pragma pack(push, 1)
struct GLrgb
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLrgb()
	{
		r = 0;
		g = 0;
		b = 0;
	}
	GLrgb(GLubyte red, GLubyte grn, GLubyte blu)
	{
		r = red;
		g = grn;
		b = blu;
	}
};
struct GLrgba
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
	GLrgba()
	{
		r = 0;
		g = 0;
		b = 0;
		a = 255;
	}
	GLrgba(GLubyte red, GLubyte grn, GLubyte blu)
	{
		r = red;
		g = grn;
		b = blu;
		a = 255;
	}
	GLrgba(GLubyte red, GLubyte grn, GLubyte blu, GLubyte alpha)
	{
		r = red;
		g = grn;
		b = blu;
		a = alpha;
	}
};
#pragma pack(pop)

#pragma pack(push, 4)	// make sure this number is the same size as a GLfloat!!!!!!!!!!!!!
struct GLvertex
{
	GLfloat X;
	GLfloat Y;
};
#pragma pack(pop)

struct glFont
{
	Graphics::TBitmap *bitmap;
	TFont  *font;
	int    height;
	GLuint base;
	GLuint base_start;
	GLYPHMETRICSFLOAT gmf[256];
};

#endif

// ****************************************************

class CGraphs
{
	private:
		Gdiplus::GdiplusStartupInput m_gdi_plus_startup_input;
		ULONG_PTR                    m_gdi_plus_token;
		Gdiplus::Graphics            *m_gdi_plus;

		TPaintBox *m_pb;
		TForm  *m_form;
		TPanel *m_panel;

		std::vector <Gdiplus::PointF> m_gdi_points;

		#ifdef USE_OPENGL
			struct
			{
				TWinControl           *win_control;
				HDC                   hdc;
				HGLRC                 hrc;
				int                   pixel_format;
				PIXELFORMATDESCRIPTOR pfd;
				std::vector <glFont>  font;
				bool                  rebuild_fonts;
			} m_gl;
		#endif

		CMedianFilter    median_filter;
		CSmoothingFilter smoothing_filter;
		CBiQuadFilter    biquad_filter;

		struct
		{
			int    graph_type;
			TColor vswr2_colour;
			TColor grid_colour;
			TColor back_colour;
			bool   both_scales;
			Graphics::TBitmap *bitmap;
			#ifdef USE_OPENGL
				GLuint bitmap_texture;
//				GLuint bitmap_pbo;
			#endif
		} m_grid[MAX_GRAPHS];

		#ifdef USE_OPENGL
			void __fastcall glRebuildFonts();
			int __fastcall glBuildFont(HDC hdc, String name, int size, bool bold);
			int __fastcall glTextWidth(int font_index, String s);
			int __fastcall glTextHeight(int font_index, String s);
			void glPrint(int font_index, GLint x, GLint y, const char *fmt, ...);
			void __fastcall glTextOut(int font_index, GLint x, GLint y, String s);
			void __fastcall glCircle(const float x, const float y, const float radius, const float deg_start, const float deg_end, const int segments, const bool fill);

			GLuint __fastcall createTexture(Graphics::TBitmap *bitmap, bool pixel_perfect);
			void __fastcall renderTexture(const TRect rect, const GLuint texture_id);

			GLrgba __fastcall colourToGLcolour(TColor colour);
		#endif

		uint32_t __fastcall colourToRGB(TColor colour, uint8_t &red, uint8_t &grn, uint8_t &blu);

		void __fastcall computeVisibleIndexRange(const int graph, const int graph_type);

		void __fastcall traceLPF(const int graph, const int graph_type, const float level, const int interations = 3);
		void __fastcall traceSmooth(const int graph, const int graph_type);

		void __fastcall computeTraceMinMax(const int graph, const int graph_type);
		void __fastcall computeCalTraceMinMax(const int graph, const int graph_type);

		void __fastcall computeGraphMinMax(const int graph, const int graph_type, double &max_levels, double &min_levels);

		void __fastcall drawMouseMarker(const int x, const int y, const int size);
		void __fastcall drawMarker(const int graph, const int x, const int y, const bool upsidedown_marker, const bool highlighted, const bool selected, String s1, String s2 = "", String s3 = "", String s4 = "", String s5 = "", String s6 = "");

		void __fastcall drawMarkersOnGraph(const int graph, const int mem, const int channel, String units_str, String id);
		void __fastcall drawMarkersOnGraph(const int graph, const int mem, const int channel1, const int channel2, String units_str, String id1, String id2);
		void __fastcall drawMarkers(const int graph, const int mem, const int channel, const float level_min, const float level_max, AnsiString units_str, const float gamma, const bool draw_v_line);
		void __fastcall drawMouse(const int graph, const int graph_type, const double level_min, const double level_max, const double gamma);

		void __fastcall drawMaxMarkers(const int graph, const int graph_type, const double level_min, const double level_max, const double gamma, String units, String id1, String id2);
		void __fastcall drawMinMarkers(const int graph, const int graph_type, const double level_min, const double level_max, const double gamma, String units, String id1, String id2);

		void __fastcall drawMarkersSmithPolar(const int graph, const int mem, const int channel, std::vector <t_pointf> &line_points, bool admittance, bool polar);
		void __fastcall drawMarkersOnSmithPolarGraph(const int graph, const int mem, const int channel, bool admittance, bool polar);
		void __fastcall drawMouseSmithPolar(const int graph, const int graph_type, const int mem, const int channel, std::vector <t_pointf> &line_points, bool admittance, bool polar);

		double __fastcall computeGraticuleScale(const double min_spacing, const double scale, double &step, int &mult_index);

		void __fastcall drawFreqLines(const int graph, const int graph_type);
		void __fastcall drawTimeDistLines(const int graph, const int mem);
		void __fastcall drawNoneLinMagLines(const int graph, const bool left_side, const double level_min, const double level_max, double gamma, const double initial_step_size, String fmt, String units, const bool is_vswr = false);
		void __fastcall drawMagLines(const int graph, const bool left_size, const double level_min, const double level_max, const double initial_step_size, String fmt, String units);

		void __fastcall drawSmithChart(const int graph, const int graph_type, const bool admittance, const float max_levels);
		void __fastcall drawPolarChart(const int graph, const int graph_type, const float max_level);

		void __fastcall drawPhaseVectorChart(const int graph, const int graph_type);

		void __fastcall drawLCMatching(const int graph, const int graph_type, const int mem);

		void __fastcall drawPoints(const int graph, const int graph_type, std::vector <t_pointf> &points_f, bool draw_spline, int line_width, bool show_points, TColor line_colour);
		void __fastcall drawGraphLines(const int graph, const int graph_type);

		void __fastcall drawDetails(const int graph, const int graph_type, String title);
		void __fastcall drawDetails(const int graph, const int graph_type, const uint8_t chan_mask, const int mem, const int index, String title, String units, String s[], String s_value[], String min_max_fmt);

		void __fastcall drawLogMagCalibrations(const int graph, const int graph_type, const uint8_t trace_mask, const bool show_marker_text);
		void __fastcall drawLogMagS11S21(const int graph, const int graph_type, const uint8_t chan_mask, const bool show_marker_text);
		void __fastcall drawLinMagS11S21(const int graph, const int graph_type, const uint8_t chan_mask, const bool show_marker_text);
		void __fastcall drawPhaseS11S21(const int graph, const int graph_type, const uint8_t chan_mask, const bool unwrap, const bool show_marker_text);
		void __fastcall drawVSWRReturnLossS11(const int graph, const int graph_type, const bool show_marker_text);
		void __fastcall drawImpedanceS11(const int graph, const int graph_type, const bool show_marker_text);
		void __fastcall drawQualityFactorS11(const int graph, const int graph_type, const bool show_marker_text);
		void __fastcall drawQCS11(const int graph, const int graph_type, const bool show_marker_text);
		void __fastcall drawQLS11(const int graph, const int graph_type, const bool show_marker_text);
		void __fastcall drawGroupDelayS11S21(const int graph, const int graph_type, const uint8_t chan_mask, const bool show_marker_text);
		void __fastcall drawSeriesRJX(const int graph, const int graph_type, const int mask, const bool show_marker_text);
		void __fastcall drawParallelRJX(const int graph, const int graph_type, const int mask, const bool show_marker_text, const bool gjb = false);
		void __fastcall drawRealImag(const int graph, const int graph_type, const int channel, const bool show_marker_text);
		void __fastcall drawCoaxLossS11(const int graph, const int graph_type, const bool show_marker_text);
		void __fastcall drawSmithS11S21(const int graph, const int graph_type, const bool admittance, const int s_channel, const bool show_marker_text);
		void __fastcall drawPolarS11S21(const int graph, const int graph_type, const int s_channel, const bool show_marker_text);
		void __fastcall drawPhaseVectorS11S21(const int graph, const int graph_type, const int s_channel, const bool show_marker_text);
		void __fastcall drawTDRLinear(const int graph, const int graph_type, const bool show_marker_text);
		void __fastcall drawTDRLog(const int graph, const int graph_type, const bool show_marker_text);
		void __fastcall drawTDRImpedance(const int graph, const int graph_type, const bool show_marker_text);

		void __fastcall updateFFT(const int graph, const int graph_type, const int mem);

		void __fastcall fetchSamples(int history_position);

		void __fastcall setGraphPos(const int graph, const int x, const int y, const int w, const int h, const int double_v_scale);
		void __fastcall arrangeGraph(const int graph_type1, const int graph_type2, const int graph_type3, const int graph_type4, const int num_graphs);

		void __fastcall drawBorder();

		void __fastcall drawGraph(const int graph, const int graph_type, const bool show_marker_text);

	public:
		__fastcall CGraphs();
		__fastcall ~CGraphs();

		TFFT m_fft[MAX_GRAPHS][MAX_MEMORIES];

		Graphics::TBitmap *m_graph_bm;

		float m_lpf_coeff;

		struct
		{
			int x;	// x
			int y;	// y
			int w;	// width
			int h;	// height
			int gx;	// graph area x
			int gy;	// graph area y
			int gw;	// graph area width
			int gh;	// graph area height
			int cx;	// graph area center x
			int cy;	// graph area center y
			int cr;	// graph area radius
		} m_graph_pos[MAX_GRAPHS];

		struct
		{
			int      x;
			int      y;
			int      graph;
			int      marker_mem;
			int      marker_index;
			int      point_mem;
			int      point_index;
			int      time_mem;
			int      time_index;
			double   Hz;
			double   secs;
			complexf cpx;
			float    mag;

			int      graph_type_select;

			int      graph_scale_pos;
			int      graph_v_scale;
			int      graph_h_scale;

			int      graph_scale_pos_drag;
			int      graph_v_scale_drag;
			int      graph_h_scale_drag;
			int      graph_drag;

			int      down_graph;
			double   down_Hz;
			double   down_max;
			double   down_min;
			double   down_val;
			int      down_x;
			int      down_y;
			int      down_marker;
		} m_mouse;

		//std::vector <float>    m_levels[MAX_MEMORIES][MAX_CHANNELS];
		//std::vector <t_pointf> m_line_points[MAX_GRAPHS][MAX_MEMORIES][MAX_CHANNELS];
		std::vector <float>      m_levels[MAX_MEMORIES][MAX_CAL_TRACES];
		std::vector <t_pointf>   m_line_points[MAX_GRAPHS][MAX_MEMORIES][MAX_CAL_TRACES];

		struct
		{
			int    channel;
			int    index;
			double value;
		} m_max[MAX_GRAPHS];

		struct
		{
			int    channel;
			int    index;
			double value;
		} m_min[MAX_GRAPHS];

		int m_min_visible_index[MAX_GRAPHS][MAX_MEMORIES];
		int m_max_visible_index[MAX_GRAPHS][MAX_MEMORIES];

		int __fastcall numberOfGraphs();

		bool __fastcall isFrequencyGraph(const int graph_type);
		bool __fastcall isSmithGraph(const int graph_type);
		bool __fastcall isAdmittanceGraph(const int graph_type);
		bool __fastcall isPolarGraph(const int graph_type);
		bool __fastcall isTDRGraph(const int graph_type);
		bool __fastcall isTimeGraph(const int graph_type);

		float __fastcall freqToX(const int graph, const int graph_type, const int64_t freq);
		int64_t __fastcall xyToFreq(const int graph, const int graph_type, int x, int y);
		bool __fastcall nearestPoint(const int graph, const int graph_type, const int x, const int y, const int max_pixel_dist, int &graph_num, int &mem, int &channel, int &index, int64_t &Hz, double &secs);

		void __fastcall resetAutoMinMaxCounters();

		#ifndef USE_OPENGL

			void __fastcall onPaint(TObject *Sender,
										const bool show_marker_text,
										const int graph_type1,
										const int graph_type2,
										const int graph_type3,
										const int graph_type4,
										const int num_graphs,
										const int history_position);

		#else

			void __fastcall glInit(TWinControl *win_control);
			void __fastcall glResize(const int width, const int height);
			void __fastcall glRenderScene(TObject *Sender, const bool show_marker_text, const int graph_type1, const int graph_type2, const int graph_type3, const int graph_type4, const int num_graphs, const int history_position);
			Graphics::TBitmap * __fastcall glCopyScreen();

			void __fastcall glQueuRebuildFonts()
			{
				m_gl.rebuild_fonts = true;
			}

		#endif
};

#endif

