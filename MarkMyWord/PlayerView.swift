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
    
    //var mmwGameSceneViewController : MMWGameSceneViewController!
    //var mmwGameScene : MMWGameScene!
    
    //        let playerBGTallTex : SKTexture = SKTexture(imageNamed: "PlayerBGTEST309x1320TestFull.jpg")
    //let playerBGTex          = SKTexture(imageNamed: "PlayerBG")
    let playerBGTex          = SKTexture(imageNamed: "PlayerBG.png")
    let playerTileGridTex    = SKTexture(imageNamed: "MMWPlayerTilesGrid")
    
    let playerTilesPlaqueTex = SKTexture(imageNamed: "MMWPlayerTilesPlaque")

    var playerTex01          = SKTexture(imageNamed: "AvatarShadowMale.png")
    var playerTex02          = SKTexture(imageNamed: "BongoSized.png")
    var playerTex03          = SKTexture(imageNamed: "orangePlayer.png")
    var playerTex04          = SKTexture(imageNamed: "greenPlayer.png")

    var playerTex            : SKTexture!
    
    let tileCoverTex         = SKTexture(imageNamed: "tileCover.png")
    let playerGridGlowTex    = SKTexture(imageNamed: "playerGridGlowTex.png")
    
    //var playerGridGlowTex    : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "Tile3D100xGLOW"), color: UIColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSizeMake(50.0, 50.0))

    let playerColor       : UIColor!
    let playerSprite      : SKSpriteNode!
    let playerTileGrid    : SKSpriteNode!
    let playerTilesPlaque : SKSpriteNode!
    
    let playerGridGlow    : SKSpriteNode!
    let playerTileCover   : SKSpriteNode!
    let playerTileBacking   : SKSpriteNode!
    
    //let playerBG : SKSpriteNode
    //        let playerBGTall : SKSpriteNode = SKSpriteNode(texture: playerBGTallTex, color: SKColor.whiteColor(), size: CGSizeMake(playerBGTallTex.size().width/2, playerBGTallTex.size().height/2) )
    //        let playerBG : SKSpriteNode = SKSpriteNode(texture: playerBGTex, color: SKColor.whiteColor(), size: CGSizeMake(playerBGTex.size().width/2, playerBGTex.size().height/2) )

    let playerScore : Int = 0
    
    init (mmwPlayer: Player) {
        
        self.mmwPlayer = mmwPlayer
        
        let playerNumber = mmwPlayer.playerID
        
        if playerNumber == 1 {
            playerTex = SKTexture(imageNamed: "AvatarShadowMale.png")
        }
        else if playerNumber == 2 {
            playerTex = SKTexture(imageNamed: "BongoSized.png")
        }
        else if playerNumber == 3 {
            playerTex = SKTexture(imageNamed: "BongoSized.png")
        }
        else if playerNumber == 4 {
            playerTex = SKTexture(imageNamed: "BongoSized.png")
        }
        else {
            playerTex = SKTexture(imageNamed: "AvatarShadowMale.png")
        }
        //let playerTexName = ( "player0\(playerNumber).png" )
        //playerTex = SKTexture(imageNamed: playerTexName)
        
        playerColor       =  gameColors[mmwPlayer.playerColor]
        
        //playerSprite      = SKSpriteNode(texture: playerTex, color: playerColor, size: CGSizeMake(playerTex.size().width, playerTex.size().height) )
        playerSprite      = SKSpriteNode(texture: playerTex, color: playerColor, size: CGSizeMake(100, 100) )
        playerSprite.colorBlendFactor = 1.0
        
        playerTileGrid    = SKSpriteNode(texture: playerTileGridTex, color: playerColor, size: CGSizeMake(playerTileGridTex.size().width, playerTileGridTex.size().height) )
        playerTilesPlaque = SKSpriteNode(texture: playerTilesPlaqueTex, color: playerColor, size: CGSizeMake(playerTilesPlaqueTex.size().width, playerTilesPlaqueTex.size().height) )
        playerTileCover  = SKSpriteNode(texture: tileCoverTex,   color: UIColorGray, size: CGSizeMake( ( (tileCoverTex.size().width) + 2), ((tileCoverTex.size().height) + 2) ) )
        playerTileBacking  = SKSpriteNode(texture: tileCoverTex,   color: UIColorGray, size: CGSizeMake( ( (tileCoverTex.size().width) + 2), ((tileCoverTex.size().height) + 2) ) )
        
        playerGridGlow   = SKSpriteNode (texture: playerGridGlowTex, color: SKColor.whiteColor(), size: CGSizeMake( ( (playerGridGlowTex.size().width)), ((playerGridGlowTex.size().height)) ) )

        
        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            super.init(texture: playerBGTex, color: UIColorAppleBlue, size: CGSizeMake( playerBGTex.size().width * 1.33 , playerBGTex.size().height * 1.33) ) // (309/2, 1319/2) )
        }
        else{
            super.init(texture: playerBGTex, color: UIColorAppleBlue, size: CGSizeMake( playerBGTex.size().width , playerBGTex.size().height) ) // (309/2, 1319/2) )
        }

        
        
