//
//  GameScene.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/10/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit
import UIKit
import CoreMotion

class MMWGameScene: SKScene, SKPhysicsContactDelegate {
    
    var mmwGameSceneViewController : MMWGameSceneViewController!

    var viewSize : CGSize!
    var backgroundNode : SKSpriteNode = SKSpriteNode( imageNamed: "MMWBG" )
    var foregroundNode : SKSpriteNode = SKSpriteNode()
    
    var mmwBoardGrid: Grid!
    
    var mmwPlayer1of2Grid: Grid!
    var mmwPlayer2of2Grid: Grid!
    var mmwPlayer1of4Grid: Grid!
    var mmwPlayer2of4Grid: Grid!
    var mmwPlayer3of4Grid: Grid!
    var mmwPlayer4of4Grid: Grid!
    
    var mmwUnplayedGrid: Grid!
//
//    var player1 : Player?
//    var player2 : Player?
//    var player3 : Player?
//    var player4 : Player?
    
    var player1View : PlayerView?
    var player2View : PlayerView?
    var player3View : PlayerView?
    var player4View : PlayerView?

    var partialWordLabel    = SKLabelNode(fontNamed: FontHUDName)
    var timeRemainingLabel  = SKLabelNode(fontNamed: FontHUDName)
    private var secondsLeft: Int = 17
    var tilesRemainingLabel = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel     = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel2    = SKLabelNode(fontNamed: FontHUDName)
    
    
    let playButton = SKSpriteNode(imageNamed: "PlayButton.png")
    let newTilesButton = SKSpriteNode(imageNamed: "NewTilesButton.png")
    let passButton = SKSpriteNode(imageNamed: "PassButton.png")
    var pauseButton = SKSpriteNode(imageNamed: "PauseButton.png")
    var placeholderTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    
    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)

    private var timer: NSTimer?
    private var isPaused: Bool = false

    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        viewSize = size
    }
    
    func setViewController (mmwGameSceneController: MMWGameSceneViewController) {
        self.mmwGameSceneViewController = mmwGameSceneController
//        self.player1 = mmwGameSceneViewController.player1
//        self.player2 = mmwGameSceneViewController.player2
//        self.player3 = mmwGameSceneViewController.player3
//        self.player4 = mmwGameSceneViewController.player4
    }
    
    func setGrids () {
        self.mmwBoardGrid = Grid(gridUpperLeftX: 156, gridUpperLeftY: (67), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15, gridName: "mmwBoardGrid") // 21
        
        self.mmwPlayer1of2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1of2Grid") //y 304
        self.mmwPlayer1of2Grid.setGridPlayer(mmwGameSceneViewController.player1)
        
        self.mmwPlayer2of2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2of2Grid")
        self.mmwPlayer2of2Grid.setGridPlayer(mmwGameSceneViewController.player2)
        
        self.mmwPlayer1of4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1of4Grid") // 0.4583
        self.mmwPlayer1of4Grid.setGridPlayer(mmwGameSceneViewController.player1)
        
        self.mmwPlayer2of4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2of4Grid")
        self.mmwPlayer2of4Grid.setGridPlayer(mmwGameSceneViewController.player2)
        
        self.mmwPlayer3of4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer3of4Grid")
        self.mmwPlayer3of4Grid.setGridPlayer(mmwGameSceneViewController.player3)
        
        self.mmwPlayer4of4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer4of4Grid")
        self.mmwPlayer4of4Grid.setGridPlayer(mmwGameSceneViewController.player4)
    }

    func changePlayerScoreDisplay (playerView: PlayerView, player: Player, score: Int) {
        player.playerScore = score
        playerView.playerScoreLabel.text = String(score)
    }
    
    func buildGameView () {
        print("buildGameView() in MMWGameScene")
        backgroundColor = SKColor(red: 0.5, green: 0.0, blue: 0.0, alpha: 1.0)
        userInteractionEnabled = true
        // add BG
        backgroundNode = SKSpriteNode( imageNamed: "MarkMyWordBGCleaniPad@2x.png" )
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: size.width/2.0, y: 0.0)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.zPosition = -100
        backgroundNode.size = viewSize;
        addChild(backgroundNode)
        
        addChild(foregroundNode)
        
        timeRemainingHUD(90)
        tilesRemainingHUD(321)
        partialWordHUD("ABCDEFG", isWord: false)  // "ABCDEFGHIJKLMNO", isWord: false)
        topDisplayHUD("Player 1 plays \"CATATONIC\" for 14 points") // ("Turn: Player 1, Special Letter Bonus In Effect, 2x Point Bonus")

        playButton.position = CGPoint(x: viewSize.width * 0.5, y: viewSize.height * 0.5)
        playButton.name = "playButton"
        self.addChild(playButton)

        newTilesButton.position = CGPoint(x: viewSize.width * 0.0751, y: viewSize.height * 0.102)
        newTilesButton.name = "newTilesButton"
        self.addChild(newTilesButton)

        passButton.position = CGPoint(x: viewSize.width * 0.0752, y: viewSize.height * 0.04)
        passButton.name = "passButton"
        self.addChild(passButton)

        pauseButton.position = CGPoint(x: viewSize.width * 0.9228, y: viewSize.height * 0.102)
        pauseButton.name = "pauseButton"
        self.addChild(pauseButton)
        
        let optionsButton = SKSpriteNode(imageNamed: "OptionsButton.png")
        optionsButton.position = CGPoint(x: viewSize.width * 0.9228, y: viewSize.height * 0.04)
        optionsButton.name = "optionsButton"
        self.addChild(optionsButton)
        
        // ADD ALL TILES TO Scene - they start as invisible
        for tile in mmwGameSceneViewController.tileCollection.mmwTileArray {
            tile.tileSprite.hidden = true
            self.addChild(tile.tileSprite)
        }

    }
    
    func showTiles (tileBuilder: MMWTileBuilder) {
        
    }
    
    func showTilesInSquares (tileBuilder: MMWTileBuilder) {
//        var location = CGPoint(x: 100, y: 100 )
//        location.y = viewSize.height - location.y
//        let snapGrid : Grid = getSnapGrid(location)!
//        
//        let gridSquareX = snapGrid.gridNumSquaresX          // snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
//        let gridSquareY = snapGrid.gridNumSquaresY          // snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
//        
//        let gridSquareXCorner = snapGrid.gridUpperLeftX     // snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerX
//        let gridSquareYCorner = snapGrid.gridUpperLeftY     // snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerY
//        
//        print("<MMWGameScene> snapGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")

        var tileArrSelectedTile = 0
        let dealTilesSound = SKAction.playSoundFileNamed("PLINK.mp3", waitForCompletion: true)
        runAction(dealTilesSound)
        //tileBuilder.mmwPlayer1LetterTileArray[2] = MMWTile(letterString: "%")  // place manual test tile

        for y in 0...1 {   // fill Player 1 letter tiles : 2 in y and 3 in x
            for x in 0...2 {
                if tileBuilder.mmwPlayer1LetterTileArray.count > tileArrSelectedTile {
                    let tileAdded : MMWTile = tileBuilder.mmwPlayer1LetterTileArray[tileArrSelectedTile]
                    tileAdded.tileSprite.hidden = false
                    if (mmwGameSceneViewController.numPlayers == 2 || mmwGameSceneViewController.numPlayers == 3) {
                        tileAdded.tileSprite.position = self.mmwPlayer1of2Grid.sendToGridSquare(self.mmwPlayer1of2Grid, squareX: x, squareY: y)
                    }
//                    if mmwGameSceneViewController.numPlayers == 3 {
//                        tileAdded.tileSprite.position = self.mmwPlayer1of2Grid.sendToGridSquare(self.mmwPlayer1of2Grid, squareX: x, squareY: y)
//                    }
                    else if mmwGameSceneViewController.numPlayers == 4 {
                        tileAdded.tileSprite.position = self.mmwPlayer1of4Grid.sendToGridSquare(self.mmwPlayer1of4Grid, squareX: x, squareY: y)
                    }
                    self.addChild(tileAdded.tileSprite)
                    tileArrSelectedTile++
                }
            }
        }
            
        tileArrSelectedTile = 0
        //let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
        runAction(dealTilesSound)
        for y in 0...1 {   // fill Player 2 letter tiles
            for x in 0...2 {
                if tileBuilder.mmwPlayer2LetterTileArray.count > tileArrSelectedTile{
                    let tileAdded : MMWTile = tileBuilder.mmwPlayer2LetterTileArray[tileArrSelectedTile]
                    tileAdded.tileSprite.hidden = false
                    if mmwGameSceneViewController.numPlayers == 2 {
                        tileAdded.tileSprite.position = self.mmwPlayer2of2Grid.sendToGridSquare(self.mmwPlayer2of2Grid, squareX: x, squareY: y)
                    }
                    if mmwGameSceneViewController.numPlayers == 3 {
                        tileAdded.tileSprite.position = self.mmwPlayer2of4Grid.sendToGridSquare(self.mmwPlayer2of4Grid, squareX: x, squareY: y)
                    }
                    else if mmwGameSceneViewController.numPlayers == 4 {
                        tileAdded.tileSprite.position = self.mmwPlayer2of4Grid.sendToGridSquare(self.mmwPlayer2of4Grid, squareX: x, squareY: y)
                    }
                    self.addChild(tileAdded.tileSprite)
                    tileArrSelectedTile++
                }
            }
        }
        
        if mmwGameSceneViewController.numPlayers == 3 {
            runAction(dealTilesSound)
            tileArrSelectedTile = 0
            for y in 0...1 {   // fill Player 3 letter tiles if 3 players (lower right)
                for x in 0...2 {
                    if tileBuilder.mmwPlayer3LetterTileArray.count > tileArrSelectedTile {
                        let tileAdded : MMWTile = tileBuilder.mmwPlayer3LetterTileArray[tileArrSelectedTile]
                        tileAdded.tileSprite.hidden = false
                        tileAdded.tileSprite.position = self.mmwPlayer4of4Grid.sendToGridSquare(self.mmwPlayer4of4Grid, squareX: x, squareY: y)
                        self.addChild(tileAdded.tileSprite)
                        tileArrSelectedTile++
                    }
                }
            }
        }

        if mmwGameSceneViewController.numPlayers == 4 {
            runAction(dealTilesSound)
            tileArrSelectedTile = 0
            for y in 0...1 {   // fill Player 3 letter tiles if 4 players (lower left)
                for x in 0...2 {
                    if tileBuilder.mmwPlayer3LetterTileArray.count > tileArrSelectedTile {
                        let tileAdded : MMWTile = tileBuilder.mmwPlayer3LetterTileArray[tileArrSelectedTile]
                        tileAdded.tileSprite.hidden = false
                        tileAdded.tileSprite.position = self.mmwPlayer3of4Grid.sendToGridSquare(self.mmwPlayer3of4Grid, squareX: x, squareY: y)
                        self.addChild(tileAdded.tileSprite)
                        tileArrSelectedTile++
                    }
                }
            }
            
            tileArrSelectedTile = 0
            runAction(dealTilesSound)
            for y in 0...1 {   // fill Player 4 letter tiles (lower right)
                for x in 0...2 {
                    if tileBuilder.mmwPlayer4LetterTileArray.count > tileArrSelectedTile {
                        let tileAdded : MMWTile = tileBuilder.mmwPlayer4LetterTileArray[tileArrSelectedTile]
                        tileAdded.tileSprite.hidden = false
                        tileAdded.tileSprite.position = self.mmwPlayer4of4Grid.sendToGridSquare(self.mmwPlayer4of4Grid, squareX: x, squareY: y)
                        self.addChild(tileAdded.tileSprite)
                        tileArrSelectedTile++
                    }
                }
            }
        }
    
        tileArrSelectedTile = 0
        runAction(dealTilesSound)
        for y in 0...14 {  // fill Board letter tiles
            for x in 0...14 {
                if tileBuilder.mmwPlayedTileArray.count > tileArrSelectedTile {
                    let tileAdded : MMWTile = tileBuilder.mmwPlayedTileArray[tileArrSelectedTile]
                    tileAdded.tileSprite.hidden = false
                    tileAdded.tileSprite.position = self.mmwBoardGrid.sendToGridSquare(self.mmwBoardGrid, squareX: x, squareY: y)
                    tileAdded.tileSprite.color = tileAdded.tileSprite.TileColors[ Int(arc4random_uniform(4)) ]
                    self.addChild(tileAdded.tileSprite)
                    tileArrSelectedTile++
                }
            }
        }
        
//        let testTile = MMWTile(letterString: "$")
//        testTile.gridX = 7
//        testTile.gridY = 7
//        testTile.tileSprite.position = mmwBoardGrid.sendToGridSquare(mmwBoardGrid, squareX: 8, squareY: 9)
//        self.addChild(testTile.tileSprite)
        
        //self.mmwBoardGrid.sendToGridSquare(mmwBoardGrid, squareX: 7, squareY: 7) // returns position
    }
    
