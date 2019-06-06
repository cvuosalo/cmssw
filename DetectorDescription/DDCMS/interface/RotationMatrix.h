#ifndef DetectorDescription_DDCMS_RotationMatrix_h
#define DetectorDescription_DDCMS_RotationMatrix_h

#include <Math/Rotation3D.h>
#include <Math/AxisAngle.h>

namespace ddcms {
  //! A RotationMatrix is currently implemented with a ROOT Rotation3D
  using RotationMatrix = ROOT::Math::Rotation3D;
  using AxisAngle = ROOT::Math::AxisAngle;
}

#endif
