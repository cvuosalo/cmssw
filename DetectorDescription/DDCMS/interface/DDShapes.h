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
}

#endif
