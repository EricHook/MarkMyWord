//
//  MarkMyWordSettings.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/17/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit

var mmwGame = MMWGame()

class MMWGame {
    
    var mainMenuScene : MainMenuScene?
    var mmwGameScene : MMWGameScene?
    var mmwGameSceneViewController : MMWGameSceneViewController?
    var screenSize : CGSize?
    
    
    init () {
 
    }
    
    
    func setMainMenuScene (mainMenuScene : MainMenuScene) {
        self.mainMenuScene = mainMenuScene
        screenSize = mainMenuScene.viewSize
    }
    
    
    func setMMWGameScene (mmwGameScene : MMWGameScene) {
        self.mmwGameScene = mmwGameScene
    }
    
    
    func setMMWGameScene (mmwGameSceneViewController : MMWGameSceneViewController) {
        self.mmwGameSceneViewController = mmwGameSceneViewController
    }
    
    
    func setScreenSize (screenSize : CGSize) {
        self.screenSize = screenSize
    }
}

//class MMWGame {
//    
//    var numPlayers : Int = -1
//    var haveColoredBombs = false
//    var haveDirectionalBombs = false
//    var haveBonusTiles = false
//    var haveBlockingTIles = false
//    var gridMode = false
//    var haveMysteryTiles = false
//    var allowOffensiveWords = false
//    var secondsLabel = false
//    var secondsEditBox = false
//    var minWordSize = false
//    var verboseAI = true
//
//
//}