//
//  PlayerView.swift
//  MarkMyWord
//
//  Created by Eric Hook on 5/8/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

class PlayerView: SKSpriteNode {
    
    var playerNameLabel = SKLabelNode(fontNamed: FontHUDName)
    var playerScoreLabel = SKLabelNode(fontNamed: FontHUDName)
    var mmwPlayer : Player!
    
    var mmwGameSceneViewController : MMWGameSceneViewController!
    var mmwGameScene : MMWGameScene!
    
    init (mmwPlayer: Player) {
        
//        let playerBGTallTex : SKTexture = SKTexture(imageNamed: "PlayerBGTEST309x1320TestFull.jpg")
        let playerBGTex : SKTexture = SKTexture(imageNamed: "PlayerBG")
        let playerTileGridTex : SKTexture = SKTexture(imageNamed: "MMWPlayerTilesGrid")
        let playerTilesPlaqueTex : SKTexture = SKTexture(imageNamed: "MMWPlayerTilesPlaque")
        let yellowPlayerTex : SKTexture = SKTexture(imageNamed: "yellowPlayer.png")
        self.mmwPlayer = mmwPlayer
        let playerColor : UIColor =   gameColors[mmwPlayer.playerColor]
        
//        let playerBGTall : SKSpriteNode = SKSpriteNode(texture: playerBGTallTex, color: SKColor.whiteColor(), size: CGSizeMake(playerBGTallTex.size().width/2, playerBGTallTex.size().height/2) )
//        let playerBG : SKSpriteNode = SKSpriteNode(texture: playerBGTex, color: SKColor.whiteColor(), size: CGSizeMake(playerBGTex.size().width/2, playerBGTex.size().height/2) )
        let yellowPlayerSprite : SKSpriteNode = SKSpriteNode(texture: yellowPlayerTex, color: playerColor, size: CGSizeMake(yellowPlayerTex.size().width/2, yellowPlayerTex.size().height/2) )
        let playerTileGrid : SKSpriteNode = SKSpriteNode(texture: playerTileGridTex, color: playerColor, size: CGSizeMake(playerTileGridTex.size().width/2, playerTileGridTex.size().height/2) )
        let playerTilesPlaque : SKSpriteNode = SKSpriteNode(texture: playerTilesPlaqueTex, color: playerColor, size: CGSizeMake(playerTilesPlaqueTex.size().width/2, playerTilesPlaqueTex.size().height/2) )
        let playerScore : Int = 0
        
        super.init(texture: playerBGTex, color: UIColorAppleBlue, size: CGSizeMake(playerBGTex.size().width/2, playerBGTex.size().height/2) )  // (309/2, 1319/2) )
        
        self.anchorPoint = CGPointMake(0, 0)
        //self.zPosition = -111
//        playerBGTall.anchorPoint = CGPoint(x: 0.0, y: 0.0)
//        playerBGTall.position = CGPoint(x: 0.0, y: -185.0 )
//        playerBGTall.userInteractionEnabled = false
//        playerBGTall.colorBlendFactor = CGFloat(0.1)
//        playerBGTall.alpha = 1.0
        //addChild(playerBGTall)
        
//        playerBG.anchorPoint = CGPoint(x: 0.0, y: 0.0)
//        playerBG.position = CGPoint(x: 0.0, y: -185.0 )
//        playerBG.userInteractionEnabled = false
//        playerBG.colorBlendFactor = CGFloat(0.1)
//        playerBG.alpha = 1.0
//        addChild(playerBG)
        
        yellowPlayerSprite.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        yellowPlayerSprite.position = CGPoint(x: size.width/2.0, y: 150.0)  // 337.0)
        yellowPlayerSprite.userInteractionEnabled = false
        //yellowPlayerSprite.colorBlendFactor = CGFloat(1.0)
        yellowPlayerSprite.alpha = 1.0
        yellowPlayerSprite.zPosition = 1
        addChild(yellowPlayerSprite)

        playerNameLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
        playerNameLabel.text = mmwPlayer.playerName
        playerNameLabel.fontSize = 17
        playerNameLabel.fontColor =  FontHUDBlack
        playerNameLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: self.frame.height * 0.517) // CGRectGetMidY(self.frame) + 10 ) // 171)
        playerNameLabel.zPosition = 2
        addChild(playerNameLabel)
        
        playerScoreLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
        playerScoreLabel.text = String(playerScore)
        playerScoreLabel.fontSize = 17
        playerScoreLabel.fontColor =  FontHUDBlack
        playerScoreLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: self.frame.height * 0.467 ) // CGRectGetMidY(self.frame) - 6 ) // 154)
        playerScoreLabel.zPosition = 2
        addChild(playerScoreLabel)

        playerTileGrid.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerTileGrid.position = CGPoint(x: size.width/2.0, y: 7.0)  // 337.0)
        playerTileGrid.userInteractionEnabled = false
        playerTileGrid.colorBlendFactor = CGFloat(1.0)
        playerTileGrid.alpha = 0.75
        playerTileGrid.zPosition = 1
        addChild(playerTileGrid)

        playerTilesPlaque.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerTilesPlaque.position = CGPoint(x: size.width/2.0, y: 152.0) // 482.0)
        playerTilesPlaque.userInteractionEnabled = false
        playerTilesPlaque.colorBlendFactor = CGFloat(1.0)
        playerTilesPlaque.alpha = 0.75
        playerTilesPlaque.zPosition = 1
        addChild(playerTilesPlaque)

    }
    
    func setPlayerGameSceneAndController (mmwGameScene: MMWGameScene, mmwGameSceneViewController: MMWGameSceneViewController) {
        self.mmwGameScene = mmwGameScene
        self.mmwGameSceneViewController = mmwGameSceneViewController
    }
    
    func changePlayerScoreDisplay () {
        let score = mmwPlayer.playerScore
        self.playerScoreLabel.text = String(score)
    }
    
    func changePlayerScoreDisplay (score: Int) {
        mmwPlayer.playerScore = score
        self.playerScoreLabel.text = String(score)
    }
    
    func playerViewBeginTurn () {
        self.playerNameLabel.fontColor = UIColor.whiteColor()
        self.playerScoreLabel.fontColor = UIColor.whiteColor()
        mmwGameScene.bottomDisplayLabel.text =  ("\(mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerName ), place letters")
        let playerGrid = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid!  // .grid2DArr
        playerGrid.makeTilesInGridInteractive(true)
//        for tileColumn in playerGrid {
//            for tile in tileColumn {
//                tile.tileSprite.userInteractionEnabled = true
//                tile.tileSprite.isMovable = true
//            }
//        }
    }
    
    func playerViewEndTurn () {
        self.playerNameLabel.fontColor =  FontHUDBlack
        self.playerScoreLabel.fontColor = FontHUDBlack
        changePlayerScoreDisplay()
        
        mmwGameScene.topDisplayLabel.text = ("Great Job, \(self.playerNameLabel.text)!!!")
        mmwGameScene.topDisplayLabel2.text = ("You rock!!!")
        mmwGameScene.bottomDisplayLabel.text = ("Word Feedback ...")
        
        let playerGrid = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid!  // .grid2DArr
        playerGrid.makeTilesInGridInteractive(false)
//        for tileColumn in playerGrid {
//            for tile in tileColumn {
//                tile.tileSprite.userInteractionEnabled = false
//                tile.tileSprite.isMovable = false
//            }
//        }   
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}