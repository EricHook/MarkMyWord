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
    var spriteTest : SKSpriteNode?
    var impulseCount = 4
    var diagnosticText : SKLabelNode?
    let MMWBoardGrid: Grid = Grid(gridLowerLeftX: 157, gridLowerLeftY: 21, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15)
    let MMWPlayer1Grid: Grid = Grid(gridLowerLeftX: 7, gridLowerLeftY: 307, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3)
    let MMWPlayer2Grid: Grid = Grid(gridLowerLeftX: 877, gridLowerLeftY: 307, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3)
    let Player1NameLoc : CGPoint = CGPointMake(20, 768 - 325)
    let Player1ScoreLoc : CGPoint = CGPointMake(20, 768 - 325)
    let Player2NameLoc : CGPoint = CGPointMake(880, 768 - 325)
    let Player2ScoreLoc : CGPoint = CGPointMake(880, 768 - 325)
    let Player3NameLoc : CGPoint = CGPointMake(20, 768 - 550)
    let Player3ScoreLoc : CGPoint = CGPointMake(20, 768 - 550)
    let Player4NameLoc : CGPoint = CGPointMake(880, 768 - 550)
    let Player4ScoreLoc : CGPoint = CGPointMake(880, 768 - 550)
    
//    var gameView: UIView!
//    var level: Level!
//    private var data = GameData()
    //private var audioController: AudioController
    //stopwatch variables
    private var secondsLeft: Int = 0
    private var timer: NSTimer?
//    let coreMotionManager = CMMotionManager()
//    var xAxisAcceleration : CGFloat = 0.0
//    let CollisionCategoryPlayer : UInt32 = 0x1 << 1
//    let CollisionCategoryPowerUp : UInt32 = 0x1 << 2
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        //MMWBoardGrid = Grid(gridLowerLeftX: 157, gridLowerLeftY: 21, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15)
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        
        println("scene in MMWGS is: \(self.description)")
        //println("Game scene test num: \( GameScene.getTestNum(<#GameScene#>) )")

        backgroundColor = SKColor(red: 0.5, green: 0.0, blue: 0.0, alpha: 1.0)
        //MMWBoardGrid = Grid(gridLowerLeftX: 157, gridLowerLeftY: 21, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15)
        userInteractionEnabled = true
        // add BG
        backgroundNode = SKSpriteNode(imageNamed: "MarkMyWordBGCleaniPad")
        backgroundNode!.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode!.position = CGPoint(x: size.width/2.0, y: 0.0)
        backgroundNode?.userInteractionEnabled = false
        backgroundNode?.zPosition = -100
        backgroundNode?.alpha = 0.5
        addChild(backgroundNode!)
        
//        ///////////////////////////
        //addPlayerView("PlayerFred")
//        //////////////////////////
//        var player1 : MMWPlayer = MMWPlayer(_playerID: 11, _playerName: "FredPlayer1")
//        addPlayerView(1, mmwPlayer: player1)
        
        foregroundNode = SKSpriteNode()
        addChild(foregroundNode!)
        
        timerHUD(90)
        partialWordHUD("ABCDEFG", isWord: false)  // "ABCDEFGHIJKLMNO", isWord: false)
        tilesRemainingHUD(55)
        //topDisplayHUD("Turn: Player 1, Special Letter Bonus In Effect, 2x Point Bonus")
        topDisplayHUD("Player 1 plays \"CATATONIC\" for 14 points")

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
        
