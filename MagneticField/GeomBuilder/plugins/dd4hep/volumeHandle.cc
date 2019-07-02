// #include "Utilities/Configuration/interface/Architecture.h"

/*
 *  See header file for a description of this class.
 *
 *  \author N. Amapane - INFN Torino
 */

#include "MagneticField/GeomBuilder/plugins/dd4hep/volumeHandle.h"

#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "DataFormats/GeometrySurface/interface/Cone.h"
#include "DataFormats/GeometryVector/interface/CoordinateSets.h"
#include "DataFormats/Math/interface/GeantUnits.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DetectorDescription/DDCMS/interface/DDShapes.h"

#include "MagneticField/Layers/interface/MagVerbosity.h"

#include <string>
#include <iterator>
#include <iomanip>
#include <iostream>
#include <boost/lexical_cast.hpp>

using namespace SurfaceOrientation;
using namespace std;
using namespace magneticfield;
using namespace cms;
using namespace ddcms;
using namespace edm;

volumeHandle::~volumeHandle() {
  if (refPlane != nullptr)
    delete refPlane;
}

volumeHandle::volumeHandle(const DDFilteredView &fv, bool expand2Pi, bool debugVal)
    : name(fv.name()),
      copyno(fv.copyNum()),
      shape(getCurrentShape(fv)),
      magVolume(nullptr),
      masterSector(1),
      theRN(0.),
      theRMin(0.),
      theRMax(0.),
      refPlane(nullptr),
      expand(expand2Pi),
      isIronFlag(false),
      solid(fv),
      debug(debugVal) {
  Double_t transArray[3];
  for (int index = 0; index < 3; ++index) {
    transArray[index] = geant_units::operators::convertMmToCm(fv.trans()[index]);
  }
  center_ = GlobalPoint(transArray[0], transArray[1], transArray[2]);

  // ASSUMPTION: volume names ends with "_NUM" where NUM is the volume number
  string volName = name;
  volName.erase(0, volName.rfind('_') + 1);
  volumeno = boost::lexical_cast<unsigned short>(volName);

  for (int i = 0; i < 6; ++i) {
    isAssigned[i] = false;
  }
  referencePlane(fv);
  switch (shape) {
    case DDSolidShape::ddbox:
      buildBox();
      break;
    case DDSolidShape::ddtrap:
      buildTrap();
      break;
    case DDSolidShape::ddcons:
      buildCons();
      break;
    case DDSolidShape::ddtubs:
      buildTubs();
      break;
    case DDSolidShape::ddtrunctubs:
      buildTruncTubs();
      break;
    case DDSolidShape::ddpseudotrap:
      buildPseudoTrap();
      break;
    default:
      LogWarning("magneticfield::volumeHandle") << "ctor: Unexpected shape # " <<
      static_cast<int>(shape) << " for vol " << name;
  }

  // Get material for this volume
  if (fv.materialName() == "Iron")
    isIronFlag = true;

  if (debug) {
    LogDebug("magneticfield::volumeHandle") << " RMin =  " << theRMin;
    LogDebug("magneticfield::volumeHandle") << " RMax =  " << theRMax;

    if (theRMin < 0 || theRN < theRMin || theRMax < theRN)
      LogDebug("magneticfield::volumeHandle") << "*** WARNING: wrong RMin/RN/RMax";

    LogDebug("magneticfield::volumeHandle")
        << "Summary: " << name << " " << copyno << " DDSolidShape= " << shape << " trasl " << center() << " R "
        << center().perp() << " phi " << center().phi() << " magFile " << magFile << " Material= " << fv.materialName()
        << " isIron= " << isIronFlag << " masterSector= " << masterSector << std::endl;

    LogDebug("magneticfield::volumeHandle") << " Orientation of surfaces:";
    std::string sideName[3] = {"positiveSide", "negativeSide", "onSurface"};
    for (int i = 0; i < 6; ++i) {
      LogDebug("magneticfield::volumeHandle") << "  " << i << ":" << sideName[surfaces[i]->side(center_, 0.3)];
    }
  }
}

const Surface::GlobalPoint &volumeHandle::center() const { return center_; }

