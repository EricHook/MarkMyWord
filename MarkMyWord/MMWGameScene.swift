//
//  GameScene.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/10/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit
//import CoreMotion
//import GoogleMobileAds

//struct validAITilePlay {
//    var tile : MMWTile = MMWTile()
//    var gridXEnd: Int = -1
//    var gridYEnd: Int = -1
//    var partOfWord = ""
//    var madeValidWord = false
//}


struct validAILetterPlay {
    var tileSpriteLetter = ""
    var gridXEnd         = -1
    var gridYEnd         = -1
    var partOfWord       = ""
    var madeValidWord    = false
}


class MMWGameScene : SKScene { // , NSObject, NSCoding { // , SKPhysicsContactDelegate {
    
    var viewSize = screenSize!
    var backgroundNode      : SKSpriteNode = SKSpriteNode(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])
    var gameGridNode        : SKSpriteNode = SKSpriteNode(imageNamed: "GameGrid.png")
    var foregroundNode      : SKSpriteNode = SKSpriteNode()
    
    var mmwBoardGrid: Grid!
    
    var mmwPlayer1Grid : Grid!
    var mmwPlayer2Grid : Grid!
    var mmwPlayer3Grid : Grid!
    var mmwPlayer4Grid : Grid!
    var playerGridArray : [Grid]!
    
    var player1View : PlayerView!
    var player2View : PlayerView!
    var player3View : PlayerView!
    var player4View : PlayerView!
    var playerViewArr = [PlayerView]()

    var timer : NSTimer?
    var timerDelay : NSTimer?

    var secondsLeft = mmwGameSceneViewController.secondsPerTurn
    var secondsDelay = 0
    var timeRemainingLabel  = SKLabelNode(fontNamed: FontHUDName)
    var secondsDelayLabel   = SKLabelNode(fontNamed: FontHUDName)
    var tilesRemainingLabel = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel     = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel2    = SKLabelNode(fontNamed: FontHUDName)
    
    let playButton     = SKSpriteNode(imageNamed: "PlayButton.png")
    var newTilesButton = SKSpriteNode(imageNamed: "NewTilesButton.png")
    let passButton     = SKSpriteNode(imageNamed: "PassButton.png")
    let pauseButton    = SKSpriteNode(imageNamed: "PauseButton.png")
    let optionsButton  = SKSpriteNode(imageNamed: "OptionsButton.png")
    var gameGrid       = SKSpriteNode(imageNamed: "GameGrid.png")
    var playAreaWhite  = SKSpriteNode(imageNamed: "PlayAreaWhite.png")
    var backgroundStripes       = SKSpriteNode(imageNamed: "BGStripesHole.png")

    var placeholderTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    
    let actionSound      = SKAction.playSoundFileNamed("pass.WAV", waitForCompletion: true)
    let dealTilesSound   = SKAction.playSoundFileNamed("pass.WAV", waitForCompletion: true)
    let destroyTileSound = SKAction.playSoundFileNamed("pass.WAV", waitForCompletion: true)
    //let backgroundMusic = SKAction.playSoundFileNamed("30Showdown.m4a", waitForCompletion: true)
    
    var gameIsPaused = true

    var foundValidWordOnTurn = false
    var numWordsPlayedInTurn = 0
    
    enum MMWErrorType: ErrorType {
        case MMWError
        case MMWError2(lettersNeeded: Int)
    }
    
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    
    override init(size: CGSize) {
        super.init(size: size)
        
        backgroundNode.anchorPoint = CGPoint(x: 0.0, y: 0.0)
        backgroundNode.position = CGPoint(x: 0.0, y: 0.0)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.name = "backgroundNode"
        backgroundNode.zPosition = -100
        backgroundNode.size.height = viewSize.height
        backgroundNode.size.width  = viewSize.width
        if self.childNodeWithName("backgroundNode") == nil {
            addChild(backgroundNode)
        }
        backgroundNode.alpha = 1.0
        
        backgroundStripes.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundStripes.position = CGPoint(x: size.width/2.0, y: 0.0)
        backgroundStripes.userInteractionEnabled = false
        backgroundStripes.name = "backgroundStripes"
        backgroundStripes.zPosition = -99
        backgroundStripes.size.height = viewSize.height
        backgroundStripes.size.width  = viewSize.width
        if self.childNodeWithName("backgroundStripes") == nil {
            addChild(backgroundStripes)
        }
        backgroundStripes.alpha = 0.15

//        // add BGBlack
//        backgroundBlackNode.position = CGPoint(x: screenSize!.width/2, y: screenSize!.height/2)
//        backgroundBlackNode.userInteractionEnabled = false
//        backgroundBlackNode.size = self.frame.size;
//        backgroundBlackNode.zPosition = -95
//        if self.childNodeWithName("backgroundBlackNode") == nil {
//            addChild(backgroundBlackNode)
//        }
//        backgroundBlackNode.alpha = 0.25

        playAreaWhite.anchorPoint = CGPoint(x: 0.0, y: 0.0)
        playAreaWhite.position = CGPoint(x: 154.5, y: 31.0)
        playAreaWhite.userInteractionEnabled = false
        playAreaWhite.name = "playAreaWhite"
        playAreaWhite.zPosition = -98
        if self.childNodeWithName("playAreaWhite") == nil {
            addChild(playAreaWhite)
        }
        playAreaWhite.alpha = 0.50

        gameGrid.anchorPoint = CGPoint(x: 0.0, y: 0.0)
        gameGrid.position = CGPoint(x: 154.5, y: 31.0)
        gameGrid.userInteractionEnabled = false
        gameGrid.name = "gameGrid"
        gameGrid.zPosition = -97
        if self.childNodeWithName("gameGrid") == nil {
            addChild(gameGrid)
        }

        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            playAreaWhite.xScale = 1.3333
            playAreaWhite.yScale = 1.3333
            playAreaWhite.position = CGPoint(x: 207.0, y: 42.0)
            
            gameGrid.xScale = 1.333
            gameGrid.yScale = 1.333
            gameGrid.position = CGPoint(x: 207.0, y: 42.0)
        }
              topDisplayHUD("Welcome to Mark My Word") // ("Turn: Player 1, Special Letter Bonus In Effect, 2x Point Bonus")
        topDisplayHUD2("Have fun!")
        
        tilesRemainingHUD(tileCollection!.mmwTileArray.count)

        timeRemainingLabel.zPosition = 1
        timeRemainingLabel.fontSize = FontHUDSize
        timeRemainingLabel.fontColor = FontHUDWhite
        timeRemainingLabel.position = CGPointMake(size.width * 0.757, size.height * 0.01)
        timeRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        timeRemainingLabel.text = "Timer: 00"
        addChild(timeRemainingLabel)
        
        playButton.position = CGPoint(x: (viewSize.width * 0.5), y: (viewSize.height * 0.1) )
        playButton.name = "playButton"
        self.addChild(playButton)
        
        newTilesButton.position = CGPoint(x: (viewSize.width * 0.0751), y: (viewSize.height * 0.125) )
        newTilesButton.name = "newTilesButton"
        newTilesButton.userInteractionEnabled = true
        self.addChild(newTilesButton)
        
        passButton.position = CGPoint(x: (viewSize.width * 0.0752), y: (viewSize.height * 0.05) )
        passButton.name = "passButton"
        passButton.userInteractionEnabled = true
        self.addChild(passButton)
        
        pauseButton.position = CGPoint(x: (viewSize.width * 0.9238), y: (viewSize.height * 0.125) ) // width old 0.9228
        pauseButton.name = "pauseButton"
        pauseButton.userInteractionEnabled = true
        self.addChild(pauseButton)
        
        optionsButton.position = CGPoint(x: (viewSize.width * 0.9238), y: (viewSize.height * 0.05) )
        optionsButton.name = "optionsButton"
        optionsButton.userInteractionEnabled = true
        if self.childNodeWithName("optionsButton") == nil {
            addChild(optionsButton)
        }
        
        let buttonResizeArr = [playButton, newTilesButton, passButton, pauseButton, optionsButton]
        
