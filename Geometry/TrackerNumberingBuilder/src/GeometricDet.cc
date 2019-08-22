#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/interface/TrackerShapeToBounds.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/DDCMS/interface/DDFilteredView.h"
#include "CondFormats/GeometryObjects/interface/PGeometricDet.h"

#include <cfloat>
#include <vector>
#include <string>

namespace {

  const std::string strue("true");

  template <typename DDView>
  double getDouble(const char* s, DDView const& ev) {
    DDValue val(s);
    std::vector<const DDsvalues_type*> result;
    ev.specificsV(result);
    std::vector<const DDsvalues_type*>::iterator it = result.begin();
    bool foundIt = false;
    for (; it != result.end(); ++it) {
      foundIt = DDfetch(*it, val);
      if (foundIt)
        break;
    }
    if (foundIt) {
      const std::vector<std::string>& temp = val.strings();
      if (temp.size() != 1) {
        throw cms::Exception("Configuration") << "I need 1 " << s << " tags";
      }
      return double(::atof(temp[0].c_str()));
    }
    return 0;
  }

  template <typename DDView>
  std::string getString(const char* s, DDView const& ev) {
    DDValue val(s);
    std::vector<const DDsvalues_type*> result;
    ev.specificsV(result);
    std::vector<const DDsvalues_type*>::iterator it = result.begin();
    bool foundIt = false;
    for (; it != result.end(); ++it) {
      foundIt = DDfetch(*it, val);
      if (foundIt)
        break;
    }
    if (foundIt) {
      const std::vector<std::string>& temp = val.strings();
      if (temp.size() != 1) {
        throw cms::Exception("Configuration") << "I need 1 " << s << " tags";
      }
      return temp[0];
    }
    return "NotFound";
  }
}  // namespace

/**
 * What to do in the destructor?
 * destroy all the daughters!
 */
GeometricDet::~GeometricDet() { deleteComponents(); }

GeometricDet::GeometricDet(DDFilteredView* fv, GeometricEnumType type)
    :  //
      // Set by hand the ddd_
      //
      trans_(fv->translation()),
      phi_(trans_.Phi()),
      rho_(trans_.Rho()),
      rot_(fv->rotation()),
      shape_(fv->shape()),
      ddname_(fv->name()),
      type_(type),
      params_(fv->parameters()),
      radLength_(getDouble("TrackerRadLength", *fv)),
      xi_(getDouble("TrackerXi", *fv)),
      pixROCRows_(getDouble("PixelROCRows", *fv)),
      pixROCCols_(getDouble("PixelROCCols", *fv)),
      pixROCx_(getDouble("PixelROC_X", *fv)),
      pixROCy_(getDouble("PixelROC_Y", *fv)),
      stereo_(getString("TrackerStereoDetectors", *fv) == strue),
      siliconAPVNum_(getDouble("SiliconAPVNumber", *fv)) {
  //  workaround instead of this at initialization ddd_(fv->navPos().begin(),fv->navPos().end()),
  const DDFilteredView::nav_type& nt = fv->navPos();
  ddd_ = nav_type(nt.begin(), nt.end());
}

GeometricDet::GeometricDet(cms::DDFilteredView* fv, GeometricEnumType type) :
      trans_(fv->translation()),
      phi_(trans_.Phi()),
      rho_(trans_.Rho()),
      shape_(DDSolidShape(static_cast<int>(fv->shape()))),
      ddname_(fv->name()),
      type_(type),
      params_(fv->parameters()),
      radLength_(fv->getDouble("TrackerRadLength")),
      xi_(fv->getDouble("TrackerXi")),
      pixROCRows_(fv->getDouble("PixelROCRows")),
      pixROCCols_(fv->getDouble("PixelROCCols")),
      pixROCx_(fv->getDouble("PixelROC_X")),
      pixROCy_(fv->getDouble("PixelROC_Y")),
      stereo_(fv->getString("TrackerStereoDetectors") == strue),
      siliconAPVNum_(fv->getDouble("SiliconAPVNumber")) {
  fv->rot(rot_);

  const std::vector<int> nt = fv->navPos(); // *** CHECK IT WORKS
  ddd_ = nav_type(nt.begin(), nt.end());
}

