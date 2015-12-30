//
//  TileView.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/30/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

/// class comment here
/// - Returns: nothing
/// - Parameters:
///     - none: nothing
///     - nope: none
class LetterTileSprite : SKSpriteNode {
    
    var mmwGameSceneViewController  : MMWGameSceneViewController! = nil
    var mmwGameScene                : MMWGameScene! = nil
    var tileSpriteParent            : MMWTile! = nil
    
    let letterLabel = SKLabelNode(fontNamed: FontHUDName)
    var isMovable   = true
    var faceUp      = true
    var hasShadow   = false
    var tileText    = ""

//    var tileScore1 : SKLabelNode = SKLabelNode()
//    var tileScore2 : SKLabelNode = SKLabelNode()
//    var tileScore3 : SKLabelNode = SKLabelNode()
    
    var frontTexture    = SKTexture(imageNamed: "Tile3D")
    var backTexture     = SKTexture(imageNamed: "Tile3DBack")
    //var glowTexture   : SKTexture = SKTexture(imageNamed: "Tile3D90xGLOW")
    var largeTexture    : SKTexture?
    var largeTextureFilename = "Tile3D"
    var tileShadow : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "TileShadow"), color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSize(width: 50.0, height: 50.0) )  // 50.0
    var tileGlow : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "Tile3DGlow"), color: UIColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSize(width: 50.0, height: 50.0) )  // 50.0
    
    //Double(viewSize.width * 0.04638772)
    
    var tileLocation : CGPoint = CGPointMake(0, 0)
    var enlarged      = false
    var savedPosition = CGPointZero
    var hasLockedPotentialWord = false

    enum TileStyle : Int {
        case basic = 0,
        metal,
        pastel,
        wood,
        neon
    }
  
//    let TileColors2 : [SKColor] = [UIColor.blackColor(), colorGameCenterBlue, colorGameCenterGreen, colorGameCenterRed, colorGameCenterYellow, UIColorApplePurple,  UIColorGray, UIColor.blackColor()]
    
//    let TileColors : [UIColor] = [UIColor.blackColor(), UIColorAppleBlue, UIColorAppleRed, UIColorAppleYellow, UIColorApplePurple, UIColorAppleGreen, UIColorAppleOrange, UIColorGray ]

    let actionSound      = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    let actionSound2     = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    let dealTilesSound   = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
    let destroyTileSound = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)  // problem with 1023.wav ???
    
    required init(coder aDecoder: NSCoder) {
        fatalError("NSCoding not supported")
    }
    
    init(tileStyle: TileStyle, withChar : String, withColor : SKColor, atPoint: CGPoint) {  //  // add letter, name, colorize?,
        tileText = withChar
        
        //var dictionaryTrei : Trie
        
//        
//        switch tileStyle {
//            case .basic:
//                frontTexture = SKTexture(imageNamed: "Tile3D95x")
//                largeTextureFilename = "Tile3D95x"
//            case .metal:
//                frontTexture = SKTexture(imageNamed: "Tile3D95x")
//                largeTextureFilename = "Tile3D95x"
//            default:
//                frontTexture = SKTexture(imageNamed: "Tile3D95x")
//                largeTextureFilename = "Tile3D95x"
//        }
        

        
        
//        if (mmwGame.screenSize?.height == 1024) {
//            tileSize.height *= 0.75
//            tileSize.width *= 0.75
//        }
        
//        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
////            tileSize.height *= 0.75
////            tileSize.width *= 0.75
//                frontTexture = SKTexture(imageNamed: "Tile3D@iPadPro.png")
//        }
        
        //let myCGSize = CGSizeMake(47.5, 47.5)
        let tileSize = CGSizeMake(47.5, 47.5)
        //let tileSize = frontTexture.size()/3
//        var tileSize = CGSizeMake(mmwGame.screenSize!.width * 0.04638671875 , mmwGame.screenSize!.height * 0.06184895833)
//        var tileSize = CGSizeMake(frontTexture.size(), <#T##height: CGFloat##CGFloat#>)
        
        
        
        super.init(texture: frontTexture, color: UIColorAppleBlue, size: tileSize)
        // call designated initializer on super
        //super.init(texture: frontTexture, color: UIColorAppleBlue, size: myCGSize ) // frontTexture.size())
        
        // initialize properties
        //self.mmwGameSceneController = mm
        //tileSpriteParent = self.
        self.name = "tileSprite"
        self.position = atPoint
        self.anchorPoint = CGPointMake(0.5, 0.5)
        self.color = withColor
        self.colorBlendFactor = 1.0
        self.zPosition = 0
        
        tileShadow.alpha = 0.35
        tileShadow.position = CGPointMake(-5, -5)
        tileShadow.zPosition = self.zPosition - 1
        tileShadow.hidden = true
        tileShadow.name = "tileShadowName"
        self.addChild(tileShadow)
        
        tileGlow.alpha = 1.0
        tileGlow.color = UIColorAppleRed
        tileGlow.position = CGPointMake(0, 0)
        tileGlow.zPosition = 95 // self.zPosition + 1
        tileGlow.hidden = true
        tileGlow.name = "glowName"
        tileGlow.hidden = true
        self.addChild(tileGlow)

        letterLabel.text = withChar
        letterLabel.fontSize = 40 // FontHUDSize
        letterLabel.fontColor = UIColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 0.98)
        letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        letterLabel.position = CGPointMake(0, -14)
        letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        letterLabel.zPosition = self.zPosition + 1
        self.addChild(letterLabel)
        centerTileToSquare(self)
        
//        // add 'bomb' detail to lower left corner of tiles
//        let letterValueLabel = SKLabelNode(fontNamed: FontHUDName)
//        letterValueLabel.text = "💣"
//        letterValueLabel.fontSize = 10
//        letterValueLabel.fontColor = FontHUDWhite
//        letterValueLabel.position = CGPointMake(-22, -20)
//        letterValueLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
//        self.addChild(letterValueLabel)

        // set properties defined in super
        userInteractionEnabled = false
        
    }

    
    func flip() {
        if faceUp {
            self.texture = self.backTexture
            if let damageLabel = self.childNodeWithName("damageLabel") {
                damageLabel.hidden = true
            }
            self.faceUp = false
        } else {
            self.texture = self.frontTexture
            if let damageLabel = self.childNodeWithName("damageLabel") {
                damageLabel.hidden = false
            }
            self.faceUp = true
        }
    }
    
    
    func centerTileToSquare(tile : LetterTileSprite) {
        tile.position.x += (mmwGame.screenSize?.width)! * 0.023193359375  // 23.75 22.5?
        tile.position.y += (mmwGame.screenSize?.width)! * 0.03094479167  // 23.75 22.5?
    }
    
    
    func lockTile () {
        let textColor = self.color
        letterLabel.fontColor =  textColor
        self.color = UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00)
        self.userInteractionEnabled = false
    }

    
    func enlarge() {
        if enlarged {
            let slide = SKAction.moveTo(savedPosition, duration:0.3)
            let scaleDown = SKAction.scaleTo(1.0, duration:0.3)
            //texture = backTexture
            runAction(SKAction.group([slide, scaleDown])) {
                self.enlarged = false
                self.zPosition = 1
                self.tileShadow.hidden = true
            }
        } else {
            enlarged = true
            savedPosition = position
            self.tileShadow.hidden = false
            if largeTexture != nil {
                texture = largeTexture
            } else {
                largeTexture = SKTexture(imageNamed: largeTextureFilename)
                texture = largeTexture
            }
            zPosition = 50
            
            let newPosition = CGPointMake(CGRectGetMidX(parent!.frame), CGRectGetMidY(parent!.frame))
            removeAllActions()
            
            let slide = SKAction.moveTo(newPosition, duration:0.3)
            let scaleUp = SKAction.scaleTo(5.0, duration:0.3)
            runAction(SKAction.group([slide, scaleUp]))
            
            self.lockTile()
            
        }
        print("  home \(self.tileSpriteParent.gridHome?.gridName) [\(self.tileSpriteParent.gridX)] [\(self.tileSpriteParent.gridY)]")
        print("  end \(self.tileSpriteParent.gridEnd?.gridName) [\(self.tileSpriteParent.gridXEnd)] [\(self.tileSpriteParent.gridYEnd)]")
        print("  state: \(self.tileSpriteParent.tileState) type:\(self.tileSpriteParent.tileType) owner:\(self.tileSpriteParent.tileOwner)")
    }
    
    
    func createLetterScoreText (startLocation: CGPoint, endLocation: CGPoint, textColor: SKColor, displayText: String) -> SKLabelNode {
        let scoreText = SKLabelNode(text: displayText)
        self.addChild(scoreText)
        scoreText.color = textColor
        scoreText.userInteractionEnabled = false
        scoreText.position = CGPointMake(0.0, 0.0)
        return scoreText
    }
    
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        super.touchesBegan(touches, withEvent: event)
        /* Called when a touch begins */
//        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//        runAction(actionSound)
        
        for touch in (touches as Set<UITouch>) {
            
//            //            if touch.tapCount > 1 {
//            //                flip()
//            //            }
//            if touch.tapCount > 1 {
//                enlarge()
//            }
//            if enlarged { return }
            
            zPosition = 90
            //tileShadow.zPosition = self.zPosition - 1
            
            
            
            
//            let liftUp = SKAction.scaleTo(1.2, duration: 0.1)
//            runAction(liftUp, withKey: "pickup")
//            tileShadow.zPosition = self.zPosition - 1
//            tileShadow.hidden = false

            
            
            
            //let location = touch.locationInNode(scene!)
            // mark home grid and x and y grid locations at start of drag
//            let gameGrid = (scene as! MMWGameScene).getSnapGrid(location)
//            tileSpriteParent.gridHome = gameGrid
//            let gridHomeLocationX = gameGrid!.getGridSquare( Float(location.x), locY: Float(location.y) ).GridSquareX
//            let gridHomeLocationY = gameGrid!.getGridSquare( Float(location.x), locY: Float(location.y) ).GridSquareY
//            //print( "<LetterTileSprite> tileFrom: gameGrid: \(tileSpriteParent.gridHome!.gridName) [\(gridHomeLocationX)][\(gridHomeLocationY)]" )
            
            
//        /* Called when a touch begins */
//        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//        runAction(actionSound)
//        
//        for touch in (touches as! Set<UITouch>) {
//            //            if touch.tapCount > 1 {
//            //                flip()
//            //            }
//            if touch.tapCount > 1 {
//                enlarge()
//            }
//            
//            if enlarged { return }
//            //            let location = touch.locationInNode(self)
//            //            let touchedNode = nodeAtPoint(location)
//            zPosition = 99
//            tileShadow.zPosition = -10
//            
//            let liftUp = SKAction.scaleTo(1.2, duration: 0.1)
//
//            runAction(liftUp, withKey: "pickup")
//            
//            tileShadow.hidden = false
//            
//            let location = touch.locationInNode(scene)
//            var testNodes = nodesAtPoint(location)
//            println("testNodeCount : \(testNodes.count ) ")
//            for node in testNodes  {
//                println("testNode ")
//            }
//            //checkForTouches(touches)
        }
    }
    
