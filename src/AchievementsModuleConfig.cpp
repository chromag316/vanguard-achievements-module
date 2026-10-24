#include "AchievementsModuleConfig.h"

#include "Log/Log.h"
#include "SystemConfig.h"

namespace cmangos_module
{
    AchievementsModuleConfig::AchievementsModuleConfig()
    : enabled(false)
    , sendMessage(false)
    , sendAddon(false)
    , sendVisual(false)
    , randomBots(false)
    , randomBotsRealmFirst(false)
    , accountAchievements(false)
    , effectId(0)
    {

    }

    bool AchievementsModuleConfig::Load()
    {
        if (!config.SetSource(SYSCONFDIR "achievements.conf", ""))
        {
            sLog.outError("Failed to open configuration file achievements.conf");
            return false;
        }

        enabled = config.GetBoolDefault("Achievements.Enable", false);
        sendMessage = config.GetBoolDefault("Achievements.SendMessage", false);
        sendAddon = config.GetBoolDefault("Achievements.SendAddon", false);
        sendVisual = config.GetBoolDefault("Achievements.SendVisual", false);
        randomBots = config.GetBoolDefault("Achievements.RandomBots", false);
        randomBotsRealmFirst = config.GetBoolDefault("Achievements.RandomBotsRealmFirst", false);
        accountAchievements = config.GetBoolDefault("Achievements.AccountAchievenemts", false);
        effectId = config.GetIntDefault("Achievements.EffectId", 0);
        return true;
    }
}