//    func updateTilesInSquares (tileBuilder: MMWTileBuilder) {
//        var location = CGPoint(x: 100, y: 100 )
//        location.y = viewSize.height - location.y
//        let snapGrid : Grid = getSnapGrid(location)!
//        
//        let gridSquareX = snapGrid.gridNumSquaresX          // snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
//        let gridSquareY = snapGrid.gridNumSquaresY          // snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
//        
//        let gridSquareXCorner = snapGrid.gridUpperLeftX     // snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerX
//        let gridSquareYCorner = snapGrid.gridUpperLeftY     // snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerY
//        
//        print("<MMWGameScene> snapGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
//        
//        var tileArrSelectedTile = 0
//        let dealTilesSound = SKAction.playSoundFileNamed("PLINK.mp3", waitForCompletion: true)
//        runAction(dealTilesSound)
//        //tileBuilder.mmwPlayer1LetterTileArray[2] = MMWTile(letterString: "%")  // place manual test tile
//        
//        for y in 0...1 {   // fill Player 1 letter tiles
//            for x in 0...2 {
//                if tileBuilder.mmwPlayer1LetterTileArray.count > tileArrSelectedTile {
//                    let tileAdded : MMWTile = tileBuilder.mmwPlayer1LetterTileArray[tileArrSelectedTile]
//                    if mmwGameSceneViewController.numPlayers == 2 {
//                        tileAdded.tileSprite.position = self.mmwPlayer1of2Grid.sendToGridSquare(self.mmwPlayer1of2Grid, squareX: x, squareY: y)
//                    }
//                    if mmwGameSceneViewController.numPlayers == 3 {
//                        tileAdded.tileSprite.position = self.mmwPlayer1of2Grid.sendToGridSquare(self.mmwPlayer1of2Grid, squareX: x, squareY: y)
//                    }
//                    else if mmwGameSceneViewController.numPlayers == 4 {
//                        tileAdded.tileSprite.position = self.mmwPlayer1of4Grid.sendToGridSquare(self.mmwPlayer1of4Grid, squareX: x, squareY: y)
//                    }
//                    //self.addChild(tileAdded.tileSprite)  // !!!
//                    tileArrSelectedTile++
//                }
//            }
//        }
//        
//        tileArrSelectedTile = 0
//        //let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//        runAction(dealTilesSound)
//        for y in 0...1 {   // fill Player 2 letter tiles
//            for x in 0...2 {
//                if tileBuilder.mmwPlayer2LetterTileArray.count > tileArrSelectedTile {
//                    let tileAdded : MMWTile = tileBuilder.mmwPlayer2LetterTileArray[tileArrSelectedTile]
//                    if mmwGameSceneViewController.numPlayers == 2 {
//                        tileAdded.tileSprite.position = self.mmwPlayer2of2Grid.sendToGridSquare(self.mmwPlayer2of2Grid, squareX: x, squareY: y)
//                    }
//                    if mmwGameSceneViewController.numPlayers == 3 {
//                        tileAdded.tileSprite.position = self.mmwPlayer2of4Grid.sendToGridSquare(self.mmwPlayer2of4Grid, squareX: x, squareY: y)
//                    }
//                    else if mmwGameSceneViewController.numPlayers == 4 {
//                        tileAdded.tileSprite.position = self.mmwPlayer2of4Grid.sendToGridSquare(self.mmwPlayer2of4Grid, squareX: x, squareY: y)
//                    }
//                    //self.addChild(tileAdded.tileSprite)
//                    tileArrSelectedTile++
//                }
//            }
//        }
//        
//        if mmwGameSceneViewController.numPlayers == 3 {
//            runAction(dealTilesSound)
//            tileArrSelectedTile = 0
//            for y in 0...1 {   // fill Player 3 letter tiles
//                for x in 0...2 {
//                    if tileBuilder.mmwPlayer3LetterTileArray.count > tileArrSelectedTile {
//                        let tileAdded : MMWTile = tileBuilder.mmwPlayer3LetterTileArray[tileArrSelectedTile]
//                        tileAdded.tileSprite.position = self.mmwPlayer4of4Grid.sendToGridSquare(self.mmwPlayer4of4Grid, squareX: x, squareY: y)
//                        //self.addChild(tileAdded.tileSprite)
//                        tileArrSelectedTile++
//                    }
//                }
//            }
//        }
//        
//        if mmwGameSceneViewController.numPlayers == 4 {
//            runAction(dealTilesSound)
//            tileArrSelectedTile = 0
//            for y in 0...1 {   // fill Player 3 letter tiles
//                for x in 0...2 {
//                    if tileBuilder.mmwPlayer3LetterTileArray.count > tileArrSelectedTile {
//                        let tileAdded : MMWTile = tileBuilder.mmwPlayer3LetterTileArray[tileArrSelectedTile]
//                        tileAdded.tileSprite.position = self.mmwPlayer3of4Grid.sendToGridSquare(self.mmwPlayer3of4Grid, squareX: x, squareY: y)
//                        //self.addChild(tileAdded.tileSprite)
//                        tileArrSelectedTile++
//                    }
//                }
//            }
//            
//            tileArrSelectedTile = 0
//            runAction(dealTilesSound)
//            for y in 0...1 {   // fill Player 4 letter tiles
//                for x in 0...2 {
//                    if tileBuilder.mmwPlayer4LetterTileArray.count > tileArrSelectedTile {
//                        let tileAdded : MMWTile = tileBuilder.mmwPlayer4LetterTileArray[tileArrSelectedTile]
//                        tileAdded.tileSprite.position = self.mmwPlayer4of4Grid.sendToGridSquare(self.mmwPlayer4of4Grid, squareX: x, squareY: y)
//                        //self.addChild(tileAdded.tileSprite)
//                        tileArrSelectedTile++
//                    }
//                }
//            }
//        }
//        
////        tileArrSelectedTile = 0
////        runAction(dealTilesSound)
////        for y in 0...14 {  // fill Board letter tiles
////            for x in 0...14 {
////                if tileBuilder.mmwPlayedTileArray.count > tileArrSelectedTile {
////                    let tileAdded : MMWTile = tileBuilder.mmwPlayedTileArray[tileArrSelectedTile]
////                    tileAdded.tileSprite.position = self.mmwBoardGrid.sendToGridSquare(self.mmwBoardGrid, squareX: x, squareY: y)
////                    tileAdded.tileSprite.color = tileAdded.tileSprite.TileColors[ Int(arc4random_uniform(4)) ]
////                    //self.addChild(tileAdded.tileSprite)
////                    tileArrSelectedTile++
////                }
////            }
////        }
//        
////        let testTile = MMWTile(letterString: "$")
////        testTile.gridX = 7
////        testTile.gridY = 7
////        testTile.tileSprite.position = mmwBoardGrid.sendToGridSquare(mmwBoardGrid, squareX: 8, squareY: 9)
////        self.addChild(testTile.tileSprite)
//        
//        //self.mmwBoardGrid.sendToGridSquare(mmwBoardGrid, squareX: 7, squareY: 7) // returns position
//    }

    
    func partialWordHUD (letters : String, isWord : Bool)  -> SKLabelNode {
        var isPartial : String
        if isWord {
            isPartial = ""
        }
        else {
            isPartial = "not"
        }
        partialWordLabel.text = "\(letters) is \(isPartial) a partial word"
        partialWordLabel.fontSize = FontHUDSize
        partialWordLabel.position = CGPointMake(size.width/2.0, 3)
        partialWordLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(partialWordLabel)
        return partialWordLabel
    }
    
    func timeRemainingHUD (timeAmount: Int)  -> SKLabelNode {
        
        timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
        
        timeRemainingLabel.zPosition = 1
        timeRemainingLabel.text =  "Timer: \( String(secondsLeft) ) " // "Timer: \(timeAmount)"
        timeRemainingLabel.fontSize = FontHUDSize
        timeRemainingLabel.fontColor = FontHUDRed
        timeRemainingLabel.position = CGPointMake(size.width/2.0 + 255.0, 3)
        timeRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(timeRemainingLabel)
        return timeRemainingLabel
    }
    
    func updateCounter() {
        timeRemainingLabel.text = "Timer: \( String(secondsLeft--) ) " // String(counter--)
        if secondsLeft <= -1 {
            if secondsLeft == -1 { runAction(actionSound) }  // play turn over sound
            timeRemainingLabel.text = ("Timer: 0")
        }
    }
    
    func tilesRemainingHUD (tilesLeft : Int) -> SKLabelNode {
        tilesRemainingLabel.zPosition = 1
        tilesRemainingLabel.text = "Tiles Left: \(tilesLeft)"
        tilesRemainingLabel.fontSize = FontHUDSize
        tilesRemainingLabel.position = CGPointMake(size.width/2.0 - 332, 3.0)
        tilesRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(tilesRemainingLabel)
        return tilesRemainingLabel
    }
    
    func topDisplayHUD (message : String)  -> SKLabelNode {
        topDisplayLabel.zPosition = 1
        topDisplayLabel.text = message
        topDisplayLabel.fontSize = FontHUDSize
        topDisplayLabel.position = CGPointMake(size.width/2.0, 744.0) // CGPointMake(size.width/2.0, 753.0) // 1 of 2 top lines
        topDisplayLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayLabel)
        return topDisplayLabel
    }
    
    func topDisplayHUD2 (message : String)  -> SKLabelNode {
        topDisplayLabel2.zPosition = 1
        topDisplayLabel2.text = message
        topDisplayLabel2.fontSize = FontHUDSize
        topDisplayLabel2.position = CGPointMake(size.width/2.0, 735.0) // 2 of 2 top lines
        topDisplayLabel2.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayLabel2)
        return topDisplayLabel2
    }

    func addPlayerView (playerNum : Int, playerView : PlayerView) {
        if mmwGameSceneViewController.numPlayers == 2 {
            // if only 2 players - one on left and one on right of UI
            if playerNum == 1 {
                playerView.position = CGPointMake(0, self.size.height * 0.384765625 ) // PS coordinates is Y: 1390, convert and flip .. x , 295.5
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(self.size.width * 0.84912109375, self.size.height * 0.384765625 ) // PS coordinates is Y: 1390, convert and flip , x , 295.5
            }
        }
        
        if mmwGameSceneViewController.numPlayers == 3 {
            // if 3 players - one on left and two on right of UI
            if playerNum == 1 {
                playerView.position = CGPointMake(0, self.size.height * 0.384765625 ) // PS coordinates is Y: 1390, convert and flip .. x , 295.5
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(viewSize.width * 0.8491, viewSize.height * 0.57096354 ) // PS coordinates is Y: 1390, convert and flip
            }
            if playerNum == 3 {
                playerView.position = CGPointMake(viewSize.width * 0.8491,  viewSize.height * 0.138 ) // PS coordinates is Y: 1390, convert and flip
            }
        }
   
        if mmwGameSceneViewController.numPlayers == 4 {
            // if 4 players - two on left and two on right of UI
            if playerNum == 1 {
                playerView.position = CGPointMake( 0,  viewSize.height * 0.57096354 ) // PS coordinates is Y: 1390, convert and flip
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(viewSize.width * 0.8491, viewSize.height * 0.57096354 ) // PS coordinates is Y: 1390, convert and flip
            }
            if playerNum == 3 {
                playerView.position = CGPointMake(0, viewSize.height * 0.138 ) // PS coordinates is Y: 1390, convert and flip
            }
            if playerNum == 4 {
                playerView.position = CGPointMake(viewSize.width * 0.8491,  viewSize.height * 0.138 ) // PS coordinates is Y: 1390, convert and flip
            }
        }
        playerView.zPosition = 0
        addChild(playerView)
    }

