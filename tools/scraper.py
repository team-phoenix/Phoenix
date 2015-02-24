#!/usr/local/bin/python3

import sys
import urllib.request as urlRequest
from xml.etree import cElementTree as ET
from pprint import pprint
from collections import defaultdict
from difflib import SequenceMatcher as SM

def valuePrint(message, percentage):
    if percentage == 100.0:
        print()
    else:
        sys.stdout.write("\r{0}{1}%".format(percentage, message))
        sys.stdout.flush()

class Scraper:
    def __init__(self, baseUrl, 
                 userAgent="Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.7) Gecko/2009021910 Firefox/3.0.7"):
                 
        self.baseUrl = baseUrl
        self.userAgent = userAgent
        self.headers={"User-Agent": userAgent}
    
    def getUrlResult(self, argv=[]):
        new_url = self.baseUrl + "".join(argv)
        request = urlRequest.Request(url=new_url, headers=self.headers)
        return urlRequest.urlopen(request)
            
    def etree_to_dict(self, t):
        d = {t.tag: {} if t.attrib else None}
        children = list(t)
        if children:
            dd = defaultdict(list)
            for dc in map(self.etree_to_dict, children):
                for k, v in dc.items():
                    dd[k].append(v)
            d = {t.tag: {k:v[0] if len(v) == 1 else v for k, v in dd.items()}}
        if t.attrib:
            d[t.tag].update(('@' + k, v) for k, v in t.attrib.items())
        if t.text:
            text = t.text.strip()
            if children or t.attrib:
                if text:
                  d[t.tag]['#text'] = text
            else:
                d[t.tag] = text
        return d
    
    def compareString(self, string1, string2):
        return SM(None, string1.lower(), string2.lower()).ratio()
        
    def fetch(self):
        '''
        Reimplement fetch()
        '''
        pass


class TheGamesDB(Scraper):
    '''
        Normal Usage:
            call fetch() method and get a list of game data.
    '''
    def __init__(self):
         super().__init__(baseUrl="http://thegamesdb.net/api/")
         self.gameFlag = "GetGame.php?id="
         self.artFlag = "GetArt.php?id="
         self.platformsListFlag = "GetPlatformsList.php"
         self.gamesListFlag = "GetGamesList.php?name="
         
         
    def getGame(self, id, format="json"):
        id = str(id)
        '''
        getGame(id=integer),
        returns an XML response as bytes or a dictionary,
        depending upon the format.
        '''
        
        if "json" in format:
            return self.etree_to_dict(ET.XML(self.getUrlResult(self.gameFlag + id).read()))
        
        return self.getUrlResult(self.gameFlag + id).read()
        
    def getArt(self, id, format="json"):
        id = str(id)
        if "json" in format:
            return self.etree_to_dict(ET.XML(self.getUrlResult(self.artFlag + id).read()))
        return self.getUrlResult(self.artFlag + id).read()
        
    def getPlatformsList(self, format="json"):
        if "json" in format:
            return self.etree_to_dict(ET.XML(self.getUrlResult(self.platformsListFlag).read()))
        return self.getUrlResult(self.platformsListFlag).read()
            

    def getGamesList(self, gameName, format="json"):
        if "json" in format:
            return self.etree_to_dict(ET.XML(self.getUrlResult(self.gamesListFlag + gameName).read()))
        return self.getUrlResult(self.gamesListFlag + gameName).read()
        
    def getID(self, gameName, platform):
        game_list = self.getGamesList(gameName)["Data"]["Game"]
        id = -1
        previous_ratio = 0.0
        previous_platform_ratio = 0.0
        for index, game in enumerate(game_list):
            current_ratio = self.compareString(gameName, game["GameTitle"])
            platform_ratio = self.compareString(platform, game["Platform"])
            
            
            if current_ratio > previous_ratio:
                previous_ratio = current_ratio
                previous_platform_ratio = platform_ratio
                id = game["id"]
                
            elif current_ratio == previous_ratio:
                if platform_ratio > previous_platform_ratio:
                    id = game["id"]
                    previous_platform_ratio = platform_ratio
            
                
        if id == -1:
            print("id was not found for: ", gameName)
 
            
        return str(id)
        
    def fetch(self, games = []):
        '''
        games = [
            [gameName, platform],
            [gameName, platform],
            [gameName, platform],
            ...
            ]
        '''
        ids = []
        for index, game in enumerate(games):
            ids.append(self.getID(game[0], game[1]))
            valuePrint(((index+1) / len(games)) * 100, "Getting id's... ")
    
        games_list = []
        for index, id in enumerate(ids):
            games_list.append(self.getGame(id))
            valuePrint(((index+1) / len(ids)) * 100, "Getting data... ")
        
        return games_list

'''    
games = [
    ["Super Mario Bros", "Nintendo Entertainment System"],
    ["Chrono Trigger", "Super Nintendo"],
    ["Mega Man", "Nintendo Entertainment System"],
    ["Super Mario 64", "Nintendo 64"],
    ["Super Mario Bros", "Nintendo Entertainment System"],
    ["Chrono Trigger", "Super Nintendo"],
    ["Mega Man", "Nintendo Entertainment System"],
    ["Super Mario 64", "Nintendo 64"],
]

    
def main():
    thegamesdb = TheGamesDB()
    thegamesdb.fetch(games)
                                    
main()
'''