//        if mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
//            for button in buttonResizeArr {
//                button.size.width  = button.size.width  * 0.4 // 0.5
//                button.size.height = button.size.height * 0.4 // 0.5
//                topDisplayLabel.position.y -= 0.15
//            }
//        }
//        else if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus {
//            for button in buttonResizeArr {
//                button.size.width  = button.size.width  * 0.4 // 0.5
//                button.size.height = button.size.height * 0.4
//            }
//        }
        
        if mmwGame.deviceType == MMWGame.DeviceType.iPad {
            for button in buttonResizeArr {
                button.size.width  = button.size.width  * 0.8 // 1.33
                button.size.height = button.size.height * 0.8
            }
        }
            
        else if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            for button in buttonResizeArr {
                button.size.width  = button.size.width  * 1.1 // 1.33
                button.size.height = button.size.height * 1.1
            }
        }
    }
    
    
    func resetPassCounter () {
        mmwGameSceneViewController.consecutivePasses = 0
        if debugMode == true { print("\(mmwGameSceneViewController.consecutivePasses) consecutive passes") }
    }
    
    
    func setGrids () {
        
        var gridSquareSize : Double!
        if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus || mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
            gridSquareSize = Double(viewSize.width * 0.04625 * 0.745)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPad {
            gridSquareSize = Double(viewSize.width * 0.04638772)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            gridSquareSize = Double(viewSize.width * 0.0463)
        }
        
        var gridBoardUpperLeftX : Double!
        if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus || mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
            gridBoardUpperLeftX = Double(viewSize.width * 0.153 * 1.54)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPad {
            gridBoardUpperLeftX = Double(viewSize.width * 0.1519)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            gridBoardUpperLeftX = Double(viewSize.width * 0.153)
        }
        
        var gridBoardUpperLeftY : Double!
        if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus || mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
            gridBoardUpperLeftY = Double(viewSize.height * 0.0882 * 1.04)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPad {
            //gridBoardUpperLeftY = Double(viewSize.height * 0.087)
            gridBoardUpperLeftY = Double(viewSize.height * 0.070)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            //gridBoardUpperLeftY = Double(viewSize.height * 0.0882)
            gridBoardUpperLeftY = Double(viewSize.height * 0.0697)
        }
        
        self.mmwBoardGrid = Grid(gridUpperLeftX: gridBoardUpperLeftX, gridUpperLeftY: gridBoardUpperLeftY, gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 15, gridNumSquaresY: 15, gridName: "mmwBoardGrid")
        
        // minus .017 in y for board shift
        //        self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.272), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer1Grid")
        self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.255), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer1Grid")
        self.mmwPlayer1Grid.setGridPlayer(mmwGameSceneViewController.player1)
        
        self.mmwPlayer2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.255), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer2Grid")
        self.mmwPlayer2Grid.setGridPlayer(mmwGameSceneViewController.player2)
        
        //        self.mmwPlayer3Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.705), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer3Grid")
        self.mmwPlayer3Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.6887), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer3Grid")
        self.mmwPlayer3Grid.setGridPlayer(mmwGameSceneViewController.player3)
        
        self.mmwPlayer4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.6887), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer4Grid")
        self.mmwPlayer4Grid.setGridPlayer(mmwGameSceneViewController.player4)
        
        if mmwGameSceneViewController.numPlayers == 2 {
            self.mmwPlayer1Grid.gridUpperLeftX = Double(viewSize.width * 0.0058)
            //self.mmwPlayer1Grid.gridUpperLeftY = Double(viewSize.height * 0.4581)
            self.mmwPlayer1Grid.gridUpperLeftY = Double(viewSize.height * 0.4411)
            
            self.mmwPlayer2Grid.gridUpperLeftX = Double(viewSize.width * 0.8550)
            self.mmwPlayer2Grid.gridUpperLeftY = Double(viewSize.height * 0.4411)
        }
        
        if mmwGameSceneViewController.numPlayers == 3 {
            self.mmwPlayer1Grid.gridUpperLeftX = Double(viewSize.width * 0.0058)
            self.mmwPlayer1Grid.gridUpperLeftY = Double(viewSize.height * 0.4411)
            
            self.mmwPlayer2Grid.gridUpperLeftX = Double(viewSize.width * 0.8550)
            //self.mmwPlayer2Grid.gridUpperLeftY = Double(viewSize.height * 0.2727)
            self.mmwPlayer2Grid.gridUpperLeftY = Double(viewSize.height * 0.2557)
            
            
            self.mmwPlayer3Grid.gridUpperLeftX = Double(viewSize.width * 0.8550)
            //self.mmwPlayer3Grid.gridUpperLeftY = Double(viewSize.height * 0.7057)
            self.mmwPlayer3Grid.gridUpperLeftY = Double(viewSize.height * 0.6887)
        }
    }
    
    
    func changePlayerScoreDisplay (playerView: PlayerView, player: Player, score: Int) {
        player.playerScore = score
        playerView.playerScoreLabel.text = String(score)
    }
    
    
    func buildGameView () {
        if debugMode == true { print("---   in buildGameView mmwGameScene") }
        //gameViewController.ViewAllOptionsUI.hidden = true
        userInteractionEnabled = true
        mmwGameSceneViewController.consecutivePasses = 0
        
        setFontHUDSize ()
        
        setGrids() // sets tile grid positions, size square, number squares and position on screen for each grid possible
        
        // ?? reset playerViewArray
        for playerViewTempNum in 0..<playerViewArr.count {
            playerViewArr.removeAtIndex(playerViewTempNum)
        }
        
        player1View = updatePlayerView(1, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.playerArray[0]))
        playerViewArr.append(player1View)
        player1View.hidden = false
        
        player2View = updatePlayerView(2, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.playerArray[1]))
        playerViewArr.append(player2View)
        player2View.hidden = false
        
        player3View = updatePlayerView(3, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.playerArray[2]))
        playerViewArr.append(player3View)
        
        if mmwGameSceneViewController.numPlayers > 2 {
            player3View.hidden = false
        } else {
            player3View.hidden = true
        }
        
        player4View = updatePlayerView(4, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.playerArray[3]))
        playerViewArr.append(player4View)
        if mmwGameSceneViewController.numPlayers > 3 {
            player4View.hidden = false
        } else {
            player4View.hidden = true
        }
        
        for playerView in playerViewArr {
            playerView.zPosition = -1
            addChild(playerView)
        }
        
        //        timer = nil
        //        timeRemainingHUD(mmwGameSceneViewController.secondsPerTurn)  // default set to         standard time remaining
        //        isPaused = true
        //        if (timer != nil) {
        //            timer!.invalidate()
        //        }
        
        // ADD ALL TILES to Scene - they start as invisible
        var tileTempXLocation = 0
        
        tileCollection = MMWTileBuilder()
        tileCollection!.resetAllTiles()
        
        for tile in tileCollection!.mmwTileArray {
            tile.tileSprite.hidden = true
            tileTempXLocation += 40
            tile.tileSprite.tileLocation = CGPoint(x: tileTempXLocation, y: 15 )
            self.addChild(tile.tileSprite)
        }

        // buttons inactive until "playButton" is pressed
        newTilesButton.userInteractionEnabled = true
        newTilesButton.alpha = 0.5
        
        passButton.userInteractionEnabled = true
        passButton.alpha = 0.5
        
        pauseButton.userInteractionEnabled = true
        pauseButton.alpha = 0.5
        
        optionsButton.userInteractionEnabled = true
        optionsButton.alpha = 0.5
    }
    
    
    func resetGameView () {
        
        mmwGameSceneViewController.consecutivePasses = 0
        if debugMode == true { print("---   in resetGameView mmwGameScene") }
        gameViewController.ViewAllOptionsUI.hidden = true
        
        // update locations and visibility of player views
        player1View = updatePlayerView(1, playerView: player1View)
        player2View = updatePlayerView(2, playerView: player2View)
        player3View = updatePlayerView(3, playerView: player3View)
        player4View = updatePlayerView(4, playerView: player4View)
        
        player1View.changePlayerScoreDisplay(0)
        player2View.changePlayerScoreDisplay(0)
        player3View.changePlayerScoreDisplay(0)
        player4View.changePlayerScoreDisplay(0)
        
        if mmwGameSceneViewController.timerIsOn == false {
            timeRemainingLabel.text = "Timer: off"
        } else {
            timeRemainingLabel.text = "Timer: \(mmwGameSceneViewController.secondsPerTurn) "
        }
        
        //bottomDisplayLabel.text =  "Begin ... "
        topDisplayLabel.text = "                     Welcome to Mark My Word"
        topDisplayLabel2.text =  "Have Fun!"
        
        tileCollection!.resetAllTiles()
        tileCollection = MMWTileBuilder()
        
        if tileCollection!.mmwTileArray.count > 0 {
            //newTilesButtonOn()
            tilesRemainingLabel.text = "Tiles: \(tileCollection!.mmwTileArray.count)"
        }
        else {
            //newTilesButtonOff()
            tilesRemainingLabel.text = "Tiles: 00"
        }

        for tile in tileCollection!.mmwTileArray {
            tile.tileSprite.hidden = true
            //tileTempXLocation += 40
            tile.tileSprite.tileLocation = CGPoint(x: 15, y: 15 )
            self.addChild(tile.tileSprite)
        }
        
        // buttons inactive until "playButton" is pressed
        newTilesButton.userInteractionEnabled = true
        newTilesButton.alpha = 0.5
        
        passButton.userInteractionEnabled = true
        passButton.alpha = 0.5
        
        pauseButton.userInteractionEnabled = true
        pauseButton.alpha = 0.5
        
        optionsButton.userInteractionEnabled = true
        optionsButton.alpha = 0.5
        
        stopTimer()
    }
    
    
    func showAllGridTiles (gridToDisplay: Grid) {
        if debugMode == true { print("---   in showAllGridTiles mmwGameScene Grid: \(gridToDisplay) #\(gridToDisplay) ") }
        var letterCount = 0
        //runAction(dealTilesSound)
        for x in 0...gridToDisplay.gridNumSquaresX - 1 {   // fill Player 1 letter tiles : 2 in y and 3 in x
            for y in 0...gridToDisplay.gridNumSquaresY - 1 {
                //                let tileAdded : MMWTile = gridToDisplay.grid2DArr[x][y] // tileBuilder.mmwPlayer1LetterTileArray[tileArrSelectedTile]
                //                tileAdded.tileSprite.hidden = false
                //                tileAdded.tileSprite.position = Grid.sendToGridSquare(gridToDisplay, squareX: x, squareY: y)
                //let tileAdded : MMWTile = gridToDisplay.grid2DArr[x][y] // tileBuilder.mmwPlayer1LetterTileArray[tileArrSelectedTile]
                gridToDisplay.grid2DArr[x][y].tileSprite.hidden = false
                gridToDisplay.grid2DArr[x][y].tileSprite.position = Grid.sendToGridSquare(gridToDisplay, squareX: x, squareY: y)
                
                if gridToDisplay.grid2DArr[x][y].tileType == TileType.Letter {letterCount += 1}
            }
        }
        if debugMode == true { print("---   in showAllGridTiles mmwGameScene Count: #\(letterCount) ") }
    }
    
    
    func showTilesInSquares (tileBuilder: MMWTileBuilder) { // shows all tiles from TileBuilder [MMWTile] arrays
        if debugMode == true { print("---   in showTilesInSquares mmwGameScene") }
        //runAction(dealTilesSound)
        
        showAllGridTiles(mmwPlayer1Grid)
        showAllGridTiles(mmwPlayer2Grid)
        
        if mmwGameSceneViewController.numPlayers > 2 {
            showAllGridTiles(mmwPlayer3Grid)
        }
        
        if mmwGameSceneViewController.numPlayers > 3 {
            showAllGridTiles(mmwPlayer4Grid)
        }
        showAllGridTiles(mmwBoardGrid)
        
        if debugMode == true { print("---   in showTilesInSquares mmwGameScene mmwDiscardedTileArray.count: #\(tileCollection?.mmwDiscardedTileArray.count)) ") }
        if debugMode == true { print("---   in showTilesInSquares mmwGameScene mmwTileArray.count: #\(tileCollection?.mmwTileArray.count)) ") }
        if debugMode == true { print("---   in showTilesInSquares mmwGameScene mmwTileArrayPlaceholder: #\(tileCollection?.mmwTileArrayPlaceholder.count)) ") }
    }
    
    
    //    func timeRemainingHUD (timeAmount: Int) { // -> SKLabelNode {
    //
    //        if secondsLeft == 999 {
    //            timeRemainingLabel.text =  "Timer: off"
    //        }
    //        else {
    //            if mmwGameSceneViewController.timerIsOn {
    //                timeRemainingLabel.text =  "Timer: \(String(secondsLeft)) " // "Timer: \(timeAmount)"
    //                //startTimer(timeAmount)
    //            }
    //            else {
    //                timeRemainingLabel.text =  "Timer: off"
    //            }
    //        }
    ////        addChild(timeRemainingLabel)
    ////        return timeRemainingLabel
    //
    //    }
    
    
    func updateCounter() {
        if mmwGameSceneViewController.timerIsOn {
            timeRemainingLabel.fontColor = SKColor(red: 1, green: 1, blue: 1, alpha: 1) // counter starts white color
            timeRemainingLabel.text = "Timer: \(String(secondsLeft) ) " // String(counter--)
            
            if secondsLeft <= 10 && secondsLeft > 5 {
                timeRemainingLabel.text =  "Timer: \( String(secondsLeft) ) "
                timeRemainingLabel.fontColor = SKColor(red: 0, green: 1, blue: 1, alpha: 1) // counter turns orange color
//                if mmwGameSceneViewController.timerIsOn == true {
//                    let countdownSmall = SKAction.playSoundFileNamed("tick.wav", waitForCompletion: true)
//                    runAction(SKAction.sequence( [countdownSmall]) )
//                }
            }
            
            if secondsLeft <= 5 {
                timeRemainingLabel.text =  "Timer: \( String(secondsLeft) )"
                timeRemainingLabel.fontColor = SKColor(red: 1, green: 0, blue: 0, alpha: 1) // counter turns red color
                if mmwGameSceneViewController.timerIsOn == true {
                    let countdownBig = SKAction.playSoundFileNamed("tick.wav", waitForCompletion: true)
                    runAction(SKAction.sequence( [countdownBig]) )
                }

                if secondsLeft <= 0 {
                    //runAction(actionSound)   // play turn over sound
                    timeRemainingLabel.text = ("Timer: 0")
                    if gameIsSuspended == false {
                        changePlayerTurn()
                    }
                    else { pauseGame() }
                }
            }
            secondsLeft -= 1
            
            print("updateCounter()  \(secondsLeft)")
        }
    }
    
