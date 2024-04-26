#include "MuonReco/TrackParam.h"

#include <vector>
#include <cmath>

using namespace std;

// Simple naive way of doing this check -- sufficient for our test cases.
const double EPSILON = 0.01;
bool almost_eq(double a, double b) {

	if(abs(a - b) < EPSILON) return true;

	return false;

}


uint64_t byteSwap(uint64_t data, uint8_t dataSize) {

	uint64_t result = 0;
	for(uint8_t byte = 0; byte < dataSize; ++byte) {

		uint64_t temp = data >> 8 * (dataSize - byte - 1);
		temp = temp & 0xff;

		result += temp << 8 * byte;

	}

	return result;

}

bool testSimple() {

	TrackParam tp;

	vector<double> x = {1, 2, 3, 4, 5};
	vector<double> y = {1, 2, 3, 4, 5};
	vector<double> r = {0, 0, 0, 0, 0};

	double slope;
	double intercept;

	double chiSquare = tp.LeastSquares(x, y, r, &slope, &intercept);

	cout << "(" << slope << ", " << intercept << ")" << endl;

	if(!almost_eq(slope    , 1.)) return false;
	if(!almost_eq(intercept, 0.)) return false;

	return 0;

}

bool testNormal() {

	TrackParam tp;

	vector<double> x = {120.122, 720.822, 705.805, 690.787, 675.77, 645.735, 630.717, 615.7, 600.682};
	vector<double> y = {238.371, 238.371, 212.36, 186.349, 160.337, 123.801, 97.7896, 71.7786, 45.7675};
	vector<double> r = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	double slope;
	double intercept;

	double chiSquare = tp.LeastSquares(x, y, r, &slope, &intercept);

	cout << "(" << slope << ", " << intercept << ")" << endl;

	if(!almost_eq(slope    , -0.0920732)) return false;
	if(!almost_eq(intercept, 208.076   )) return false;

	return 0;

}

bool testNormal2() {

	TrackParam tp;

	vector<double> x = {180.192, 540.612, 870.997, 886.015, 855.98, 825.945, 810.927};
	vector<double> y = {238.371, 238.371, 97.7896, 123.801, 71.7786, 71.7786, 45.7675};
	vector<double> r = {0, 0, 0, 0, 0, 0, 0};

	double slope;
	double intercept;

	double chiSquare = tp.LeastSquares(x, y, r, &slope, &intercept);

	cout << "(" << slope << ", " << intercept << ")" << endl;
	cout << -TMath::ATan(1. / slope) << " " << -intercept / slope << endl;

	if(!almost_eq(slope    , -0.2572)) return false;
	if(!almost_eq(intercept, 309.497)) return false;

	return 0;

}

bool testNormal3() {

	TrackParam tp;
	vector<double> x = {150.157, 90.0875, 540.612, 525.595, 540.612, 525.595, 420.472, 525.595, 540.612, 525.595, 510.577}; 
	vector<double> y = {238.371, 238.371, 238.371, 212.36, 186.349, 160.337, 238.371, 71.7786, 97.7896, 123.801, 45.7675};
	vector<double> r = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	double slope;
	double intercept;

	double chiSquare = tp.LeastSquares(x, y, r, &slope, &intercept);

	cout << "(" << slope << ", " << intercept << ")" << endl;
	cout << -TMath::ATan(1. / slope) << " " << -intercept / slope << endl;

	if(!almost_eq(slope    , -0.2288)) return false;
	if(!almost_eq(intercept, 270.167)) return false;

	return 0;

}

bool testHorizontal() {

	TrackParam tp;

	vector<double> x = {0, 1, 2, 3, 4, 5};
	vector<double> y = {9, 9, 9, 9, 9, 9};
	vector<double> r = {0, 0, 0, 0, 0, 0};

	double slope;
	double intercept;

	double chiSquare = tp.LeastSquares(x, y, r, &slope, &intercept);

	cout << "(" << slope << ", " << intercept << ")" << endl;

	if(!almost_eq(slope    , 0.)) return false;
	if(!almost_eq(intercept, 9 )) return false;

	return 0;

}

int main(int argc, char **argv) {

	testSimple();
	testNormal();
	testNormal2();
	testNormal3();
	testHorizontal();

	return 0;

}