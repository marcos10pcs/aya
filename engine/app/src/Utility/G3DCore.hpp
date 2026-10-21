

// this is just to stop multiple includes,
// but why the hell was it originally
// #define _70F7A2EE1B6E4dd0AF07E4BFA609A3D1 ????
#ifndef G3D_CORE_INCLUDED
#define G3D_CORE_INCLUDED

#include "Graphics3D/Vector2.hpp"
#include "Graphics3D/Vector3.hpp"
#include "Graphics3D/Vector4.hpp"
#include "Graphics3D/Matrix3.hpp"
#include "Graphics3D/Matrix4.hpp"
#include "Graphics3D/Vector3int16.hpp"
#include "Graphics3D/Vector2int16.hpp"
#include "Graphics3D/Color4uint8.hpp"
#include "Graphics3D/Color3uint8.hpp"
#include "Graphics3D/CoordinateFrame.hpp"
#include "Graphics3D/Plane.hpp"
#include "Graphics3D/Line.hpp"
#include "Graphics3D/LineSegment.hpp"

#include "Graphics3D/AABox.hpp"
#include "Graphics3D/Box.hpp"
#include "Graphics3D/aya/RbxCamera.hpp"
#include "Graphics3D/Color3.hpp"
#include "Graphics3D/Color4.hpp"
#include "Graphics3D/g3dmath.hpp"
#include "Graphics3D/Rect2D.hpp"
#include "Graphics3D/Sphere.hpp"

#include "Graphics3D/vectorMath.hpp"
#include "Graphics3D/debug.hpp"

// TODO: this can cause namespace collisions:
// using G3D::Array;

namespace Aya
{
typedef G3D::Vector2 Vector2;
typedef G3D::Vector3 Vector3;
typedef G3D::Vector4 Vector4;
typedef G3D::Vector2int16 Vector2int16;
typedef G3D::Vector3int16 Vector3int16;
typedef G3D::Color4uint8 Color4uint8;
typedef G3D::Color3uint8 Color3uint8;
typedef G3D::Matrix3 Matrix3;
typedef G3D::Matrix4 Matrix4;
typedef G3D::CoordinateFrame CoordinateFrame;
typedef Aya::RbxRay Ray;
typedef G3D::Plane Plane;
typedef G3D::Line Line;
typedef G3D::LineSegment LineSegment;
typedef G3D::Color3 Color3;
typedef G3D::Color4 Color4;
typedef G3D::Rect2D Rect2D;
typedef G3D::Box Box;
typedef G3D::AABox AABox;
typedef G3D::Sphere Sphere;

enum IntersectResult
{
    irNone = 0,
    irPartial = 1,
    irFull = 2
};
} // namespace Aya

namespace G3D
{
std::size_t hash_value(const G3D::Vector3& v);
std::size_t hash_value(const G3D::Vector3int16& v);
} // namespace G3D

#endif