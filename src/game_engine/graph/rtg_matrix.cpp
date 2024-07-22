#include "rtg_graph_inter.h"
#include <sstream>
#include <iomanip> // For std::setprecision
namespace rt_graph {
	//
	// Implementation of class RtgMatrix12
	//

	RtgMatrix12 RtgMatrix12::matIdentity = RtgMatrix12
	(
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 0.f
	);

	RtgMatrix12::RtgMatrix12(const RtgMatrix12& rhs)
	{
		*this = rhs;
	}

	RtgMatrix12::RtgMatrix12(const float m[12])
	{
		m_m[0] = m[0];  m_m[1] = m[1];   m_m[2] = m[2];
		m_m[3] = m[3];  m_m[4] = m[4];   m_m[5] = m[5];
		m_m[6] = m[6];  m_m[7] = m[7];   m_m[8] = m[8];
		m_m[9] = m[9];  m_m[10] = m[10];  m_m[11] = m[11];
	}

	RtgMatrix12::RtgMatrix12(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22, float _30, float _31, float _32)
	{
		this->_00 = _00; this->_01 = _01; this->_02 = _02;
		this->_10 = _10; this->_11 = _11; this->_12 = _12;
		this->_20 = _20; this->_21 = _21; this->_22 = _22;
		this->_30 = _30; this->_31 = _31; this->_32 = _32;
	}


	RtgMatrix12::RtgMatrix12(const float face[3], const float up[3], const float right[3], const float pos[3]) // tDAN 0831, 2001
	{
		if (right) { m_m[0] = right[0];  m_m[1] = right[1];   m_m[2] = right[2]; }
		else
		{
			float v[3];
			rtgVectorCross(v, face, up);
			m_m[0] = v[0];  m_m[1] = v[1];   m_m[2] = v[2];
		}

		m_m[3] = face[0];   m_m[4] = face[1];   m_m[5] = face[2];
		m_m[6] = up[0];     m_m[7] = up[1];     m_m[8] = up[2];

		if (pos) { m_m[9] = pos[0];  m_m[10] = pos[1];  m_m[11] = pos[2]; }
		else { m_m[9] = 0.0f;  m_m[10] = 0.0f;  m_m[11] = 0.0f; }

		NormalizeMatrix();
	}

	void RtgMatrix12::Zero(void)
	{
		m_m[0] = m_m[1] = m_m[2] =
			m_m[3] = m_m[4] = m_m[5] =
			m_m[6] = m_m[7] = m_m[8] =
			m_m[9] = m_m[10] = m_m[11] = 0.0f;
	}

	void RtgMatrix12::Unit(void)
	{
		m_m[1] = m_m[2] = m_m[3] = m_m[5] = m_m[6] = m_m[7] =
			m_m[9] = m_m[10] = m_m[11] = 0.0f;
		m_m[0] = m_m[4] = m_m[8] = 1.0f;
	}


	void RtgMatrix12::NormalizeMatrix(void)
	{
		float l;
		l = fsqrt(_00 * _00 + _01 * _01 + _02 * _02);
		_00 /= l;
		_01 /= l;
		_02 /= l;

		l = fsqrt(_10 * _10 + _11 * _11 + _12 * _12);
		_10 /= l;
		_11 /= l;
		_12 /= l;

		l = fsqrt(_20 * _20 + _21 * _21 + _22 * _22);
		_20 /= l;
		_21 /= l;
		_22 /= l;
	}


	RtgMatrix12 RtgMatrix12::operator+(const RtgMatrix12& rhs)
	{
		RtgMatrix12 M;
		M[0] = m_m[0] + rhs[0];  M[1] = m_m[1] + rhs[1];  M[2] = m_m[2] + rhs[2];
		M[3] = m_m[3] + rhs[3];  M[4] = m_m[4] + rhs[4];  M[5] = m_m[5] + rhs[5];
		M[6] = m_m[6] + rhs[6];  M[7] = m_m[7] + rhs[7];  M[8] = m_m[8] + rhs[8];
		M[9] = m_m[9] + rhs[9];  M[10] = m_m[10] + rhs[10]; M[11] = m_m[11] + rhs[11];
		return M;
	}

	RtgMatrix12 RtgMatrix12::operator-(const RtgMatrix12& rhs)
	{
		RtgMatrix12 M;
		M[0] = m_m[0] - rhs[0];  M[1] = m_m[1] - rhs[1];  M[2] = m_m[2] - rhs[2];
		M[3] = m_m[3] - rhs[3];  M[4] = m_m[4] - rhs[4];  M[5] = m_m[5] - rhs[5];
		M[6] = m_m[6] - rhs[6];  M[7] = m_m[7] - rhs[7];  M[8] = m_m[8] - rhs[8];
		M[9] = m_m[9] - rhs[9];  M[10] = m_m[10] - rhs[10]; M[11] = m_m[11] - rhs[11];
		return M;
	}

	RtgMatrix12 RtgMatrix12::operator+(const float m[12])
	{
		RtgMatrix12 M;
		M[0] = m_m[0] + m[0];  M[1] = m_m[1] + m[1];  M[2] = m_m[2] + m[2];
		M[3] = m_m[3] + m[3];  M[4] = m_m[4] + m[4];  M[5] = m_m[5] + m[5];
		M[6] = m_m[6] + m[6];  M[7] = m_m[7] + m[7];  M[8] = m_m[8] + m[8];
		M[9] = m_m[9] + m[9];  M[10] = m_m[10] + m[10]; M[11] = m_m[11] + m[11];
		return M;
	}

	RtgMatrix12 RtgMatrix12::operator-(const float m[12])
	{
		RtgMatrix12 M;
		M[0] = m_m[0] - m[0];  M[1] = m_m[1] - m[1];  M[2] = m_m[2] - m[2];
		M[3] = m_m[3] - m[3];  M[4] = m_m[4] - m[4];  M[5] = m_m[5] - m[5];
		M[6] = m_m[6] - m[6];  M[7] = m_m[7] - m[7];  M[8] = m_m[8] - m[8];
		M[9] = m_m[9] - m[9];  M[10] = m_m[10] - m[10]; M[11] = m_m[11] - m[11];
		return M;
	}

	RtgMatrix12& RtgMatrix12::operator+=(const RtgMatrix12& rhs)
	{
		m_m[0] += rhs[0];  m_m[1] += rhs[1];  m_m[2] += rhs[2];
		m_m[3] += rhs[3];  m_m[4] += rhs[4];  m_m[5] += rhs[5];
		m_m[6] += rhs[6];  m_m[7] += rhs[7];  m_m[8] += rhs[8];
		m_m[9] += rhs[9];  m_m[10] += rhs[10]; m_m[11] += rhs[11];
		return *this;
	}

	RtgMatrix12& RtgMatrix12::operator-=(const RtgMatrix12& rhs)
	{
		m_m[0] -= rhs[0];  m_m[1] -= rhs[1];  m_m[2] -= rhs[2];
		m_m[3] -= rhs[3];  m_m[4] -= rhs[4];  m_m[5] -= rhs[5];
		m_m[6] -= rhs[6];  m_m[7] -= rhs[7];  m_m[8] -= rhs[8];
		m_m[9] -= rhs[9];  m_m[10] -= rhs[10]; m_m[11] -= rhs[11];
		return *this;
	}

	RtgMatrix12& RtgMatrix12::operator+=(const float m[12])
	{
		m_m[0] += m[0];  m_m[1] += m[1];  m_m[2] += m[2];
		m_m[3] += m[3];  m_m[4] += m[4];  m_m[5] += m[5];
		m_m[6] += m[6];  m_m[7] += m[7];  m_m[8] += m[8];
		m_m[9] += m[9];  m_m[10] += m[10]; m_m[11] += m[11];
		return *this;
	}

	RtgMatrix12& RtgMatrix12::operator-=(const float m[12])
	{
		m_m[0] -= m[0];  m_m[1] -= m[1];  m_m[2] -= m[2];
		m_m[3] -= m[3];  m_m[4] -= m[4];  m_m[5] -= m[5];
		m_m[6] -= m[6];  m_m[7] -= m[7];  m_m[8] -= m[8];
		m_m[9] -= m[9];  m_m[10] -= m[10]; m_m[11] -= m[11];
		return *this;
	}

	RtgMatrix12 RtgMatrix12::operator*(const RtgMatrix12& rhs) const
	{
		RtgMatrix12 M;
		M[0] = m_m[0] * rhs[0] + m_m[1] * rhs[3] + m_m[2] * rhs[6];
		M[1] = m_m[0] * rhs[1] + m_m[1] * rhs[4] + m_m[2] * rhs[7];
		M[2] = m_m[0] * rhs[2] + m_m[1] * rhs[5] + m_m[2] * rhs[8];
		M[3] = m_m[3] * rhs[0] + m_m[4] * rhs[3] + m_m[5] * rhs[6];
		M[4] = m_m[3] * rhs[1] + m_m[4] * rhs[4] + m_m[5] * rhs[7];
		M[5] = m_m[3] * rhs[2] + m_m[4] * rhs[5] + m_m[5] * rhs[8];
		M[6] = m_m[6] * rhs[0] + m_m[7] * rhs[3] + m_m[8] * rhs[6];
		M[7] = m_m[6] * rhs[1] + m_m[7] * rhs[4] + m_m[8] * rhs[7];
		M[8] = m_m[6] * rhs[2] + m_m[7] * rhs[5] + m_m[8] * rhs[8];
		M[9] = m_m[9] * rhs[0] + m_m[10] * rhs[3] + m_m[11] * rhs[6] + rhs[9];
		M[10] = m_m[9] * rhs[1] + m_m[10] * rhs[4] + m_m[11] * rhs[7] + rhs[10];
		M[11] = m_m[9] * rhs[2] + m_m[10] * rhs[5] + m_m[11] * rhs[8] + rhs[11];
		return M;
	}

	RtgMatrix12 RtgMatrix12::operator*(float f)
	{
		RtgMatrix12 M;
		M[0] = m_m[0] * f;  M[1] = m_m[1] * f;  M[2] = m_m[2] * f;
		M[3] = m_m[3] * f;  M[4] = m_m[4] * f;  M[5] = m_m[5] * f;
		M[6] = m_m[6] * f;  M[7] = m_m[7] * f;  M[8] = m_m[8] * f;
		M[9] = m_m[9] * f;  M[10] = m_m[10] * f;  M[11] = m_m[11] * f;
		return M;
	}

	RtgMatrix12 RtgMatrix12::operator/(float f)
	{
		CHECK(f != 0.0f);

		RtgMatrix12 M;
		M[0] = m_m[0] / f;  M[1] = m_m[1] / f;  M[2] = m_m[2] / f;
		M[3] = m_m[3] / f;  M[4] = m_m[4] / f;  M[5] = m_m[5] / f;
		M[6] = m_m[6] / f;  M[7] = m_m[7] / f;  M[8] = m_m[8] / f;
		M[9] = m_m[9] / f;  M[10] = m_m[10] / f;  M[11] = m_m[11] / f;
		return M;
	}

	RtgMatrix12 RtgMatrix12::operator*=(float f)
	{
		m_m[0] *= f;  m_m[1] *= f;  m_m[2] *= f;
		m_m[3] *= f;  m_m[4] *= f;  m_m[5] *= f;
		m_m[6] *= f;  m_m[7] *= f;  m_m[8] *= f;
		m_m[9] *= f;  m_m[10] *= f;  m_m[11] *= f;
		return *this;
	}

	RtgMatrix12 RtgMatrix12::operator/=(float f)
	{
		CHECK(f != 0.0f);

		m_m[0] /= f;  m_m[1] /= f;  m_m[2] /= f;
		m_m[3] /= f;  m_m[4] /= f;  m_m[5] /= f;
		m_m[6] /= f;  m_m[7] /= f;  m_m[8] /= f;
		m_m[9] /= f;  m_m[10] /= f;  m_m[11] /= f;
		return *this;
	}

	RtgMatrix12& RtgMatrix12::operator=(const RtgMatrix12& rhs)
	{
		_00 = rhs._00;  _01 = rhs._01;  _02 = rhs._02;
		_10 = rhs._10;  _11 = rhs._11;  _12 = rhs._12;
		_20 = rhs._20;  _21 = rhs._21;  _22 = rhs._22;
		_30 = rhs._30;  _31 = rhs._31;  _32 = rhs._32;
		return *this;
	}

	RtgMatrix12& RtgMatrix12::operator= (const RtgMatrix16& rhs)
	{
		_00 = rhs._00;  _01 = rhs._01;  _02 = rhs._02;
		_10 = rhs._10;  _11 = rhs._11;  _12 = rhs._12;
		_20 = rhs._20;  _21 = rhs._21;  _22 = rhs._22;
		_30 = rhs._30;  _31 = rhs._31;  _32 = rhs._32;
		return *this;
	}

