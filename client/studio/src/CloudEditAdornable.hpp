#pragma once

#include "GfxBase/IAdornable.hpp"
#include "GfxBase/Part.hpp"
#include "Tree/Instance.hpp"

#include "DataModel/DataModel.hpp"


#include <boost/shared_ptr.hpp>

namespace Aya
{
class Camera;
}

class PlayersDataManager;

class CloudEditAdornable : public Aya::IAdornable
{
    shared_ptr<Aya::DataModel> m_pDataModel;
    shared_ptr<PlayersDataManager> m_pPlayersDataManager;

    static void renderSelection(Aya::Color3 playerColor, shared_ptr<Aya::Instance> instance, Aya::Part part, float lineThickness, Aya::Adorn* adorn);
    static void renderCamera(Aya::Color3 playerColor, shared_ptr<Aya::Network::Player> player, const Aya::Camera* localCamera, Aya::Adorn* adorn);

protected:
    bool shouldRender3dAdorn() const override
    {
        return true;
    }

public:
    void attach(shared_ptr<Aya::DataModel> dataModel, shared_ptr<PlayersDataManager> playersDataManager); // requires DataModel write lock is held
    void render3dAdorn(Aya::Adorn* adorn) override;
};
