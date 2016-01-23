//
//  MMWPlayer.swift
//  MarkMyWord
//
//  Created by Eric Hook on 5/14/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

class Player {
    
    var playerID    = 0
    var playerName  = "Player"
    var playerScore = 0
    var playerColor = 0

    var isHuman = true
    
    var playerAvatarNumber = 0
    
    var playerMeyamaNumber = 0
    
    var playerSkillLevel = 1
    
    enum playerSkillLevel : Int {
        case low = 0, mid = 1, high = 2
    }

    var playerLetterGrid: Grid! = nil
    var playerView : PlayerView! = nil
 
    init (_playerID : Int, _playerName : String, _playerColor : Int) {
        playerColor = _playerColor
        playerID = _playerID
        playerName = _playerName
        playerAvatarNumber = (playerID > 1) ? ( playerID - 1 ) : 0
        playerMeyamaNumber = (playerID > 1) ? ( playerID - 1 ) : 0
    }

    func setPlayerTilesGrid (inout playerTilesGrid: Grid) {
        self.playerLetterGrid = playerTilesGrid
    }
    
    func setPlayerView (inout playerView: PlayerView) {
        self.playerView = playerView
    }
}