//        var playerLMid = PlayerView(_playerName: "Doyyy123", _playerColor: UIColorAppleRed)
//        playerLMid.position = CGPointMake(0, ( 768 - ( (1319/2) - 187 ) ) )// PS coordinates is Y: 1390, convert and flip
//        playerLMid.zPosition = 100
//        addChild(playerLMid)
        //testOutput ()
        
        // bottom left corner of game board
        var xPosition : CGFloat = 157.0
        var yPosition : CGFloat = 21.0
        
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
        var partialWordHUD = SKLabelNode(fontNamed: FontHUDName)
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
        var timeRemainingHUD = SKLabelNode(fontNamed: FontHUDName)
        timeRemainingHUD.text = "Timer: \(timeAmount)"
        timeRemainingHUD.fontSize = FontHUDSize
        timeRemainingHUD.fontColor = FontHUDRed
        timeRemainingHUD.position = CGPointMake(size.width/2.0 + 255.0, 3)
        timeRemainingHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(timeRemainingHUD)
    }
    
    func tilesRemainingHUD (tilesLeft : Int){
        var tilesRemainingHUD = SKLabelNode(fontNamed: FontHUDName)
        tilesRemainingHUD.text = "Tiles Left: \(tilesLeft)"
        tilesRemainingHUD.fontSize = FontHUDSize
        tilesRemainingHUD.position = CGPointMake(size.width/2.0 - 332, 3.0)
        tilesRemainingHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(tilesRemainingHUD)
    }
    
    func topDisplayHUD (message : String) {
        var topDisplayHUD = SKLabelNode(fontNamed: FontHUDName)
        topDisplayHUD.text = message
        topDisplayHUD.fontSize = FontHUDSize
        topDisplayHUD.position = CGPointMake(size.width/2.0, 744.0) // CGPointMake(size.width/2.0, 753.0) // 1 of 2 top lines
        topDisplayHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayHUD)
    }
    
    func topDisplayHUD2 (message : String) {
        var topDisplayHUD = SKLabelNode(fontNamed: FontHUDName)
        topDisplayHUD.text = message
        topDisplayHUD.fontSize = FontHUDSize
        topDisplayHUD.position = CGPointMake(size.width/2.0, 735.0) // 2 of 2 top lines
        topDisplayHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        addChild(topDisplayHUD)
    }
    
    func testOutput () {
        println("testOutput")
    }
    
//    buttonPlaceholder.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//    buttonPlaceholder.position = CGPoint(x: size.width/2.0, y: 482.0)
//    buttonPlaceholder.userInteractionEnabled = false
//    //playerTileGrid.zPosition = -100
//    buttonPlaceholder.colorBlendFactor = CGFloat(0.1)
//    //playerTileGrid.
//    addChild(buttonPlaceholder)
    