//    overide func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?) {
//        <#code#>
//    }


    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        super.touchesMoved(touches, withEvent: event)
        
        zPosition = 90
        
        if enlarged { return }
        
        hasLockedPotentialWord = false
        
        for touch in (touches as Set<UITouch>) {
            if isMovable {
                let location = touch.locationInNode(scene!)
                let touchedNode = nodeAtPoint(location)
                touchedNode.position = location
            }
            
            let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)
            
            // IF VALID DROP LOCATION
            //if (tileSnapTouch.x > 160 && tileSnapTouch.x < 860) && (tileSnapTouch.y > 50 && tileSnapTouch.y < 747) { // checks that drag location within game grid boundaries    ??           314,76/157,38   xx  1732,1492/866, 746
            
            if (tileSnapTouch.x > (mmwGame.screenSize?.width)! * 0.153320 && tileSnapTouch.x < (mmwGame.screenSize?.width)! * 0.84553) && (tileSnapTouch.y > (mmwGame.screenSize?.height)! * 0.04947917 && tileSnapTouch.y < (mmwGame.screenSize?.height)! * 0.97135416) { // checks that drag location within game grid boundaries
                
                
                let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
                let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y)) // gets grid x, y on tile drag
                
                let tileSnapResultsXGrid = tileSnapResults.GridSquareX
                let tileSnapResultsYGrid = tileSnapResults.GridSquareY
                print("touch: \(tileSnapTouch.x), \(tileSnapTouch.y) /  \(tileSnapResultsXGrid), \(tileSnapResultsYGrid)  \(mmwGameScene.getSnapGrid(tileSnapTouch)?.gridName)")
                checkForValidWordsOnTileDrag(tileSnapResultsXGrid, gridYSpot: tileSnapResultsYGrid, IsAI: false)
            }
            else {
                removeBoardTileHighlights ()
            }
        }
    }
    
    
    /// touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?)
    /// - Returns: nothing
    /// - Parameters:
    ///     - touches: Set<UITouch>: touches
    ///     - withEvent event: UIEvent?): event
    override func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?) {
        if enlarged { return }
        //var location = touch.locationInNode(self)
        
        print( (mmwGame.screenSize?.width)! )
        
        for touch in (touches as Set<UITouch>) {
            zPosition = 10
//            let dropDown = SKAction.scaleTo(1.0, duration: 0.1)
//            runAction(dropDown, withKey: "drop")
            tileShadow.hidden = true
            
            let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)

            // IF NOT VALID DROP LOCATION ON BOARD, RETURN TILE TO PLAYER
            
            //if (tileSnapTouch.x <= 160 || tileSnapTouch.x >= 860) {
//            if (tileSnapTouch.x <= (mmwGame.screenSize?.width)! * 0.15625 || tileSnapTouch.x >= (mmwGame.screenSize?.width)! * 0.83984375) {
//                print("Tried drop tile outside game board touchesEnded")
//                returnTileToGridHome()
//                return
//            }
            
            if (tileSnapTouch.x <= (mmwGame.screenSize?.width)! * 0.153320 || tileSnapTouch.x >= (mmwGame.screenSize?.width)! * 0.84553) || (tileSnapTouch.y <= (mmwGame.screenSize?.height)! * 0.04947917 || tileSnapTouch.y >= (mmwGame.screenSize?.height)! * 0.97135416) {
                    print("Tried drop tile outside game board touchesEnded")
                    returnTileToGridHome()
                    return
                
            }
            

            let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
            let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y))
            let tileSnapResultsXGrid = tileSnapResults.GridSquareX
            let tileSnapResultsYGrid = tileSnapResults.GridSquareY
            //let tileSnapTouchX = tileSnapTouch.x
            //let tileSnapTouchY = tileSnapTouch.y
            //let tileSnapResultsCalculateX = tileSnapResults.GridSquareUpperLeftCornerX
            //let tileSnapResultsCalculateY = tileSnapResults.GridSquareUpperLeftCornerY - 15.5 // -15.5 on y touch point adjusts snapping too high to lower square

            //let tileAtDropSpot : MMWTile = (gameGrid?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid])! // ERROR IF DRAG TOO FAR RIGHT _ INDEX OUT OF RANGE !!! ///////////
            
            if !hasLockedPotentialWord { // checked for locked tile on drag, return
                returnTileToGridHome()
                continue
            }
            
            //////////// TEST FOR TILE UNDER DROP SPOT
            if testForTileAtDropSpot(gameGrid!, tileSnapResultsXGrid: tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid) {
                break
            }
                
            else {
                let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
                let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y))
                let tileSnapResultsXGrid = tileSnapResults.GridSquareX
                let tileSnapResultsYGrid = tileSnapResults.GridSquareY
                
                let validWordTestAtDropSpot = testForValidWordsAtDropSpot(tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid, isAI: false)
                
                
                ///////////////////////
                
                
                if validWordTestAtDropSpot.validHorizontalPartialWord && validWordTestAtDropSpot.validVerticalPartialWord {
                    self.tileSpriteParent.tileState = TileState.Played
//                    self.tileSpriteParent.tileGrid = gameGrid
//                    self.tileSpriteParent.gridX = tileSnapResultsXGrid
//                    self.tileSpriteParent.gridY = tileSnapResultsYGrid
//                    self.tileSpriteParent.gridXEnd = tileSnapResultsXGrid
//                    self.tileSpriteParent.gridYEnd = tileSnapResultsYGrid
                    
//                    gameGrid!.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid] = self.tileSpriteParent
                    
               
                ////////////////////
                    
                    
                updateWordsAtDropSpot(Float(tileSnapTouch.x), touchY: Float(tileSnapTouch.y)) () // tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid,  touchX:
                }
            }
            print("tile drop = \(tileSnapResultsXGrid), \(tileSnapResultsYGrid)")
        }

        mmwGameSceneViewController.resetConsequtivePasses()
        mmwGameSceneViewController.lettersPlayedInTurn += 1
        print("letters playedInTurn = \(mmwGameSceneViewController.lettersPlayedInTurn) ViewSize: \(mmwGameScene.viewSize.width)")
        
        // Change turns if player has no letter tiles remaining
        let letterTilesRemaining = self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.playerArray[(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.playerTurn)! - 1].playerLetterGrid.numLetterTilesInGrid()
        if letterTilesRemaining <= 0 {
            tileSpriteParent.tileBuilder!.mmwGameSceneViewController!.mmwGameScene.changePlayerTurn()
        }
    }
    

    /// returnTileToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - none: nothing
    func returnTileToGridHome () {
        let scaleUp : SKAction!
        let scaleDown : SKAction!
        self.zPosition = 25
        let returnPosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridY)
        let slide = SKAction.moveTo(returnPosition, duration:0.3)
        //let scaleUp = SKAction.scaleTo(1.5, duration:0.15)
        //let scaleDown = SKAction.scaleTo(1.0, duration:0.15)
        
        if mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
            scaleUp = SKAction.scaleTo(1.0, duration:0.15)
            scaleDown = SKAction.scaleTo(0.42, duration:0.15)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus {
            scaleUp = SKAction.scaleTo(1.0, duration:0.15)
            scaleDown = SKAction.scaleTo(0.5, duration:0.15)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            scaleUp = SKAction.scaleTo(2.0, duration:0.15)
            scaleDown = SKAction.scaleTo(1.33, duration:0.15)
        }
        else {
            scaleUp = SKAction.scaleTo(1.5, duration:0.15)
            scaleDown = SKAction.scaleTo(1.0,  duration:0.15)
        }

        runAction(SKAction.group([slide, scaleUp, scaleDown]))
        self.tileGlow.hidden = true
        removeBoardTileHighlights ()
        runAction(actionSound2)
    }
    
    
    /// playTileToBoardGrid (pauseDuration: Double)
    /// - Returns: nothing
    /// - Parameters: pauseDuration: Double
    ///     -
    func playTileToBoardGrid (pauseDuration: Double) {
        let scaleUp : SKAction!
        let scaleDown : SKAction!
        self.zPosition = 100
        let boardPosition = Grid.sendToGridSquare(self.mmwGameScene.mmwBoardGrid, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
        
        let slide = SKAction.moveTo(boardPosition, duration:0.8)
        
        //let scaleUp = SKAction.scaleTo(1.33, duration:0.4)
        //let scaleDown = SKAction.scaleTo(1.33, duration:0.4)
        
        if mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
            scaleUp = SKAction.scaleTo(1.0, duration:0.4)
            scaleDown = SKAction.scaleTo(0.42, duration:0.4)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus {
            scaleUp = SKAction.scaleTo(1.0, duration:0.4)
            scaleDown = SKAction.scaleTo(0.51, duration:0.4)
        }
        else if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            scaleUp = SKAction.scaleTo(2.0, duration:0.4)
            scaleDown = SKAction.scaleTo(1.33, duration:0.4)
        }
        else { // iPad device
            scaleUp = SKAction.scaleTo(1.5, duration:0.4)
            scaleDown = SKAction.scaleTo(1.00, duration:0.4)
        }

        let pauseSlide = SKAction.sequence([slide])
        let scaleUpDown = SKAction.sequence([scaleUp, scaleDown, self.actionSound2, ])
        self.runAction(SKAction.group([pauseSlide, scaleUpDown]))
        
        // save original tile info for animations
        self.tileSpriteParent.gridTest = self.tileSpriteParent.gridHome
        self.tileSpriteParent.gridXTest = self.tileSpriteParent.gridX
        self.tileSpriteParent.gridYTest = self.tileSpriteParent.gridY
        
        // update tile info
//        self.tileSpriteParent.gridHome = self.mmwGameScene.mmwBoardGrid
//        self.tileSpriteParent.gridHome?.grid2DArr[self.tileSpriteParent.gridXEnd][self.tileSpriteParent.gridYEnd] = self.tileSpriteParent
//        self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
//        self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
        
        self.userInteractionEnabled = false
        self.tileGlow.hidden = true
        self.removeBoardTileHighlights ()
        delay(1.0) {
            self.zPosition = 10
        }
        
        
//        func playTile() {
//            let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
//            dispatch_async(dispatch_get_global_queue(priority, 2), { ()->() in   // priority def 0
//                ///////////////////
//                print("gcd playTileToBoardGrid hello")
//
////                        self.zPosition = 100
////                        let boardPosition = Grid.sendToGridSquare(self.mmwGameScene.mmwBoardGrid, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
////                
////                        let slide = SKAction.moveTo(boardPosition, duration:1.0)
////                        let scaleUp = SKAction.scaleTo(1.5, duration:0.5)
////                        let scaleDown = SKAction.scaleTo(1.0, duration:0.5)
////                        let pauseSlide = SKAction.sequence([slide])
////                        let scaleUpDown = SKAction.sequence([scaleUp, scaleDown, self.actionSound2, ])
////                        self.runAction(SKAction.group([pauseSlide, scaleUpDown]))
////                
////                        // save original tile info for animations
////                        self.tileSpriteParent.gridTest = self.tileSpriteParent.gridHome
////                        self.tileSpriteParent.gridXTest = self.tileSpriteParent.gridX
////                        self.tileSpriteParent.gridYTest = self.tileSpriteParent.gridY
////                        // update tile info
////                        self.tileSpriteParent.gridHome = self.mmwGameScene.mmwBoardGrid
////                        self.tileSpriteParent.gridHome?.grid2DArr[self.tileSpriteParent.gridXEnd][self.tileSpriteParent.gridYEnd] = self.tileSpriteParent
////                        self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
////                        self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
////                
////                        self.userInteractionEnabled = false
////                        self.tileGlow.hidden = true
////                        self.removeBoardTileHighlights ()
////                        self.zPosition = 10
//
//
//                dispatch_async(dispatch_get_main_queue(), {
//                    print("hello from playTileToBoardGrid thread executed as dispatch")
//                })
//            })
//
//            print("hello from playTileToBoardGrid thread")
//        }
//        
//        
//        playTile()
        
//        CATransaction.commit()
        
    }

    
    /// showTileScoreTextToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - none: nothing
    func showTileScoreTextToGridHome (delaySec: Int, pointsForTile: Int ) {

        var tileScore = SKLabelNode()
        tileScore = self.createLetterScoreText ( CGPointMake(0.0, 0.0) , endLocation: CGPointMake(0.0, 0.0), textColor: gameColors[self.tileSpriteParent.tileOwner.rawValue], displayText: String(pointsForTile) )
        tileScore.zPosition = 100
        tileScore.text = String(pointsForTile)
        tileScore.fontColor =  gameColors[self.tileSpriteParent.tileOwner.rawValue]
        tileScore.fontSize = 45
        tileScore.fontName = FontHUDName
        
        let homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridY)
        //let homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridTest!, squareX: self.tileSpriteParent.gridXTest, squareY: self.tileSpriteParent.gridYTest)
        let endPosition  = Grid.sendToGridSquare(self.tileSpriteParent.gridEnd!, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)

        //self.mmwGameScene.mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1])

        let changeInX : CGFloat = -(endPosition.x - homePosition.x)
        let changeInY : CGFloat = -(endPosition.y - homePosition.y)
        let returnPosition = CGPointMake(changeInX, changeInY)
        let slide     = SKAction.moveTo(returnPosition, duration:1.0)
        let scaleUp   = SKAction.scaleTo(1.5, duration:0.5)
        let scaleDown = SKAction.scaleTo(0.66, duration:0.5)
        let fadeIn    = SKAction.fadeInWithDuration(1.0)
        let fadeOut   = SKAction.fadeOutWithDuration(1.0)
        let animPart1 = SKAction.group([fadeIn, scaleUp])
        let animPart2 = SKAction.group([fadeOut, scaleDown])
        let removeText = SKAction.removeFromParent()
        let tileEffects = SKAction.sequence([animPart1, animPart2,removeText])
        tileScore.runAction(SKAction.group([slide, tileEffects]) )
        //self.runAction(self.actionSound2)
        
        // update tile info
