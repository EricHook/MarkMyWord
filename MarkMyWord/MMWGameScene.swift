
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

//struct validAITilePlay {
//    var tile : MMWTile = MMWTile()
//    var gridXEnd: Int = -1
//    var gridYEnd: Int = -1
//    var partOfWord = ""
//    var madeValidWord = false
//}

struct validAILetterPlay {
    var tileSpriteLetter : String = ""
    var gridXEnd: Int = -1
    var gridYEnd: Int = -1
    var partOfWord = ""
    var madeValidWord = false
}


class MMWGameScene : SKScene { // , NSObject, NSCoding { // , SKPhysicsContactDelegate {
    
    var viewSize = screenSize!
    var backgroundNode   : SKSpriteNode = SKSpriteNode(imageNamed: "MarkMyWordBGCleaniPad.png" )
    var newGameSpriteNode   : SKSpriteNode = SKSpriteNode(imageNamed: "NewGameScreen.png" )
    var optionsLayerNode : SKSpriteNode = SKSpriteNode(imageNamed: "MMWOptionsScreen.png")
    var foregroundNode   : SKSpriteNode = SKSpriteNode()
    
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
    
    //var timer = NSTimer()
//    private var timer : NSTimer?
    //private var timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
//    private var secondsLeft = mmwGameSceneViewController.secondsPerTurn
//    var timeRemainingLabel  = SKLabelNode(fontNamed: FontHUDName)
    var tilesRemainingLabel = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel     = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel2    = SKLabelNode(fontNamed: FontHUDName)
    var bottomDisplayLabel  = SKLabelNode(fontNamed: FontHUDName)
    
    let playButton     = SKSpriteNode(imageNamed: "PlayButton.png")
    var newTilesButton = SKSpriteNode(imageNamed: "NewTilesButton.png")
    let passButton     = SKSpriteNode(imageNamed: "PassButton.png")
//    let pauseButton    = SKSpriteNode(imageNamed: "PauseButton.png")
    let optionsButton  = SKSpriteNode(imageNamed: "OptionsButton.png")
    var gameGrid       = SKSpriteNode(imageNamed: "GameGrid.png")
    //
    //    let smallSquare    = SKSpriteNode(imageNamed: "SmallSquare.png")
    
    var placeholderTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    
    let actionSound      = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    let dealTilesSound   = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
    let destroyTileSound = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
    //let backgroundMusic = SKAction.playSoundFileNamed("30Showdown.m4a", waitForCompletion: true)
    
    private var isPaused: Bool = false
    
    var foundValidWordOnTurn = false
    var animationsTimedOut = false
    
    
    enum MMWErrorType: ErrorType {
        case MMWError
        case MMWError2(lettersNeeded: Int)
    }
    
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    
    override init(size: CGSize) {
        super.init(size: size)
    }
    
    
    func resetPassCounter () {
        mmwGameSceneViewController.consecutivePasses = 0
        print("\(mmwGameSceneViewController.consecutivePasses) consecutive passes")
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
            gridBoardUpperLeftY = Double(viewSize.height * 0.087)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            gridBoardUpperLeftY = Double(viewSize.height * 0.0882)
        }
        
        self.mmwBoardGrid = Grid(gridUpperLeftX: gridBoardUpperLeftX, gridUpperLeftY: gridBoardUpperLeftY, gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 15, gridNumSquaresY: 15, gridName: "mmwBoardGrid")

        self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.272), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer1Grid")
        self.mmwPlayer1Grid.setGridPlayer(mmwGameSceneViewController.player1)
        
        self.mmwPlayer2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.272), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer2Grid")
        self.mmwPlayer2Grid.setGridPlayer(mmwGameSceneViewController.player2)
        
        self.mmwPlayer3Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.705), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer3Grid")
        self.mmwPlayer3Grid.setGridPlayer(mmwGameSceneViewController.player3)
        
        self.mmwPlayer4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.705), gridSquareSizeX: gridSquareSize, gridSquareSizeY: gridSquareSize, gridNumSquaresX: 3, gridNumSquaresY: 2, gridName: "mmwPlayer4Grid")
        self.mmwPlayer4Grid.setGridPlayer(mmwGameSceneViewController.player4)
        
        if mmwGameSceneViewController.numPlayers == 2 {
            self.mmwPlayer1Grid.gridUpperLeftX = Double(viewSize.width * 0.0058)
            self.mmwPlayer1Grid.gridUpperLeftY = Double(viewSize.height * 0.4581)
            
            self.mmwPlayer2Grid.gridUpperLeftX = Double(viewSize.width * 0.8550)
            self.mmwPlayer2Grid.gridUpperLeftY = Double(viewSize.height * 0.4581)
        }
        
        if mmwGameSceneViewController.numPlayers == 3 {
            self.mmwPlayer1Grid.gridUpperLeftX = Double(viewSize.width * 0.0058)
            self.mmwPlayer1Grid.gridUpperLeftY = Double(viewSize.height * 0.4581)
            
            self.mmwPlayer2Grid.gridUpperLeftX = Double(viewSize.width * 0.8550)
            self.mmwPlayer2Grid.gridUpperLeftY = Double(viewSize.height * 0.2727)
            
            self.mmwPlayer3Grid.gridUpperLeftX = Double(viewSize.width * 0.8550)
            self.mmwPlayer3Grid.gridUpperLeftY = Double(viewSize.height * 0.7057)
        }
    }
    
    func changePlayerScoreDisplay (playerView: PlayerView, player: Player, score: Int) {
        player.playerScore = score
        playerView.playerScoreLabel.text = String(score)
    }
    
    
    func buildGameView () {
        print("---   in buildGameView mmwGameScene")
        gameViewController.ViewAllOptionsUI.hidden = true
        userInteractionEnabled = true
        
        setGrids() // sets tile grid positions, size square, number squares and position on screen for each grid possible

        tileCollection!.resetAllTiles()
        
        //        // ADD ALL TILES to Scene - they start as invisible
        //        var tileTempXLocation = 0
        //        for tile in tileCollection!.mmwTileArray {
        //            tile.tileSprite.hidden = false
        //            tileTempXLocation += 40
        //            tile.tileSprite.tileLocation = CGPoint(x: tileTempXLocation, y: 15 )
        //            self.addChild(tile.tileSprite)
        //        }
        
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: size.width/2.0, y: 0.0)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.zPosition = -100
        backgroundNode.size.height = viewSize.height
        backgroundNode.size.width  = viewSize.width
        if self.childNodeWithName("backgroundNode") == nil {
            addChild(backgroundNode)
        }

        newGameSpriteNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        newGameSpriteNode.position = CGPoint(x: size.width/2.0, y: 0.0)
        newGameSpriteNode.userInteractionEnabled = false
        newGameSpriteNode.hidden = true
        newGameSpriteNode.zPosition = 100
        newGameSpriteNode.size.height = viewSize.height
        newGameSpriteNode.size.width  = viewSize.width
        addChild(newGameSpriteNode)

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

        
//        if mmwGameSceneViewController.playerArray[0].isHuman == false {
//            mmwGameSceneViewController.playerArray[0].playerView.playerTex = SKTexture(imageNamed:gameViewController.meyamaImageArray[3])
//        }
//        else {
//            mmwGameSceneViewController.playerArray[0].playerView.playerTex = SKTexture(imageNamed: gameViewController.playerImageArray[3])
//        }
        
        
        
        //        playerViewArr.append(player2View)
        //        if mmwGameSceneViewController.numPlayers > 2 {
        //            player3View = updatePlayerView(3, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player3))
        //            playerViewArr.append(player3View)
        //        }
        //        if mmwGameSceneViewController.numPlayers > 3 {
        //            player4View = updatePlayerView(4, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player4))
        //            playerViewArr.append(player4View)
        //        }
        
        
        //        addChild(foregroundNode) // generic SKNode to separate foreground elements from background
        
        backgroundNode.zPosition = -50
        
        // Set GameGrid Size
        if mmwGame.deviceType == MMWGame.DeviceType.iPhone5 || mmwGame.deviceType == MMWGame.DeviceType.iPhone6 {
            //gameGrid = SKSpriteNode(imageNamed: "GameGrid@iPadPro.png")
            gameGrid.xScale = 0.412
            gameGrid.yScale = 0.412
            FontHUD = FontHUDiPhone
        }
            
        else if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus {
            //gameGrid = SKSpriteNode(imageNamed: "GameGrid@iPadPro.png")
            gameGrid.xScale = 0.533
            gameGrid.yScale = 0.533
            FontHUD = FontHUDiPhone
            for playerView in playerViewArr {
                playerView.position.x += 13
            }
            for playerGrid in playerGridArray {
                playerGrid.gridUpperLeftX += 8.5
            }
        }
            
        else if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            //gameGrid = SKSpriteNode(imageNamed: "GameGrid@iPadPro.png")
            gameGrid.xScale = 1.33
            gameGrid.yScale = 1.33
        }