//    func updateDelayTimer() {
//        //secondsDelayLabel.fontColor = SKColor(red: 1, green: 1, blue: 1, alpha: 1) // counter starts white color
//        secondsDelayLabel.text = "Timer: \(String(secondsDelay) ) " // String(counter--)
//
//        if secondsDelay <= 0 {
//            print("updateDelayTimer() \(secondsDelay)")
//        }
//
//        secondsDelay -= 1
//
//        print("updateDelayTimer() <= 0")
//    }
//    
//    func checkDelayTimer() -> ( Bool ){
//        var timesUp = false
//
//        if secondsDelay <= 0 {
//            timesUp = true
//        }
//        
//        return timesUp
//    }
    
    
    func tilesRemainingHUD (tilesLeft : Int) -> SKLabelNode {
        tilesRemainingLabel.zPosition = 1
        tilesRemainingLabel.text = "Tiles: \(tileCollection!.mmwTileArray.count  )" // "Tiles Left: \(tilesLeft)"
        tilesRemainingLabel.fontSize = FontHUDSize
        tilesRemainingLabel.position = CGPointMake(size.width * 0.17, size.height * 0.01)
        //tilesRemainingLabel.position = CGPointMake(size.width * 0.873, size.height * 0.10)
        tilesRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(tilesRemainingLabel)
        return tilesRemainingLabel
    }
    
    
    //    func bottomDisplayHUD (letters : String)  -> SKLabelNode {        // bottom text display area - first line
    //        bottomDisplayLabel.zPosition = 1
    //        bottomDisplayLabel.text = "\(letters)"
    //        bottomDisplayLabel.fontSize = FontHUDSize
    //        bottomDisplayLabel.position = CGPointMake(size.width/2.0, size.height * 0.004)
    //        bottomDisplayLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
    //        addChild(bottomDisplayLabel)
    //        return bottomDisplayLabel
    //        bottomDisplayLabel.hidden = true
    //    }
    
    
    func topDisplayHUD (message : String) { //  -> SKLabelNode {         // top text display area - first line
        topDisplayLabel.zPosition = 1
        topDisplayLabel.text = "\(message)"
        topDisplayLabel.fontSize = FontHUDSize
        
        //topDisplayLabel.position = CGPointMake(size.width/2.0, self.size.height * 0.980) // CGPointMake(size.width/2.0, 753.0) // 1 of 2 top lines
        //topDisplayLabel.position = CGPointMake(size.width * 0.313, self.size.height * 0.0029)
        topDisplayLabel.position = CGPointMake(size.width * 0.313, self.size.height * 0.978)
        
        topDisplayLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center // = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayLabel)
        //return topDisplayLabel
    }
    
    
    func topDisplayHUD2 (message : String) { //  -> SKLabelNode {        // top text display area - second line
        topDisplayLabel2.zPosition = 1
        topDisplayLabel2.text = message
        topDisplayLabel2.fontSize = FontHUDSize
        
        //        topDisplayLabel2.position = CGPointMake(size.width/2.0, (self.size.height * 0.959) )// 2 of 2 top lines CGPointMake(size.width/2.0, 735.0)
        //topDisplayLabel2.position = CGPointMake(size.width * 0.685, self.size.height * 0.0029)
        topDisplayLabel2.position = CGPointMake(size.width * 0.685, self.size.height * 0.978)
        
        topDisplayLabel2.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
        addChild(topDisplayLabel2)
        //return topDisplayLabel2
    }
    
    
    func updatePlayerView (playerNum : Int, playerView : PlayerView)  -> PlayerView {
        if debugMode == true { print("---   in updatePlayerView mmwGameScene") }
        
        let isHuman = playerView.mmwPlayer.isHuman
        let meyamaImageNumber = playerView.mmwPlayer.playerMeyamaNumber
        let avatarImageNumber = playerView.mmwPlayer.playerAvatarNumber
        
        if  isHuman == false { // is Meyama
            playerView.playerSprite.texture  = SKTexture(imageNamed:gameViewController.meyamaImageArray[meyamaImageNumber])
            playerView.playerNameLabel.text = gameViewController.meyamaAvatarNames[meyamaImageNumber]
            
        }
        else { // is Human
            playerView.playerSprite.texture = SKTexture(imageNamed:gameViewController.playerImageArray[avatarImageNumber])
            //playerView.playerNameLabel.text = gameViewController.playerAvatarNames[avatarImageNumber]
            playerView.playerNameLabel.text = mmwGameSceneViewController.playerArray[playerNum-1].playerName
        }
        
        if mmwGameSceneViewController.numPlayers == 2 { // if 2 one left and one right
            if playerNum == 1 {
                //playerView.position = CGPointMake(0, self.size.height * 0.3853 )
                playerView.position = CGPointMake(0, self.size.height * 0.4023 )
                playerView.hidden = false
                //playerView.playerViewBeginTurn()
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(self.size.width * 0.8495, self.size.height * 0.4023 )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
            if playerNum == 3 {
                playerView.hidden = true
            }
            if playerNum == 4 {
                playerView.hidden = true
            }
        }
        
        if mmwGameSceneViewController.numPlayers == 3 { // if 3 one left two right
            if playerNum == 1 {
                playerView.position = CGPointMake(0, self.size.height * 0.4023 )
                playerView.hidden = false
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(viewSize.width * 0.8495, viewSize.height * 0.588 )
                playerView.hidden = false
            }
            if playerNum == 3 {
                playerView.position = CGPointMake(viewSize.width * 0.8495,  viewSize.height * 0.155 )
                playerView.hidden = false
            }
            if playerNum == 4 {
                //playerView.position = CGPointMake(viewSize.width * 0.8495,  viewSize.height * 0.155 )
                playerView.hidden = true
            }
        }
        
        if mmwGameSceneViewController.numPlayers == 4 { // if 4 two left two right
            if playerNum == 1 {
                playerView.position = CGPointMake( 0,  viewSize.height * 0.588 )
                playerView.hidden = false
                playerView.playerViewBeginTurn()
                //playerView.playerViewEndTurn()
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(viewSize.width * 0.8495, viewSize.height * 0.588 )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
            if playerNum == 3 {
                playerView.position = CGPointMake(viewSize.width * 0.8495,  viewSize.height * 0.155 )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
            if playerNum == 4 {
                playerView.position = CGPointMake(0, viewSize.height * 0.155   )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
        }
        return playerView
    }
    
    
    func updateGridInScene (gridToUpdate: Grid) {  // sets position of tile in grid to proper screen location
        for y in 0...(gridToUpdate.gridNumSquaresY - 1) {
            for x in 0...(gridToUpdate.gridNumSquaresX - 1) {
                let tileToUpdate : MMWTile = gridToUpdate.grid2DArr[x][y]
                tileToUpdate.tileSprite.tileLocation = Grid.sendToGridSquare(gridToUpdate, squareX: x, squareY: y)
            }
        }
    }
    
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
        for touch: AnyObject in touches {
            let location = touch.locationInNode(self)
            let _node: SKNode = self.nodeAtPoint(location)
            
//            if(_node.name == "optionsLayerNode"){
//                //if userInteractionEnabled {
//                //runAction(actionSound)
//                if debugMode == true { print(">>> optionsLayerNode PRESSED >>>") }
//                optionsLayerNode.hidden = true
//            }
            
            if(_node.name == "playButton"){
                playButton(_node)
            }
            
            if(_node.name == "newTilesButton"){
                newTilesButton(_node)
            }
            
            if(_node.name == "passButton"){
                playerPass()
                //passButton(_node)
            }
            
            if(_node.name == "optionsButton"){
                optionsButton (_node)
            }
            
            if(_node.name == "pauseButton"){
                pauseButton (_node)
            }
            
            if(_node.name == "playBtnPlay"){
                playBtnPlay ()
            }
            //            if(_node.name == "testPlayButton"){
            //                testPlayButton ()
            //            }
        }
    }
    
    
    func playButton (buttonNode: SKNode) {
        if userInteractionEnabled {
            if debugMode == true { print(">>> PLAY BUTTON PRESSED >>>") }
            startGame()  // get starter words, set buttons, get timles, show tiles, set timer
            
            timeRemainingLabel.hidden = false
            
            mmwGameScene.playButton.hidden = true
            //buttonNode.removeFromParent() // gets rid of play button in middle of screen
        }
    }
    
    
    func dealStarterWords() {
        if debugMode == true { print("---   in startGame mmwGameScene") }
        var starterWord0 = mmwGameSceneViewController.getRandomWord()
        while mmwGameSceneViewController.checkTilesForWord(starterWord0, letterTileArray: tileCollection!.mmwTileArray) == false {
            starterWord0 = mmwGameSceneViewController.getRandomWord()
        }
        var starterWord0TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord0, letterTileArray: &tileCollection!.mmwTileArray)
        
        var starterWord1 = mmwGameSceneViewController.getRandomWord()
        //        while mmwGameSceneViewController.checkTilesForWord(starterWord1, letterTileArray: tileCollection!.mmwTileArray) == false {
        //            starterWord1 = mmwGameSceneViewController.getRandomWord()
        //        }
        var starterWord1TileArray : [MMWTile]
        //        //var starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)
        //
        //        var starterWord2 = mmwGameSceneViewController.getRandomWord()
        //        while (starterWord2 == starterWord1) || (mmwGameSceneViewController.checkTilesForWord(starterWord2, letterTileArray: tileCollection!.mmwTileArray ) == false )  {
        //            starterWord2 = mmwGameSceneViewController.getRandomWord()
        //        }
        var starterWord2TileArray : [MMWTile]
        //        //var starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)
        
        if arc4random_uniform(100) > 50 { // VERTICAL
            if mmwGameSceneViewController.numStarterWords == 1 {
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 7, yStartSquare: 5, IsHorizonal: false, player: mmwGameSceneViewController.player0)
            }
            
            if mmwGameSceneViewController.numStarterWords == 2 {
                //                starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)!
                
                
                var starterWord1 = mmwGameSceneViewController.getRandomWord()
                while mmwGameSceneViewController.checkTilesForWord(starterWord1, letterTileArray: tileCollection!.mmwTileArray) == false {
                    starterWord1 = mmwGameSceneViewController.getRandomWord()
                }
                var starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 3, yStartSquare: 3, IsHorizonal: false, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 11, yStartSquare: 7, IsHorizonal: false, player: mmwGameSceneViewController.player0)
            }
            
            if mmwGameSceneViewController.numStarterWords == 3 {
                
                var starterWord1 = mmwGameSceneViewController.getRandomWord()
                while mmwGameSceneViewController.checkTilesForWord(starterWord1, letterTileArray: tileCollection!.mmwTileArray) == false {
                    starterWord1 = mmwGameSceneViewController.getRandomWord()
                }
                var starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)
                
                var starterWord2 = mmwGameSceneViewController.getRandomWord()
                while mmwGameSceneViewController.checkTilesForWord(starterWord2, letterTileArray: tileCollection!.mmwTileArray) == false {
                    starterWord2 = mmwGameSceneViewController.getRandomWord()
                }
                var starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord2, letterTileArray: &tileCollection!.mmwTileArray)
                
                //                starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)!
                //
                //                starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord2, letterTileArray: &tileCollection!.mmwTileArray)!
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 3, yStartSquare: 2, IsHorizonal: false, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 11, yStartSquare: 2, IsHorizonal: false, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord2TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 7, yStartSquare: 8, IsHorizonal: false, player: mmwGameSceneViewController.player0)
            }
        }
            
        else { // HORIZONTAL
            if mmwGameSceneViewController.numStarterWords == 1 {
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 5, yStartSquare: 7, IsHorizonal: true, player: mmwGameSceneViewController.player0)
            }
            
            if mmwGameSceneViewController.numStarterWords == 2 {
                
                var starterWord1 = mmwGameSceneViewController.getRandomWord()
                while mmwGameSceneViewController.checkTilesForWord(starterWord1, letterTileArray: tileCollection!.mmwTileArray) == false {
                    starterWord1 = mmwGameSceneViewController.getRandomWord()
                }
                var starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)
                
                //                starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)!
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 3, yStartSquare: 3, IsHorizonal: true, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 7, yStartSquare: 11, IsHorizonal: true, player: mmwGameSceneViewController.player0)
            }
            
            if mmwGameSceneViewController.numStarterWords == 3 {
                
                var starterWord1 = mmwGameSceneViewController.getRandomWord()
                while mmwGameSceneViewController.checkTilesForWord(starterWord1, letterTileArray: tileCollection!.mmwTileArray) == false {
                    starterWord1 = mmwGameSceneViewController.getRandomWord()
                }
                var starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)
                
                var starterWord2 = mmwGameSceneViewController.getRandomWord()
                while mmwGameSceneViewController.checkTilesForWord(starterWord2, letterTileArray: tileCollection!.mmwTileArray) == false {
                    starterWord2 = mmwGameSceneViewController.getRandomWord()
                }
                var starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord2, letterTileArray: &tileCollection!.mmwTileArray)
                
                //                starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)!
                //
                //                starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord2, letterTileArray: &tileCollection!.mmwTileArray)!
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 2, yStartSquare: 3, IsHorizonal: true, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 8, yStartSquare: 7, IsHorizonal: true, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord2TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 2, yStartSquare: 11, IsHorizonal: true, player: mmwGameSceneViewController.player0)
            }
        }
    }
    
    
    // get starter words, set buttons, get tiles, show tiles, set timer
    func startGame () {
        
        if debugMode == true { print("---   in resetGameView mmwGameScene") }
        gameViewController.ViewAllOptionsUI.hidden = true

        backgroundNode.texture = SKTexture(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])
            