//
//    func addTile (letterTile : Character, location : CGPoint){
//        var tile = SKSpriteNode(imageNamed: "LetterTest90x90")
//        tile.position = location
//        tile.anchorPoint = CGPointMake(0, 0)
//        foregroundNode!.addChild(tile)
//    }
    
//    func updateGridFromArray (tileArray: [MMWTile], gridToUpdate: Grid ) {
//        mmwPlayer1of2Grid.fillGridFromArray(tileArray, gridToFill: gridToUpdate)
//        print(">>> updatePlayerTileRack(player) ")
//        var tileArrSelectedTile = 0
//        for y in 0...1 {   // fill Playerletter tiles
//            for x in 0...2 {
//                if tileArrSelectedTile < 6 {
//                    let tileAdded = tileArray[tileArrSelectedTile]
//                    tileAdded.tileSprite.hidden = false
//                    
//                    
//                    tileAdded.tileSprite.position = player.playerLetterGrid!.sendToGridSquare(mmwPlayer1of2Grid, squareX: x, squareY: y)
//                    tileAdded.tileSprite.removeFromParent()
//                    self.addChild(tileAdded.tileSprite)
//                    tileArrSelectedTile++
//                    print("\(tileAdded.tileSprite.tileText)")
//                }
//            }
//        }
//        
//    }
    
    func updateGridFromArray (arrayOfTiles: [MMWTile], grid: Grid) {
         grid.fillGridFromArray(arrayOfTiles, gridToFill: grid)
        
        var tileArrSelectedTile = 0
        for y in 0...1 {   // fill Player 3 letter tiles
            for x in 0...2 {
                if mmwGameSceneViewController.tileCollection.mmwPlayedTileArray.count > tileArrSelectedTile {
                    let tileToGrid : MMWTile = arrayOfTiles[tileArrSelectedTile]
                    tileToGrid.tileSprite.hidden = false
                    // grid.   tileToGrid.tileSprite
                    tileToGrid.tileSprite.position = grid.sendToGridSquare(grid, squareX: x, squareY: y)
                    //tileToGrid.tileSprite.removeFromParent()
                    //self.addChild(tileToGrid.tileSprite)
                    tileArrSelectedTile++
                }
            }
        }
        printGrid(grid)
        
//        var tileArrSelectedTile = 0
//        for y in 0...1 {   // fill Player 3 letter tiles
//            for x in 0...2 {
//                if mmwGameSceneViewController.tileCollection.mmwPlayedTileArray.count > tileArrSelectedTile {
//                    let tileToGrid : MMWTile = arrayOfTiles[tileArrSelectedTile]
//                    tileToGrid.tileSprite.hidden = false
//                    // grid.   tileToGrid.tileSprite
//                    tileToGrid.tileSprite.position = grid.sendToGridSquare(grid, squareX: x, squareY: y)
//                    tileToGrid.tileSprite.removeFromParent()
//                    self.addChild(tileToGrid.tileSprite)
//                    tileArrSelectedTile++
//                }
//            }
//        }
//        printGrid(grid)
    }
    
    func printGrid (gridToPrint: Grid) {
        print(">>> printGrid: ")
        var arrayInMarker = 0
        for y in 0...(gridToPrint.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(gridToPrint.gridNumSquaresX - 1) {
                if arrayInMarker < gridToPrint.gridNumSquaresX * gridToPrint.gridNumSquaresY {
                    //gridToPrint.gridArr[y][x].tileSprite.letterLabel     // = arrayIn[arrayInMarker] as MMWTile
                    print(" \( gridToPrint.gridArr[y][x].tileSprite.tileText ) ")
                    arrayInMarker++
                }
            }
        }
    }

    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
        for touch: AnyObject in touches {
            let location = touch.locationInNode(self)
            let _node:SKNode = self.nodeAtPoint(location)
            
            if(_node.name == "playButton"){
                if userInteractionEnabled {
                    runAction(actionSound)
                    showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    _node.removeFromParent()
                }
            }
            
            if(_node.name == "newTilesButton"){
                if userInteractionEnabled {
                    runAction(actionSound)
                    
                    mmwGameSceneViewController.tileCollection.updateTiles(&mmwGameSceneViewController.tileCollection.mmwTileArray, tilesTo: &mmwGameSceneViewController.tileCollection.mmwPlayer1LetterTileArray, numTilesGet: 6, changeColorTo: (1))
                    
                    mmwGameSceneViewController.tileCollection.updateTiles(&mmwGameSceneViewController.tileCollection.mmwTileArray, tilesTo: &mmwGameSceneViewController.tileCollection.mmwPlayer2LetterTileArray, numTilesGet: 6, changeColorTo: (2))
                    
//                    mmwGameSceneViewController.tileCollection.updateTiles(&mmwGameSceneViewController.tileCollection.mmwTileArray, tilesTo: &mmwGameSceneViewController.tileCollection.mmwPlayer3LetterTileArray, numTilesGet: 6, changeColorTo: (3))
//                    
//                    mmwGameSceneViewController.tileCollection.updateTiles(&mmwGameSceneViewController.tileCollection.mmwTileArray, tilesTo: &mmwGameSceneViewController.tileCollection.mmwPlayer4LetterTileArray, numTilesGet: 6, changeColorTo: (4))
                    
                    //showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    if mmwGameSceneViewController.numPlayers == 2 {
                        
//                        mmwPlayer1of2Grid.fillGridFromArray(mmwGameSceneViewController.tileCollection.mmwPlayer1LetterTileArray, gridToFill: mmwPlayer1of2Grid)
//                        fillPlayerTileRack(mmwGameSceneViewController.player1)
//                        mmwPlayer2of2Grid.fillGridFromArray(mmwGameSceneViewController.tileCollection.mmwPlayer2LetterTileArray, gridToFill: mmwPlayer2of2Grid)
//                        updatePlayerTileRack(mmwGameSceneViewController.player2)
                        
                        updateGridFromArray(mmwGameSceneViewController.tileCollection.mmwPlayer1LetterTileArray, grid: mmwPlayer1of2Grid)
                        updateGridFromArray(mmwGameSceneViewController.tileCollection.mmwPlayer2LetterTileArray, grid: mmwPlayer2of2Grid)

                    }
                    
//                    else if mmwGameSceneViewController.numPlayers == 3 {
//                        mmwPlayer1of2Grid.fillGridFromArray(mmwGameSceneViewController.player1.playerLetterTiles!, gridToFill: mmwPlayer1of2Grid)
//                        updatePlayerTileRack(mmwGameSceneViewController.player1)
//                        mmwPlayer2of4Grid.fillGridFromArray(mmwGameSceneViewController.player2.playerLetterTiles!, gridToFill: mmwPlayer2of4Grid)
//                        updatePlayerTileRack(mmwGameSceneViewController.player2)
//                        mmwPlayer4of4Grid.fillGridFromArray(mmwGameSceneViewController.player3.playerLetterTiles!, gridToFill: mmwPlayer4of4Grid)
//                        updatePlayerTileRack(mmwGameSceneViewController.player3)
//                    }
//                    
//                    else if mmwGameSceneViewController.numPlayers == 4 {
//                        mmwPlayer1of4Grid.fillGridFromArray(mmwGameSceneViewController.player1.playerLetterTiles!, gridToFill: mmwPlayer1of4Grid)
//                        updatePlayerTileRack(mmwGameSceneViewController.player1)
//                        mmwPlayer2of4Grid.fillGridFromArray(mmwGameSceneViewController.player2.playerLetterTiles!, gridToFill: mmwPlayer2of4Grid)
//                        updatePlayerTileRack(mmwGameSceneViewController.player2)
//                        mmwPlayer3of4Grid.fillGridFromArray(mmwGameSceneViewController.player3.playerLetterTiles!, gridToFill: mmwPlayer3of4Grid)
//                        updatePlayerTileRack(mmwGameSceneViewController.player3)
//                        mmwPlayer4of4Grid.fillGridFromArray(mmwGameSceneViewController.player4.playerLetterTiles!, gridToFill: mmwPlayer4of4Grid)
//                        updatePlayerTileRack(mmwGameSceneViewController.player4)
//                    }
                    
                    //mmwBoardGrid.fillGridFromArray(mmwGameSceneViewController.tileCollection.mmwTileArray, gridToFill: mmwBoardGrid)
                    //updateBoard(mmwBoardGrid)
                    
                    tilesRemainingLabel.text = "Tiles Left: \(mmwGameSceneViewController.tileCollection.mmwTileArray.count  )" // " (mmwGameSceneViewController.tileCollection.mmwTileArray.count))"
                    if mmwGameSceneViewController.tileCollection.mmwTileArray.count <= 0 {
                        tilesRemainingLabel.text = "Tiles Left: None"
                    }
                }
            }
            
            if(_node.name == "passButton"){
                if userInteractionEnabled {
                    runAction(actionSound)
                    //showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    numEmptyTileSlots(mmwPlayer1of4Grid)
                    //_node.removeFromParent()
                }
            }
            
            if(_node.name == "optionsButton"){
                if userInteractionEnabled {
                    runAction(actionSound)
                    //showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    
                    print("mmwGameSceneViewController.tileCollection.mmwPlayer1LetterTileArray: ")
                    mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwPlayer1LetterTileArray)
                    mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwPlayer2LetterTileArray)
