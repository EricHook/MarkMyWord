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

class MMWGameScene: SKScene { // , SKPhysicsContactDelegate {
    
    var mmwGameSceneViewController : MMWGameSceneViewController!

    var viewSize : CGSize!
    var backgroundNode : SKSpriteNode = SKSpriteNode( imageNamed: "MarkMyWordBGCleaniPad@2x.png" ) // ( imageNamed: ("MarkMyWordBGCleaniPad@2x.png" ) "MarkMyWordBGCleaniPhone@3x.png" )
    var optionsLayerNode  : SKSpriteNode = SKSpriteNode(imageNamed: "MMWOptionsScreen.jpg")
    var foregroundNode : SKSpriteNode = SKSpriteNode()
    
    var mmwBoardGrid: Grid!
    
    var mmwPlayer1Grid : Grid!
    var mmwPlayer2Grid : Grid!
    var mmwPlayer3Grid : Grid!
    var mmwPlayer4Grid : Grid!
    
    var player1View : PlayerView!
    var player2View : PlayerView!
    var player3View : PlayerView!
    var player4View : PlayerView!

    var timeRemainingLabel  = SKLabelNode(fontNamed: FontHUDName)
    private var secondsLeft: Int = 20
    var tilesRemainingLabel = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel     = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel2    = SKLabelNode(fontNamed: FontHUDName)
    var bottomDisplayLabel    = SKLabelNode(fontNamed: FontHUDName)
    
    let playButton = SKSpriteNode(imageNamed: "PlayButton.png")
    var newTilesButton = SKSpriteNode(imageNamed: "NewTilesButton.png")
    let passButton = SKSpriteNode(imageNamed: "PassButton.png")
    let pauseButton = SKSpriteNode(imageNamed: "PauseButton.png")
    let optionsButton = SKSpriteNode(imageNamed: "OptionsButton.png")

    var placeholderTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    
    let actionSound = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    let dealTilesSound = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
    let destroyTileSound = SKAction.playSoundFileNamed("1023.WAV", waitForCompletion: true)

    //let backgroundMusic = SKAction.playSoundFileNamed("30Showdown.m4a", waitForCompletion: true)

//    let animPart1 = SKAction.group([unhide, slide])
//    let animScale = SKAction.sequence([scaleUp, scaleDown, hide])
//    self.tileScore2!.runAction(SKAction.group([fadeIn, animPart1, fadeOut, animScale]) )

    private var timer: NSTimer?
    private var isPaused: Bool = false
    