//                self.tileSpriteParent.gridHome = self.mmwGameScene.mmwBoardGrid
//                self.tileSpriteParent.gridHome?.grid2DArr[self.tileSpriteParent.gridXEnd][self.tileSpriteParent.gridYEnd] = self.tileSpriteParent
//                self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
//                self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
   
    }
    
    
    /// showWordScoreTextToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - none: nothing
    func showWordScoreTextToGridHome (number: Int, isHorizontalScore: Bool) {
        
        var tileScore = SKLabelNode()
        
        tileScore = createLetterScoreText (CGPointMake(0.0, 0.0), endLocation: CGPointMake(0.0, 0.0), textColor: gameColors[self.tileSpriteParent.tileOwner.rawValue], displayText: String("") )
        tileScore.fontColor = gameColors[self.tileSpriteParent.tileOwner.rawValue]
        tileScore.fontSize = 65
        tileScore.fontName = FontHUDName
        tileScore.text = (String(number) )
        tileScore.zPosition = 75
        
        let endPosition = Grid.sendToGridSquare(self.tileSpriteParent.gridEnd!, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
        var homePosition = endPosition
        if isHorizontalScore == true {
            homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridX)
        }
        else {
            homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: 0, squareY: 0)
        }
        
        
        let changeInX : CGFloat = -(endPosition.x - homePosition.x)
        let changeInY : CGFloat = -(endPosition.y - homePosition.y)
        let returnPosition = CGPointMake(changeInX, changeInY)

        let delay = SKAction.waitForDuration( Double(0.0) )
        self.runAction( delay ) {  //  !! Delay for show word score text set here
 
            let unhide = SKAction.unhide()
            let slide = SKAction.moveTo(returnPosition, duration:1.5)
            let scaleUp = SKAction.scaleTo(2.0, duration:0.75)
            let scaleDown = SKAction.scaleTo(0.5, duration:0.75)
            let fadeIn = SKAction.fadeInWithDuration(0.75)
            let fadeOut = SKAction.fadeOutWithDuration(0.75)
            let remove = SKAction.removeFromParent()
            
            let animPart1 = SKAction.group([unhide, slide])
            let animScale = SKAction.sequence([scaleUp, scaleDown, remove])

            tileScore.runAction(SKAction.group([fadeIn, animPart1, fadeOut, animScale]) )

            self.mmwGameScene.explosion(endPosition)
            //self.runAction(self.actionSound)
        }
    }
    
    
    /// showWordScoreTextToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - number: the value to send to home grid
    func showNegativeScoreTextToGridHome (number: Int) {
        
        var tileScore = SKLabelNode()
        
        tileScore = createLetterScoreText ( CGPointMake(0.0, 0.0) , endLocation: CGPointMake(0.0, 0.0), textColor: gameColors[self.tileSpriteParent.tileOwner.rawValue], displayText: String("-5") )
        tileScore.text = String("-5")
        tileScore.fontColor = gameColors[self.tileSpriteParent.tileOwner.rawValue]
        tileScore.fontSize = 45
        tileScore.fontName = FontHUDName
        tileScore.zPosition = 75
        let endPosition = self.position
        //let endPosition = Grid.sendToGridSquare(self.tileSpriteParent.gridEnd!, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
        let homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridTest!, squareX: self.tileSpriteParent.gridXTest, squareY: self.tileSpriteParent.gridYTest)
        let changeInX : CGFloat = -(endPosition.x - homePosition.x)
        let changeInY : CGFloat = -(endPosition.y - homePosition.y)
        let returnPosition = CGPointMake(changeInX, changeInY)
        let unhide = SKAction.unhide()
        let slide = SKAction.moveTo(returnPosition, duration:1.5)
        let scaleUp = SKAction.scaleTo(2.0, duration:0.75)
        let scaleDown = SKAction.scaleTo(0.5, duration:0.75)
        let hide = SKAction.hide() //   .resizeToHeight(400, duration: 1.0)  //.removeFromParent()
        let fadeIn = SKAction.fadeInWithDuration(0.75)
        let fadeOut = SKAction.fadeOutWithDuration(0.75)
        let animPart1 = SKAction.group([unhide, slide])
        let animScale = SKAction.sequence([scaleUp, scaleDown, hide])
        tileScore.runAction(SKAction.group([fadeIn, animPart1, fadeOut, animScale]) )
        
        runAction(actionSound)
        
        mmwGameScene.explosion(endPosition)
    }
    
    
    /// removeBoardTileHighlights() resets rollover highlights for all tiles on board -- otherwise last word remain highlighted
    /// - Returns: nothing
    /// - Parameters: none
    func removeBoardTileHighlights () {
        for tileArr in (tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.mmwGameScene.mmwBoardGrid.grid2DArr)! {
            for tile in tileArr {
                tile.tileSprite.tileGlow.hidden = true
            }
        }
    }
    
    
    /// func checkForValidWords (gridXSpot: Int, gridYSpot: Int, IsAI: Bool)
    /// used on touchesMoved to determine highlighted tile hints and possible partial word
    /// -Parameters: gridXSpot: Int, gridYSpot: Int, IsAI: Bool
    /// -Returns: none
    func checkForValidWordsOnTileDrag (gridXSpot: Int, gridYSpot: Int, IsAI: Bool) {
        ////////////  TEST FOR ADJACENT TILE PARTIAL WORDS
        var leftString : String = ""
        var rightString : String = ""
        var upString : String = ""
        var downString : String = ""
        var horizontalString = ""
        var verticalString : String = ""
        var tilesToAdd : [MMWTile] = [MMWTile]()
        var horizontalLeftTiles : [MMWTile] = [MMWTile]()
        //var horizontalRightTiles : [MMWTile] = [MMWTile]()
        var verticalUpTiles : [MMWTile] = [MMWTile]()
        //var verticalDownTiles : [MMWTile] = [MMWTile]()
        var horizontalTiles : [MMWTile] = [MMWTile]()
        var verticalTiles : [MMWTile] = [MMWTile]()
        var foundLockedTile = false
        var numHorizontalTiles = 0
        var numVerticalTiles = 0
        
        var hasValidLockedTile = false
        
        removeBoardTileHighlights()
        print("checkForValidWords \(gridXSpot),\(gridYSpot), \(IsAI)")
        
        if mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot].tileType != TileType.Letter {
            print("checkForValidWords")
            if (gridXSpot >= 0 && gridXSpot < 15 && gridYSpot >= 0 && gridYSpot < 15) {
                removeBoardTileHighlights ()
                // LEFT
                var currentCheckXGridNum = gridXSpot
                var currentCheckYGridNum = gridYSpot
                var stringToAdd : String = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckXGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Played) ) )  {
                    
                    numHorizontalTiles++
                    tilesToAdd.append((mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot]))
                    
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileText)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd) // (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].letterString)

                    if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked {
                        foundLockedTile = true // makes sure locked tile in potential word
                        hasLockedPotentialWord = true
                        hasValidLockedTile = true
                    }
                    currentCheckXGridNum--
                }

                for char in stringToAdd.characters {
                    leftString = (String(char) + leftString)
                }
                leftString = leftString.stringByAppendingString(self.tileText) // add current letter string to leftward letters
                
                for tile in tilesToAdd {
                    horizontalLeftTiles.append(tile)
                }
                horizontalLeftTiles = horizontalLeftTiles.reverse()
                horizontalLeftTiles.append(self.tileSpriteParent) // add current tile to leftward tiles

                //RIGHT
                currentCheckXGridNum = gridXSpot
                currentCheckYGridNum = gridYSpot
                stringToAdd = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckXGridNum < 14) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Played) ) )  {
                    
                    numHorizontalTiles++
                    tilesToAdd.append((mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot]))
                    
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileText)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    
                    if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Locked {
                        hasLockedPotentialWord = true
                        hasValidLockedTile = true
                    }
                    currentCheckXGridNum++
                }

                for tile in tilesToAdd {
                    horizontalTiles.append(tile)
                }

                horizontalString = leftString.stringByAppendingString(stringToAdd)
                
                print(horizontalString + " checkForValidWords")
                
                ////////////////////////////
                
                // UP
                currentCheckXGridNum = gridXSpot
                currentCheckYGridNum = gridYSpot
                stringToAdd = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckYGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Played) ) )  {
                    numVerticalTiles++
                    tilesToAdd.append((mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1]))
                    
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileText)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd) // (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].letterString)
                    
                    if mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Locked {
                        foundLockedTile = true // stops check on last last locked tile
                        hasLockedPotentialWord = true
                        hasValidLockedTile = true
                    }
                    currentCheckYGridNum--
                }
                
                //                for char in stringToAdd.characters {
                //                    leftString = (String(char) + leftString)
                //                }
                //                leftString = leftString.stringByAppendingString(self.tileText) // add current letter string to leftward letters
                //
                //                for tile in tilesToAdd {
                //                    horizontalLeftTiles.append(tile)
                //                }
                //                horizontalLeftTiles = horizontalLeftTiles.reverse()
                //                horizontalLeftTiles.append(self.tileSpriteParent) // add current tile to leftward tiles

                for char in stringToAdd.characters {
                    upString = (String(char) + upString)
                }
                upString = upString.stringByAppendingString(self.tileText) // add current letter string to upward letters

                
                //if verticalUpTiles.count > 1 {
                    for tile in tilesToAdd {
                        verticalUpTiles.append(tile)
                    }
                    verticalUpTiles = verticalUpTiles.reverse()
                //}
                verticalUpTiles.append(self.tileSpriteParent)
                

                //DOWN
                currentCheckXGridNum = gridXSpot
                currentCheckYGridNum = gridYSpot
                stringToAdd = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckYGridNum < 14) && (mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum][currentCheckYGridNum + 1].tileState == TileState.Played) ) )  {
                    numVerticalTiles++
                    tilesToAdd.append((mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1]))
                    
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileText)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    
                    //if currentCheckYGridNum != 0 {
                        if mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked {
                            foundLockedTile = true // stops check on last last locked tile
                            hasLockedPotentialWord = true
                            hasValidLockedTile = true
                        }
                        currentCheckYGridNum++
                    //}
                }
                
                //verticalTiles.append(self.tileSpriteParent)
                
                for tile in tilesToAdd {
                    verticalTiles.append(tile)
                }
                
                downString = self.tileText.stringByAppendingString(stringToAdd)
                verticalString = upString.stringByAppendingString(stringToAdd)
                
                print(verticalString + " checkForValidWords")
                
                if hasValidLockedTile {
                    for tile in horizontalLeftTiles {
                        tile.tileSprite.tileGlow.hidden = false
                    }
                    for tile in horizontalTiles {
                        tile.tileSprite.tileGlow.hidden = false
                    }
                    for tile in verticalUpTiles {
                        tile.tileSprite.tileGlow.hidden = false
                    }
                    for tile in verticalTiles {
                        tile.tileSprite.tileGlow.hidden = false
                    }
                }
            }
        }
    }
    
    
    func testForTileAtDropSpot(gameGrid: Grid, tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int) -> Bool {
        if gameGrid.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid].tileType == TileType.Letter {
            print("Tried drop tile on existing letter touchesEnded gameGrid: \(gameGrid) \(tileSnapResultsXGrid) Y: \(tileSnapResultsYGrid) \(gameGrid.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid])")
            returnTileToGridHome()
            return true
        }
        return false
    }
    
    
    /// func testForValidWordsAtDropSpot (gridXSpot: Int, gridYSpot: Int, IsAI: Bool)
    /// used on touchesMoved to determine highlighted tile hints and possible partial word
    /// -Parameters: tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, isAI: Bool
    /// -Returns: validHorizontalPartialWord: Bool, validVerticalPartialWord: Bool, validHorizontalWholeWord: Bool, validVerticalWholeWord: Bool
    /// AI settings used as Ai iterates through tiles and would be too many false results
    func testForValidWordsAtDropSpot (tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, isAI: Bool) -> (validHorizontalPartialWord: Bool, validVerticalPartialWord: Bool, validHorizontalWholeWord: Bool, validVerticalWholeWord: Bool) {
        var leftString : String = ""
        var rightString : String = ""
        var upString : String = ""
        var downString : String = ""
        var horizontalString : String = ""
        var verticalString : String = ""
        
        var possibleWordTilesHorizontal : [MMWTile] = []
        var possibleWordTilesVertical : [MMWTile] = []
        
        var validHorizontalPartialWord : Bool = false
        var validVerticalPartialWord : Bool = false
        
        var validHorizontalWholeWord : Bool = false
        var validVerticalWholeWord : Bool = false
        
        var numHorizontalAdjacentLetters = 0
        var numVerticalAdjacentLetters = 0
        var foundLockedTile = false

        /////////////// Check for words in both directions
        
        if (tileSnapResultsXGrid >= 0 && tileSnapResultsXGrid < 15 && tileSnapResultsYGrid >= 0 && tileSnapResultsYGrid < 15) {  // make sure drop spot in game board grid to prevent error

                // LEFT
                var currentCheckXGridNum = tileSnapResultsXGrid
                var currentCheckYGridNum = tileSnapResultsYGrid
                var stringToAdd : String = ""
                while ( (currentCheckXGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.PlayedMadeWord) ) ) {
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileText)"
                    stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
                    
                    if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked {
                        foundLockedTile = true // stops check on last last locked tile
                        hasLockedPotentialWord = true
                    }
                    currentCheckXGridNum--
                }
            
                leftString = stringToAdd.stringByAppendingString(self.tileText)
                //possibleWordTilesHorizontal = possibleWordTilesHorizontal.reverse()   ////////////
                possibleWordTilesHorizontal.append(tileSpriteParent)

                //RIGHT
                currentCheckXGridNum = tileSnapResultsXGrid
                stringToAdd = ""
                while ( (currentCheckXGridNum < 14) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.PlayedMadeWord) ) )  {
                    numHorizontalAdjacentLetters++
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileText)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked {
                        foundLockedTile = true
                        hasLockedPotentialWord = true
                    }
                    
                    
                    currentCheckXGridNum++
                }
            
                rightString = stringToAdd
                horizontalString = leftString.stringByAppendingString(rightString)
            
            
            
            
            
            if (( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(horizontalString)) == false) {
                validHorizontalPartialWord = false
                
                
                validHorizontalWholeWord = false
                
                
                
                self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Horizontal \(horizontalString)-> Not a valid partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
                print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> INVALID horizontal partial word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift" )
            }
            else {
                validHorizontalPartialWord = true
                if (( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(horizontalString)) == true) {
                    validHorizontalWholeWord = true
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Horizontal \(horizontalString)-> VALID whole word !!! testForValidWordsAtDropSpot LetterTileSprite")
                    print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> VALID horizontal whole word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift" )
                }
                else {
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Horizontal \(horizontalString)-> VALID partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
                    print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> VALID horizontal partial word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift" )
                }
            }

            
            
            
            
            
            
                
