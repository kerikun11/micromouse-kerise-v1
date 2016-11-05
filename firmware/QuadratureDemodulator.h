/*
 * QuadratureDemodulator.h
 *
 *  Created on: 2016/10/28
 *      Author: idt12312
 */

#ifndef QUADRATUREDEMODULATOR_H_
#define QUADRATUREDEMODULATOR_H_

#include <cstdint>
#include <cstddef>
#include <memory>
#include <cmath>
#include <cstdio>

class QuadratureDemodulator {
private:
	// (1<<sacle)がsin, cosを整数で表現するときのfull rangeの値
	const size_t sin_scale = 15;
	const size_t N;
	const size_t n;
	const size_t m;
	std::unique_ptr<int16_t[]> Wc;
	std::unique_ptr<int16_t[]> Ws;

public:
	// Nがnの整数倍になる必要がある
	QuadratureDemodulator(size_t __N, size_t _n) :
			N(__N), n(_n), m(__N / _n), Wc(new int16_t[__N / _n]), Ws(
					new int16_t[__N / _n]) {
		for (size_t i = 0; i < m; i++) {
			Wc[i] = (int16_t)(
					std::cos(-2.0f * M_PI / m * i) * (float) (1 << sin_scale));
			Ws[i] = (int16_t)(
					std::sin(-2.0f * M_PI / m * i) * (float) (1 << sin_scale));
		}
	}
	virtual ~QuadratureDemodulator() {
	}

	uint32_t calc(const int16_t *x) {
		int32_t Xim = 0;
		int32_t Xre = 0;
		for (size_t i = 0; i < m; i++) {
			int32_t xsum = 0;
			for (size_t j = 0; j < n; j++) {
				xsum += x[i + m * j];
			}
			Xre += Wc[i] * xsum;
			Xim += Ws[i] * xsum;
		}

		// sin cosをスケーリングしていたので、その分を戻す
		Xre = Xre >> sin_scale;
		Xim = Xre >> sin_scale;

		// ノルムを計算
		// int32_tのまま2乗するとオーバーフローするのでint64_tに
		const int64_t Xre_f = (int64_t) Xre;
		const int64_t Xim_f = (int64_t) Xim;
		const uint64_t X_square = Xre_f * Xre_f + Xim_f * Xim_f;

		return (uint32_t) std::sqrt(X_square / N);
	}
};

#endif /* QUADRATUREDEMODULATOR_H_ */
