import os

from collections import OrderedDict

from .sqldatabase import SqlDatabase
from .retrieve_core_info import retrieveCoreInfo

# Root class that all SQL table updaters derive from
class SqlTableUpdater():

    def __init__(self, tableName, tableColumns=[], coreInfo={}):
        self.tableName = tableName
        self.columnsDict = OrderedDict(tableColumns)
        self.dbFile = os.path.join(os.getcwd().replace("python", "metadata"), "libretro.sqlite")
        self.dbFileExists = os.path.isfile(self.dbFile)
        self.coreInfo = coreInfo

        # self.filterUnusedCores()

    def updateTable(self):
        pass

    def updateColumns(self, database, additionalStatement: str = ""):

        if not self.dbFileExists:
            database.createTable(self.tableName, self.columnsDict, additionalStatement)
        else:
            try:
                database.deleteTable(self.tableName)
            except:
                database.createTable(self.tableName, self.columnsDict, additionalStatement)

    def __del__(self):
        print("Updated " + self.tableName + " table.")

    def libretroSystemList(self):
        systems = []
        for k, v in self.coreInfo['cores'].items():

            if "categories" not in v or v["categories"] != "Emulator":
                continue

            if "database" in v:
                name = v["database"].split("|")

                for n in name:
                    systems.append(n)

                    # Split console and manufacturer names
                    # Not really necessary for Libretro identifiers
                    #tup = n.split(" - ")
                    #
                    ## "MAME"
                    #if len(tup) == 1:
                    #    systems.append(tup[0])
                    #
                    ## Nearly every one
                    #elif len(tup) == 2:
                    #    systems.append(tup[1])
                    #
                    ## Sega - Master System - Mark III
                    ## Sega - Mega Drive - Genesis
                    #elif len(tup) == 3:
                    #    systems.append(tup[1])

            # There are some cores that do not have "database" defined
            elif "systemname" in v:
                systems.append(v["systemname"])

        systems = list(set(systems))
        systems.sort()
        return systems

    # This map defines all Libretro-based systems that Phoenix supports. If it isn't in here, it isn't supported by Phoenix!
    # TODO: Place this information into an entirely separate database
    # WARNING: Do NOT change Phoenix UUIDs (1st column), even if there are spelling mistakes. Change friendlyName if you really need to.
    phoenixSystemDatabase = {
    # friendlyName: North American console name without manufacturer
    # shortName: Abbreviation (typically 3 letters)
    # enabled: True iff a core is available, Phoenix can run it, and the game scanner can find it (extensions set)

        # Everything else
        "Arcade":                                           {"enabled": False, "defaultCore": "mame_libretro",                      "friendlyName": "",                    "shortName": "", "manufacturer": "(Various)"       },

        # Conspicuously missing from No-Intro
        "Amstrad - CPC":                                    {"enabled": False, "defaultCore": "cap32_libretro",                     "friendlyName": "",                    "shortName": "", "manufacturer": "Amstrad"         },
        "Atari - 2600":                                     {"enabled": True,  "defaultCore": "stella_libretro",                    "friendlyName": "",                    "shortName": "", "manufacturer": "Atari"           },
        "Capcom - CP System I":                             {"enabled": False, "defaultCore": "fb_alpha_cps1_libretro",             "friendlyName": "",                    "shortName": "", "manufacturer": "Capcom"          },
        "Capcom - CP System II":                            {"enabled": False, "defaultCore": "fb_alpha_cps2_libretro",             "friendlyName": "",                    "shortName": "", "manufacturer": "Capcom"          },
        "Capcom - CP System III":                           {"enabled": False, "defaultCore": "fbalpha2012_cps3_libretro",          "friendlyName": "",                    "shortName": "", "manufacturer": "Capcom"          },
        "Capcom - CPS Changer":                             {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Capcom"          },
        "CHIP-8":                                           {"enabled": False, "defaultCore": "emux_chip8_libretro",                "friendlyName": "",                    "shortName": "", "manufacturer": "(Various)"       },
        "DOS":                                              {"enabled": False, "defaultCore": "dosbox_libretro",                    "friendlyName": "",                    "shortName": "", "manufacturer": "(Various)"       },
        "Mattel - Intellivision":                           {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Mattel"          },
        "Nintendo - Game & Watch":                          {"enabled": False, "defaultCore": "gw_libretro",                        "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Sinclair - ZX81":                                  {"enabled": False, "defaultCore": "81_libretro",                        "friendlyName": "",                    "shortName": "", "manufacturer": "Sinclair"        },
        "SNK - Neo Geo":                                    {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "SNK"             },

        # No-Intro, both official and non-official (ROM-based games)
        "Atari - 5200":                                     {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Atari"           },
        "Atari - 7800":                                     {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Atari"           },
        "Atari - Jaguar":                                   {"enabled": True,  "defaultCore": "virtualjaguar_libretro",             "friendlyName": "",                    "shortName": "", "manufacturer": "Atari"           },
        "Atari - Lynx":                                     {"enabled": True,  "defaultCore": "mednafen_lynx_libretro",             "friendlyName": "",                    "shortName": "", "manufacturer": "Atari"           },
        "Atari - ST":                                       {"enabled": True,  "defaultCore": "hatari_libretro",                    "friendlyName": "",                    "shortName": "", "manufacturer": "Atari"           },
        "Bandai - WonderSwan Color":                        {"enabled": True,  "defaultCore": "mednafen_wswan_libretro",            "friendlyName": "",                    "shortName": "", "manufacturer": "Bandai"          },
        "Bandai - WonderSwan":                              {"enabled": True,  "defaultCore": "mednafen_wswan_libretro",            "friendlyName": "",                    "shortName": "", "manufacturer": "Bandai"          },
        "Casio - Loopy":                                    {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Casio"           },
        "Casio - PV-1000":                                  {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Casio"           },
        "Coleco - ColecoVision":                            {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Coleco"          },
        #"Commodore - 64 (PP)":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        #"Commodore - 64 (Tapes)":                          {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        "Commodore - 64":                                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        "Commodore - Amiga":                                {"enabled": True,  "defaultCore": "puae_libretro",                      "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        "Commodore - Plus-4":                               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        "Commodore - VIC-20":                               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        "Emerson - Arcadia 2001":                           {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Emerson"         },
        "Entex - Adventure Vision":                         {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Entex"           },
        "Epoch - Super Cassette Vision":                    {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Epoch"           },
        "Fairchild - Channel F":                            {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Fairchild"       },
        "Funtech - Super Acan":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Funtech"         },
        "GamePark - GP32":                                  {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "GamePark"        },
        "GCE - Vectrex":                                    {"enabled": True,  "defaultCore": "vecx_libretro",                      "friendlyName": "",                    "shortName": "", "manufacturer": "GCE"             },
        "Hartung - Game Master":                            {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Hartung"         },
        "LeapFrog - Leapster Learning Game System":         {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "LeapFrog"        },
        "Magnavox - Odyssey2":                              {"enabled": False, "defaultCore": "o2em_libretro",                      "friendlyName": u"Odyssey²",           "shortName": "", "manufacturer": "Magnavox"        },
        "Microsoft - MSX 2":                                {"enabled": False, "defaultCore": "bluemsx_libretro",                   "friendlyName": "MSX2",                "shortName": "", "manufacturer": "Microsoft"       },
        "Microsoft - MSX":                                  {"enabled": False, "defaultCore": "bluemsx_libretro",                   "friendlyName": "",                    "shortName": "", "manufacturer": "Microsoft"       },
        #"Microsoft - XBOX 360 (DLC)":                      {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Microsoft"       },
        #"Microsoft - XBOX 360 (Games on Demand)":          {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Microsoft"       },
        #"Microsoft - XBOX 360 (Title Updates)":            {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Microsoft"       },
        "NEC - PC Engine - TurboGrafx 16":                  {"enabled": True,  "defaultCore": "mednafen_pce_fast_libretro",         "friendlyName": "TurboGrafx 16",       "shortName": "", "manufacturer": "NEC"             },
        "NEC - Super Grafx":                                {"enabled": True,  "defaultCore": "mednafen_supergrafx_libretro",       "friendlyName": "SuperGrafx",          "shortName": "", "manufacturer": "NEC"             },
        #"Nintendo - Famicom Disk System":                  {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Game Boy Advance (e-Cards)":            {"enabled": True,  "defaultCore": "vbam_libretro",                      "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Game Boy Advance":                      {"enabled": True,  "defaultCore": "vbam_libretro",                      "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Game Boy Color":                        {"enabled": True,  "defaultCore": "gambatte_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Game Boy":                              {"enabled": True,  "defaultCore": "gambatte_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        #"Nintendo - New Nintendo 3DS (DLC)":               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - New Nintendo 3DS":                      {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        #"Nintendo - Nintendo 3DS (DLC)":                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Nintendo 3DS":                          {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Nintendo 64":                           {"enabled": True,  "defaultCore": "mupen64plus_libretro",               "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        #"Nintendo - Nintendo DS (Download Play) (BETA)":   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Nintendo DS":                           {"enabled": True,  "defaultCore": "desmume_libretro",                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        #"Nintendo - Nintendo DSi (DLC)":                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Nintendo DSi":                          {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Nintendo Entertainment System":         {"enabled": True,  "defaultCore": "fceumm_libretro",                    "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        #"Nintendo - Nintendo Wii (DLC)":                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Pokemon Mini":                          {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Satellaview":                           {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Sufami Turbo":                          {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Super Nintendo Entertainment System":   {"enabled": True,  "defaultCore": "bsnes_mercury_balanced_libretro",    "friendlyName": "Super Nintendo",      "shortName": "", "manufacturer": "Nintendo"        },
        "Nintendo - Virtual Boy":                           {"enabled": True,  "defaultCore": "mednafen_vb_libretro",               "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Nokia - N-Gage":                                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nokia"           },
        "Philips - Videopac+":                              {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Philips"         },
        "RCA - Studio II":                                  {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "RCA"             },
        "Sega - 32X":                                       {"enabled": True,  "defaultCore": "picodrive_libretro",                 "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sega - Game Gear":                                 {"enabled": True,  "defaultCore": "genesis_plus_gx_libretro",           "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sega - Master System - Mark III":                  {"enabled": False, "defaultCore": "emux_sms_libretro",                  "friendlyName": "Master System",       "shortName": "", "manufacturer": "Sega"            },
        "Sega - Mega Drive - Genesis":                      {"enabled": True,  "defaultCore": "genesis_plus_gx_libretro",           "friendlyName": "Genesis",             "shortName": "", "manufacturer": "Sega"            },
        "Sega - PICO":                                      {"enabled": True,  "defaultCore": "picodrive_libretro",                 "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sega - SG-1000":                                   {"enabled": True,  "defaultCore": "genesis_plus_gx_libretro",           "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sinclair - ZX Spectrum +3":                        {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sinclair"        },
        "SNK - Neo Geo Pocket Color":                       {"enabled": True,  "defaultCore": "mednafen_ngp_libretro",              "friendlyName": "",                    "shortName": "", "manufacturer": "SNK"             },
        "SNK - Neo Geo Pocket":                             {"enabled": True,  "defaultCore": "mednafen_ngp_libretro",              "friendlyName": "",                    "shortName": "", "manufacturer": "SNK"             },
        #"Sony - PlayStation 3 (DLC)":                      {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        #"Sony - PlayStation 3 (Downloadable)":             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        #"Sony - PlayStation 3 (PSN)":                      {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        #"Sony - PlayStation Portable (DLC)":               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        #"Sony - PlayStation Portable (PSN)":               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        #"Sony - PlayStation Portable (PSX2PSP)":           {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        #"Sony - PlayStation Portable (UMD Music)":         {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        #"Sony - PlayStation Portable (UMD Video)":         {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        "Sony - PlayStation Portable":                      {"enabled": True,  "defaultCore": "ppsspp_libretro",                    "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        "Tiger - Game.com":                                 {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Tiger"           },
        "Tiger - Gizmondo":                                 {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Tiger"           },
        "VTech - CreatiVision":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "VTech"           },
        "VTech - V.Smile":                                  {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "VTech"           },
        "Watara - Supervision":                             {"enabled": True,  "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Watara"          },

        # Redump.org (disc-based games)
        "Apple - Macintosh":                                {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Apple"           },
        "Bandai - Playdia":                                 {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Bandai"          },
        "Bandai / Apple - Pippin":                          {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Bandai / Apple"  },
        "Commodore - Amiga CD":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        "Commodore - Amiga CD32":                           {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        "Commodore - Amiga CDTV":                           {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Commodore"       },
        "Fujitsu - FM Towns series":                        {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "Fujitsu"         },
        "IBM PC compatible":                                {"enabled": False, "defaultCore": "",                                   "friendlyName": "PC",                  "shortName": "", "manufacturer": "(Various)"       },
        "Mattel - HyperScan":                               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Mattel"          },
        "Microsoft - Xbox":                                 {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Microsoft"       },
        "Namco / Sega / Nintendo - Triforce":               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "NEC - PC Engine CD - TurboGrafx-CD":               {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "TurboGrafx-CD",       "shortName": "", "manufacturer": "NEC"             },
        "NEC - PC-88 series":                               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "NEC"             },
        "NEC - PC-98 series":                               {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "NEC"             },
        "NEC - PC-FX - PC-FXGA":                            {"enabled": False, "defaultCore": "mednafen_pcfx_libretro",             "friendlyName": "",                    "shortName": "", "manufacturer": "NEC"             },
        "Nintendo - GameCube":                              {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Palm OS":                                          {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Palm"            },
        "Panasonic - 3DO Interactive Multiplayer":          {"enabled": False, "defaultCore": "4do_libretro",                       "friendlyName": "",                    "shortName": "", "manufacturer": "Panasonic"       },
        "Philips - CD-i":                                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Philips"         },
        "Photo - CD":                                       {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "(Various)"       },
        "Sega - Chihiro":                                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sega - Dreamcast":                                 {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sega - Lindbergh":                                 {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sega - Mega-CD":                                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sega - Naomi":                                     {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "Sega - Saturn":                                    {"enabled": True,  "defaultCore": "yabause_libretro",                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sega"            },
        "SNK - Neo Geo CD":                                 {"enabled": False, "defaultCore": "mess2014_libretro",                  "friendlyName": "",                    "shortName": "", "manufacturer": "SNK"             },
        "Sony - PlayStation 2":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        "Sony - PlayStation":                               {"enabled": True,  "defaultCore": "mednafen_psx_libretro",              "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        "VTech - V.Flash":                                  {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "VTech"           },

        # Seventh-generation consoles (circa 2005)
        "Microsoft - Xbox 360":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Microsoft"       },
        "Nintendo - Wii":                                   {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Sony - PlayStation 3":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },

        # Eighth-generation consoles (circa 2012)
        "Microsoft - Xbox One":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Microsoft"       },
        "Nintendo - Wii U":                                 {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
        "Sony - PlayStation 4":                             {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Sony"            },
        
        # Ninth-generation consoles (circa 2017)
        "Microsoft - Xbox One X":                           {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Microsoft"       },
        "Nintendo - Switch":                                {"enabled": False, "defaultCore": "",                                   "friendlyName": "",                    "shortName": "", "manufacturer": "Nintendo"        },
}

    def phoenixSystems(self):
        return OrderedDict(sorted(self.phoenixSystemDatabase.items(), key=lambda t: t[0]))

    def libretroToPhoenix(self, libretroSystem):
        return self.libretroToPhoenixMap[libretroSystem]

    # This map essentially says "Register this Libretro core for this (these) Phoenix system(s)" when a .info file claims support for that system
    # If a core claims support for some Libretro ID, register that core for each Phoenix ID
    libretroToPhoenixMap = {
        "3DO":                                                      {"Panasonic - 3DO Interactive Multiplayer"},
        "Arcade (various)":                                         {"Arcade"},
        "Atari - 2600":                                             {"Atari - 2600"},
        "Atari - 5200":                                             {"Atari - 5200"},
        "Atari - 7800":                                             {"Atari - 7800"},
        "Atari - Jaguar":                                           {"Atari - Jaguar"},
        "Atari - Lynx":                                             {"Atari - Lynx"},
        "Atari ST/STE/TT/Falcon":                                   {"Atari - ST"},
        "Bandai - WonderSwan Color":                                {"Bandai - WonderSwan Color"},
        "Bandai - WonderSwan":                                      {"Bandai - WonderSwan"},
        "CHIP-8":                                                   {"CHIP-8"},
        "Commodore Amiga":                                          {"Commodore - Amiga"},
        "Commodore - C128":                                         {"Arcade"},
        "Commodore - 64":                                           {"Commodore - 64"},
        "CP System I":                                              {"Capcom - CP System I"},
        "CP System II":                                             {"Capcom - CP System II"},
        "CP System III":                                            {"Capcom - CP System III"},
        "CPC":                                                      {"Amstrad - CPC"},
        "DOS":                                                      {"DOS"},
        "FB Alpha - Arcade Games":                                  {"Arcade"},
        "GCE - Vectrex":                                            {"GCE - Vectrex"},
        "Handheld Electronic Game":                                 {"Nintendo - Game & Watch"},
        "IBM PC compatible":                                        {"IBM PC compatible"},
        "Magnavox - Odyssey2":                                      {"Magnavox - Odyssey2"},
        "MAME":                                                     {"Arcade"},
        "MAME2003":                                                 {"Arcade"},
        "Microsoft - MSX 2":                                        {"Microsoft - MSX 2"},
        "Microsoft - MSX2":                                         {"Microsoft - MSX 2"},
        "Microsoft - MSX":                                          {"Microsoft - MSX"},

        # MESS and UME
        # http://nonmame.retrogames.com/
        "MULTI (various)":                                          {
                                                                     "Atari - 2600",
                                                                     "Atari - 5200",
                                                                     "Atari - 7800",
                                                                     "Atari - Lynx",
                                                                     "Bandai - WonderSwan Color"
                                                                     "Bandai - WonderSwan",
                                                                     "Capcom - CPS Changer",
                                                                     "Coleco - ColecoVision",
                                                                     "Fujitsu - FM Towns series",
                                                                     "Magnavox - Odyssey2",
                                                                     "Mattel - Intellivision",
                                                                     "NEC - PC Engine - TurboGrafx 16",
                                                                     "NEC - PC Engine CD - TurboGrafx-CD",
                                                                     "NEC - Super Grafx",
                                                                     "Nintendo - Game Boy Advance",
                                                                     "Nintendo - Game Boy",
                                                                     "Philips - Videopac+",
                                                                     "RCA - Studio II",
                                                                     "Sega - Game Gear",
                                                                     "Sega - Master System - Mark III",
                                                                     "Sega - Mega Drive - Genesis",
                                                                     "Sega - PICO",
                                                                     "Sega - SG-1000",
                                                                     "SNK - Neo Geo CD",
                                                                     "SNK - Neo Geo",
                                                                     "Watara - Supervision",
                                                                    },

        "NEC - PC Engine - TurboGrafx 16":                          {"NEC - PC Engine - TurboGrafx 16"},
        "NEC - PC Engine SuperGrafx":                               {"NEC - Super Grafx"},
        "NEC - PC Engine CD - TurboGrafx-CD":                       {"NEC - Super Grafx"},
        "NEC - PC-FX":                                              {"NEC - PC-FX - PC-FXGA"},
        "NEC - Super Grafx":                                        {"NEC - Super Grafx"},
        "Neo Geo":                                                  {"SNK - Neo Geo"},
        "Nintendo - 3DS":                                           {"Nintendo - Nintendo 3DS"},
        "Nintendo - Family Computer Disk System":                   {"Nintendo - Nintendo Entertainment System"},
        "Nintendo - Famicom Disk System":                           {"Nintendo - Nintendo Entertainment System"},
        "Nintendo - Game & Watch":                                  {"Nintendo - Game & Watch"},
        "Nintendo - Game Boy Advance (e-Cards)":                    {"Nintendo - Game Boy Advance (e-Cards)"},
        "Nintendo - Game Boy Advance":                              {"Nintendo - Game Boy Advance"},
        "Nintendo - Game Boy Color":                                {"Nintendo - Game Boy Color"},
        "Nintendo - Game Boy":                                      {"Nintendo - Game Boy"},
        "Nintendo - GameCube":                                      {"Nintendo - GameCube"},
        "Nintendo - Nintendo 64":                                   {"Nintendo - Nintendo 64"},
        "Nintendo - Nintendo 64DD":                                 {"Nintendo - Nintendo 64"},
        "Nintendo - Nintendo DS":                                   {"Nintendo - Nintendo DS"},
        "Nintendo - Nintendo DS (Download Play)":                   {"Nintendo - Nintendo DS"},
        "Nintendo - Nintendo DS (Download Play) (BETA)":            {"Nintendo - Nintendo DS"},
        "Nintendo - Nintendo DS Decrypted":                         {"Nintendo - Nintendo DS"},
        "Nintendo - Nintendo Entertainment System":                 {"Nintendo - Nintendo Entertainment System"},
        "Nintendo - Pokemon Mini":                                  {"Nintendo - Pokemon Mini"},
        "Nintendo - Sufami Turbo":                                  {"Nintendo - Sufami Turbo"},
        "Nintendo - Super Nintendo Entertainment System":           {"Nintendo - Super Nintendo Entertainment System"},
        "Nintendo - Virtual Boy":                                   {"Nintendo - Virtual Boy"},
        "Nintendo - Wii":                                           {"Nintendo - Wii"},
        "PC":                                                       {"IBM PC compatible"},
        "PC-FX":                                                    {"NEC - PC-FX - PC-FXGA"},
        "PC-98":                                                    {"NEC - PC-98 series"},
        "Phillips - Videopac+":                                     {"Philips - Videopac+"},
        "Sega - 32X":                                               {"Sega - 32X"},
        "Sega - Dreamcast":                                         {"Sega - Dreamcast"},
        "Sega - Game Gear":                                         {"Sega - Game Gear"},
        "Sega - Master System - Mark III":                          {"Sega - Master System - Mark III"},
        "Sega - Mega Drive - Genesis":                              {"Sega - Mega Drive - Genesis"},
        "Sega - Mega-CD - Sega CD":                                 {"Sega - Mega-CD"},
        "Sega - NAOMI":                                             {"Sega - Naomi"},
        "Sega - PICO":                                              {"Sega - PICO"},
        "Sega - Saturn":                                            {"Sega - Saturn"},
        "Sega - SG-1000":                                           {"Sega - SG-1000"},
        "Sharp - X68000":                                           {"Arcade"},
        "Sinclair - ZX 81":                                         {"Sinclair - ZX81"},
        "Sinclair - ZX Spectrum":                                   {"Sinclair - ZX Spectrum +3"},
        "Sinclair - ZX Spectrum +3":                                {"Sinclair - ZX Spectrum +3"},
        "SNK - Neo Geo Pocket Color":                               {"SNK - Neo Geo Pocket Color"},
        "SNK - Neo Geo Pocket":                                     {"SNK - Neo Geo Pocket"},
        "Sony - PlayStation Portable":                              {"Sony - PlayStation Portable"},
        "Sony - PlayStation":                                       {"Sony - PlayStation"},
        "The 3DO Company - 3DO":                                    {"Panasonic - 3DO Interactive Multiplayer"},
        "Uzebox":                                                   {"Arcade"},
        "ZX Spectrum (various)":                                    {"Sinclair - ZX Spectrum +3"},
    }

    # Not all Phoenix IDs are availble in OpenVGDB, fail silently and gracefully if a match isn't found
    def phoenixToOpenVGDB(self, phoenixID):
        ret = ""
        try:
            ret = self.phoenixToOpenVGDBMap[phoenixID]
        except KeyError:
            ret = ""
        return ret

    phoenixToOpenVGDBMap = {
        "Panasonic - 3DO Interactive Multiplayer": "3DO Interactive Multiplayer",
        "Arcade": "Arcade",
        "Atari - 2600": "Atari 2600",
        "Atari - 5200": "Atari 5200",
        "Atari - 7800": "Atari 7800",
        "Atari - Jaguar": "Atari Jaguar CD",
        "Atari - Jaguar": "Atari Jaguar",
        "Atari - Lynx": "Atari Lynx",
        "Bandai - WonderSwan Color": "Bandai WonderSwan Color",
        "Bandai - WonderSwan": "Bandai WonderSwan",
        "Coleco - ColecoVision": "Coleco ColecoVision",
        "GCE - Vectrex": "GCE Vectrex",
        "Mattel - Intellivision": "Intellivision",
        "Magnavox - Odyssey2": "Magnavox Odyssey2",
        "NEC - PC Engine CD - TurboGrafx-CD": "NEC PC Engine CD/TurboGrafx-CD",
        "NEC - PC Engine - TurboGrafx 16": "NEC PC Engine/TurboGrafx-16",
        "NEC - PC-FX - PC-FXGA": "NEC PC-FX",
        "NEC - Super Grafx": "NEC SuperGrafx",
        "Nintendo - Nintendo 64": "Nintendo 64",
        "Nintendo - Nintendo DS": "Nintendo DS",
        "Nintendo - Nintendo Entertainment System": "Nintendo Entertainment System",
        "Nintendo - Nintendo Entertainment System": "Nintendo Famicom Disk System",
        "Nintendo - Game Boy Advance": "Nintendo Game Boy Advance",
        "Nintendo - Game Boy Color": "Nintendo Game Boy Color",
        "Nintendo - Game Boy": "Nintendo Game Boy",
        "Nintendo - GameCube": "Nintendo GameCube",
        "Nintendo - Super Nintendo Entertainment System": "Nintendo Super Nintendo Entertainment System",
        "Nintendo - Virtual Boy": "Nintendo Virtual Boy",
        "Nintendo - Wii": "Nintendo Wii",
        "Sega - 32X": "Sega 32X",
        "Sega - Mega-CD": "Sega CD/Mega-CD",
        "Sega - Game Gear": "Sega Game Gear",
        "Sega - Mega Drive - Genesis": "Sega Genesis/Mega Drive",
        "Sega - Master System - Mark III": "Sega Master System",
        "Sega - Saturn": "Sega Saturn",
        "Sega - SG-1000": "Sega SG-1000",
        "SNK - Neo Geo Pocket Color": "SNK Neo Geo Pocket Color",
        "SNK - Neo Geo Pocket": "SNK Neo Geo Pocket",
        "Sony - PlayStation Portable": "Sony PlayStation Portable",
        "Sony - PlayStation": "Sony PlayStation",
    }

    def getOpenVGDBToPhoenixMap(self):
        return OrderedDict(sorted(self.openVGDBToPhoenixMap.items(), key=lambda t: t[0]))

    openVGDBToPhoenixMap = {
        "3DO Interactive Multiplayer": "Panasonic - 3DO Interactive Multiplayer",
        "Arcade": "Arcade",
        "Atari 2600": "Atari - 2600",
        "Atari 5200": "Atari - 5200",
        "Atari 7800": "Atari - 7800",
        "Atari Jaguar CD": "Atari - Jaguar",
        "Atari Jaguar": "Atari - Jaguar",
        "Atari Lynx": "Atari - Lynx",
        "Bandai WonderSwan Color": "Bandai - WonderSwan Color",
        "Bandai WonderSwan": "Bandai - WonderSwan",
        "Coleco ColecoVision": "Coleco - ColecoVision",
        "GCE Vectrex": "GCE - Vectrex",
        "Intellivision": "Mattel - Intellivision",
        "Magnavox Odyssey2": "Magnavox - Odyssey2",
        "NEC PC Engine CD/TurboGrafx-CD": "NEC - PC Engine CD - TurboGrafx-CD",
        "NEC PC Engine/TurboGrafx-16": "NEC - PC Engine - TurboGrafx 16",
        "NEC PC-FX": "NEC - PC-FX - PC-FXGA",
        "NEC SuperGrafx": "NEC - Super Grafx",
        "Nintendo 64": "Nintendo - Nintendo 64",
        "Nintendo DS": "Nintendo - Nintendo DS",
        "Nintendo Entertainment System": "Nintendo - Nintendo Entertainment System",
        "Nintendo Famicom Disk System": "Nintendo - Nintendo Entertainment System",
        "Nintendo Game Boy Advance": "Nintendo - Game Boy Advance",
        "Nintendo Game Boy Color": "Nintendo - Game Boy Color",
        "Nintendo Game Boy": "Nintendo - Game Boy",
        "Nintendo GameCube": "Nintendo - GameCube",
        "Nintendo Super Nintendo Entertainment System": "Nintendo - Super Nintendo Entertainment System",
        "Nintendo Virtual Boy": "Nintendo - Virtual Boy",
        "Nintendo Wii": "Nintendo - Wii",
        "Sega 32X": "Sega - 32X",
        "Sega CD/Mega-CD": "Sega - Mega-CD",
        "Sega Game Gear": "Sega - Game Gear",
        "Sega Genesis/Mega Drive": "Sega - Mega Drive - Genesis",
        "Sega Master System": "Sega - Master System - Mark III",
        "Sega Saturn": "Sega - Saturn",
        "Sega SG-1000": "Sega - SG-1000",
        "SNK Neo Geo Pocket Color": "SNK - Neo Geo Pocket Color",
        "SNK Neo Geo Pocket": "SNK - Neo Geo Pocket",
        "Sony PlayStation Portable": "Sony - PlayStation Portable",
        "Sony PlayStation": "Sony - PlayStation",
    }

    def filterUnusedCores(self):
        for key in self.coreInfo["cores"].keys():
            if (

                # No reason specified
                #"4do_libretro" == key
                # or "81_libretro" == key
                # or "bluemsx_libretro" == key
                # or "bsnes_accuracy_libretro" == key
                # or "bsnes_balanced_libretro" == key
                # or "bsnes_performance_libretro" == key
                # or "cap32_libretro" == key
                # or "catsfc_libretro" == key
                # or "dosbox_libretro" == key
                # or "emux_chip8_libretro" == key
                # or "fb_alpha_cps1_libretro" == key
                # or "fb_alpha_cps2_libretro" == key
                # or "fmsx_libretro" == key
                # or "gpsp_libretro" == key
                # or "gw_libretro" == key
                # or "handy_libretro" == key
                # or "hatari_libretro" == key
                # or "imame4all_libretro" == key
                # or "mame078_libretro" == key
                # or "mame2010_libretro" == key
                # or "mame2014_libretro" == key
                # or "meteor_libretro" == key
                # or "o2em_libretro" == key
                # or "prosystem_libretro" == key
                # or "puae_libretro" == key
                # or "ume2014_libretro" == key
                # or "vecx_libretro" == key
                # or "virtualjaguar_libretro" == key

                # ARM cores
                 "pcsx" in key
                 or "pocketsnes_libretro" == key

                 ):
                del self.coreInfo["cores"][key]