	RtgMatrix12& RtgMatrix12::operator=(const float m[12])
	{
		m_m[0] = m[0];  m_m[1] = m[1];  m_m[2] = m[2];
		m_m[3] = m[3];  m_m[4] = m[4];  m_m[5] = m[5];
		m_m[6] = m[6];  m_m[7] = m[7];  m_m[8] = m[8];
		m_m[9] = m[9];  m_m[10] = m[10]; m_m[11] = m[11];
		return *this;
	}

	void  RtgMatrix12::Dump(float m[12]) const
	{
		for (int i = 0; i < 12; i++) {
			m[i] = m_m[i];
		}
	}

	std::string RtgMatrix12::ToString() const
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(6); // 设置浮点数的精度
		oss << _00 << "," << _01 << "," << _02 << ",\n";
		oss << _10 << "," << _11 << "," << _12 << ",\n";
		oss << _20 << "," << _21 << "," << _22 << ",\n";
		oss << _30 << "," << _31 << "," << _32 << "\n";
		return oss.str();

	}

	RtgMatrix12 RtgMatrix12::Inverse(void) const
	{
		int i, j, indx[4];
		float tmp[16], column[4], d;
		RtgMatrix12 m12;

		// Backup the matrix for decomposition
		rtgM16AssignM12(tmp, (float*)this);

		// Perform LU decomposition
		rtgM16LUDecomposition(tmp, indx, &d, 4);

		// Backward substitute to get the answer
		for (j = 0; j < 4; j++) {
			for (i = 0; i < 4; i++)
				column[i] = 0.0f;
			column[j] = 1.0f;

			rtgM16LUBackSubstitute(tmp, indx, column, 4);

			if (j != 3) {
				for (i = 0; i < 4; i++) {
					m12[i * 3 + j] = column[i];
				}
			}
		}

		return m12;
	}


	// counterclockwise rotate matrix about local X-axis
	// [0]  1  [1]  0   [2]  0
	// [3]  0  [4] cos  [5] sin   * m
	// [6]  0  [7]-sin  [8] cos
	// [9]  0 [10]  0  [11]  0
	void RtgMatrix12::RotateLX(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float m3 = m_m[3], m4 = m_m[4], m5 = m_m[5];
		//m_m[0] = m_m[0];
		//m_m[1] = m_m[1];
		//m_m[2] = m_m[2];
		m_m[3] = ca * m3 + sa * m_m[6];
		m_m[4] = ca * m4 + sa * m_m[7];
		m_m[5] = ca * m5 + sa * m_m[8];
		m_m[6] = -sa * m3 + ca * m_m[6];
		m_m[7] = -sa * m4 + ca * m_m[7];
		m_m[8] = -sa * m5 + ca * m_m[8];
		//m_m[9] = m_m[9];
		//m_m[10] = m_m[10];
		//m_m[11] = m_m[11];
	}

	// counterclockwise rotate matrix about local Y-axis
	// [0] cos  [1]  0  [2]-sin
	// [3]  0   [4]  1  [5]  0   * m
	// [6] sin  [7]  0  [8] cos
	// [9]  0  [10]  0 [11]  0
	void RtgMatrix12::RotateLY(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float m0 = m_m[0], m1 = m_m[1], m2 = m_m[2];
		m_m[0] = ca * m0 - sa * m_m[6];
		m_m[1] = ca * m1 - sa * m_m[7];
		m_m[2] = ca * m2 - sa * m_m[8];
		//m_m[3] = m_m[3];
		//m_m[4] = m_m[4];
		//m_m[5] = m_m[5];
		m_m[6] = sa * m0 + ca * m_m[6];
		m_m[7] = sa * m1 + ca * m_m[7];
		m_m[8] = sa * m2 + ca * m_m[8];
		//m_m[9] = m_m[9];
		//m_m[10] = m_m[10];
		//m_m[11] = m_m[11];
	}

	// counterclockwise rotate matrix about local Z-axis
	// [0] cos  [1] sin [2]  0
	// [3]-sin  [4] cos [5]  0   * m 
	// [6]  0   [7]  0  [8]  1
	// [9]  0  [10]  0 [11]  0
	void RtgMatrix12::RotateLZ(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float m0 = m_m[0], m1 = m_m[1], m2 = m_m[2];
		m_m[0] = ca * m0 + sa * m_m[3];
		m_m[1] = ca * m1 + sa * m_m[4];
		m_m[2] = ca * m2 + sa * m_m[5];
		m_m[3] = -sa * m0 + ca * m_m[3];
		m_m[4] = -sa * m1 + ca * m_m[4];
		m_m[5] = -sa * m2 + ca * m_m[5];
		//m_m[6] = m_m[6];
		//m_m[7] = m_m[7];
		//m_m[8] = m_m[8];
		//m_m[9] = m_m[9];
		//m_m[10] = m_m[10];
		//m_m[11] = m_m[11];
	}

	// counterclockwise rotate matrix about X-axis
	//     [0]  1  [1]  0   [2]  0
	// m * [3]  0  [4] cos  [5] sin 
	//     [6]  0  [7]-sin  [8] cos
	//     [9]  0 [10]  0  [11]  0
	void RtgMatrix12::RotateX(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float f1, f2;

		//m_m[0] = m_m[0];
		f1 = m_m[1] * ca - m_m[2] * sa;
		f2 = m_m[1] * sa + m_m[2] * ca;
		m_m[1] = f1;
		m_m[2] = f2;
		//m_m[3] = m_m[3];
		f1 = m_m[4] * ca - m_m[5] * sa;
		f2 = m_m[4] * sa + m_m[5] * ca;
		m_m[4] = f1;
		m_m[5] = f2;
		//m_m[6] = m_m[6];
		f1 = m_m[7] * ca - m_m[8] * sa;
		f2 = m_m[7] * sa + m_m[8] * ca;
		m_m[7] = f1;
		m_m[8] = f2;
		//m_m[9] = m_m[9];
		f1 = m_m[10] * ca - m_m[11] * sa;
		f2 = m_m[10] * sa + m_m[11] * ca;
		m_m[10] = f1;
		m_m[11] = f2;
	}

	// counterclockwise rotate matrix about Y-axis
	//     [0] cos  [1]  0  [2]-sin
	// m * [3]  0   [4]  1  [5]  0 
	//     [6] sin  [7]  0  [8] cos
	//     [9]  0  [10]  0 [11]  0
	void RtgMatrix12::RotateY(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float f1, f2;

		f1 = m_m[0] * ca + m_m[2] * sa;
		//M[1] = m_m[1];
		f2 = m_m[2] * ca - m_m[0] * sa;
		m_m[0] = f1;
		m_m[2] = f2;
		f1 = m_m[3] * ca + m_m[5] * sa;
		//M[4] = m_m[4];
		f2 = m_m[5] * ca - m_m[3] * sa;
		m_m[3] = f1;
		m_m[5] = f2;
		f1 = m_m[6] * ca + m_m[8] * sa;
		//M[7] = m_m[7];
		f2 = m_m[8] * ca - m_m[6] * sa;
		m_m[6] = f1;
		m_m[8] = f2;
		f1 = m_m[9] * ca + m_m[11] * sa;
		//M[10]= m_m[10];
		f2 = m_m[11] * ca - m_m[9] * sa;
		m_m[9] = f1;
		m_m[11] = f2;
	}

	// counterclockwise rotate matrix about Z-axis
	//     [0] cos  [1] sin [2]  0
	// m * [3]-sin  [4] cos [5]  0 
	//     [6]  0   [7]  0  [8]  1
	//     [9]  0  [10]  0 [11]  0
	void RtgMatrix12::RotateZ(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float f1, f2;

		f1 = m_m[0] * ca - m_m[1] * sa;
		f2 = m_m[0] * sa + m_m[1] * ca;
		m_m[0] = f1;
		m_m[1] = f2;
		//M[2] = m_m[2];
		f1 = m_m[3] * ca - m_m[4] * sa;
		f2 = m_m[3] * sa + m_m[4] * ca;
		m_m[3] = f1;
		m_m[4] = f2;
		//M[5] = m_m[5];
		f1 = m_m[6] * ca - m_m[7] * sa;
		f2 = m_m[6] * sa + m_m[7] * ca;
		m_m[6] = f1;
		m_m[7] = f2;
		//M[8] = m_m[8];
		f1 = m_m[9] * ca - m_m[10] * sa;
		f2 = m_m[9] * sa + m_m[10] * ca;
		m_m[9] = f1;
		m_m[10] = f2;
		//M[11] = m_m[11];
	}

	void RtgMatrix12::TranslateX(const float Distance)
	{
		m_m[9] += m_m[0] * Distance;
		m_m[10] += m_m[1] * Distance;
		m_m[11] += m_m[2] * Distance;
	}

	void RtgMatrix12::TranslateY(const float Distance)
	{
		m_m[9] += m_m[3] * Distance;
		m_m[10] += m_m[4] * Distance;
		m_m[11] += m_m[5] * Distance;
	}

	void RtgMatrix12::TranslateZ(const float Distance)
	{
		m_m[9] += m_m[6] * Distance;
		m_m[10] += m_m[7] * Distance;
		m_m[11] += m_m[8] * Distance;
	}

	void RtgMatrix12::Translate(float x, float y, float z)
	{
		m_m[9] += m_m[0] * x + m_m[3] * y + m_m[6] * z;
		m_m[10] += m_m[1] * x + m_m[4] * y + m_m[7] * z;
		m_m[11] += m_m[2] * x + m_m[5] * y + m_m[8] * z;
	}

	// make a counterclockwise rotation matrix about X-axis
	// [0]  1  [1]  0   [2]  0
	// [3]  0  [4] cos  [5] sin 
	// [6]  0  [7]-sin  [8] cos
	// [9]  0 [10]  0  [11]  0
	void RtgMatrix12::FillRotateX(float degree)
	{
		m_m[0] = 1.0f;
		m_m[1] = 0.0f;
		m_m[2] = 0.0f;
		m_m[3] = 0.0f;
		m_m[4] = fcos(degree);
		m_m[5] = fsin(degree);
		m_m[6] = 0.0f;
		m_m[7] = -m_m[5];
		m_m[8] = m_m[4];
		m_m[9] = 0.0f;
		m_m[10] = 0.0f;
		m_m[11] = 0.0f;
	}

	// make a counterclockwise rotation matrix about Y-axis
	// [0] cos  [1]  0  [2]-sin
	// [3]  0   [4]  1  [5]  0 
	// [6] sin  [7]  0  [8] cos
	// [9]  0  [10]  0 [11]  0
	void RtgMatrix12::FillRotateY(float degree)
	{
		m_m[0] = fcos(degree);
		m_m[6] = fsin(degree);

		m_m[1] = 0.0f;
		m_m[2] = -m_m[6];
		m_m[3] = 0.0f;
		m_m[4] = 1.0f;
		m_m[5] = 0.0f;
		m_m[7] = 0.0f;
		m_m[8] = m_m[0];
		m_m[9] = 0.0f;
		m_m[10] = 0.0f;
		m_m[11] = 0.0f;
	}

	// make a counterclockwise rotation matrix about X-axis
	// [0] cos  [1] sin [2]  0
	// [3]-sin  [4] cos [5]  0 
	// [6]  0   [7]  0  [8]  1
	// [9]  0  [10]  0 [11]  0
	void RtgMatrix12::FillRotateZ(float degree)
	{
		m_m[0] = fcos(degree);
		m_m[1] = fsin(degree);
		m_m[2] = 0.0f;
		m_m[3] = -m_m[1];
		m_m[4] = m_m[0];
		m_m[5] = 0.0f;
		m_m[6] = 0.0f;
		m_m[7] = 0.0f;
		m_m[8] = 1.0f;
		m_m[9] = 0.0f;
		m_m[10] = 0.0f;
		m_m[11] = 0.0f;
	}

	void RtgMatrix12::Output()
	{
		RtCoreLog().Info("Matrix12\n%8.4f %8.4f %8.4f\n%8.4f %8.4f %8.4f\n%8.4f %8.4f %8.4f\n%8.4f %8.4f %8.4f\n",
			_00, _01, _02, _10, _11, _12, _20, _21, _22, _30, _31, _32);
	}
	//
	// Implementation of class RtgMatrix16
	//

	RtgMatrix16::RtgMatrix16(float v00, float v01, float v02, float v03,
		float v10, float v11, float v12, float v13,
		float v20, float v21, float v22, float v23,
		float v30, float v31, float v32, float v33)
	{
		_00 = v00;
		_01 = v01;
		_02 = v02;
		_03 = v03;

		_10 = v10;
		_11 = v11;
		_12 = v12;
		_13 = v13;

		_20 = v20;
		_21 = v21;
		_22 = v22;
		_23 = v23;

		_30 = v30;
		_31 = v31;
		_32 = v32;
		_33 = v33;
	}

	RtgMatrix16::RtgMatrix16(const RtgMatrix16& rhs)
	{
		*this = rhs;
	}

	RtgMatrix16::RtgMatrix16(const float m[16])
	{
		m_m[0] = m[0];   m_m[1] = m[1];   m_m[2] = m[2];   m_m[3] = m[3];
		m_m[4] = m[4];   m_m[5] = m[5];   m_m[6] = m[6];   m_m[7] = m[7];
		m_m[8] = m[8];   m_m[9] = m[9];   m_m[10] = m[10];  m_m[11] = m[11];
		m_m[12] = m[12]; m_m[13] = m[13];  m_m[14] = m[14];  m_m[15] = m[15];
	}

	void RtgMatrix16::Zero(void)
	{
		m_m[0] = m_m[1] = m_m[2] = m_m[3] =
			m_m[4] = m_m[5] = m_m[6] = m_m[7] =
			m_m[8] = m_m[9] = m_m[10] = m_m[11] =
			m_m[12] = m_m[13] = m_m[14] = m_m[15] = 0.0f;
	}

	void RtgMatrix16::Unit(void)
	{
		m_m[1] = m_m[2] = m_m[3] = m_m[4] =
			m_m[6] = m_m[7] = m_m[8] = m_m[9] =
			m_m[11] = m_m[12] = m_m[13] = m_m[14] = 0.0f;
		m_m[0] = m_m[5] = m_m[10] = m_m[15] = 1.0f;
	}

	RtgMatrix16 RtgMatrix16::operator+(const RtgMatrix16& rhs)
	{
		RtgMatrix16 M;
		for (int i = 0; i < 16; i++) {
			M[i] = m_m[i] + rhs[i];
		}
		return M;
	}

	RtgMatrix16 RtgMatrix16::operator-(const RtgMatrix16& rhs)
	{
		RtgMatrix16 M;
		for (int i = 0; i < 16; i++) {
			M[i] = m_m[i] - rhs[i];
		}
		return M;
	}

	RtgMatrix16 RtgMatrix16::operator+(const float m[16])
	{
		RtgMatrix16 M;
		for (int i = 0; i < 16; i++) {
			M[i] = m_m[i] + m[i];
		}
		return M;
	}

	RtgMatrix16 RtgMatrix16::operator-(const float m[12])
	{
		RtgMatrix16 M;
		for (int i = 0; i < 16; i++) {
			M[i] = m_m[i] - m[i];
		}
		return M;
	}

	RtgMatrix16& RtgMatrix16::operator+=(const RtgMatrix16& rhs)
	{
		for (int i = 0; i < 16; i++) {
			m_m[i] += rhs[i];
		}
		return *this;
	}

	RtgMatrix16& RtgMatrix16::operator-=(const RtgMatrix16& rhs)
	{
		for (int i = 0; i < 16; i++) {
			m_m[i] -= rhs[i];
		}
		return *this;
	}

	RtgMatrix16& RtgMatrix16::operator+=(const float m[16])
	{
		for (int i = 0; i < 16; i++) {
			m_m[i] += m[i];
		}
		return *this;
	}

	RtgMatrix16& RtgMatrix16::operator-=(const float m[12])
	{
		for (int i = 0; i < 16; i++) {
			m_m[i] -= m[i];
		}
		return *this;
	}

	RtgMatrix16 RtgMatrix16::operator*(const RtgMatrix16& rhs)
	{
		RtgMatrix16 M;
		M[0] = m_m[0] * rhs.m_m[0] + m_m[1] * rhs.m_m[4] + m_m[2] * rhs.m_m[8] + m_m[3] * rhs.m_m[12];
		M[1] = m_m[0] * rhs.m_m[1] + m_m[1] * rhs.m_m[5] + m_m[2] * rhs.m_m[9] + m_m[3] * rhs.m_m[13];
		M[2] = m_m[0] * rhs.m_m[2] + m_m[1] * rhs.m_m[6] + m_m[2] * rhs.m_m[10] + m_m[3] * rhs.m_m[14];
		M[3] = m_m[0] * rhs.m_m[3] + m_m[1] * rhs.m_m[7] + m_m[2] * rhs.m_m[11] + m_m[3] * rhs.m_m[15];
		M[4] = m_m[4] * rhs.m_m[0] + m_m[5] * rhs.m_m[4] + m_m[6] * rhs.m_m[8] + m_m[7] * rhs.m_m[12];
		M[5] = m_m[4] * rhs.m_m[1] + m_m[5] * rhs.m_m[5] + m_m[6] * rhs.m_m[9] + m_m[7] * rhs.m_m[13];
		M[6] = m_m[4] * rhs.m_m[2] + m_m[5] * rhs.m_m[6] + m_m[6] * rhs.m_m[10] + m_m[7] * rhs.m_m[14];
		M[7] = m_m[4] * rhs.m_m[3] + m_m[5] * rhs.m_m[7] + m_m[6] * rhs.m_m[11] + m_m[7] * rhs.m_m[15];
		M[8] = m_m[8] * rhs.m_m[0] + m_m[9] * rhs.m_m[4] + m_m[10] * rhs.m_m[8] + m_m[11] * rhs.m_m[12];
		M[9] = m_m[8] * rhs.m_m[1] + m_m[9] * rhs.m_m[5] + m_m[10] * rhs.m_m[9] + m_m[11] * rhs.m_m[13];
		M[10] = m_m[8] * rhs.m_m[2] + m_m[9] * rhs.m_m[6] + m_m[10] * rhs.m_m[10] + m_m[11] * rhs.m_m[14];
		M[11] = m_m[8] * rhs.m_m[3] + m_m[9] * rhs.m_m[7] + m_m[10] * rhs.m_m[11] + m_m[11] * rhs.m_m[15];
		M[12] = m_m[12] * rhs.m_m[0] + m_m[13] * rhs.m_m[4] + m_m[14] * rhs.m_m[8] + m_m[15] * rhs.m_m[12];
		M[13] = m_m[12] * rhs.m_m[1] + m_m[13] * rhs.m_m[5] + m_m[14] * rhs.m_m[9] + m_m[15] * rhs.m_m[13];
		M[14] = m_m[12] * rhs.m_m[2] + m_m[13] * rhs.m_m[6] + m_m[14] * rhs.m_m[10] + m_m[15] * rhs.m_m[14];
		M[15] = m_m[12] * rhs.m_m[3] + m_m[13] * rhs.m_m[7] + m_m[14] * rhs.m_m[11] + m_m[15] * rhs.m_m[15];
		return M;
	}

	RtgMatrix16 RtgMatrix16::operator*(float f)
	{
		RtgMatrix16 M;
		for (int i = 0; i < 16; i++) {
			M[i] = m_m[i] * f;
		}
		return M;
	}

	RtgMatrix16 RtgMatrix16::operator/(float f)
	{
		CHECK(f != 0.0f);
		RtgMatrix16 M;
		for (int i = 0; i < 16; i++) {
			M[i] = m_m[i] / f;
		}
		return M;
	}

	RtgMatrix16 RtgMatrix16::operator*=(float f)
	{
		for (int i = 0; i < 16; i++) {
			m_m[i] *= f;
		}
		return *this;
	}

	RtgMatrix16 RtgMatrix16::operator/=(float f)
	{
		CHECK(f != 0.0f);
		for (int i = 0; i < 16; i++) {
			m_m[i] /= f;
		}
		return *this;
	}

	RtgMatrix16& RtgMatrix16::operator=(const RtgMatrix16& rhs)
	{
		_00 = rhs._00; _01 = rhs._01; _02 = rhs._02; _03 = rhs._03;
		_10 = rhs._10; _11 = rhs._11; _12 = rhs._12; _13 = rhs._13;
		_20 = rhs._20; _21 = rhs._21; _22 = rhs._22; _23 = rhs._23;
		_30 = rhs._30; _31 = rhs._31; _32 = rhs._32; _33 = rhs._33;
		return *this;
	}

	RtgMatrix16& RtgMatrix16::operator= (const RtgMatrix12& rhs)
	{
		_00 = rhs._00; _01 = rhs._01; _02 = rhs._02; _03 = 0.f;
		_10 = rhs._10; _11 = rhs._11; _12 = rhs._12; _13 = 0.f;
		_20 = rhs._20; _21 = rhs._21; _22 = rhs._22; _23 = 0.f;
		_30 = rhs._30; _31 = rhs._31; _32 = rhs._32; _33 = 1.f;
		return *this;
	}

	RtgMatrix16& RtgMatrix16::operator=(const float m[16])
	{
		for (int i = 0; i < 16; i++) {
			m_m[i] = m[i];
		}
		return *this;
	}

	void  RtgMatrix16::Dump(float m[16]) const
	{
		for (int i = 0; i < 16; i++) {
			m[i] = m_m[i];
		}
	}

	std::string RtgMatrix16::ToString() const
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(6); // Set precision for floating point numbers
		oss << _00 << "," << _01 << "," << _02 << "," << _03 << ",\n";
		oss << _10 << "," << _11 << "," << _12 << "," << _13 << ",\n";
		oss << _20 << "," << _21 << "," << _22 << "," << _23 << ",\n";
		oss << _30 << "," << _31 << "," << _32 << "," << _33 << "\n";
		return oss.str();
	}

	RtgMatrix16 RtgMatrix16::Inverse(void) const
	{
		return (1.0f / Determinant()) * Adjoint();
	}