//                    mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwPlayer3LetterTileArray)
//                    mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwPlayer4LetterTileArray)
                    print("END mmwGameSceneViewController.tileCollection.mmwPlayer1LetterTileArray: ")
                    
                }
            }
            
            if(_node.name == "pauseButton"){
                if userInteractionEnabled {
                    //mmwPlayer3of4Grid.fillArrayFromGrid((mmwGameSceneViewController.player1.playerLetterTiles)!, gridOut: mmwPlayer3of4Grid)
                    runAction(actionSound)
                    if !isPaused {
                        isPaused = true
                        pauseButton.texture = SKTexture(imageNamed: "PlayButton.png")
                        timer!.invalidate()
                    }
                    else {
                        isPaused = false
                        pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
                        timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
                    }
                }
            }
        }
        

//        //checkForTouches(touches, withEvent: event)
//        print("TOUCH began in MMWGameScene: \(event!.description) ")
//        
//        for touch: AnyObject in touches {
//            var location = (touch as! UITouch).locationInView(self.view)
//            
//            location.y = 768 - location.y
//            //////////////let location = touch.locationInNode(self)
//            let nodes = nodesAtPoint(location) as [SKNode]
//            let snapGrid : Grid = getSnapGrid(location)!
//            
//            let gridSquareX = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
//            let gridSquareY = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
//            let gridSquareXCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerX
//            let gridSquareYCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerY
//            print("snapGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
//            print("nodes.count is: \(nodes.count) ")
//            
//            for node in nodes {
//                if node.isKindOfClass(LetterTileSprite){
//                    node.touchesBegan(touches, withEvent: event)
//                    node.touchesMoved(touches, withEvent: event)
//                }
//            }
//        }
    }
    
    func getSnapGrid (testSpot : CGPoint) -> Grid? {
        var snapGrid : Grid = mmwBoardGrid
        
        if ((testSpot.x > 157) && (testSpot.x < 869.5)) {snapGrid = mmwBoardGrid}
        
        else if mmwGameSceneViewController.numPlayers == 2 {
            if (testSpot.x < 154.5 ) {snapGrid = mmwPlayer1of2Grid}
            if (testSpot.x > 869.5) {snapGrid = mmwPlayer2of2Grid}
        }
        else if mmwGameSceneViewController.numPlayers == 4 {
            if (testSpot.x < 154.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer1of4Grid}
            if (testSpot.x > 869.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer2of4Grid}
            if (testSpot.x < 154.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer3of4Grid}
            if (testSpot.x > 869.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer4of4Grid}
        }
        //else {snapGrid = nil}
        print("<MMWGameScene.GetSnapGrid> \(testSpot.x), \(testSpot.y), and grid \(snapGrid), and \(snapGrid.gridName)" )
        return snapGrid
    }
    
    
    func numEmptyTileSlots (tileGrid: Grid) -> Int {
        var numEmpty = 0
//        for slot in tileGrid.gridArr {
//            if slot.description != "MarkMyWord.MMWTile" {
//                numEmpty++
//            }
//        }
//        print(">> There are \(numEmpty) slots != MarkMyWord.MMWTile in grid")
        
        for arrayX in tileGrid.gridArr {
            //print("> debugDescription \(arrayX.debugDescription)")
            for arrayY in arrayX {
                //print("> debugDescription \(arrayY.debugDescription)")
                if arrayY.letterString == "!" {
                    //print(">> arrayX debugDescription check == 0")
                    numEmpty++
                }
            }
        }
        print("<MMWGameScene>numEmptyTileSlots ( = 0) in grid : \(numEmpty) " )
        return numEmpty
    }
    
    override func update(currentTime: NSTimeInterval) {
        // runAction(actionSound)
    }
    
    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        tilesRemainingLabel.zPosition = 1
        tilesRemainingLabel.text = "TilesRemainTouchMove"
        tilesRemainingLabel.text = String(987)
        
        player2View?.playerScoreLabel.zPosition = 1
        player2View?.playerScoreLabel.text = "MMWGameSceneTouchesMoved"
    }
    
    func presentMenuScene() {
        let transition = SKTransition.crossFadeWithDuration(0.5)
        let menuScene = MenuScene(size: size,
            gameResult: true,
            score: 123)
        view?.presentScene(menuScene, transition: transition)
    }
    
