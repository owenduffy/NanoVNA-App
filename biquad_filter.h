
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef biquad_filterH
#define biquad_filterH

#include "types.h"

/*
			// https://arachnoid.com/BiQuadDesigner/
			const float omega = (pi2 * frequency) / sample_rate;
			const float sn = sinf(omega);
			const float cs = cosf(omega);
			const float alpha = sn / (2 * q);
			const float norm = 1.0f / (1 + alpha);
			m_a0 =  norm;
			m_a1 =  (2 * cs)    * norm;
			m_a2 = -(1 - alpha) * norm;
			m_b0 =  alpha       * norm;
			m_b1 =  0           * norm;
			m_b2 = -alpha       * norm;

			// http://www.earlevel.com/main/2011/01/02/biquad-formulas
			// http://www.earlevel.com/main/2013/10/13/biquad-calculator-v2
			// http://www.earlevel.com/main/2016/12/08/filter-frequency-response-grapher

function calcBiquad(type, Fc, Fs, Q, peakGain, plotType) {
	var m_a0,m_a1,m_a2,m_b1,m_b2,norm;
	var ymin, ymax, minVal, maxVal;

	var V = Math.pow(10, Math.abs(peakGain) / 20);
	var K = Math.tan(Math.PI * Fc / Fs);

	switch (type) {
		case "one-pole lp":
			m_b1 = Math.exp(-2.0 * Math.PI * (Fc / Fs));
				m_a0 = 1.0 - m_b1;
				m_b1 = -m_b1;
			m_a1 = m_a2 = m_b2 = 0;
			break;

		case "one-pole hp":
			m_b1 = -Math.exp(-2.0 * Math.PI * (0.5 - Fc / Fs));
				m_a0 = 1.0 + m_b1;
				m_b1 = -m_b1;
			m_a1 = m_a2 = m_b2 = 0;
			break;

		case "lowpass":
			norm = 1 / (1 + K / Q + K * K);
			m_a0 = K * K * norm;
			m_a1 = 2 * m_a0;
			m_a2 = m_a0;
			m_b1 = 2 * (K * K - 1) * norm;
			m_b2 = (1 - K / Q + K * K) * norm;
			break;

		case "highpass":
			norm = 1 / (1 + K / Q + K * K);
			m_a0 = 1 * norm;
			m_a1 = -2 * m_a0;
			m_a2 = m_a0;
			m_b1 = 2 * (K * K - 1) * norm;
			m_b2 = (1 - K / Q + K * K) * norm;
			break;

		case "bandpass":
			norm = 1 / (1 + K / Q + K * K);
			m_a0 = K / Q * norm;
			m_a1 = 0;
			m_a2 = -m_a0;
			m_b1 = 2 * (K * K - 1) * norm;
			m_b2 = (1 - K / Q + K * K) * norm;
			break;

		case "notch":
			norm = 1 / (1 + K / Q + K * K);
			m_a0 = (1 + K * K) * norm;
			m_a1 = 2 * (K * K - 1) * norm;
			m_a2 = m_a0;
			m_b1 = m_a1;
			m_b2 = (1 - K / Q + K * K) * norm;
			break;

		case "peak":
			if (peakGain >= 0) {
				norm = 1 / (1 + 1 / Q * K + K * K);
				m_a0 = (1 + V/Q * K + K * K) * norm;
				m_a1 = 2 * (K * K - 1) * norm;
				m_a2 = (1 - V/Q * K + K * K) * norm;
				m_b1 = m_a1;
				m_b2 = (1 - 1/Q * K + K * K) * norm;
			}
			else {
				norm = 1 / (1 + V / Q * K + K * K);
				m_a0 = (1 + 1/Q * K + K * K) * norm;
				m_a1 = 2 * (K * K - 1) * norm;
				m_a2 = (1 - 1/Q * K + K * K) * norm;
				m_b1 = m_a1;
				m_b2 = (1 - V/Q * K + K * K) * norm;
			}
			break;

		case "lowShelf":
			if (peakGain >= 0) {
				norm = 1 / (1 + Math.SQRT2 * K + K * K);
				m_a0 = (1 + Math.sqrt(2*V) * K + V * K * K) * norm;
				m_a1 = 2 * (V * K * K - 1) * norm;
				m_a2 = (1 - Math.sqrt(2*V) * K + V * K * K) * norm;
				m_b1 = 2 * (K * K - 1) * norm;
				m_b2 = (1 - Math.SQRT2 * K + K * K) * norm;
			}
			else {
				norm = 1 / (1 + Math.sqrt(2*V) * K + V * K * K);
				m_a0 = (1 + Math.SQRT2 * K + K * K) * norm;
				m_a1 = 2 * (K * K - 1) * norm;
				m_a2 = (1 - Math.SQRT2 * K + K * K) * norm;
				m_b1 = 2 * (V * K * K - 1) * norm;
				m_b2 = (1 - Math.sqrt(2*V) * K + V * K * K) * norm;
			}
			break;

		case "highShelf":
				if (peakGain >= 0) {
					 norm = 1 / (1 + Math.SQRT2 * K + K * K);
					 m_a0 = (V + Math.sqrt(2*V) * K + K * K) * norm;
					 m_a1 = 2 * (K * K - V) * norm;
					 m_a2 = (V - Math.sqrt(2*V) * K + K * K) * norm;
					 m_b1 = 2 * (K * K - 1) * norm;
					 m_b2 = (1 - Math.SQRT2 * K + K * K) * norm;
				}
				else {
					 norm = 1 / (V + Math.sqrt(2*V) * K + K * K);
                m_a0 = (1 + Math.SQRT2 * K + K * K) * norm;
                m_a1 = 2 * (K * K - 1) * norm;
                m_a2 = (1 - Math.SQRT2 * K + K * K) * norm;
                m_b1 = 2 * (K * K - V) * norm;
                m_b2 = (V - Math.sqrt(2*V) * K + K * K) * norm;
			}
			break;
	}

	var len = 512;
	var magPlot = [];
	for (var idx = 0; idx < len; idx++) {
		var w;
		if (plotType == "linear")
			w = idx / (len - 1) * Math.PI;	// 0 to pi, linear scale
		else
			w = Math.exp(Math.log(1 / 0.001) * idx / (len - 1)) * 0.001 * Math.PI;	// 0.001 to 1, times pi, log scale

		var phi = Math.pow(Math.sin(w/2), 2);
		var y = Math.log(Math.pow(m_a0+m_a1+m_a2, 2) - 4*(m_a0*m_a1 + 4*m_a0*m_a2 + m_a1*m_a2)*phi + 16*m_a0*m_a2*phi*phi) - Math.log(Math.pow(1+m_b1+m_b2, 2) - 4*(m_b1 + 4*m_b2 + m_b1*m_b2)*phi + 16*m_b2*phi*phi);
		y = y * 10 / Math.LN10
		if (y == -Infinity)
			y = -200;

		if (plotType == "linear")
			magPlot.push([idx / (len - 1) * Fs / 2, y]);
		else
			magPlot.push([idx / (len - 1) / 2, y]);

		if (idx == 0)
			minVal = maxVal = y;
		else if (y < minVal)
			minVal = y;
		else if (y > maxVal)
			maxVal = y;
	}

	// configure y-axis
	switch (type) {
		default:
		case "lowpass":
		case "highpass":
		case "bandpass":
		case "notch":
			ymin = -100;
			ymax = 0;
			if (maxVal > ymax)
				ymax = maxVal;
			break;
		case "peak":
		case "lowShelf":
		case "highShelf":
			ymin = -10;
			ymax = 10;
			if (maxVal > ymax)
				ymax = maxVal;
			else if (minVal < ymin)
				ymin = minVal;
			break;
        case "one-pole lp":
        case "one-pole hp":
			ymin = -40;
			ymax = 0;
            break;
	}
	if (plotType == "linear")
		Flotr.draw(document.getElementById('container-20131013'), [ magPlot], { yaxis: { max: ymax, min: ymin} });
	else
		Flotr.draw(document.getElementById('container-20131013'), [ magPlot], { yaxis: { max: ymax, min: ymin}, xaxis: {tickFormatter: nullTickFormatter} });

	// list coefficients
	var coefsList = "m_a0 = " + m_a0 + "\n";
	coefsList += "m_a1 = " + m_a1 + "\n";
	coefsList += "m_a2 = " + m_a2 + "\n";
	coefsList += "m_b1 = " + m_b1 + "\n";
	coefsList += "m_b2 = " + m_b2;
	 var taNode = document.getElementById("biquad_coefsList");
	 // remove existing child txt node
	 while (taNode.firstChild)
		taNode.removeChild(taNode.firstChild);
	 var listNode = document.createTextNode(coefsList);
	 taNode.appendChild(listNode);
}
*/