//        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
//            super.init(texture: playerTileGridTex, color: UIColorAppleBlue, size: CGSizeMake( playerBGTex.size().width * 1.33 , playerBGTex.size().height * 1.33) ) // (309/2, 1319/2) )
//        }
//        else{
//            super.init(texture: playerTileGridTex, color: UIColorAppleBlue, size: CGSizeMake( playerBGTex.size().width , playerBGTex.size().height) ) // (309/2, 1319/2) )
//        }
        
        self.zPosition = -90
        self.anchorPoint = CGPointMake(0, 0)
        
        let playerGraphics = [playerSprite, playerTileGrid, playerTilesPlaque, playerTileCover, playerGridGlow]
        
        for graphic in playerGraphics {
            if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
                graphic.size.width *= 1.33
                graphic.size.height *= 1.33
            }
        }
        
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
        
        //playerTex = SKTexture(imageNamed: "orangePlayer.png")
        
        playerSprite.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerSprite.position = CGPoint(x: size.width/2.0, y: self.frame.height * 0.58)  // 337.0)
        playerSprite.userInteractionEnabled = false
        //yellowPlayerSprite.colorBlendFactor = CGFloat(1.0)
        playerSprite.alpha = 1.0
        playerSprite.zPosition = -1
        addChild(playerSprite)

        playerNameLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
        playerNameLabel.text = mmwPlayer.playerName
        playerNameLabel.fontSize = 17
        playerNameLabel.fontColor =  FontHUDBlack
        playerNameLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: self.frame.height * 0.531) // CGRectGetMidY(self.frame) + 10 ) // 171)
        playerNameLabel.zPosition = 3
        addChild(playerNameLabel)
        
        playerScoreLabel = SKLabelNode(fontNamed: FontHUDName) // SKLabelNode(fontNamed:"Chalkduster")
        playerScoreLabel.text = String(playerScore)
        playerScoreLabel.fontSize = 17
        playerScoreLabel.fontColor =  FontHUDBlack
        playerScoreLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: self.frame.height * 0.48 ) // CGRectGetMidY(self.frame) - 6 ) // 154)
        playerScoreLabel.zPosition = 3
        addChild(playerScoreLabel)

        playerTileGrid.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerTileGrid.position = CGPoint(x: size.width/2.0, y: screenHeight*0.071)  // 337.0)  // y 7
        playerTileGrid.userInteractionEnabled = false
        playerTileGrid.colorBlendFactor = CGFloat(1.0)
        playerTileGrid.alpha = 1.0
        playerTileGrid.zPosition = 0
        addChild(playerTileGrid)

        playerTilesPlaque.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerTilesPlaque.position = CGPoint(x: size.width/2.0, y: self.frame.height * 0.469) // 482.0)  y 152
        playerTilesPlaque.userInteractionEnabled = false
        playerTilesPlaque.colorBlendFactor = CGFloat(1.0)
        playerTilesPlaque.alpha = 1.0
        playerTilesPlaque.zPosition = 2
        addChild(playerTilesPlaque)
        
        playerTileCover.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerTileCover.position = CGPoint(x: size.width/2.0 , y: screenHeight*0.070 ) // 482.0)
        playerTileCover.userInteractionEnabled = false
        playerTileCover.colorBlendFactor = CGFloat(1.0)
        playerTileCover.size.height = playerTileCover.size.height * 0.75
        playerTileCover.size.width = playerTileCover.size.width + 2
        playerTileCover.alpha = 0.6
        playerTileCover.zPosition = 10
        addChild(playerTileCover)
        
        playerTileBacking.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        playerTileBacking.position = CGPoint(x: size.width/2.0, y: screenHeight*0.071 ) // 482.0)
        playerTileBacking.userInteractionEnabled = false
        playerTileBacking.colorBlendFactor = CGFloat(1.0)
        playerTileBacking.size.height = playerTileCover.size.height * 0.75
        playerTileBacking.alpha = 1.0
        playerTileBacking.zPosition = -5
        addChild(playerTileBacking)
        
//        playerGridGlow.anchorPoint = CGPoint(x: 0.0, y: 0.0)
//        playerGridGlow.position = CGPoint(x: 0, y: 0) // 482.0)
//        playerGridGlow.userInteractionEnabled = false
//        playerGridGlow.colorBlendFactor = CGFloat(1.0)
//        playerGridGlow.alpha = 0.9
//        playerGridGlow.zPosition = 2
//        playerGridGlow.hidden = true
//        addChild(playerGridGlow)
    }
    
    
//    func setPlayerGameSceneAndController (mmwGameScene: MMWGameScene, mmwGameSceneViewController: MMWGameSceneViewController) {
//        //self.mmwGameScene = mmwGameScene
//        //self.mmwGameSceneViewController = mmwGameSceneViewController
//        
//        //self.xScale = (mmwGame.screenSize!.width)/1024
//        self.xScale = (screenSize!.height)/768
//        self.yScale = (screenSize!.height)/768
//    }
    
    
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
        self.playerTileCover.hidden = true
        self.playerGridGlow.hidden = false
        mmwGameScene.bottomDisplayLabel.text =  ("\(mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerName ), place letters")
        
        let playerGrid = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid!
        playerGrid.makeTilesInGridInteractive(true)
    }
    
    
    func playerViewEndTurn () {
        self.playerNameLabel.fontColor =  FontHUDBlack
        self.playerScoreLabel.fontColor = FontHUDBlack
        //changePlayerScoreDisplay()
        self.playerTileCover.hidden = false
        self.playerGridGlow.hidden = true
        mmwGameScene.topDisplayLabel.text = ("Great Job, \(self.playerNameLabel.text)!!!")
        mmwGameScene.topDisplayLabel2.text = ("You rock!!!")
        mmwGameScene.bottomDisplayLabel.text = ("Word Feedback ...")
        
        let playerGrid = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid!  // .grid2DArr
        playerGrid.makeTilesInGridInteractive(false)
    }
    
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}