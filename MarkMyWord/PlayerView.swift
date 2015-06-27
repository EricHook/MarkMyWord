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
    
    init (mmwPlayer: MMWPlayer) {
        
//        let playerBGTallTex : SKTexture = SKTexture(imageNamed: "PlayerBGTEST309x1320TestFull.jpg")
        let playerBGTex : SKTexture = SKTexture(imageNamed: "PlayerBGTest309x660")
        let playerTileGridTex : SKTexture = SKTexture(imageNamed: "MMWPlayerTilesGrid")
        let playerTilesPlaqueTex : SKTexture = SKTexture(imageNamed: "MMWPlayerTilesPlaque")

        let playerColor : UIColor = mmwPlayer.playerColor
        
//        let playerBGTall : SKSpriteNode = SKSpriteNode(texture: playerBGTallTex, color: SKColor.whiteColor(), size: CGSizeMake(playerBGTallTex.size().width/2, playerBGTallTex.size().height/2) )
//        let playerBG : SKSpriteNode = SKSpriteNode(texture: playerBGTex, color: SKColor.whiteColor(), size: CGSizeMake(playerBGTex.size().width/2, playerBGTex.size().height/2) )
        let playerTileGrid : SKSpriteNode = SKSpriteNode(texture: playerTileGridTex, color: playerColor, size: CGSizeMake(playerTileGridTex.size().width/2, playerTileGridTex.size().height/2) )
        let playerTilesPlaque : SKSpriteNode = SKSpriteNode(texture: playerTilesPlaqueTex, color: playerColor, size: CGSizeMake(playerTilesPlaqueTex.size().width/2, playerTilesPlaqueTex.size().height/2) )
        let playerScore : Int = 0
        
        super.init(texture: playerBGTex, color: UIColorAppleBlue, size: CGSizeMake(playerBGTex.size().width/2, playerBGTex.size().height/2) )  // (309/2, 1319/2) )
        
        self.anchorPoint = CGPointMake(0, 0)

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
        
        playerTileGrid.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerTileGrid.position = CGPoint(x: size.width/2.0, y: 7.0)  // 337.0)
        playerTileGrid.userInteractionEnabled = false
        playerTileGrid.colorBlendFactor = CGFloat(1.0)
        playerTileGrid.alpha = 0.75
        addChild(playerTileGrid)
        
        playerTilesPlaque.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerTilesPlaque.position = CGPoint(x: size.width/2.0, y: 152.0) // 482.0)
        playerTilesPlaque.userInteractionEnabled = false
        playerTilesPlaque.colorBlendFactor = CGFloat(1.0)
        playerTileGrid.alpha = 0.75
        addChild(playerTilesPlaque)
        
        playerNameLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
        playerNameLabel.text = mmwPlayer.playerName
        playerNameLabel.fontSize = 17
        playerNameLabel.fontColor =  FontHUDBlack
        playerNameLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: self.frame.height * 0.517) // CGRectGetMidY(self.frame) + 10 ) // 171)
        addChild(playerNameLabel)
        
        playerScoreLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
        playerScoreLabel.text = String(playerScore)
        playerScoreLabel.fontSize = 17
        playerScoreLabel.fontColor =  FontHUDBlack
        playerScoreLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: self.frame.height * 0.467 ) // CGRectGetMidY(self.frame) - 6 ) // 154)
        addChild(playerScoreLabel)
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}