class CBiQuadFilter
{
	private:
		float m_sample_rate_hz;
		float m_freq_hz;
		float m_q;

		// zeros
		float m_a0;
		float m_a1;
		float m_a2;

		// poles
		float m_b1;
		float m_b2;

		// memory
		float m_z1;
		float m_z2;
		complexf m_cz1;
		complexf m_cz2;

		int __fastcall biquadUpdateSplane(std::vector <float> &q_list, int order)
		{	// calculate the Q values when cascading biquads
			q_list.resize(0);

			if (order > 0)
			{
				const int pairs      = order >> 1;
				const int odd_poles  = order & 1;
				const float pole_inc = (float)M_PI / order;

				// show coefficients
				float first_angle = pole_inc;
				if (!odd_poles)
					first_angle /= 2;
				else
					q_list.push_back(0.5f);	// "0.5 (one-pole)\n";

				for (int i = 0; i < pairs; i++)
				{
					const float qVal = 1.0f / (2.0f * cosf(first_angle + (pole_inc * i)));
					q_list.push_back(qVal);
				}
			}

			return q_list.size();
		}

/*
			// plot frequency response
			var len = 512;
			var magPlot = [];
			for (var idx = 0; idx < len; idx++)
			{
				var w;
				if (plotType == "linear")
					w = idx / (len - 1) * Math.PI;	// 0 to pi, linear scale
				else
					w = Math.exp(Math.log(1 / 0.001) * idx / (len - 1)) * 0.001 * Math.PI;	// 0.001 to 1, times pi, log scale

				var phi = Math.pow(Math.sin(w / 2), 2);
				var y = Math.log(Math.pow(m_a0 + m_a1 + m_a2, 2) - 4*(m_a0*m_a1 + 4*m_a0*m_a2 + m_a1*m_a2)*phi + 16*m_a0*m_a2*phi*phi) - Math.log(Math.pow(1+m_b1+m_b2, 2) - 4*(m_b1 + 4*m_b2 + m_b1*m_b2)*phi + 16*m_b2*phi*phi);
				y = y * 10 / Math.LN10
				if (y == -Infinity)
					y = -200;

				if (plotType == "linear")
					magPlot.push([idx / (len - 1) * Fs / 2, y]);
				else
					magPlot.push([idx / (len - 1) / 2, y]);

				if (idx == 0)
					minVal = maxVal = y;
				else if (y < minVal)
					minVal = y;
				else if (y > maxVal)
					maxVal = y;
			}
*/

