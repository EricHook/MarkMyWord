//
//  OptionScreen.swift
//  MarkMyWord
//
//  Created by Eric Hook on 1/3/16.
//  Copyright © 2016 Hook Studios. All rights reserved.
//

import UIKit

import Foundation
import SpriteKit

class OptionScreen: SKSpriteNode {
    
    let gameGridTex = SKTexture(imageNamed: "GameGrid.png")
    let OptButtonWordLength2Tex = SKTexture(imageNamed: "OptButtonWordLength2.png")
    let OptButtonWordLength2BoxedTex = SKTexture(imageNamed: "OptButtonWordLength2Boxed.png")
    let OptButtonWordLength3Tex = SKTexture(imageNamed: "OptButtonWordLength3.png")
    let OptButtonWordLength3BoxedTex = SKTexture(imageNamed: "OptButtonWordLength3Boxed.png")

    required init?(coder aDecoder: NSCoder) {
        
        fatalError("init(coder:) has not been implemented")
    }
    
    init () {

        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            super.init(texture: gameGridTex, color: UIColorAppleBlue, size: CGSizeMake( gameGridTex.size().width * 1.33 , gameGridTex.size().height * 1.33) ) // (309/2, 1319/2) )
        }
        else {
            super.init(texture: gameGridTex, color: UIColorAppleBlue, size: CGSizeMake( gameGridTex.size().width , gameGridTex.size().height) ) // (309/2, 1319/2) )
        }

//        self.zPosition = -100
//        self.anchorPoint = CGPointMake(0, 0)
        
        let gameGridNode = SKSpriteNode(texture: gameGridTex, color: SKColor.whiteColor(), size: CGSizeMake(gameGridTex.size().width/2, gameGridTex.size().height/2) )
        let OptButtonWordLength2Node = SKSpriteNode(texture: OptButtonWordLength2Tex, color: SKColor.whiteColor(), size: CGSizeMake(OptButtonWordLength2Tex.size().width/2, OptButtonWordLength2Tex.size().height/2) )
       let OptButtonWordLength2BoxedNode = SKSpriteNode(texture: OptButtonWordLength2BoxedTex, color: SKColor.whiteColor(), size: CGSizeMake(OptButtonWordLength2BoxedTex.size().width/2, OptButtonWordLength2Tex.size().height/2) )
       let OptButtonWordLength3Node = SKSpriteNode(texture: OptButtonWordLength3Tex, color: SKColor.whiteColor(), size: CGSizeMake(OptButtonWordLength3Tex.size().width/2, OptButtonWordLength2Tex.size().height/2) )
       let OptButtonWordLength3BoxedNode = SKSpriteNode(texture: OptButtonWordLength3BoxedTex, color: SKColor.whiteColor(), size: CGSizeMake(OptButtonWordLength3BoxedTex.size().width/2, OptButtonWordLength3BoxedTex.size().height/2) )
        
        OptButtonWordLength2Node.anchorPoint = CGPoint(x: 0.0, y: 0.0)
        OptButtonWordLength2Node.position = CGPoint(x: OptButtonWordLength2Tex.size().width/4, y: 0) // 482.0)
        OptButtonWordLength2Node.userInteractionEnabled = false
        OptButtonWordLength2Node.colorBlendFactor = CGFloat(1.0)
        OptButtonWordLength2Node.alpha = 0.9
        OptButtonWordLength2Node.zPosition = 2
        OptButtonWordLength2Node.hidden = false
        addChild(OptButtonWordLength2Node)
        
        OptButtonWordLength2BoxedNode.anchorPoint = CGPoint(x: 0.0, y: 0.0)
        OptButtonWordLength2BoxedNode.position = CGPoint(x: OptButtonWordLength2BoxedTex.size().width/4, y: 0) // 482.0)
        OptButtonWordLength2BoxedNode.userInteractionEnabled = false
        OptButtonWordLength2BoxedNode.colorBlendFactor = CGFloat(1.0)
        OptButtonWordLength2BoxedNode.alpha = 0.9
        OptButtonWordLength2BoxedNode.zPosition = 2
        OptButtonWordLength2BoxedNode.hidden = false
        addChild(OptButtonWordLength2BoxedNode)
        
        OptButtonWordLength3Node.anchorPoint = CGPoint(x: 0.0, y: 0.0)
        OptButtonWordLength3Node.position = CGPoint(x: OptButtonWordLength3Tex.size().width/2 + 45, y: 0) // 482.0)
        OptButtonWordLength3Node.userInteractionEnabled = false
        OptButtonWordLength3Node.colorBlendFactor = CGFloat(1.0)
        OptButtonWordLength3Node.alpha = 0.9
        OptButtonWordLength3Node.zPosition = 2
        OptButtonWordLength3Node.hidden = false
        addChild(OptButtonWordLength3Node)
        
        OptButtonWordLength3BoxedNode.anchorPoint = CGPoint(x: 0.0, y: 0.0)
        OptButtonWordLength3BoxedNode.position = CGPoint(x: OptButtonWordLength3BoxedTex.size().width/2 + 45, y: 0) // 482.0)
        OptButtonWordLength3BoxedNode.userInteractionEnabled = false
        OptButtonWordLength3BoxedNode.colorBlendFactor = CGFloat(1.0)
        OptButtonWordLength3BoxedNode.alpha = 0.9
        OptButtonWordLength3BoxedNode.zPosition = 2
        OptButtonWordLength3BoxedNode.hidden = false
        addChild(OptButtonWordLength3BoxedNode)
        
    }