//    func fillGrid(gridToFill: Grid){
//        for tileSpot in gridToFill.gridArr {
//            if tileSpot.isEmpty {
//                tileSpot.
//        }
//    }
    
//    func showAllTileSprites () {
//        let tileCollectionTest = mmwGameSceneViewController.tileCollection
//        let tileSnap : CGPoint = CGPointMake(165.0, 28.0)
//        let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnap) // .getBoardGrid()
//        
//        // get snap location for point touch ended // -15.5 on y touch point adjusts for snapping too high to lower square
//        //var tileSnapCalculate = gameGrid.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y))
//        // get snap x, y values
//        
//        let tileSnapCalculateX : CGFloat = CGFloat(gameGrid!.getGridSquare(Float(tileSnap.x), locY: Float(tileSnap.y)).GridSquareUpperLeftCornerX)
//        print("tileSnapCalculateX \(tileSnapCalculateX)" )
//        let tileSnapCalculateY : CGFloat = CGFloat(gameGrid!.getGridSquare(Float(tileSnap.x), locY: Float(tileSnap.y)).GridSquareUpperLeftCornerY - 15.5) // -15.5
//        print("tileSnapCalculateY \(tileSnapCalculateY)" )
//        // get snap grid array [[]] positions // -15.5 on y touch point adjusts for snapping too high to lower square
//        let tileSnapXGrid : Int = (gameGrid!.getGridSquare(Float(tileSnap.x), locY: Float(tileSnap.y)).GridSquareX)
//        print("tileSnapXGrid \(tileSnapXGrid)" )
//        let tileSnapYGrid : Int = (gameGrid!.getGridSquare(Float(tileSnap.x), locY: Float(tileSnap.y)).GridSquareY ) // -15.5
//        print("tileSnapYGrid \(tileSnapYGrid)" )
//        
//        // move tile to snap point
//        self.position.x = tileSnapCalculateX + 23.75  //adjusts 22.5 for tile center in middle of tile
//        self.position.y = 768 - (tileSnapCalculateY + 8.25) //38 adjusts for tile center and for board not in exact middle when flipping coords
//        
//        print("The scene at tile end touch : \(scene?.description) and grid location : \(tileSnapXGrid), \(tileSnapYGrid) ")
//        
//        //                let snapGrid : Grid = getSnapGrid(location)!
//        //
//        //                let gridSquareX = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
//        //                let gridSquareY = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
//        //                let gridSquareXCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerX
//        //                let gridSquareYCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerY
//        //                print("snapGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
//        //
//        //
//        var tileArrSelectedTile = 0
//        let tileAdded : MMWTile = tileCollectionTest.mmwTileArray[tileArrSelectedTile]
//        
//        tileAdded.tileSprite.position = CGPointMake(tileSnapCalculateX, tileSnapCalculateY)
//        self.addChild(tileAdded.tileSprite)
//        tileArrSelectedTile++
//    }

}

 