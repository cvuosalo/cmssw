#ifndef RecoBTag_GhostTrackPrediction_h
#define RecoBTag_GhostTrackPrediction_h

#include <cmath>

#include <Math/SVector.h>
#include <Math/SMatrix.h>

#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/GlobalError.h"

namespace reco {

class GhostTrackPrediction {
    public:
	// z0, tIP, dz/dr, phi

	typedef ROOT::Math::SVector<double, 4> Vector;
	typedef ROOT::Math::SMatrix<double, 4, 4,
			ROOT::Math::MatRepSym<double, 4> > Error;

	GhostTrackPrediction() {}
	GhostTrackPrediction(const Vector &prediction, const Error &error) :
		prediction_(prediction), covariance_(error)
	{}

	double z() const { return prediction_[0]; }
	double ip() const { return prediction_[1]; }
	double cotTheta() const { return prediction_[2]; }
	double phi() const { return prediction_[3]; }

	double rho2() const { return cotTheta() * cotTheta() + 1.; }
	double rho() const { return std::sqrt(rho2()); }
	double sz() const { return z() / rho(); }
	double theta() const { return M_PI_2 - std::atan(cotTheta()); }
	double eta() const { return -std::log(rho() - cotTheta()); }

	const Vector &prediction() const { return prediction_; }
	const Error &covariance() const { return covariance_; }

	const GlobalPoint origin() const
	{ return GlobalPoint(-std::sin(phi()) * ip(), std::cos(phi()) * ip(), z()); }
	const GlobalVector direction() const
	{ return GlobalVector(std::cos(phi()), std::sin(phi()), cotTheta()); }

	double lambda(const GlobalPoint &point) const
	{ return (point - origin()) * direction() / rho2(); }

	GlobalPoint position(double lambda = 0.) const
	{ return origin() + lambda * direction(); }
	GlobalError positionError(double lambda = 0.) const;

    private:
	Vector	prediction_;
	Error	covariance_;
};

}
#endif // RecoBTag_GhostTrackPrediction_h