//        gameGrid.position = CGPoint(x: (viewSize.width/2), y: (viewSize.height * 0.490 ) )
//        gameGrid.name = "gameGrid"
//        //gameGrid.userInteractionEnabled = false
//        gameGrid.zPosition = -49
//        gameGrid.alpha     = 0.1
//        self.addChild(gameGrid)

        topDisplayHUD("Welcome to Mark My Word") // ("Turn: Player 1, Special Letter Bonus In Effect, 2x Point Bonus")
        topDisplayHUD2("topDisplayHUD2")
        
        tilesRemainingHUD(tileCollection!.mmwTileArray.count)

        bottomDisplayHUD("Begin ... ")
        
        //timeRemainingLabel.text = "Timer: 00"
//        timeRemainingHUD(mmwGameSceneViewController.secondsPerTurn)  // default set to         standard time remaining
//        isPaused = true
//        if (timer != nil) {
//            timer!.invalidate()
//        }

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
        
//        pauseButton.position = CGPoint(x: (viewSize.width * 0.9238), y: (viewSize.height * 0.125) ) // width old 0.9228
//        pauseButton.name = "pauseButton"
//        pauseButton.userInteractionEnabled = true
//        self.addChild(pauseButton)
        
        optionsButton.position = CGPoint(x: (viewSize.width * 0.9238), y: (viewSize.height * 0.05) )
        optionsButton.name = "optionsButton"
        optionsButton.userInteractionEnabled = true
        self.addChild(optionsButton)
        
        let buttonResizeArr = [playButton, newTilesButton, passButton, optionsButton] // [playButton, newTilesButton, passButton, pauseButton, optionsButton]
        
        if mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
            for button in buttonResizeArr {
                button.size.width  = button.size.width  * 0.4 // 0.5
                button.size.height = button.size.height * 0.4 // 0.5
                //                timeRemainingLabel
                //                tilesRemainingLabel
                topDisplayLabel.position.y -= 0.15
                topDisplayLabel2.position.y -= 0.4
                //                bottomDisplayLabel
            }
            
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus {
            for button in buttonResizeArr {
                button.size.width  = button.size.width  * 0.4 // 0.5
                button.size.height = button.size.height * 0.4
            }
        }
            
        else if mmwGame.deviceType == MMWGame.DeviceType.iPad {
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
        
        // ADD ALL TILES to Scene - they start as invisible
        var tileTempXLocation = 0
        
        for tile in tileCollection!.mmwTileArray {
            tile.tileSprite.hidden = true
            tileTempXLocation += 40
            tile.tileSprite.tileLocation = CGPoint(x: tileTempXLocation, y: 15 )
            self.addChild(tile.tileSprite)
        }
        
//        optionsLayerNode.name = "optionsLayerNode"
//        optionsLayerNode.anchorPoint = CGPoint(x: 0.5, y: 0.5)
//        optionsLayerNode.position = CGPoint(x: size.width/2.0, y: size.height/2.0)
//        optionsLayerNode.userInteractionEnabled = false
//        optionsLayerNode.zPosition = 100
//        optionsLayerNode.size = CGSize(width: (viewSize.width * 1.0),  height: (viewSize.height * 1.0) )
//        optionsLayerNode.alpha = 0.95
//        optionsLayerNode.hidden = true
//        self.addChild(optionsLayerNode)
        
        //        // Placeholder for dictionary data loading progress display
        //        let playBtnTEMP = SKSpriteNode(imageNamed: "PlayButton.png")
        //        playBtnTEMP.position = CGPoint(x: viewSize.width/4, y: viewSize.height/6)
        //        self.addChild(playBtnTEMP)
        //        playBtnTEMP.name = "playBtnTEMP"
        //        playBtnTEMP.zPosition = 100
        //        playBtnTEMP.anchorPoint = CGPointMake(0, 0)
        //        let scaleHoriz = SKAction.scaleXTo(2, duration: 3.0)
        //        let loadingAnim = SKAction.group([scaleHoriz])
        //        let loadingAnimSequence = SKAction.sequence([loadingAnim, SKAction.removeFromParent()])
        //        playBtnTEMP.runAction(loadingAnimSequence)
        
        // Placeholder for AI play
        let playBtnPlay = SKSpriteNode(imageNamed: "PlayButton.png")
        playBtnPlay.position = CGPoint(x: viewSize.width * 0.0, y: viewSize.height * 0.00)
        playBtnPlay.anchorPoint = CGPointMake(0, 0)
        playBtnPlay.name = "playBtnPlay"
        playBtnPlay.zPosition = 100
        playBtnPlay.size = CGSizeMake(40.0, 40.0)
        self.addChild(playBtnPlay)
        newTilesButton.userInteractionEnabled = true
        
        // Placeholder for Test Code
        let testPlayButton = SKSpriteNode(imageNamed: "PlayButton.png")
        testPlayButton.position = CGPoint(x: viewSize.width * 0.0, y: viewSize.height * 1.0)
        //testPlayButton.position = CGPoint(x: 50, y: 50)
        testPlayButton.anchorPoint = CGPointMake(0, 1)
        testPlayButton.name = "testPlayButton"
        testPlayButton.zPosition = 100
        testPlayButton.size = CGSizeMake(40.0, 40.0)
        self.addChild(testPlayButton)
        
        
        //        var logoFrame = CGRectMake(0,0,118,40)
        //        self.addChild(logoFrame)
        //        var imageView = UIImageView(frame: logoFrame)
        //        imageView.image = UIImage(named:"Logo")
        //        self.view!.addSubview(imageView)
        
        
        // buttons inactive until "playButton" is pressed
        newTilesButton.userInteractionEnabled = true
        newTilesButton.alpha = 0.5
        passButton.userInteractionEnabled = true
        passButton.alpha = 0.5
//        pauseButton.userInteractionEnabled = true
//        pauseButton.alpha = 0.5
        optionsButton.userInteractionEnabled = true
        optionsButton.alpha = 0.5
    }
    
    
    func resetGameView () {
        print("---   in resetGameView mmwGameScene")
        gameViewController.ViewAllOptionsUI.hidden = true

        // update locations and visibility of player views
        player1View = updatePlayerView(1, playerView: player1View)
        player2View = updatePlayerView(2, playerView: player2View)
        player3View = updatePlayerView(3, playerView: player3View)
        player4View = updatePlayerView(4, playerView: player4View)

//        if mmwGameSceneViewController.timerIsOn == false {
//            timeRemainingLabel.text = "Timer: off"
//        } else {
//            timeRemainingLabel.text = "Timer: \(mmwGameSceneViewController.secondsPerTurn) "
//        }
        
        bottomDisplayLabel.text =  "Begin ... "
        topDisplayLabel.text = "Welcome to Mark My Word"
        topDisplayLabel2.text =  ""
        
        tileCollection!.resetAllTiles()
    }
    
    
    func showAllGridTiles (gridToDisplay: Grid) {
        print("---   in showAllGridTiles mmwGameScene Grid: \(gridToDisplay) #\(gridToDisplay) ")
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
                
                if gridToDisplay.grid2DArr[x][y].tileType == TileType.Letter {letterCount++}
            }
        }
        print("---   in showAllGridTiles mmwGameScene Count: #\(letterCount) ")
    }
    
    func showTilesInSquares (tileBuilder: MMWTileBuilder) { // shows all tiles from TileBuilder [MMWTile] arrays
        print("---   in showTilesInSquares mmwGameScene")
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
        
        print("---   in showTilesInSquares mmwGameScene mmwDiscardedTileArray.count: #\(tileCollection?.mmwDiscardedTileArray.count)) ")
        print("---   in showTilesInSquares mmwGameScene mmwTileArray.count: #\(tileCollection?.mmwTileArray.count)) ")
        print("---   in showTilesInSquares mmwGameScene mmwTileArrayPlaceholder: #\(tileCollection?.mmwTileArrayPlaceholder.count)) ")
    }
    
    