void volumeHandle::referencePlane(const DDFilteredView &fv) {
  // The refPlane is the "main plane" for the solid. It corresponds to the
  // x,y plane in the DDD local frame, and defines a frame where the local
  // coordinates are the same as in DDD.
  // In the geometry version 85l_030919, this plane is normal to the
  // beam line for all volumes but pseudotraps, so that global R is along Y,
  // global phi is along -X and global Z along Z:
  //
  //   Global(for vol at pi/2)    Local
  //   +R (+Y)                    +Y
  //   +phi(-X)                   -X
  //   +Z                         +Z
  //
  // For pseudotraps the refPlane is parallel to beam line and global R is
  // along Z, global phi is along +-X and and global Z along Y:
  //
  //   Global(for vol at pi/2)    Local
  //   +R (+Y)                    +Z
  //   +phi(-X)                   +X
  //   +Z                         +Y
  //
  // Note that the frame is centered in the DDD volume center, which is
  // inside the volume for DDD boxes and (pesudo)trapezoids, on the beam line
  // for tubs, cons and trunctubs.

  // In geometry version 1103l, trapezoids have X and Z in the opposite direction
  // than the above.  Boxes are either oriented as described above or in some case
  // have opposite direction for Y and X.

  // The global position
  Surface::PositionType &posResult = center_;

  // The reference plane rotation
  math::XYZVector x, y, z;
  RotationMatrix refRot;
  fv.rot(refRot);
  refRot.GetComponents(x, y, z);
  if (debug) {
    if (x.Cross(y).Dot(z) < 0.5) {
      LogDebug("magneticfield::volumeHandle") << "*** WARNING: Rotation is not RH ";
    }
  }

  // The global rotation
  Surface::RotationType rotResult(float(x.X()),
                                  float(x.Y()),
                                  float(x.Z()),
                                  float(y.X()),
                                  float(y.Y()),
                                  float(y.Z()),
                                  float(z.X()),
                                  float(z.Y()),
                                  float(z.Z()));

  refPlane = new GloballyPositioned<float>(posResult, rotResult);

  // Check correct orientation
  if (debug) {
    LogDebug("magneticfield::volumeHandle") << "Refplane pos  " << refPlane->position();

    // See comments above for the conventions for orientation.
    LocalVector globalZdir(0., 0., 1.);  // Local direction of the axis along global Z
    if (getCurrentDDSolidShape(fv) == DDSolidShape::PseudoTrapezoid) {
      globalZdir = LocalVector(0., 1., 0.);
    }
    if (refPlane->toGlobal(globalZdir).z() < 0.) {
      globalZdir = -globalZdir;
    }
    float chk = refPlane->toGlobal(globalZdir).dot(GlobalVector(0, 0, 1));
    if (chk < .999)
      LogDebug("magneticfield::volumeHandle") << "*** WARNING RefPlane check failed!***" << chk;
  }
}