	public:
		CBiQuadFilter()
		{
			m_a0 = 0.0f;
			m_a1 = 0.0f;
			m_a2 = 0.0f;

			m_b1 = 0.0f;
			m_b2 = 0.0f;

			reset();
		}

		void __fastcall reset()
		{
			m_z1 = 0.0f;
			m_z2 = 0.0f;

			m_cz1 = 0;
			m_cz2 = 0;
		}

		void __fastcall makeLowPass(const float sample_rate, float frequency, float q)
		{
			reset();

			if (frequency < 0.0f)
				 frequency = 0.0f;
			else
			if (frequency > sample_rate / 2)
				 frequency = sample_rate / 2;

			if (q < 0.01f)
				q = 0.01f;

			const float k = tanf(((float)M_PI * frequency) / sample_rate);
			const float kq = k / q;
			const float kk = k * k;
			const float norm = 1.0f / (1.0f + kq + kk);

			m_sample_rate_hz = sample_rate;
			m_freq_hz = frequency;
			m_q = q;

			m_a0 = kk * norm;
			m_a1 = 2.0f * m_a0;
			m_a2 = m_a0;

			m_b1 = -2.0f * (kk - 1.0f) * norm;
			m_b2 = -(1.0f - kq + kk) * norm;
		}


		void __fastcall makeHighPass(const float sample_rate, float corner_frequency, float q)
		{
			reset();

			if (corner_frequency < 0.0f)
				 corner_frequency = 0.0f;
			else
			if (corner_frequency > sample_rate / 2)
				 corner_frequency = sample_rate / 2;

			if (q < 0.01f)
				q = 0.01f;

			const float k = tanf(((float)M_PI * corner_frequency) / sample_rate);
			const float kq = k / q;
			const float kk = k * k;
			const float norm = 1.0f / (1.0f + kq + kk);

			m_sample_rate_hz = sample_rate;
			m_freq_hz = corner_frequency;
			m_q = q;

			m_a0 =  1.0f * norm;
			m_a1 = -2.0f * m_a0;
			m_a2 = m_a0;

			m_b1 =  -2.0f * (kk - 1) * norm;
			m_b2 = -(1.0f - kq + kk) * norm;
		}

