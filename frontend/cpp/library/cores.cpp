
#include "cores.h"

// this file is machine generated, DO NOT EDIT
// last generated at 2015-09-25 12:27:28 UTC

using namespace Library;

const QString Cores::toString( const LibretroCore &core ) {
    switch ( core ) {
    
    case CORE_2048_LIBRETRO:
        return QStringLiteral( "2048" );

    case CORE_3DENGINE_LIBRETRO:
        return QStringLiteral( "3Dengine" );

    case CORE_4DO_LIBRETRO:
        return QStringLiteral( "4Do" );

    case CORE_81_LIBRETRO:
        return QStringLiteral( "81" );

    case CORE_BEETLE_PSX_LIBRETRO:
        return QStringLiteral( "Beetle Psx" );

    case CORE_BLUEMSX_LIBRETRO:
        return QStringLiteral( "Bluemsx" );

    case CORE_BNES_LIBRETRO:
        return QStringLiteral( "Bnes" );

    case CORE_BSNES_ACCURACY_LIBRETRO:
        return QStringLiteral( "Bsnes Accuracy" );

    case CORE_BSNES_BALANCED_LIBRETRO:
        return QStringLiteral( "Bsnes Balanced" );

    case CORE_BSNES_CPLUSPLUS98_LIBRETRO:
        return QStringLiteral( "Bsnes Cplusplus98" );

    case CORE_BSNES_MERCURY_ACCURACY_LIBRETRO:
        return QStringLiteral( "Bsnes Mercury Accuracy" );

    case CORE_BSNES_MERCURY_BALANCED_LIBRETRO:
        return QStringLiteral( "Bsnes Mercury Balanced" );

    case CORE_BSNES_MERCURY_PERFORMANCE_LIBRETRO:
        return QStringLiteral( "Bsnes Mercury Performance" );

    case CORE_BSNES_PERFORMANCE_LIBRETRO:
        return QStringLiteral( "Bsnes Performance" );

    case CORE_CAP32_LIBRETRO:
        return QStringLiteral( "Cap32" );

    case CORE_CATSFC_LIBRETRO:
        return QStringLiteral( "Catsfc" );

    case CORE_DESMUME_LIBRETRO:
        return QStringLiteral( "Desmume" );

    case CORE_DINOTHAWR_LIBRETRO:
        return QStringLiteral( "Dinothawr" );

    case CORE_DOLPHIN_LIBRETRO:
        return QStringLiteral( "Dolphin" );

    case CORE_DOSBOX_LIBRETRO:
        return QStringLiteral( "Dosbox" );

    case CORE_EMUX_CHIP8_LIBRETRO:
        return QStringLiteral( "Emux Chip8" );

    case CORE_EMUX_GB_LIBRETRO:
        return QStringLiteral( "Emux Gb" );

    case CORE_EMUX_NES_LIBRETRO:
        return QStringLiteral( "Emux Nes" );

    case CORE_EMUX_SMS_LIBRETRO:
        return QStringLiteral( "Emux Sms" );

    case CORE_FB_ALPHA_CPS1_LIBRETRO:
        return QStringLiteral( "Fb Alpha Cps1" );

    case CORE_FB_ALPHA_CPS2_LIBRETRO:
        return QStringLiteral( "Fb Alpha Cps2" );

    case CORE_FB_ALPHA_LIBRETRO:
        return QStringLiteral( "Fb Alpha" );

    case CORE_FB_ALPHA_NEO_LIBRETRO:
        return QStringLiteral( "Fb Alpha Neo" );

    case CORE_FBA_LIBRETRO:
        return QStringLiteral( "Fba" );

    case CORE_FCEUMM_LIBRETRO:
        return QStringLiteral( "Fceumm" );

    case CORE_FFMPEG_LIBRETRO:
        return QStringLiteral( "Ffmpeg" );

    case CORE_FMSX_LIBRETRO:
        return QStringLiteral( "Fmsx" );

    case CORE_FUSE_LIBRETRO:
        return QStringLiteral( "Fuse" );

    case CORE_GAMBATTE_LIBRETRO:
        return QStringLiteral( "Gambatte" );

    case CORE_GENESIS_PLUS_GX_LIBRETRO:
        return QStringLiteral( "Genesis Plus Gx" );

    case CORE_GPSP_LIBRETRO:
        return QStringLiteral( "Gpsp" );

    case CORE_GW_LIBRETRO:
        return QStringLiteral( "Gw" );

    case CORE_HANDY_LIBRETRO:
        return QStringLiteral( "Handy" );

    case CORE_HATARI_LIBRETRO:
        return QStringLiteral( "Hatari" );

    case CORE_IMAGEVIEWER_LIBRETRO:
        return QStringLiteral( "Imageviewer" );

    case CORE_IMAME4ALL_LIBRETRO:
        return QStringLiteral( "Imame4All" );

    case CORE_LUTRO_LIBRETRO:
        return QStringLiteral( "Lutro" );

    case CORE_MAME078_LIBRETRO:
        return QStringLiteral( "Mame078" );

    case CORE_MAME2010_LIBRETRO:
        return QStringLiteral( "Mame2010" );

    case CORE_MAME2014_LIBRETRO:
        return QStringLiteral( "Mame2014" );

    case CORE_MAME_LIBRETRO:
        return QStringLiteral( "Mame" );

    case CORE_MEDNAFEN_GBA_LIBRETRO:
        return QStringLiteral( "Mednafen Gba" );

    case CORE_MEDNAFEN_LYNX_LIBRETRO:
        return QStringLiteral( "Mednafen Lynx" );

    case CORE_MEDNAFEN_NGP_LIBRETRO:
        return QStringLiteral( "Mednafen Ngp" );

    case CORE_MEDNAFEN_PCE_FAST_LIBRETRO:
        return QStringLiteral( "Mednafen Pce Fast" );

    case CORE_MEDNAFEN_PCFX_LIBRETRO:
        return QStringLiteral( "Mednafen Pcfx" );

    case CORE_MEDNAFEN_PSX_LIBRETRO:
        return QStringLiteral( "Mednafen Psx" );

    case CORE_MEDNAFEN_SNES_LIBRETRO:
        return QStringLiteral( "Mednafen Snes" );

    case CORE_MEDNAFEN_SUPERGRAFX_LIBRETRO:
        return QStringLiteral( "Mednafen Supergrafx" );

    case CORE_MEDNAFEN_VB_LIBRETRO:
        return QStringLiteral( "Mednafen Vb" );

    case CORE_MEDNAFEN_WSWAN_LIBRETRO:
        return QStringLiteral( "Mednafen Wswan" );

    case CORE_MESS2014_LIBRETRO:
        return QStringLiteral( "Mess2014" );

    case CORE_METEOR_LIBRETRO:
        return QStringLiteral( "Meteor" );

    case CORE_MGBA_LIBRETRO:
        return QStringLiteral( "Mgba" );

    case CORE_MUPEN64PLUS_LIBRETRO:
        return QStringLiteral( "Mupen64Plus" );

    case CORE_NESTOPIA_LIBRETRO:
        return QStringLiteral( "Nestopia" );

    case CORE_NXENGINE_LIBRETRO:
        return QStringLiteral( "Nxengine" );

    case CORE_O2EM_LIBRETRO:
        return QStringLiteral( "O2Em" );

    case CORE_PASCAL_PONG_LIBRETRO:
        return QStringLiteral( "Pascal Pong" );

    case CORE_PCSX1_LIBRETRO:
        return QStringLiteral( "Pcsx1" );

    case CORE_PCSX_REARMED_INTERPRETER_LIBRETRO:
        return QStringLiteral( "Pcsx Rearmed Interpreter" );

    case CORE_PCSX_REARMED_LIBRETRO:
        return QStringLiteral( "Pcsx Rearmed" );

    case CORE_PCSX_REARMED_LIBRETRO_NEON:
        return QStringLiteral( "Pcsx Rearmed Neon" );

    case CORE_PICODRIVE_LIBRETRO:
        return QStringLiteral( "Picodrive" );

    case CORE_POCKETSNES_LIBRETRO:
        return QStringLiteral( "Pocketsnes" );

    case CORE_PPSSPP_LIBRETRO:
        return QStringLiteral( "Ppsspp" );

    case CORE_PRBOOM_LIBRETRO:
        return QStringLiteral( "Prboom" );

    case CORE_PROSYSTEM_LIBRETRO:
        return QStringLiteral( "Prosystem" );

    case CORE_PUAE_LIBRETRO:
        return QStringLiteral( "Puae" );

    case CORE_QUICKNES_LIBRETRO:
        return QStringLiteral( "Quicknes" );

    case CORE_REMOTEJOY_LIBRETRO:
        return QStringLiteral( "Remotejoy" );

    case CORE_SCUMMVM_LIBRETRO:
        return QStringLiteral( "Scummvm" );

    case CORE_SNES9X_LIBRETRO:
        return QStringLiteral( "Snes9X" );

    case CORE_SNES9X_NEXT_LIBRETRO:
        return QStringLiteral( "Snes9X Next" );

    case CORE_STELLA_LIBRETRO:
        return QStringLiteral( "Stella" );

    case CORE_STONESOUP_LIBRETRO:
        return QStringLiteral( "Stonesoup" );

    case CORE_TEMPGBA_LIBRETRO:
        return QStringLiteral( "Tempgba" );

    case CORE_TEST_LIBRETRO:
        return QStringLiteral( "Test" );

    case CORE_TESTGL_LIBRETRO:
        return QStringLiteral( "Testgl" );

    case CORE_TGBDUAL_LIBRETRO:
        return QStringLiteral( "Tgbdual" );

    case CORE_TYRQUAKE_LIBRETRO:
        return QStringLiteral( "Tyrquake" );

    case CORE_UME2014_LIBRETRO:
        return QStringLiteral( "Ume2014" );

    case CORE_VBA_NEXT_LIBRETRO:
        return QStringLiteral( "Vba Next" );

    case CORE_VBAM_LIBRETRO:
        return QStringLiteral( "Vbam" );

    case CORE_VECX_LIBRETRO:
        return QStringLiteral( "Vecx" );

    case CORE_VIRTUALJAGUAR_LIBRETRO:
        return QStringLiteral( "Virtualjaguar" );

    case CORE_YABAUSE_LIBRETRO:
        return QStringLiteral( "Yabause" );
    default:
        // This default case should never be called
        break;
    }
    Q_ASSERT( false );
    return QStringLiteral( "" );
}