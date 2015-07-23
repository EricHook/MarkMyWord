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
    
    var mmwPlayer1Grid : Grid!
    var mmwPlayer2Grid : Grid!
    var mmwPlayer3Grid : Grid!
    var mmwPlayer4Grid : Grid!
    
//    var player1View : PlayerView! = mmwGameScene.addPlayerView(1, playerView: PlayerView(mmwGameSceneCont  mmwPlayer: player1))
//    mmwGameScene.addPlayerView(1, playerView: PlayerView(mmwPlayer: player1))
    
    var player1View : PlayerView! // = addPlayerView(1, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player1))
    var player2View : PlayerView!
    var player3View : PlayerView!
    var player4View : PlayerView!
    
    //                player1View = addPlayerView(1, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player1))
    //                mmwGameSceneViewController.player1.setPlayerView(player1View)

    var partialWordLabel    = SKLabelNode(fontNamed: FontHUDName)
    var timeRemainingLabel  = SKLabelNode(fontNamed: FontHUDName)
    private var secondsLeft: Int = 17
    var tilesRemainingLabel = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel     = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel2    = SKLabelNode(fontNamed: FontHUDName)
    
    let playButton = SKSpriteNode(imageNamed: "PlayButton.png")
    let newTilesButton = SKSpriteNode(imageNamed: "NewTilesButton.png")
    let passButton = SKSpriteNode(imageNamed: "PassButton.png")
    let pauseButton = SKSpriteNode(imageNamed: "PauseButton.png")
    let optionsButton = SKSpriteNode(imageNamed: "OptionsButton.png")
    
    var placeholderTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    
    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
    let dealTilesSound = SKAction.playSoundFileNamed("PLINK.mp3", waitForCompletion: true)

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
    
    func setGrids () {
        self.mmwBoardGrid = Grid(gridUpperLeftX: 156, gridUpperLeftY: (67), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15, gridName: "mmwBoardGrid", mmwGameScene: self)
        
        if mmwGameSceneViewController.numPlayers == 2 {
            self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1Grid", mmwGameScene: self) //y 304
            self.mmwPlayer1Grid.setGridPlayer(mmwGameSceneViewController.player1)
            
            self.mmwPlayer2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2Grid", mmwGameScene: self)
            self.mmwPlayer2Grid.setGridPlayer(mmwGameSceneViewController.player2)
        }

        if mmwGameSceneViewController.numPlayers == 3 {
            self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1Grid", mmwGameScene: self) // 0.4583
            self.mmwPlayer1Grid.setGridPlayer(mmwGameSceneViewController.player1)
            
            self.mmwPlayer2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2Grid", mmwGameScene: self)
            self.mmwPlayer2Grid.setGridPlayer(mmwGameSceneViewController.player2)
            
            self.mmwPlayer3Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer3Grid", mmwGameScene: self)
            self.mmwPlayer3Grid.setGridPlayer(mmwGameSceneViewController.player3)
        }
        
        if mmwGameSceneViewController.numPlayers == 4 {
            self.mmwPlayer1Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1Grid", mmwGameScene: self) // 0.4583
            self.mmwPlayer1Grid.setGridPlayer(mmwGameSceneViewController.player1)
            
            self.mmwPlayer2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2Grid", mmwGameScene: self)
            self.mmwPlayer2Grid.setGridPlayer(mmwGameSceneViewController.player2)
            
            self.mmwPlayer3Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer3Grid", mmwGameScene: self)
            self.mmwPlayer3Grid.setGridPlayer(mmwGameSceneViewController.player3)
            
            self.mmwPlayer4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer4Grid", mmwGameScene: self)
            self.mmwPlayer4Grid.setGridPlayer(mmwGameSceneViewController.player4)
        }
    }
 
    func changePlayerScoreDisplay (playerView: PlayerView, player: Player, score: Int) {
        player.playerScore = score
        playerView.playerScoreLabel.text = String(score)
    }
    
    func buildGameView () {
        //print("buildGameView() in MMWGameScene")
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
        
        
        player1View = addPlayerView(1, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player1))
        player2View = addPlayerView(2, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player2))
        player3View = addPlayerView(3, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player3))
        player4View = addPlayerView(4, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player4))
        
        
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

        optionsButton.position = CGPoint(x: viewSize.width * 0.9228, y: viewSize.height * 0.04)
        optionsButton.name = "optionsButton"
        self.addChild(optionsButton)
        
        // ADD ALL TILES TO Scene - they start as invisible
        for tile in mmwGameSceneViewController.tileCollection.mmwTileArray {
            tile.tileSprite.hidden = false /////
            self.addChild(tile.tileSprite)
        }
    }
    
    func showAllGridTiles (gridToDisplay: Grid) {
        runAction(dealTilesSound)
        for x in 0...gridToDisplay.gridNumSquaresX - 1 {   // fill Player 1 letter tiles : 2 in y and 3 in x
            for y in 0...gridToDisplay.gridNumSquaresY - 1 {
                let tileAdded : MMWTile = gridToDisplay.grid2DArr[x][y] // tileBuilder.mmwPlayer1LetterTileArray[tileArrSelectedTile]
                tileAdded.tileSprite.hidden = false
                tileAdded.tileSprite.position = gridToDisplay.sendToGridSquare(gridToDisplay, squareX: x, squareY: y)
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
    }

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

    func addPlayerView (playerNum : Int, playerView : PlayerView) -> PlayerView {
        if mmwGameSceneViewController.numPlayers == 2 {
            // if only 2 players - one on left and one on right of UI
            if playerNum == 1 {
//                
//                player1View = addPlayerView(1, playerView: PlayerView(mmwPlayer: mmwGameSceneViewController.player1))
//                mmwGameSceneViewController.player1.setPlayerView(player1View)
                
                playerView.position = CGPointMake(0, self.size.height * 0.384765625 ) // PS coordinates is Y: 1390, convert and flip .. x , 295.5
                //mmwGameSceneViewController.player1.setPlayerView(&<#T##playerView: PlayerView##PlayerView#>)
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
        playerView.zPosition = -1
        addChild(playerView)
        return playerView
    }
    
    func updateGridInScene (gridToUpdate: Grid) {  // sets position of tile in grid to proper screen location
        for y in 0...(gridToUpdate.gridNumSquaresY - 1) {
            for x in 0...(gridToUpdate.gridNumSquaresX - 1) {
                let tileToUpdate : MMWTile = gridToUpdate.grid2DArr[x][y]
                tileToUpdate.tileSprite.tileLocation = gridToUpdate.sendToGridSquare(gridToUpdate, squareX: x, squareY: y)
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
                    print(">>> PLAY BUTTON PRESSED >>>")
                    runAction(actionSound)
                    
                    mmwPlayer1Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 1)
                    mmwPlayer2Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 2)
                    
                    if mmwGameSceneViewController.numPlayers > 2 {
                        mmwPlayer3Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 3)
                    }
                    
                    if mmwGameSceneViewController.numPlayers > 3 {
                        mmwPlayer4Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 4)
                    }
                    
                    showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    
                    _node.removeFromParent() // gets rid of play button in middle of screen  
                    
                    mmwPlayer1Grid.makeTilesInGridInteractive(true)
                }
            }
            
            if(_node.name == "newTilesButton"){
                if userInteractionEnabled {
                    print(">>> NEW TILES BUTTON PRESSED >>>")
                    runAction(actionSound)
                    
                    mmwGameSceneViewController.playerArray[mmwGameSceneViewController.playerTurn - 1].playerLetterGrid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: (mmwGameSceneViewController.playerTurn))
                    
//                    mmwPlayer1Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 1)
//                    mmwPlayer2Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 2)
//                    
//                    if mmwGameSceneViewController.numPlayers > 2 {
//                        mmwPlayer3Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 3)
//                    }
//                    
//                    if mmwGameSceneViewController.numPlayers > 3 {
//                        mmwPlayer4Grid.dealGridFromArrayRandom(&mmwGameSceneViewController.tileCollection.mmwTileArray, numTilesToDeal: 6, playerNum: 4)
//                    }
                    
                    showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    
                    changePlayerTurn()
                }
                
                tilesRemainingLabel.text = "Tiles Left: \(mmwGameSceneViewController.tileCollection.mmwTileArray.count  )" // " (mmwGameSceneViewController.tileCollection.mmwTileArray.count))"
                if mmwGameSceneViewController.tileCollection.mmwTileArray.count <= 0 {
                    tilesRemainingLabel.text = "Tiles Left: None"
                }
            }
  
            if(_node.name == "passButton"){
                if userInteractionEnabled {
                    runAction(actionSound)
                    print("SHOW TILE ARRAYS (undealt and discarded)")
                    mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwTileArray)
                    mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwDiscardedTileArray)
                    changePlayerTurn()
                }
            }
            
            if(_node.name == "optionsButton"){
                if userInteractionEnabled {
                    runAction(actionSound)
                    
//                    print("SHOW TILE ARRAYS")
//                    print("mmwGameSceneViewController.tileCollection.tileCollection.mmwDiscardedTileArray: ")
//                    mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwDiscardedTileArray)
//                    print("mmwGameSceneViewController.tileCollection.mmwTileArray: ")
//                    mmwGameSceneViewController.tileCollection.displayTileArrayValues(mmwGameSceneViewController.tileCollection.mmwTileArray)
                    
                    print("SHOW TILE GRIDS (player tiles and board grid)")
                    print(">>>mmwGameSceneViewController.mmwGameScene.mmwPlayer1Grid: ")
                    mmwPlayer1Grid.printGrid()
                    print(">>>mmwGameSceneViewController.mmwGameScene.mmwPlayer2Grid: ")
                    mmwPlayer2Grid.printGrid()
                    if mmwGameSceneViewController.numPlayers > 2 {
                        print(">>>mmwGameSceneViewController.mmwGameScene.mmwPlayer3Grid: ")
                        mmwPlayer3Grid.printGrid()
                    }
                    if mmwGameSceneViewController.numPlayers > 3 {
                        print(">>>mmwGameSceneViewController.mmwGameScene.mmwPlayer4Grid: ")
                        mmwPlayer4Grid.printGrid()
                    }
                    print(">>>mmwGameSceneViewController.mmwGameScene.mmwBoardGrid: ")
                    mmwBoardGrid.printGrid()
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
    }
    
    func changePlayerTurn () {
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
            if (testSpot.x < 154.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer3Grid}
            if (testSpot.x > 869.5 ) && testSpot.y > 384 {snapGrid = mmwPlayer4Grid}
        }
        //else {snapGrid = nil}
        print("<MMWGameScene.GetSnapGrid> \(testSpot.x), \(testSpot.y), and grid \(snapGrid), and \(snapGrid.gridName)" )
        return snapGrid
    }
    
    
    func numEmptyTileSlots (tileGrid: Grid) -> Int {
        var numEmpty = 0
        
        for arrayX in tileGrid.grid2DArr {
            for arrayY in arrayX {
                if arrayY.letterString == "!" {
                    numEmpty++
                }
            }
        }
        print("<MMWGameScene>numEmptyTileSlots ( = 0) in grid : \(numEmpty) " )
        return numEmpty
    }
    
//    override func update(currentTime: NSTimeInterval) {
//        // runAction(actionSound)
//    }
    
    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        tilesRemainingLabel.zPosition = 1
        tilesRemainingLabel.text = "TilesRemaing: xxx"
        
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
    
}

 