void volumeHandle::buildPhiZSurf(double startPhi, double deltaPhi, double zhalf, double rCentr) {
  // This is 100% equal for cons and tubs!!!

  GlobalVector planeXAxis = refPlane->toGlobal(LocalVector(1, 0, 0));
  GlobalVector planeYAxis = refPlane->toGlobal(LocalVector(0, 1, 0));
  GlobalVector planeZAxis = refPlane->toGlobal(LocalVector(0, 0, 1));

  // Local Y axis of the faces at +-phi.
  GlobalVector y_phiplus = refPlane->toGlobal(LocalVector(cos(startPhi + deltaPhi), sin(startPhi + deltaPhi), 0.));
  GlobalVector y_phiminus = refPlane->toGlobal(LocalVector(cos(startPhi), sin(startPhi), 0.));

  Surface::RotationType rot_Z(planeXAxis, planeYAxis);
  Surface::RotationType rot_phiplus(planeZAxis, y_phiplus);
  Surface::RotationType rot_phiminus(planeZAxis, y_phiminus);

  GlobalPoint pos_zplus(center_.x(), center_.y(), center_.z() + zhalf);
  GlobalPoint pos_zminus(center_.x(), center_.y(), center_.z() - zhalf);
  // BEWARE: in this case, the origin for phiplus,phiminus surfaces is
  // at radius R and not on a plane passing by center_ orthogonal to the radius.
  GlobalPoint pos_phiplus(
      refPlane->toGlobal(LocalPoint(rCentr * cos(startPhi + deltaPhi), rCentr * sin(startPhi + deltaPhi), 0.)));
  GlobalPoint pos_phiminus(refPlane->toGlobal(LocalPoint(rCentr * cos(startPhi), rCentr * sin(startPhi), 0.)));
  surfaces[zplus] = new Plane(pos_zplus, rot_Z);
  surfaces[zminus] = new Plane(pos_zminus, rot_Z);
  surfaces[phiplus] = new Plane(pos_phiplus, rot_phiplus);
  surfaces[phiminus] = new Plane(pos_phiminus, rot_phiminus);

  if (debug) {
    LogDebug("magneticfield::volumeHandle")
        << "Actual Center at: " << center_ << " R " << center_.perp() << " phi " << center_.phi();
    LogDebug("magneticfield::volumeHandle") << "RN            " << theRN << endl;
    LogDebug("magneticfield::volumeHandle")
        << "pos_zplus    " << pos_zplus << " " << pos_zplus.perp() << " " << pos_zplus.phi() << endl
        << "pos_zminus   " << pos_zminus << " " << pos_zminus.perp() << " " << pos_zminus.phi() << endl
        << "pos_phiplus  " << pos_phiplus << " " << pos_phiplus.perp() << " " << pos_phiplus.phi() << endl
        << "pos_phiminus " << pos_phiminus << " " << pos_phiminus.perp() << " " << pos_phiminus.phi() << endl;

    LogDebug("magneticfield::volumeHandle") << "y_phiplus " << y_phiplus << endl;
    LogDebug("magneticfield::volumeHandle") << "y_phiminus " << y_phiminus << endl;

    LogDebug("magneticfield::volumeHandle")
        << "rot_Z " << surfaces[zplus]->toGlobal(LocalVector(0., 0., 1.)) << endl
        << "rot_phi+ " << surfaces[phiplus]->toGlobal(LocalVector(0., 0., 1.)) << " phi "
        << surfaces[phiplus]->toGlobal(LocalVector(0., 0., 1.)).phi() << endl
        << "rot_phi- " << surfaces[phiminus]->toGlobal(LocalVector(0., 0., 1.)) << " phi "
        << surfaces[phiminus]->toGlobal(LocalVector(0., 0., 1.)).phi() << endl;
  }

  //   // Check ordering.
  if (debug) {
    if (pos_zplus.z() < pos_zminus.z()) {
      LogDebug("magneticfield::volumeHandle") << "*** WARNING: pos_zplus < pos_zminus " << endl;
    }
    if (Geom::Phi<float>(pos_phiplus.phi() - pos_phiminus.phi()) < 0.) {
      LogDebug("magneticfield::volumeHandle") << "*** WARNING: pos_phiplus < pos_phiminus " << endl;
    }
  }
}

bool volumeHandle::sameSurface(const Surface &s1, Sides which_side, float tolerance) {
  //Check for null comparison
  if (&s1 == (surfaces[which_side]).get()) {
    if (debug)
      LogDebug("magneticfield::volumeHandle") << "      sameSurface: OK (same ptr)" << endl;
    return true;
  }

  const float maxtilt = 0.999;

  const Surface &s2 = *(surfaces[which_side]);
  // Try with a plane.
  const Plane *p1 = dynamic_cast<const Plane *>(&s1);
  if (p1 != nullptr) {
    const Plane *p2 = dynamic_cast<const Plane *>(&s2);
    if (p2 == nullptr) {
      if (debug)
        LogDebug("magneticfield::volumeHandle") << "      sameSurface: different types" << endl;
      return false;
    }

    if ((fabs(p1->normalVector().dot(p2->normalVector())) > maxtilt) &&
        (fabs((p1->toLocal(p2->position())).z()) < tolerance)) {
      if (debug)
        LogDebug("magneticfield::volumeHandle")
            << "      sameSurface: OK " << fabs(p1->normalVector().dot(p2->normalVector())) << " "
            << fabs((p1->toLocal(p2->position())).z()) << endl;
      return true;
    } else {
      if (debug)
        LogDebug("magneticfield::volumeHandle")
            << "      sameSurface: not the same: " << p1->normalVector() << p1->position() << endl
            << "                                 " << p2->normalVector() << p2->position() << endl
            << fabs(p1->normalVector().dot(p2->normalVector())) << " " << (p1->toLocal(p2->position())).z() << endl;
      return false;
    }
  }

  // Try with a cylinder.
  const Cylinder *cy1 = dynamic_cast<const Cylinder *>(&s1);
  if (cy1 != nullptr) {
    const Cylinder *cy2 = dynamic_cast<const Cylinder *>(&s2);
    if (cy2 == nullptr) {
      if (debug)
        LogDebug("magneticfield::volumeHandle") << "      sameSurface: different types" << endl;
      return false;
    }
    // Assume axis is the same!
    if (fabs(cy1->radius() - cy2->radius()) < tolerance) {
      return true;
    } else {
      return false;
    }
  }

  // Try with a cone.
  const Cone *co1 = dynamic_cast<const Cone *>(&s1);
  if (co1 != nullptr) {
    const Cone *co2 = dynamic_cast<const Cone *>(&s2);
    if (co2 == nullptr) {
      if (debug)
        LogDebug("magneticfield::volumeHandle") << "      sameSurface: different types" << endl;
      return false;
    }
    // FIXME
    if (std::abs(co1->openingAngle() - co2->openingAngle()) < maxtilt &&
        (co1->vertex() - co2->vertex()).mag() < tolerance) {
      return true;
    } else {
      return false;
    }
  }

  if (debug)
    LogDebug("magneticfield::volumeHandle") << "      sameSurface: unknown surfaces..." << endl;
  return false;
}