// PGeometricDet is persistent version... make it... then come back here and make the
// constructor.
GeometricDet::GeometricDet(const PGeometricDet::Item& onePGD, GeometricEnumType type)
    : trans_(onePGD._x, onePGD._y, onePGD._z),
      phi_(onePGD._phi),  //trans_.Phi()),
      rho_(onePGD._rho),  //trans_.Rho()),
      rot_(onePGD._a11,
           onePGD._a12,
           onePGD._a13,
           onePGD._a21,
           onePGD._a22,
           onePGD._a23,
           onePGD._a31,
           onePGD._a32,
           onePGD._a33),
      shape_(static_cast<DDSolidShape>(onePGD._shape)),
      ddd_(),
      ddname_(onePGD._name),
      type_(type),
      params_(),
      geographicalID_(onePGD._geographicalID),
      radLength_(onePGD._radLength),
      xi_(onePGD._xi),
      pixROCRows_(onePGD._pixROCRows),
      pixROCCols_(onePGD._pixROCCols),
      pixROCx_(onePGD._pixROCx),
      pixROCy_(onePGD._pixROCy),
      stereo_(onePGD._stereo),
      siliconAPVNum_(onePGD._siliconAPVNum) {
  if (shape_ == DDSolidShape::ddbox || shape_ == DDSolidShape::ddtrap) {
  //The params vector is needed only in the case of box or trap shape
    params_.reserve(11);
    params_.emplace_back(onePGD._params0);
    params_.emplace_back(onePGD._params1);
    params_.emplace_back(onePGD._params2);
    params_.emplace_back(onePGD._params3);
    params_.emplace_back(onePGD._params4);
    params_.emplace_back(onePGD._params5);
    params_.emplace_back(onePGD._params6);
    params_.emplace_back(onePGD._params7);
    params_.emplace_back(onePGD._params8);
    params_.emplace_back(onePGD._params9);
    params_.emplace_back(onePGD._params10);
  }

  ddd_.reserve(onePGD._numnt);
  ddd_.emplace_back(onePGD._nt0);
  ddd_.emplace_back(onePGD._nt1);
  ddd_.emplace_back(onePGD._nt2);
  ddd_.emplace_back(onePGD._nt3);
  if (onePGD._numnt > 4) {
    ddd_.emplace_back(onePGD._nt4);
    if (onePGD._numnt > 5) {
      ddd_.emplace_back(onePGD._nt5);
      if (onePGD._numnt > 6) {
        ddd_.emplace_back(onePGD._nt6);
        if (onePGD._numnt > 7) {
          ddd_.emplace_back(onePGD._nt7);
          if (onePGD._numnt > 8) {
            ddd_.emplace_back(onePGD._nt8);
            if (onePGD._numnt > 9) {
              ddd_.emplace_back(onePGD._nt9);
              if (onePGD._numnt > 10) {
                ddd_.emplace_back(onePGD._nt10);
              }
            }
          }
        }
      }
    }
  }
}

GeometricDet::ConstGeometricDetContainer GeometricDet::deepComponents() const {
  //
  // iterate on all the components ;)
  //
  ConstGeometricDetContainer _temp;
  deepComponents(_temp);
  return _temp;
}

void GeometricDet::deepComponents(ConstGeometricDetContainer& cont) const {
  if (isLeaf())
    cont.emplace_back(this);
  else
    std::for_each(container_.begin(), container_.end(), [&](const GeometricDet* iDet) { iDet->deepComponents(cont); });
}

void GeometricDet::addComponents(GeometricDetContainer const& cont) {
  container_.reserve(container_.size() + cont.size());
  std::copy(cont.begin(), cont.end(), back_inserter(container_));
}

void GeometricDet::addComponents(ConstGeometricDetContainer const& cont) {
  container_.reserve(container_.size() + cont.size());
  std::copy(cont.begin(), cont.end(), back_inserter(container_));
}

void GeometricDet::addComponent(GeometricDet* det) { container_.emplace_back(det); }

namespace {
  struct Deleter {
    void operator()(GeometricDet const* det) const { delete const_cast<GeometricDet*>(det); }
  };
}  // namespace

void GeometricDet::deleteComponents() {
  std::for_each(container_.begin(), container_.end(), Deleter());
  container_.clear();
}

GeometricDet::Position GeometricDet::positionBounds() const {
  // Watch units. Is DD4hep now returning CMS units?
  Position _pos(float(trans_.x()), float(trans_.y()), float(trans_.z()));
  return _pos;
}

GeometricDet::Rotation GeometricDet::rotationBounds() const {
  Translation x, y, z;
  rot_.GetComponents(x, y, z);
  Rotation theRotation(float(x.X()),
                     float(x.Y()),
                     float(x.Z()),
                     float(y.X()),
                     float(y.Y()),
                     float(y.Z()),
                     float(z.X()),
                     float(z.Y()),
                     float(z.Z()));
  return theRotation;
}

std::unique_ptr<Bounds> GeometricDet::bounds() const {
  const std::vector<double>& par = params_;
  TrackerShapeToBounds shapeToBounds;
  return std::unique_ptr<Bounds>(shapeToBounds.buildBounds(shape_, par));
}
