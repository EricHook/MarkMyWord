//
//  GameViewController.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/10/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit
import CoreMotion

class MMWGameSceneViewController { 
    
    var mmwGameScene: MMWGameScene!
    var viewSize : CGSize!
    var tileCollection = MMWTileBuilder()
    
    var numPlayers : Int = 2
    
    var player1 : Player = Player(_playerID: 1, _playerName: "Abe", _playerColor: 1)
    var player2 : Player = Player(_playerID: 2, _playerName: "Bart", _playerColor: 2)
    var player3 : Player = Player(_playerID: 3, _playerName: "Charlie", _playerColor: 3)
    var player4 : Player = Player(_playerID: 4, _playerName: "Dan", _playerColor: 4)

    init (size: CGSize) {
        viewSize = size
        mmwGameScene = MMWGameScene(size: viewSize)

//        self.tileCollection.mmwTileBuilderController = self
//        self.tileCollection.mmwTileBuilderScene = self.mmwGameScene
        
        /* Set the scale mode to scale to fit the window */
        mmwGameScene.scaleMode = .AspectFill
        
        mmwGameScene.setViewController(self)

        mmwGameScene.setGrids()

        setUpPlayers()

        mmwGameScene.buildGameView()
        
    }
    
    func setUpPlayers () {
        if numPlayers == 2 {
            makeTwoPlayers()
        }
        
        if numPlayers == 3 {
            //mmwGameScene.setViewController(self)
            makeThreePlayers()
        }
        
        if numPlayers == 4 {
            //mmwGameScene.setViewController(self)
            makeFourPlayers()
        }
    }
    
    func createPlayerInScene (player : Player) {
        //mmwGameScene.
    }
    
    func makeTwoPlayers () {
        
        //tileCollection.getNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayedTileArray, numTilesGet: 32, changeColorTo: 0 )

        //player1.playerLetterTiles = tileCollection.mmwPlayer1LetterTileArray
        //tileCollection.getAllNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer1LetterTileArray, numTilesGet: 6, changeColorTo: player1.playerColor)
        mmwGameScene.addPlayerView(1, playerView: PlayerView(mmwPlayer: player1))
        player1.setPlayerTilesGrid(&mmwGameScene.mmwPlayer1of2Grid!)
        //mmwGameScene.updateGridFromArray(tileCollection.mmwPlayer1LetterTileArray, grid: player1.playerLetterGrid)
        
        
        //player2.playerLetterTiles = tileCollection.mmwPlayer2LetterTileArray
        //tileCollection.getAllNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer2LetterTileArray, numTilesGet: 6, changeColorTo: player2.playerColor)
        mmwGameScene.addPlayerView(2, playerView: PlayerView(mmwPlayer: player2))
        player2.setPlayerTilesGrid(&mmwGameScene.mmwPlayer2of2Grid!)
        //mmwGameScene.updateGridFromArray(tileCollection.mmwPlayer2LetterTileArray, grid: player2.playerLetterGrid)
    }
    
