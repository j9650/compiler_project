#pragma once
#include <random>
#include <vector>
#include <cassert>
template <typename T>
class Vector {
	std::vector<T> v;
public:
	Vector() {
		;
	}
	Vector(int w) {
		v.resize(w);
	}
	void setValue(int idx, T value) {
		v[idx] = value;
	}
	T getValue(int idx) {
		return v[idx];
	}
	inline T& operator [](int idx) {
		return v[idx];
	}

	//inline T operator [](int idx) const {
	//	return v[idx];
	//}

	inline int size() const {
		return v.size();
	}

	void resize(int t) {
		v.resize(t);
	}
};

template <typename T>
class Matrix {
protected:
	int ww, hh;
	std::vector<Vector<T>> mat;
public:	
	Matrix() {
		ww = hh = 0;
	}
	Matrix(int w, int h, int init = 1) {
		std::random_device rd(__TIMESTAMP__);
		ww = w;
		hh = h;
		mat.resize(hh);
		for (auto& vec : mat) {
			vec.resize(ww);
			for (int i = 0; i < ww; i++)
				vec[i] = (init == 0 ? 0 : rd() % 100);
		}
	}

	int height() {
		return hh;
	}

	int weight() {
		return ww;
	}
	inline int size() {
		return hh;
	}

	inline Vector<T>& operator [](int idx) {
		return mat[idx];
	}

	//inline Vector<T> operator [](int idx) const {
	//	return mat[idx];
	//}
/*	static Vector<T>* multi(Vector<T> * v, Matrix<T> * M) {
		Vector<T> vec(v->size());
		assert(v->size() == M->height());
		for (int i = 0; i < v->size(); i++) {
			for (int j = 0; j < v->size(); j++) {
				*(vec)[j] += (*v)[i] * (*M)[i][j];
			}
			
		}
		return vec;
	}
	*/
	static void multiV(Vector<T> * v, Matrix<T> *M, Vector<T>* V) {

		assert(v->size() == M->height());
		for (int i = 0; i < v->size(); i++) {
			for (int j = 0; j < v->size(); j++) {
				(*V)[j] += (*v)[i] * (*M)[i][j];
			}

		}
	}

	static void verify(Matrix<T> *M1, Matrix<T>* M2) {
		for (int i = 0; i < M1->height(); i++) {
			for (int j = 0; j < M2->weight(); j++) {
				assert((*M1)[i][j] == (*M2)[i][j]);
			}
		}
	}
};









class MatrixMultiplicationBaseline {
public:
	int hh;
	int ww;
	MatrixMultiplicationBaseline() {};
	MatrixMultiplicationBaseline(int h, int w) {
		hh = h; ww = w;
	};
	virtual Matrix<int>* multi(Matrix<int> * A, Matrix<int> * B, Matrix<int> * C, Matrix<int> * D) {
		Matrix<int> RAB(A->height(), B->weight(), 0);

		for (int i = 0; i < A->height(); i++) {
			for (int j = 0; j < B->weight(); j++) {
				int sum = 0;
				for (int k = 0; k < B->weight(); k++) {
					RAB[i][j] += (*A)[i][k] * (*B)[k][j];
				}
			}
		}


		for (int i = 0; i < A->height(); i++) {
			for (int j = 0; j < B->weight(); j++) {
				int sum = 0;
				for (int k = 0; k < B->weight(); k++) {
					(*D)[i][j] += RAB[i][k] * (*C)[k][j];
				}
			}
		}
		return D;
	}

	virtual  Matrix<int>*  exec() {

		auto A = new Matrix<int>(hh, ww);
		auto B = new Matrix<int>(hh, ww);
		auto C = new Matrix<int>(hh, ww);
		auto D = new Matrix<int>(hh, ww, 0);

		multi(A, B, C, D);
		return D;
	}


};

class MatrixMultiplication : public MatrixMultiplicationBaseline {
public:

	MatrixMultiplication(int h, int w) {
		hh = h; ww = w;
	};
	virtual Matrix<int>*  multi(Matrix<int> * A, Matrix<int> * B, Matrix<int> * C, Matrix<int> * D) {
		for (int i = 0; i < A->size(); i++) {
			auto vec = new Vector<int>(A->size());
			Matrix<int>::multiV(&((*A)[i]), B, vec);
			Matrix<int>::multiV(vec, C, &((*D)[i]));
		}
		return D;
	}


	virtual  Matrix<int>*  exec() {

		auto A = new Matrix<int>(hh, ww);
		auto B = new Matrix<int>(hh, ww);
		auto C = new Matrix<int>(hh, ww);
		auto D = new Matrix<int>(hh, ww, 0);

		multi(A, B, C, D);
		return D;
	}


};

