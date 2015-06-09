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
    
    var backgroundNode : SKSpriteNode?
    var foregroundNode : SKSpriteNode?
    //var spriteTest : SKSpriteNode?
    //var impulseCount = 4
    //var diagnosticText : SKLabelNode?
    let MMWBoardGrid: Grid = Grid(gridLowerLeftX: 157, gridLowerLeftY: 21, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15, gridName: "MMWBoardGrid")
    
    let MMWPlayer1Grid: Grid = Grid(gridLowerLeftX: 7, gridLowerLeftY: 307, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "MMWPlayer1Grid")
    let MMWPlayer2Grid: Grid = Grid(gridLowerLeftX: 877, gridLowerLeftY: 307, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "MMWPlayer2Grid")
    
    let MMWPlayer1of4Grid: Grid = Grid(gridLowerLeftX: 7, gridLowerLeftY: 448.5, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "MMWPlayer1of4Grid")
    let MMWPlayer2of4Grid: Grid = Grid(gridLowerLeftX: 877, gridLowerLeftY: 116.5, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "MMWPlayer2of4Grid")
    let MMWPlayer3of4Grid: Grid = Grid(gridLowerLeftX: 7, gridLowerLeftY: 448.5, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "MMWPlayer3of4Grid")
    let MMWPlayer4of4Grid: Grid = Grid(gridLowerLeftX: 877, gridLowerLeftY: 116.5, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3, gridName: "MMWPlayer4of4Grid")
    
    var numPlayers : Int = 2
    
//    let Player1NameLoc : CGPoint = CGPointMake(20, 768 - 325)
//    let Player1ScoreLoc : CGPoint = CGPointMake(20, 768 - 325)
//    let Player2NameLoc : CGPoint = CGPointMake(880, 768 - 325)
//    let Player2ScoreLoc : CGPoint = CGPointMake(880, 768 - 325)
//    let Player3NameLoc : CGPoint = CGPointMake(20, 768 - 550)
//    let Player3ScoreLoc : CGPoint = CGPointMake(20, 768 - 550)
//    let Player4NameLoc : CGPoint = CGPointMake(880, 768 - 550)
//    let Player4ScoreLoc : CGPoint = CGPointMake(880, 768 - 550)
    
    private var secondsLeft: Int = 0
    private var timer: NSTimer?
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        // bottom left corner of game board
        //var xPosition : CGFloat = 157.0
        //var yPosition : CGFloat = 21.0
        backgroundColor = SKColor(red: 0.5, green: 0.0, blue: 0.0, alpha: 1.0)
        userInteractionEnabled = true
        // add BG
        backgroundNode = SKSpriteNode(imageNamed: "MarkMyWordBGCleaniPad")
        backgroundNode!.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode!.position = CGPoint(x: size.width/2.0, y: 0.0)
        backgroundNode?.userInteractionEnabled = false
        backgroundNode?.zPosition = -100
        backgroundNode?.alpha = 0.5
        addChild(backgroundNode!)
        
        addFillerData()
    }
    
    func addFillerData () {
        //        ///////////////////////////
        //addPlayerView("PlayerFred")
        //        //////////////////////////
        //        var player1 : MMWPlayer = MMWPlayer(_playerID: 11, _playerName: "FredPlayer1")
        //        addPlayerView(1, mmwPlayer: player1)
        
        foregroundNode = SKSpriteNode()
        addChild(foregroundNode!)
        foregroundNode?.zPosition=1
        timerHUD(90)
        partialWordHUD("ABCDEFG", isWord: false)  // "ABCDEFGHIJKLMNO", isWord: false)
        tilesRemainingHUD(55)
        topDisplayHUD("Player 1 plays \"CATATONIC\" for 14 points") // ("Turn: Player 1, Special Letter Bonus In Effect, 2x Point Bonus")
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "P", withColor: UIColorAppleRed, atPoint: CGPointMake(7, 448.5)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "M", withColor: UIColorAppleRed, atPoint: CGPointMake(54.5, 543)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "O", withColor: UIColorAppleRed, atPoint: CGPointMake(102, 496)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "H", withColor: UIColorAppleGreen, atPoint: CGPointMake(157, 68.5)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "J", withColor: UIColorAppleGreen, atPoint: CGPointMake(157, 21)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "K", withColor: UIColorAppleOrange, atPoint: CGPointMake(204.5, 68.5)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "P", withColor: UIColorApplePurple, atPoint: CGPointMake(877, 448.5)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "O", withColor: UIColorApplePurple, atPoint: CGPointMake(877, 496)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "M", withColor: UIColorApplePurple, atPoint: CGPointMake(877, 543)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "A", withColor: UIColorApplePurple, atPoint: CGPointMake(924.5, 448.5)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "B", withColor: UIColorApplePurple, atPoint: CGPointMake(924.5, 496)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "C", withColor: UIColorApplePurple, atPoint: CGPointMake(924.5, 543)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "1", withColor: UIColorApplePurple, atPoint: CGPointMake(7, 305)) )
        addChild(LetterTileSprite(tileStyle: .basic, withChar: "2", withColor: UIColorApplePurple, atPoint: CGPointMake(877, 305)) )
        
        let player1 = MMWPlayer(_playerID: 1, _playerName: "TestE1")
        addPlayerView(1, mmwPlayer: player1)
        
        let player2 = MMWPlayer(_playerID: 2, _playerName: "TestE2")
        addPlayerView(2, mmwPlayer: player2)
        