    enum MMWErrorType: ErrorType {
        case MMWError
        case MMWError2(lettersNeeded: Int)
    }

    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        viewSize = size
    }
    
    func setViewController (mmwGameSceneController: MMWGameSceneViewController) {
        self.mmwGameSceneViewController = mmwGameSceneController
    }
    
    func resetPassCounter () {
        mmwGameSceneViewController.consequtivePasses = 0
        print("\(mmwGameSceneViewController.consequtivePasses) consequtive passes")
    }
    
    func setGrids () {
        self.mmwBoardGrid = Grid(gridUpperLeftX: 156, gridUpperLeftY: (67), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15, gridName: "mmwBoardGrid", mmwGameScene: self)
        
        if mmwGameSceneViewController.numPlayers == 2 {
            self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1Grid", mmwGameScene: self) //y 304
            self.mmwPlayer1Grid.setGridPlayer(mmwGameSceneViewController.player1)
            
            self.mmwPlayer2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2Grid", mmwGameScene: self)
            self.mmwPlayer2Grid.setGridPlayer(mmwGameSceneViewController.player2)
        }

        if mmwGameSceneViewController.numPlayers == 3 {
            self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1Grid", mmwGameScene: self) // 0.4583
            self.mmwPlayer1Grid.setGridPlayer(mmwGameSceneViewController.player1)
            
            self.mmwPlayer2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2Grid", mmwGameScene: self)
            self.mmwPlayer2Grid.setGridPlayer(mmwGameSceneViewController.player2)
            
            self.mmwPlayer3Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer3Grid", mmwGameScene: self)
            self.mmwPlayer3Grid.setGridPlayer(mmwGameSceneViewController.player3)
        }
        
        if mmwGameSceneViewController.numPlayers == 4 {
            self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1Grid", mmwGameScene: self) // 0.4583
            self.mmwPlayer1Grid.setGridPlayer(mmwGameSceneViewController.player1)
            
            self.mmwPlayer2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2Grid", mmwGameScene: self)
            self.mmwPlayer2Grid.setGridPlayer(mmwGameSceneViewController.player2)

            
            self.mmwPlayer3Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer3Grid", mmwGameScene: self)
            self.mmwPlayer3Grid.setGridPlayer(mmwGameSceneViewController.player3)
            
            
            self.mmwPlayer4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer4Grid", mmwGameScene: self)
            self.mmwPlayer4Grid.setGridPlayer(mmwGameSceneViewController.player4)
        }
    }
 
    func changePlayerScoreDisplay (playerView: PlayerView, player: Player, score: Int) {
        player.playerScore = score
        playerView.playerScoreLabel.text = String(score)
    }

    
    func buildGameView () {
        backgroundColor = SKColor(red: 0.5, green: 0.0, blue: 0.0, alpha: 1.0)
        
        userInteractionEnabled = true
        // add backgroundNode
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: size.width/2.0, y: 0.0)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.zPosition = -100
        backgroundNode.size.height = viewSize.height;
        backgroundNode.size.width = viewSize.width;
        addChild(backgroundNode)

        player1View = addPlayerView(1, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player1))
        player1View.setPlayerGameSceneAndController(self, mmwGameSceneViewController: self.mmwGameSceneViewController)
        player2View = addPlayerView(2, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player2))
        player2View.setPlayerGameSceneAndController(self, mmwGameSceneViewController: self.mmwGameSceneViewController)
        if mmwGameSceneViewController.numPlayers > 2 {
            player3View = addPlayerView(3, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player3))
            player3View.setPlayerGameSceneAndController(self, mmwGameSceneViewController: self.mmwGameSceneViewController)
        }
        if mmwGameSceneViewController.numPlayers > 3 {
            player4View = addPlayerView(4, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player4))
            player4View.setPlayerGameSceneAndController(self, mmwGameSceneViewController: self.mmwGameSceneViewController)
        }

        addChild(foregroundNode) // generic SKNode to separate foreground elements from background
        
        backgroundNode.zPosition = -50

        tilesRemainingHUD(mmwGameSceneViewController.tileCollection.mmwTileArray.count) // default test number to tiles remaining

        timeRemainingLabel.text = "Timer: 00"

        bottomDisplayHUD("Begin ... ")
        
        topDisplayHUD("Welcome to Mark My Word") // ("Turn: Player 1, Special Letter Bonus In Effect, 2x Point Bonus")
        topDisplayHUD2("topDisplayHUD2")

        playButton.position = CGPoint(x: viewSize.width * 0.5, y: viewSize.height * 0.5)
        playButton.name = "playButton"
        self.addChild(playButton)

        newTilesButton.position = CGPoint(x: viewSize.width * 0.0751, y: viewSize.height * 0.102)
        newTilesButton.name = "newTilesButton"
        newTilesButton.userInteractionEnabled = true
        self.addChild(newTilesButton)

        passButton.position = CGPoint(x: viewSize.width * 0.0752, y: viewSize.height * 0.04)
        passButton.name = "passButton"
        passButton.userInteractionEnabled = true
        self.addChild(passButton)

        pauseButton.position = CGPoint(x: viewSize.width * 0.9228, y: viewSize.height * 0.102)
        pauseButton.name = "pauseButton"
        pauseButton.userInteractionEnabled = true
        self.addChild(pauseButton)

        optionsButton.position = CGPoint(x: viewSize.width * 0.9228, y: viewSize.height * 0.04)
        optionsButton.name = "optionsButton"
        optionsButton.userInteractionEnabled = true
        self.addChild(optionsButton)

        // ADD ALL TILES to Scene - they start as invisible
        var tileTempXLocation = 0
        for tile in mmwGameSceneViewController.tileCollection.mmwTileArray {
            tile.tileSprite.hidden = true
            tileTempXLocation += 40
            tile.tileSprite.tileLocation = CGPoint(x: tileTempXLocation, y: 15 )
            self.addChild(tile.tileSprite)
        }
        
        optionsLayerNode.name = "optionsLayerNode"
        optionsLayerNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        optionsLayerNode.position = CGPoint(x: size.width/2.0, y: size.height/2.0)
        optionsLayerNode.userInteractionEnabled = false
        optionsLayerNode.zPosition = 100
        optionsLayerNode.size = CGSize(width: viewSize.width * 0.9,  height: viewSize.height * 0.8)
        optionsLayerNode.alpha = 0.75
        optionsLayerNode.hidden = true
        self.addChild(optionsLayerNode)
        
        // Placeholder for dictionary data loading progress display
        let playBtnTEMP = SKSpriteNode(imageNamed: "PlayButton.png")
        playBtnTEMP.position = CGPoint(x: viewSize.width/4, y: viewSize.height/6)
        self.addChild(playBtnTEMP)
        playBtnTEMP.name = "playBtnTEMP"
        playBtnTEMP.zPosition = 100
        playBtnTEMP.anchorPoint = CGPointMake(0, 0)
        let scaleHoriz = SKAction.scaleXTo(2, duration: 3.0)
        let loadingAnim = SKAction.group([scaleHoriz])
        let loadingAnimSequence = SKAction.sequence([loadingAnim, SKAction.removeFromParent()])
        playBtnTEMP.runAction(loadingAnimSequence)

        // Placeholder for AI play
        let playBtnPlay = SKSpriteNode(imageNamed: "PlayButton.png")
        playBtnPlay.position = CGPoint(x: viewSize.width * 0.9228, y: viewSize.height * 0.04)
        playBtnPlay.name = "playBtnPlay"
        playBtnPlay.zPosition = 100
        playBtnPlay.anchorPoint = CGPointMake(0, 0)
        self.addChild(playBtnPlay)
        newTilesButton.userInteractionEnabled = true
    }
    
    //func displayOptions
    
    func showAllGridTiles (gridToDisplay: Grid) {
        runAction(dealTilesSound)
        for x in 0...gridToDisplay.gridNumSquaresX - 1 {   // fill Player 1 letter tiles : 2 in y and 3 in x
            for y in 0...gridToDisplay.gridNumSquaresY - 1 {
                let tileAdded : MMWTile = gridToDisplay.grid2DArr[x][y] // tileBuilder.mmwPlayer1LetterTileArray[tileArrSelectedTile]
                tileAdded.tileSprite.hidden = false
                tileAdded.tileSprite.position = Grid.sendToGridSquare(gridToDisplay, squareX: x, squareY: y)
            }
        }
    }
    
    func showTilesInSquares (tileBuilder: MMWTileBuilder) { // shows all tiles from TileBuilder [MMWTile] arrays
        
        runAction(dealTilesSound)

        showAllGridTiles(mmwPlayer1Grid)
        showAllGridTiles(mmwPlayer2Grid)
        
        if mmwGameSceneViewController.numPlayers > 2 {
            showAllGridTiles(mmwPlayer3Grid)
        }
        
        if mmwGameSceneViewController.numPlayers > 3 {
            showAllGridTiles(mmwPlayer4Grid)
        }
        
        showAllGridTiles(mmwBoardGrid)
    }

    func timeRemainingHUD (timeAmount: Int)  -> SKLabelNode {
        timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
        timeRemainingLabel.zPosition = 1
        timeRemainingLabel.text =  "Timer: \( String(secondsLeft) ) " // "Timer: \(timeAmount)"
        timeRemainingLabel.fontSize = FontHUDSize
        timeRemainingLabel.fontColor = FontHUDWhite
        timeRemainingLabel.position = CGPointMake(size.width/2.0 + 255.0, 3)
        timeRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(timeRemainingLabel)
        return timeRemainingLabel
    }
    
    func updateCounter() {
        timeRemainingLabel.fontColor = SKColor(red: 1, green: 1, blue: 1, alpha: 1) // counter starts white color
        timeRemainingLabel.text = "Timer: \( String(secondsLeft) ) " // String(counter--)
        
        if secondsLeft <= 10 && secondsLeft > 5 {
            timeRemainingLabel.text =  "Timer: \( String(secondsLeft) ) "
            let countdownSmall = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
            timeRemainingLabel.fontColor = SKColor(red: 0, green: 1, blue: 1, alpha: 1) // counter turns orange color
            runAction(SKAction.sequence( [countdownSmall]) )
        }
        if secondsLeft <= 5 {
            timeRemainingLabel.text =  "Timer: \( String(secondsLeft) ) "
            let countdownBig = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
            runAction(SKAction.sequence( [countdownBig]) )
            timeRemainingLabel.fontColor = SKColor(red: 1, green: 0, blue: 0, alpha: 1) // counter turns red color
            if secondsLeft <= 0 {
                runAction(actionSound)   // play turn over sound
                timeRemainingLabel.text = ("Timer: 0")
                changePlayerTurn()
            }
        }
        --secondsLeft
    }
    
    func tilesRemainingHUD (tilesLeft : Int) -> SKLabelNode {
        tilesRemainingLabel.zPosition = 1
        tilesRemainingLabel.text = "Tiles Left: \(mmwGameSceneViewController.tileCollection.mmwTileArray.count  )" // "Tiles Left: \(tilesLeft)"
        tilesRemainingLabel.fontSize = FontHUDSize
        tilesRemainingLabel.position = CGPointMake(size.width/2.0 - 332, 3.0)
        tilesRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(tilesRemainingLabel)
        return tilesRemainingLabel
    }
     
    func bottomDisplayHUD (letters : String)  -> SKLabelNode {        // bottom text display area - first line
        bottomDisplayLabel.zPosition = 1
        bottomDisplayLabel.text = "\(letters)"
        bottomDisplayLabel.fontSize = FontHUDSize
        bottomDisplayLabel.position = CGPointMake(size.width/2.0, 3)
        bottomDisplayLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(bottomDisplayLabel)
        return bottomDisplayLabel
    }
    
    func topDisplayHUD (message : String)  -> SKLabelNode {         // top text display area - first line
        topDisplayLabel.zPosition = 1
        topDisplayLabel.text = "\(message)"
        topDisplayLabel.fontSize = FontHUDSize
        topDisplayLabel.position = CGPointMake(size.width/2.0, 753.0) // CGPointMake(size.width/2.0, 753.0) // 1 of 2 top lines
        topDisplayLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayLabel)
        return topDisplayLabel
    }
    
    func topDisplayHUD2 (message : String)  -> SKLabelNode {        // top text display area - second line
        topDisplayLabel2.zPosition = 1
        topDisplayLabel2.text = message
        topDisplayLabel2.fontSize = FontHUDSize
        topDisplayLabel2.position = CGPointMake(size.width/2.0, 735.0) // 2 of 2 top lines
        topDisplayLabel2.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayLabel2)
        return topDisplayLabel2
    }

    func addPlayerView (playerNum : Int, playerView : PlayerView) -> PlayerView {
        if mmwGameSceneViewController.numPlayers == 2 {         // if only 2 players - one on left and one on right of UI
            if playerNum == 1 {
                playerView.position = CGPointMake(0, self.size.height * 0.384765625 ) // PS coordinates is Y: 1390, convert and flip .. x , 295.5
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(self.size.width * 0.84912109375, self.size.height * 0.384765625 ) // PS coordinates is Y: 1390, convert and flip , x , 295.5
            }
        }
        
        if mmwGameSceneViewController.numPlayers == 3 {     // if 3 players - one on left and two on right of UI
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
   
        if mmwGameSceneViewController.numPlayers == 4 {     // if 4 players - two on left and two on right of UI
            if playerNum == 1 {
                playerView.position = CGPointMake( 0,  viewSize.height * 0.57096354 ) // PS coordinates is Y: 1390, convert and flip
            }
            if playerNum == 2 {
                playerView.position = CGPointMake(viewSize.width * 0.8491, viewSize.height * 0.57096354 ) // PS coordinates is Y: 1390, convert and flip
            }
            if playerNum == 3 {
                playerView.position = CGPointMake(viewSize.width * 0.8491,  viewSize.height * 0.138 ) // PS coordinates is Y: 1390, convert and flip
            }
            if playerNum == 4 {
                playerView.position = CGPointMake(0, viewSize.height * 0.138 ) // PS coordinates is Y: 1390, convert and flip
            }
        }
        playerView.zPosition = -1
        addChild(playerView)
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
                    runAction(actionSound)
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
            
            if(_node.name == "pauseButton"){
                pauseButton (_node)
            }
            
            if(_node.name == "playBtnPlay"){
                playBtnPlay (_node)
            }
        }
    }
    
    func playButton (buttonNode: SKNode) {
        if userInteractionEnabled {
            print(">>> PLAY BUTTON PRESSED >>>")
            
            //let starterWord = mmwGameSceneViewController.getRandomWord()
            
            let starterWord = "ar"
            var starterWordTileArray = mmwGameSceneViewController.checkUndealtTilesForWord(starterWord, letterTileArray: &mmwGameSceneViewController.tileCollection.mmwTileArray)
            
            let starterWord2 = "n"
            var starterWordTileArray2 = mmwGameSceneViewController.checkUndealtTilesForWord(starterWord2, letterTileArray: &mmwGameSceneViewController.tileCollection.mmwTileArray)
            
            // SENDS RANDOM WORD TO CENTER OF BOARD // half of the time Horizontal and the rest Vertical
            
/////////////////////////////////////////////////////

            if arc4random_uniform(100) < 0 { // VERTICAL  !!!!!!!!! CURRENTLY SET SO ALWAYS HORIZONTAL
                mmwGameSceneViewController.sendWordToBoard(&starterWordTileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 7, yStartSquare: 5, IsHorizonal: false, player: mmwGameSceneViewController.player0)
            }
            else { // HORIZONTAL
                mmwGameSceneViewController.sendWordToBoard(&starterWordTileArray!, gridToDisplay: mmwBoardGrid, xStartSquare: 5, yStartSquare: 7, IsHorizonal: true, player: mmwGameSceneViewController.player0)
                
                mmwGameSceneViewController.sendWordToBoard(&starterWordTileArray2!, gridToDisplay: mmwBoardGrid, xStartSquare: 3, yStartSquare: 7, IsHorizonal: true, player: mmwGameSceneViewController.player0)
            }
            
            mmwPlayer1Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 1, clearGrid: false)
            mmwPlayer2Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 2, clearGrid: false)
            
            if mmwGameSceneViewController.numPlayers > 2 {
                mmwPlayer3Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 3, clearGrid: false)
            }
            
            if mmwGameSceneViewController.numPlayers > 3 {
                mmwPlayer4Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 4, clearGrid: false)
            }
            
            buttonNode.removeFromParent() // gets rid of play button in middle of screen
            
            showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles
            
            tilesRemainingLabel.text = "Tiles Left: \(mmwGameSceneViewController.tileCollection.mmwTileArray.count)"
            mmwPlayer1Grid.makeTilesInGridInteractive(true)
            
            mmwGameSceneViewController.player1.playerView.playerViewBeginTurn()
            
            // buttons inactive until "playButton" is pressed
            newTilesButton.userInteractionEnabled = false
            passButton.userInteractionEnabled = false
            pauseButton.userInteractionEnabled = false
            optionsButton.userInteractionEnabled = false
            
            timeRemainingHUD(mmwGameSceneViewController.secondsPerTurn)  // default set to standard time remaining
        }
    }
    
    
    func playBtnPlay (playButtonPlayNode: SKNode) {
        //get tile from current player
        print("tile to play \(mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid ) ")
        mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn-1].playerLetterGrid.printGrid()
        
        //lookForPlayedSpots()
        
        checkForValidWordsAI(5 , gridYSpot: 7,  availableTiles: [MMWTile()], numLettersToPlay: 1)
    }
    
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
                mmwGameSceneViewController.consequtivePasses += 1
            }
            
            print("\(mmwGameSceneViewController.consequtivePasses) consequtive passes")
            if mmwGameSceneViewController.consequtivePasses >= mmwGameSceneViewController.numPlayers {
                print("CHECK TO END GAME > PASSES = \(mmwGameSceneViewController.consequtivePasses)")
            }
            //                  runAction(actionSound)
            changePlayerTurn()
            if isPaused == true {
                startTimer(mmwGameSceneViewController.secondsPerTurn)
            }
        }
    }
    
    func newTilesButton(newTilesButtonNode: SKNode) {
        if userInteractionEnabled {
            runAction(actionSound)
            
//            for tile in mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.grid2DArr {
//            }
            
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: (mmwGameSceneViewController.playerTurn), clearGrid: true)

            showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
            mmwGameSceneViewController.resetConsequtivePasses()
            changePlayerTurn()
            if isPaused == true {
                startTimer(mmwGameSceneViewController.secondsPerTurn)
            }
        }
    }
    
    func optionsButton (optionsButtonNode: SKNode) {
        if userInteractionEnabled {
            runAction(actionSound)
            stopTimer()
            mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwDiscardedTileArray)
            mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwTileArray)
            mmwPlayer1Grid.printGrid()
            mmwPlayer2Grid.printGrid()
            if mmwGameSceneViewController.numPlayers > 2 {
                mmwPlayer3Grid.printGrid()
            }
            if mmwGameSceneViewController.numPlayers > 3 {
                mmwPlayer4Grid.printGrid()
            }
            mmwBoardGrid.printGrid()
            
            optionsLayerNode.hidden = false
        }
    }
    
    func pauseButton (pauseButtonNode : SKNode) {
        if userInteractionEnabled {
            //runAction(actionSound)
            if !isPaused {
                stopTimer()
            }
            else {
                startTimer(secondsLeft)
            }
        }
    }
    
    func startTimer(seconds: Int) {
        runAction(actionSound)
        isPaused = false
        pauseButton.texture = SKTexture(imageNamed: "PauseButton.png")
        timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
    }
    
    func stopTimer() {
        isPaused = true
        pauseButton.texture = SKTexture(imageNamed: "PlayButton.png")
        timer!.invalidate()
    }

    func explosion(pos: CGPoint) {
        let emitterNode = SKEmitterNode(fileNamed: "MagicParticle.sks")
        emitterNode?.zPosition = 100
        emitterNode!.particlePosition = pos
        self.addChild(emitterNode!)
        // Don't forget to remove the emitter node after the explosion
        self.runAction(SKAction.waitForDuration(2), completion: { emitterNode!.removeFromParent() })
        runAction(SKAction.playSoundFileNamed("1023.wav", waitForCompletion: false))
    }
    
    func changePlayerTurn () {
        
        runAction(actionSound)
        //explosion(CGPointMake(self.size.width/2, self.size.height/2))
  
        mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: (mmwGameSceneViewController.playerTurn), clearGrid: false)
        
        showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing

        let oldPlayer = mmwGameSceneViewController.playerTurn - 1  // player array is 0 based, players are 1 through 4
        mmwGameSceneViewController.playerArray[oldPlayer].playerLetterGrid.makeTilesInGridInteractive(false)
        if oldPlayer < mmwGameSceneViewController.numPlayers - 1 {
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewEndTurn()
            mmwGameSceneViewController.playerTurn++
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()
            
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.makeTilesInGridInteractive(true)
        }
        else {
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewEndTurn()
            mmwGameSceneViewController.playerTurn = 1
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()
            
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.makeTilesInGridInteractive(true)
            mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerView.playerViewBeginTurn()
        }

        if mmwGameSceneViewController.tileCollection.mmwTileArray.count > 0 {
            tilesRemainingLabel.text = "Tiles Left: \(mmwGameSceneViewController.tileCollection.mmwTileArray.count)"
        }
        else {
            newTilesButtonOff()
            tilesRemainingLabel.text = "Tiles Left: None"
        }

        if mmwGameSceneViewController.tileCollection.mmwTileArray.count > 0 { newTilesButtonOn() }
        
        secondsLeft = 21
        
        mmwGameSceneViewController.lettersPlayedInTurn = 0
    }
    
    func getSnapGrid (testSpot : CGPoint) -> Grid? {
        var snapGrid : Grid = mmwBoardGrid
        
        if ((testSpot.x > 157) && (testSpot.x < 869.5)) {snapGrid = mmwBoardGrid}
        
        else if mmwGameSceneViewController.numPlayers == 2 {
            if (testSpot.x < 154.5 ) {snapGrid = mmwPlayer1Grid}
            if (testSpot.x > 869.5) {snapGrid = mmwPlayer2Grid}
        }
        else if mmwGameSceneViewController.numPlayers == 3 {
            if (testSpot.x < 154.5 ) {snapGrid = mmwPlayer1Grid}
            //if (testSpot.x < 154.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer1Grid}
            if (testSpot.x > 869.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer2Grid}
            if (testSpot.x > 869.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer3Grid}
        }
        else if mmwGameSceneViewController.numPlayers == 4 {
            if (testSpot.x < 154.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer1Grid}
            if (testSpot.x > 869.5 ) && testSpot.y < 384 {snapGrid = mmwPlayer2Grid}
            if (testSpot.x > 869.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer3Grid}
            if (testSpot.x < 154.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer4Grid}
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
    
    override func update(currentTime: NSTimeInterval) {
        //runAction(actionSound)
    }
    
    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {

    }
    
    func newTilesButtonOn () {
        self.newTilesButton.alpha = 1.0
        self.newTilesButton.userInteractionEnabled = false
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
    
//    func checkForValidWords (var gridXSpot: Int, gridYSpot: Int, availableTiles: [MMWTile]) {
//
//        ////////////  TEST FOR ADJACENT TILE PARTIAL WORDS
//        var gridTestX = gridXSpot
//        var gridTestY = gridYSpot
//        
//        var lockedBoardTilesArr = mmwGameSceneViewController.mmwGameScene.mmwBoardGrid.getArrayLetterTilesInGrid()
//        var playerTilesArr = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.getArrayLetterTilesInGrid()
//        var playerTilesLettersArr : [String] = [String]()
//        var playerTilesPermutations : [String] = [String]()
//        
//        var testValidWordTileArr : [MMWTile] = [MMWTile]()
//        
//        ++gridXSpot // just to test letter to right of existing locked letter
//        
//        for tile in playerTilesArr {                    // prints out letters in player tiles
//            print("\(tile.tileText) ", terminator: "")
//            playerTilesLettersArr.append(tile.tileText)
//        }
//        print("")      // to add a return on last letter
//        
//        
//        var tilesToPlay = playerTilesArr
//        var placedTiles = 0
//        var existingPlayedTiles = 0
//        var horizontalString = ""               // mmwGameSceneViewController.mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot].letterString
//        var numLettersLeft = 0
//        var numLettersRight = 0
//        
//        //RIGHT//
//
//        testValidWordTileArr.append(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY]) // get letter at initial spot
//        
//        for var testTile in playerTilesArr {
//            //sleep(1)
//            while self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter && (gridTestX < 14 ){
//                ++gridTestX
//                existingPlayedTiles++
//                
//                if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
//                    testValidWordTileArr.append(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY])
//                    testTile.tileSprite.checkForValidWords(gridTestX, gridYSpot: gridTestY, IsAI: true)
//                }
//            }
//
//            if (testTile.tileSprite.testForValidPartialWordsAtDropSpot(gridTestX, tileSnapResultsYGrid: gridTestY, isAI: true).validHorizontalPartialWord) && (testTile.tileSprite.tileText != "!") {
//
//                placedTiles++
//
//                testTile.tileSprite.checkForValidWords(gridTestX, gridYSpot: gridTestY, IsAI: true)
//
//                mmwGameSceneViewController.dealLetter(&testTile, gridToPlaceLetter: mmwBoardGrid, xSquare: gridTestX, ySquare: gridTestY)
//
//                testTile.gridXEnd = gridTestX
//                testTile.gridYEnd = gridTestY
//                testTile.gridEnd = mmwBoardGrid
//                testTile.tileSprite.playTileToBoardGrid(Double(placedTiles))
//                
//                //sleep(UInt32(placedTiles))
//
//                testValidWordTileArr.append(testTile)
//                
//                horizontalString = ""
//                
//                let playerTilesPermutations = permuteLetters(playerTilesLettersArr, minStringLen: 0, maxStringLen: 1)
//                print("Permutations:: \(playerTilesPermutations)")
//                
//                for playerTile in testValidWordTileArr {
//                    horizontalString = horizontalString + playerTile.tileSprite.tileText
//                }
//                
//                print(horizontalString)
//
//                if (( self.mmwGameSceneViewController.checkWholeWordMatch(horizontalString) ) == false || horizontalString.characters.count < mmwGameSceneViewController!.minWordSize  ) {
//                    // updates GUI for feedback on horizonal partial word
//                    updatePartialWordFeedback("\(horizontalString) is a partial word (horizontalString)")
//                    print ( "\(horizontalString) is a partial word (horizontalString)" )
//                }
//                    
//                else {
//                    print ( "\(horizontalString) is a valid word (horizontalString)" )
//                    //sleep(UInt32(placedTiles))
//                    testTile.tileSprite.scoreTilesInArr(testValidWordTileArr, wordString: horizontalString)
//                }
//                
//                if (testTile.tileState != TileState.Locked) {
//                    testTile.tileState = TileState.Played // if put on valid board location set TileState to played if NOT already locked
//                }
//                
//                // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
//                let replacementPlaceholderTile : MMWTile = MMWTile()
//                replacementPlaceholderTile.gridHome = testTile.gridHome
//                replacementPlaceholderTile.gridX = testTile.gridX
//                replacementPlaceholderTile.gridY = testTile.gridY
//                testTile.gridHome?.grid2DArr[testTile.gridX][testTile.gridY] = replacementPlaceholderTile
//
//                // set value of snap results grid location to the MMWTile if valid location
//                testTile.gridEnd? = testTile.gridEnd!
//                testTile.gridEnd?.grid2DArr[testTile.gridX][testTile.gridY] = testTile
//                testTile.gridXEnd = testTile.gridX
//                testTile.gridYEnd = testTile.gridY
//                testTile.tileSprite.isMovable = false // so can't remove tile once placed
//                testTile.tileSprite.userInteractionEnabled = false
//                testTile.tileSprite.removeBoardTileHighlights ()
//                testTile.gridHome?.mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
//
//            }
//          
//        }
//
////        gridTestX--
////        horizontalString = ""
////        
////        for playerTile in 0..<(existingPlayedTiles) {
////            
////            if (gridTestX) >= 0 {
////                
////                horizontalString = mmwGameSceneViewController.mmwGameScene.mmwBoardGrid.grid2DArr[gridTestX--][gridYSpot].tileText + horizontalString
////
////            }
////        }
////        print(horizontalString)
//
//        for char in horizontalString.lowercaseString.characters {
//            //print(char.debugDescription.lowercaseString)
//            
//            if String(char) == "!" {
//                print("X", terminator: "")
//            }
//            else {
//                print("\(char)", terminator: "")
//            }
//        }
//
////                var stringToAdd : String = ""
////                tilesToAdd = [MMWTile]()
////                
////                while ( (currentCheckXGridNum > 0) && (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Played) ) )  {
////                    
////                    numHorizontalTiles++
////                    tilesToAdd.append((gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot]))
////                    
////                    let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].letterString)"
////                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd) // (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].letterString)
////                    
////                    
////                    if gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked {
////                        foundLockedTile = true // makes sure locked tile in potential word
////                        hasLockedPotentialWord = true
////                        hasValidLockedTile = true
////                    }
////                    currentCheckXGridNum--
////                }
////                
////                for char in stringToAdd.characters {
////                    leftString = (String(char) + leftString)
////                }
////                leftString = leftString.stringByAppendingString(self.tileText) // add current letter string to leftward letters
////                
////                for tile in tilesToAdd {
////                    horizontalLeftTiles.append(tile)
////                }
////                horizontalLeftTiles = horizontalLeftTiles.reverse()
////                horizontalLeftTiles.append(self.tileSpriteParent) // add current tile to leftward tiles
//
//                
//                
//                
//                
//                
////                //RIGHT
////                currentCheckXGridNum = gridXSpot
////                currentCheckYGridNum = gridYSpot
////                stringToAdd = ""
////                tilesToAdd = [MMWTile]()
////                
////                while ( (currentCheckXGridNum < 14) && (gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Played) ) )  {
////                    
////                    numHorizontalTiles++
////                    tilesToAdd.append((gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot]))
////                    
////                    let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].letterString)"
////                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
////                    
////                    if gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Locked {
////                        hasLockedPotentialWord = true
////                        hasValidLockedTile = true
////                    }
////                    currentCheckXGridNum++
////                }
////                
////                for tile in tilesToAdd {
////                    horizontalTiles.append(tile)
////                }
////                
////                horizontalString = leftString.stringByAppendingString(stringToAdd)
////                
////                print(horizontalString + " checkForValidWords")
////                
////                ////////////////////////////
////                
////                // UP
////                currentCheckXGridNum = gridXSpot
////                currentCheckYGridNum = gridYSpot
////                stringToAdd = ""
////                tilesToAdd = [MMWTile]()
////                
////                while ( (currentCheckYGridNum > 0) && (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Locked || (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Played) ) )  {
////                    numVerticalTiles++
////                    tilesToAdd.append((gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1]))
////                    
////                    let letterToAdd : String = "\(gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].letterString)"
////                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd) // (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].letterString)
////                    
////                    if gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Locked {
////                        foundLockedTile = true // stops check on last last locked tile
////                        hasLockedPotentialWord = true
////                        hasValidLockedTile = true
////                    }
////                    currentCheckYGridNum--
////                }
////                
////                for char in stringToAdd.characters {
////                    upString = (String(char) + upString)
////                }
////                upString = upString.stringByAppendingString(self.tileText) // add current letter string to upward letters
////                
////                for tile in tilesToAdd {
////                    verticalUpTiles.append(tile)
////                }
////                
////                verticalUpTiles = verticalUpTiles.reverse()
////                verticalUpTiles.append(self.tileSpriteParent)
////                
////                //DOWN
////                currentCheckXGridNum = gridXSpot
////                currentCheckYGridNum = gridYSpot
////                stringToAdd = ""
////                tilesToAdd = [MMWTile]()
////                
////                while ( (currentCheckYGridNum < 14) && (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum][currentCheckYGridNum + 1].tileState == TileState.Played) ) )  {
////                    numVerticalTiles++
////                    tilesToAdd.append((gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1]))
////                    
////                    let letterToAdd : String = "\(gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].letterString)"
////                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
////                    
////                    if currentCheckYGridNum != 0 {
////                        if gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked {
////                            foundLockedTile = true // stops check on last last locked tile
////                            hasLockedPotentialWord = true
////                            hasValidLockedTile = true
////                        }
////                        currentCheckYGridNum++
////                    }
////                }
////                
////                verticalTiles.append(self.tileSpriteParent)
////                
////                for tile in tilesToAdd {
////                    verticalTiles.append(tile)
////                }
////                
////                downString = self.tileText.stringByAppendingString(stringToAdd)
////                verticalString = upString.stringByAppendingString(stringToAdd)
////                
////                print(verticalString + " checkForValidWords")
////                
////                if hasValidLockedTile {
////                    for tile in horizontalLeftTiles {
////                        tile.tileSprite.tileGlow.hidden = false
////                    }
////                    for tile in horizontalTiles {
////                        tile.tileSprite.tileGlow.hidden = false
////                    }
////                    for tile in verticalUpTiles {
////                        tile.tileSprite.tileGlow.hidden = false
////                    }
////                    for tile in verticalTiles {
////                        tile.tileSprite.tileGlow.hidden = false
////                    }
////                }
////            }
////          }
//    }
    
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
        print("open locations: l r u d" )
        print(leftOpenTileLocations, rightOpenTileLocations, upOpenTileLocations, downOpenTileLocations)
        return (leftOpenTileLocations, rightOpenTileLocations, upOpenTileLocations, downOpenTileLocations)
    }
    
    func checkForValidWordsAI (var gridXSpot: Int, gridYSpot: Int, availableTiles: [MMWTile], var numLettersToPlay: Int) {  // might have to check if numLettersToPlay more than player tile letters ???
        
        ////////////  TEST FOR ADJACENT TILE PARTIAL WORDS
        var gridTestX = gridXSpot
        var gridTestY = gridYSpot
        
        var lockedBoardTilesArr = mmwGameSceneViewController.mmwGameScene.mmwBoardGrid.getArrayLetterTilesInGrid() // start at locked and move L/R and Up/Down

        var playerLetterTilesArr = mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.getArrayLetterTilesInGrid() // num of tilesToPlay 0 to numTiles in player grid
        var playerTilesLettersArr : [String] = [String]()  // the playerTilesArr in corresponding Letter array
        var playerTilesPermutations : [String] = [String]() // an array of all pemutations to play
        
        var testValidWordTileArr : [MMWTile] = [MMWTile]() // an array of words that makes a valid partial word // if find higher pt array, replace existing (randomly)
        
        for tile in playerLetterTilesArr {                    // prints out letters in player tiles
            print("\(tile.tileText) ", terminator: "")
            playerTilesLettersArr.append(tile.tileText) // adds letters in player tiles to playerTilesLettersArr[]
        }
        print("")      // to add a return on last letter

        //++gridXSpot // just to test letter to right of existing locked letter
        
        
        
        var permutationsToPlay : Set<String> = permuteLetters(playerTilesLettersArr, minStringLen: 3, maxStringLen: 3)
        
        print(permutationsToPlay)
        
        //var permutationsToPlay : Set<String> = Set<String>()    // make and remake permuations based on letters and number of squares to play
        var placedTiles = 0
        var existingPlayedTiles = 0
        var horizontalString = ""               // mmwGameSceneViewController.mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot].letterString
 
        // start 0 L and 1 R, then 1 L and 1 R, etc, accounting for existing tiles, adjoining existing tiles, and edge of board grid
        //
        // edge of grid to test spot MINUS existing letters = possible tile locations
        
        var numEmptyLetterSpotsLeft = calculateOpenTileLocations(gridXSpot, gridYStart: gridXSpot).leftOpenTileLocations // from 0 to num player tiles PLUS existing tiles MINUS numLettersRight AND more than grid x = 0
        var numEmptyLetterSpotsRight = calculateOpenTileLocations(gridXSpot, gridYStart: gridXSpot).rightOpenTileLocations // from 0 to num player tiles PLUS existing tiles MINUS numLettersLeft AND more than grid x = 0

        
        numLettersToPlay = 3 //////////////////////////////// hard code of argument 
        
        
        var numTilesToPlayLeftMaxComplete = (numLettersToPlay <= numEmptyLetterSpotsLeft) ?  numLettersToPlay  :  numEmptyLetterSpotsLeft // the number of tiles to play Left : 0 to < playerTilesArr && < numEmptyLetterSpotsLeft
        var numTilesToPlayRightMaxComplete = (numLettersToPlay <= numEmptyLetterSpotsLeft) ?  numLettersToPlay  :  numEmptyLetterSpotsLeft // the number of tiles to play Left : 0 to < playerTilesArr && < numEmptyLetterSpotsLeft

        var testTile = playerTilesLettersArr[0]
        
        //var testTileGridSpotEnd = self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY]
        
        var testString = self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText // get letter at start tile
        
        var shiftLeft = numTilesToPlayLeftMaxComplete
        var shiftRight = 0
        
        var playerLettersToTest = ""
        var playerLettersToTestLeft = ""
        var playerLettersToTestRight = ""
        
        //for tileSpot in 0..<numTilesToPlayLeftMax { // drop in all the player letters into each availible spots to LEFT of start square

            //playerLetterTilesArr[0].tileSprite.checkForValidWords(gridTestX, gridYSpot: gridTestX, IsAI: true)

        for playerLetters in permutationsToPlay {  // get all permutations of player letters based on number

            shiftLeft = numTilesToPlayLeftMaxComplete
            shiftRight = 0

            for shiftToRight in 0..<playerLetters.characters.count {
//                //print(playerLetters)
//                playerLettersToTest = playerLetters
//                //playerLettersToTestLeft = "L"
//                playerLettersToTestLeft.appendContentsOf(playerLetters)
//                //playerLettersToTestRight = "R"
//                playerLettersToTestRight.appendContentsOf(playerLetters)

                
                while shiftRight <= numTilesToPlayRightMaxComplete {
                    
                    //print(playerLetters)
                    
                    
                    playerLettersToTest = playerLetters
                    playerLettersToTestLeft = playerLetters
                    playerLettersToTestRight = playerLetters
                    var numTilesToPlayLeftMax = numTilesToPlayLeftMaxComplete
                    var numTilesToPlayRightMax = numTilesToPlayRightMaxComplete
                    
                    if playerLettersToTestLeft.characters.count > 0 {
                        let rangeLeft : Range! = playerLettersToTestLeft.endIndex.advancedBy(-shiftRight)..<playerLettersToTestLeft.endIndex
                        playerLettersToTestLeft.removeRange(rangeLeft)  // make copy of letters to manipulate and remove from
                    }
                    if playerLettersToTestRight.characters.count > 0 {
                        let rangeRight : Range! = playerLettersToTestRight.startIndex..<playerLettersToTestRight.endIndex.advancedBy(-shiftRight)
                        playerLettersToTestRight.removeRange(rangeRight)  // make copy of letters to manipulate and remove from
                    }

                    // CHECK LEFT
                    gridTestX = gridXSpot // reset test spot as it may have been moved to right in code below
                    gridTestY = gridYSpot

                    while playerLettersToTestLeft.characters.count > (0) && playerLettersToTestLeft.characters.count <= numTilesToPlayLeftMax {
                        --gridTestX
                        --numTilesToPlayLeftMax
                        if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType != TileType.Letter {
                            var permutationLeftLastLetter = String( playerLettersToTestLeft.removeAtIndex(playerLettersToTestLeft.endIndex.predecessor()) )
                            testString = permutationLeftLastLetter + testString

                            while self.mmwBoardGrid.grid2DArr[gridTestX-1][gridTestY].tileType == TileType.Letter {
                                --gridTestX
                                var existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText)
                                //print("Added existing letter: \(mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText) ")
                                testString = existingLetterToAdd + testString
                                //print (testString)
                            }
                            //print("Added permutation letter: \(existingLetterToAdd) : testString \(testString) ")
                        }
                        
                        else {
                            while self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                                
                                var existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText)
                                //print("Added existing letter: \(mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText) ")
                                testString = existingLetterToAdd + testString
                                //print (testString)
                                --gridTestX
                            }
                        }
                    }
                    
                    gridTestX = gridXSpot // reset test spot as it was moved to left in code above
                    gridTestY = gridYSpot
                    
                    // CHECK RIGHT
                    ++gridTestX
                    // grab existing letters to right even if no shift right characters to test
                    while self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                        var existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText)
                        //print("Added existing letter: \(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText) ")
                        testString = testString + existingLetterToAdd
                        ++gridTestX
                    }

                    while playerLettersToTestRight.characters.count > 0 && playerLettersToTestRight.characters.count <= numTilesToPlayRightMax {
                        --numTilesToPlayRightMax
                        // if NOT a letter then play a letter
                        if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType != TileType.Letter {
                            var permutationRightFirstLetter = String( playerLettersToTestRight.removeAtIndex(playerLettersToTestRight.startIndex))
                            testString = testString + permutationRightFirstLetter
                            // if IS a letter then add a letter(s)
                            while self.mmwBoardGrid.grid2DArr[gridTestX+1][gridTestY].tileType == TileType.Letter {
                                var existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText)
                                //print("Added existing letter: \(mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText) ")
                                testString = testString + existingLetterToAdd
                                ++gridTestX
                                //print (testString)
                            }
                        }
  
                        else {
                            while self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
                                
                                var existingLetterToAdd = String(mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText)
                                //print("Added existing letter: \(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileText) ")
                                testString = testString + existingLetterToAdd
                                ++gridTestX
                            }
                        }
                        //print("Added permutation letter: \(existingLetterToAdd) : testString \(testString) ")
                    }
                    
                    if (self.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(testString) == true ) {
                        print("Horizontal partial Word Match! \(testString)" )
                        if (self.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(testString) == true ) {
                            print("!!! Horizontal WHOLE Word Match! \(testString)" )
                            // letterTile.checkForValidWord  x y
                        }
                    }
                        
                    else {
                        print("NOT Horizontal partial Word Match! \(testString)" )
                    }
                    
                    
                    testString = self.mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot].tileText // get letter at start tile
                    shiftLeft--
                    shiftRight++
                }

            }
            
        }
        //}
        


                

        
        
        //RIGHT//
        
        
        
        