//                if (( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(horizontalString)) == false) {
//                    validHorizontalPartialWord = false
//                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Horizontal \(horizontalString)-> Not a valid partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
//                    print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> INVALID horizontal partial word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift" )
//                }
//                else {
//                    validHorizontalPartialWord = true
//                    if (( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(horizontalString)) == true) {
//                        //&&  horizontalString.characters.count >= mmwGameSceneViewController.minWordSize {
//                        validHorizontalWholeWord = true
//                        self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Horizontal \(horizontalString)-> VALID whole word !!! testForValidWordsAtDropSpot LetterTileSprite")
//                        print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> VALID horizontal whole word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift" )
//                    }
////                    else if (( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(horizontalString)) == true) &&  horizontalString.characters.count < mmwGameSceneViewController.minWordSize {
////                        validHorizontalWholeWord = false
////                        self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Horizontal \(horizontalString)-> VALID whole word BUT too short!!! testForValidWordsAtDropSpot LetterTileSprite")
////                        print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> VALID horizontal whole word BUT too short!!! testForValidWordsAtDropSpot  LetterTileSprite.swift" )
////                    }
//                    else {
//                        self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Horizontal \(horizontalString)-> VALID partial word BUT too short!!! testForValidWordsAtDropSpot LetterTileSprite")
//                        print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> VALID horizontal partial word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift" )
//                    }
//                }
            
            
            
            
            
            
            
                stringToAdd = ""
                foundLockedTile = false

            /////////////
            
            currentCheckXGridNum = tileSnapResultsXGrid
            currentCheckYGridNum = tileSnapResultsYGrid
            
            ////////////
            
                //UP
                while ( (currentCheckYGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Played)  || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.PlayedMadeWord) ) )  {
                    numVerticalAdjacentLetters++
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileText)"
                    stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
                    
                    possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1]))
                    if mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked {
                        foundLockedTile = true
                        hasLockedPotentialWord = true
                    }
                    currentCheckYGridNum--
                }
                
                upString = stringToAdd.stringByAppendingString(self.tileText)
        //possibleWordTilesVertical = upString.reverse()   ////////////
                possibleWordTilesVertical.append(tileSpriteParent)
                
                //DOWN
                currentCheckYGridNum = tileSnapResultsYGrid
                stringToAdd = ""
                while ( (currentCheckYGridNum < 14) && ( (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.PlayedMadeWord) ) ) {
                    numVerticalAdjacentLetters++
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileText)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    if mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked {
                        foundLockedTile = true
                        hasLockedPotentialWord = true
                    }
                    
                    
                    
                    
                    possibleWordTilesVertical.append(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1])
                    
                    
                    
                    
                    
                    
                    currentCheckYGridNum++
                }
            