//        var player3 = MMWPlayer(_playerID: 3, _playerName: "TestE3")
//        addPlayerView(3, mmwPlayer: player3)
//        
//        var player4 = MMWPlayer(_playerID: 4, _playerName: "TestE4")
//        addPlayerView(4, mmwPlayer: player4)
        
        //        var playerLMid = PlayerView(_playerName: "Doyyy123", _playerColor: UIColorAppleRed)
        //        playerLMid.position = CGPointMake(0, ( 768 - ( (1319/2) - 187 ) ) )// PS coordinates is Y: 1390, convert and flip
        //        playerLMid.zPosition = 100
        //        addChild(playerLMid)
        
        //        for indexY in 1...15 {
        //            for indexX in 1...15 {
        //                var tileAdded : LetterTile = LetterTile(tileStyle: .basic, withChar: "C", withColor: SKColor(red: 0.0, green: 0.6132, blue: 0.8593, alpha: 1.0), atPoint: CGPointMake(xPosition, yPosition))
        //                tileAdded.name = "X: \(xPosition), Y: \(yPosition), Letter: \(tileAdded.tileText)"
        //                addChild(tileAdded)
        //                xPosition += 47.5
        //                //zPosition = -100
        //            }
        //            xPosition = 157.0
        //            yPosition += 47.5
        //        }
        //        println("test grid below")
        //        var testGrid = Grid()
        //        testGrid.getGridSquare(25, locY: 15)
        //        //println("Grid count: \(testGrid.gridValueType.count)")
    }
    
    func partialWordHUD (letters : String, isWord : Bool) {
        let partialWordHUD = SKLabelNode(fontNamed: FontHUDName)
        var isPartial : String
        if isWord {
            isPartial = ""
        }
        else {
            isPartial = "not"
        }
        partialWordHUD.text = "\(letters) is \(isPartial) a partial word"
        partialWordHUD.fontSize = FontHUDSize
        partialWordHUD.position = CGPointMake(size.width/2.0, 3)
        partialWordHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(partialWordHUD)
    }
    
    func timerHUD (timeAmount: Int) {
        let timeRemainingHUD = SKLabelNode(fontNamed: FontHUDName)
        timeRemainingHUD.text = "Timer: \(timeAmount)"
        timeRemainingHUD.fontSize = FontHUDSize
        timeRemainingHUD.fontColor = FontHUDRed
        timeRemainingHUD.position = CGPointMake(size.width/2.0 + 255.0, 3)
        timeRemainingHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(timeRemainingHUD)
    }
    
    func tilesRemainingHUD (tilesLeft : Int){
        let tilesRemainingHUD = SKLabelNode(fontNamed: FontHUDName)
        tilesRemainingHUD.text = "Tiles Left: \(tilesLeft)"
        tilesRemainingHUD.fontSize = FontHUDSize
        tilesRemainingHUD.position = CGPointMake(size.width/2.0 - 332, 3.0)
        tilesRemainingHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(tilesRemainingHUD)
    }
    
    func topDisplayHUD (message : String) {
        let topDisplayHUD = SKLabelNode(fontNamed: FontHUDName)
        topDisplayHUD.text = message
        topDisplayHUD.fontSize = FontHUDSize
        topDisplayHUD.position = CGPointMake(size.width/2.0, 744.0) // CGPointMake(size.width/2.0, 753.0) // 1 of 2 top lines
        topDisplayHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayHUD)
    }
    
    func topDisplayHUD2 (message : String) {
        let topDisplayHUD = SKLabelNode(fontNamed: FontHUDName)
        topDisplayHUD.text = message
        topDisplayHUD.fontSize = FontHUDSize
        topDisplayHUD.position = CGPointMake(size.width/2.0, 735.0) // 2 of 2 top lines
        topDisplayHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayHUD)
    }
    
    func testOutput () {
        print("testOutput")
    }

    func addPlayerView (playerNum : Int, mmwPlayer : MMWPlayer) {
        //var playerView = PlayerView(_playerName: "Doyyypprrtt", _playerColor: UIColorAppleRed)
        let playerView = PlayerView(_playerName: mmwPlayer.playerName , _playerColor: mmwPlayer.playerSeat.seatUIColor) // , _playerColor: UIColorAppleRed)
        playerView.zPosition = -99
        
        if playerNum == 1 {
            playerView.position = CGPointMake(0, ( 768 - ( (1319/2) - 187 ) ) ) // PS coordinates is Y: 1390, convert and flip
        }
        if playerNum == 2 {
            playerView.position = CGPointMake(869.5, ( 768 - ( (1319/2) - 187 ) ) ) // PS coordinates is Y: 1390, convert and flip
        }

//        if playerNum == 1 {
//            playerView.position = CGPointMake(0, ( 768 - ( (1319/2) - 330 ) ) ) // PS coordinates is Y: 1390, convert and flip
//        }
//        if playerNum == 2 {
//            playerView.position = CGPointMake(869.5, ( 768 - ( (1319/2) - 330 ) ) ) // PS coordinates is Y: 1390, convert and flip
//        }
//        if playerNum == 3 {
//            playerView.position = CGPointMake(0, ( 768 - ( (1319/2) + 2 ) ) ) // PS coordinates is Y: 1390, convert and flip
//        }
//        if playerNum == 4 {
//            playerView.position = CGPointMake(869.5, ( 768 - ( (1319/2) + 2 ) ) ) // PS coordinates is Y: 1390, convert and flip
//        }
        
        addChild(playerView)
        
//        var playerL = PlayerView(_playerName: "Freddy", _playerColor: UIColorAppleOrange)
//        playerL.position = CGPointMake(0, ( 768 - (1319/2) - 2 ) ) // PS coordinates is Y: 1390, convert and flip
//        addChild(playerL)
//
//        var playerR = PlayerView(_playerName: "Eric Von Hook", _playerColor: UIColorAppleGreen)
//        playerR.position = CGPointMake(869.5,  ( 768 - (1319/2) - 2 ) ) // PS coordinates is Y: 1390, convert and flip
//        addChild(playerR)
//
//        var playerLTop = PlayerView(_playerName: "Sayyyyyppppyyy", _playerColor: UIColorAppleRed)
//        playerLTop.position = CGPointMake(0,  ( 768 - ( (1319/2) - 330 ) ) )// PS coordinates is Y: 1390, convert and flip
//        addChild(playerLTop)
//
//        var playerRTop = PlayerView(_playerName: "Doyyypprrtt", _playerColor: UIColorApplePurple)
//        playerRTop.position = CGPointMake(869.5,  ( 768 - ( (1319/2) - 330 ) ) )// PS coordinates is Y: 1390, convert and flip
//        addChild(playerRTop)
        
//        var playerLMid = PlayerView(_playerName: "LLDoyyypprrtt", _playerColor: UIColorAppleRed)
//        //playerLMid.position = CGPointMake(0, ( 768 - ( (1319/2) - 187 ) ) )// PS coordinates is Y: 1390, convert and flip
//        playerLMid.position = CGPointMake(0,0)
//        playerLMid.zPosition = 100
//        addChild(playerLMid)
//
//        var playerRMid = PlayerView(_playerName: "RRDoyyypprrtt", _playerColor: UIColorApplePurple)
//        playerRMid.position = CGPointMake(869.5, ( 768 - ( (1319/2) - 187 ) ) )// PS coordinates is Y: 1390, convert and flip
//        addChild(playerRMid)
    }

    func getBoardGrid () -> (Grid) {
        return MMWBoardGrid
    }
    
