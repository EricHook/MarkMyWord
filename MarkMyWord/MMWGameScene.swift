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

    var player1 : MMWPlayer?
    var player2 : MMWPlayer?
    var player3 : MMWPlayer?
    var player4 : MMWPlayer?
    
    var player1View : PlayerView?
    var player2View : PlayerView?
    var player3View : PlayerView?
    var player4View : PlayerView?

    var partialWordLabel    = SKLabelNode(fontNamed: FontHUDName)
    var timeRemainingLabel  = SKLabelNode(fontNamed: FontHUDName)
    var tilesRemainingLabel = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel     = SKLabelNode(fontNamed: FontHUDName)
    var topDisplayLabel2    = SKLabelNode(fontNamed: FontHUDName)
    
    var placeholderTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    
    private var secondsLeft: Int = 0
    private var timer: NSTimer?

    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        viewSize = size
        print("view size: \(viewSize)")  //  var viewSize = self.view!.bounds.size
        
        self.mmwBoardGrid = Grid(gridUpperLeftX: 156, gridUpperLeftY: (67), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15, gridName: "mmwBoardGrid") // 21
        
        self.mmwPlayer1of2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1of2Grid") //y 304
        self.mmwPlayer2of2Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.45830), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2of2Grid")
        
        self.mmwPlayer1of4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer1of4Grid") // 0.4583
        self.mmwPlayer2of4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.273), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer2of4Grid")
        self.mmwPlayer3of4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.0058), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer3of4Grid")
        self.mmwPlayer4of4Grid = Grid(gridUpperLeftX: Double(viewSize.width * 0.8550), gridUpperLeftY: Double(viewSize.height * 0.706), gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "mmwPlayer4of4Grid")
    }
    
    func setViewController (mmwGameSceneController: MMWGameSceneViewController) {
        self.mmwGameSceneViewController = mmwGameSceneController
    }

    func changePlayerScoreDisplay (playerView: PlayerView, player: MMWPlayer, score: Int) {
        player.playerScore = score
        playerView.playerScoreLabel.text = String(score)
    }
    
    func buildGameView () {
        print("buildGameView() in MMWGameScene")
        backgroundColor = SKColor(red: 0.5, green: 0.0, blue: 0.0, alpha: 1.0)
        userInteractionEnabled = true
        // add BG
        backgroundNode = SKSpriteNode( imageNamed: "MMWBG" )
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
        
        let playBtn = SKSpriteNode(imageNamed: "TileBackTest90x90")
        playBtn.position = CGPoint(x: viewSize.width * 0.5, y: viewSize.height * 0.5)
        playBtn.size.width = playBtn.size.width * 2
        playBtn.name = "playBtn"
        playBtn.alpha = 0.7
        self.addChild(playBtn)
         
        let pauseBtn = SKSpriteNode(texture: placeholderTexture, color: UIColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 0.00), size: CGSizeMake(50.0, 50.0))
        pauseBtn.position = CGPoint(x: viewSize.width * 0.93, y: viewSize.height * 0.12)
        pauseBtn.size.width = playBtn.size.width * 1.0
        pauseBtn.size.height = playBtn.size.height * 0.55
        pauseBtn.alpha = 0.5
        pauseBtn.name = "pauseBtn"
        self.addChild(pauseBtn)
        
        let optionsBtn = SKSpriteNode(imageNamed: "TileBackTest90x90")
        optionsBtn.position = CGPoint(x: viewSize.width * 0.925, y: viewSize.height * 0.02)
        optionsBtn.size.width = playBtn.size.width * 0.9
        optionsBtn.size.height = playBtn.size.height * 0.65
        optionsBtn.alpha = 0.5
        optionsBtn.name = "optionsBtn"
        self.addChild(optionsBtn)
    }
    
    func showTiles (tileBuilder: MMWTileBuilder) {
        
    }
    
    func showTilesInSquares (tileBuilder: MMWTileBuilder) {
        var location = CGPoint(x: 100, y: 100 )
        location.y = viewSize.height - location.y
        let snapGrid : Grid = getSnapGrid(location)!
        let gridSquareX = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
        let gridSquareY = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
        let gridSquareXCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerX
        let gridSquareYCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareUpperLeftCornerY
        print("<MMWGameScene> snapGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")

        var tileArrSelectedTile = 0
        let actionSound = SKAction.playSoundFileNamed("PLINK.mp3", waitForCompletion: true)
        runAction(actionSound)

        tileBuilder.mmwPlayer1LetterTileArray[2] = MMWTile(letterString: "%")

        for y in 0...1 {   // fill Player 1 letter tiles
            for x in 0...2 {
                if tileBuilder.mmwPlayer1LetterTileArray.count > tileArrSelectedTile {
                    let tileAdded : MMWTile = tileBuilder.mmwPlayer1LetterTileArray[tileArrSelectedTile]
                    if mmwGameSceneViewController.numPlayers == 2 {
                        tileAdded.tileSprite.position = self.mmwPlayer1of2Grid.sendToGridSquare(self.mmwPlayer1of2Grid, squareX: x, squareY: y)
                    }
                    if mmwGameSceneViewController.numPlayers == 3 {
                        tileAdded.tileSprite.position = self.mmwPlayer1of2Grid.sendToGridSquare(self.mmwPlayer1of2Grid, squareX: x, squareY: y)
                    }
                    else if mmwGameSceneViewController.numPlayers == 4 {
                        tileAdded.tileSprite.position = self.mmwPlayer1of4Grid.sendToGridSquare(self.mmwPlayer1of4Grid, squareX: x, squareY: y)
                    }
                    // tileAdded.tileSprite.color = tileAdded.tileSprite.tileColors[ Int(arc4random_uniform(4)) ]
                    //tileAdded.tileSprite.color =  player1View!.color  // UIColorAppleRed // player1?.playerSeat.seatUIColor
                    self.addChild(tileAdded.tileSprite)
                    tileArrSelectedTile++
                }
            }
        }
            
        tileArrSelectedTile = 0
        //let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
        runAction(actionSound)
        for y in 0...1 {   // fill Player 2 letter tiles
            for x in 0...2 {
                if tileBuilder.mmwPlayer2LetterTileArray.count > tileArrSelectedTile {
                    let tileAdded : MMWTile = tileBuilder.mmwPlayer2LetterTileArray[tileArrSelectedTile]
                    if mmwGameSceneViewController.numPlayers == 2 {
                        tileAdded.tileSprite.position = self.mmwPlayer2of2Grid.sendToGridSquare(self.mmwPlayer2of2Grid, squareX: x, squareY: y)
                    }
                    if mmwGameSceneViewController.numPlayers == 3 {
                        tileAdded.tileSprite.position = self.mmwPlayer2of4Grid.sendToGridSquare(self.mmwPlayer2of4Grid, squareX: x, squareY: y)
                    }
                    else if mmwGameSceneViewController.numPlayers == 4 {
                        tileAdded.tileSprite.position = self.mmwPlayer2of4Grid.sendToGridSquare(self.mmwPlayer2of4Grid, squareX: x, squareY: y)
                    }
                    //tileAdded.tileSprite.color = tileAdded.tileSprite.tileColors[ Int(arc4random_uniform(4)) ]
                    // tileAdded.tileSprite.color = (player2?.playerSeat.seatUIColor)!
                    //tileAdded.tileSprite.color =  UIColorApplePurple // player1?.playerSeat.seatUIColor
                    self.addChild(tileAdded.tileSprite)
                    tileArrSelectedTile++
                }
            }
        }
        
        if mmwGameSceneViewController.numPlayers == 3 {
            runAction(actionSound)
            tileArrSelectedTile = 0
            for y in 0...1 {   // fill Player 3 letter tiles
                for x in 0...2 {
                    if tileBuilder.mmwPlayer3LetterTileArray.count > tileArrSelectedTile {
                        let tileAdded : MMWTile = tileBuilder.mmwPlayer3LetterTileArray[tileArrSelectedTile]
                        tileAdded.tileSprite.position = self.mmwPlayer4of4Grid.sendToGridSquare(self.mmwPlayer4of4Grid, squareX: x, squareY: y)
                        self.addChild(tileAdded.tileSprite)
                        tileArrSelectedTile++
                    }
                }
            }
        }

        if mmwGameSceneViewController.numPlayers == 4 {
            runAction(actionSound)
            tileArrSelectedTile = 0
            for y in 0...1 {   // fill Player 3 letter tiles
                for x in 0...2 {
                    if tileBuilder.mmwPlayer3LetterTileArray.count > tileArrSelectedTile {
                        let tileAdded : MMWTile = tileBuilder.mmwPlayer3LetterTileArray[tileArrSelectedTile]
                        tileAdded.tileSprite.position = self.mmwPlayer3of4Grid.sendToGridSquare(self.mmwPlayer3of4Grid, squareX: x, squareY: y)
                        self.addChild(tileAdded.tileSprite)
                        tileArrSelectedTile++
                    }
                }
            }
            
            tileArrSelectedTile = 0
            runAction(actionSound)
            for y in 0...1 {   // fill Player 4 letter tiles
                for x in 0...2 {
                    if tileBuilder.mmwPlayer4LetterTileArray.count > tileArrSelectedTile {
                        let tileAdded : MMWTile = tileBuilder.mmwPlayer4LetterTileArray[tileArrSelectedTile]
                        tileAdded.tileSprite.position = self.mmwPlayer4of4Grid.sendToGridSquare(self.mmwPlayer4of4Grid, squareX: x, squareY: y)
                        self.addChild(tileAdded.tileSprite)
                        tileArrSelectedTile++
                    }
                }
            }
        }
    
        tileArrSelectedTile = 0
        runAction(actionSound)
        for y in 0...14 {  // fill Board letter tiles
            for x in 0...14 {
                if tileBuilder.mmwPlayedTileArray.count > tileArrSelectedTile {
                    let tileAdded : MMWTile = tileBuilder.mmwPlayedTileArray[tileArrSelectedTile]
                    tileAdded.tileSprite.position = self.mmwBoardGrid.sendToGridSquare(self.mmwBoardGrid, squareX: x, squareY: y)
                    tileAdded.tileSprite.color = tileAdded.tileSprite.TileColors[ Int(arc4random_uniform(4)) ]
                    self.addChild(tileAdded.tileSprite)
                    tileArrSelectedTile++
                }
            }
        }
        
        var testTile = MMWTile(letterString: "$")
        testTile.gridX = 7
        testTile.gridY = 7
        testTile.tileSprite.position = mmwBoardGrid.sendToGridSquare(mmwBoardGrid, squareX: 8, squareY: 9)
        
        self.addChild(testTile.tileSprite)
        
        //self.mmwBoardGrid.sendToGridSquare(mmwBoardGrid, squareX: 7, squareY: 7) // returns position
        
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
        timeRemainingLabel.zPosition = 1
        timeRemainingLabel.text = "Timer: \(timeAmount)"
        timeRemainingLabel.fontSize = FontHUDSize
        timeRemainingLabel.fontColor = FontHUDRed
        timeRemainingLabel.position = CGPointMake(size.width/2.0 + 255.0, 3)
        timeRemainingLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(timeRemainingLabel)
        return timeRemainingLabel
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
    
    func testOutput () {
        print("testOutput")
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
            // if only 2 players - one on left and one on right of UI
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

    func getBoardGrid () -> (Grid) {
        return mmwBoardGrid
    }
    
//    func addTile (letterTile : Character, location : CGPoint){
//        var tile = SKSpriteNode(imageNamed: "LetterTest90x90")
//        tile.position = location
//        tile.anchorPoint = CGPointMake(0, 0)
//        foregroundNode!.addChild(tile)
//    }
    
    func updatePlayerTileRack (mmwPlayer: MMWPlayer) {
        
        
        
//        tileArrSelectedTile = 0
//        for y in 0...1 {   // fill Player 3 letter tiles
//            for x in 0...2 {
//                if tileBuilder.mmwPlayer3LetterTileArray.count > tileArrSelectedTile {
//                    let tileAdded : MMWTile = tileBuilder.mmwPlayer3LetterTileArray[tileArrSelectedTile]
//                    tileAdded.tileSprite.position = self.MMWPlayer4of4Grid.sendToGridSquare(self.MMWPlayer4of4Grid, squareX: x, squareY: y)
//                    self.addChild(tileAdded.tileSprite)
//                    tileArrSelectedTile++
//                }
//            }
//        }
        
    }

    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
        for touch: AnyObject in touches {
            let location = touch.locationInNode(self)
            let _node:SKNode = self.nodeAtPoint(location)
            
            if(_node.name == "playBtn"){
                //print("going to mmw scene") //create MMW controller
                //mmwGameSceneViewController = createMMWSceneController()
                //mmwGameSceneViewController.testMMWCont() // print to test
                //mmwGameSceneViewController.mmwGameScene.setViewController(mmwGameSceneViewController)
                //presentMMWScene()
                // mmwGameScene.scaleMode = SKSceneScaleMode.ResizeFill
                if userInteractionEnabled {
                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
                    runAction(actionSound)
                    showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    _node.removeFromParent()
                }
            }
            
            if(_node.name == "optionsBtn"){
                //print("going to mmw scene") //create MMW controller
                //mmwGameSceneViewController = createMMWSceneController()
                //mmwGameSceneViewController.testMMWCont() // print to test
                //mmwGameSceneViewController.mmwGameScene.setViewController(mmwGameSceneViewController)
                //presentMMWScene()
                // mmwGameScene.scaleMode = SKSceneScaleMode.ResizeFill
                if userInteractionEnabled {
                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
                    runAction(actionSound)
                    //showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    _node.removeFromParent()
                }
            }
            
            if(_node.name == "pauseBtn"){
                //print("going to mmw scene") //create MMW controller
                //mmwGameSceneViewController = createMMWSceneController()
                //mmwGameSceneViewController.testMMWCont() // print to test
                //mmwGameSceneViewController.mmwGameScene.setViewController(mmwGameSceneViewController)
                //presentMMWScene()
                // mmwGameScene.scaleMode = SKSceneScaleMode.ResizeFill
                if userInteractionEnabled {
                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
                    runAction(actionSound)
                    //showTilesInSquares(mmwGameSceneViewController.tileCollection) // 'deals' player tiles and shows demo tiles on board for testing
                    _node.removeFromParent()
                }
            }
            
//            if(_node.name == "tile"){
//                //print("going to mmw scene") //create MMW controller
//                //mmwGameSceneViewController = createMMWSceneController()
//                //mmwGameSceneViewController.testMMWCont() // print to test
//                //mmwGameSceneViewController.mmwGameScene.setViewController(mmwGameSceneViewController)
//                //presentMMWScene()
//                // mmwGameScene.scaleMode = SKSceneScaleMode.ResizeFill
//                if userInteractionEnabled {
//                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//                    runAction(actionSound)
//                    print("Selected Tile \(self.name)")
//                }
//            }

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
    
    override func update(currentTime: NSTimeInterval) {
    }
    
    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        tilesRemainingLabel.zPosition = 1
        tilesRemainingLabel.text = "TilesRemainTouchMove"
        tilesRemainingLabel.text = String(987)
        
        player2View?.playerScoreLabel.zPosition = 1
        player2View?.playerScoreLabel.text = "MMWGameSceneTouchesMoved"
    }
    
    func presentMenuScene() {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        let menuScene = MenuScene(size: size,
            gameResult: true,
            score: 123)
        view?.presentScene(menuScene, transition: transition)
    }
    
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

 