//            downString = self.tileText.stringByAppendingString(stringToAdd)
//            verticalString = upString.stringByAppendingString(stringToAdd)
//            
//            if mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) == false {
//                // updates GUI for feedback on horizonal partial word
//                validVerticalPartialWord = false
//                validVerticalWholeWord = false
//                self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(verticalString)-> Not a valid partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
//                print ( "Vertical \(verticalString)-> INVALID horizontal partial word !!! testForValidPartialWordsAtDropSpot  LetterTIleSprite.swift" )
//            }
//                
//            else {
//                validVerticalPartialWord = true
//                self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Vertical \(verticalString)-> VALID partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
//                print ( "\(verticalString) -> VALID vartical partial word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift"  )
//            }
            
                downString = stringToAdd
                verticalString = upString.stringByAppendingString(downString)
      
            
            if mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) == false {
                // updates GUI for feedback on horizonal partial word
                validVerticalPartialWord = false
                validVerticalWholeWord = false
                self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("\(verticalString)-> Not a valid partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
                print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> INVALID horizontal partial word !!! testForValidPartialWordsAtDropSpot  LetterTIleSprite.swift" )
            }
                
            else {
                validVerticalPartialWord = true
                if mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) == true {
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> VALID whole word !!! testForValidWordsAtDropSpot LetterTileSprite")
                    print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> VALID vertical whole word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift"  )
                }
                else {
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> VALID partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
                    print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> VALID vartical partial word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift"  )
                }
                
                
            }

            
            
            
            
            
            
            
            
            
            
            
            
//                if mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) == false {
//                    // updates GUI for feedback on horizonal partial word
//                    validVerticalPartialWord = false
//                    validVerticalWholeWord = false
//                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("\(verticalString)-> Not a valid partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
//                    print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> INVALID horizontal partial word !!! testForValidPartialWordsAtDropSpot  LetterTIleSprite.swift" )
//                }
//                    
//                else {
//                    validVerticalPartialWord = true
//                    
//                    if mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) == true {
//                        validVerticalWholeWord = true
//                        self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> VALID whole word !!! testForValidWordsAtDropSpot LetterTileSprite")
//                        print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> VALID vertical whole word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift"  )
//                    }
////                    else if mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) == true &&  horizontalString.characters.count < mmwGameSceneViewController.minWordSize {
////                        self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> VALID whole word BUT too short!!! testForValidWordsAtDropSpot LetterTileSprite")
////                        print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> VALID vertical whole word BUT too short!!! testForValidWordsAtDropSpot  LetterTileSprite.swift"  )
////                    }
//                        
//                    else {
//                        self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> VALID partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
//                        print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> VALID vartical partial word BUT too short!!! testForValidWordsAtDropSpot  LetterTileSprite.swift"  )
//                    }
//                }
        }
        
        if !isAI {  // BAD DROP BY PLAYER ! placeholder skip allow AI place invalid letter in valid spot
            // if horizontal OR vertical invalid partial word > 1 letter (itself) then return tile, invalid play
            if (horizontalString.characters.count > 1 && !validHorizontalPartialWord) ||
                (verticalString.characters.count > 1 && !validVerticalPartialWord ) {
                    runAction(actionSound)
                    
                    // -5 points for non-partial word // SCORE
                    self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerArray[(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerTurn)! - 1].playerScore -= 5
                    showNegativeScoreTextToGridHome(-5)
                    self.tileSpriteParent.tileOwner = TileOwner.None
                    mmwGameSceneViewController.tileCollection.mmwDiscardedTileArray.append(self.tileSpriteParent)
                    self.hidden = true
                    self.tileSpriteParent.gridHome?.grid2DArr[self.tileSpriteParent.gridX][self.tileSpriteParent.gridY] = MMWTile()
                    tileSpriteParent.tileBuilder!.mmwGameSceneViewController!.mmwGameScene.changePlayerTurn()
                    print ( "Horizontal '\(horizontalString)' && Vertical '\(verticalString)' -> NOT valid partial words - 5 points !!! testForValidWordsAtDropSpot LetterTileSprite" )
                    return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord)
            }
            else {
                print("VALID partial words Horizontal AND Vertical! testForValidWordsAtDropSpot  LetterTileSprite.swift")
                return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord)
            }
        }
            
        else {   // IsAI is true
            if (horizontalString.characters.count > 1 && !validHorizontalPartialWord) ||
                 (verticalString.characters.count > 1 && !validVerticalPartialWord ) {
                    //print("AI : No valid partial word: - 5 points LetterTileSprite")
                    print ( "AI : Horizontal \(horizontalString) && Vertical \(verticalString) -> NOT valid partial words - 5 points !!! testForValidWordsAtDropSpot LetterTileSprite.swift" )
                    return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord)
            }
            else {
                print("AI : Horizontal \(horizontalString) && Vertical \(verticalString) : VALID partial words Horizontal AND Vertical! testForValidWordsAtDropSpot LetterTileSprite.swift")
                return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord)
            }
        }
        //print("AI : VALID partial words Horizontal AND Vertical! testForValidWordsAtDropSpot  LetterTileSprite")
        //return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord)
    }

    
    /// func updateWordsAtDropSpot (tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, touchX: Float, touchY: Float) ()
    /// used on touchesEnded to determine played tile
    /// -Parameters: tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, touchX: Float, touchY: Float
    /// -Returns: none
    /// used to test when a whole or partial word play exists at drop spot & updates tile data on successful play
    func updateWordsAtDropSpot (touchX: Float, touchY: Float) () {  // tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int,
        
        let tileSnapResults = mmwGameScene.mmwBoardGrid.getGridSquare(touchX, locY: touchY)
        let tileSnapResultsCalculateX = tileSnapResults.GridSquareUpperLeftCornerX
        let tileSnapResultsCalculateY = tileSnapResults.GridSquareUpperLeftCornerY - Double((mmwGame.screenSize?.height)! * 0.02018229167) // -15.5 on y touch point adjusts snapping too high to lower square
        let tileSnapResultsXGrid = tileSnapResults.GridSquareX
        let tileSnapResultsYGrid = tileSnapResults.GridSquareY
        let tileAtDropSpot : MMWTile = (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid]) // ERROR IF DRAG TOO FAR RIGHT _ INDEX OUT OF RANGE !!! ///////////
        
        var leftString : String = ""
        var rightString : String = ""
        var upString : String = ""
        var downString : String = ""
        var horizontalString : String = ""
        var verticalString : String = ""
        
        var possibleWordTilesHorizontal : [MMWTile] = []
        var possibleWordTilesVertical : [MMWTile] = []
        
        var numHorizontalAdjacentLetters = 0
        var numVerticalAdjacentLetters = 0
        var foundLockedTile = false
        
        self.tileSpriteParent.gridEnd = mmwGameScene.mmwBoardGrid // set tileSprite parent (MMWTile) grid to grid snapped to
        tileSpriteParent.gridXEnd = tileSnapResults.GridSquareX
        tileSpriteParent.gridYEnd = tileSnapResults.GridSquareY
        
        // LEFT
        var currentCheckXGridNum = tileSnapResultsXGrid
        var currentCheckYGridNum = tileSnapResultsYGrid
        var stringToAdd : String = ""
        while ( (currentCheckXGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.PlayedMadeWord) ) )  {
            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileText)"
            stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
            possibleWordTilesHorizontal.append((mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid]))
            //gameGrid?.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileOwner
            if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked {
                foundLockedTile = true // stops check on last last locked tile
                hasLockedPotentialWord = true
            }
            currentCheckXGridNum--
        }
        
        leftString = stringToAdd.stringByAppendingString(self.tileText)
  
        possibleWordTilesHorizontal = possibleWordTilesHorizontal.reverse()     ///

        possibleWordTilesHorizontal.append(tileSpriteParent)
        
        //RIGHT
        currentCheckXGridNum = tileSnapResultsXGrid
        stringToAdd = ""
        while ( (currentCheckXGridNum < 14) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.PlayedMadeWord) ) )  {
            numHorizontalAdjacentLetters++
            numHorizontalAdjacentLetters++
            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileText)"
            stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
            if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked {
                foundLockedTile = true
                hasLockedPotentialWord = true
            }
            possibleWordTilesHorizontal.append(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid])
            currentCheckXGridNum++
        }
        
        rightString = self.tileText.stringByAppendingString(stringToAdd)
        horizontalString = leftString.stringByAppendingString(stringToAdd)

        
        //UP
        stringToAdd = ""
        foundLockedTile = false
        while ( (currentCheckYGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.PlayedMadeWord) ) )  {
            numVerticalAdjacentLetters++
            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileText)"
            stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
            
            possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1]))
            if mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked {
                foundLockedTile = true
                hasLockedPotentialWord = true
            }
            currentCheckYGridNum--
        }
        
        upString = stringToAdd.stringByAppendingString(self.tileText)
        
        possibleWordTilesVertical = possibleWordTilesVertical.reverse()   ////
        
        possibleWordTilesVertical.append(tileSpriteParent)
        
        //DOWN
        currentCheckYGridNum = tileSnapResultsYGrid
        stringToAdd = ""
        while ( (currentCheckYGridNum < 14) && ( (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.PlayedMadeWord) ) ) {
            numVerticalAdjacentLetters++
            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileText)"
            stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
            possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1]))
            if mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked {
                foundLockedTile = true
                hasLockedPotentialWord = true
            }
            currentCheckYGridNum++
        }
        
        downString = self.tileText.stringByAppendingString(stringToAdd)
        verticalString = upString.stringByAppendingString(stringToAdd)
        
        ///////////////////
        
        
        if ( ( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(horizontalString) ) == false || horizontalString.characters.count < tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
            // updates GUI for feedback on horizonal partial word
            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("\(horizontalString) is a partial word (horizontal) updateWordsAtDropSpot LetterTileSprite")
            print ( "\(horizontalString) is a partial word (horizontal) updateWordsAtDropSpot LetterTileSprite" )
        }
            
        else {
            print ( "\(horizontalString) is a valid word (horizontal) updateWordsAtDropSpot LetterTileSprite ... possibleWordTilesHorizontal" )
            for tile in possibleWordTilesHorizontal{
                print("\(tile.tileText)  :  \(tile.tileState)")
            }
            tileSpriteParent.tileState = TileState.PlayedMadeWord
            
            if self.tileSpriteParent.playedMadeWord != PlayedMadeWord.Both {
                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Both
                }
                else {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Horizontal
                }
            }