#define SWAP(a,b) { float c=a; a=b; b=c; }

	inline float MINOR(const RtgMatrix16& m, const int r0, const int r1, const int r2, const int c0, const int c1, const int c2)
	{
		return m(r0, c0) * (m(r1, c1) * m(r2, c2) - m(r2, c1) * m(r1, c2)) -
			m(r0, c1) * (m(r1, c0) * m(r2, c2) - m(r2, c0) * m(r1, c2)) +
			m(r0, c2) * (m(r1, c0) * m(r2, c1) - m(r2, c0) * m(r1, c1));
	}

	RtgMatrix16 RtgMatrix16::Adjoint(void) const
	{
		RtgMatrix16 ma(MINOR(*this, 1, 2, 3, 1, 2, 3),
			-MINOR(*this, 0, 2, 3, 1, 2, 3),
			MINOR(*this, 0, 1, 3, 1, 2, 3),
			-MINOR(*this, 0, 1, 2, 1, 2, 3),

			-MINOR(*this, 1, 2, 3, 0, 2, 3),
			MINOR(*this, 0, 2, 3, 0, 2, 3),
			-MINOR(*this, 0, 1, 3, 0, 2, 3),
			MINOR(*this, 0, 1, 2, 0, 2, 3),

			MINOR(*this, 1, 2, 3, 0, 1, 3),
			-MINOR(*this, 0, 2, 3, 0, 1, 3),
			MINOR(*this, 0, 1, 3, 0, 1, 3),
			-MINOR(*this, 0, 1, 2, 0, 1, 3),

			-MINOR(*this, 1, 2, 3, 0, 1, 2),
			MINOR(*this, 0, 2, 3, 0, 1, 2),
			-MINOR(*this, 0, 1, 3, 0, 1, 2),
			MINOR(*this, 0, 1, 2, 0, 1, 2));
		return ma;
	}

	float RtgMatrix16::Determinant(void) const
	{
		return _00 * MINOR(*this, 1, 2, 3, 1, 2, 3) -
			_01 * MINOR(*this, 1, 2, 3, 0, 2, 3) +
			_02 * MINOR(*this, 1, 2, 3, 0, 1, 3) -
			_03 * MINOR(*this, 1, 2, 3, 0, 1, 2);
	}

	void RtgMatrix16::Invert(void)
	{
		*this = Inverse();
	}

	void RtgMatrix16::Transport(void)
	{
		Exchange(_01, _10);
		Exchange(_02, _20);
		Exchange(_03, _30);
		Exchange(_12, _21);
		Exchange(_13, _31);
		Exchange(_23, _32);
	}

	// current = Rx * current
	void RtgMatrix16::RotateLX(const float Degrees)
	{
		float Temp10, Temp11, Temp12, Temp13;
		float Temp20, Temp21, Temp22, Temp23;
		float Sin = fsin(Degrees), Cos = fcos(Degrees);

		Temp10 = _10 * Cos + _20 * Sin;
		Temp11 = _11 * Cos + _21 * Sin;
		Temp12 = _12 * Cos + _22 * Sin;
		Temp13 = _13 * Cos + _23 * Sin;

		Temp20 = _10 * -Sin + _20 * Cos;
		Temp21 = _11 * -Sin + _21 * Cos;
		Temp22 = _12 * -Sin + _22 * Cos;
		Temp23 = _13 * -Sin + _23 * Cos;

		_10 = Temp10;
		_11 = Temp11;
		_12 = Temp12;
		_13 = Temp13;
		_20 = Temp20;
		_21 = Temp21;
		_22 = Temp22;
		_23 = Temp23;
	}

	// current = Ry * current
	void RtgMatrix16::RotateLY(const float Degrees)
	{
		float Temp00, Temp01, Temp02, Temp03;
		float Temp20, Temp21, Temp22, Temp23;
		float Sin = fsin(Degrees), Cos = fcos(Degrees);

		Temp00 = _00 * Cos - _20 * Sin;
		Temp01 = _01 * Cos - _21 * Sin;
		Temp02 = _02 * Cos - _22 * Sin;
		Temp03 = _03 * Cos - _23 * Sin;

		Temp20 = _00 * Sin + _20 * Cos;
		Temp21 = _01 * Sin + _21 * Cos;
		Temp22 = _02 * Sin + _22 * Cos;
		Temp23 = _03 * Sin + _23 * Cos;

		_00 = Temp00;
		_01 = Temp01;
		_02 = Temp02;
		_03 = Temp03;
		_20 = Temp20;
		_21 = Temp21;
		_22 = Temp22;
		_23 = Temp23;
	}

	// current = Rz * current
	void RtgMatrix16::RotateLZ(const float Degrees)
	{
		float Temp00, Temp01, Temp02, Temp03;
		float Temp10, Temp11, Temp12, Temp13;
		float Sin = fsin(Degrees), Cos = fcos(Degrees);

		Temp00 = _00 * Cos + _10 * Sin;
		Temp01 = _01 * Cos + _11 * Sin;
		Temp02 = _02 * Cos + _12 * Sin;
		Temp03 = _03 * Cos + _13 * Sin;

		Temp10 = _00 * -Sin + _10 * Cos;
		Temp11 = _01 * -Sin + _11 * Cos;
		Temp12 = _02 * -Sin + _12 * Cos;
		Temp13 = _03 * -Sin + _13 * Cos;

		_00 = Temp00;
		_01 = Temp01;
		_02 = Temp02;
		_03 = Temp03;
		_10 = Temp10;
		_11 = Temp11;
		_12 = Temp12;
		_13 = Temp13;
	}

	int RtgMatrix16::Rotate(const float frad, float x, float y, float z)
	{
		float     fCos = fcos(frad);
		float     fSin = fsin(frad);
		float	  sqrsum = x * x + y * y + z * z;

		if (IsZero(sqrsum))
			return 0;

		if (sqrsum != 1.0f)
		{
			float len = fsqrt(x * x + y * y + z * z);
			x /= len;
			y /= len;
			z /= len;
		}

		RtgMatrix16	  local, final;

		local.Identity();

		local._00 = (x * x) * (1.0f - fCos) + fCos;
		local._01 = (x * y) * (1.0f - fCos) + (z * fSin);
		local._02 = (x * z) * (1.0f - fCos) - (y * fSin);

		local._10 = (y * x) * (1.0f - fCos) - (z * fSin);
		local._11 = (y * y) * (1.0f - fCos) + fCos;
		local._12 = (y * z) * (1.0f - fCos) + (x * fSin);

		local._20 = (z * x) * (1.0f - fCos) + (y * fSin);
		local._21 = (z * y) * (1.0f - fCos) - (x * fSin);
		local._22 = (z * z) * (1.0f - fCos) + fCos;

		final = local * (*this);
		*this = final;
		return 1;
	}

	// counterclockwise rotate matrix about X-axis
	//     [0]  1  [1]  0   [2]  0  [3]  0
	// m * [4]  0  [5] cos  [6] sin [7]  0
	//     [8]  0  [9]-sin  [10]cos [11] 0
	//     [12] 0  [13] 0   [14] 0  [15] 1
	void RtgMatrix16::RotateX(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float f1, f2;
		f1 = m_m[1] * ca - m_m[2] * sa;
		f2 = m_m[1] * sa + m_m[2] * ca;
		m_m[1] = f1;
		m_m[2] = f2;
		f1 = m_m[5] * ca - m_m[6] * sa;
		f2 = m_m[5] * sa + m_m[6] * ca;
		m_m[5] = f1;
		m_m[6] = f2;
		f1 = m_m[9] * ca - m_m[10] * sa;
		f2 = m_m[9] * sa + m_m[10] * ca;
		m_m[9] = f1;
		m_m[10] = f2;
		f1 = m_m[13] * ca - m_m[14] * sa;
		f2 = m_m[13] * sa + m_m[14] * ca;
		m_m[13] = f1;
		m_m[14] = f2;
	}

	// counterclockwise rotate matrix about Y-axis
	//     [0] cos  [1]  0  [2]-sin [3] 0
	// m * [4]  0   [5]  1  [6]  0  [7] 0
	//     [8] sin  [9]  0  [10]cos [11]0
	//     [12] 0   [13] 0  [14] 0  [15]1
	void RtgMatrix16::RotateY(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float f1, f2;
		f1 = m_m[0] * ca + m_m[2] * sa;
		f2 = m_m[2] * ca - m_m[0] * sa;
		m_m[0] = f1;
		m_m[2] = f2;
		f1 = m_m[4] * ca + m_m[6] * sa;
		f2 = m_m[6] * ca - m_m[4] * sa;
		m_m[4] = f1;
		m_m[6] = f2;
		f1 = m_m[8] * ca + m_m[10] * sa;
		f2 = m_m[10] * ca - m_m[8] * sa;
		m_m[8] = f1;
		m_m[10] = f2;
		f1 = m_m[12] * ca + m_m[14] * sa;
		f2 = m_m[14] * ca - m_m[12] * sa;
		m_m[12] = f1;
		m_m[14] = f2;
	}

	// counterclockwise rotate matrix about Z-axis
	//     [0] cos  [1] sin [2]  0  [3]  0
	// m * [4]-sin  [5] cos [6]  0  [7]  0
	//     [8]  0   [9]  0 [10]  1 [11]  0
	//    [12]  0  [13]  0 [14]  0 [15]  1
	void RtgMatrix16::RotateZ(float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float f1, f2;
		f1 = m_m[0] * ca - m_m[1] * sa;
		f2 = m_m[0] * sa + m_m[1] * ca;
		m_m[0] = f1;
		m_m[1] = f2;
		f1 = m_m[4] * ca - m_m[5] * sa;
		f2 = m_m[4] * sa + m_m[5] * ca;
		m_m[4] = f1;
		m_m[5] = f2;
		f1 = m_m[8] * ca - m_m[9] * sa;
		f2 = m_m[8] * sa + m_m[9] * ca;
		m_m[8] = f1;
		m_m[9] = f2;
		f1 = m_m[12] * ca - m_m[13] * sa;
		f2 = m_m[12] * sa + m_m[13] * ca;
		m_m[12] = f1;
		m_m[13] = f2;
	}

	// make a counterclockwise rotation matrix about X-axis
	// [0]  1  [1]  0   [2]  0   [3]  0
	// [4]  0  [5] cos  [6] sin  [7]  0
	// [8]  0  [9]-sin [10] cos [11]  0
	//[11]  0 [11]  0  [14]  0  [15]  1
	void RtgMatrix16::FillRotateX(float degree)
	{
		m_m[0] = 1.0f;
		m_m[1] = 0.0f;
		m_m[2] = 0.0f;
		m_m[3] = 0.0f;
		m_m[4] = 0.0f;
		m_m[5] = fcos(degree);
		m_m[6] = fsin(degree);
		m_m[7] = 0.0f;
		m_m[8] = 0.0f;
		m_m[9] = -m_m[6];
		m_m[10] = m_m[5];
		m_m[11] = 0.0f;
		m_m[12] = 0.0f;
		m_m[13] = 0.0f;
		m_m[14] = 0.0f;
		m_m[15] = 1.0f;
	}

	// make a counterclockwise rotation matrix about Y-axis
	// [0] cos  [1]  0  [2]-sin  [3]  0
	// [4]  0   [5]  1  [6]  0   [7]  0
	// [8] sin  [9]  0 [10] cos [11]  0
	//[12]  0  [13]  0 [14]  0  [15]  1
	void RtgMatrix16::FillRotateY(float degree)
	{
		m_m[8] = fsin(degree);

		m_m[0] = fcos(degree);
		m_m[1] = 0.0f;
		m_m[2] = -m_m[8];
		m_m[3] = 0.0f;
		m_m[4] = 0.0f;
		m_m[5] = 1.0f;
		m_m[6] = 0.0f;
		m_m[7] = 0.0f;

		m_m[9] = 0.0f;
		m_m[10] = m_m[0];
		m_m[11] = 0.0f;
		m_m[12] = 0.0f;
		m_m[13] = 0.0f;
		m_m[14] = 0.0f;
		m_m[15] = 1.0f;
	}

	// make a counterclockwise rotation matrix about X-axis
	// [0] cos  [1] sin [2]  0  [3]  0
	// [4]-sin  [5] cos [6]  0  [7]  0
	// [8]  0   [9]  0 [10]  1 [11]  0
	//[12]  0  [13]  0 [14]  0 [15]  1
	void RtgMatrix16::FillRotateZ(float degree)
	{
		m_m[0] = fcos(degree);
		m_m[1] = fsin(degree);
		m_m[2] = 0.0f;
		m_m[3] = 0.0f;
		m_m[4] = -m_m[1];
		m_m[5] = m_m[0];
		m_m[6] = 0.0f;
		m_m[7] = 0.0f;
		m_m[8] = 0.0f;
		m_m[9] = 0.0f;
		m_m[10] = 1.0f;
		m_m[11] = 0.0f;
		m_m[12] = 0.0f;
		m_m[13] = 0.0f;
		m_m[14] = 0.0f;
		m_m[15] = 0.0f;
	}

	void RtgMatrix16::GetAxis(int axis, RtgVertex3& vct) const
	{
		switch (axis)
		{
		case 0: vct.Set(_00, _10, _20); break;
		case 1: vct.Set(_01, _11, _21); break;
		case 2: vct.Set(_02, _12, _22); break;
		}
	}

	void RtgMatrix16::SetAxis(int axis, const float* pVet)
	{
		switch (axis)
		{
		case 0: _00 = pVet[0]; _10 = pVet[1]; _20 = pVet[2]; break;
		case 1: _01 = pVet[0]; _11 = pVet[1]; _21 = pVet[2]; break;
		case 2: _02 = pVet[0]; _12 = pVet[1]; _22 = pVet[2]; break;
		}
	}


	void RtgMatrix16::GetCol(int axis, RtgVertex3& vct) const
	{
		switch (axis)
		{
		case 0: vct.Set(_00, _10, _20); break;
		case 1: vct.Set(_01, _11, _21); break;
		case 2: vct.Set(_02, _12, _22); break;
		}
	}

	void RtgMatrix16::SetCol(int axis, const float* pVet)
	{
		switch (axis)
		{
		case 0: _00 = pVet[0]; _10 = pVet[1]; _20 = pVet[2]; break;
		case 1: _01 = pVet[0]; _11 = pVet[1]; _21 = pVet[2]; break;
		case 2: _02 = pVet[0]; _12 = pVet[1]; _22 = pVet[2]; break;
		}
	}

	void RtgMatrix16::SetRow(int row, RtgVertex3& vct)
	{
		switch (row)
		{
		case 0: _00 = vct.m_v[0]; _01 = vct.m_v[1]; _02 = vct.m_v[2]; break;
		case 1: _10 = vct.m_v[0]; _11 = vct.m_v[1]; _12 = vct.m_v[2]; break;
		case 2: _20 = vct.m_v[0]; _21 = vct.m_v[1]; _22 = vct.m_v[2]; break;
		case 3: _30 = vct.m_v[0]; _31 = vct.m_v[1]; _32 = vct.m_v[2]; break;
		}
	}

	void RtgMatrix16::SetRow(int row, const float* pVet)
	{
		switch (row)
		{
		case 0: _00 = pVet[0]; _01 = pVet[1]; _02 = pVet[2]; break;
		case 1: _10 = pVet[0]; _11 = pVet[1]; _12 = pVet[2]; break;
		case 2: _20 = pVet[0]; _21 = pVet[1]; _22 = pVet[2]; break;
		case 3: _30 = pVet[0]; _31 = pVet[1]; _32 = pVet[2]; break;
		}
	}

	void RtgMatrix16::GetRow(int row, float* pOut) const
	{
		switch (row)
		{
		case 0: pOut[0] = _00; pOut[1] = _01; pOut[2] = _02; break;
		case 1: pOut[0] = _10; pOut[1] = _11; pOut[2] = _12; break;
		case 2: pOut[0] = _20; pOut[1] = _21; pOut[2] = _22; break;
		case 3: pOut[0] = _30; pOut[1] = _31; pOut[2] = _32; break;
		}
	}

	void RtgMatrix16::GetRow(int row, RtgVertex3& vct) const
	{
		switch (row)
		{
		case 0: vct.Set((float*)&_00); break;
		case 1: vct.Set((float*)&_10); break;
		case 2: vct.Set((float*)&_20); break;
		case 3: vct.Set((float*)&_30); break;
		}
	}

	void RtgMatrix16::Scale(float x, float y, float z)
	{
		RtgMatrix16 ma;
		ma.Unit();
		ma._00 = x;
		ma._11 = y;
		ma._22 = z;
		(*this) = ma * (*this);
	}

	// vFace是方向的矢量，vUp是向上方向的矢量，两个参数都不一定需要单位化
	void RtgMatrix16::FaceTo(RtgVertex3& vFace, RtgVertex3& vUp)
	{
		RtgVertex3 vRight;
		vRight = vFace.Cross(vUp);
		_00 = vRight.x; _01 = vRight.y; _02 = vRight.z;
		_10 = vFace.x;  _11 = vFace.y;  _12 = vFace.z;
		_20 = vUp.x;    _21 = vUp.y;    _22 = vUp.z;
		NormalizeMatrix();
		this->RotateLZ(PI);
	}

	// vPos是目标点的坐标，vUp是向上方向的矢量，vUp不一定需要单位化
	void RtgMatrix16::FaceToVertex(RtgVertex3& vPos, RtgVertex3& vUp)
	{
		RtgVertex3 vOrPos(Position());
		RtgVertex3 vFace;
		vFace = vPos - vOrPos;
		if (vFace.x != 0.f || vFace.y != 0.f || vFace.z != 0.f)
		{
			FaceTo(vFace, vUp);
		}
	}

	// vPos是目标点的坐标，向上方向为垂直向上
	void RtgMatrix16::FaceToVertex(RtgVertex3& vPos)
	{
		FaceToVertex(vPos, RtgVertex3(0.f, 0.f, 1.f));
	}

	void RtgMatrix16::Perspective(float fFov, float fAspect, float zNear, float zFar)
	{
		RtgMatrix16 local, final;
		local.Identity();

		/*
		// right hand
		float yScale = 1/ftan(fFov/2.f);
		float xScale = fAspect * yScale;
		float Q = zFar/(zNear-zFar);
		local._00 = xScale;
		local._11 = yScale;
		local._22 = Q;
		local._23 = -1;
		local._32 = zNear*Q;
		local._33 = 0;
		*/

		// left hand
		// float yScale = 1/ftan(fFov/2.f);
		// float xScale = fAspect * yScale;
		// float Q = zFar/(zFar-zNear);
		// local._00 = xScale;
		// local._11 = yScale;
		// local._22 = Q;
		// local._23 = 1;
		// local._32 = -zNear*Q;
		// local._33 = 0;

		// old
		float fTan = ftan(fFov / 2.0f);
		float ymax = fTan * zNear;
		float ymin = -ymax;
		float xmax = ymax * fAspect;
		float xmin = -xmax;
		local._00 = 2.0f * zNear / (xmax - xmin);
		local._11 = 2.0f * zNear / (ymax - ymin);
		local._22 = -(zFar + zNear) / (zFar - zNear);
		local._32 = -(2.0f * zFar * zNear) / (zFar - zNear);
		local._33 = 0.0f;
		local._02 = (xmax + xmin) / (xmax - xmin);
		local._12 = (ymax + ymin) / (ymax - ymin);
		local._23 = -1;

		final = local * (*this);
		(*this) = final;
	}

	void RtgMatrix16::Ortho(float left, float right, float bottom, float top, float nearplane, float farplane, bool bRighthand)
	{
		RtgMatrix16 local, final;
		local.Identity();

		local._00 = 2.0f / (right - left);
		local._11 = 2.0f / (bottom - top);
		if (bRighthand) local._22 = 1.0f / (nearplane - farplane);
		else            local._22 = 1.0f / (farplane - nearplane);
		local._32 = nearplane / (nearplane - farplane);

		final = local * (*this);
		(*this) = final;
	}

	void RtgMatrix16::SetView(float fromX, float fromY, float fromZ, float cX, float cY, float cZ, float uX, float uY, float uZ)
	{
		RtgVertex3 from(fromX, fromY, fromZ);
		RtgVertex3 center(cX, cY, cZ);
		RtgVertex3 up(uX, uY, uZ);
		SetView(from, center, up);
	}

	void RtgMatrix16::SetView(RtgVertex3 pos, RtgVertex3 center, RtgVertex3 up)
	{
		RtgVertex3 front;
		RtgVertex3 right;
		RtgMatrix16 matrix, final;

		front = pos - center;    front.Normalize();
		right = up.Cross(front); right.Normalize();
		up = front.Cross(right); up.Normalize();

		matrix.Identity();
		matrix._00 = right.x;
		matrix._10 = right.y;
		matrix._20 = right.z;

		matrix._01 = up.x;
		matrix._11 = up.y;
		matrix._21 = up.z;

		matrix._02 = front.x;
		matrix._12 = front.y;
		matrix._22 = front.z;

		matrix._30 = -(right.x * pos.x + right.y * pos.y + right.z * pos.z);
		matrix._31 = -(up.x * pos.x + up.y * pos.y + up.z * pos.z);
		matrix._32 = -(front.x * pos.x + front.y * pos.y + front.z * pos.z);
		//	matrix.Translate( pos.x, pos.y, pos.z );

		final = matrix * (*this);
		(*this) = final;
	}

	void RtgMatrix16::Reflect(RtgVertex4& vPlane)
	{
		RtgVertex4 P = vPlane;
		P.Normalize3();
		_00 = -2 * P.x * P.x + 1;  _01 = -2 * P.y * P.x;      _02 = -2 * P.z * P.x;        _03 = 0;
		_10 = -2 * P.x * P.y;      _11 = -2 * P.y * P.y + 1;  _12 = -2 * P.z * P.y;        _13 = 0;
		_20 = -2 * P.x * P.z;      _21 = -2 * P.y * P.z;      _22 = -2 * P.z * P.z + 1;    _23 = 0;
		_30 = -2 * P.x * P.w;      _31 = -2 * P.y * P.w;      _32 = -2 * P.z * P.w;        _33 = 1;
	}

	// current = Tx * current
	void RtgMatrix16::TranslateX(const float Distance)
	{
		_30 += _00 * Distance;
		_31 += _01 * Distance;
		_32 += _02 * Distance;
		_33 += _03 * Distance;
	}

	// current = Ty * current
	void RtgMatrix16::TranslateY(const float Distance)
	{
		_30 += _10 * Distance;
		_31 += _11 * Distance;
		_32 += _12 * Distance;
		_33 += _13 * Distance;
	}

	// current = Tz * current
	void RtgMatrix16::TranslateZ(const float Distance)
	{
		_30 += _20 * Distance;
		_31 += _21 * Distance;
		_32 += _22 * Distance;
		_33 += _23 * Distance;
	}

	// current = T * current
	void RtgMatrix16::Translate(float x, float y, float z)
	{
		_30 += _00 * x + _10 * y + _20 * z;
		_31 += _01 * x + _11 * y + _21 * z;
		_32 += _02 * x + _12 * y + _22 * z;
		_33 += _03 * x + _13 * y + _23 * z;
	}

	void RtgMatrix16::Set4X3Matrix(const float* pMatrix)
	{
		memcpy(&_00, &pMatrix[0], sizeof(float) * 3); _03 = 0.0f;
		memcpy(&_10, &pMatrix[3], sizeof(float) * 3); _13 = 0.0f;
		memcpy(&_20, &pMatrix[6], sizeof(float) * 3); _23 = 0.0f;
		memcpy(&_30, &pMatrix[9], sizeof(float) * 3); _33 = 1.0f;
	}

	void RtgMatrix16::Get4X3Matrix(float* pMatrix) const
	{
		memcpy(&pMatrix[0], &_00, sizeof(float) * 3);
		memcpy(&pMatrix[3], &_10, sizeof(float) * 3);
		memcpy(&pMatrix[6], &_20, sizeof(float) * 3);
		memcpy(&pMatrix[9], &_30, sizeof(float) * 3);
	}

	float GetAngle(float fSin, float fCos)
	{
		float rad;

		if (fSin >= 0 && fCos >= 0)
		{
			rad = fasin(fSin);
		}
		else if (fSin >= 0 && fCos < 0)
		{
			rad = PI - fasin(fSin);
		}
		else if (fSin < 0 && fCos < 0)
		{
			rad = PI - fasin(fSin);
		}
		else if (fSin < 0 && fCos >= 0)
		{
			rad = PI * 2.0f + fasin(fSin);
		}
		return rad;
	}

	/*
		get rotation parameter from matrix
	*/
	int RtgMatrix16::GetRotate(float* frad, float* x, float* y, float* z) const
	{
		float _01_add_10 = _01 + _10;
		float _02_add_20 = _02 + _20;
		float _12_add_21 = _12 + _21;
		float vx, vy, vz;
		float fSin, fCos;
		float poweradd, len;
		int	  addzerocount = 0;

		if (IsZero(_01_add_10)) addzerocount++;
		if (IsZero(_02_add_20)) addzerocount++;
		if (IsZero(_12_add_21)) addzerocount++;

		switch (addzerocount)
		{
			// use add information
		case 0:
		case 1:
			if (!IsZero(_02_add_20) && !IsZero(_01_add_10))
			{
				vx = 1.0f;
				vy = _12_add_21 / _02_add_20;
				vz = _12_add_21 / _01_add_10;
			}
			// y=1.0f
			else if (!IsZero(_12_add_21) && !IsZero(_01_add_10))
			{
				vx = _02_add_20 / _12_add_21;
				vy = 1.0f;
				vz = _02_add_20 / _01_add_10;
			}
			// z=1.0f
			else if (!IsZero(_12_add_21) && !IsZero(_02_add_20))
			{
				vx = _01_add_10 / _12_add_21;
				vy = _01_add_10 / _02_add_20;
				vz = 1.0f;
			}
			break;
			// use sub information
		case 2:
		{
			float _01_sub_10 = _01 - _10;
			float _20_sub_02 = _20 - _02;
			float _12_sub_21 = _12 - _21;

			if (!IsZero(_12_sub_21))
			{
				vx = 1.0f;
				vy = _20_sub_02 / _12_sub_21;
				vz = _01_sub_10 / _12_sub_21;
			}
			else if (!IsZero(_20_sub_02))
			{
				vx = _12_sub_21 / _20_sub_02;
				vy = 1.0f;
				vz = _01_sub_10 / _20_sub_02;
			}
			else if (!IsZero(_01_sub_10))
			{
				vx = _12_sub_21 / _01_sub_10;
				vy = _20_sub_02 / _01_sub_10;
				vz = 1.0f;
			}
			else
			{
				*frad = *x = *z = 0.0f;
				*y = 1.0f;
				return 0;
			}
			break;
		}
		// 在正轴旋转
		case 3:
			// 在X轴旋转
			if (!IsZero(_00) && IsZero(_10) && IsZero(_20))
			{
				vx = _00;
				vy = 0.0f;
				vz = 0.0f;
				fCos = _11;
				fSin = _12;
			}
			// 在Y轴旋转
			else if (IsZero(_01) && !IsZero(_11) && IsZero(_21))
			{
				vx = 0.0f;
				vy = _11;
				vz = 0.0f;
				fCos = _00;
				fSin = _20;
			}
			// 在Z轴旋转
			else if (IsZero(_02) && IsZero(_12) && !IsZero(_22))
			{
				vx = 0.0f;
				vy = 0.0f;
				vz = _22;
				fCos = _00;
				fSin = _01;
			}
			else
			{
				*frad = *x = *z = 0.0f;
				*y = 1.0f;
				return 0;
			}

			if (Abs(fSin) > 1.0f || Abs(fCos) > 1.0f)
			{
				*frad = *x = *z = 0.0f;
				*y = 1.0f;
				return 0;
			}

			poweradd = vx * vx + vy * vy + vz * vz;
			if (IsZero(poweradd))
			{
				*frad = *x = *z = 0.0f;
				*y = 1.0f;
				return 0;
			}

			if (IsZero(poweradd - 1.0f))
			{
				*x = vx;
				*y = vy;
				*z = vz;
			}
			else
			{
				len = fsqrt(poweradd);
				*x = vx / len;
				*y = vy / len;
				*z = vz / len;
			}
			*frad = GetAngle(fSin, fCos);
			return 1;
			break;
			// internal error
		default:
			*frad = *x = *z = 0.0;
			*y = 1.0f;
			return 0;
			break;
		}

		poweradd = vx * vx + vy * vy + vz * vz;
		if (IsZero(poweradd))
		{
			*frad = *x = *z = 0.0f;
			*y = 1.0f;
			return 0;
		}

		if (!IsZero(poweradd - 1.0f))
		{
			len = fsqrt(poweradd);
			vx /= len;
			vy /= len;
			vz /= len;
		}

		float power;

		if (!IsZero(vx))
		{
			fSin = (_12 - _21) / (2.0f * vx);
			power = vx * vx;
			fCos = (_00 - power) / (1.0f - power);
		}
		else if (!IsZero(vy))
		{
			fSin = (_20 - _02) / (2.0f * vy);
			power = vy * vy;
			fCos = (_11 - power) / (1.0f - power);
		}
		else if (!IsZero(vz))
		{
			fSin = (_01 - _10) / (2.0f * vz);
			power = vz * vz;
			fCos = (_22 - power) / (1.0f - power);
		}
		else
		{
			*frad = *x = *z = 0.0f;
			*y = 1.0f;
			return 0;
		}

		// invalid fSin/fCos value
		if (fSin > 1.0f)
			fSin = 1.0f;

		if (fSin < -1.0f)
			fSin = -1.0f;

		if (fCos > 1.0f)
			fCos = 1.0f;

		if (fCos < -1.0f)
			fCos = -1.0f;

		/*	if ( Abs(fSin) > 1.0001f || Abs(fCos) > 1.0001f )
			{
				*frad = *x = *z = 0.0f;
				*y = 1.0f;
				return 0;
			}
		*/
		*frad = GetAngle(fSin, fCos);
		*x = vx; *y = vy; *z = vz;
		return 1;
	}

	int RtgMatrix16::GetOrientation(RtgOrientation& orient)
	{
		return GetRotate(&orient.ang, &orient.x, &orient.y, &orient.z);
	}

	void RtgMatrix16::QuaternionFromMatrix(RtgVertex4& out)
	{
		float  tr, s, q[4];
		int    i, j, k;
		int nxt[3] = { 1, 2, 0 };
		float m[4][4];
		GetMatrix(&m[0][0]);
		tr = m[0][0] + m[1][1] + m[2][2];

		// check the diagonal
		if (tr > 0.0)
		{
			s = fsqrt(tr + 1.0f);
			out.w = s / 2.0f;
			s = 0.5f / s;
			out.x = (m[1][2] - m[2][1]) * s;
			out.y = (m[2][0] - m[0][2]) * s;
			out.z = (m[0][1] - m[1][0]) * s;
		}
		else
		{
			// diagonal is negative
			i = 0;
			if (m[1][1] > m[0][0]) i = 1;
			if (m[2][2] > m[i][i]) i = 2;
			j = nxt[i];
			k = nxt[j];
			s = fsqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0);

			q[i] = s * 0.5f;

			if (s != 0.0f) s = 0.5f / s;
			q[3] = (m[j][k] - m[k][j]) * s;
			q[j] = (m[i][j] + m[j][i]) * s;
			q[k] = (m[i][k] + m[k][i]) * s;

			out.x = q[0];
			out.y = q[1];
			out.z = q[2];
			out.w = q[3];
		}
		//out.w = -out.w;
	}

	void RtgMatrix16::NormalizeMatrix(void)
	{
		float l;
		l = fsqrt(_00 * _00 + _01 * _01 + _02 * _02);
		if (l > DELTA)
		{
			_00 /= l;
			_01 /= l;
			_02 /= l;
		}
		else
		{
			_00 = 1.f;
			_01 = _02 = 0.f;
		}

		l = fsqrt(_10 * _10 + _11 * _11 + _12 * _12);
		if (l > DELTA)
		{
			_10 /= l;
			_11 /= l;
			_12 /= l;
		}
		else
		{
			_11 = 1.f;
			_10 = _12 = 0.f;
		}

		l = fsqrt(_20 * _20 + _21 * _21 + _22 * _22);
		if (l > DELTA)
		{
			_20 /= l;
			_21 /= l;
			_22 /= l;
		}
		else
		{
			_22 = 1.f;
			_20 = _21 = 0.f;
		}
	}

	void RtgMatrix16::MatrixFromQuaternion(RtgVertex4 v)
	{
		RtgMatrix16 mat;
		float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

		x2 = v.x + v.x;
		y2 = v.y + v.y;
		z2 = v.z + v.z;
		xx = v.x * x2;
		xy = v.x * y2;
		xz = v.x * z2;
		yy = v.y * y2;
		yz = v.y * z2;
		zz = v.z * z2;
		wx = v.w * x2;
		wy = v.w * y2;
		wz = v.w * z2;

		mat._00 = 1.0f - (yy + zz);
		mat._10 = xy - wz;
		mat._20 = xz + wy;
		mat._30 = 0.0f;

		mat._01 = xy + wz;
		mat._11 = 1.0f - (xx + zz);
		mat._21 = yz - wx;
		mat._31 = 0.0f;

		mat._02 = xz - wy;
		mat._12 = yz + wx;
		mat._22 = 1.0f - (xx + yy);
		mat._32 = 0.0f;

		mat._03 = 0.0f;
		mat._13 = 0.0f;
		mat._23 = 0.0f;
		mat._33 = 1.0f;

		(*this) = mat * (*this);
	}

	void RtgMatrix16::Fix(void)
	{
#define ZERO 0.0001f
#define MAKEZERO(A) { if ( Abs(A)<ZERO ) A=0.0f; }
		float* M = &_00;
		for (int i = 0; i < 16; i++)
			MAKEZERO(M[i]);
	}

	void RtgMatrix16::SetMatrix(const float* matrix)
	{
		memcpy(&_00, matrix, sizeof(float) * 16);
	}

	// get matrix to float matrix[16]
	void RtgMatrix16::GetMatrix(float* matrix) const
	{
		memcpy(matrix, &_00, sizeof(float) * 16);
	}

	void RtgMatrix16::GetMatrix(double* matrixd) const
	{
		float matrixf[16];
		GetMatrix(matrixf);
		float* pointer = matrixf;
		for (int i = 0; i < 16; i++)
		{
			matrixd[i] = (double)matrixf[i];
		}
	}

	float RtgMatrix16::GetElement(const int row, const int column) const
	{
		float* pointer = (float*)&_00;
		return pointer[row * 4 + column];
	}

	void RtgMatrix16::SetElement(const int row, const int column, const float value)
	{
		float* pointer = &_00;
		pointer[row * 4 + column] = value;
	}

	RtgVertex3 RtgMatrix16::MapRotation(RtgVertex3 v)
	{
		RtgVertex3 result;
		result.x = v.x * _00 + v.y * _10 + v.z * _20;
		result.y = v.x * _01 + v.y * _11 + v.z * _21;
		result.z = v.x * _02 + v.y * _12 + v.z * _22;
		return result;
	}

	void RtgMatrix16::MatrixFromFace(RtgVertex3& f)
	{
		RtgVertex3 face(-f.x, -f.y, -f.z);
		RtgVertex3 up(0.0f, 0.0f, 1.0f);
		RtgVertex3 right = rtgVectorCross(face, up);
		rtgVectorCross(up, right, face);
		SetRow(0, right);
		SetRow(1, face);
		SetRow(2, up);
	}

	void RtgMatrix16::Output()
	{
		RtCoreLog().Info("Matrix16\n%8.4f %8.4f %8.4f %8.4f\n%8.4f %8.4f %8.4f %8.4f\n%8.4f %8.4f %8.4f %8.4f\n%8.4f %8.4f %8.4f %8.4f\n",
			_00, _01, _02, _03, _10, _11, _12, _13, _20, _21, _22, _23, _30, _31, _32, _33);
	}

	//
	// Matrix - M12
	//

	void rtgM12MakeUnit(float dest[12])
	{
		dest[0] = 1.0f;   dest[1] = 0.0f;   dest[2] = 0.0f;
		dest[3] = 0.0f;   dest[4] = 1.0f;   dest[5] = 0.0f;
		dest[6] = 0.0f;   dest[7] = 0.0f;   dest[8] = 1.0f;
		dest[9] = 0.0f;   dest[10] = 0.0f;   dest[11] = 0.0f;
	}

	void rtgM12Assign(float dest[12], float m[12])
	{
		::memcpy(dest, m, sizeof(float) * 12);
	}

	void rtgM12AssignM16(float dest[12], float m[16])
	{
		dest[0] = m[0];   dest[1] = m[1];   dest[2] = m[2];
		dest[3] = m[4];   dest[4] = m[5];   dest[5] = m[6];
		dest[6] = m[8];   dest[7] = m[9];   dest[8] = m[10];
		dest[9] = m[12];  dest[10] = m[13];  dest[11] = m[14];
	}

	// counterclockwise rotate matrix about X-axis
	//     [0]  1  [1]  0   [2]  0
	// m * [3]  0  [4] cos  [5] sin 
	//     [6]  0  [7]-sin  [8] cos
	//     [9]  0 [10]  0  [11]  0
	void rtgM12RotateX(float m[12], float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float t[8];

		t[0] = m[1];  t[1] = m[2];
		t[2] = m[4];  t[3] = m[5];
		t[4] = m[7];  t[5] = m[8];
		t[6] = m[10]; t[7] = m[11];

		m[1] = t[0] * ca - t[1] * sa;   m[2] = t[0] * sa + t[1] * ca;
		m[4] = t[2] * ca - t[3] * sa;   m[5] = t[2] * sa + t[3] * ca;
		m[7] = t[4] * ca - t[5] * sa;   m[8] = t[4] * sa + t[5] * ca;
		m[10] = t[6] * ca - t[7] * sa;   m[11] = t[6] * sa + t[7] * ca;
	}

	// counterclockwise rotate matrix about Y-axis
	//     [0] cos  [1]  0  [2]-sin
	// m * [3]  0   [4]  1  [5]  0 
	//     [6] sin  [7]  0  [8] cos
	//     [9]  0  [10]  0 [11]  0
	void rtgM12RotateY(float m[12], float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float t[8];

		t[0] = m[0];  t[1] = m[2];
		t[2] = m[3];  t[3] = m[5];
		t[4] = m[6];  t[5] = m[8];
		t[6] = m[9];  t[7] = m[11];

		m[0] = t[0] * ca + t[1] * sa;   m[2] = t[1] * ca - t[0] * sa;
		m[3] = t[2] * ca + t[3] * sa;   m[5] = t[3] * ca - t[2] * sa;
		m[6] = t[4] * ca + t[5] * sa;   m[8] = t[5] * ca - t[4] * sa;
		m[9] = t[6] * ca + t[7] * sa;   m[11] = t[7] * ca - t[6] * sa;
	}

	// counterclockwise rotate matrix about Z-axis
	//     [0] cos  [1] sin [2]  0
	// m * [3]-sin  [4] cos [5]  0 
	//     [6]  0   [7]  0  [8]  1
	//     [9]  0  [10]  0 [11]  0
	void rtgM12RotateZ(float m[12], float degree)
	{
		float ca = fcos(degree);
		float sa = fsin(degree);
		float t[8];

		t[0] = m[0];  t[1] = m[1];
		t[2] = m[3];  t[3] = m[4];
		t[4] = m[6];  t[5] = m[7];
		t[6] = m[9];  t[7] = m[10];

		m[0] = t[0] * ca - t[1] * sa;   m[1] = t[0] * sa + t[1] * ca;
		m[3] = t[2] * ca - t[3] * sa;   m[4] = t[2] * sa + t[3] * ca;
		m[6] = t[4] * ca - t[5] * sa;   m[7] = t[4] * sa + t[5] * ca;
		m[9] = t[6] * ca - t[7] * sa;   m[10] = t[6] * sa + t[7] * ca;
	}

	/*----------------------------------------------
	  form a rotation matrix with an arbitrary axis
	  (the matrix is in M12 format)
	-----------------------------------------------*/
	void rtgM12MakeArbitraryRotate(float M[12], float xyz[3], float dxyz[3], float angle)
	{
		float M4[16];

		rtgM16MakeArbitraryRotate(M4, xyz, dxyz, angle);
		M[0] = M4[0];  M[1] = M4[1];  M[2] = M4[2];
		M[3] = M4[4];  M[4] = M4[5];  M[5] = M4[6];
		M[6] = M4[8];  M[7] = M4[9];  M[8] = M4[10];
		M[9] = M4[12]; M[10] = M4[13]; M[11] = M4[14];
	}

	// make a counterclockwise rotation matrix about X-axis
	// [0]  1  [1]  0   [2]  0
	// [3]  0  [4] cos  [5] sin 
	// [6]  0  [7]-sin  [8] cos
	// [9]  0 [10]  0  [11]  0
	void rtgM12MakeRotateX(float dest[12], float degree)
	{
		dest[0] = 1.0f;
		dest[1] = 0.0f;
		dest[2] = 0.0f;
		dest[3] = 0.0f;
		dest[4] = fcos(degree);
		dest[5] = fsin(degree);
		dest[6] = 0.0f;
		dest[7] = -dest[5];
		dest[8] = dest[4];
		dest[9] = 0.0f;
		dest[10] = 0.0f;
		dest[11] = 0.0f;
	}

	// make a counterclockwise rotation matrix about Y-axis
	// [0] cos  [1]  0  [2]-sin
	// [3]  0   [4]  1  [5]  0 
	// [6] sin  [7]  0  [8] cos
	// [9]  0  [10]  0 [11]  0
	void rtgM12MakeRotateY(float dest[12], float degree)
	{
		dest[0] = fcos(degree);
		dest[6] = fsin(degree);

		dest[1] = 0.0f;
		dest[2] = -dest[6];
		dest[3] = 0.0f;
		dest[4] = 1.0f;
		dest[5] = 0.0f;
		dest[7] = 0.0f;
		dest[8] = dest[0];
		dest[9] = 0.0f;
		dest[10] = 0.0f;
		dest[11] = 0.0f;
	}

	// make a counterclockwise rotation matrix about X-axis
	// [0] cos  [1] sin [2]  0
	// [3]-sin  [4] cos [5]  0 
	// [6]  0   [7]  0  [8]  1
	// [9]  0  [10]  0 [11]  0
	void rtgM12MakeRotateZ(float dest[12], float degree)
	{
		dest[0] = fcos(degree);
		dest[1] = fsin(degree);
		dest[2] = 0.0f;
		dest[3] = -dest[1];
		dest[4] = dest[0];
		dest[5] = 0.0f;
		dest[6] = 0.0f;
		dest[7] = 0.0f;
		dest[8] = 1.0f;
		dest[9] = 0.0f;
		dest[10] = 0.0f;
		dest[11] = 0.0f;
	}

	void rtgM12MakeTransformationYZ(float dest[12], float pos[3], float y[3], float z[3])
	{
		float d1 = y[0] * y[0] + y[1] * y[1];
		float d2 = d1 + y[2] * y[2];
		float d3 = rtgVectorLength(z);

		d1 = fsqrt(d1);
		d2 = fsqrt(d2);

		// Check if error data assigned
		if (IsZero(d1) || IsZero(d2)) {
			rtgM12MakeUnit(dest);
			return;
		}

		float ca, sa, cb, sb;
		if (IsZero(d1)) { // dir of y is parallel z axis
			ca = 1.0f;
			sa = 0.0f;
			cb = 1.0f;
			sb = 0.0f;
		}
		else {
			ca = y[1] / d1;
			sa = -y[0] / d1;
			cb = d1 / d2;
			sb = y[2] / d2;
		}

		CHECK(d3 != 0.0f);

		float cr = (sa * sb * z[0] - ca * sb * z[1] + cb * z[2]) / d3;
		float sr = 1.0f - cr * cr;

		if (IsZero(sr)) sr = 0.0f;
		else             sr = fsqrt(sr);

		dest[0] = ca * cr - sa * sb * sr;
		dest[1] = -sa * cb;
		dest[2] = ca * sr + sa * sb * cr;
		dest[3] = sa * cr + cr * sb * sr;
		dest[4] = ca * cb;
		dest[5] = sa * sr - ca * sb * cr;
		dest[6] = -cb * sr;
		dest[7] = sb;
		dest[8] = cb * cr;
		dest[9] = -(pos[0] * dest[0] + pos[1] * dest[3] + pos[2] * dest[6]);
		dest[10] = -(pos[0] * dest[1] + pos[1] * dest[4] + pos[2] * dest[7]);
		dest[11] = -(pos[0] * dest[2] + pos[1] * dest[5] + pos[2] * dest[8]);
	}

	/*-------------------------------------------------------------------
	  solve the inverse matrix of a M16 matrix
	  I update the method from the one in the book "Numeric Recipe in C"
	--------------------------------------------------------------------*/
	void rtgM12MakeInverse(float dest[12], float m[12])
	{
		int i, j, indx[4];
		float tmp[16], column[4], d;

		// Backup the matrix for decomposition
		rtgM16AssignM12(tmp, m);

		// Perform LU decomposition
		rtgM16LUDecomposition(tmp, indx, &d, 4);

		// Backward substitute to get the answer
		for (j = 0; j < 4; j++) {
			for (i = 0; i < 4; i++)
				column[i] = 0.0f;
			column[j] = 1.0f;

			rtgM16LUBackSubstitute(tmp, indx, column, 4);

			if (j != 3) {
				for (i = 0; i < 4; i++) {
					dest[i * 3 + j] = column[i];
				}
			}
		}
	}

	void rtgM12Multiply(float dest[12], float m1[12], float m2[12])
	{
		float fff12[12];
		float* dest1 = dest;

		if (dest == m1 || dest == m2) dest1 = fff12;

		dest1[0] = m1[0] * m2[0] + m1[1] * m2[3] + m1[2] * m2[6];
		dest1[1] = m1[0] * m2[1] + m1[1] * m2[4] + m1[2] * m2[7];
		dest1[2] = m1[0] * m2[2] + m1[1] * m2[5] + m1[2] * m2[8];
		dest1[3] = m1[3] * m2[0] + m1[4] * m2[3] + m1[5] * m2[6];
		dest1[4] = m1[3] * m2[1] + m1[4] * m2[4] + m1[5] * m2[7];
		dest1[5] = m1[3] * m2[2] + m1[4] * m2[5] + m1[5] * m2[8];
		dest1[6] = m1[6] * m2[0] + m1[7] * m2[3] + m1[8] * m2[6];
		dest1[7] = m1[6] * m2[1] + m1[7] * m2[4] + m1[8] * m2[7];
		dest1[8] = m1[6] * m2[2] + m1[7] * m2[5] + m1[8] * m2[8];
		dest1[9] = m1[9] * m2[0] + m1[10] * m2[3] + m1[11] * m2[6] + m2[9];
		dest1[10] = m1[9] * m2[1] + m1[10] * m2[4] + m1[11] * m2[7] + m2[10];
		dest1[11] = m1[9] * m2[2] + m1[10] * m2[5] + m1[11] * m2[8] + m2[11];

		if (dest1 == dest) return;

		::memcpy((void*)dest, (void*)dest1, 12 * sizeof(float));
	}

	// ---------------------------------------------------------------
	//  form a transformation matrix by position, face direction, 
	//  up direction, and scale
	// ---------------------------------------------------------------
	void rtgMakeMatrix(float* M, float* pos, float* fDir, float* uDir, float* scale)
	{
		float len, l, d[3], u[3], csZ, snZ, csX, snX, csY, snY, s[3];
		float M2[16], Rz[16], Rx[16], Ry[16], M1[16], M2Inv[16], MP[16];

		/* normalize the fDir & uDir */
		if (fDir != NULL)
		{
			len = fsqrt(fDir[0] * fDir[0] + fDir[1] * fDir[1] + fDir[2] * fDir[2]);

			CHECK(len != 0.0f);

			d[0] = fDir[0] / len;
			d[1] = fDir[1] / len;
			d[2] = fDir[2] / len;
		}
		else
		{
			d[0] = 0.0f;
			d[1] = 0.0f;
			d[2] = -1.0f;
		}

		if (uDir != NULL)
		{
			len = fsqrt(uDir[0] * uDir[0] + uDir[1] * uDir[1] + uDir[2] * uDir[2]);

			CHECK(len != 0.0f);

			u[0] = uDir[0] / len;
			u[1] = uDir[1] / len;
			u[2] = uDir[2] / len;
		}

		/* form the rotation matrices - M1 & M2 */
		l = fsqrt(d[0] * d[0] + d[1] * d[1]);
		csX = l;
		snX = d[2];

		l = fsqrt(d[0] * d[0] + d[1] * d[1]);
		if (IsZero(l))
		{
			csZ = d[1] / l;
			snZ = d[0] / l;

			/* form the Rz[A+180] */
			rtgM16MakeUnit(Rz);
			Rz[0] = Rz[5] = -csZ;
			Rz[1] = -snZ;
			Rz[4] = snZ;

			/* form the Rx[A] */
			rtgM16MakeUnit(Rx);
			Rx[5] = Rx[10] = csX;
			Rx[6] = snX;
			Rx[9] = -snX;

			/* get the M1 */
			rtgM16Multiply(M1, Rz, Rx);
		}
		else {
			/* in this case, that Rz is no existing */
			rtgM16MakeUnit(Rx);
			if (d[2] > 0.0f) {
				Rx[5] = Rx[10] = 0.0f;
				Rx[6] = 1.0f;
				Rx[9] = -1.0f;
			}
			else {
				Rx[5] = Rx[10] = 0.0f;
				Rx[6] = -1.0f;
				Rx[9] = 1.0f;
			}

			/* get M1 */
			memcpy(M1, Rx, sizeof(float) * 16);
		}

		/* convert u using M1 */
		if (uDir != NULL)
		{
			s[0] = u[0] * M1[0] + u[1] * M1[4] + u[2] * M1[8];
			s[1] = u[0] * M1[1] + u[1] * M1[5] + u[2] * M1[9];
			s[2] = u[0] * M1[2] + u[1] * M1[6] + u[2] * M1[10];
			csY = s[2];
			snY = s[0];

			/* form the Ry[-A] */
			rtgM16MakeUnit(Ry);
			Ry[0] = Ry[10] = csY;
			Ry[2] = snY;
			Ry[8] = -snY;
		}
		else {
			rtgM16MakeUnit(Ry);
		}

		/* get the M2 */
		rtgM16Multiply(M2, M1, Ry);

		/* get the M2's inverse */
		rtgM16MakeInverse(M2Inv, M2); // M2Inv

		/* form Mscale */
		rtgM16MakeUnit(MP);

		MP[0] = scale[0];
		MP[5] = scale[1];
		MP[10] = scale[2];

		/* form M' */
		//LZAxisConvert(MP, node->faceDir, node->upDir, NEG_Y_AXIS, Z_AXIS);

		/*                         -1
		get the result = M' * M2    */
		rtgM16Multiply(MP, M2Inv, MP);

		M[0] = MP[0]; M[1] = MP[1]; M[2] = MP[2];
		M[3] = MP[4]; M[4] = MP[5]; M[5] = MP[6];
		M[6] = MP[8]; M[7] = MP[9]; M[8] = MP[10];
		M[9] = MP[12] + pos[0];
		M[10] = MP[13] + pos[1];
		M[11] = MP[14] + pos[2];
	}


	//
	// M16
	//
	void rtgM16MakeUnit(float dest[12])
	{
		dest[0] = 1.0f;   dest[1] = 0.0f;   dest[2] = 0.0f;   dest[3] = 0.0f;
		dest[4] = 0.0f;   dest[5] = 1.0f;   dest[6] = 0.0f;   dest[7] = 0.0f;
		dest[8] = 0.0f;   dest[9] = 0.0f;   dest[10] = 1.0f;   dest[11] = 0.0f;
		dest[12] = 0.0f;   dest[13] = 0.0f;   dest[14] = 0.0f;   dest[15] = 1.0f;
	}

	void rtgM16Assign(float dest[16], float m[16])
	{
		::memcpy(dest, m, sizeof(float) * 16);
	}

	void rtgM16AssignM12(float dest[16], float m[12])
	{
		dest[0] = m[0];   dest[1] = m[1];   dest[2] = m[2];   dest[3] = 0.0f;
		dest[4] = m[3];   dest[5] = m[4];   dest[6] = m[5];   dest[7] = 0.0f;
		dest[8] = m[6];   dest[9] = m[7];   dest[10] = m[8];   dest[11] = 0.0f;
		dest[12] = m[9];   dest[13] = m[10];  dest[14] = m[11];  dest[15] = 1.0f;
	}

	// make a counterclockwise rotation matrix about X-axis
	// [0]  1  [1]  0   [2]  0   [3] 0
	// [4]  0  [5] cos  [6] sin  [7] 0
	// [8]  0  [9]-sin [10] cos [11] 0
	//[12]  0 [13]  0  [14]  0  [15] 1
	void rtgM16MakeRotateX(float dest[16], float degree)
	{
		dest[0] = 1.0f;
		dest[1] = 0.0f;
		dest[2] = 0.0f;
		dest[3] = 0.0f;
		dest[4] = 0.0f;
		dest[5] = fcos(degree);
		dest[6] = fsin(degree);
		dest[7] = 0.0f;
		dest[8] = 0.0f;
		dest[9] = -dest[5];
		dest[10] = dest[6];
		dest[11] = 0.0f;
		dest[12] = 0.0f;
		dest[13] = 0.0f;
		dest[14] = 0.0f;
		dest[14] = 1.0f;
	}

	// make a counterclockwise rotation matrix about Y-axis
	// [0] cos [1]  0   [2]-sin  [3] 0
	// [4]  0  [5]  1   [6]  0   [7] 0
	// [8] sin [9]  0  [10] cos [11] 0
	//[12]  0 [13]  0  [14]  0  [15] 1
	void rtgM16MakeRotateY(float dest[16], float degree)
	{
		dest[8] = fsin(degree);

		dest[0] = fcos(degree);
		dest[1] = 0.0f;
		dest[2] = -dest[8];
		dest[3] = 0.0f;
		dest[4] = 0.0f;
		dest[5] = 1.0f;
		dest[6] = 0.0f;
		dest[7] = 0.0f;
		dest[9] = 0.0f;
		dest[10] = dest[0];
		dest[11] = 0.0f;
		dest[12] = 0.0f;
		dest[13] = 0.0f;
		dest[14] = 0.0f;
		dest[15] = 1.0f;
	}

	// make a counterclockwise rotation matrix about X-axis
	// [0] cos [1] sin  [2]  0   [3] 0
	// [4]-sin [5] cos  [6]  0   [7] 0
	// [8]  0  [9]  0  [10]  1  [11] 0
	//[12]  0 [13]  0  [14]  0  [15] 1
	void rtgM16MakeRotateZ(float dest[16], float degree)
	{
		dest[0] = fcos(degree);
		dest[1] = fsin(degree);
		dest[2] = 0.0f;
		dest[3] = 0.0f;
		dest[4] = -dest[1];
		dest[5] = dest[0];
		dest[6] = 0.0f;
		dest[7] = 0.0f;
		dest[8] = 0.0f;
		dest[9] = 0.0f;
		dest[10] = 1.0f;
		dest[11] = 0.0f;
		dest[12] = 0.0f;
		dest[13] = 0.0f;
		dest[14] = 0.0f;
		dest[15] = 1.0f;
	}

	/*----------------------------------------------------------------
	  form a rotation matrix with an arbitrary axis

	  Note:
										  -1    -1    -1
		 M = T1 * R1 * R2 * Rx(angle) * R2  * R1  * T1

		 T1 = / 1  0  0 0    R1 = / cs2 -sn2 0 0
				0  1  0 0    -z     sn2  cs2 0 0
				0  0  1 0             0   0  1 0
			   -x -y -z 1 /           0   0  0 1 /

		 R2 = / cs1 0 -sn1 0     Rx = / 1   0  0 0
		  y       0 1   0  0            0  cs sn 0
				sn1 0  cs1 0            0 -sn cs 0
				  0 0   0  1 /          0   0  0 1 /

	  updated from Geometry Library v1.0 (C)1992-1998,Chuan-Chang Wang
	 -----------------------------------------------------------------*/
	void rtgM16MakeArbitraryRotate(float M[16], float xyz[3], float dxyz[3], float angle)
	{
		float cs1, cs2, sn1, sn2, cs, sn, l1, l2;
		float M1[16], M2[16], Ma[16];

		if (dxyz[0] == 0.0f && dxyz[1] == 0.0f) {
			rtgM16MakeUnit(M1);
			M1[12] = -xyz[0];
			M1[13] = -xyz[1];
			M1[14] = -xyz[2];
			if (dxyz[2] > 0.0f) {
				rtgM16MakeRotateZ(Ma, angle);
			}
			else if (dxyz[2] < 0.0f) {
				rtgM16MakeRotateZ(Ma, -angle);
			}
			rtgM16Multiply(M, M1, Ma);
			M[12] += xyz[0];
			M[13] += xyz[1];
			M[14] += xyz[2];
			return;
		}

		cs = (float)cos(angle * PI / 180.0f);
		sn = (float)sin(angle * PI / 180.0f);
		rtgM16MakeUnit(Ma);
		Ma[5] = Ma[10] = cs;
		Ma[6] = sn;
		Ma[9] = -sn;

		l1 = fsqrt(dxyz[0] * dxyz[0] + dxyz[1] * dxyz[1] + dxyz[2] * dxyz[2]);
		l2 = fsqrt(dxyz[0] * dxyz[0] + dxyz[1] * dxyz[1]);

		CHECK(l1 != 0.0f);
		CHECK(l2 != 0.0f);

		cs1 = l2 / l1;
		sn1 = dxyz[2] / l1;
		cs2 = dxyz[0] / l2;
		sn2 = dxyz[1] / l2;

		M1[0] = cs2 * cs1;
		M1[1] = -sn2;
		M1[2] = -cs2 * sn1;
		M1[3] = 0.0f;
		M1[4] = sn2 * cs1;
		M1[5] = cs2;
		M1[6] = -sn1 * sn2;
		M1[7] = 0.0f;
		M1[8] = sn1;
		M1[9] = 0.0f;
		M1[10] = cs1;
		M1[11] = 0.0f;
		M1[12] = -xyz[0] * cs2 * cs1 - xyz[1] * sn2 * cs1 - xyz[2] * sn1;
		M1[13] = xyz[0] * sn2 - xyz[1] * cs2;
		M1[14] = xyz[0] * sn1 * cs2 + xyz[1] * sn1 * sn2 - xyz[2] * cs1;
		M1[15] = 1.0f;

		M2[0] = cs1 * cs2;
		M2[1] = cs1 * sn2;
		M2[2] = sn1;
		M2[3] = 0.0f;
		M2[4] = -sn2;
		M2[5] = cs2;
		M2[6] = 0.0f;
		M2[7] = 0.0f;
		M2[8] = -sn1 * cs2;
		M2[9] = -sn1 * sn2;
		M2[10] = cs1;
		M2[11] = 0.0f;
		M2[12] = xyz[0];
		M2[13] = xyz[1];
		M2[14] = xyz[2];
		M2[15] = 1.0f;

		rtgM16Multiply(M, M1, Ma);
		rtgM16Multiply(M, M, M2);
	}

	/*-------------------------------------------------------------------
	  solve the inverse matrix of a M16 matrix
	  I update the method from the one in the book "Numeric Recipe in C"
	--------------------------------------------------------------------*/
	void rtgM16MakeInverse(float IM[16], float M[16])
	{
		int i, j, indx[4];
		float tmp[16], column[4], d;

		// Backup the matrix for decomposition
		rtgM16Assign(tmp, M);

		// Perform LU decomposition
		rtgM16LUDecomposition(tmp, indx, &d, 4);

		// Backward substitute to get the answer
		for (j = 0; j < 4; j++) {
			for (i = 0; i < 4; i++)
				column[i] = 0.0f;
			column[j] = 1.0f;

			rtgM16LUBackSubstitute(tmp, indx, column, 4);

			for (i = 0; i < 4; i++)
				IM[i * 4 + j] = column[i];
		}
	}

	/*-------------------------------------------------------------------
	  solve the inverse matrix of a M16 matrix
	  I update the method from the one in the book "Numeric Recipe in C"
	--------------------------------------------------------------------*/
	void rtgM16MakeInverseM12(float dest[16], float m[12])
	{
		int i, j, indx[4];
		float tmp[16], column[4], d;

		// Backup the matrix for decomposition
		rtgM16AssignM12(tmp, m);

		// Perform LU decomposition
		rtgM16LUDecomposition(tmp, indx, &d, 4);

		// Backward substitute to get the answer
		for (j = 0; j < 4; j++) {
			for (i = 0; i < 4; i++)
				column[i] = 0.0f;
			column[j] = 1.0f;

			rtgM16LUBackSubstitute(tmp, indx, column, 4);

			for (i = 0; i < 4; i++)
				dest[i * 4 + j] = column[i];
		}
	}

	void rtgM16Multiply(float dest[16], float m1[16], float m2[16])
	{
		float fff16[16];
		float* dest1 = dest;

		if (dest == m1 || dest == m2) dest1 = fff16;

		dest1[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
		dest1[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
		dest1[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
		dest1[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];
		dest1[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
		dest1[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
		dest1[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
		dest1[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];
		dest1[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
		dest1[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
		dest1[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
		dest1[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];
		dest1[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
		dest1[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
		dest1[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
		dest1[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];

		if (dest1 == dest) return;

		rtgM16Assign(dest, dest1);
	}

	/*-----------------------------
	  LU decomposition of a matrix
	  matrix size is 16x16 maximum
	------------------------------*/
	void rtgM16LUDecomposition(float* m, int* indx, float* d, int dim)
	{
		int i, j, k, iMax;
		float big, dum, sum, tmp, value, v[16];

		/* No row interchanges yet */
		*d = 1.0f;

		/* loop over rows to get implicit scaling information */
		for (i = 0; i < dim; i++) {
			big = 0.0f;
			for (j = 0; j < dim; j++) {
				value = m[i * dim + j];
				if ((tmp = Abs(value)) > big) {
					big = tmp;
				}
			}

			CHECK(big != 0.0f);
			v[i] = 1.0f / big;
		}

		/* loop over columns of Crout's method */
		for (j = 0; j < dim; j++) {
			for (i = 0; i < j; i++) {
				sum = m[i * dim + j];
				for (k = 0; k < i; k++) {
					sum -= (m[i * dim + k] * m[k * dim + j]);
				}
				m[i * dim + j] = sum;
			}

			big = 0.0f;
			for (i = j; i < dim; i++) {
				sum = *(m + i * dim + j);
				for (k = 0; k < j; k++) {
					sum -= m[i * dim + k] * m[k * dim + j];
				}

				m[i * dim + j] = sum;
				if ((dum = v[i] * Abs(sum)) >= big) {
					big = dum;
					iMax = i;
				}
			}

			if (j != iMax) {
				for (k = 0; k < dim; k++) {
					dum = m[iMax * dim + k];
					m[iMax * dim + k] = m[j * dim + k];
					m[j * dim + k] = dum;
				}
				*d = -(*d);
				v[iMax] = v[j];
			}

			indx[j] = iMax;
			if (m[j * dim + j] == 0.0f) {
				m[j * dim + j] = 0.0000001f;
			}

			if (j != dim - 1) {
				dum = 1.0f / m[j * dim + j];
				for (i = j + 1; i < dim; i++) {
					m[i * dim + j] *= dum;
				}
			}
		}
	}

	/*--------------------------------------
	  backsubstitution for LU decomposition
	  matrix size is 16x16 maximum
	---------------------------------------*/
	void rtgM16LUBackSubstitute(float* m, int* indx, float* b, int dim)
	{
		int i, ii = -1, ip, j;
		float sum;

		for (i = 0; i < dim; i++) {
			ip = indx[i];
			sum = b[ip];
			b[ip] = b[i];
			if (ii != -1)
				for (j = ii; j <= i - 1; j++)
					sum -= m[i * dim + j] * b[j];
			else if (sum)
				ii = i;
			b[i] = sum;
		}

		for (i = dim - 1; i >= 0; i--) {
			sum = b[i];
			for (j = i + 1; j < dim; j++)
				sum -= m[i * dim + j] * b[j];
			CHECK(m[i * dim + i] != 0.0f);
			b[i] = sum / m[i * dim + i];
		}
	}

	void VxM(RtgVertex3& out, RtgVertex3& v, RtgMatrix16& m)
	{
		out.x = v.x * m._00 + v.y * m._10 + v.z * m._20 + m._30;
		out.y = v.x * m._01 + v.y * m._11 + v.z * m._21 + m._31;
		out.z = v.x * m._02 + v.y * m._12 + v.z * m._22 + m._32;
	}

	RtgVertex3 operator*(const RtgVertex3& v, const RtgMatrix16& m)
	{
		RtgVertex3 result;
		result.x = v.x * m._00 + v.y * m._10 + v.z * m._20 + m._30;
		result.y = v.x * m._01 + v.y * m._11 + v.z * m._21 + m._31;
		result.z = v.x * m._02 + v.y * m._12 + v.z * m._22 + m._32;
		return result;
	}

	RtgVertex3 operator* (const RtgVertex3& v, const RtgMatrix12& m)
	{
		RtgVertex3 result;
		result.x = v.x * m._00 + v.y * m._10 + v.z * m._20 + m._30;
		result.y = v.x * m._01 + v.y * m._11 + v.z * m._21 + m._31;
		result.z = v.x * m._02 + v.y * m._12 + v.z * m._22 + m._32;
		return result;
	}

	RtgVertex4 operator*(const RtgVertex4& v, const RtgMatrix16& m)
	{
		RtgVertex4 result;
		result.x = v.x * m._00 + v.y * m._10 + v.z * m._20 + v.w * m._30;
		result.y = v.x * m._01 + v.y * m._11 + v.z * m._21 + v.w * m._31;
		result.z = v.x * m._02 + v.y * m._12 + v.z * m._22 + v.w * m._32;
		result.w = v.x * m._03 + v.y * m._13 + v.z * m._23 + v.w * m._33;
		return result;
	}

	RtgMatrix16 operator*(RtgMatrix16 const& l, RtgMatrix16 const& r)
	{
		RtgMatrix16 result;

		result._00 = l._00 * r._00 + l._01 * r._10 + l._02 * r._20 + l._03 * r._30;
		result._01 = l._00 * r._01 + l._01 * r._11 + l._02 * r._21 + l._03 * r._31;
		result._02 = l._00 * r._02 + l._01 * r._12 + l._02 * r._22 + l._03 * r._32;
		result._03 = l._00 * r._03 + l._01 * r._13 + l._02 * r._23 + l._03 * r._33;

		result._10 = l._10 * r._00 + l._11 * r._10 + l._12 * r._20 + l._13 * r._30;
		result._11 = l._10 * r._01 + l._11 * r._11 + l._12 * r._21 + l._13 * r._31;
		result._12 = l._10 * r._02 + l._11 * r._12 + l._12 * r._22 + l._13 * r._32;
		result._13 = l._10 * r._03 + l._11 * r._13 + l._12 * r._23 + l._13 * r._33;

		result._20 = l._20 * r._00 + l._21 * r._10 + l._22 * r._20 + l._23 * r._30;
		result._21 = l._20 * r._01 + l._21 * r._11 + l._22 * r._21 + l._23 * r._31;
		result._22 = l._20 * r._02 + l._21 * r._12 + l._22 * r._22 + l._23 * r._32;
		result._23 = l._20 * r._03 + l._21 * r._13 + l._22 * r._23 + l._23 * r._33;

		result._30 = l._30 * r._00 + l._31 * r._10 + l._32 * r._20 + l._33 * r._30;
		result._31 = l._30 * r._01 + l._31 * r._11 + l._32 * r._21 + l._33 * r._31;
		result._32 = l._30 * r._02 + l._31 * r._12 + l._32 * r._22 + l._33 * r._32;
		result._33 = l._30 * r._03 + l._31 * r._13 + l._32 * r._23 + l._33 * r._33;

		return result;
	}

	RtgMatrix16 operator*(float r, RtgMatrix16& m)
	{
		RtgMatrix16 ma;
		float* src = &m._00;
		float* dest = &ma._00;

		for (int i = 0; i < 16; i++)
			*dest++ = r * *src++;

		return ma;
	}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
