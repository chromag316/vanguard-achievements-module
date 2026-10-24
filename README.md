# Achievements Module for CMaNGOS Classic

[![Build & Validate](https://github.com/nemanuel/forged-cmangos-achievements/actions/workflows/build.yml/badge.svg)](https://github.com/nemanuel/forged-cmangos-achievements/actions/workflows/build.yml)

Backports WotLK achievements to CMaNGOS Classic.

This module was ported from https://github.com/tsaah/core/tree/hb-achievements and has been
updated to work with the latest CMaNGOS Classic core and classic-db.

The core calls this module through one achievement-specific interface,
`src/game/Achievements/AchievementHooks.h`, carried by the `vanguard` branch of
[chromag316/vanguard-core](https://github.com/chromag316/vanguard-core). The old generic
`ModuleMgr`/`ModuleConfig` framework and `patches/classic.patch` are no longer used.

<!-- ![Achievements UI](https://github.com/davidonete/cmangos-achievements/assets/11618807/caa813e9-0053-4405-8d00-cf04fe5c205f) -->

## Supported Cores

| Core | Status |
|------|--------|
| CMaNGOS Classic | Supported |
| CMaNGOS TBC | Untested |
| CMaNGOS WoTLK | Unsupported |

## Core integration

- `AchievementHooks` (in the core) declares every game event the module uses, with a
  do-nothing default. `AchievementsModule` overrides them, and the core registers it
  through `AddAchievementHooks()` at world start-up.
- All call sites are guarded by `#ifdef BUILD_ACHIEVEMENTS`, so a core built without the
  module carries no achievement code.
- `.achievements <command>` chat commands (used by the Achiever addon) arrive through
  `AchievementHooks::OnChatCommand`.

## Installation

1. **Build the core with achievements.** Use the `vanguard` branch of
   `chromag316/vanguard-core`; CMake fetches this module at the commit pinned in
   `cmake/vanguard-modules.cmake`:
   ```bash
   cmake -S vanguard-core -B vanguard-core/build -G "Visual Studio 17 2022" -A x64 -DBUILD_PLAYERBOTS=ON -DBUILD_ACHIEVEMENTS=ON -DBoost_DIR=<boost>/lib64-msvc-14.3/cmake/Boost-1.85.0
   ```
   To build a local checkout of this repository instead of the pinned commit, add
   `-DFETCHCONTENT_SOURCE_DIR_ACHIEVEMENTS=<path to this checkout>`. Vanguard's
   `devkit/scripts/build_vanguard.ps1` sets all of this up.

2. **Install the configuration file** — copy
   `src/modules/achievements/src/achievements.conf.dist.in` to the directory that
   contains your `mangosd` executable and rename it to `achievements.conf`. Edit the
   options as needed (see [Configuration](#configuration)).

3. **Install the database schemas** (first time only) - run PowerShell commands:
   ```bash
   # World database
   Get-ChildItem -Path ".\mangos-classic\src\modules\Achievements\sql\install\world\*.sql" | ForEach-Object {
      Get-Content $_.FullName | mysql -u root -p --database=classicmangos
   }

   # Characters database
   Get-ChildItem -Path ".\mangos-classic\src\modules\Achievements\sql\install\characters\*.sql" | ForEach-Object {
      Get-Content $_.FullName | mysql -u root -p --database=classiccharacters
   }
   ```

4. **Upgrading an existing installation** — run the migration scripts instead:
   ```bash
   mysql -u<user> -p<pass> <world_db>   < sql/migrate/world_migrate.sql
   mysql -u<user> -p<pass> <chars_db>   < sql/migrate/characters_migrate.sql
   ```
   The migration scripts are idempotent and safe to re-run.

5. **Install the client addon** — download and install
   https://github.com/celguar/Achiever

## Configuration

| Option | Default | Description |
|--------|---------|-------------|
| `Achievements.Enable` | `0` | Enable the achievements system |
| `Achievements.SendMessage` | `1` | Announce achievements in chat/guild |
| `Achievements.SendAddon` | `1` | Send Achiever addon data packets |
| `Achievements.SendVisual` | `1` | Play visual effect on achievement earn |
| `Achievements.RandomBots` | `0` | Enable achievements for random bots |
| `Achievements.RandomBotsRealmFirst` | `0` | Allow bots to earn Realm First achievements |
| `Achievements.AccountAchievenemts` | `0` | Sync achievements across all characters on an account (note: key name has a typo preserved for backward compatibility) |
| `Achievements.EffectId` | `146` | Spell visual effect ID played on achievement earn |

## Uninstallation

**Option A — Disable only:**
Set `Achievements.Enable = 0` in `achievements.conf` and restart the server.

**Option B — Full removal:**
1. Recompile without `-DBUILD_ACHIEVEMENTS=ON`
2. Remove database tables:
   ```bash
   mysql -u<user> -p<pass> <chars_db>   < sql/uninstall/characters/characters.sql
   mysql -u<user> -p<pass> <world_db>   < sql/uninstall/world/world.sql
   ```

## Architecture & Documentation

| Document | Description |
|----------|-------------|
| [docs/architecture-analysis.md](docs/architecture-analysis.md) | Current architecture, event flow, data structures, technical debt |
| [docs/hook-audit.md](docs/hook-audit.md) | Audit of all core hooks — required, replaceable, and obsolete |
| [docs/migration/01-build-audit.md](docs/migration/01-build-audit.md) | Build audit showing why the historical module framework no longer links against current upstream cores |
| [docs/migration/02-achievement-hook-audit.md](docs/migration/02-achievement-hook-audit.md) | Full hook inventory for the achievement consumers |
| [docs/migration/03-scriptmgr-migration-audit.md](docs/migration/03-scriptmgr-migration-audit.md) | Analysis of what ScriptMgr can and cannot replace |
| [docs/migration/04-minimal-hook-set.md](docs/migration/04-minimal-hook-set.md) | Minimal residual hook set needed for current CMaNGOS |
| [docs/event-architecture.md](docs/event-architecture.md) | Target modern event architecture and migration strategy |
| [docs/classic-patch-modernization-report.md](docs/classic-patch-modernization-report.md) | Compile errors, failing patch hunks, ModuleMgr dependencies, and patch-footprint replacement recommendations |

The documents above were written while planning the move away from the module
framework; the implemented hook set is `AchievementHooks.h` in the core.

## CI

The GitHub Actions workflow (`.github/workflows/build.yml`) automatically:

1. Checks out the `vanguard` branch of `chromag316/vanguard-core`
2. Builds the core with this checkout of the module (`BUILD_ACHIEVEMENTS=ON`)
3. Validates all SQL scripts against a live MySQL 8.0 instance
4. Verifies idempotency of the migration scripts

## Credits

* Original port: https://github.com/tsaah/core/tree/hb-achievements
* Module packaging: https://github.com/davidonete/cmangos-achievements
* Client addon: https://github.com/celguar/Achiever