//            scoreTilesInArr(possibleWordTilesHorizontal, tileArrToScoreVertical: possibleWordTilesVertical, wordStringHorizontal: horizontalString, wordStringVertical: verticalString)
//            for tile in possibleWordTilesHorizontal{
//                print("\(tile.tileText)  :  \(tile.tileState)")
//            }
        }
        
        ///////////////////
        
        
        
        if (( mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == false || verticalString.characters.count < tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
            // updates GUI for feedback on horizonal partial word
            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(verticalString) is a partial word (vertical) updateWordsAtDropSpot LetterTileSprite")
            print ( "\(verticalString) is a partial word (vertical) updateWordsAtDropSpot LetterTileSprite" )
        }
            
        else {
            print ( "\(verticalString) is a valid word (vertical) updateWordsAtDropSpot LetterTileSprite" )
            tileSpriteParent.tileState = TileState.PlayedMadeWord
            
            if self.tileSpriteParent.playedMadeWord != PlayedMadeWord.Both {
                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Both
                }
                else {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Vertical
                }
            }
            
//            scoreTilesInArr(possibleWordTilesHorizontal, tileArrToScoreVertical: possibleWordTilesVertical, wordStringHorizontal: horizontalString, wordStringVertical: verticalString)
//            for tile in possibleWordTilesVertical{
//                print("\(tile.tileText)  :  \(tile.tileState)")
//            }
        }
        
        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
            scoreTilesInArr(possibleWordTilesHorizontal, tileArrToScoreVertical: possibleWordTilesVertical, wordStringHorizontal: horizontalString, wordStringVertical: verticalString)
        }
    
        //////////////////

        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        let replacementPlaceholderTile : MMWTile = MMWTile()
        replacementPlaceholderTile.gridHome = self.tileSpriteParent.gridHome
        replacementPlaceholderTile.gridX = self.tileSpriteParent.gridX
        replacementPlaceholderTile.gridY = self.tileSpriteParent.gridY
        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile

        // set value of snap results grid location to the MMWTile if valid location
        self.tileSpriteParent.gridEnd? = self.tileSpriteParent.gridEnd!
        self.tileSpriteParent.gridEnd?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid] = self.tileSpriteParent

        self.tileSpriteParent.gridXTest = self.tileSpriteParent.gridX
        self.tileSpriteParent.gridYTest = self.tileSpriteParent.gridY

        self.tileSpriteParent.gridXEnd = tileSnapResultsXGrid
        self.tileSpriteParent.gridYEnd = tileSnapResultsYGrid
        
//        self.tileSpriteParent.gridXTest = tileSnapResultsXGrid
//        self.tileSpriteParent.gridYTest = tileSnapResultsYGrid
        
//        self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
//        self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
        

        // move tile to snap point
        self.position.x = CGFloat(tileSnapResultsCalculateX)  + ( CGFloat((mmwGame.screenSize?.width)!) * CGFloat(0.023193359375))  //adjusts 23.75 for tile center in middle of tile
        self.position.y = CGFloat((mmwGame.screenSize?.height)!) - ( CGFloat(tileSnapResultsCalculateY) + ( CGFloat((mmwGame.screenSize?.height)!) * 0.0107421875) ) //38 adjusts for tile center and for board not in exact middle when flipping coords
        
        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
            tileSpriteParent.tileState = TileState.Locked // if put on valid board location set TileState to played if NOT already locked
        }

        self.isMovable = false // so can't remove tile once placed
        self.userInteractionEnabled = false
        
        removeBoardTileHighlights ()
        
        self.tileSpriteParent.gridHome?.mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
    }

    
    
    
    
    