//
//        let playerGraphics = [playerSprite, playerTileGrid, playerTilesPlaque, playerTileCover, playerGridGlow]
//        
//        for graphic in playerGraphics {
//            if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
//                graphic.size.width *= 1.33
//                graphic.size.height *= 1.33
//            }
//        }
//        
//        //self.zPosition = -111
//        //        playerBGTall.anchorPoint = CGPoint(x: 0.0, y: 0.0)
//        //        playerBGTall.position = CGPoint(x: 0.0, y: -185.0 )
//        //        playerBGTall.userInteractionEnabled = false
//        //        playerBGTall.colorBlendFactor = CGFloat(0.1)
//        //        playerBGTall.alpha = 1.0
//        //addChild(playerBGTall)
//        
//        //        playerBG.anchorPoint = CGPoint(x: 0.0, y: 0.0)
//        //        playerBG.position = CGPoint(x: 0.0, y: -185.0 )
//        //        playerBG.userInteractionEnabled = false
//        //        playerBG.colorBlendFactor = CGFloat(0.1)
//        //        playerBG.alpha = 1.0
//        //        addChild(playerBG)
//        
//        //playerTex = SKTexture(imageNamed: "orangePlayer.png")
//        
//        playerSprite.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        playerSprite.position = CGPoint(x: size.width/2.0, y: self.frame.height * 0.4)  // 337.0)
//        playerSprite.userInteractionEnabled = false
//        //yellowPlayerSprite.colorBlendFactor = CGFloat(1.0)
//        playerSprite.alpha = 1.0
//        playerSprite.zPosition = 1
//        addChild(playerSprite)
//        
//        playerNameLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
//        playerNameLabel.text = mmwPlayer.playerName
//        playerNameLabel.fontSize = 17
//        playerNameLabel.fontColor =  FontHUDBlack
//        playerNameLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: self.frame.height * 0.521) // CGRectGetMidY(self.frame) + 10 ) // 171)
//        playerNameLabel.zPosition = 3
//        addChild(playerNameLabel)
//        
//        playerScoreLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
//        playerScoreLabel.text = String(playerScore)
//        playerScoreLabel.fontSize = 17
//        playerScoreLabel.fontColor =  FontHUDBlack
//        playerScoreLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: self.frame.height * 0.47 ) // CGRectGetMidY(self.frame) - 6 ) // 154)
//        playerScoreLabel.zPosition = 3
//        addChild(playerScoreLabel)
//        
//        playerTileGrid.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        playerTileGrid.position = CGPoint(x: size.width/2.0, y: screenHeight*0.0091146)  // 337.0)  // y 7
//        playerTileGrid.userInteractionEnabled = false
//        playerTileGrid.colorBlendFactor = CGFloat(1.0)
//        playerTileGrid.alpha = 0.75
//        playerTileGrid.zPosition = 2
//        addChild(playerTileGrid)
//        
//        playerTilesPlaque.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        playerTilesPlaque.position = CGPoint(x: size.width/2.0, y: self.frame.height * 0.46) // 482.0)  y 152
//        playerTilesPlaque.userInteractionEnabled = false
//        playerTilesPlaque.colorBlendFactor = CGFloat(1.0)
//        playerTilesPlaque.alpha = 0.75
//        playerTilesPlaque.zPosition = 2
//        addChild(playerTilesPlaque)
//        
//        playerTileCover.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        playerTileCover.position = CGPoint(x: size.width/2.0, y: screenHeight*0.010 ) // 482.0)
//        playerTileCover.userInteractionEnabled = false
//        playerTileCover.colorBlendFactor = CGFloat(1.0)
//        playerTileCover.alpha = 0.5
//        playerTileCover.zPosition = 99
//        addChild(playerTileCover)
//        

    
}
//
//    
//    //    func setPlayerGameSceneAndController (mmwGameScene: MMWGameScene, mmwGameSceneViewController: MMWGameSceneViewController) {
//    //        //self.mmwGameScene = mmwGameScene
//    //        //self.mmwGameSceneViewController = mmwGameSceneViewController
//    //
//    //        //self.xScale = (mmwGame.screenSize!.width)/1024
//    //        self.xScale = (screenSize!.height)/768
//    //        self.yScale = (screenSize!.height)/768
//    //    }
//    
//    
//    func changePlayerScoreDisplay () {
//        let score = mmwPlayer.playerScore
//        self.playerScoreLabel.text = String(score)
//    }
//    
//    
//    func changePlayerScoreDisplay (score: Int) {
//        mmwPlayer.playerScore = score
//        self.playerScoreLabel.text = String(score)
//    }
//    
//    
//    func playerViewBeginTurn () {
//        self.playerNameLabel.fontColor = UIColor.whiteColor()
//        self.playerScoreLabel.fontColor = UIColor.whiteColor()
//        self.playerTileCover.hidden = true
//        self.playerGridGlow.hidden = false
//        mmwGameScene.bottomDisplayLabel.text =  ("\(mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerName ), place letters")
//        
//        let playerGrid = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid!
//        playerGrid.makeTilesInGridInteractive(true)
//    }
//    
//    
//    func playerViewEndTurn () {
//        self.playerNameLabel.fontColor =  FontHUDBlack
//        self.playerScoreLabel.fontColor = FontHUDBlack
//        //changePlayerScoreDisplay()
//        self.playerTileCover.hidden = false
//        self.playerGridGlow.hidden = true
//        mmwGameScene.topDisplayLabel.text = ("Great Job, \(self.playerNameLabel.text)!!!")
//        mmwGameScene.topDisplayLabel2.text = ("You rock!!!")
//        mmwGameScene.bottomDisplayLabel.text = ("Word Feedback ...")
//        
//        let playerGrid = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid!  // .grid2DArr
//        playerGrid.makeTilesInGridInteractive(false)
//    }
