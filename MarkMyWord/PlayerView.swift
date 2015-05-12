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
    

    //var playerNameLabel : SKLabelNode
    
    init (_playerName: String, _playerColor: UIColor) {
        
        var playerBGTallTex : SKTexture = SKTexture(imageNamed: "PlayerBGTEST309x1320TestFull.jpg")
        var playerBGTex : SKTexture = SKTexture(imageNamed: "PlayerBGTest309x660")
        var playerTileGridTex : SKTexture = SKTexture(imageNamed: "MMWPlayerTilesGrid")
        var playerTilesPlaqueTex : SKTexture = SKTexture(imageNamed: "MMWPlayerTilesPlaque")

        var playerColor : UIColor = _playerColor
        
        var playerBGTall : SKSpriteNode = SKSpriteNode(texture: playerBGTallTex, color: SKColor.whiteColor(), size: CGSizeMake(playerBGTallTex.size().width/2, playerBGTallTex.size().height/2) )
        var playerTileGrid : SKSpriteNode = SKSpriteNode(texture: playerTileGridTex, color: playerColor, size: CGSizeMake(playerTileGridTex.size().width/2, playerTileGridTex.size().height/2) )
        var playerTilesPlaque : SKSpriteNode = SKSpriteNode(texture: playerTilesPlaqueTex, color: playerColor, size: CGSizeMake(playerTilesPlaqueTex.size().width/2, playerTilesPlaqueTex.size().height/2) )
        let playerName : String = _playerName
        var playerScore : Int = 0
        
        
        //self.playerName = _playerName
        //self.playerNameLabel = SKLabelNode(text: playerName)
        super.init(texture: playerBGTex, color: nil, size: CGSizeMake(playerBGTex.size().width/2, playerBGTex.size().height/2) )  // (309/2, 1319/2) )
        self.anchorPoint = CGPointMake(0, 0)
        self.alpha = (CGFloat(1.0))
        
        //let playerColor : UIColor = UIColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 1.00)
        //var MMWPlayerTilesGridTex = texture(
        //playerTileGrid = SKSpriteNode(texture: playerTileGridTex, color: playerColor, size: CGSizeMake(playerTileGridTex.size().width, playerTileGridTex.size().height) )
        //
        //(imageNamed: "MMWPlayerTilesGrid",)
        //SKSpriteNode(texture: shadow, color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSizeMake(50.0, 50.0))
        
        playerBGTall.anchorPoint = CGPoint(x: 0.0, y: 0.0)
        playerBGTall.position = CGPoint(x: 0.0, y: -185.0 )
        playerBGTall.userInteractionEnabled = false
        playerBGTall.colorBlendFactor = CGFloat(0.1)
        playerBGTall.alpha = 1.0
        addChild(playerBGTall)
        
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
        
        let playerNameLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
        playerNameLabel.text = _playerName
        playerNameLabel.fontSize = 17
        playerNameLabel.fontColor =  FontHUDBlack
        playerNameLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: 169 ) // CGRectGetMidY(self.frame) + 10 ) // 170)
        addChild(playerNameLabel)
        
        let playerScoreLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
        playerScoreLabel.text = String(playerScore)
        playerScoreLabel.fontSize = 17
        playerScoreLabel.fontColor =  FontHUDBlack
        playerScoreLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: 153 )// CGRectGetMidY(self.frame) - 6 ) // 154)
        addChild(playerScoreLabel)

    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
//    func placePlayerName () {
//        playerName = SKLabelNode(text: "Player1Node")
//    }
}