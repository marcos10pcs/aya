


#include "MachineConfig.hpp"

// Roblox Headers
#include "MachineConfiguration.hpp"

// Roblox Studio Headers
#include "RobloxSettings.hpp"

MachineConfig* MachineConfig::machineConfig = NULL;
////////////////////////////////////////////////////////////////////////////////////

void MachineConfig::PostMachineConfiguration(bool authSuccess)
{
    if (authSuccess)
    {
        disconnect(&AuthenticationHelper::Instance(), SIGNAL(authenticationDone(bool)), machineConfig, SLOT(PostMachineConfiguration(bool)));

        RobloxSettings settings;
        int lastGfxMode = settings.contains("lastGFXMode") ? settings.value("lastGFXMode").toInt() : 0;

        Aya::postMachineConfiguration(RobloxSettings::getBaseURL().toLocal8Bit().constData(), lastGfxMode);
    }
}
/////////////////////////////////////////////////////////////////////////////////