//    func timeRemainingHUD (timeAmount: Int)  -> SKLabelNode {
//        timeRemainingLabel.zPosition = 1
//        timeRemainingLabel.fontSize = FontHUDSize
//        timeRemainingLabel.fontColor = FontHUDWhite
//        timeRemainingLabel.position = CGPointMake(size.width * 0.76, size.height * 0.004)
//        timeRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
//        
////        if secondsLeft == 999 {
////            timeRemainingLabel.text =  "Timer: off"
////        }
////        else {
////            if mmwGameSceneViewController.timerIsOn {
////                timeRemainingLabel.text =  "Timer: \(String(secondsLeft)) " // "Timer: \(timeAmount)"
////                startTimer(timeAmount)
////            }
////            else {
////                timeRemainingLabel.text =  "Timer: off"
////            }
////        }
//        addChild(timeRemainingLabel)
//        return timeRemainingLabel
//    }
    
    func updateCounter() {
        if mmwGameSceneViewController.timerIsOn {
//            timeRemainingLabel.fontColor = SKColor(red: 1, green: 1, blue: 1, alpha: 1) // counter starts white color
//            timeRemainingLabel.text = "Timer: \(String(secondsLeft) ) " // String(counter--)
//            
//                    if secondsLeft <= 10 && secondsLeft > 5 {
//                        timeRemainingLabel.text =  "Timer: \( String(secondsLeft) ) "
//                        let countdownSmall = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
//                        timeRemainingLabel.fontColor = SKColor(red: 0, green: 1, blue: 1, alpha: 1) // counter turns orange color
//                        runAction(SKAction.sequence( [countdownSmall]) )
//                    }
//                    if secondsLeft <= 5 {
//                        timeRemainingLabel.text =  "Timer: \( String(secondsLeft) ) "
//                        let countdownBig = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
//                        runAction(SKAction.sequence( [countdownBig]) )
//                        timeRemainingLabel.fontColor = SKColor(red: 1, green: 0, blue: 0, alpha: 1) // counter turns red color
//                        if secondsLeft <= 0 {
//                            //runAction(actionSound)   // play turn over sound
//                            timeRemainingLabel.text = ("Timer: 0")
//                            changePlayerTurn()
//                        }
//                    }
//            
//            --secondsLeft
        }
    }
    
    func tilesRemainingHUD (tilesLeft : Int) -> SKLabelNode {
        tilesRemainingLabel.zPosition = 1
        tilesRemainingLabel.text = "Tiles Left: \(tileCollection!.mmwTileArray.count  )" // "Tiles Left: \(tilesLeft)"
        tilesRemainingLabel.fontSize = FontHUDSize
        tilesRemainingLabel.position = CGPointMake(size.width * 0.160, size.height * 0.004)
        tilesRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(tilesRemainingLabel)
        return tilesRemainingLabel
    }
    
    func bottomDisplayHUD (letters : String)  -> SKLabelNode {        // bottom text display area - first line
        bottomDisplayLabel.zPosition = 1
        bottomDisplayLabel.text = "\(letters)"
        bottomDisplayLabel.fontSize = FontHUDSize
        bottomDisplayLabel.position = CGPointMake(size.width/2.0, size.height * 0.004)
        bottomDisplayLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(bottomDisplayLabel)
        return bottomDisplayLabel
    }
    
    func topDisplayHUD (message : String)  -> SKLabelNode {         // top text display area - first line
        topDisplayLabel.zPosition = 1
        topDisplayLabel.text = "\(message)"
        topDisplayLabel.fontSize = FontHUDSize
        topDisplayLabel.position = CGPointMake(size.width/2.0, self.size.height * 0.980) // CGPointMake(size.width/2.0, 753.0) // 1 of 2 top lines
        topDisplayLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayLabel)
        return topDisplayLabel
    }
    
    func topDisplayHUD2 (message : String)  -> SKLabelNode {        // top text display area - second line
        topDisplayLabel2.zPosition = 1
        topDisplayLabel2.text = message
        topDisplayLabel2.fontSize = FontHUDSize
        topDisplayLabel2.position = CGPointMake(size.width/2.0, (self.size.height * 0.959) )// 2 of 2 top lines CGPointMake(size.width/2.0, 735.0)
        topDisplayLabel2.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayLabel2)
        return topDisplayLabel2
    }
    
    
    func updatePlayerView (playerNum : Int, playerView : PlayerView) -> PlayerView {
        print("---   in updatePlayerView mmwGameScene")

        let isHuman = playerView.mmwPlayer.isHuman
        let meyamaImageNumber = playerView.mmwPlayer.playerMeyamaNumber
        let avatarImageNumber = playerView.mmwPlayer.playerAvatarNumber
        
        if  isHuman == false {
            playerView.playerSprite.texture  = SKTexture(imageNamed:gameViewController.meyamaImageArray[meyamaImageNumber])
            playerView.playerNameLabel.text = gameViewController.meyamaAvatarNames[meyamaImageNumber]
        }
        else {
            playerView.playerSprite.texture = SKTexture(imageNamed:gameViewController.playerImageArray[avatarImageNumber])
            playerView.playerNameLabel.text = gameViewController.playerAvatarNames[avatarImageNumber]
        }
        
        if mmwGameSceneViewController.numPlayers == 2 { // if 2 one left and one right
            if playerNum == 1 {
                playerView.position = CGPointMake(0, self.size.height * 0.3853 )
                playerView.hidden = false
                //playerView.playerViewBeginTurn()
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(self.size.width * 0.8495, self.size.height * 0.3853 )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
        }
        
        if mmwGameSceneViewController.numPlayers == 3 { // if 3 one left two right
            if playerNum == 1 {
                playerView.position = CGPointMake(0, self.size.height * 0.3853 )
                playerView.hidden = false
                //playerView.playerViewBeginTurn()
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(viewSize.width * 0.8495, viewSize.height * 0.571 )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
            if playerNum == 3 {
                playerView.position = CGPointMake(viewSize.width * 0.8495,  viewSize.height * 0.138 )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
        }
        
        if mmwGameSceneViewController.numPlayers == 4 { // if 4 two left two right
            if playerNum == 1 {
                playerView.position = CGPointMake( 0,  viewSize.height * 0.571 )
                playerView.hidden = false
                playerView.playerViewBeginTurn()
                //playerView.playerViewEndTurn()
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(viewSize.width * 0.8495, viewSize.height * 0.571 )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
            if playerNum == 3 {
                playerView.position = CGPointMake(viewSize.width * 0.8495,  viewSize.height * 0.138 )
                playerView.hidden = false
                //playerView.playerViewEndTurn()
            }
            if playerNum == 4 {
                playerView.position = CGPointMake(0, viewSize.height * 0.138 )
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
            
            if(_node.name == "optionsLayerNode"){
                //if userInteractionEnabled {
                //runAction(actionSound)
                print(">>> optionsLayerNode PRESSED >>>")
                optionsLayerNode.hidden = true
            }
            
            if(_node.name == "playButton"){
                playButton(_node)
            }
            
            if(_node.name == "newTilesButton"){
                newTilesButton(_node)
            }
            
            if(_node.name == "passButton"){
                passButton(_node)
            }
            
            if(_node.name == "optionsButton"){
                optionsButton (_node)
            }
//            
//            if(_node.name == "pauseButton"){
//                pauseButton (_node)
//            }
            
            if(_node.name == "playBtnPlay"){
                playBtnPlay (_node)
            }
            if(_node.name == "testPlayButton"){
                testPlayButton ()
            }
        }
    }
    
    func playButton (buttonNode: SKNode) {
        if userInteractionEnabled {
            print(">>> PLAY BUTTON PRESSED >>>")
            startGame()  // get starter words, set buttons, get timles, show tiles, set timer
            buttonNode.removeFromParent() // gets rid of play button in middle of screen
        }
    }
    
    func dealStarterWords() {
        print("---   in startGame mmwGameScene")
        var starterWord0 = mmwGameSceneViewController.getRandomWord()
        while mmwGameSceneViewController.checkTilesForWord(starterWord0, letterTileArray: tileCollection!.mmwTileArray) == false {
            starterWord0 = mmwGameSceneViewController.getRandomWord()
        }
        var starterWord0TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord0, letterTileArray: &tileCollection!.mmwTileArray)
        
        var starterWord1 = mmwGameSceneViewController.getRandomWord()
        while mmwGameSceneViewController.checkTilesForWord(starterWord1, letterTileArray: tileCollection!.mmwTileArray) == false {
            starterWord1 = mmwGameSceneViewController.getRandomWord()
        }
        var starterWord1TileArray : [MMWTile]
        
        var starterWord2 = mmwGameSceneViewController.getRandomWord()
        while (starterWord2 == starterWord1) || (mmwGameSceneViewController.checkTilesForWord(starterWord2, letterTileArray: tileCollection!.mmwTileArray ) == false )  {
            starterWord2 = mmwGameSceneViewController.getRandomWord()
        }
        var starterWord2TileArray : [MMWTile]
        //        var starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord2, letterTileArray: &tileCollection!.mmwTileArray)
        
        //        var starterWord3 = mmwGameSceneViewController.getRandomWord()
        //        while (starterWord3 == starterWord2) || (starterWord3 == starterWord1) || (mmwGameSceneViewController.checkTilesForWord(starterWord3, letterTileArray: tileCollection!.mmwTileArray ) == false )  {
        //            starterWord3 = mmwGameSceneViewController.getRandomWord()
        //        }
        //        var starterWord3TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord3, letterTileArray: &tileCollection!.mmwTileArray)
        //
        //        var starterWord4 = mmwGameSceneViewController.getRandomWord()
        //        while (starterWord4 == starterWord3) || (starterWord4 == starterWord2) || (starterWord4 == starterWord1) || (mmwGameSceneViewController.checkTilesForWord(starterWord4, letterTileArray: tileCollection!.mmwTileArray ) == false )  {
        //            //print ("new pass on obtain starter word 4 \(starterWord4)")
        //            starterWord4 = mmwGameSceneViewController.getRandomWord()
        //        }
        //        var starterWord4TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord4, letterTileArray: &tileCollection!.mmwTileArray)
        
        // SENDS RANDOM WORD(S) TO CENTER OF BOARD // half of the time Horizontal and the rest Vertical
        
        //            /////////////////////////////////////////////////////
        //
        //            mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 2, yStartSquare: 2, IsHorizonal: false, player: mmwGameSceneViewController.player0)
        //            mmwGameSceneViewController.sendWordToBoard(&starterWord2TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 4, yStartSquare: 3, IsHorizonal: false, player: mmwGameSceneViewController.player0)
        //            mmwGameSceneViewController.sendWordToBoard(&starterWord3TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 6, yStartSquare: 5, IsHorizonal: true, player: mmwGameSceneViewController.player0)
        //            mmwGameSceneViewController.sendWordToBoard(&starterWord4TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 2, yStartSquare: 14, IsHorizonal: true, player: mmwGameSceneViewController.player0)
        //
        //            /////////////////////////////////////////////////////
        
        if arc4random_uniform(100) > 50 { // VERTICAL
            if mmwGameSceneViewController.numStarterWords == 1 {
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 7, yStartSquare: 5, IsHorizonal: false, player: mmwGameSceneViewController.player0)
            }
            
            if mmwGameSceneViewController.numStarterWords == 2 {
                starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)!
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 3, yStartSquare: 3, IsHorizonal: false, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray, gridToDisplay: mmwBoardGrid, xStartSquare: 11, yStartSquare: 7, IsHorizonal: false, player: mmwGameSceneViewController.player0)
            }
            
            if mmwGameSceneViewController.numStarterWords == 3 {
                starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)!
                
                starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord2, letterTileArray: &tileCollection!.mmwTileArray)!
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 3, yStartSquare: 2, IsHorizonal: false, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray, gridToDisplay: mmwBoardGrid, xStartSquare: 11, yStartSquare: 2, IsHorizonal: false, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord2TileArray, gridToDisplay: mmwBoardGrid, xStartSquare: 7, yStartSquare: 8, IsHorizonal: false, player: mmwGameSceneViewController.player0)
            }
            
        }
            
        else { // HORIZONTAL
            if mmwGameSceneViewController.numStarterWords == 1 {
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 5, yStartSquare: 7, IsHorizonal: true, player: mmwGameSceneViewController.player0)
            }
            
            if mmwGameSceneViewController.numStarterWords == 2 {
                starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)!
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 3, yStartSquare: 3, IsHorizonal: true, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray, gridToDisplay: mmwBoardGrid, xStartSquare: 7, yStartSquare: 11, IsHorizonal: true, player: mmwGameSceneViewController.player0)
            }
            
            if mmwGameSceneViewController.numStarterWords == 3 {
                starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &tileCollection!.mmwTileArray)!
                
                starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord2, letterTileArray: &tileCollection!.mmwTileArray)!
                
                mmwGameSceneViewController.sendWordToBoard(&starterWord0TileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 2, yStartSquare: 3, IsHorizonal: true, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord1TileArray, gridToDisplay: mmwBoardGrid, xStartSquare: 8, yStartSquare: 7, IsHorizonal: true, player: mmwGameSceneViewController.player0)
                mmwGameSceneViewController.sendWordToBoard(&starterWord2TileArray, gridToDisplay: mmwBoardGrid, xStartSquare: 2, yStartSquare: 11, IsHorizonal: true, player: mmwGameSceneViewController.player0)
            }
        }
        
    }
    
    
    // get starter words, set buttons, get timles, show tiles, set timer
    func startGame () {
        
        print("---   in resetGameView mmwGameScene")
        gameViewController.ViewAllOptionsUI.hidden = true
        
        tileCollection!.resetAllTiles()
        
        // update locations and visibility of player views
        player1View = updatePlayerView(1, playerView: player1View)
        
        player2View = updatePlayerView(2, playerView: player2View)
        
        player3View = updatePlayerView(3, playerView: player3View)
        
        player4View = updatePlayerView(4, playerView: player4View)

        
        // buttons inactive until "playButton" is pressed
        newTilesButton.userInteractionEnabled = false
        newTilesButton.alpha = 1
        passButton.userInteractionEnabled = false
        passButton.alpha = 1
//        pauseButton.userInteractionEnabled = false
//        pauseButton.alpha = 1
        optionsButton.userInteractionEnabled = false
        optionsButton.alpha = 1

        
//        if mmwGameSceneViewController.timerIsOn == false {
//            secondsLeft = 999
//            timeRemainingLabel.text = "Timer: off"
//            pauseButton.userInteractionEnabled = false
//            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
//            //pauseButton.texture = SKTexture(imageNamed: "MMWResumeButton.png")
//            pauseButton.alpha = 0.4
//            playButton.hidden = true
//        } else {
//            secondsLeft = mmwGameSceneViewController.secondsPerTurn
//            timeRemainingLabel.text = "Timer: \(secondsLeft) "
//            isPaused = false
//            //pauseButton.texture = SKTexture(imageNamed: "MMWResumeButton.png")
//            startTimer()
//        }
        
        bottomDisplayLabel.text =  "Begin ... "
        topDisplayLabel.text = "Welcome to Mark My Word"
        topDisplayLabel2.text =  ""
        
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
        
        tilesRemainingLabel.text = "Tiles Left: \(tileCollection!.mmwTileArray.count)"
        
        // set player turn to storing in mmwGameSceneViewController
        mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()

        for player in mmwGameSceneViewController.playerArray {
            player.playerScore = 0
            player.playerView!.changePlayerScoreDisplay(player.playerScore)
            player.playerView!.playerViewEndTurn()
        }
        player1View.playerViewBeginTurn()
        
        animationsActive(false)
        
        //playerTex = SKTexture(imageNamed: "avatar000.png")
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
            }
            playerArrayNumber++
        }
        
    }
    
    func testPlayButton () {
        var row = 0
        var col = 0
        for tileRow in mmwGameScene.mmwPlayer1Grid.grid2DArr {
            for tile in tileRow {
                print("Tile \(tile.tileSprite.tileText) \(row), \(col) ")
                tile.tileSprite.tileText = "X"
                //tile.tileSprite.hidden = true
                tile.resetTileValues()
                col++
            }
            row++
            col = 0
        }
        
        print("testPlayButton ()")
    }
    
    
    func playBtnPlay (playButtonPlayNode: SKNode) {

        playAILetters()
        
        mmwGameSceneViewController.resetConsequtivePasses()
        
        //changePlayerTurn()
    }
    
    func playAILetters() {

        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
        dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in
            
            print("Currently dispatched thread asynchronously 0 playBtnPlay playAILetters")
            
            //get tile from current player
            //print("Player tiles to play \(self.mmwGameSceneViewController.playerArray[self.mmwGameSceneViewController.playerTurn - 1].playerLetterGrid ) ")
            //self.mmwGameSceneViewController.playerArray[self.mmwGameSceneViewController.playerTurn-1].playerLetterGrid.printGrid()
            
            //lookForPlayedSpots()
            //var validWholeWords = 0
            let lockedTilesArr = self.mmwBoardGrid.getArrayLockedLetterTilesInGrid()
            //var possibleWordPlays : [[validAILetterPlay]] = [[validAILetterPlay]]()
            //var validWordPlays    : [[validAILetterPlay]] = [[validAILetterPlay]]()
            var numLockedTilesSearched = 0
            var checkForValidWordsAISearchedTRUE = 0
            
            let playerLetterTilesArr = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.getArrayLetterTilesInGrid() // num of tilesToPlay 0 to numTiles in player grid
            var playerTilesLettersArr : [String] = [String]()  // the playerTilesArr in corresponding Letter array
            
            for tile in playerLetterTilesArr {                    // prints out letters in player tiles
                print("\(tile.tileText) ", terminator: "")
                playerTilesLettersArr.append(tile.tileText) // adds letters in player tiles to playerTilesLettersArr[]
            };  print("")      // to add a return on last letter
            
            let numLettersToPlayMin = 1
            var numLettersToPlayMax = 6
            
            var allPossiblePartialWordPlays = [[validAILetterPlay]]()
            var allPossibleWholeWordPlays = [[validAILetterPlay]]()
            
            
            if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerSkillLevel == 0 {
                numLettersToPlayMax = 6
            }
            if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerSkillLevel == 1 {
                numLettersToPlayMax = 6
            }
            if mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerSkillLevel == 2 {
                numLettersToPlayMax = 6
            }
            
            let permutationsToPlay : Set<String> = self.permuteLetters(playerTilesLettersArr, minStringLen: numLettersToPlayMin, maxStringLen: numLettersToPlayMax)
            
            for lockedTile in lockedTilesArr {
                numLockedTilesSearched++
                print("looking at tile \(lockedTile.gridXEnd), \(lockedTile.gridYEnd)" )
                // checks for playable valid whole word in player tiles
                ////////////////////////
                var tileAIWordCheck = self.checkForValidWordsAI(lockedTile.gridXEnd, gridYSpot: lockedTile.gridYEnd, permutationsToPlay: permutationsToPlay, lookHorizontal: true)
                
                if (tileAIWordCheck.allFoundPartialWordPlaysAtLockedTile.count > 0) {
                    
                    allPossiblePartialWordPlays += tileAIWordCheck.allFoundPartialWordPlaysAtLockedTile
                    
                    if (tileAIWordCheck.allFoundWholeWordPlaysAtLockedTile.count > 0) {
                        
                        allPossibleWholeWordPlays += tileAIWordCheck.allFoundWholeWordPlaysAtLockedTile
                        print("AI found \(allPossiblePartialWordPlays.count )  \(allPossibleWholeWordPlays.count ) at lockedTile \(lockedTile.gridX), \(lockedTile.gridX) Horizontal")
                    }
                    
                    //mmwGameScene.animationsActive(true)
                    
                    //                    delay(12.0){  // animationTime +
                    //                        mmwGameScene.animationsActive(false)
                    //                    }
                    
                    //                    checkForValidWordsAISearchedTRUE++
                    //                    break
                }
                
                
                tileAIWordCheck = self.checkForValidWordsAI(lockedTile.gridXEnd, gridYSpot: lockedTile.gridYEnd, permutationsToPlay: permutationsToPlay, lookHorizontal: false)
                
                if (tileAIWordCheck.allFoundPartialWordPlaysAtLockedTile.count > 0) {
                    
                    allPossiblePartialWordPlays += tileAIWordCheck.allFoundPartialWordPlaysAtLockedTile
                    
                    if (tileAIWordCheck.allFoundWholeWordPlaysAtLockedTile.count > 0) {
                        
                        allPossibleWholeWordPlays += tileAIWordCheck.allFoundWholeWordPlaysAtLockedTile
                        
                        print("AI found P:\(allPossiblePartialWordPlays.count )  W:\(allPossibleWholeWordPlays.count ) at lockedTile \(lockedTile.gridX), \(lockedTile.gridX) Horizontal")
                    }
                    
                    //                    mmwGameScene.animationsActive(true)
                    
                    //                    delay(12.0){  // animationTime +
                    //                        mmwGameScene.animationsActive(false)
                    //                    }
                    
                    //                    checkForValidWordsAISearchedTRUE++
                    //                    break
                }
            }
            
            // placedTiles = playAIPlaceholderTilesToBoard ( allFoundWholeWordPlays[0] )
            
            // if (self.checkForValidWordsAI(lockedTile.gridXEnd, gridYSpot: lockedTile.gridYEnd, numLettersToPlayMin: numLettersToPlayMin, numLettersToPlayMax: numLettersToPlayMax, permutationsToPlay: permutationsToPlay).hasValidWord) == true {
            
            ////////////////////////
            
            //}
            // check for playable partial word (if low AI or no valid word)
            
            //                    if (self.checkForValidWordsAI(lockedTile.gridXEnd, gridYSpot: lockedTile.gridYEnd, numLettersToPlayMin: numLettersToPlayMin, numLettersToPlayMax: numLettersToPlayMax, permutationsToPlay: permutationsToPlay).   .hasValidWord) == true  {
            //                        checkForValidWordsAISearchedTRUE++
            //                        break
            //                    }
            //}
            
            print(">> numLockedTilesSearched : \(numLockedTilesSearched++), checkForValidWordsAISearchedTRUE : \(checkForValidWordsAISearchedTRUE), allPossiblePartialWordPlays: \(allPossiblePartialWordPlays.count), allPossibleWholeWordPlays: \(allPossibleWholeWordPlays.count) playBtnPlay loadWords() mmwGameScene")
            
            
            if allPossibleWholeWordPlays.count >= 1 {
                self.playAIPlaceholderTilesToBoard (allPossibleWholeWordPlays[0])
            }
            
            // Change turns if player has no letter tiles remaining
            let letterTilesRemaining = mmwGameSceneViewController.playerArray[(mmwGameSceneViewController.playerTurn) - 1].playerLetterGrid.numLetterTilesInGrid()
            if letterTilesRemaining <= 0 {
                self.changePlayerTurn()
            }
            
            
            
            
            dispatch_async(dispatch_get_main_queue(), {
                print("hello from playBtnPlay playAILetters thread executed as dispatch")
            })
        })
        print("hello from playBtnPlay playAILetters thread")
        
    }
    
    
    
    //        //get tile from current player
    //        print("Player tiles to play \(mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid ) ")
    //        mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid.printGrid()
    //
    //        //lookForPlayedSpots()
    //        var validWholeWords = 0
    //        let lockedTilesArr = mmwBoardGrid.getArrayLockedLetterTilesInGrid()
    //        var possibleWordPlays : [[validAILetterPlay]] = [[validAILetterPlay]]()
    //        var validWordPlays : [[validAILetterPlay]] = [[validAILetterPlay]]()
    //
    //        for lockedTile in lockedTilesArr {
    //            if checkForValidWordsAI(lockedTile.gridXEnd , gridYSpot: lockedTile.gridYEnd,  availableTiles: [MMWTile()], numLettersToPlayMin: 2, numLettersToPlayMax: 4) == true {
    //                break
    //            }
    //        }
    //    }
    
    //            for word in possibleWordPlays {
    //                print(word)
    //                var isValidWholeWord = true
    //
    //                for letter in word {
    //                    var letterTileTest = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid.getTileWithLetter(letter.tileSpriteLetter)
    //                    var validWordTestAtDropSpot = letterTileTest?.tileSprite.testForValidWordsAtDropSpot(letter.gridXEnd, tileSnapResultsYGrid: letter.gridYEnd, isAI: true)
    //
    //                    if validWordTestAtDropSpot!.validHorizontalPartialWord == false || validWordTestAtDropSpot!.validVerticalPartialWord == false {
    //                        isValidWholeWord = false
    //                        break
    //                    }
    //                }
    //
    //                if isValidWholeWord == true && word.count > 3 {
    //                    print("Found word : \(word)")
    //                    //validWordPlays.append(word)
    //
    //                    var numLetters = 0.0
    //                    for letter in word {
    //                        var letterTilePlayable : MMWTile
    //                        letterTilePlayable  = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid.getTileWithLetter(letter.tileSpriteLetter)!
    //                        letterTilePlayable.gridEnd = mmwBoardGrid
    //                        letterTilePlayable.gridXEnd = letter.gridXEnd
    //                        letterTilePlayable.gridYEnd = letter.gridYEnd
    //                        letterTilePlayable.gridXTest = letter.gridXEnd
    //                        letterTilePlayable.gridYTest = letter.gridYEnd
    //
    //                        //letterTilePlayable.tileSprite.updateWordsAtDropSpot2(letter.gridXEnd, tileSnapResultsYGrid: letter.gridYEnd)  // , isWholeWord: true)()
    //                        letterTilePlayable.tileSprite.updateAIWordsAtDropSpot(letter.gridXEnd, tileYGridDestination: letter.gridYEnd)
    //
    //                        numLetters++
    //                        //break
    //                    }
    //                    if word.count > 2 { validWholeWords++ }
    //                    isValidWholeWord = true
    //                }
    //
    //                if isValidWholeWord == true { break }
    //
    //                mmwGameSceneViewController.resetConsequtivePasses()
    //                // Change turns if player has no letter tiles remaining
    //                let letterTilesRemaining = mmwGameSceneViewController.playerArray[(mmwGameSceneViewController.playerTurn) - 1].playerLetterGrid.numLetterTilesInGrid()
    //                if letterTilesRemaining <= 0 {
    //                    self.changePlayerTurn()
    //                }
    //            }
    
    //        if validWholeWords > 1 {
    //            var numLetters = 0.0
    //            for letter in validWordPlays.last! {
    //                var letterTilePlayable : MMWTile
    //
    //                letterTilePlayable  = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid.getTileWithLetter(letter.tileSpriteLetter)!
    //
    //                letterTilePlayable.gridEnd = mmwBoardGrid
    //                letterTilePlayable.gridXEnd = letter.gridXEnd
    //                letterTilePlayable.gridYEnd = letter.gridYEnd
    //                letterTilePlayable.gridXTest = letter.gridXEnd
    //                letterTilePlayable.gridYTest = letter.gridYEnd
    
    /////////////////////////////////////////
    //                let seconds = numLetters
    //                let delay = seconds * Double(NSEC_PER_SEC)  // nanoseconds per seconds
    //                let dispatchTime = dispatch_time(DISPATCH_TIME_NOW, Int64(delay))
    //
    //                dispatch_after(dispatchTime, dispatch_get_main_queue(), {
    //                    // here code perfomed with delay
    //                    letterTilePlayable.tileSprite.updateWordsAtDropSpot2(letter.gridXEnd, tileSnapResultsYGrid: letter.gridYEnd)
    //                    numLetters++
    //                })
    /////////////////////////////////////////
    //                func myPerformeCode(timer : NSTimer) {
    //                    // here code to perform
    //                    letterTilePlayable.tileSprite.updateWordsAtDropSpot2(letter.gridXEnd, tileSnapResultsYGrid: letter.gridYEnd)  // , isWholeWord: true)()
    //                    //letterTilePlayable.tileSprite.updateAIWordsAtDropSpot(letter.gridXEnd, tileYGridDestination: letter.gridYEnd)
    //                    numLetters++
    //                }
    //                let myTimer : NSTimer = NSTimer.scheduledTimerWithTimeInterval(numLetters, target: self, selector: Selector("myPerformeCode:"), userInfo: nil, repeats: false)
    /////////////////////////////////////////
    //                let delay = SKAction.waitForDuration( Double(numLetters) )
    //                letterTilePlayable.tileSprite.runAction( delay ) {
    //                    //run code here after 5 secs
    //
    //                    //letterTilePlayable.tileSprite.updateWordsAtDropSpot2(letter.gridXEnd, tileSnapResultsYGrid: letter.gridYEnd)  // , isWholeWord: true)()
    //                    letterTilePlayable.tileSprite.updateAIWordsAtDropSpot(letter.gridXEnd, tileYGridDestination: letter.gridYEnd)
    //                    numLetters++
    //                }
    /////////////////////////////////////////
    //                letterTilePlayable.tileSprite.updateWordsAtDropSpot2(letter.gridXEnd, tileSnapResultsYGrid: letter.gridYEnd)  // , isWholeWord: true)()
    //                //letterTilePlayable.tileSprite.updateAIWordsAtDropSpot(letter.gridXEnd, tileYGridDestination: letter.gridYEnd)
    //                numLetters++
    /////////////////////////////////////////
    //            }
    //
    //            mmwGameSceneViewController.resetConsequtivePasses()
    //            // Change turns if player has no letter tiles remaining
    //            let letterTilesRemaining = mmwGameSceneViewController.playerArray[(mmwGameSceneViewController.playerTurn) - 1].playerLetterGrid.numLetterTilesInGrid()
    //            if letterTilesRemaining <= 0 {
    //                self.changePlayerTurn()
    //            }
    //        }
    //    }
    
    //        var dictionaryController = UIReferenceLibraryViewController.init(term: "test")
    //        var dictionary = UIReferenceLibraryViewController.dictionaryHasDefinitionForTerm("word")
    //        print( " UIReferenceLibraryViewController.dictionaryHasDefinitionForTerm(\"word\")  \(dictionary)")
    //        print( " UIReferenceLibraryViewController.dictionaryHasDefinitionForTerm(\"wordqq\")  \(UIReferenceLibraryViewController.dictionaryHasDefinitionForTerm("word")) " )
    
    //class func dictionaryHasDefinitionForTerm(_ term: String) -> Bool
    
    //        if ( var dictionary = UIReferenceLibraryViewController.dictionaryHasDefinitionForTerm("word") ) {
    //            UIReferenceLibraryViewController.init(term: "word")
    //            UIReferenceLibraryViewController.presentViewController(dictionaryController)(
    //        }
    
    //}
    
    func lookForPlayedSpots() -> ([MMWTile]){
        var playedSpots = [MMWTile]()
        var numLetterTiles = 0
        for y in 0...(mmwBoardGrid.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(mmwBoardGrid.gridNumSquaresX - 1) {
                if mmwBoardGrid.grid2DArr[x][y].tileType == TileType.Letter {
                    numLetterTiles++
                    playedSpots.append(mmwBoardGrid.grid2DArr[x][y])
                    print("\(mmwBoardGrid.grid2DArr[x][y].tileSprite.tileText) , x: \(x) y: \(y) ")
                }
            }
        }
        return playedSpots
    }
    
    //    func lookForPlayableSpots() -> ([playableLockedTile]){
    //        var playableLockedTile : (tile: MMWTile, left: Int, right: Int, up: Int, down: Int) = (0, 0, 0, 0) // number of free/playble spots in each direction from existing playable tile
    //        var playableSpots = [MMWTile]()
    //        var numLetterTiles = 0
    //        for y in 0...(mmwBoardGrid.gridNumSquaresY - 1) {   // fill letter tiles
    //            for x in 0...(mmwBoardGrid.gridNumSquaresX - 1) {
    //                if mmwBoardGrid.grid2DArr[x][y].tileType == TileType.Letter {
    //                    numLetterTiles++
    //                    playableSpots.append(mmwBoardGrid.grid2DArr[x][y])
    //                    print("\(mmwBoardGrid.grid2DArr[x][y].tileSprite.tileText) , x: \(x) y: \(y) ")
    //                }
    //            }
    //        }
    //        return playableSpots
    //    }
    
    
    func passButton (passButtonNode: SKNode) {
        if userInteractionEnabled {
            if mmwGameSceneViewController.lettersPlayedInTurn == 0 {
                mmwGameSceneViewController.consecutivePasses += 1
            }
            
            print("\(mmwGameSceneViewController.consecutivePasses) consecutive passes")
            if mmwGameSceneViewController.consecutivePasses >= mmwGameSceneViewController.numPlayers {
                print("CHECK TO END GAME > PASSES = \(mmwGameSceneViewController.consecutivePasses)")
                
                //view?.presentScene(mmwGameScene)
                
                view?.presentScene(endGameScene)
                
                //self.view.presentScene(endGameScene)
                
            }
            //                  runAction(actionSound)
            changePlayerTurn()
            //            if isPaused == true {
            //                startTimer(mmwGameSceneViewController.secondsPerTurn)
            //            }
        }
    }
    
    func newTilesButton(newTilesButtonNode: SKNode) {
        if userInteractionEnabled == true {
            
            //runAction(actionSound)
            
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.dealGridFromArrayRandom(&tileCollection!.mmwTileArray, numTilesToDeal: 6, playerNum: (mmwGameSceneViewController.playerTurn), clearGrid: true)
            
            showTilesInSquares(tileCollection!) // 'deals' player tiles and shows demo tiles on board for testing
            
            mmwGameSceneViewController.resetConsequtivePasses()
            
            changePlayerTurn()
        }
    }
    
    func animationsActive (isActive: Bool) {
        if isActive {
            optionsButton.alpha = 0.5
            optionsButton.userInteractionEnabled = true
        }
        else {
            optionsButton.alpha = 1.0
            optionsButton.userInteractionEnabled = false
        }
    }
    
    func optionsButton (optionsButtonNode: SKNode) {
        var readyToRestart  = false
        
        print("options button pressed")
        
        view?.presentScene(mmwOptionScreen)
        
        print("back to  mmwGameScene from optionsButton")
        
        delay (0) {
            print ("!!!!!!!!!!!!!!!!!!!!!!!!  delay 0 optionsButton DELAY ready to restart")
            readyToRestart = true
            //self.animationsTimedOut = true
            
        }
        
        //        while readyToRestart == false {
        //            print("false")
        //        }
        
        //optionsLayerNode.hidden = false  // pop up graphic ... click to hide//if userInteractionEnabled {
        
        //runAction(actionSound)
        
        
//        stopTimer()
        
        
        
        //viewOptions.alpha = 1.0
        
        //        print("tileCollection.displayTileArrayValues(tileCollection.mmwDiscardedTileArray)")
        //        tileCollection!.displayTileArrayValues(tileCollection!.mmwDiscardedTileArray)
        //        print("tileCollection.displayTileArrayValues(tileCollection.mmwTileArray)")
        //        tileCollection!.displayTileArrayValues(tileCollection!.mmwTileArray)
        //
        //        mmwPlayer1Grid.printGrid()
        //        mmwPlayer2Grid.printGrid()
        //        mmwPlayer3Grid.printGrid()
        //        mmwPlayer4Grid.printGrid()
        //
        //        mmwBoardGrid.printGrid()
        //        for tile in (tileCollection?.mmwDiscardedTileArray)! {
        //            print  ("   \(tile.tileText) ")
        //        }
        
        //        var readyToRestart = false
        //        print("option button pressed")
        //
        //        delay (15) {
        //            print ("!!!!!!!!!!!!!!!!!!!!!!!!  delay 15 optionsButton DELAY ready to retart")
        //            readyToRestart = true
        //        }
        //
        //
        //        //        let transition = SKTransition.  .crossFadeWithDuration(0.2)
        //        //        view?.presentScene(mmwOptionScreen, transition: transition)
        //        view?.presentScene(mmwOptionScreen)
        //        print(">>>   view?.presentScene(mmwOptionScreen)")
        //
        //
        //
        ////        while readyToRestart == false {
        ////
        ////        }
        //        newGameSpriteNode.hidden = true
        //
        //
        //        let playBtn = SKSpriteNode(imageNamed: "PlayButton.png")
        //        playBtn.position = CGPoint(x: screenSize!.width/3, y: screenSize!.height/3)
        //        self.addChild(playBtn)
        //        playBtn.name = "playBtn"
        //        playBtn.zPosition = 100
        //        playBtn.anchorPoint = CGPoint(x: 0, y: 0)
        //        playBtn.xScale = 0
        //        let scaleAction = SKAction.scaleXTo(1.5, y: 1.0, duration: 20)
        //        playBtn.runAction(scaleAction)
        //        playBtn.removeFromParent()
        //
        //        print("delay1")
        ////        delay (5) {
        ////
        //////            while readyToRestart == false {
        //////                continue
        ////            }
        //        print("delay5")
        //        print(">>>>>>>>>>>>>>>>>>>>  mmwGameScene.resetGameView() PRE")
        //        mmwGameScene.resetGameView()
        //        print(">>>>>>>>>>>>>>>>>>>>  mmwGameScene.resetGameView() POST")
        //        //mmwGameSceneViewController.resetGame()
        //        print(">>>>>>>>>>>>>>>>>>>>  mmwGameScene.startGame() PRE")
        //        mmwGameScene.startGame()
        //        print(">>>>>>>>>>>>>>>>>>>>  mmwGameScene.startGame() POST")
        //
        //        self.newGameSpriteNode.hidden = true
        //
        //
        //
        //
        //        //MMWOptionsScreen.hidden = true  // pop up graphic ... click to hide//if userInteractionEnabled {
        //
        //        //runAction(actionSound)
        //
        //
        //        stopTimer()
        //
        //        //viewOptions.alpha = 1.0
        //
        ////        print("tileCollection.displayTileArrayValues(tileCollection.mmwDiscardedTileArray)")
        ////        tileCollection!.displayTileArrayValues(tileCollection!.mmwDiscardedTileArray)
        ////        print("tileCollection.displayTileArrayValues(tileCollection.mmwTileArray)")
        ////        tileCollection!.displayTileArrayValues(tileCollection!.mmwTileArray)
        ////        mmwPlayer1Grid.printGrid()
        ////        mmwPlayer2Grid.printGrid()
        ////        mmwPlayer3Grid.printGrid()
        ////        mmwPlayer4Grid.printGrid()
        ////
        ////        //        if mmwGameSceneViewController.numPlayers > 2 {
        ////        //            mmwPlayer3Grid.printGrid()
        ////        //        }
        ////        //        if mmwGameSceneViewController.numPlayers > 3 {
        ////        //            mmwPlayer4Grid.printGrid()
        ////        //        }
        ////
        ////        mmwBoardGrid.printGrid()
        ////        for tile in (tileCollection?.mmwDiscardedTileArray)! {
        ////            print  ("   \(tile.tileText) ")
        ////        }
        
    }
    
    