//    /// func updateWordsAtDropSpot2 (tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, touchX: Float, touchY: Float) ()
//    /// used on touchesEnded to determine played tile
//    /// -Parameters: tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, touchX: Float, touchY: Float
//    /// -Returns: none
//    /// used to test when a whole or partial word play exists at drop spot & updates tile data on successful play
//    func updateWordsAtDropSpot2(tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int) {
//        
//        let tileSnapResultsXGrid = tileSnapResultsXGrid
//        let tileSnapResultsYGrid = tileSnapResultsYGrid
//        var tileAtDropSpot = mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid]
//        
//        var leftString : String = ""
//        var rightString : String = ""
//        var upString : String = ""
//        var downString : String = ""
//        var horizontalString : String = ""
//        var verticalString : String = ""
//        
//        var possibleWordTilesHorizontal : [MMWTile] = []
//        var possibleWordTilesVertical : [MMWTile] = []
//        
//        var numHorizontalAdjacentLetters = 0
//        var numVerticalAdjacentLetters = 0
//        var foundLockedTile = false
//        
//        self.tileSpriteParent.gridEnd = mmwGameScene.mmwBoardGrid // set tileSprite parent (MMWTile) grid to grid snapped to
//        tileSpriteParent.gridXEnd = tileSnapResultsXGrid
//        tileSpriteParent.gridYEnd = tileSnapResultsYGrid
//        
//        // LEFT
//        var currentCheckXGridNum = tileSnapResultsXGrid
//        var currentCheckYGridNum = tileSnapResultsYGrid
//        var stringToAdd : String = ""
//        while ( (currentCheckXGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.PlayedMadeWord) ) )  {
//            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileText)"
//            stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
//            possibleWordTilesHorizontal.append((mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid]))
//            //gameGrid?.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileOwner
//            if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked {
//                foundLockedTile = true // stops check on last last locked tile
//                hasLockedPotentialWord = true
//            }
//            currentCheckXGridNum--
//        }
//        
//        leftString = stringToAdd.stringByAppendingString(self.tileText)
//        possibleWordTilesHorizontal.append(tileSpriteParent)
//        
//        //RIGHT
//        currentCheckXGridNum = tileSnapResultsXGrid
//        stringToAdd = ""
//        while ( (currentCheckXGridNum < 14) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.PlayedMadeWord)) )  {
//            numHorizontalAdjacentLetters++
//            numHorizontalAdjacentLetters++
//            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileText)"
//            stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
//            if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked {
//                foundLockedTile = true
//                hasLockedPotentialWord = true
//            }
//            
//            possibleWordTilesHorizontal.append(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid])
//            currentCheckXGridNum++
//        }
//        rightString = self.tileText.stringByAppendingString(stringToAdd)
//        horizontalString = leftString.stringByAppendingString(stringToAdd)
//        
//        if ( ( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(horizontalString) ) == false || horizontalString.characters.count < tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
//            // updates GUI for feedback on horizonal partial word
//            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("\(horizontalString) is a partial word (horizontal) updateWordsAtDropSpot LetterTileSprite")
//            print ( "\(horizontalString) is a partial word (horizontal) updateWordsAtDropSpot LetterTileSprite" )
//
//        }
//            
//        else {
//            print ( "\(horizontalString) is a valid word (horizontal) updateWordsAtDropSpot LetterTileSprite" )
//            scoreTilesInArr(possibleWordTilesHorizontal, wordString: horizontalString)
//        }
//        
//        //playTileToBoardGrid (0)
//
//        //UP
//        stringToAdd = ""
//        foundLockedTile = false
//        while ( (currentCheckYGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.PlayedMadeWord) ) )  {
//            numVerticalAdjacentLetters++
//            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileText)"
//            stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
//            
//            possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1]))
//            if mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked {
//                foundLockedTile = true
//                hasLockedPotentialWord = true
//            }
//            currentCheckYGridNum--
//        }
//        
//        upString = stringToAdd.stringByAppendingString(self.tileText)
//        possibleWordTilesVertical.append(tileSpriteParent)
//        
//        //DOWN
//        currentCheckYGridNum = tileSnapResultsYGrid
//        stringToAdd = ""
//        while ( (currentCheckYGridNum < 14) && ( (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.PlayedMadeWord) ) ) {
//            numVerticalAdjacentLetters++
//            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileText)"
//            stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
//            possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1]))
//            if mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked {
//                foundLockedTile = true
//                hasLockedPotentialWord = true
//            }
//            currentCheckYGridNum++
//        }
//        
//        downString = self.tileText.stringByAppendingString(stringToAdd)
//        verticalString = upString.stringByAppendingString(stringToAdd)
//        
//        if (( mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == false || verticalString.characters.count < tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
//            // updates GUI for feedback on horizonal partial word
//            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(verticalString) is a partial word (vertical) updateWordsAtDropSpot LetterTileSprite")
//            print ( "\(verticalString) is a partial word (vertical) updateWordsAtDropSpot LetterTileSprite" )
//        }
//            
//        else {
//            print ( "\(verticalString) is a valid word (vertical) updateWordsAtDropSpot LetterTileSprite" )
//            //scoreTilesInArr(possibleWordTilesVertical, wordString: verticalString)
//        }
//        
//        
////        scoreTilesInArr(possibleWordTilesHorizontal, wordString: horizontalString) // IF VALID HORIZONTAL WORD
////        //scoreTilesInArr(possibleWordTilesVertical, wordString: verticalString)   // IF VALID VERTICAL WORD
////        
////        if (tileSpriteParent.tileState != TileState.Locked) {
////            tileSpriteParent.tileState = TileState.Played // if put on valid board location set TileState to played if NOT already locked
////        }
////        
////        
////        // set value of snap results grid location to the MMWTile if valid location
////        self.tileSpriteParent.gridEnd? = mmwGameScene.mmwBoardGrid
////        self.tileSpriteParent.gridEnd?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid] = self.tileSpriteParent
////        self.tileSpriteParent.gridXEnd = tileSnapResultsXGrid
////        self.tileSpriteParent.gridYEnd = tileSnapResultsYGrid
////        
////        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
////        let replacementPlaceholderTile : MMWTile = MMWTile()
////        replacementPlaceholderTile.gridHome = self.tileSpriteParent.gridHome
////        replacementPlaceholderTile.gridX = self.tileSpriteParent.gridX
////        replacementPlaceholderTile.gridY = self.tileSpriteParent.gridY
////        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
////        
////        //        // move tile to snap point
////        //        self.position.x = (CGFloat)(tileSnapResultsCalculateX + 23.75)  //adjusts 22.5 for tile center in middle of tile
////        //        self.position.y = 768 - (CGFloat)(tileSnapResultsCalculateY + 8.25) //38 adjusts for tile center and for board not in exact middle when flipping coords
////        
////        self.isMovable = false // so can't remove tile once placed
////        self.userInteractionEnabled = false
////        //self.lockTile()
////        
////        removeBoardTileHighlights ()
////        
////        self.tileSpriteParent.gridHome?.mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
//
//        if (tileSpriteParent.tileState != TileState.Locked) {
//            tileSpriteParent.tileState = TileState.Played // if put on valid board location set TileState to played if NOT already locked
//        }
//        
//        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
//        let replacementPlaceholderTile : MMWTile = MMWTile()
//        replacementPlaceholderTile.gridHome = self.tileSpriteParent.gridHome
//        replacementPlaceholderTile.gridX = self.tileSpriteParent.gridX
//        replacementPlaceholderTile.gridY = self.tileSpriteParent.gridY
//        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
//        
//        // set value of snap results grid location to the MMWTile if valid location
//        self.tileSpriteParent.gridEnd? = mmwGameScene.mmwBoardGrid
//        
//        self.tileSpriteParent.gridXEnd = tileSnapResultsXGrid
//        self.tileSpriteParent.gridYEnd = tileSnapResultsYGrid
//        self.isMovable = false // so can't remove tile once placed
//        self.userInteractionEnabled = true
//   
//        self.playTileToBoardGrid(0)
//        
//        self.tileSpriteParent.gridEnd?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid] = self.tileSpriteParent
//
//        removeBoardTileHighlights ()
//
//        self.tileSpriteParent.gridHome?.mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
//    }


    /// func updateAIWordsAtDropSpot (tileXGridDestination: Int, tileYGridDestination: Int) ()
    /// used on AI play of tile
    /// -Parameters: tileXGridDestination: Int, tileYGridDestination: Int
    /// -Returns: none
    /// used to send an AI tile to board on valid tile play -- AFTER successful testForValidWordsAtDropSpot(tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid, isAI: false)
    func updateAIWordsAtDropSpot (tileXGridDestination: Int, tileYGridDestination: Int, madeValidWord: Bool ) {    // isWholeWord : Bool  //

        var tileAtDropSpot : MMWTile = (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][tileYGridDestination])
        
        var leftString       : String = ""
        var rightString      : String = ""
        var upString         : String = ""
        var downString       : String = ""
        var horizontalString : String = ""
        var verticalString   : String = ""
        var stringToAdd      : String = ""
        
        var possibleWordTilesHorizontal : [MMWTile] = []
        var possibleWordTilesVertical   : [MMWTile] = []
        
        var numHorizontalAdjacentLetters = 0
        var numVerticalAdjacentLetters   = 0
        var foundLockedTile = false
        
        tileSpriteParent.gridXEnd = tileXGridDestination
        tileSpriteParent.gridYEnd = tileYGridDestination
        self.tileSpriteParent.gridEnd = mmwGameScene.mmwBoardGrid // set tileSprite parent (MMWTile) grid to grid snapped to
        mmwGameScene.mmwBoardGrid.grid2DArr[tileSpriteParent.gridXEnd][tileSpriteParent.gridYEnd] = self.tileSpriteParent // set tileSprite parent (MMWTile) grid to grid snapped to
        
        var currentCheckXGridNum = tileXGridDestination
        var currentCheckYGridNum = tileYGridDestination
        
        
        /////////////////////////////
        
        // LEFT

        while ( (currentCheckXGridNum > 0)
            && ((mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileYGridDestination].tileState == TileState.Locked
            ||  (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileYGridDestination].tileState == TileState.Played)
            ||  (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileYGridDestination].tileState == TileState.PlayedMadeWord)) ) )  {
                
            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileYGridDestination].tileText)"
            stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
            possibleWordTilesHorizontal.append((mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileYGridDestination]))
            if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][tileYGridDestination].tileState == TileState.Locked {
                foundLockedTile = true // stops check on last last locked tile
                hasLockedPotentialWord = true
            }
            currentCheckXGridNum--
        }
        
        leftString = stringToAdd.stringByAppendingString(self.tileText)
        
        possibleWordTilesHorizontal = possibleWordTilesHorizontal.reverse()
        possibleWordTilesHorizontal.append(tileSpriteParent)
        
        // RIGHT
        currentCheckXGridNum = tileXGridDestination
        stringToAdd = ""
        while ( (currentCheckXGridNum < 14) &&
            (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileYGridDestination].tileState == TileState.Locked ||
            (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileYGridDestination].tileState == TileState.Played) ||
            (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileYGridDestination].tileState == TileState.PlayedMadeWord)) )  {
            numHorizontalAdjacentLetters++
            numHorizontalAdjacentLetters++
            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileYGridDestination].tileText)"
            stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
            if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileYGridDestination].tileState == TileState.Locked {
                foundLockedTile = true
                hasLockedPotentialWord = true
            }
            possibleWordTilesHorizontal.append(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileYGridDestination])
            currentCheckXGridNum++
        }
 
        rightString =   self.tileText.stringByAppendingString(stringToAdd)
        horizontalString = leftString.stringByAppendingString(stringToAdd)
        
        if ( ( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(horizontalString) ) == true ) {
            print ( "\(horizontalString) is a partial word string (horizontal) updateAIWordsAtDropSpot LetterTileSprite" )
            self.tileSpriteParent.tileState = TileState.Played
        }

        if ( ( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(horizontalString) ) == true && horizontalString.characters.count >= tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
            print ( "\(horizontalString) is a valid whole word string (horizontal) updateAIWordsAtDropSpot LetterTileSprite" )
            self.tileSpriteParent.tileState = TileState.PlayedMadeWord
            
            if self.tileSpriteParent.playedMadeWord != PlayedMadeWord.Both {
                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Both
                }
                else {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Horizontal
                }
            }

            for tile in possibleWordTilesHorizontal{
                print("\(tile.tileText)  :  \(tile.tileState)")
            }
        }
        else {
            print ( "\(horizontalString) is NOT a valid word string (horizontal) updateAIWordsAtDropSpot LetterTileSprite" )
        }

        /////////////////////////////

        // UP
        currentCheckXGridNum = tileXGridDestination
        currentCheckYGridNum = tileYGridDestination
        stringToAdd = ""
        while ( (currentCheckYGridNum > 0)
            && ((mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum-1].tileState == TileState.Locked
                ||  (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum-1].tileState == TileState.Played)
                ||  (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum-1].tileState == TileState.PlayedMadeWord)) ) )  {
                    
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum-1].tileText)"
                    stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
                    possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum-1]))
                    if mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum-1].tileState == TileState.Locked {
                        foundLockedTile = true // stops check on last last locked tile
                        hasLockedPotentialWord = true
                    }
                    currentCheckYGridNum--
        }
        
        upString = stringToAdd.stringByAppendingString(self.tileText)
        
        possibleWordTilesVertical = possibleWordTilesVertical.reverse()
        possibleWordTilesVertical.append(tileSpriteParent)
        
        // DOWN
        currentCheckYGridNum = tileYGridDestination
        stringToAdd = ""
        while ( (currentCheckYGridNum < 14) &&
            (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileState == TileState.Locked ||
                (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileState == TileState.Played) ||
                (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileState == TileState.PlayedMadeWord)) )  {
                    numVerticalAdjacentLetters++
                    numVerticalAdjacentLetters++
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileText)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    if mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileState == TileState.Locked {
                        foundLockedTile = true
                        hasLockedPotentialWord = true
                    }
                    possibleWordTilesVertical.append(mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1])
                    currentCheckYGridNum++
        }
        
        downString =   self.tileText.stringByAppendingString(stringToAdd)
        verticalString = upString.stringByAppendingString(stringToAdd)
        
        if ( ( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) ) == true ) && self.tileSpriteParent.tileState != TileState.PlayedMadeWord {
            print ( "\(verticalString) is a partial word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
            self.tileSpriteParent.tileState = TileState.Played
        }
        
        if ( ( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == true && verticalString.characters.count >= tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
            print ( "\(verticalString) is a valid whole word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
            self.tileSpriteParent.tileState = TileState.PlayedMadeWord
            
            if self.tileSpriteParent.playedMadeWord != PlayedMadeWord.Both {
                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Both
                }
                else {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Vertical
                }
            }

            for tile in possibleWordTilesVertical{
                print("\(tile.tileText)  :  \(tile.tileState)")
            }
        }
        else {
            print ( "\(verticalString) is NOT a valid word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
        }
 
        /////////////////////////////
        

        // set value of snap results grid location to the MMWTile if valid location
        self.tileSpriteParent.gridEnd? = mmwGameScene.mmwBoardGrid
        self.tileSpriteParent.gridEnd?.grid2DArr[tileXGridDestination][tileYGridDestination] = self.tileSpriteParent
        self.tileSpriteParent.gridXEnd = tileXGridDestination
        self.tileSpriteParent.gridYEnd = tileYGridDestination

        self.playTileToBoardGrid(0)  // animation to move to position on board and set Grid, x and y values for tile at tile End Position
        
        
        
        
        
        
//        var madeValidWord = false
//        if self.tileSpriteParent.tileState == TileState.PlayedMadeWord {
//            madeValidWord = true
//        }
//        
//        let tileLocation = mmwGameScene.mmwBoardGrid.sendToGridSquare(self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
//        //self.updateWordsAtDropSpot( self.tileSpriteParent.gridXEnd, tileSnapResultsYGrid: self.tileSpriteParent.gridYEnd, touchX: Float(tileLocation.x), touchY: Float(tileLocation.y) )
//        
//        self.updateAIWordsAtDropSpot(self.tileSpriteParent.gridXEnd, tileYGridDestination: self.tileSpriteParent.gridYEnd, madeValidWord: madeValidWord)
        

        
        
        
        
        
        
        if self.tileSpriteParent.tileState == TileState.PlayedMadeWord  {
            delay(0.25 * Double(possibleWordTilesHorizontal.count)) {
                self.scoreTilesInArr(possibleWordTilesHorizontal, tileArrToScoreVertical: possibleWordTilesVertical, wordStringHorizontal: horizontalString, wordStringVertical: verticalString)
            }
        }
        
        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        let replacementPlaceholderTile : MMWTile = MMWTile()
        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
        
//        tileSpriteParent.gridHome = self.tileSpriteParent.gridEnd
//        tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
//        tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd

        self.tileSpriteParent.gridHome?.mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
    }


    /// func scoreTilesInArr (tileArrToScore: [MMWTile], wordString: String) -> Int
    /// -Parameters: tileArrToScore: [MMWTile], wordString: String
    /// -Returns: Int
    /// counts tile in up/down or left/right tile array and adds points for unlocked tiles and complete word and kicks off points awarded animations
    func scoreTilesInArr (tileArrToScoreHorizontal: [MMWTile], tileArrToScoreVertical: [MMWTile], wordStringHorizontal: String, wordStringVertical: String) -> Int {
        //tileArrToScore = tileArrToScore.reverse()
        var numPasses = 0
        var tileArrToScore = tileArrToScoreVertical
        var tileNum = 0.0
        var delayTime = 0.0
        
        while numPasses <= 1 {
            print("### ScoreTileInArray: \(numPasses)")
            if numPasses == 1 {
                tileArrToScore = tileArrToScoreHorizontal
            }
            let wordLen = tileArrToScore.count
            
            tileNum = 0.0
            
            //tileArrToScore = tileArrToScore.reverse()
            // + 2*wordLen points for locking a tile // SCORE  // display points awarded animation (for locked tile)
            for tile in tileArrToScore {
                let tileWordState = tile.tileSprite.testForValidWordsAtDropSpot(tile.gridXEnd, tileSnapResultsYGrid: tile.gridYEnd, isAI: true)
                var pointsForTile = 0
                
                if numPasses == 0 {
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("\(wordStringVertical) is a valid Horizontal partial/whole word scoreTilesInArr LetterTileSprite")
                    
                }
                else {
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(wordStringHorizontal) is a valid Horizontal partial/whole word scoreTilesInArr LetterTileSprite")
                }
                
                print("Tile: \(tile.tileText) \(tile.tileState) \(tile.tileSprite.tileSpriteParent.spritename)")
                if tile.tileState == TileState.Played { // || tile.tileState == TileState.PlayedMadeWord {
                    
                    if wordLen >= self.mmwGameSceneViewController.minWordSize {
                        let pointsForTile = wordLen * 2     // + wordLen points for each unlocked letter to tile owner // SCORE
                        self.adjustPlayerPoints(pointsForTile, player: (self.mmwGameScene.mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1]) )
                        if tile.tileState == TileState.Played  {
                            tile.tileState = TileState.Locked
                            delayTime += 0.5
                        }

                        
                        delay(delayTime) {
                            tile.tileSprite.showTileScoreTextToGridHome (1, pointsForTile: pointsForTile )
                            
                            tile.gridHome = tile.gridEnd
                            tile.gridX = tile.gridXEnd
                            tile.gridY = tile.gridYEnd
                            
                            tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
                            print("#POINTS for locking tile: \(pointsForTile) : \(self.tileText) :  Player : \(self.mmwGameScene.mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1])) ")
                            tile.mmwGameScene?.explosion(tile.tileSprite.position)
                            
                            if tileWordState.validHorizontalWholeWord && tileWordState.validVerticalWholeWord {
                                tile.tileSprite.color = UIColor.blackColor()
                            }
                            else {
                                tile.tileSprite.color = UIColor.blackColor()
                                //tile.tileSprite.color = UIColor.grayColor()
                            }
                         }
                    }
                }
                pointsForTile = 0
                ++tileNum
            }
            numPasses++
        }
        
        // + 5 * wordLen points for making a new complete word // SCORE  // display points awarded animation (for completing whole word)
        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
            
            //tileSpriteParent.mmwGameScene?.explosion(tileSpriteParent.tileSprite.position)
            
            
            //let tileWordState = self.testForValidWordsAtDropSpot(self.tileSpriteParent.gridXEnd, tileSnapResultsYGrid: self.tileSpriteParent.gridYEnd, isAI: true)
            var pointsForCompletingWord = 0
            if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
                //self.color = UIColor.grayColor()
                pointsForCompletingWord = tileArrToScoreVertical.count * 5
                print("+POINTS for completing word VERTICAL : \(pointsForCompletingWord) : \(self.tileText) ")
                tileSpriteParent.tileState = TileState.Locked
            }
            
            if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal {
                //self.color = UIColor.grayColor()
                pointsForCompletingWord = tileArrToScoreHorizontal.count * 5
                print("+POINTS for completing word HORIZONTAL : \(pointsForCompletingWord) : \(self.tileText) ")
                tileSpriteParent.tileState = TileState.Locked
            }
                
            else if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Both {
                //self.color = UIColor.blackColor()
                pointsForCompletingWord = ( (tileArrToScoreVertical.count * 5) + (tileArrToScoreHorizontal.count * 5) )
                print("+POINTS for completing word BOTH : \(pointsForCompletingWord) : \(self.tileText) ")
                tileSpriteParent.tileState = TileState.Locked
            }
            
            let delay = SKAction.waitForDuration( delayTime + 0.5 )
            self.runAction( delay ) {

                //run code here after delay secs
                //if wordLen >= self.mmwGameSceneViewController.minWordSize {
                    self.showWordScoreTextToGridHome(pointsForCompletingWord, isHorizontalScore: true)      // doesn't actually use value, already set in setting letterScore.2 above
                    self.tileSpriteParent.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
                    self.color = UIColor.blackColor()
                    self.adjustPlayerPoints(pointsForCompletingWord, player: (self.mmwGameScene.mmwGameSceneViewController.playerArray[self.tileSpriteParent.tileOwner.rawValue - 1]) )
                    print("#POINTS for completing word: \(pointsForCompletingWord) : \(self.tileText) ")
                
                self.tileSpriteParent.gridHome = self.tileSpriteParent.gridEnd
                self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
                self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
            }
        }

        for tile in tileArrToScoreHorizontal {                                                                        // Lock all tiles in locked word tile array
            if tile.tileState == TileState.Played || tile.tileState == TileState.PlayedMadeWord  {
                tile.tileState = TileState.Locked
            }
//            tile.gridHome = tile.gridEnd
//            tile.gridX = tile.gridXEnd
//            tile.gridY = tile.gridYEnd
        }
        
        for tile in tileArrToScoreVertical {                                                                        // Lock all tiles in locked word tile array
            if tile.tileState == TileState.Played || tile.tileState == TileState.PlayedMadeWord  {
                tile.tileState = TileState.Locked
            }
//                tile.gridHome = tile.gridEnd
//                tile.gridX = tile.gridXEnd
//                tile.gridY = tile.gridYEnd
            //}
        }
        
        return 0
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    func adjustPlayerPoints (points: Int, player : Player) {
        player.playerScore += points
        player.playerView.playerScoreLabel.text = String(player.playerScore)
    }
    
    
    func checkForTouches(touches: NSSet) {
//        let touch = touches.anyObject() as! UITouch
//        let location = touch.locationInNode(self)
//        let nodes = nodesAtPoint(location) as! [SKNode]
//        for node in nodes {
//            if node.isKindOfClass(SKSpriteNode.self) {
//                let sprite = node as! SKSpriteNode
//                //println("Selected SK SpriteNode ...  Fred? .. sprite.name? \(sprite.name) description? \(sprite.description)")
//                let spriteParent = sprite.parent
//                //println("Selected spriteParent ...  Fred? .. sprite.name? \(spriteParent?.name) description? \(spriteParent?.description)")
//                if spriteParent!.name == "P" {
//                    //spriteParent!.name = "Pselected"
//                    //spriteParent.colorBlendFactor = 0
//                    println("Selected spriteParent.name P")
//                }
//            }
//        }
    }
}
