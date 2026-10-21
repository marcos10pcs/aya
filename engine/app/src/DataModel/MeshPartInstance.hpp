
#include "boost/smart_ptr/shared_ptr.hpp"

#include "DataModel/BasicPartInstance.hpp"
#include "DataModel/DataModelMesh.hpp"
#include "GfxBase/FileMeshData.hpp"
#include "DataModel/FileMesh.hpp"

namespace Aya
{

extern const char* const sMeshPartInstance;

class MeshPartInstance : public DescribedCreatable<MeshPartInstance, BasicPartInstance, sMeshPartInstance>
{
protected:
    boost::shared_ptr<FileMeshData> meshData;
    ContentId meshId;
    ContentId textureId;
    bool doubleSided;

public:
    MeshPartInstance();
    ~MeshPartInstance();

    static const Reflection::PropDescriptor<MeshPartInstance, ContentId> prop_meshPartId;
    static const Reflection::PropDescriptor<MeshPartInstance, ContentId> prop_meshPartTextureId;
    static const Reflection::PropDescriptor<MeshPartInstance, bool> prop_doubleSided;

    const ContentId& getTextureId() const
    {
        return textureId;
    };
    const ContentId& getMeshId() const
    {
        return meshId;
    };
    const bool& getDoubleSided() const
    {
        return doubleSided;
    }
    boost::shared_ptr<FileMeshData> getMeshData() const
    {
        return meshData;
    }
    virtual void setMeshData(boost::shared_ptr<FileMeshData> data)
    {
        meshData = data;
    };
    virtual void setTextureId(const ContentId& textureId);
    virtual void setMeshId(const ContentId& meshId);
    virtual void setDoubleSided(const bool& value);

    bool isActualMeshIdSet();
    bool isActualTextureIdSet();

    /* override */ virtual PartType getPartType() const;
};
} // namespace Aya