//            backgroundNode    : SKSpriteNode = SKSpriteNode(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])
        
        tileCollection!.resetAllTiles()
        
        // update locations and visibility of player views
        //player1View = updatePlayerView(1, playerView: player1View)
        
        updatePlayerView(1, playerView: player1View)
        updatePlayerView(2, playerView: player2View)
        updatePlayerView(3, playerView: player3View)
        updatePlayerView(4, playerView: player4View)
        
        // buttons inactive until "playButton" is pressed
        newTilesButton.userInteractionEnabled = false
        newTilesButton.alpha = 1
        passButton.userInteractionEnabled = false
        passButton.alpha = 1
        pauseButton.userInteractionEnabled = false
        pauseButton.alpha = 1
        optionsButton.userInteractionEnabled = false
        optionsButton.alpha = 1
        
//        gameGridCover.hidden = true
        
        if mmwGameSceneViewController.timerIsOn == false {
            secondsLeft = 999
            timeRemainingLabel.text = "Timer: off"
            pauseButton.userInteractionEnabled = false
            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
            //pauseButton.texture = SKTexture(imageNamed: "MMWResumeButton.png")
            pauseButton.alpha = 0.5
            playButton.hidden = true
        } else {
            secondsLeft = mmwGameSceneViewController.secondsPerTurn
            timeRemainingLabel.text = "Timer: \(secondsLeft) "
            gameIsPaused = false
            //pauseButton.texture = SKTexture(imageNamed: "ResumeButton.png")
            stopTimer()
            startTimer(secondsLeft)
        }
        
        //bottomDisplayLabel.text =  "Begin ... "
        topDisplayLabel.text = " Welcome to Mark My Word"
        topDisplayLabel2.text =  "Have fun!"
        
        mmwGameScene.setGrids() // sets tile grid positions, size square, number squares and position on screen for each grid possible
        
        mmwGameSceneViewController.setUpPlayers() // add player to view, match player to grid, fill grid with starter tiles and colorize to player color
        
        mmwGameSceneViewController.playerTurn = 1
        
        dealStarterWords()
        
        mmwPlayer1Grid.dealGridFromArrayRandom(&tileCollection!.mmwTileArray, numTilesToDeal: 6, playerNum: 1, clearGrid: false)
        mmwPlayer2Grid.dealGridFromArrayRandom(&tileCollection!.mmwTileArray, numTilesToDeal: 6, playerNum: 2, clearGrid: false)
        player3View.hidden = true
        player4View.hidden = true
        
        if mmwGameSceneViewController.numPlayers > 2 {
            mmwPlayer3Grid.dealGridFromArrayRandom(&tileCollection!.mmwTileArray, numTilesToDeal: 6, playerNum: 3, clearGrid: false)
            player3View.hidden = false
            player4View.hidden = true
        }
        
        if mmwGameSceneViewController.numPlayers > 3 {
            mmwPlayer4Grid.dealGridFromArrayRandom(&tileCollection!.mmwTileArray, numTilesToDeal: 6, playerNum: 4, clearGrid: false)
            player3View.hidden = false
            player4View.hidden = false
        }
        
        showTilesInSquares(tileCollection!) // 'deals' player tiles
        
        tilesRemainingLabel.text = "Tiles: \(tileCollection!.mmwTileArray.count)"
        
        // set player turn to storing in mmwGameSceneViewController
        mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()
        
        for player in mmwGameSceneViewController.playerArray {
            player.playerScore = 0
            player.playerView!.changePlayerScoreDisplay(player.playerScore)
            player.playerView!.playerViewEndTurn()
        }
        
        player1View.playerViewBeginTurn()
        
        //animationsActive(true)

        var playerArrayNumber = 0
        for playerView in playerViewArr {
            
            //if playerArrayNumber > 0 { playerView.mmwPlayer.isHuman = false }
            
            let isHuman = playerView.mmwPlayer.isHuman
            
            let meyamaImageNumber = playerView.mmwPlayer.playerMeyamaNumber
            let avatarImageNumber = playerView.mmwPlayer.playerAvatarNumber
            
            if  isHuman == false {
                playerViewArr[playerArrayNumber].playerTex = SKTexture(imageNamed:gameViewController.meyamaImageArray[meyamaImageNumber])
            }
            else {
                playerViewArr[playerArrayNumber].playerTex = SKTexture(imageNamed:gameViewController.playerImageArray[avatarImageNumber])
                //                playerViewArr[playerArrayNumber].playerNameLabel.text = mmwGameSceneViewController.playerArray[playerArrayNumber].playerName
            }
            playerArrayNumber += 1
        }
        
        
        //backgroundNode = SKSpriteNode(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])
        //backgroundNode = SKSpriteNode(imageNamed: gameViewController.backgroundImageArray[4])
        
    }
    
    //    func testPlayButton () {
    //        var row = 0
    //        var col = 0
    //        for tileRow in mmwGameScene.mmwPlayer1Grid.grid2DArr {
    //            for tile in tileRow {
    //                print("Tile \(tile.tileSprite.tileText) \(row), \(col) ")
    //                tile.tileSprite.tileText = "X"
    //                //tile.tileSprite.hidden = true
    //                tile.resetTileValues()
    //                col++
    //            }
    //            row++
    //            col = 0
    //        }
    //        print("testPlayButton ()")
    //    }
    
    
    func numPlayerLettersOnBoard(player: Player) -> Int {
        var numLetterTiles = 0
        for y in 0...(mmwBoardGrid.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(mmwBoardGrid.gridNumSquaresX - 1) {
                if mmwBoardGrid.grid2DArr[x][y].tileType == TileType.Letter {
                    numLetterTiles += 1
                }
            }
        }
        return numLetterTiles
    }
    
    
    func playBtnPlay () { // playButtonPlayNode: SKNode
        
        unowned let playersTurn = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1]
        let playerLevel = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerSkillLevel
        
        let pauseTime = 7.0
        
        var numLettersRemaining = (mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid).numLetterTilesInGrid()
        
        var wordAttemptsPlayed = 0
        var playAnotherWord = 0
        
        if numLettersRemaining > 0 {
            // play first word attempt here
            let tileplay1 : (playedPartialWord: Int, playedWholeWord: Int) = playAILetters()
            
            let tileplay1Count = tileplay1.playedPartialWord > tileplay1.playedWholeWord ? tileplay1.playedPartialWord : tileplay1.playedWholeWord
            
            if (tileplay1.playedPartialWord > 0 || tileplay1.playedWholeWord > 0) {
                mmwGameSceneViewController.resetConsequtivePasses()
                if tileplay1.playedWholeWord > 0 {
                    wordAttemptsPlayed += 1
                }
            }
            
            numLettersRemaining = (mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid).numLetterTilesInGrid()
            
            //                if numLettersRemaining == 0 && mmwGameScene.secondsLeft > Int(12) { // 0 tiles left so pause and change turn
            //                    mmwGameSceneViewController.consecutivePasses++
            //                    delay(11.0){
            //                        self.changePlayerTurn()
            //                    }
            //                }
            
            if wordAttemptsPlayed == 1 && numLettersRemaining > 0 && mmwGameScene.secondsLeft > Int(12) && gameIsSuspended == false {
                
                //                    numLettersRemaining = (mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid).numLetterTilesInGrid()
                
                playAnotherWord = Int(arc4random_uniform(10))
                
                if playAnotherWord + (playerLevel * 3) > 5 && numLettersRemaining > 0 {
                    
                    delay(Double(tileplay1Count + 4)) { // (8.0) { // allow time for first word play
                        //Play second whole word, or partial
                        let tileplay2 : (playedPartialWord: Int, playedWholeWord: Int) = self.playAILetters()
                        let tileplay2Count = tileplay2.playedPartialWord > tileplay2.playedWholeWord ? tileplay2.playedPartialWord : tileplay2.playedWholeWord
                        
                        if tileplay2Count > 0 {
                            
                            wordAttemptsPlayed += 1
                            
//                            if numLettersRemaining == 0  && mmwGameScene.secondsLeft > Int(Double(tileplay2Count + 5)) {
//                                delay(Double(tileplay2Count + 4)){
//                                    self.changePlayerTurn()
//                                }
//                            }
                            
                            numLettersRemaining = (mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid).numLetterTilesInGrid()
                            
                            if numLettersRemaining == 0  && mmwGameScene.secondsLeft > Int(Double(tileplay2Count + 5)) {
                                delay(Double(tileplay2Count + 4)){
                                    self.changePlayerTurn()
                                }
                            }

                            
                            if tileplay2.playedWholeWord > 0 && numLettersRemaining > 0 && mmwGameScene.secondsLeft > Int(Double(tileplay2Count + 5)) && gameIsSuspended == false {
                                
                                playAnotherWord = Int(arc4random_uniform(10))
                                
                                if playAnotherWord + (playerLevel * 3) > 9 && numLettersRemaining > 0 {
                                    
                                    delay(Double(tileplay2Count + 4)) {           // pause to allow second word play
                                        
                                        let tileplay3 : (playedPartialWord: Int, playedWholeWord: Int) = self.playAILetters()
                                        let tileplay3Count = tileplay3.playedPartialWord > tileplay3.playedWholeWord ? tileplay3.playedPartialWord : tileplay3.playedWholeWord
                                        
//                                        self.playAILetters()     // Play third whole word, or partial
                                        
                                        //numLettersRemaining = (mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid).numLetterTilesInGrid()
                                        
                                        if mmwGameScene.secondsLeft > Int(Double(tileplay3Count + 4)) && gameIsSuspended == false {
                                            delay(Double(tileplay3Count + 3)){
                                                self.changePlayerTurn()
                                            }
                                        }
                                    }
                                }
                                    
                                else if mmwGameScene.secondsLeft > Int(Double(tileplay2Count + 5)) { // don't play third word attempt, either AI skip or no letters
                                    delay(Double(tileplay2Count + 4))  {
                                        self.changePlayerTurn()
                                    }
                                }
                            }
                            
                            else if mmwGameScene.secondsLeft > Int(Double(tileplay2Count + 5)) { // don't play third word attempt, either AI skip or no letters
                                delay(Double(tileplay2Count + 4))  {
                                    self.changePlayerTurn()
                                }
                            }
                        }
                        
                        else if mmwGameScene.secondsLeft > Int(Double(tileplay2Count + 5)) { // don't play third word attempt, either AI skip or no letters
                            delay(Double(tileplay1Count + 6))  {
                                self.changePlayerTurn()
                            }
                        }
                    }
                }
                    
                    
                else if mmwGameScene.secondsLeft > Int(tileplay1Count + 7) { // don't play second word attempt, either AI skip or no letters
                    delay(Double(tileplay1Count + 6)){
                        self.changePlayerTurn()
                    }
                }
            }
            
            else if gameIsSuspended == false { // 0 tiles left so pause and change turn
                delay(Double(tileplay1Count + 6)){
                    self.changePlayerTurn()
                }
            }
        }
            
            
        
        
