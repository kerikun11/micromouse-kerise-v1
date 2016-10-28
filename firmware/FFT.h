/*
 * FFT.h
 *
 *  Created on: 2016/10/27
 *      Author: kerikun11
 */

#ifndef FFT_H_
#define FFT_H_

#include "mbed.h"
#include <algorithm> // swap
#include <complex>

using namespace std;

typedef complex<double> Complex;
typedef unsigned int uint;

static Complex one(1.0, 0.0);
static Complex ione(0.0, 1.0);

class FFT {
public:
	FFT(size_t s):size(s), verbose(false) {
		array = new Complex[size];
	}
	~FFT() {
		delete[] array;
	}

	Complex& operator[](int index) const {
		return array[index];
	}
	bool setVerbose(bool b) {
		return verbose = b;
	}
	void execute(bool isReverse = false);
	void inverse();
private:
	Complex *array;
	size_t size;

	bool verbose; // 配列ダンプのモード指定に使う
	void bitReverse();
};

void FFT::bitReverse() {
	uint k, b, a;
	for (uint i = 0; i < size; i++) {
		k = 0;
		b = size >> 1;
		a = 1;
		while (b >= a) {
			if (b & i)
				k |= a;
			if (a & i)
				k |= b;
			b >>= 1;
			a <<= 1;
		}
		if (i < k)
			swap(array[i], array[k]);
	}
}

void FFT::execute(bool isReverse) {
	bitReverse();
	size_t m = 2;
	Complex w, ww, t;

	while (m <= size) {
		double arg = -2.0 * M_PI / m;
		w = Complex(cos(arg), sin(arg));
		if (isReverse)
			w = one / w; //-1乗 -(-2.0*PI/size) = 2.0*PI/size

		for (uint i = 0; i < size; i += m) {
			ww = 1.0;
			for (uint j = 0; j < m / 2; j++) {
				int a = i + j;
				int b = i + j + m / 2;

				t = ww * array[b];

				array[b] = array[a] - t;
				array[a] = array[a] + t;

				ww *= w;
			}
		}
		m *= 2;
	}
}

void FFT::inverse() {
	execute(true);
	double s = (double) size;
	for (uint i = 0; i < size; i++)
		array[i] /= s;
}

#endif /* FFT_H_ */
