#ifndef Detector_Description_DDCMS_DDShapes_h
#define Detector_Description_DDCMS_DDShapes_h

#include "DetectorDescription/DDCMS/interface/DDFilteredView.h"

namespace ddcms {
  enum class Shape {
    Null,
    Shapeless,
    Box,
    Cone,
    Sphere,
    Torus,
    Trapezoid,
    PseudoTrapezoid,
    Tube,
    CutTube,
    EllipticalTube,
    Intersection,
    Subtraction,
    UnionShape,
    ExtrudedPolygon
  };

  Shape getCurrentShape(const cms::DDFilteredView &fview);

  class BoxPars {
  public:
    BoxPars(const DDFilteredView &fv);

    double Dx() const  {
      return (dx);
    }
    double Dy() const  {
      return (dy);
    }
    double Dz() const  {
      return (dz);
    }

    const bool valid;

  private:
    double dx;
    double dy;
    double dz;
  };

// *** Revise these like BoxPars
  struct ConePars {
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