//        changePlayerTurn()
        

            
        else if gameIsSuspended == false { // 0 tiles left so pause and change turn
            mmwGameSceneViewController.consecutivePasses += 1
            delay(3.0){
                self.changePlayerTurn()
            }
        }
    }
    

    
    func playAILetters() -> (playedWholeWord: Int, playedPartialWord: Int){
        numWordsPlayedInTurn = 0
        var playedWholeWord = 0
        var playedPartialWord = 0
        
        //        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
        //        dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in
        //            print("Currently dispatched thread asynchronously 0 playBtnPlay playAILetters")
        
        let lockedTilesArr = self.mmwBoardGrid.getArrayLockedLetterTilesInGrid()
        var numLockedTilesSearched = 0
        var checkForValidWordsAISearchedTRUE = 0
        
        let playerLetterTilesArr = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.getArrayLetterTilesInGrid() // num of tilesToPlay 0 to numTiles in player grid
        var playerTilesLettersArr : [String] = [String]()  // the playerTilesArr in corresponding Letter array
        
        for tile in playerLetterTilesArr {                    // prints out letters in player tiles
            if debugMode == true { print("\(tile.tileText) ", terminator: "") }
            playerTilesLettersArr.append(tile.tileText) // adds letters in player tiles to playerTilesLettersArr[]
            };  if debugMode == true { print("")   }    // to add a return on last letter
        
        let numLettersToPlayMin = 1
        var numLettersToPlayMax = 6
        
        var allPossiblePartialWordPlays = [[validAILetterPlay]]()
        var allPossibleWholeWordPlays   = [[validAILetterPlay]]()
        
        if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerSkillLevel == 0 {
            numLettersToPlayMax = 4
        }
        if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerSkillLevel == 1 {
            numLettersToPlayMax = 5
        }
        if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerSkillLevel == 2 {
            numLettersToPlayMax = 6
        }
        
        let permutationsToPlay : Set<String> = self.permuteLetters(playerTilesLettersArr, minStringLen: numLettersToPlayMin, maxStringLen: numLettersToPlayMax)
        
        for lockedTile in lockedTilesArr {
            
            if mmwGameScene.secondsLeft <= 5 {
                break
            }
            
            numLockedTilesSearched += 1
            if debugMode == true { print("looking at tile \(lockedTile.gridXEnd), \(lockedTile.gridYEnd)" ) }
            // checks for playable valid whole word in player tiles
            ////////////////////////
            var tileAIWordCheck = self.checkForValidWordsAI(lockedTile.gridXEnd, gridYSpot: lockedTile.gridYEnd, permutationsToPlay: permutationsToPlay, lookHorizontal: true)
            
            if (tileAIWordCheck.allFoundPartialWordPlaysAtLockedTile.count > 0) {
                
                allPossiblePartialWordPlays += tileAIWordCheck.allFoundPartialWordPlaysAtLockedTile
                
                if (tileAIWordCheck.allFoundWholeWordPlaysAtLockedTile.count > 0) {
                    
                    allPossibleWholeWordPlays += tileAIWordCheck.allFoundWholeWordPlaysAtLockedTile
                    if debugMode == true { print("AI found \(allPossiblePartialWordPlays.count )  \(allPossibleWholeWordPlays.count ) at lockedTile \(lockedTile.gridX), \(lockedTile.gridX) Horizontal") }
                }
            }
            
            tileAIWordCheck = self.checkForValidWordsAI(lockedTile.gridXEnd, gridYSpot: lockedTile.gridYEnd, permutationsToPlay: permutationsToPlay, lookHorizontal: false)
            
            if (tileAIWordCheck.allFoundPartialWordPlaysAtLockedTile.count > 0) {
                
                allPossiblePartialWordPlays += tileAIWordCheck.allFoundPartialWordPlaysAtLockedTile
                
                if (tileAIWordCheck.allFoundWholeWordPlaysAtLockedTile.count > 0) {
                    
                    allPossibleWholeWordPlays += tileAIWordCheck.allFoundWholeWordPlaysAtLockedTile
                    
                    if debugMode == true { print("AI found P:\(allPossiblePartialWordPlays.count )  W:\(allPossibleWholeWordPlays.count ) at lockedTile \(lockedTile.gridX), \(lockedTile.gridX) Horizontal") }
                    
                    if allPossibleWholeWordPlays.count > 0 {  // stop on first tile that finds possible whole word
                        break
                    }
                }
                break
            }
        }
        
        if debugMode == true { print(">> numLockedTilesSearched : \(numLockedTilesSearched += 1), checkForValidWordsAISearchedTRUE : \(checkForValidWordsAISearchedTRUE), allPossiblePartialWordPlays: \(allPossiblePartialWordPlays.count), allPossibleWholeWordPlays: \(allPossibleWholeWordPlays.count) playBtnPlay loadWords() mmwGameScene") }
        
        
        if allPossibleWholeWordPlays.count >= 1 {
            let wordToPlay = Int(arc4random_uniform(UInt32(allPossibleWholeWordPlays.count)))
            self.playAIPlaceholderTilesToBoard (allPossibleWholeWordPlays[wordToPlay])
            playedWholeWord = allPossibleWholeWordPlays[wordToPlay].count
        }
        else {
            var notAnEdgeWord = true

            for partialWord in allPossiblePartialWordPlays {
                notAnEdgeWord = true
                for letter in partialWord {
                    if letter.gridXEnd<2 || letter.gridXEnd>12 || letter.gridYEnd<2 || letter.gridYEnd>12 {
                        notAnEdgeWord = false
                    }
                    // might also check for 2 plus open spots in a direction to make future words
                }
                if notAnEdgeWord == true {
                    self.playAIPlaceholderTilesToBoard (partialWord)
                    playedPartialWord = partialWord.count
                    break
                }
            }
        }

        //            dispatch_async(dispatch_get_main_queue(), {
        //                print("hello from playBtnPlay playAILetters thread executed as dispatch")
        //            })
        //        })
        
        //print("hello from playBtnPlay playAILetters thread")
        return (playedWholeWord, playedPartialWord)
        
    }
    

    func lookForPlayedSpots() -> ([MMWTile]){
        var playedSpots = [MMWTile]()
        var numLetterTiles = 0
        for y in 0...(mmwBoardGrid.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(mmwBoardGrid.gridNumSquaresX - 1) {
                if mmwBoardGrid.grid2DArr[x][y].tileType == TileType.Letter {
                    numLetterTiles += 1
                    playedSpots.append(mmwBoardGrid.grid2DArr[x][y])
                    if debugMode == true { print("\(mmwBoardGrid.grid2DArr[x][y].tileSprite.tileText) , x: \(x) y: \(y) ") }
                }
            }
        }
        return playedSpots
    }
    
    
    func playerPass () {
        
        playerPassButtonOff()
        
        newTilesButtonOff()

        mmwGameScene.optionsButton.userInteractionEnabled = true
        mmwGameScene.optionsButton.alpha = 0.5
        
        mmwGameScene.pauseButton.userInteractionEnabled = true
        mmwGameScene.pauseButton.alpha = 0.5
        
        if mmwGameSceneViewController.lettersPlayedInTurn == 0 {
            mmwGameSceneViewController.consecutivePasses += 1
        }

        if mmwGameSceneViewController.playerArray[ mmwGameSceneViewController.playerTurn - 1].isHuman == true {

            if mmwGameSceneViewController.consecutivePasses >= mmwGameSceneViewController.numPlayers {
                //print("all players passed on turn")
                view?.presentScene(mmwOptionScreen)
                mmwOptionScreen.allPlayersPassed()
            }
            else {
                delay(0.5) {
                    self.changePlayerTurn()
                }
            }
        }
        else {
            delay(0.5) {
                self.changePlayerTurn()
            }
        }
    }
    
    
    func newTilesButton(newTilesButtonNode: SKNode) {
        if userInteractionEnabled == true {
            newTilesButtonOff()

            mmwGameScene.optionsButton.userInteractionEnabled = true
            mmwGameScene.optionsButton.alpha = 0.5
            
            playerPassButtonOff()
            newTilesButtonOff()
            
            mmwGameScene.pauseButton.userInteractionEnabled = true
            mmwGameScene.pauseButton.alpha = 0.5


    
            if mmwGameSceneViewController.audioOn == true {
                runAction(actionSound)
            }
    
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.dealGridFromArrayRandom(&tileCollection!.mmwTileArray, numTilesToDeal: 6, playerNum: (mmwGameSceneViewController.playerTurn), clearGrid: true)
            
            showTilesInSquares(tileCollection!) // 'deals' player tiles and shows demo tiles on board for testing
            
            mmwGameSceneViewController.resetConsequtivePasses()
            
            
            
            delay(1.0) {
                self.changePlayerTurn()
            }
        }
    }
    
    
    func optionsButton (optionsButtonNode: SKNode) {

        stopTimer()
        
        gameGrid.hidden = true
        playAreaWhite.hidden = true
        
        view?.presentScene(mmwOptionScreen)
        
        if debugMode == true { print("back to  mmwGameScene from optionsButton") }
        
        if debugMode == true { print("mmwGameScenegameViewController func optionsButton()  --> gameViewController.updateUIDeluxeVersion()") }
        gameViewController.updateUIDeluxeVersion()
        
    }
    
    
    func pauseButton (pauseButtonNode : SKNode) {
        if userInteractionEnabled == true {
            //runAction(actionSound)
            if !gameIsPaused {
                stopTimer()
            }
            else {
                startTimer(secondsLeft)
            }
        }
    }
    
    func pauseGame () {
        mmwGameSceneViewController.appPaused = true
        //runAction(actionSound)
        if !gameIsPaused {
            stopTimer()
        }
        else {
            startTimer(secondsLeft)
        }

        
    }
    
    func resumeGame () {
        mmwGameSceneViewController.appPaused = false
        //runAction(actionSound)
        if !gameIsPaused {
            startTimer(secondsLeft)
        }
        else {
            stopTimer()
            
        }

    }
    
    
    
    
    func startTimer(seconds: Int) {
        //timeRemainingLabel.hidden = false
        if mmwGameSceneViewController.secondsPerTurn  == 999 {
            mmwGameSceneViewController.timerIsOn = false
        } else {
            mmwGameSceneViewController.timerIsOn = true
        }
        
        if mmwGameSceneViewController.timerIsOn == false {
            timeRemainingLabel.text = "Timer: off"
            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
            pauseButton.alpha = 0.5
        } else {
            gameIsPaused = false
            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
            if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].isHuman == true {
                pauseButton.alpha = 1.0
            }
            else {
                pauseButton.alpha = 0.5
            }
            
            
            
//                    let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
//                    dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in
//                        print("Currently dispatched thread asynchronously 0 playBtnPlay playAILetters")
            
            
            
            timeRemainingLabel.text = "Timer: \(seconds)"
            timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: #selector(MMWGameScene.updateCounter), userInfo: nil, repeats: true)

            
            
//                        dispatch_async(dispatch_get_main_queue(), {
//                            print("hello from playBtnPlay playAILetters thread executed as dispatch")
//                        })
//                    })
        }
    }
    
    
    func stopTimer() {
        gameIsPaused = true
        pauseButton.texture = SKTexture(imageNamed: "ResumeButton.png")
        if (timer != nil) {
            timer!.invalidate()
            timer = nil
        }
    }
    
    
    func explosion(pos: CGPoint, color: UIColor) {
        
        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
        dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in
            if debugMode == true { print("Currently dispatched thread asynchronously 0 explosion") }
            
            
            let emitterNode = SKEmitterNode(fileNamed: "MagicParticle.sks")
            emitterNode?.zPosition = 100
            emitterNode!.particlePosition = pos
            
            emitterNode!.particleColorSequence = nil
            emitterNode!.particleColorBlendFactor = 1.0
            emitterNode!.particleColor = color
            
            self.addChild(emitterNode!)
            self.runAction(SKAction.waitForDuration(2), completion: { emitterNode!.removeFromParent() })
            
            
            dispatch_async(dispatch_get_main_queue(), {
                if debugMode == true { print("hello from explosion() thread executed as dispatch") }
            })
        })
    }
    
    
    func changePlayerTurn () {
        
        if mmwGameSceneViewController.appPaused == false {
            
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.dealGridFromArrayRandom(&tileCollection!.mmwTileArray, numTilesToDeal: 6, playerNum: (mmwGameSceneViewController.playerTurn), clearGrid: false)
            
            showTilesInSquares(tileCollection!) // 'deals' player tiles and shows demo tiles on board for testing
            
            let tilesPlayedOnBoard = self.mmwBoardGrid.convert2DGridToArray(mmwBoardGrid.grid2DArr)
            
            for tile in tilesPlayedOnBoard! {
                tile.tileSprite.tileGlow.hidden = true
            }
            
            let oldPlayer = mmwGameSceneViewController.playerTurn - 1  // player array is 0 based, players are 1 through 4
            mmwGameSceneViewController.playerArray[oldPlayer].playerLetterGrid.makeTilesInGridInteractive(false)
            
            if oldPlayer < mmwGameSceneViewController.numPlayers - 1 {
                mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewEndTurn()
                mmwGameSceneViewController.playerTurn += 1
                mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()
                
                if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].isHuman == true {
                    mmwGameScene.optionsButton.userInteractionEnabled = false
                    mmwGameScene.optionsButton.alpha = 1.0
                    playerPassButtonOn()
                    
                    mmwGameScene.pauseButton.userInteractionEnabled = false
                    mmwGameScene.pauseButton.alpha = 0.5
                    if tileCollection!.mmwTileArray.count > 0 {
                        newTilesButtonOn()
                    }
                    else { newTilesButtonOff() }
                }
                    
                else {
                    mmwGameScene.optionsButton.userInteractionEnabled = true
                    mmwGameScene.optionsButton.alpha = 0.5
                    playerPassButtonOff()
                    
                    newTilesButtonOff()
                    mmwGameScene.pauseButton.userInteractionEnabled = true
                    mmwGameScene.pauseButton.alpha = 0.5
                }
            }
                
            else {
                mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewEndTurn()
                mmwGameSceneViewController.playerTurn = 1
                mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()
                
                if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].isHuman == true {
                    mmwGameScene.optionsButton.userInteractionEnabled = false
                    mmwGameScene.optionsButton.alpha = 1.0
                    playerPassButtonOn()
                    
                    if tileCollection!.mmwTileArray.count > 0 {
                        newTilesButtonOn()
                    }
                    else { newTilesButtonOff() }
                    mmwGameScene.pauseButton.userInteractionEnabled = false
                    mmwGameScene.pauseButton.alpha = 0.5
                }
                    
                else {
                    mmwGameScene.optionsButton.userInteractionEnabled = true
                    mmwGameScene.optionsButton.alpha = 0.5
                    playerPassButtonOff()
                    
                    newTilesButtonOff()
                    mmwGameScene.pauseButton.userInteractionEnabled = true
                    mmwGameScene.pauseButton.alpha = 0.5
                }
            }
            
            if tileCollection!.mmwTileArray.count > 0 {
                tilesRemainingLabel.text = "Tiles: \(tileCollection!.mmwTileArray.count)"
            }
            else {
                tilesRemainingLabel.text = "Tiles: 00"
            }
            
            timeRemainingLabel.color = UIColor.whiteColor()
            
            if mmwGameSceneViewController.timerIsOn {
                stopTimer()
                secondsLeft = mmwGameSceneViewController.secondsPerTurn
                if gameIsPaused == true {
                    gameIsPaused = false
                }
                startTimer(secondsLeft)
            }
            
            mmwGameSceneViewController.lettersPlayedInTurn = 0
            
            if mmwGameSceneViewController.audioOn == true {
                runAction(SKAction.playSoundFileNamed("endTurn.wav", waitForCompletion: false))
            }

            delay (0.5) {
                if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].isHuman == false {  // is AI so autoplay letters in rack
                    self.playBtnPlay()
                }
                else if mmwGameSceneViewController.consecutivePasses >= mmwGameSceneViewController.numPlayers {
                    if debugMode == true { print("all players passed on turn") }
                }
            }
        }
    }
    
    
    func getSnapGrid (testSpot : CGPoint) -> Grid? {
        var snapGrid : Grid = mmwBoardGrid
        
        if ((testSpot.x > (viewSize.width * 0.1533)) && (testSpot.x < 869.5)) {snapGrid = mmwBoardGrid}
            
        else if mmwGameSceneViewController.numPlayers == 2 {
            if (testSpot.x < (viewSize.width * 0.1509) ) {snapGrid = mmwPlayer1Grid}
            if (testSpot.x > (viewSize.width * 0.8491) ) {snapGrid = mmwPlayer2Grid}
        }
        else if mmwGameSceneViewController.numPlayers == 3 {
            if (testSpot.x < (viewSize.width * 0.1509) ) {snapGrid = mmwPlayer1Grid}
            //if (testSpot.x < 154.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer1Grid}
            if (testSpot.x > (viewSize.width * 0.8491) ) && testSpot.y < (viewSize.height * 0.5) {snapGrid = mmwPlayer2Grid}
            if (testSpot.x > (viewSize.width * 0.8491) ) && testSpot.y > (viewSize.height * 0.5) {snapGrid = mmwPlayer3Grid}
        }
        else if mmwGameSceneViewController.numPlayers == 4 {
            if (testSpot.x < (viewSize.width * 0.1509) ) && testSpot.y < (viewSize.height * 0.5) {snapGrid = mmwPlayer1Grid}
            if (testSpot.x > (viewSize.width * 0.8491) ) && testSpot.y < (viewSize.height * 0.5) {snapGrid = mmwPlayer2Grid}
            if (testSpot.x > (viewSize.width * 0.8491) ) && testSpot.y > (viewSize.height * 0.5) {snapGrid = mmwPlayer3Grid}
            if (testSpot.x < (viewSize.width * 0.1509) ) && testSpot.y > (viewSize.height * 0.5) {snapGrid = mmwPlayer4Grid}
        }
        return snapGrid
    }
    
    
    func numEmptyTileSlotsInGrid (tileGrid: Grid) -> Int {
        var numEmpty = 0
        for arrayX in tileGrid.grid2DArr {
            for arrayY in arrayX {
                if arrayY.tileText == "!" {
                    numEmpty += 1
                }
            }
        }
        if debugMode == true { print("<MMWGameScene>numEmptyTileSlots ( = 0) in grid : \(numEmpty) " ) }
        return numEmpty
    }
    
    
    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        
    }
    
    
    func newTilesButtonOn () {
        self.newTilesButton.alpha = 1.0
        self.newTilesButton.userInteractionEnabled = false // ??? why false, seems backwards but works this way
    }
    
    
    func playerPassButtonOff () {
        mmwGameScene.passButton.userInteractionEnabled = true
        mmwGameScene.passButton.alpha = 0.5
    }
    
    
    func playerPassButtonOn () {
        mmwGameScene.passButton.userInteractionEnabled = false
        mmwGameScene.passButton.alpha = 1.0
    }
    
    
    func newTilesButtonOff () {
        self.newTilesButton.alpha = 0.5
        self.newTilesButton.userInteractionEnabled = true
    }
    
    
    func updatePartialWordFeedback (updatedText: String) {
        topDisplayLabel.text = updatedText
    }
    
    
    func updatePartialWordFeedback2(updatedText: String) {
        topDisplayLabel2.text = updatedText
    }
    
    
    func calculateOpenTileLocations (gridXStart: Int, gridYStart: Int) -> (leftOpenTileLocations: Int, rightOpenTileLocations: Int, upOpenTileLocations: Int, downOpenTileLocations: Int) {
        var gridTestX = gridXStart
        var gridTestY = gridYStart
        var numExistingLetterTilesInDirection = 0
        var leftOpenTileLocations = 0
        var rightOpenTileLocations = 0
        var upOpenTileLocations = 0
        var downOpenTileLocations = 0
        
        while gridTestX > 0 {
            gridTestX -= 1
            if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                numExistingLetterTilesInDirection += 1
            }
            leftOpenTileLocations = gridXStart - numExistingLetterTilesInDirection
        }
        gridTestX = gridXStart
        numExistingLetterTilesInDirection = 0
        
        while gridTestX < 14 {
            gridTestX += 1
            if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                numExistingLetterTilesInDirection += 1
            }
            rightOpenTileLocations = (14 - gridXStart) - numExistingLetterTilesInDirection
        }
        gridTestX = gridXStart
        numExistingLetterTilesInDirection = 0
        
        while gridTestY > 0 {
            gridTestY -= 1
            if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                numExistingLetterTilesInDirection += 1
            }
            upOpenTileLocations = gridYStart - numExistingLetterTilesInDirection
        }
        gridTestY = gridYStart
        numExistingLetterTilesInDirection = 0
        
        while gridTestY < 14 {
            gridTestY += 1
            if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                numExistingLetterTilesInDirection += 1
            }
            
            downOpenTileLocations = (14 - gridYStart) - numExistingLetterTilesInDirection
        }
        gridTestY = gridYStart
        numExistingLetterTilesInDirection = 0
        
        if debugMode == true { print("calculateOpenTileLocations (\(gridXStart), \(gridYStart)) : l:\(leftOpenTileLocations) r:\(rightOpenTileLocations) u:\(upOpenTileLocations) d:\(downOpenTileLocations)  calculateOpenTileLocations mmwGameScene" ) }
        
        return (leftOpenTileLocations, rightOpenTileLocations, upOpenTileLocations, downOpenTileLocations)
    }
    
    //////////////////////////////////
    
    func checkForValidWordsAI (gridXSpot: Int, gridYSpot: Int, permutationsToPlay : Set<String>, lookHorizontal : Bool) -> (allFoundPartialWordPlaysAtLockedTile: [[validAILetterPlay]], allFoundWholeWordPlaysAtLockedTile: [[validAILetterPlay]] ) {
    
        
        var playersTurn = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1]
        
        ////////////  TEST FOR ADJACENT TILE PARTIAL WORDS
        var tileArrayToPlay : [MMWTile] = [MMWTile]()
        var gridTestX = gridXSpot
        var gridTestY = gridYSpot
        var lockedBoardTilesArr = mmwGameScene.mmwBoardGrid.getArrayLetterTilesInGrid() // start at locked and move L/R and Up/Down
        var playerLetterTilesGrid = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid // num of tilesToPlay 0 to numTiles in player grid
        
        ///////////////// HORIZONTAL && VERTICAL
        var currentTestAILetterPlay : validAILetterPlay? = validAILetterPlay()
        var currentPossibleAILetterPlayArr = [validAILetterPlay]()
        var validPartialAILetterPlayArr    : [validAILetterPlay] = [validAILetterPlay]()
        var validWholeWordAILetterPlayArr  : [validAILetterPlay] = [validAILetterPlay]()
        
        var allFoundPartialWordPlaysAtLockedTile = [[validAILetterPlay]]()
        var allFoundWholeWordPlaysAtLockedTile   = [[validAILetterPlay]]()
        
        var foundPartialWordsNumberAtLockedTile : Int = 0
        var foundWholeWordsNumberAtLockedTile   : Int = 0
        var placedTiles = 0
        var existingPlayedTiles = 0
        var wordString = ""               // mmwGameSceneViewController.mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot].letterString
        
        let openTileLocations = calculateOpenTileLocations(gridXSpot, gridYStart: gridYSpot)
        
        let numEmptyLetterSpotsLeft  = openTileLocations.leftOpenTileLocations  // from 0 to num player tiles PLUS existing tiles MINUS numLettersRight AND more than grid x = 0
        let numEmptyLetterSpotsRight = openTileLocations.rightOpenTileLocations // from 0 to num player tiles PLUS existing tiles MINUS numLettersLeft  AND more than grid x = 0
        let numEmptyLetterSpotsUp    = openTileLocations.upOpenTileLocations    // from 0 to num player tiles PLUS existing tiles MINUS numLettersRight AND more than grid x = 0
        let numEmptyLetterSpotsDown  = openTileLocations.downOpenTileLocations  // from 0 to num player tiles PLUS existing tiles MINUS numLettersLeft  AND more than grid x = 0
        
        var numEmptyLetterSpotsLeftOrUp     = numEmptyLetterSpotsLeft           // from 0 to num player tiles PLUS existing tiles MINUS numLettersRight AND more than grid x = 0
        var numEmptyLetterSpotsRightOrDown  = numEmptyLetterSpotsRight          // from 0 to num player tiles PLUS existing tiles MINUS numLettersLeft  AND more than grid x = 0
        
        var shiftLeftOrUp = 0
        var shiftRightOrDown = 0
        
        var numTilesToPlayLeftOrUpMaxComplete  = 0 //(availableTiles.count <= numEmptyLetterSpotsLeft)  ? availableTiles.count : numEmptyLetterSpotsLeft // # tiles to play Left : 0 to < playerTilesArr && < numEmptyLetterSpotsLeft
        var numTilesToPlayRightOrDownMaxComplete = 0 //(availableTiles.count <= numEmptyLetterSpotsRight) ? availableTiles.count : numEmptyLetterSpotsRight // # tiles to play Left : 0 to < playerTilesArr && < numEmptyLetterSpotsLeft
        
        var testString = self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText // string starts with letter at valid locked tile location
        
        var playerLettersToTest      = ""
        var playerLettersToTestLeftOrUp  = ""
        var playerLettersToTestRightOrDown = ""
        
        var foundWholeWord = false
        
        // check horizontal pass +- x , gridToTest X
        var passNumber = 0
        if lookHorizontal == false { passNumber = 1 }
        
        var isHorizontalWord = true
        
        var gridValueToCheck = gridTestX
        
        if passNumber == 1 { // horizontal pass == 0 , vertical pass == 1
            isHorizontalWord = false
            gridValueToCheck = gridTestY
        }
        
        var gridToTest = gridTestX
        var tileShiftXLeftOrUp = -1
        var tileShiftYLeftOrUp = 0
        var tileShiftXRightOrDown = 1
        var tileShiftYRightOrDown = 0
        
        if passNumber == 1 {   // vertical pass
            gridToTest = gridTestY
            tileShiftXLeftOrUp = 0
            tileShiftYLeftOrUp = -1
            tileShiftXRightOrDown = 0
            tileShiftYRightOrDown = 1
            numEmptyLetterSpotsLeftOrUp    = numEmptyLetterSpotsUp  // from 0 to num player tiles PLUS existing tiles MINUS numLettersRight AND more than grid x = 0
            numEmptyLetterSpotsRightOrDown = numEmptyLetterSpotsDown
        }
        
        for playerLetters in permutationsToPlay {  // get all permutations of player letters based on number  //  if self.secondsLeft < 5 { break }
            
            
            if passNumber == 0 {
                if gridXSpot >= 1 && gridXSpot <= 13 {
                    if mmwBoardGrid.grid2DArr[gridXSpot - 1][gridYSpot].tileType == TileType.Letter &&  mmwBoardGrid.grid2DArr[gridXSpot + 1][gridYSpot].tileType == TileType.Letter {
                        return ( allFoundPartialWordPlaysAtLockedTile , allFoundWholeWordPlaysAtLockedTile )
                    }
                }
            }
            if passNumber == 1 {
                if gridYSpot >= 1 && gridYSpot <= 13 {
                    if mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot - 1].tileType == TileType.Letter &&  mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot + 1].tileType == TileType.Letter {
                        return ( allFoundPartialWordPlaysAtLockedTile , allFoundWholeWordPlaysAtLockedTile )
                    }
                }
            }
            
            
            var lettersPlayedInPermutation = 0
            
            numTilesToPlayLeftOrUpMaxComplete    = (numEmptyLetterSpotsLeftOrUp  <= playerLetters.characters.count) ? numEmptyLetterSpotsLeftOrUp  : playerLetters.characters.count
            numTilesToPlayRightOrDownMaxComplete = (numEmptyLetterSpotsRightOrDown <= playerLetters.characters.count) ? numEmptyLetterSpotsRightOrDown : playerLetters.characters.count
            
            shiftLeftOrUp = numTilesToPlayLeftOrUpMaxComplete
            shiftRightOrDown = 0 // numTilesToPlayRightOrDownMaxComplete
            
            for shiftToRightOrDown in 0..<playerLetters.characters.count {
                
                while shiftRightOrDown <= numTilesToPlayRightOrDownMaxComplete {
                    playerLettersToTest            = playerLetters                          // Strings of letters in Player Tile 2D Array
                    playerLettersToTestLeftOrUp    = playerLetters                          // Strings of letters to play LEFT of base location
                    playerLettersToTestRightOrDown = playerLetters                          // Strings of letters to play RIGHT of base location
                    var numTilesToPlayLeftOrUpMax  = numTilesToPlayLeftOrUpMaxComplete      // most possible tile slots to left of base location
                    var numTilesToPlayRightOrDownMax = numTilesToPlayRightOrDownMaxComplete // most possible tile slots to right of base location
                    
                    shiftRightOrDown +=  (playerLettersToTestLeftOrUp.characters.count - numTilesToPlayLeftOrUpMax)
                    
                    if playerLettersToTestLeftOrUp.characters.count > 0 && shiftRightOrDown <= playerLetters.characters.count {
                        let rangeLeftOrUp : Range! = playerLettersToTestLeftOrUp.endIndex.advancedBy(-shiftRightOrDown)..<playerLettersToTestLeftOrUp.endIndex
                        playerLettersToTestLeftOrUp.removeRange(rangeLeftOrUp)          // make copy of letters to manipulate and remove from
                    }
                    if playerLettersToTestRightOrDown.characters.count > 0 && shiftRightOrDown <= playerLetters.characters.count {
                        let rangeRightOrDown : Range! = playerLettersToTestRightOrDown.startIndex..<playerLettersToTestRightOrDown.endIndex.advancedBy(-shiftRightOrDown)
                        playerLettersToTestRightOrDown.removeRange(rangeRightOrDown)    // make copy of letters to manipulate and remove from
                    }
                    // CHECK LEFT  /  OR UP
                    
                    gridTestX = gridXSpot // reset test spot as it may have been moved to right or down in code below
                    gridTestY = gridYSpot
                    gridToTest = gridXSpot
                    if passNumber == 1  {
                        gridToTest = gridYSpot
                    }
                    
                    while playerLettersToTestLeftOrUp.characters.count > (0) && playerLettersToTestLeftOrUp.characters.count <= numTilesToPlayLeftOrUpMax {
                        
                        if (gridToTest>=1 && gridToTest<=14) && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileType != TileType.Letter {
                            
                            numTilesToPlayLeftOrUpMax -= 1
                            let permutationLeftOrUpLastLetter = String( playerLettersToTestLeftOrUp.removeAtIndex(playerLettersToTestLeftOrUp.endIndex.predecessor()) )
                            
                            currentTestAILetterPlay!.tileSpriteLetter = permutationLeftOrUpLastLetter
                            
                            currentTestAILetterPlay!.gridXEnd = gridTestX + tileShiftXLeftOrUp
                            currentTestAILetterPlay!.gridYEnd = gridTestY + tileShiftYLeftOrUp
                            
                            currentPossibleAILetterPlayArr.append(currentTestAILetterPlay!)
                            lettersPlayedInPermutation += 1
                            testString = permutationLeftOrUpLastLetter + testString
                            
                            if (gridToTest>=1) {
                                gridToTest -= 1
                                if passNumber == 0 {
                                    gridTestX -= 1
                                }
                                if passNumber == 1 {
                                    gridTestY -= 1
                                }
                            }
                            
                            while gridToTest>=1 && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileType == TileType.Letter {
                                let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileText)
                                testString = existingLetterToAdd + testString
                                
                                if (gridToTest>=1) {
                                    gridToTest -= 1
                                    if passNumber == 0 {
                                        gridTestX -= 1
                                    }
                                    if passNumber == 1 {
                                        gridTestY -= 1
                                    }
                                }
                                
                            }
                        }
                            
                        else {
                            while gridToTest>=1 && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileType == TileType.Letter {
                                let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileText)
                                testString = existingLetterToAdd + testString
                                
                                if (gridToTest>=1) {
                                    gridToTest -= 1
                                    if passNumber == 0 {
                                        gridTestX -= 1
                                    }
                                    if passNumber == 1 {
                                        gridTestY -= 1
                                    }
                                }
                            }
                        }
                    }
                    
                    // grab left letters if only right letters on full right shift
                    if gridToTest >= 1 &&  self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileType == TileType.Letter { // && playerLettersToTestLeft.characters.count == (0) {
                        gridTestX = gridXSpot // reset test spot as it was moved to left in code above
                        gridTestY = gridYSpot
                        
                        gridToTest = gridXSpot
                        if passNumber == 1  {
                            gridToTest = gridYSpot
                        }
                        
                        while gridToTest>=1 && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileType == TileType.Letter {
                            let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileText)
                            testString = existingLetterToAdd + testString
                            gridToTest -= 1
                            if passNumber == 0 {
                                gridTestX -= 1
                            }
                            if passNumber == 1 {
                                gridTestY -= 1
                            }
                        }
                    }
                    
                    // CHECK RIGHT / OR DOWN // grab existing letters to right even if no shift right characters to test
                    gridTestX = gridXSpot // reset test spot as it was moved to left in code above
                    gridTestY = gridYSpot
                    gridToTest = gridXSpot
                    if passNumber == 1  {
                        gridToTest = gridYSpot
                    }
                    
                    while gridToTest<14 && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileType == TileType.Letter {
                        let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileText)
                        //print("Added existing letter: \(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText) ")
                        testString = testString + existingLetterToAdd
                        gridToTest += 1
                        if passNumber == 0 {
                            gridTestX += 1
                        }
                        if passNumber == 1 {
                            gridTestY += 1
                        }
                    }
                    //////////  added && gridToTest<14 ????? to fix prible with y array going to 14 adding 1 and going off grid
                    while playerLettersToTestRightOrDown.characters.count > 0 && playerLettersToTestRightOrDown.characters.count <= numTilesToPlayRightOrDownMax && gridToTest < 14 {
                        if self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileType != TileType.Letter { // if NOT a letter then play a letter
                            let permutationRightOrDownFirstLetter = String(playerLettersToTestRightOrDown.removeAtIndex(playerLettersToTestRightOrDown.startIndex))
                            numTilesToPlayRightOrDownMax -= 1
                            
                            lettersPlayedInPermutation += 1
                            currentTestAILetterPlay!.tileSpriteLetter = permutationRightOrDownFirstLetter
                            currentTestAILetterPlay!.gridXEnd = gridTestX + tileShiftXRightOrDown
                            currentTestAILetterPlay!.gridYEnd = gridTestY + tileShiftYRightOrDown
                            
                            currentPossibleAILetterPlayArr.append(currentTestAILetterPlay!)
                            lettersPlayedInPermutation += 1
                            
                            testString = testString + permutationRightOrDownFirstLetter
                            gridToTest += 1
                            if passNumber == 0 {
                                gridTestX += 1
                            }
                            if passNumber == 1 {
                                gridTestY += 1
                            }
                            
                            // if IS a letter then add a letter(s)
                            while gridTestX<14 && gridTestY<14 && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileType == TileType.Letter {
                                let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileText)
                                //print("Added existing letter: \(mmwBoardGrid.grid2DArr[gridTestX+1][gridTestY].tileText) ")
                                testString = testString + existingLetterToAdd
                                
                                gridToTest += 1
                                if passNumber == 0 {
                                    gridTestX += 1
                                }
                                if passNumber == 1 {
                                    gridTestY += 1
                                }
                                //print (testString)
                            }
                        }
                            
                        else {
                            while self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileType == TileType.Letter {
                                let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileText)      //   ????? + 1 ???
                                //print("Added existing letter: \(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText) ")
                                testString = testString + existingLetterToAdd
                                gridToTest += 1
                                if passNumber == 0 {
                                    gridTestX += 1
                                }
                                if passNumber == 1 {
                                    gridTestY += 1
                                }
                            }
                        }
                    }
                    
                    if (mmwGameSceneViewController.checkPartialWordMatch(testString) == true) { //&& LetterTileSprite.testLetterAtDropSpot(currentTestAILetterPlay.gridXEnd, tileSnapResultsYGrid: currentTestAILetterPlay.gridYEnd, letterToTest: currentTestAILetterPlay.tileSpriteLetter).validHorizontalPartialWord == true && LetterTileSprite.testLetterAtDropSpot(currentTestAILetterPlay.gridXEnd, tileSnapResultsYGrid: currentTestAILetterPlay.gridYEnd, letterToTest: currentTestAILetterPlay.tileSpriteLetter).validVerticalPartialWord == true ) {
                        
                        if currentPossibleAILetterPlayArr.count > 0 {
                            if currentPossibleAILetterPlayArr[0].gridXEnd == -1 {
                                currentPossibleAILetterPlayArr.removeFirst()
                            }
                        }
                        
                        var allLettersPossibleWords = true
                        
                        var currentPossibleAILetterPlayArrIndex = 0
                        for letterPlaceholder in currentPossibleAILetterPlayArr {
                            
                            let isValidCurrentLetterAtDropSpot = LetterTileSprite.testLetterAtDropSpot(letterPlaceholder.gridXEnd, tileSnapResultsYGrid: letterPlaceholder.gridYEnd, letterToTest: letterPlaceholder.tileSpriteLetter)
                            
                            if (isValidCurrentLetterAtDropSpot.validHorizontalPartialWord == false) || (isValidCurrentLetterAtDropSpot.validVerticalPartialWord == false) {
                                allLettersPossibleWords = false
                            }
                            currentPossibleAILetterPlayArrIndex += 1
                        }
                        
                        if allLettersPossibleWords && currentPossibleAILetterPlayArr.count > 0 {
                            
                            validPartialAILetterPlayArr = currentPossibleAILetterPlayArr
                            
                            foundPartialWordsNumberAtLockedTile += 1
                            
                            allFoundPartialWordPlaysAtLockedTile.append(validPartialAILetterPlayArr)
                            
                            if (mmwGameSceneViewController.checkWholeWordMatch(testString) == true ) && testString.characters.count >= (mmwGameSceneViewController.minWordSize) {
                                // update existing placeholder data to reflect that tile made word
                                foundWholeWordsNumberAtLockedTile += 1
                                
                        
                                
                                if validWholeWordAILetterPlayArr.count > 0 {
                                    if validWholeWordAILetterPlayArr[0].gridXEnd == -1 {
                                        validWholeWordAILetterPlayArr.removeFirst()
                                    }
                                }
                                
                                for var placeHolderTile in validPartialAILetterPlayArr {
                                    placeHolderTile.madeValidWord = true
                                    placeHolderTile.partOfWord = testString
                                    validWholeWordAILetterPlayArr.append(placeHolderTile)
                                }
                                allFoundWholeWordPlaysAtLockedTile.append(validWholeWordAILetterPlayArr)
                                if debugMode == true { print("!!! allValidWholeWordTilePlayArr \(validWholeWordAILetterPlayArr) \(testString) checkForValidWordsAI mmwGameScene" ) }
                                
                                if  foundWholeWordsNumberAtLockedTile > 0 { return ( allFoundPartialWordPlaysAtLockedTile , allFoundWholeWordPlaysAtLockedTile ) }
                            }
                        }
                        else {
                            break
                        }
                    }
                    else {
                        if debugMode == true { print("NOT partial Word Match! \(testString) checkForValidWordsAI mmwGameScene" ) }
                    }
                    
                    if allFoundWholeWordPlaysAtLockedTile.count > 1 {
                        break // break early to speed AI play - can alter later to optimize to give AI choices of valid tile plays
                    }
                    
                    currentTestAILetterPlay = validAILetterPlay()
                    currentPossibleAILetterPlayArr  = [validAILetterPlay()]
                    
                    validPartialAILetterPlayArr = [validAILetterPlay()]
                    validWholeWordAILetterPlayArr = [validAILetterPlay()]
                    
                    lettersPlayedInPermutation = 0
                    tileArrayToPlay = [MMWTile]()
                    testString = self.mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot].tileText // get letter at start tile
                    shiftLeftOrUp -= 1
                    shiftRightOrDown += 1
                }
            }
            gridTestX = gridXSpot // reset test spot as it may have been moved to right in code below
            gridTestY = gridYSpot
        }
        
        if allFoundPartialWordPlaysAtLockedTile.count > 0 {
            if allFoundPartialWordPlaysAtLockedTile[0].count == 0 { allFoundWholeWordPlaysAtLockedTile.removeFirst() }
        }
        if allFoundWholeWordPlaysAtLockedTile.count > 0 {
            if allFoundWholeWordPlaysAtLockedTile[0].count   == 0 { allFoundWholeWordPlaysAtLockedTile.removeFirst() }
        }
        return ( allFoundPartialWordPlaysAtLockedTile , allFoundWholeWordPlaysAtLockedTile )
    }
    
    
    func playAIPlaceholderTilesToBoard (validWholeWordAILetterPlayArr: [validAILetterPlay] ) -> (Bool) {
        
        var placedTiles = 0
        
        for letter in validWholeWordAILetterPlayArr {
            
            let delayForNextTile : Double = Double(1.5 * Double(placedTiles) )
            
            delay(delayForNextTile) {
                
                var letterTilePlayable : MMWTile
                
                letterTilePlayable  = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid.getTileWithLetter(letter.tileSpriteLetter)!
                
                if letter.madeValidWord == true {
                    letterTilePlayable.tileState = TileState.PlayedMadeWord
                } else {
                    letterTilePlayable.tileState = TileState.Played
                }
                if debugMode == true { print("H updating letterTilePlayable.tileSprite.updateAIWordsAtDropSpot \(letter.tileSpriteLetter), \(letter.gridXEnd), \(letter.gridYEnd), \(letter.partOfWord), \(letterTilePlayable.tileState), \(letterTilePlayable.tileState.rawValue ) checkForValidWordsAI mmwGameScene") }
                letterTilePlayable.tileSprite.updateAIWordsAtDropSpot(letter.gridXEnd, tileYGridDestination: letter.gridYEnd, madeValidWord: letter.madeValidWord)
            }
            placedTiles += 1
        }
        return true
    }
    
    
    func subtractTilesRemainingOnBoard() {
        
        mmwGameScene.stopTimer()
        
        for playerView in playerViewArr {
            playerView.playerNameLabel.fontColor = UIColor.whiteColor()
            playerView.playerScoreLabel.fontColor = UIColor.whiteColor()
            playerView.playerTileCover.hidden = true
        }
        
        var tilesToSubtractBoard = 0
        for y in 0...(mmwBoardGrid.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(mmwBoardGrid.gridNumSquaresX - 1) {
                if mmwBoardGrid.grid2DArr[x][y].tileState == TileState.Played  {
                    tilesToSubtractBoard += 1
                    delay (Double (tilesToSubtractBoard) * 0.5 ) {

                        mmwGameScene.explosion(self.mmwBoardGrid.grid2DArr[x][y].tileSprite.position, color: tileColors[self.mmwBoardGrid.grid2DArr[x][y].tileOwner.rawValue])
                        
                        self.mmwBoardGrid.grid2DArr[x][y].tileSprite.showNegativeScoreTextToGridHome(-2)
                        mmwGameSceneViewController.playerArray[self.mmwBoardGrid.grid2DArr[x][y].tileOwner.rawValue - 1].playerScore -= 2
                        mmwGameScene.playerViewArr[self.mmwBoardGrid.grid2DArr[x][y].tileOwner.rawValue - 1].changePlayerScoreDisplay()
                        self.mmwBoardGrid.grid2DArr[x][y].tileSprite.alpha = 0.5
                    }
                }
            }
        }
        
        var tilesToSubtract = 0
        let gameGrids = [ mmwGameScene.mmwPlayer1Grid, mmwGameScene.mmwPlayer2Grid, mmwGameScene.mmwPlayer3Grid, mmwGameScene.mmwPlayer4Grid ]
        // remove all actions from tiles e.g. moving, scaling, etc
        var playerNum = 0
        for grid in gameGrids {
            // move letter tiles back to mmwTileArray and replace with placeholder tiles
            for tileRow in 0..<grid.grid2DArr.count {  //self.mmwBoardTile2DArray! {
                for tileCol in 0..<grid.grid2DArr[tileRow].count {
                    let tile = grid.grid2DArr[tileRow][tileCol]
                    if tile.tileType == TileType.Letter {
                        tilesToSubtract += 1
                        delay (Double (tilesToSubtract) * 0.5) {
                            mmwGameScene.explosion(tile.tileSprite.position, color: tileColors[tile.tileOwner.rawValue])
                            mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1].playerScore -= 2
                            tile.tileSprite.showNegativeScoreTextToGridHome(-2)
                            mmwGameScene.playerViewArr[tile.tileOwner.rawValue - 1].changePlayerScoreDisplay()
                            tile.tileSprite.alpha = 0.5
                        }
                        tilesToSubtract += 1
                    }
                }
            }
            playerNum += 1
        }
        
        let tileDelay = tilesToSubtractBoard >= tilesToSubtract ? tilesToSubtractBoard : tilesToSubtract
        
        delay ( (Double(tileDelay) * 0.5) +  5.0 ) {
            self.gameIsOver()
        }
    }
    
    
    func gameIsOver() {
        view?.presentScene(mmwOptionScreen)
        mmwGameScene.stopTimer()
        gameViewController.goToResults()
    }
    
    
    func permuteLetters(list: [String], minStringLen : Int, maxStringLen: Int) -> Set<String> {
        func permuteLetters(fromList: [String], toList: [String], minStringLen: Int, maxStringLen: Int, inout set: Set<String>) {
            if toList.count >= minStringLen && toList.count <= maxStringLen {
                set.insert(toList.joinWithSeparator(""))
            }
            if !fromList.isEmpty {
                for (index, item) in fromList.enumerate() {
                    var newFrom = fromList
                    newFrom.removeAtIndex(index)
                    permuteLetters(newFrom, toList: toList + [item], minStringLen: minStringLen, maxStringLen: maxStringLen, set: &set)
                }
            }
        }
        var set = Set<String>()
        permuteLetters(list, toList:[], minStringLen: minStringLen, maxStringLen: maxStringLen, set: &set)
        return set
    }
    
    
    func presentResultsScene() {
        //        let transition = SKTransition.crossFadeWithDuration(0.5)
        //        let mmwResultsScene = MMWResultsScene(size: size,
        //            gameResult: true,
        //            score: 123)
        //        view?.presentScene(mmwResultsScene, transition: transition)
        view?.presentScene(mmwOptionScreen)
    }
}