//    func pauseButton (pauseButtonNode : SKNode) {
//        if userInteractionEnabled == true {
//            //runAction(actionSound)
//            if !isPaused {
//                stopTimer()
//            }
//            else {
//                startTimer(secondsLeft)
//            }
//        }
//    }
    
    
//    func startTimer(seconds: Int) {
//        if mmwGameSceneViewController.secondsPerTurn  == 999 {
//            mmwGameSceneViewController.timerIsOn = false
//        } else {
//            mmwGameSceneViewController.timerIsOn = true
//        }
//        
//        if mmwGameSceneViewController.timerIsOn == false {
//            timeRemainingLabel.text = "Timer: off"
//            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
//            pauseButton.alpha = 0.4
//        } else {
//            isPaused = false
//            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
//            pauseButton.alpha = 1.0
//            timeRemainingLabel.text = "Timer: \(seconds)"
//            timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
//        }
//        
//        
//        //        if mmwGameSceneViewController.timerIsOn == true {
//        //            //runAction(actionSound)
//        //            isPaused = false
//        //            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
//        //            timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
//        //        }
//    }
//    
//    func startTimer() {
//        //runAction(actionSound)
//        if mmwGameSceneViewController.secondsPerTurn  == 999 {
//            mmwGameSceneViewController.timerIsOn = false
//        } else {
//            mmwGameSceneViewController.timerIsOn = true
//        }
//        
//        if mmwGameSceneViewController.timerIsOn == false {
//            timeRemainingLabel.text = "Timer: off"
//            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
//            pauseButton.alpha = 0.4
//        } else {
//            isPaused = false
//            pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
//            pauseButton.alpha = 1.0
//            //timeRemainingLabel.text = "Timer: \(mmwGameSceneViewController.secondsPerTurn)"
//            timeRemainingLabel.text = "Timer: \(secondsLeft)"
//            
//            
//            timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
//        }
//    }
//    
//    
//    func stopTimer() {
//        isPaused = true
//        pauseButton.texture = SKTexture(imageNamed: "MMWResumeButton.png")
//        if (timer != nil) {
//            timer!.invalidate()
//        }
//    }
    
    
    func explosion(pos: CGPoint) {
        let emitterNode = SKEmitterNode(fileNamed: "MagicParticle.sks")
        emitterNode?.zPosition = 100
        emitterNode!.particlePosition = pos
        self.addChild(emitterNode!)
        // Don't forget to remove emitter node after explosion
        runAction(SKAction.playSoundFileNamed("1003.wav", waitForCompletion: false))
        self.runAction(SKAction.waitForDuration(2), completion: { emitterNode!.removeFromParent() })
    }
    
    
    func changePlayerTurn () {
        
        //runAction(actionSound)
        //explosion(CGPointMake(self.size.width/2, self.size.height/2))
        
        mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.dealGridFromArrayRandom(&tileCollection!.mmwTileArray, numTilesToDeal: 6, playerNum: (mmwGameSceneViewController.playerTurn), clearGrid: false)
        
        showTilesInSquares(tileCollection!) // 'deals' player tiles and shows demo tiles on board for testing
        
        let tilesPlayedOnBoard = self.mmwBoardGrid.convert2DGridToArray(mmwBoardGrid.grid2DArr)
        
        for tile in tilesPlayedOnBoard! {
            tile.tileSprite.tileGlow.hidden = true
        }
        
        if tileCollection!.mmwTileArray.count > 0 {
            newTilesButtonOn()
            tilesRemainingLabel.text = "Tiles Left: \(tileCollection!.mmwTileArray.count)"
        }
        else {
            newTilesButtonOff()
            tilesRemainingLabel.text = "Tiles Left: None"
        }
        
        let oldPlayer = mmwGameSceneViewController.playerTurn - 1  // player array is 0 based, players are 1 through 4
        mmwGameSceneViewController.playerArray[oldPlayer].playerLetterGrid.makeTilesInGridInteractive(false)
        
        if oldPlayer < mmwGameSceneViewController.numPlayers - 1 {
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewEndTurn()
            mmwGameSceneViewController.playerTurn++
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()
        }
        else {
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewEndTurn()
            mmwGameSceneViewController.playerTurn = 1
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()
        }
        
//        if mmwGameSceneViewController.timerIsOn {
//            secondsLeft = mmwGameSceneViewController.secondsPerTurn
//            if isPaused == true {
//                startTimer(secondsLeft)
//            }
//        }
        
        
        mmwGameSceneViewController.lettersPlayedInTurn = 0
        
        //tilesPlayedOnBoard = nil
    }
    
    
    func getSnapGrid (testSpot : CGPoint) -> Grid? {
        var snapGrid : Grid = mmwBoardGrid
        
        //        if ((testSpot.x > 157) && (testSpot.x < 869.5)) {snapGrid = mmwBoardGrid}
        //
        //        else if mmwGameSceneViewController.numPlayers == 2 {
        //            if (testSpot.x < 154.5 ) {snapGrid = mmwPlayer1Grid}
        //            if (testSpot.x > 869.5) {snapGrid = mmwPlayer2Grid}
        //        }
        //        else if mmwGameSceneViewController.numPlayers == 3 {
        //            if (testSpot.x < 154.5 ) {snapGrid = mmwPlayer1Grid}
        //            //if (testSpot.x < 154.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer1Grid}
        //            if (testSpot.x > 869.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer2Grid}
        //            if (testSpot.x > 869.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer3Grid}
        //        }
        //        else if mmwGameSceneViewController.numPlayers == 4 {
        //            if (testSpot.x < 154.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer1Grid}
        //            if (testSpot.x > 869.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer2Grid}
        //            if (testSpot.x > 869.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer3Grid}
        //            if (testSpot.x < 154.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer4Grid}
        //        }
        
        //playerView.position = CGPointMake(viewSize.width * 0.8491,  viewSize.height * 0.140 )
        
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
        
        //else {snapGrid = nil
        return snapGrid
    }
    
    
    func numEmptyTileSlotsInGrid (tileGrid: Grid) -> Int {
        var numEmpty = 0
        for arrayX in tileGrid.grid2DArr {
            for arrayY in arrayX {
                if arrayY.tileText == "!" {
                    numEmpty++
                }
            }
        }
        print("<MMWGameScene>numEmptyTileSlots ( = 0) in grid : \(numEmpty) " )
        return numEmpty
    }
    
    
    //    override func update(currentTime: NSTimeInterval) {
    //        //runAction(actionSound)
    //    }
    
    
    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        
    }
    
    
    func newTilesButtonOn () {
        self.newTilesButton.alpha = 1.0
        self.newTilesButton.userInteractionEnabled = false // ??? why false, seems backwards but works this way
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
            --gridTestX
            if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                numExistingLetterTilesInDirection++
            }
            leftOpenTileLocations = gridXStart - numExistingLetterTilesInDirection
        }
        gridTestX = gridXStart
        numExistingLetterTilesInDirection = 0
        
        while gridTestX < 14 {
            ++gridTestX
            if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                numExistingLetterTilesInDirection++
            }
            rightOpenTileLocations = (14 - gridXStart) - numExistingLetterTilesInDirection
        }
        gridTestX = gridXStart
        numExistingLetterTilesInDirection = 0
        
        while gridTestY > 0 {
            --gridTestY
            if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                numExistingLetterTilesInDirection++
            }
            upOpenTileLocations = gridYStart - numExistingLetterTilesInDirection
        }
        gridTestY = gridYStart
        numExistingLetterTilesInDirection = 0
        
        while gridTestY < 14 {
            ++gridTestY
            if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                numExistingLetterTilesInDirection++
            }
            
            downOpenTileLocations = (14 - gridYStart) - numExistingLetterTilesInDirection
        }
        gridTestY = gridYStart
        numExistingLetterTilesInDirection = 0
        print("calculateOpenTileLocations (\(gridXStart), \(gridYStart)) : l:\(leftOpenTileLocations) r:\(rightOpenTileLocations) u:\(upOpenTileLocations) d:\(downOpenTileLocations)  calculateOpenTileLocations mmwGameScene" )
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
        var currentTestAILetterPlay        = validAILetterPlay()
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
                            
                            --numTilesToPlayLeftOrUpMax
                            let permutationLeftOrUpLastLetter = String( playerLettersToTestLeftOrUp.removeAtIndex(playerLettersToTestLeftOrUp.endIndex.predecessor()) )
                            
                            currentTestAILetterPlay.tileSpriteLetter = permutationLeftOrUpLastLetter
                            
                            currentTestAILetterPlay.gridXEnd = gridTestX + tileShiftXLeftOrUp
                            currentTestAILetterPlay.gridYEnd = gridTestY + tileShiftYLeftOrUp
                            
                            currentPossibleAILetterPlayArr.append(currentTestAILetterPlay)
                            lettersPlayedInPermutation++
                            testString = permutationLeftOrUpLastLetter + testString
                            
                            if (gridToTest>=1) {
                                --gridToTest
                                if passNumber == 0 {
                                    --gridTestX
                                }
                                if passNumber == 1 {
                                    --gridTestY
                                }
                            }
                            
                            while gridToTest>=1 && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileType == TileType.Letter {
                                let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileText)
                                testString = existingLetterToAdd + testString
                                
                                if (gridToTest>=1) {
                                    --gridToTest
                                    if passNumber == 0 {
                                        --gridTestX
                                    }
                                    if passNumber == 1 {
                                        --gridTestY
                                    }
                                }
                                
                            }
                        }
                            
                            
                        else {
                            while gridToTest>=1 && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileType == TileType.Letter {
                                let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXLeftOrUp][gridTestY + tileShiftYLeftOrUp].tileText)
                                testString = existingLetterToAdd + testString
                                
                                if (gridToTest>=1) {
                                    --gridToTest
                                    if passNumber == 0 {
                                        --gridTestX
                                    }
                                    if passNumber == 1 {
                                        --gridTestY
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
                            --gridToTest
                            if passNumber == 0 {
                                --gridTestX
                            }
                            if passNumber == 1 {
                                --gridTestY
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
                        ++gridToTest
                        if passNumber == 0 {
                            ++gridTestX
                        }
                        if passNumber == 1 {
                            ++gridTestY
                        }
                    }
                    //////////  added && gridToTest<14 ????? to fix prible with y array going to 14 adding 1 and going off grid
                    while playerLettersToTestRightOrDown.characters.count > 0 && playerLettersToTestRightOrDown.characters.count <= numTilesToPlayRightOrDownMax && gridToTest < 14 {
                        if self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileType != TileType.Letter { // if NOT a letter then play a letter
                            let permutationRightOrDownFirstLetter = String(playerLettersToTestRightOrDown.removeAtIndex(playerLettersToTestRightOrDown.startIndex))
                            --numTilesToPlayRightOrDownMax
                            
                            lettersPlayedInPermutation++
                            currentTestAILetterPlay.tileSpriteLetter = permutationRightOrDownFirstLetter
                            currentTestAILetterPlay.gridXEnd = gridTestX + tileShiftXRightOrDown
                            currentTestAILetterPlay.gridYEnd = gridTestY + tileShiftYRightOrDown
                            
                            currentPossibleAILetterPlayArr.append(currentTestAILetterPlay)
                            lettersPlayedInPermutation++
                            
                            testString = testString + permutationRightOrDownFirstLetter
                            ++gridToTest
                            if passNumber == 0 {
                                ++gridTestX
                            }
                            if passNumber == 1 {
                                ++gridTestY
                            }
                            
                            // if IS a letter then add a letter(s)
                            while gridTestX<14 && gridTestY<14 && self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileType == TileType.Letter {
                                let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileText)
                                //print("Added existing letter: \(mmwBoardGrid.grid2DArr[gridTestX+1][gridTestY].tileText) ")
                                testString = testString + existingLetterToAdd
                                
                                ++gridToTest
                                if passNumber == 0 {
                                    ++gridTestX
                                }
                                if passNumber == 1 {
                                    ++gridTestY
                                }
                                //print (testString)
                            }
                        }
                            
                        else {
                            while self.mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileType == TileType.Letter {
                                let existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX + tileShiftXRightOrDown][gridTestY + tileShiftYRightOrDown].tileText)      //   ????? + 1 ???
                                //print("Added existing letter: \(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText) ")
                                testString = testString + existingLetterToAdd
                                ++gridToTest
                                if passNumber == 0 {
                                    ++gridTestX
                                }
                                if passNumber == 1 {
                                    ++gridTestY
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
                            
                            currentPossibleAILetterPlayArrIndex++
                        
                        }

                            if allLettersPossibleWords && currentPossibleAILetterPlayArr.count > 0 {
                                
                                validPartialAILetterPlayArr = currentPossibleAILetterPlayArr

                                foundPartialWordsNumberAtLockedTile++
   
                                allFoundPartialWordPlaysAtLockedTile.append(validPartialAILetterPlayArr)
                                
                                if (mmwGameSceneViewController.checkWholeWordMatch(testString) == true ) && testString.characters.count >= (mmwGameSceneViewController.minWordSize) {
                                    // update existing placeholder data to reflect that tile made word
                                    foundWholeWordsNumberAtLockedTile++
                                    
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
                                    print("!!! allValidWholeWordTilePlayArr \(validWholeWordAILetterPlayArr) \(testString) checkForValidWordsAI mmwGameScene" )
                                }
                            }
                            else {
                                break
                            }
                        }
                    else {
                        //print("NOT partial Word Match! \(testString) checkForValidWordsAI mmwGameScene" )
                    }
                    
                    currentTestAILetterPlay = validAILetterPlay()
                    currentPossibleAILetterPlayArr  = [validAILetterPlay()]
                    
                    validPartialAILetterPlayArr = [validAILetterPlay()]
                    validWholeWordAILetterPlayArr = [validAILetterPlay()]
                    
                    lettersPlayedInPermutation = 0
                    tileArrayToPlay = [MMWTile]()
                    testString = self.mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot].tileText // get letter at start tile
                    shiftLeftOrUp--
                    shiftRightOrDown++
                }
            }            /////////////////////
            gridTestX = gridXSpot // reset test spot as it may have been moved to right in code below
            gridTestY = gridYSpot
            ////////////////////
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
        //print("H Found word : \(testString) and now getting tiles and playing letters")
        //validWordPlays.append(word)
        //var numLetters = 0.0
        
        var placedTiles = 0
        
        for letter in validWholeWordAILetterPlayArr {
            //letter.partOfWord = letter.tileSpriteLetter
            // !!! having some problems with 2 of same letter. sometimes only 1 will show on board (didn't work out of delay so trying inside delay)
            
            let delayForNextTile : Double = Double(1.5 * Double(placedTiles) )
            
            delay(delayForNextTile) {
                
                var letterTilePlayable : MMWTile
                
                letterTilePlayable  = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid.getTileWithLetter(letter.tileSpriteLetter)!
                
                if letter.madeValidWord == true {
                    letterTilePlayable.tileState = TileState.PlayedMadeWord
                } else {
                    letterTilePlayable.tileState = TileState.Played
                }
                print("H updating letterTilePlayable.tileSprite.updateAIWordsAtDropSpot \(letter.tileSpriteLetter), \(letter.gridXEnd), \(letter.gridYEnd), \(letter.partOfWord), \(letterTilePlayable.tileState), \(letterTilePlayable.tileState.rawValue ) checkForValidWordsAI mmwGameScene")
                letterTilePlayable.tileSprite.updateAIWordsAtDropSpot(letter.gridXEnd, tileYGridDestination: letter.gridYEnd, madeValidWord: letter.madeValidWord)
            }
            placedTiles++
            
            
            //numLetters++
        }
        return true
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
        let transition = SKTransition.crossFadeWithDuration(0.5)
        let mmwResultsScene = MMWResultsScene(size: size,
            gameResult: true,
            score: 123)
        view?.presentScene(mmwResultsScene, transition: transition)
    }
}

