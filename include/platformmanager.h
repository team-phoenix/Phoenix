#ifndef PLATFORMMANAGER_H
#define PLATFORMMANAGER_H

#include <QString>
#include <QMap>

class PlatformManager {
public:
    PlatformManager() :
        cleaned_system_name(QMap<QString,QString>{
            { "Sega 16bit + 32X (Various)", sega_genesis },
            { "Super Nintendo Entertainment System", super_nintendo },
            { "DOOM game engine", doom },
            { "PSP", psp },
            { "Commodore Amiga", amiga },
            { "Nintendo Entertainment System", nintendo },
            { "PSP RemotePlay", psp_remote_play },
            { "Game Boy/Game Boy Color", gameboy },
            { "Quake 1 game engine", quake },
            { "Saturn", sega_saturn },
            { "2048 game clone", game },
            { "MSX/SVI/ColecoVision/SG-1000", msx },
            { "Arcade (various)", arcade },
            { "Atari ST/STE/TT/Falcon", atari_st }
        }),
        preferred_cores(QMap<QString,QString>{
            { "Nintendo", fceumm },
            { "Super Nintendo", snes9x },
            { "Game Boy", gambatte },
            { "Game Boy Advance", vbam },
            { "Sony PlayStation", mednafen_psx }
        })
    {}
    ~PlatformManager() {}


    // List of available consoles
    QString _2048 = "2048_libretro";
    QString _3dengine = "3dengine_libretro";
    QString _4do = "4do_libretro";
    QString bluemsx = "bluemsx_libretro";
    QString bnes = "bnes_libretro";
    QString bsnes_accuracy = "bsnes_accuracy_libretro";
    QString bsnes_balanced = "bsnes_balanced_libretro";
    QString bsnes_performance = "bsnes_performance_libretro";
    QString bsnes_c98 = "bsnes_cplusplus98_libretro";
    QString bsnes_accuracy_mercury = "bsnes_mercury_accuracy_libretro";
    QString bsnes_balanced_mercury = "bsnes_mercury_balanced_libretro";
    QString bsnes_performance_mercury = "bsnes_mercury_performance_libretro";
    QString desmume = "desmume_libretro";
    QString dinothawr = "dinothawr_libretro";
    QString doxbox = "dosbox_libretro";
    QString fb_alpha = "fb_alpha_libretro";
    QString fba_cps1 = "fba_cores_cps1_libretro";
    QString fba_cps2 = "fba_cores_cps2_libretro";
    QString fba_neo = "fba_cores_neo_libretro";
    QString fceumm = "fceumm_libretro";
    QString ffmpeg = "ffmpeg_libretro";
    QString fmsx = "fmsx_libretro";
    QString gambatte = "gambatte_libretro";
    QString genesis_plus = "genesis_plus_gx_libretro";
    QString handy = "handy_libretro";
    QString hatari = "hatari_libretro";
    QString imame4all = "imame4all_libretro";
    QString mame078 = "mame078_libretro";
    QString mame2010 = "mame2010_libretro";
    QString mame = "mame_libretro";
    QString mednafen_gba = "mednafen_gba_libretro";
    QString mednafen_lynx = "mednafen_lynx_libretro";
    QString mednafen_ngp = "mednafen_ngp_libretro";
    QString mednafen_pce = "mednafen_pce_fast_libretro";
    QString mednafen_pcfx_libretro = "mednafen_pcfx_libretro";
    QString mednafen_psx = "mednafen_psx_libretro";
    QString mednafen_snes = "mednafen_snes_libretro";
    QString mednafen_supergrafx = "mednafen_supergrafx_libretro";
    QString mednafen_vb = "mednafen_vb_libretro";
    QString mednafen_wswan = "mednafen_wswan_libretro";
    QString mess = "mess_libretro";
    QString meteor = "metero_libertro";
    QString mupen64plus = "mupen64plus_libretro";
    QString nestopia = "nestopia_libretro";
    QString nxengine = "nxengine_libretro";
    QString o2em = "o2em_libretro";
    QString pcsx_rearmed = "pcsx_rearmed_libretro";
    QString pcsx_rearmed_neon = "pcsx_rearmed_libretro_neon";
    QString picodrive = "picodrive_libretro";
    QString pocketsnes = "pocketsnes_libretro";
    QString ppsspp = "ppsspp_libretro";
    QString prboom = "prboom_libretro";
    QString prosystem = "prosystem_libretro";
    QString puae = "puae_libretro";
    QString quicknes = "quicknes_libretro";
    QString remotejoy = "remotejoy_libretro";
    QString scummvm = "scummvm_libretro";
    QString snes9x = "snes9x_libretro";
    QString snes9x_next = "snes9x_next_libretro";
    QString stella = "stella_libretro";
    QString tgbdual = "tgbdual_libretro";
    QString tyrquake = "tryquake_libretro";
    QString vba_next = "vba_next_libretro";
    QString vbam = "vbam_libretro";
    QString vecx = "vecx_libretro";
    QString virtualjaguar = "virtualjaguar_libretro";
    QString yabause = "yabause_libretro";

    // Proper system names
    QString super_nintendo = "Super Nintendo";
    QString nintendo = "Nintendo";
    QString sega_genesis = "Sega Genesis";
    QString doom = "DOOM";
    QString psp = "PlayStation Portable";
    QString amiga = "Amiga";
    QString nintendo_ds = "Nintendo DS";
    QString nintendo_64 = "Nintendo 64";
    QString playstation = "Sony PlayStation";
    QString arcade = "Arcade";
    QString msx = "MSX";
    QString atari_st = "Atari ST";
    QString atari_7800 = "Atari 7800";
    QString psp_remote_play = "PlayStation Portable (Remote)";
    QString gameboy = "Game Boy";
    QString gameboy_advance = "Game Boy Advance";
    QString quake = "Quake";
    QString game = "Game";
    QString sega_saturn = "Sega Saturn";
    QString _3do = "3DO";
    QString atari_lynx = "Atari Lynx";
    QString dos = "DOS";
    QString video = "Film";


    QMap<QString, QString> cleaned_system_name;
    QMap<QString, QString> preferred_cores;
};

#endif // PLATFORMMANAGER_H
