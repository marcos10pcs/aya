
#include "Tree/Property.hpp"
#include "DataModel/MeshPartInstance.hpp"

namespace Aya
{

const char* const sMeshPartInstance = "MeshPart";

const Reflection::PropDescriptor<MeshPartInstance, ContentId> MeshPartInstance::prop_meshPartId(
    "MeshId", category_Appearance, &MeshPartInstance::getMeshId, &MeshPartInstance::setMeshId);
const Reflection::PropDescriptor<MeshPartInstance, ContentId> MeshPartInstance::prop_meshPartTextureId(
    "TextureID", category_Appearance, &MeshPartInstance::getTextureId, &MeshPartInstance::setTextureId);
const Reflection::PropDescriptor<MeshPartInstance, bool> MeshPartInstance::prop_doubleSided(
    "DoubleSided", category_Appearance, &MeshPartInstance::getDoubleSided, &MeshPartInstance::setDoubleSided);

MeshPartInstance::MeshPartInstance()
{
    setName("MeshPart");
}

MeshPartInstance::~MeshPartInstance() {}

PartType MeshPartInstance::getPartType() const
{
    return MESH_PART;
}

bool MeshPartInstance::isActualMeshIdSet()
{
    return !(!this->getMeshId().isAssetId() && !this->getMeshId().isAsset() && !this->getMeshId().isHttp() && !this->getMeshId().isRbxHttp());
}

bool MeshPartInstance::isActualTextureIdSet()
{
    return !(
        !this->getTextureId().isAssetId() && !this->getTextureId().isAsset() && !this->getTextureId().isHttp() && !this->getTextureId().isRbxHttp());
}

void MeshPartInstance::setMeshId(const ContentId& value)
{
    if (meshId != value)
    {
        meshId = value;
        raisePropertyChanged(prop_meshPartId);
        updatePrimitiveState();
    }
}

void MeshPartInstance::setTextureId(const ContentId& value)
{
    if (textureId != value)
    {
        textureId = value;
        raisePropertyChanged(prop_meshPartTextureId);
    }
}

void MeshPartInstance::setDoubleSided(const bool& value)
{
    if (doubleSided != value)
    {
        doubleSided = value;
        raisePropertyChanged(prop_doubleSided);
    }
}

} // namespace Aya
