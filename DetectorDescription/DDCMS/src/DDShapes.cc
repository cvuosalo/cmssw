#include "DetectorDescription/DDCMS/interface/DDShapes.h"

using namespace ddcms;

Shape getCurrentShape(const cms::DDFilteredView &fview) {
  if (fview.isABox())
    return (Shape::Box);
  if (fview.isACone())
    return (Shape::Cone);
  if (fview.isAPseudoTrapezoid())
    return (Shape::PseudoTrapezoid);
  if (fview.isATrapezoid())
    return (Shape::Trapezoid);
  if (fview.isACutTube())
    return (Shape::CutTube);
  if (fview.isATube())
    return (Shape::Tube);
  return (Shape::Null);
}

BoxPars::BoxPars(const DDFilteredView &fv) :
  valid{fv.isABox()}
{
  if (valid) {
    const TGeoBBox* box = fv.getShapePtr<TGeoBBox*>();
    dx = box->GetDX();
    dy = box->GetDY();
    dz = box->GetDZ();
  }
}


// ****** Change like done for BoxPars
  struct ConePars {
bool DDFilteredView::getShapePars(ddcms::ConePars &coneArg) const {
  if (isACone() == false)
    return (false);
  Volume currVol = node_->GetVolume();
  const TGeoCone* cone = static_cast<const TGeoCone *>(currVol->GetShape());
  coneArg.rmin1 = cone->GetRmin1();
  coneArg.rmin2 = cone->GetRmin2();
  coneArg.rmax1 = cone->GetRmax1();
  coneArg.rmax2 = cone->GetRmax2();
  coneArg.phi1 = cone->GetPhi1();
  coneArg.phi2 = cone->GetPhi2();
  coneArg.dz = cone->GetDz();
  return (true);
}

    double dz;
    double rmin1;
    double rmax1;
    double rmin2;
    double rmax2;
    double phi1;
    double phi2;
  };

  struct PseudoTrapezoidPars {
    double dz;
    double xyMinusDz[4][2];
    double xyPlusDz[4][2];
  };

  struct TrapezoidPars {
    double dz;
    double theta;
    double phi;
    double h1;
    double h2;
    double bl1;
    double bl2;
    double alpha1;
    double alpha2;
  };

  struct CutTubePars {
    double dz;
    double rmin;
    double rmax;
    double phi1;
    double phi2;
  };

  struct TubePars {
    double dz;
    double rmin;
    double rmax;
  };

}

#endif