bool volumeHandle::setSurface(const Surface &s1, Sides which_side) {
  //Check for null assignment
  if (&s1 == (surfaces[which_side]).get()) {
    isAssigned[which_side] = true;
    return true;
  }

  if (!sameSurface(s1, which_side)) {
    LogDebug("magneticfield::volumeHandle")
        << "***ERROR: setSurface: trying to assign a surface that does not match destination surface. Skipping."
        << endl;
    const Surface &s2 = *(surfaces[which_side]);
    //FIXME: Just planes for the time being!!!
    const Plane *p1 = dynamic_cast<const Plane *>(&s1);
    const Plane *p2 = dynamic_cast<const Plane *>(&s2);
    if (p1 != nullptr && p2 != nullptr)
      LogDebug("magneticfield::volumeHandle") << p1->normalVector() << p1->position() << endl
                                              << p2->normalVector() << p2->position() << endl;
    return false;
  }

  if (isAssigned[which_side]) {
    if (&s1 != (surfaces[which_side]).get()) {
      LogDebug("magneticfield::volumeHandle")
          << "*** WARNING volumeHandle::setSurface: trying to reassign a surface to a different surface instance"
          << endl;
      return false;
    }
  } else {
    surfaces[which_side] = &s1;
    isAssigned[which_side] = true;
    if (debug)
      LogDebug("magneticfield::volumeHandle")
          << "     Volume " << name << " # " << copyno << " Assigned: " << (int)which_side << endl;
    return true;
  }

  return false;  // let the compiler be happy
}

const Surface &volumeHandle::surface(Sides which_side) const { return *(surfaces[which_side]); }

const Surface &volumeHandle::surface(int which_side) const {
  assert(which_side >= 0 && which_side < 6);
  return *(surfaces[which_side]);
}

std::vector<VolumeSide> volumeHandle::sides() const {
  std::vector<VolumeSide> result;
  for (int i = 0; i < 6; ++i) {
    // If this is just a master volume out of wich a 2pi volume
    // should be built (e.g. central cylinder), skip the phi boundaries.
    if (expand && (i == phiplus || i == phiminus))
      continue;

    // FIXME: Skip null inner degenerate cylindrical surface
    if (shape == DDSolidShape::ddtubs && i == SurfaceOrientation::inner && theRMin < 0.001)
      continue;

    ReferenceCountingPointer<Surface> s = const_cast<Surface *>(surfaces[i].get());
    result.push_back(VolumeSide(s, GlobalFace(i), surfaces[i]->side(center_, 0.3)));
  }
  return result;
}

// printUniqueNames is for debugging only
void volumeHandle::printUniqueNames(handles::const_iterator begin, handles::const_iterator end, bool uniq) {
  std::vector<std::string> names;
  for (handles::const_iterator i = begin; i != end; ++i) {
    if (uniq)
      names.push_back((*i)->name);
    else
      names.push_back((*i)->name + ":" + std::to_string((*i)->copyno));
  }
  sort(names.begin(), names.end());
  if (uniq) {
    std::vector<std::string>::iterator i = unique(names.begin(), names.end());
    int nvols = int(i - names.begin());
    cout << nvols << " ";
    copy(names.begin(), i, ostream_iterator<std::string>(cout, " "));
  } else {
    cout << names.size() << " ";
    copy(names.begin(), names.end(), ostream_iterator<std::string>(cout, " "));
  }
  cout << endl;
}

#include "MagneticField/GeomBuilder/src/buildBox.icc"
#include "MagneticField/GeomBuilder/src/buildTrap.icc"
#include "MagneticField/GeomBuilder/src/buildTubs.icc"
#include "MagneticField/GeomBuilder/src/buildCons.icc"
#include "MagneticField/GeomBuilder/src/buildPseudoTrap.icc"
#include "MagneticField/GeomBuilder/src/buildTruncTubs.icc"