//    func addPlayerView (playerNum : Int, mmwPlayer : MMWPlayer) {
    func addPlayerView (playerNum : Int, mmwPlayer : MMWPlayer) {
    
        //var playerView = PlayerView(_playerName: "Doyyypprrtt", _playerColor: UIColorAppleRed)
        var playerView = PlayerView(_playerName: mmwPlayer.playerName , _playerColor: mmwPlayer.playerSeat.seatUIColor) // , _playerColor: UIColorAppleRed)
        
//        if playerNum == 1 {
//            playerView.position = CGPointMake(0, ( 768 - ( (1319/2) - 187 ) ) )// PS coordinates is Y: 1390, convert and flip
//        }
//        if playerNum == 2 {
//            playerView.position = CGPointMake(869.5, ( 768 - ( (1319/2) - 187 ) ) )// PS coordinates is Y: 1390, convert and flip
//        }
        
        playerView.position = CGPointMake(869.5, ( 768 - ( (1319/2) - 187 ) ) )// PS coordinates is Y: 1390, convert and flip
        playerView.zPosition = 55
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
        
        
        
        var playerLMid = PlayerView(_playerName: "Doyyypprrtt", _playerColor: UIColorAppleRed)
        //playerLMid.position = CGPointMake(0, ( 768 - ( (1319/2) - 187 ) ) )// PS coordinates is Y: 1390, convert and flip
        playerLMid.position = CGPointMake(0,0)
        playerLMid.zPosition = 100
        addChild(playerLMid)
//
//        var playerRMid = PlayerView(_playerName: "Doyyypprrtt", _playerColor: UIColorApplePurple)
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

//    func addOrbsToForeground () {
//        var orbNodePosition = CGPoint(x: spriteTest!.position.x, y: spriteTest!.position.y + 100)
//        var orbXShift : CGFloat = -1.0
//        for _ in 1...50 {
//            var orbNode = SKSpriteNode(imageNamed: "avatarpic")
//            if orbNodePosition.x - (orbNode.size.width * 2) <= 0 {
//                orbXShift = 1.0
//            }
//            if orbNodePosition.x + orbNode.size.width >= self.size.width {
//                orbXShift = -1.0
//            }
//            orbNodePosition.x += 40.0 * orbXShift
//            orbNodePosition.y += 120
//            orbNode.position = orbNodePosition
//            orbNode.physicsBody = SKPhysicsBody(circleOfRadius: orbNode.size.width / 2)
//            orbNode.physicsBody!.dynamic = false
//            
//            orbNode.physicsBody!.categoryBitMask = CollisionCategoryPowerUp
//            orbNode.physicsBody!.collisionBitMask = 0
//            orbNode.name = "POWER_UP_ORB"
//            foregroundNode!.addChild(orbNode)
//        }
//        
//        enumerateChildNodesWithName("POWER_UP_ORB", usingBlock: {
//            node, stop in
//            // do something with node or stop
//            println("Found node \(node)");
//            //stop.memory = true // stop iterating over each node found
//        })
//    }

    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {

        //checkForTouches(touches, withEvent: event)
        println("TOUCH began in MMWGameScene: \(event.description) ")
        
        
        for touch: AnyObject in touches {
            var location = (touch as! UITouch).locationInView(self.view)
            
            location.y = 768 - location.y
            //////////////let location = touch.locationInNode(self)
            let nodes = nodesAtPoint(location) as! [SKNode]
            
            var snapGrid : Grid = getSnapGrid(location)!
            
            var gridSquareX = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
            var gridSquareY = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
            var gridSquareXCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerX
            var gridSquareYCorner = snapGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerY
            println("snapGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
            println("nodes.count is: \(nodes.count) ")
            var foundNode = false
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

            
            
//            // player Left area
//            if (location.x < 154.5) {
//                println("Player1 tile area selected")
//                var gridSquareX = MMWPlayer1Grid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
//                var gridSquareY = MMWPlayer1Grid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
//                var gridSquareXCorner = MMWPlayer1Grid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerX
//                var gridSquareYCorner = MMWPlayer1Grid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerY
//                println("MMWPlayer1Grid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
//            }
//            // player Right area
//            if (location.x > 869.5) {
//                println("Player2 tile area selected")
//                var gridSquareX = MMWPlayer2Grid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
//                var gridSquareY = MMWPlayer2Grid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
//                var gridSquareXCorner = MMWPlayer2Grid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerX
//                var gridSquareYCorner = MMWPlayer2Grid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerY
//                println("MMWPlayer2Grid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
//            }
//            // game board area
//            if (location.x > 157) && (location.x < 869.5) {
//                var gridSquareX = MMWBoardGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareX
//                var gridSquareY = MMWBoardGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareY
//                var gridSquareXCorner = MMWBoardGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerX
//                var gridSquareYCorner = MMWBoardGrid.getGridSquare(Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerY
//                println("MMWBoardGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
//                println("nodes.count is: \(nodes.count) ")
//                var foundNode = false
//                for node in nodes {
//                    //                if !foundNode {
//                    //                    if (node.name?.rangeOfString("Letter: C") != nil) {
//                    //                        println("you selected \(node.description)")
//                    //                        node.touchesBegan(touches, withEvent: event)//as LetterTile.touchesBeganLetterTile(touch, withEvent: event)
//                    //                        foundNode = true
//                    //                    }
//                    //                }
//                    if node.isKindOfClass(LetterTile){
//                        node.touchesBegan(touches, withEvent: event)
//                        node.touchesMoved(touches, withEvent: event)
//                    }
//                }
//            }
    }
    
    
    func getSnapGrid (testSpot : CGPoint) -> Grid? {
        
        var snapGrid : Grid = MMWBoardGrid
        if (testSpot.x < 154.5 ) {snapGrid = MMWPlayer1Grid}
        if (testSpot.x > 869.5) {snapGrid = MMWPlayer2Grid}
        if ((testSpot.x > 157) && (testSpot.x < 869.5)) {snapGrid = MMWBoardGrid}
        //else {snapGrid = nil}
        println("GetSnapGrid \(testSpot.x), \(testSpot.y), and grid \(snapGrid)" )
        return snapGrid
        
    }
    
//    var MMWBoardGrid: Grid = Grid(gridLowerLeftX: 157, gridLowerLeftY: 21, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 15, gridNumSquaresY: 15)
//    var MMWPlayer1Grid: Grid = Grid(gridLowerLeftX: 7, gridLowerLeftY: 305, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3)
//    var MMWPlayer2Grid: Grid = Grid(gridLowerLeftX: 877, gridLowerLeftY: 305, gridSquareSizeX: 47.5, gridSquareSizeY: 47.5, gridNumSquaresX: 3, gridNumSquaresY: 3)
    
    
    
//        let touch = touches.anyObject() as! UITouch
//        let location = touch.locationInNode(self)
//        let nodes = nodesAtPoint(location) as! [SKNode]
//        println("nodes.count is: \(nodes.count) ")
//        
//        for node in nodes {
//            if (node.name?.rangeOfString("Letter: C") != nil) {
//                println("you selected X: 157.0, Y: 686.0, Letter: C")
//                node.touchesBegan(touches: NSSet, withEvent: event)//as LetterTile.touchesBeganLetterTile(touch, withEvent: event)
//            }

        
//        // add orangeTile
//        var orangeTile : Tile
//        //orangeTile = Tile.makeTile(Tile())()
//        orangeTile = Tile()
//        orangeTile.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        orangeTile.position = CGPoint(x: size.width/2.0 + 25, y: 10.0)
//        self.addChild(orangeTile)
 //   }
    
    
//    func didBeginContact(contact: SKPhysicsContact) {
////        var nodeB = contact.bodyB!.node!
////        if nodeB.name == "POWER_UP_ORB" {
////            impulseCount++
////            nodeB.removeFromParent()
////        }
////        println("CONTACT! contactPoint:\(contact.contactPoint)")
//    }
    
    
//    func addTile (atPoint: CGPoint, withChar : String, withColor : SKColor){   // add letter, name, colorize?,
//        self.userInteractionEnabled = true
//        var letter : String = withChar
//        var tileNode = SKSpriteNode(imageNamed: "LetterTest90x90")
//        tileNode.name = "tile"
//        tileNode.position = atPoint
//        tileNode.anchorPoint = CGPointMake(0, 0)
//        
//        tileNode.color = withColor
//        tileNode.colorBlendFactor = 1.0
//        
//        var letterLabel = SKLabelNode(fontNamed: FontHUDName)
//        letterLabel.text = letter
//        letterLabel.fontSize = 44 // FontHUDSize
//        //var colorDarkGreen : UIColor = UIColor(red: 0.1, green: 1.0, blue: 0.2, alpha: 0.80)
//        letterLabel.fontColor = FontHUDWhite
//        letterLabel.position = CGPointMake(22.5, 6)
//        letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
//        tileNode.addChild(letterLabel)
//        foregroundNode!.addChild(tileNode)
//    }
    
    
    override func update(currentTime: NSTimeInterval) {
//        if spriteTest!.position.y >= 180.0 {
//            self.backgroundNode!.position = CGPointMake(self.backgroundNode!.position.x, -((self.spriteTest!.position.y-180.0)/8))
//            foregroundNode!.position = CGPointMake(foregroundNode!.position.x, -((spriteTest!.position.y-180.0)/8))
//        }
    }
    
    func presentMenuScene() {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        let menuScene = MenuScene(size: size,
            gameResult: true,
            score: 123)
        let skView = view
        view?.presentScene(menuScene, transition: transition)
    }
    
    
//    override func didSimulatePhysics() {
//        self.spriteTest!.physicsBody!.velocity = CGVectorMake(self.xAxisAcceleration * 380.0, self.spriteTest!.physicsBody!.velocity.dy)
//        if spriteTest!.position.x < -(spriteTest!.size.width / 2) {
//            spriteTest!.position = CGPointMake(size.width - spriteTest!.size.width / 2, spriteTest!.position.y)
//        }
//        else if spriteTest!.position.x > self.size.width {
//            spriteTest!.position = CGPointMake(spriteTest!.size.width / 2, spriteTest!.position.y)
//        }
//    }
    
//    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
//    //override func touchesMoved(    (touches: NSSet, withEvent event: UIEvent) {
//        println("Tetris touch moved")
//        
////        for touch: AnyObject in touches {
////            let location: CGPoint! = touch.locationInView(self.view)   // (<#view: UIView?#>)   //.locationInNode(self)
////            print("location \(location) .. ")
////            let nodeAtPoint = self.nodeAtPoint(location)
////            println("nodeAtPoint.location \(nodeAtPoint) .. ")
////            //self.nodeAtPoint(location) = location
////        }
//        
//        for touch : AnyObject in touches {
//            let location : CGPoint! = touch.locationInView(self.view) /// touch.locationInNode(self)
//            let touchedNode = nodeAtPoint(location)
//            touchedNode.position = location
//        }
//    }
    
    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
        presentMenuScene()
        
//        for touch in (touches as! Set<UITouch>) {
//            let location = touch.locationInNode(self)
//            let touchedNode = nodeAtPoint(location)
//            touchedNode.position = location
        //////checkForTouches(touches touches, withEvent event)
        }
    }
    
//    func checkForTouches(touches: NSSet, withEvent event: UIEvent) {
//        println("check for touches in MMWGameScene")
//        
//        let touch = touches.anyObject() as! UITouch
//        let location = touch.locationInNode(self)
//        
//        let nodes = nodesAtPoint(location) as! [SKNode]
//        println("nodes.count is: \(nodes.count) ")
//
//        for node in nodes {
//            if (node.name?.rangeOfString("Letter: C") != nil) {
//                println("you selected X: 157.0, Y: 686.0, Letter: C")
//                //node.touchesBegan(touches: NSSet, withEvent: event)//as LetterTile.touchesBeganLetterTile(touch, withEvent: event)
//            }
//            
////            println("Selected  MMW Node ... description? \(node.description)")
////            println("nodename is \(node.name)")
////            if node.isKindOfClass(SKSpriteNode.self) {
////                println("node.isKindOfClass(SKSpriteNode.self)")
////                let sprite = node as! SKSpriteNode
////                //println("Selected SK SpriteNode ...  Fred? .. sprite.name? \(sprite.name) description? \(sprite.description)")
////                let spriteParent = sprite.parent
////                //println("Selected spriteParent ...  Fred? .. sprite.name? \(spriteParent?.name) description? \(spriteParent?.description)")
////                if spriteParent!.name == "P" {
////                    spriteParent!.name = "Pselected"
////                    //spriteParent.colorBlendFactor = 0
////                    println("Selected MMWGameScene spriteParent.name P")
////                }
////            }
//        }

    
    
//    deinit {  // ??? for optimization of performance
//        self.coreMotionManager.stopAccelerometerUpdates()
//    }
    
    
    //    override func update(currentTime: CFTimeInterval) {
    //        /* Called before each frame is rendered */
    //    }
//}


//class GameScene: SKScene {
//    override func didMoveToView(view: SKView) {
//        /* Setup your scene here */
//        let myLabel = SKLabelNode(fontNamed:"Chalkduster")
//        myLabel.text = "Hello, World!";
//        myLabel.fontSize = 65;
//        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y:CGRectGetMidY(self.frame));
//        
//        self.addChild(myLabel)
//    }
//    
//    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
//        /* Called when a touch begins */
//        
//        for touch in (touches as! Set<UITouch>) {
//            let location = touch.locationInNode(self)
//            
//            let sprite = SKSpriteNode(imageNamed:"Spaceship")
//            
//            sprite.xScale = 0.5
//            sprite.yScale = 0.5
//            sprite.position = location
//            
//            let action = SKAction.rotateByAngle(CGFloat(M_PI), duration:1)
//            
//            sprite.runAction(SKAction.repeatActionForever(action))
//            
//            self.addChild(sprite)
//        }
//    }
//   
//    override func update(currentTime: CFTimeInterval) {
//        /* Called before each frame is rendered */
//    }
//}