		void __fastcall makeBandPass(const float sample_rate, float frequency, float q)
		{
			reset();

			if (frequency < 0.0f)
				 frequency = 0.0f;
			else
			if (frequency > sample_rate / 2)
				 frequency = sample_rate / 2;

			if (q < 0.01f)
				q = 0.01f;

			const float k = tanf(((float)M_PI * frequency) / sample_rate);
			const float kq = k / q;
			const float kk = k * k;
			const float norm = 1.0f / (1.0f + kq + kk);

			m_sample_rate_hz = sample_rate;
			m_freq_hz = frequency;
			m_q = q;

			m_a0 = kq * norm;
			m_a1 = 0.0f;
			m_a2 = -m_a0;

			m_b1 = -2.0f * (kk - 1.0f) * norm;
			m_b2 = -(1.0f - kq + kk) * norm;
		}

		void __fastcall makeNotch(const float sample_rate, float frequency, float q)
		{
			reset();

			if (frequency < 0.0f)
				 frequency = 0.0f;
			else
			if (frequency > sample_rate / 2)
				 frequency = sample_rate / 2;

			if (q < 0.01f)
				q = 0.01f;

			const float k = tanf(((float)M_PI * frequency) / sample_rate);
			const float kq = k / q;
			const float kk = k * k;
			const float norm = 1.0f / (1.0f + kq + kk);

			m_sample_rate_hz = sample_rate;
			m_freq_hz = frequency;
			m_q = q;

			m_a0 = (1.0f + kk) * norm;
			m_a1 = 2.0f * (kk - 1.0f) * norm;
			m_a2 = m_a0;

			m_b1 = -m_a1;
			m_b2 = -(1.0f - kq + kk) * norm;
		}

		void __fastcall makePeak(const float sample_rate, float frequency, float q, float peak_gain_dB)
		{
			reset();

			if (peak_gain_dB == 0.0f)
				return;

			if (frequency < 0.0f)
				 frequency = 0.0f;
			else
			if (frequency > sample_rate / 2)
				 frequency = sample_rate / 2;

			if (q < 0.01f)
				q = 0.01f;

			const float v = powf(10.0f, fabsf(peak_gain_dB) / 20);
			const float k = tanf(((float)M_PI * frequency) / sample_rate);
			const float kq = k / q;
			const float kk = k * k;

			m_sample_rate_hz = sample_rate;
			m_freq_hz = frequency;
			m_q = q;

			if (peak_gain_dB >= 0.0f)
			{
				const float norm = 1.0f / (1.0f + kq + kk);

				m_a0 = (1.0f + (v * kq) + kk) * norm;
				m_a1 = 2.0f * (kk - 1.0f) * norm;
				m_a2 = (1.0f - (v * kq) + kk) * norm;

				m_b1 = -m_a1;
				m_b2 = -(1.0f - kq + kk) * norm;
			}
			else
			{
				const float norm = 1.0f / (1.0f + (v * kq) + kk);

				m_a0 = (1.0f + kq + kk) * norm;
				m_a1 = 2.0f * (kk - 1.0f) * norm;
				m_a2 = (1.0f - kq + kk) * norm;

				m_b1 = -m_a1;
				m_b2 = -(1.0f - (v * kq) + kk) * norm;
			}
		}

		void __fastcall prime(const float x)
		{
			m_z2 = x - (m_a0 * x);
			m_z1 = m_z2 - ((m_a1 * x) + (m_b1 * x));
		}

		void __fastcall prime(const complexf x)
		{
			m_cz2 = x - (x * m_a0);
			m_cz1 = m_cz2 - ((x * m_a1) + (x * m_b1));
		}

		float __fastcall process(const float x)
		{
			// IIR Direct form 2
			//const float z = (m_z2 * m_b2) + (m_z1 * m_b1) + x;
			//const float y = (m_z2 * m_a2) + (m_z1 * m_a1) + (z * m_a0);
			//m_z2 = m_z1;
			//m_z1 = z;

			// IIR Transposed Direct form 2
			const float y = (m_a0 * x) + m_z2;
			m_z2 = (m_a1 * x) + (m_b1 * y) + m_z1;
			m_z1 = (m_a2 * x) + (m_b2 * y);

			return y;
		}

		complexf __fastcall process(const complexf x)
		{
			// IIR Direct form 2
			//const complexf z = (m_cz2 * m_b2) + (m_cz1 * m_b1) + x;
			//const complexf y = (m_cz2 * m_a2) + (m_cz1 * m_a1) + (z * m_a0);
			//m_cz2 = m_cz1;
			//m_cz1 = z;

			// IIR Transposed Direct form 2
			const complexf y = (x * m_a0) + m_cz2;
			m_cz2 = (x * m_a1) + (y * m_b1) + m_cz1;
			m_cz1 = (x * m_a2) + (y * m_b2);

			return y;
		}
};

#endif

