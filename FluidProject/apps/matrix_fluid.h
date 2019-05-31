#pragma once
#include "matrix.h"
#include "../fluid/guard.h"
#include "../fluid/valve.h"
#include "../fluid/fluid.h"
#include "../fluid/guardscheduler.h"
class MatrixMultiplicationFluid : public MatrixMultiplicationBaseline {
public:
	MatrixMultiplicationFluid(int h, int w) {
		hh = h; ww = w;
	};
	virtual  Matrix<int>* multi(Matrix<int>* A, Matrix<int>* B, Matrix<int>* C, Matrix<int>* D) {
		auto tf = new TaskFactory();
		auto gs = new AggressiveGS(1);

		auto tp0 = tf->newTask<void(*)(void *), void *>("Begin", &nullfun, NULL);
		auto tp0status = new __fluid__<bool>(tp0->isfinished());
		auto v0 = new ValveEQ<bool>(tp0status, true);
		
		auto vec = new Vector<int>(A->size());

		for (int i = 0; i < A->size(); i++) {
			auto tpb1 = std::bind(&(Matrix<int>::multiV), &(*A)[i], B, std::placeholders::_1);
			auto tp1 = tf->newTask<decltype(tpb1), Vector<int>*>("multi" + std::to_string(i), tpb1, vec);
			Guard* g1 = gs->newGuard("Guardmulti" + std::to_string(i), {}, tp1, {});

			
			auto tp1status = new __fluid__<bool>(tp1->isfinished());
			auto v1 = new ValveEQ<bool>(tp1status, true);

			auto tpb2 = std::bind(&(Matrix<int>::multiV), vec, C, std::placeholders::_1);
			auto tp2 = tf->newTask<decltype(tpb2), Vector<int>*>("multi" + std::to_string(i), tpb2, &((*D)[i]));
			Guard* g2 = gs->newGuard("Guardmulti" + std::to_string(i), { v1 }, tp2, {});
		}
		auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
		auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
		auto v3 = new ValveEQ<bool>(tp3status, true);
		//while (*(tp3->parent->isfinished()) == false)
		//	; // HARD HACK!
		gs->sync(tp3);
		return D;
	}

	virtual Matrix<int>* exec() {
		auto A = new Matrix<int>(hh, ww);
		auto B = new Matrix<int>(hh, ww);
		auto C = new Matrix<int>(hh, ww);
		auto D = new Matrix<int>(hh, ww, 0);

		multi(A, B, C, D);
		return D;
	}


};