    func makeThreePlayers () {
//        tileCollection.getNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayedTileArray, numTilesGet: 32, changeColorTo: 0 )
//        
//        player1.playerLetterTiles = tileCollection.mmwPlayer1LetterTileArray
//        tileCollection.getNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer1LetterTileArray, numTilesGet: 1, changeColorTo: player1.playerColor)
//        mmwGameScene.addPlayerView(1, playerView: PlayerView(mmwPlayer: player1))
//        
//        player2.playerLetterTiles = tileCollection.mmwPlayer2LetterTileArray
//        tileCollection.getNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer2LetterTileArray, numTilesGet: 2, changeColorTo: player2.playerColor)
//        mmwGameScene.addPlayerView(2, playerView: PlayerView(mmwPlayer: player2))
        
        makeTwoPlayers()
        
        //player3.playerLetterTiles = tileCollection.mmwPlayer3LetterTileArray
        tileCollection.getAllNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer3LetterTileArray, numTilesGet: 6, changeColorTo: player3.playerColor)
        mmwGameScene.addPlayerView(3, playerView: PlayerView(mmwPlayer: player3))
        player3.setPlayerTilesGrid(&mmwGameScene.mmwPlayer3of4Grid!)
    }
    
    func makeFourPlayers () {
//        tileCollection.getNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayedTileArray, numTilesGet: 32, changeColorTo: 0 )
//        
//        player1.playerLetterTiles = tileCollection.mmwPlayer1LetterTileArray
//        tileCollection.getNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer1LetterTileArray, numTilesGet: 1, changeColorTo: player1.playerColor)
//        mmwGameScene.addPlayerView(1, playerView: PlayerView(mmwPlayer: player1))
//        
//        player2.playerLetterTiles = tileCollection.mmwPlayer2LetterTileArray
//        tileCollection.getNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer2LetterTileArray, numTilesGet: 2, changeColorTo: player2.playerColor)
//        mmwGameScene.addPlayerView(2, playerView: PlayerView(mmwPlayer: player2))
//        
//        player3.playerLetterTiles = tileCollection.mmwPlayer3LetterTileArray
//        tileCollection.getNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer3LetterTileArray, numTilesGet: 3, changeColorTo: player3.playerColor)
//        mmwGameScene.addPlayerView(3, playerView: PlayerView(mmwPlayer: player3))
        
        makeThreePlayers()
        
        //player4.playerLetterTiles = tileCollection.mmwPlayer4LetterTileArray
        tileCollection.getAllNewTiles(&tileCollection.mmwTileArray, tilesTo: &tileCollection.mmwPlayer4LetterTileArray, numTilesGet: 6, changeColorTo: player4.playerColor)
        mmwGameScene.addPlayerView(4, playerView: PlayerView(mmwPlayer: player4))
        player4.setPlayerTilesGrid(&mmwGameScene.mmwPlayer4of4Grid!)
    }

//    func presentMMWScene() {
//
//        let transition = SKTransition.crossFadeWithDuration(2.0)
//        let MMWScene = MMWGameScene(size: view.bounds.size)
//        MMWScene.name = "MMWScene instance name made in GameScene"
//        //var currentScene = MMWScene
//        mmwGameScene.presentMenuScene()
//        //scene.presentScene(MMWScene, transition: transition)
//        print("presentMMWScene")
//        MMWScene.player1View?.playerNameLabel.text = "Fred"
//        
//        //MMWScene.player1View?.playerNameLabel.text = MMWScene.player1!.playerName
//        //MMWScene.changePlayerScore(MMWScene.player1View!, player: MMWScene.player1!, score: 22)
//        MMWScene.player1View?.playerScoreLabel.text = String(33)
//    }
//    
//    func presentMMWScene() {
//        
//        print("in GameViewController presentMMWScene 1")
//        let mmwScene = MMWGameScene(size: view.bounds.size)
//        
//        // Configure the view.
//        let skView = self.view as! SKView
//        skView.showsFPS = true
//        skView.showsNodeCount = true
//        
//        /* Sprite Kit applies additional optimizations to improve rendering performance */
//        skView.ignoresSiblingOrder = true
//        
//        /* Set the scale mode to scale to fit the window */
//        mmwScene.scaleMode = .AspectFill
//        
//        skView.presentScene(mmwScene)
//        print("in GameViewController presentMMWScene 2")
//        
//        //MMWGameSceneViewController()
//        //presentMMWScene()
//        print("in GameViewController presentMMWScene 3")
//        
//        mmwScene.changePlayerScore(mmwScene.player2View!, player: mmwScene.player2!, score: 2468)
//    }

    //func fillTiles(inout tilesFrom: [MMWTile], inout tilesTo: [MMWTile], changeColorTo: UIColor) {
//        let originalTilesGet = numTilesGet
//        while numTilesGet > 0 {
//            let numTiles : UInt32 = UInt32(tilesFrom.count - 1)
//            let tileInArr = arc4random_uniform( numTiles ) // select random tile in FROM array
//            let tileRemoved : MMWTile = tilesFrom.removeAtIndex( Int(tileInArr) )
//            tileRemoved.tileSprite.color = changeColorTo
//            tilesTo.insert(tileRemoved, atIndex: originalTilesGet - numTilesGet)
//            //tilesTo.append( tileRemoved )
//            numTilesGet--
//        }
//    }

//    override func didReceiveMemoryWarning() {
//        super.didReceiveMemoryWarning()
//        // Release any cached data, images, etc that aren't in use.
//    }
}




