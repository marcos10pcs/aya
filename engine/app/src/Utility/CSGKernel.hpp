

#pragma once

#include "DataModel/CSGMesh.hpp"

#include <boost/unordered_map.hpp>
#include <set>

#include "Utility/Extents.hpp"
#include "Utility/Vector3int32.hpp"

namespace Aya
{

class CSGMeshCGAL : public CSGMesh
{
public:
    CSGMeshCGAL(){};
    CSGMeshCGAL(const CSGMeshCGAL& mesh){};
    ~CSGMeshCGAL(){};

    virtual CSGMesh* clone() const
    {
        return new CSGMeshCGAL(*this);
    };

    virtual CSGMeshCGAL& operator=(const CSGMeshCGAL& mesh)
    {
        return *this;
    };

    virtual bool isValid() const
    {
        return true;
    };

    virtual void translate(const G3D::Vector3& translation) {};
    virtual void applyCoordinateFrame(G3D::CoordinateFrame cFrame) {};
    virtual void applyTranslation(const G3D::Vector3& trans) {};
    virtual void applyScale(const G3D::Vector3& size) {};
    virtual void applyColor(const G3D::Vector3& color) {};
    virtual void triangulate() {};
    virtual bool newTriangulate()
    {
        return false;
    };
    virtual void weldMesh(bool positionOnly = false) {};

    virtual std::string getBRepBinaryString() const
    {
        return "";
    };
    virtual void setBRepFromBinaryString(const std::string& str) {};

    virtual void buildBRep() {};

    virtual bool unionMesh(const CSGMesh* a, const CSGMesh* b)
    {
        return false;
    };
    virtual bool intersectMesh(const CSGMesh* a, const CSGMesh* b)
    {
        return false;
    };
    virtual bool subractMesh(const CSGMesh* a, const CSGMesh* b)
    {
        return false;
    };

    virtual size_t clusterVertices(float resolution)
    {
        return 0;
    };
    virtual bool makeHalfEdges(std::vector<int>& vertexEdges)
    {
        return true;
    };

    virtual G3D::Vector3 extentsCenter()
    {
        return G3D::Vector3::zero();
    };
    virtual G3D::Vector3 extentsSize()
    {
        return G3D::Vector3::zero();
    };
};

class CSGMeshFactoryCGAL : public CSGMeshFactory
{
public:
    virtual CSGMesh* createMesh() const
    {
        return new CSGMeshCGAL();
    };
};

} // namespace Aya