//    func addTile (letterTile : Character, location : CGPoint){
//        var tile = SKSpriteNode(imageNamed: "LetterTest90x90")
//        tile.position = location
//        tile.anchorPoint = CGPointMake(0, 0)
//        foregroundNode!.addChild(tile)
//    }
    
//    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
//        <#code#>
//    }

    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {

        //checkForTouches(touches, withEvent: event)
        print("TOUCH began in MMWGameScene: \(event!.description) ")
        
        for touch: AnyObject in touches {
            var location = (touch as! UITouch).locationInView(self.view)
            
            location.y = 768 - location.y
            //////////////let location = touch.locationInNode(self)
            let nodes = nodesAtPoint(location) as [SKNode]
            
            let snapGrid : Grid = getSnapGrid(location)!
            
            let gridSquareX = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
            let gridSquareY = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
            let gridSquareXCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerX
            let gridSquareYCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerY
            print("snapGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
            print("nodes.count is: \(nodes.count) ")
            //var foundNode = false
            for node in nodes {
                //                if !foundNode {
                //                    if (node.name?.rangeOfString("Letter: C") != nil) {
                //                        println("you selected \(node.description)")
                //                        node.touchesBegan(touches, withEvent: event)//as LetterTile.touchesBeganLetterTile(touch, withEvent: event)
                //                        foundNode = true
                //                    }
                //                }
                if node.isKindOfClass(LetterTileSprite){
                    node.touchesBegan(touches, withEvent: event)
                    node.touchesMoved(touches, withEvent: event)
                }
            }
            //presentMenuScene()
        }
    }
    
    func getSnapGrid (testSpot : CGPoint) -> Grid? {
        var snapGrid : Grid = MMWBoardGrid
        
        if ((testSpot.x > 157) && (testSpot.x < 869.5)) {snapGrid = MMWBoardGrid}
        
        else if numPlayers == 2 {
            if (testSpot.x < 154.5 ) {snapGrid = MMWPlayer1Grid}
            if (testSpot.x > 869.5) {snapGrid = MMWPlayer2Grid}
        }
        else if numPlayers == 4 {
            if (testSpot.x < 154.5 ) && testSpot.y > 384 {snapGrid = MMWPlayer1of4Grid}
            if (testSpot.x > 869.5 ) && testSpot.y > 384 {snapGrid = MMWPlayer2of4Grid}
            if (testSpot.x < 154.5 ) && testSpot.y < 384 {snapGrid = MMWPlayer3of4Grid}
            if (testSpot.x > 869.5 ) && testSpot.y < 384 {snapGrid = MMWPlayer4of4Grid}
        }
        //else {snapGrid = nil}
        print("GetSnapGrid \(testSpot.x), \(testSpot.y), and grid \(snapGrid), and \(snapGrid.gridName)" )
        return snapGrid
    }
    
    override func update(currentTime: NSTimeInterval) {
//        if spriteTest!.position.y >= 180.0 {
//            self.backgroundNode!.position = CGPointMake(self.backgroundNode!.position.x, -((self.spriteTest!.position.y-180.0)/8))
//            foregroundNode!.position = CGPointMake(foregroundNode!.position.x, -((spriteTest!.position.y-180.0)/8))
//        }
    }
    
//    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
//        <#code#>
//    }
    
    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        //presentMenuScene()
//        for touch in (touches as! Set<UITouch>) {
//            let location = touch.locationInNode(self)
//            let touchedNode = nodeAtPoint(location)
//            touchedNode.position = location
        //////checkForTouches(touches touches, withEvent event)
    }
    
    func presentMenuScene() {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        let menuScene = MenuScene(size: size,
            gameResult: true,
            score: 123)
        //let skView = view
        view?.presentScene(menuScene, transition: transition)
    }
}

 