//        testValidWordTileArr.append(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY]) // get letter at initial spot > Locked so MUST be valid
//        
//        for var testTile in playerTilesArr {
//            //sleep(1)
//            while self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter && (gridTestX < 14 ){
//                ++gridTestX
//                existingPlayedTiles++
//                
//                if self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY].tileType == TileType.Letter {
//                    testValidWordTileArr.append(self.mmwBoardGrid.grid2DArr[gridTestX][gridTestY])
//                    testTile.tileSprite.checkForValidWords(gridTestX, gridYSpot: gridTestY, IsAI: true)
//                }
//            }
//            
//            if (testTile.tileSprite.testForValidPartialWordsAtDropSpot(gridTestX, tileSnapResultsYGrid: gridTestY, isAI: true).validHorizontalPartialWord) && (testTile.tileSprite.tileText != "!") {
//                
//                placedTiles++
//                
//                testTile.tileSprite.checkForValidWords(gridTestX, gridYSpot: gridTestY, IsAI: true)
//                
//                mmwGameSceneViewController.dealLetter(&testTile, gridToPlaceLetter: mmwBoardGrid, xSquare: gridTestX, ySquare: gridTestY)
//                
//                testTile.gridXEnd = gridTestX
//                testTile.gridYEnd = gridTestY
//                testTile.gridEnd = mmwBoardGrid
//                testTile.tileSprite.playTileToBoardGrid(Double(placedTiles))
//                
//                //sleep(UInt32(placedTiles))
//                
//                testValidWordTileArr.append(testTile)
//                
//                horizontalString = ""
//                
//                let playerTilesPermutations = permuteLetters(playerTilesLettersArr, minStringLen: 0, maxStringLen: 1)
//                print("Permutations:: \(playerTilesPermutations)")
//                
//                for playerTile in testValidWordTileArr {
//                    horizontalString = horizontalString + playerTile.tileSprite.tileText
//                }
//                
//                print(horizontalString)
//                
//                if (( self.mmwGameSceneViewController.checkWholeWordMatch(horizontalString) ) == false || horizontalString.characters.count < mmwGameSceneViewController!.minWordSize  ) {
//                    // updates GUI for feedback on horizonal partial word
//                    updatePartialWordFeedback("\(horizontalString) is a partial word (horizontalString)")
//                    print ( "\(horizontalString) is a partial word (horizontalString)" )
//                }
//                    
//                else {
//                    print ( "\(horizontalString) is a valid word (horizontalString)" )
//                    //sleep(UInt32(placedTiles))
//                    testTile.tileSprite.scoreTilesInArr(testValidWordTileArr, wordString: horizontalString)
//                }
//                
//                if (testTile.tileState != TileState.Locked) {
//                    testTile.tileState = TileState.Played // if put on valid board location set TileState to played if NOT already locked
//                }
//                
//                // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
//                let replacementPlaceholderTile : MMWTile = MMWTile()
//                replacementPlaceholderTile.gridHome = testTile.gridHome
//                replacementPlaceholderTile.gridX = testTile.gridX
//                replacementPlaceholderTile.gridY = testTile.gridY
//                testTile.gridHome?.grid2DArr[testTile.gridX][testTile.gridY] = replacementPlaceholderTile
//                
//                // set value of snap results grid location to the MMWTile if valid location
//                testTile.gridEnd? = testTile.gridEnd!
//                testTile.gridEnd?.grid2DArr[testTile.gridX][testTile.gridY] = testTile
//                testTile.gridXEnd = testTile.gridX
//                testTile.gridYEnd = testTile.gridY
//                testTile.tileSprite.isMovable = false // so can't remove tile once placed
//                testTile.tileSprite.userInteractionEnabled = false
//                testTile.tileSprite.removeBoardTileHighlights ()
//                testTile.gridHome?.mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
//                
//            }
//            
//        }
//        
//        //        gridTestX--
//        //        horizontalString = ""
//        //
//        //        for playerTile in 0..<(existingPlayedTiles) {
//        //
//        //            if (gridTestX) >= 0 {
//        //
//        //                horizontalString = mmwGameSceneViewController.mmwGameScene.mmwBoardGrid.grid2DArr[gridTestX--][gridYSpot].tileText + horizontalString
//        //
//        //            }
//        //        }
//        //        print(horizontalString)
//        
//        for char in horizontalString.lowercaseString.characters {
//            //print(char.debugDescription.lowercaseString)
//            
//            if String(char) == "!" {
//                print("X", terminator: "")
//            }
//            else {
//                print("\(char)", terminator: "")
//            }
//        }
//        
//        //                var stringToAdd : String = ""
//        //                tilesToAdd = [MMWTile]()
//        //
//        //                while ( (currentCheckXGridNum > 0) && (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Played) ) )  {
//        //
//        //                    numHorizontalTiles++
//        //                    tilesToAdd.append((gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot]))
//        //
//        //                    let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].letterString)"
//        //                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd) // (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].letterString)
//        //
//        //
//        //                    if gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked {
//        //                        foundLockedTile = true // makes sure locked tile in potential word
//        //                        hasLockedPotentialWord = true
//        //                        hasValidLockedTile = true
//        //                    }
//        //                    currentCheckXGridNum--
//        //                }
//        //
//        //                for char in stringToAdd.characters {
//        //                    leftString = (String(char) + leftString)
//        //                }
//        //                leftString = leftString.stringByAppendingString(self.tileText) // add current letter string to leftward letters
//        //
//        //                for tile in tilesToAdd {
//        //                    horizontalLeftTiles.append(tile)
//        //                }
//        //                horizontalLeftTiles = horizontalLeftTiles.reverse()
//        //                horizontalLeftTiles.append(self.tileSpriteParent) // add current tile to leftward tiles
//        
//        
//        
//        
//        
//        
//        //                //RIGHT
//        //                currentCheckXGridNum = gridXSpot
//        //                currentCheckYGridNum = gridYSpot
//        //                stringToAdd = ""
//        //                tilesToAdd = [MMWTile]()
//        //
//        //                while ( (currentCheckXGridNum < 14) && (gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Played) ) )  {
//        //
//        //                    numHorizontalTiles++
//        //                    tilesToAdd.append((gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot]))
//        //
//        //                    let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].letterString)"
//        //                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
//        //
//        //                    if gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Locked {
//        //                        hasLockedPotentialWord = true
//        //                        hasValidLockedTile = true
//        //                    }
//        //                    currentCheckXGridNum++
//        //                }
//        //
//        //                for tile in tilesToAdd {
//        //                    horizontalTiles.append(tile)
//        //                }
//        //
//        //                horizontalString = leftString.stringByAppendingString(stringToAdd)
//        //
//        //                print(horizontalString + " checkForValidWords")
//        //
//        //                ////////////////////////////
//        //
//        //                // UP
//        //                currentCheckXGridNum = gridXSpot
//        //                currentCheckYGridNum = gridYSpot
//        //                stringToAdd = ""
//        //                tilesToAdd = [MMWTile]()
//        //
//        //                while ( (currentCheckYGridNum > 0) && (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Locked || (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Played) ) )  {
//        //                    numVerticalTiles++
//        //                    tilesToAdd.append((gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1]))
//        //
//        //                    let letterToAdd : String = "\(gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].letterString)"
//        //                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd) // (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].letterString)
//        //
//        //                    if gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Locked {
//        //                        foundLockedTile = true // stops check on last last locked tile
//        //                        hasLockedPotentialWord = true
//        //                        hasValidLockedTile = true
//        //                    }
//        //                    currentCheckYGridNum--
//        //                }
//        //
//        //                for char in stringToAdd.characters {
//        //                    upString = (String(char) + upString)
//        //                }
//        //                upString = upString.stringByAppendingString(self.tileText) // add current letter string to upward letters
//        //
//        //                for tile in tilesToAdd {
//        //                    verticalUpTiles.append(tile)
//        //                }
//        //
//        //                verticalUpTiles = verticalUpTiles.reverse()
//        //                verticalUpTiles.append(self.tileSpriteParent)
//        //
//        //                //DOWN
//        //                currentCheckXGridNum = gridXSpot
//        //                currentCheckYGridNum = gridYSpot
//        //                stringToAdd = ""
//        //                tilesToAdd = [MMWTile]()
//        //
//        //                while ( (currentCheckYGridNum < 14) && (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum][currentCheckYGridNum + 1].tileState == TileState.Played) ) )  {
//        //                    numVerticalTiles++
//        //                    tilesToAdd.append((gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1]))
//        //
//        //                    let letterToAdd : String = "\(gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].letterString)"
//        //                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
//        //
//        //                    if currentCheckYGridNum != 0 {
//        //                        if gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked {
//        //                            foundLockedTile = true // stops check on last last locked tile
//        //                            hasLockedPotentialWord = true
//        //                            hasValidLockedTile = true
//        //                        }
//        //                        currentCheckYGridNum++
//        //                    }
//        //                }
//        //                
//        //                verticalTiles.append(self.tileSpriteParent)
//        //                
//        //                for tile in tilesToAdd {
//        //                    verticalTiles.append(tile)
//        //                }
//        //                
//        //                downString = self.tileText.stringByAppendingString(stringToAdd)
//        //                verticalString = upString.stringByAppendingString(stringToAdd)
//        //                
//        //                print(verticalString + " checkForValidWords")
//        //                
//        //                if hasValidLockedTile {
//        //                    for tile in horizontalLeftTiles {
//        //                        tile.tileSprite.tileGlow.hidden = false
//        //                    }
//        //                    for tile in horizontalTiles {
//        //                        tile.tileSprite.tileGlow.hidden = false
//        //                    }
//        //                    for tile in verticalUpTiles {
//        //                        tile.tileSprite.tileGlow.hidden = false
//        //                    }
//        //                    for tile in verticalTiles {
//        //                        tile.tileSprite.tileGlow.hidden = false
//        //                    }
//        //                }
//        //            }
//        //          }
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
    

  
    func presentMenuScene() {
        let transition = SKTransition.crossFadeWithDuration(0.5)
        let menuScene = MenuScene(size: size,
            gameResult: true,
            score: 123)
        view?.presentScene(menuScene, transition: transition)
    }
}

 