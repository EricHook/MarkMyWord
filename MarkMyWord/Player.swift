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
    
    var playerID : Int = 0
    var playerName : String = ""
    var playerScore : Int = 0
    var doIt : Bool = false
    var didIt : Bool = false
    //var playerSeat : MMWSeat
    var playerColor : Int = 0
    
    var playerLetterTiles : [MMWTile]! = nil
    var playerLetterGrid: Grid! = nil
    
    var playerView : PlayerView! = nil
    
    var playerSpecialTiles : [MMWTile]! = nil
    var playerLetterTilesPlayed : [MMWTile]? = nil
    
    init (_playerID : Int, _playerName : String, _playerColor : Int) {
        //playerSeat = MMWSeat(_playerSeatNum: 1, _seatColorNumber: _playerColor)
        //playerColor = playerSeat.seatUIColor
        playerColor = _playerColor
        playerID = _playerID
        playerName = _playerName
    }
    
    func setPlayerTilesArray (inout playerTiles: [MMWTile]) {
        self.playerLetterTiles = playerTiles
    }
    
    func setPlayerTilesGrid (inout playerTilesGrid: Grid) {
        self.playerLetterGrid = playerTilesGrid
    }
    
//    func checkPlayerTilesGrid (playerTilesGrid: Grid) -> Int {
//        var numBlankTiles = 0
//        for tileX in playerTilesGrid.grid2DArr {
//            for tileY in tileX {
//                playerTilesGrid
//                if tileY.spritename == "Blank" {
//                    
//                    (playerTilesGrid.grid2DArr  .gridArr.tileSprite.tileSpriteParent.tileType)
//                    
//                    numBlankTiles ++
//                }
//            }
//        }
//    }
    
    func setPlayerView (playerView: PlayerView) {
        self.playerView = playerView
    }
    
//    getNewTiles (numTilesToGet: Int) {
//        while playerLetterTiles.count < 7 &&
//    }
    
}