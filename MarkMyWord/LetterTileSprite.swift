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
    
    var mmwGameSceneViewController : MMWGameSceneViewController! = nil
    var mmwGameScene : MMWGameScene! = nil
    var tileSpriteParent : MMWTile! = nil
    
    let letterLabel = SKLabelNode(fontNamed: FontHUDName)
    var isMovable : Bool = true
    var faceUp : Bool = true
    var hasShadow : Bool = false
    var tileText = ""

//    var tileScore1 : SKLabelNode = SKLabelNode()
//    var tileScore2 : SKLabelNode = SKLabelNode()
//    var tileScore3 : SKLabelNode = SKLabelNode()
 
    var frontTexture : SKTexture? = SKTexture(imageNamed: "Tile3D95x")
    var backTexture : SKTexture? = SKTexture(imageNamed: "Tile3D95xBack") // "TileBackTest90x90")
    //var glowTexture : SKTexture = SKTexture(imageNamed: "Tile3D90xGLOW")
    var largeTexture : SKTexture?
    var largeTextureFilename : String = "Tile3D95x"
    var tileShadow : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "TileShadow90x90"), color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSizeMake(50.0, 50.0))
    var tileGlow : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "Tile3D100xGLOW"), color: UIColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSizeMake(50.0, 50.0))
    var tileLocation : CGPoint = CGPointMake(0, 0)
    var enlarged = false
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

    let actionSound = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    let actionSound2 = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    let dealTilesSound = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
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
        
        let myCGSize = CGSizeMake(47.5, 47.5)
        super.init(texture: frontTexture, color: UIColorAppleBlue, size: myCGSize)
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
        tile.position.x += 23.75  // 22.5?
        tile.position.y += 23.75  // 22.5?
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
            
            let liftUp = SKAction.scaleTo(1.2, duration: 0.1)
            runAction(liftUp, withKey: "pickup")
            tileShadow.zPosition = self.zPosition - 1
            tileShadow.hidden = false

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
            if (tileSnapTouch.x > 160 && tileSnapTouch.x < 860) && (tileSnapTouch.y > 50 && tileSnapTouch.y < 747) { // checks that drag location within game grid boundaries
                
                let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
                let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y)) // gets grid x, y on tile drag
                
                let tileSnapResultsXGrid = tileSnapResults.GridSquareX
                let tileSnapResultsYGrid = tileSnapResults.GridSquareY
                
                checkForValidWordsOnTileDrag(tileSnapResultsXGrid, gridYSpot: tileSnapResultsYGrid, IsAI: false)
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
        
        for touch in (touches as Set<UITouch>) {
            zPosition = 10
            let dropDown = SKAction.scaleTo(1.0, duration: 0.1)
            runAction(dropDown, withKey: "drop")
            tileShadow.hidden = true
            
            let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)

            // IF NOT VALID DROP LOCATION ON BOARD, RETURN TILE TO PLAYER
            if (tileSnapTouch.x <= 160 || tileSnapTouch.x >= 860) {
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
                    
                    
                    
                    updateWordsAtDropSpot(tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid, touchX: Float(tileSnapTouch.x), touchY: Float(tileSnapTouch.y)) ()
                }
            }
            print("tile drop = \(tileSnapResultsXGrid), \(tileSnapResultsYGrid)")
        }

        mmwGameSceneViewController.resetConsequtivePasses()
        mmwGameSceneViewController.lettersPlayedInTurn += 1
        print("letters playedInTurn = \(mmwGameSceneViewController.lettersPlayedInTurn)")
        
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
        self.zPosition = 25
        let returnPosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridY)
        let slide = SKAction.moveTo(returnPosition, duration:0.3)
        let scaleUp = SKAction.scaleTo(1.5, duration:0.15)
        let scaleDown = SKAction.scaleTo(1.0, duration:0.15)
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

        self.zPosition = 100
        let boardPosition = Grid.sendToGridSquare(self.mmwGameScene.mmwBoardGrid, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
        
        let slide = SKAction.moveTo(boardPosition, duration:1.0)
        let scaleUp = SKAction.scaleTo(1.5, duration:0.5)
        let scaleDown = SKAction.scaleTo(1.0, duration:0.5)
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
        self.zPosition = 10
        
        
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
    func showWordScoreTextToGridHome (number: Int) {
        
        var tileScore = SKLabelNode()
        
        tileScore = createLetterScoreText (CGPointMake(0.0, 0.0), endLocation: CGPointMake(0.0, 0.0), textColor: gameColors[self.tileSpriteParent.tileOwner.rawValue], displayText: String("") )
        tileScore.fontColor = gameColors[self.tileSpriteParent.tileOwner.rawValue]
        tileScore.fontSize = 65
        tileScore.fontName = FontHUDName
        tileScore.text = (String(number) )
        tileScore.zPosition = 75
        
        let endPosition = Grid.sendToGridSquare(self.tileSpriteParent.gridEnd!, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
        let homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridX)
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
        print("checkForValidWords \(gridXSpot),\(gridXSpot), \(IsAI)")
        
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
                    possibleWordTilesHorizontal.append(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid])
                    currentCheckXGridNum++
                }
            
                rightString = stringToAdd
                horizontalString = leftString.stringByAppendingString(rightString)
                
                if (( mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(horizontalString)) == false) {
                    validHorizontalPartialWord = false
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Horizontal \(horizontalString)-> Not a valid partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
                    print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> INVALID horizontal partial word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift" )
                }
                else {
                    validHorizontalPartialWord = true
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Horizontal \(horizontalString)-> VALID partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
                    print ( "Horizontal L:\(leftString) R:\(rightString) H:\(horizontalString)-> VALID horizontal partial word !!! testForValidWordsAtDropSpot  LetterTIleSprite.swift" )
                }
            
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
                    currentCheckYGridNum++
                }
            
                downString = stringToAdd
                verticalString = upString.stringByAppendingString(downString)

                if mmwGameScene.mmwBoardGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) == false {
                    // updates GUI for feedback on horizonal partial word
                    validVerticalPartialWord = false
                    validVerticalWholeWord = false
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(verticalString)-> Not a valid partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
                    print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> INVALID horizontal partial word !!! testForValidPartialWordsAtDropSpot  LetterTIleSprite.swift" )
                }
                    
                else {
                    validVerticalPartialWord = true
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("Vertical \(verticalString)-> VALID partial word !!! testForValidWordsAtDropSpot LetterTileSprite")
                    print ( "Vertical U:'\(upString)' D:'\(downString)' V:'\(verticalString)'-> VALID vartical partial word !!! testForValidWordsAtDropSpot  LetterTileSprite.swift"  )
                }
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
    func updateWordsAtDropSpot (tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, touchX: Float, touchY: Float) () {
        
        let tileSnapResults = mmwGameScene.mmwBoardGrid.getGridSquare(touchX, locY: touchY)
        let tileSnapResultsCalculateX = tileSnapResults.GridSquareUpperLeftCornerX
        let tileSnapResultsCalculateY = tileSnapResults.GridSquareUpperLeftCornerY - 15.5 // -15.5 on y touch point adjusts snapping too high to lower square
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
            scoreTilesInArr(possibleWordTilesHorizontal, wordString: horizontalString)
            for tile in possibleWordTilesHorizontal{
                print("\(tile.tileText)  :  \(tile.tileState)")
            }
        }
        
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
        
        if (( mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == false || verticalString.characters.count < tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
            // updates GUI for feedback on horizonal partial word
            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(verticalString) is a partial word (vertical) updateWordsAtDropSpot LetterTileSprite")
            print ( "\(verticalString) is a partial word (vertical) updateWordsAtDropSpot LetterTileSprite" )
        }
            
        else {
            print ( "\(verticalString) is a valid word (vertical) updateWordsAtDropSpot LetterTileSprite" )
            tileSpriteParent.tileState = TileState.PlayedMadeWord
            scoreTilesInArr(possibleWordTilesVertical, wordString: verticalString)
        }
        
        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        let replacementPlaceholderTile : MMWTile = MMWTile()
        replacementPlaceholderTile.gridHome = self.tileSpriteParent.gridHome
        replacementPlaceholderTile.gridX = self.tileSpriteParent.gridX
        replacementPlaceholderTile.gridY = self.tileSpriteParent.gridY
        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile

        // set value of snap results grid location to the MMWTile if valid location
        self.tileSpriteParent.gridEnd? = self.tileSpriteParent.gridEnd!
        self.tileSpriteParent.gridEnd?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid] = self.tileSpriteParent
        
//        self.tileSpriteParent.gridX = tileSnapResultsXGrid
//        self.tileSpriteParent.gridY = tileSnapResultsYGrid
        
        self.tileSpriteParent.gridXEnd = tileSnapResultsXGrid
        self.tileSpriteParent.gridYEnd = tileSnapResultsYGrid
        
        self.tileSpriteParent.gridXTest = tileSnapResultsXGrid
        self.tileSpriteParent.gridYTest = tileSnapResultsYGrid

        // move tile to snap point
        self.position.x = (CGFloat)(tileSnapResultsCalculateX + 23.75)  //adjusts 22.5 for tile center in middle of tile
        self.position.y = 768 - (CGFloat)(tileSnapResultsCalculateY + 8.25) //38 adjusts for tile center and for board not in exact middle when flipping coords
        
        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
            tileSpriteParent.tileState = TileState.Locked // if put on valid board location set TileState to played if NOT already locked
        }

        self.isMovable = false // so can't remove tile once placed
        self.userInteractionEnabled = false
        //self.lockTile()
        
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

        let tileAtDropSpot : MMWTile = (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][tileYGridDestination])
        
        var leftString  : String = ""
        var rightString : String = ""
        var upString   : String = ""
        var downString : String = ""
        var horizontalString : String = ""
        var verticalString   : String = ""
        
        var possibleWordTilesHorizontal : [MMWTile] = []
        var possibleWordTilesVertical   : [MMWTile] = []
        
        var numHorizontalAdjacentLetters = 0
        var numVerticalAdjacentLetters = 0
        var foundLockedTile = false
        
        tileSpriteParent.gridXEnd = tileXGridDestination
        tileSpriteParent.gridYEnd = tileYGridDestination
        self.tileSpriteParent.gridEnd = mmwGameScene.mmwBoardGrid // set tileSprite parent (MMWTile) grid to grid snapped to
        mmwGameScene.mmwBoardGrid.grid2DArr[tileSpriteParent.gridXEnd][tileSpriteParent.gridYEnd] = self.tileSpriteParent // set tileSprite parent (MMWTile) grid to grid snapped to
        
        // LEFT
        var currentCheckXGridNum = tileXGridDestination
        var currentCheckYGridNum = tileYGridDestination
        var stringToAdd : String = ""
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
            
            tileSpriteParent.tileState = TileState.PlayedMadeWord
            //scoreTilesInArr(possibleWordTilesHorizontal, wordString: horizontalString)
            for tile in possibleWordTilesHorizontal{
                print("\(tile.tileText)  :  \(tile.tileState)")
            }
        }
        
        else {
            print ( "\(horizontalString) is NOT a valid word string (horizontal) updateAIWordsAtDropSpot LetterTileSprite" )
        }
        
        // set value of snap results grid location to the MMWTile if valid location
        self.tileSpriteParent.gridEnd? = mmwGameScene.mmwBoardGrid
        self.tileSpriteParent.gridEnd?.grid2DArr[tileXGridDestination][tileYGridDestination] = self.tileSpriteParent
        self.tileSpriteParent.gridXEnd = tileXGridDestination
        self.tileSpriteParent.gridYEnd = tileYGridDestination

        self.playTileToBoardGrid(0)  // animation to move to position on board and set Grid, x and y values for tile at tile End Position

        if self.tileSpriteParent.tileState == TileState.PlayedMadeWord  {
            //self.tileSpriteParent.tileState = TileState.PlayedMadeWord   // track which letter made valid word so can run scoring when letter playe
            delay(0.25 * Double(possibleWordTilesHorizontal.count)) {
    //          self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(horizontalString) is a word (horizontal) updateAIWordsAtDropSpot LetterTileSprite") // updates GUI for feedback on horizonal whole word
    //          print ( "\(horizontalString) is a whole word string (horizontal) updateAIWordsAtDropSpot LetterTileSprite" )
                //if self.tileSpriteParent.tileState == TileState.PlayedMadeWord {
                    self.scoreTilesInArr(possibleWordTilesHorizontal, wordString: horizontalString)
            }
        }
        
        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        let replacementPlaceholderTile : MMWTile = MMWTile()
        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
        
        tileSpriteParent.gridHome = self.tileSpriteParent.gridEnd
        tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
        tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd

        self.tileSpriteParent.gridHome?.mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
    }


//        //UP
//        stringToAdd = ""
//        foundLockedTile = false
//        while ( (currentCheckYGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum - 1].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum - 1].tileState == TileState.Played) ) )  {
//            numVerticalAdjacentLetters++
//            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum - 1].tileText)"
//            stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
//            
//            possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum - 1]))
//            if mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum - 1].tileState == TileState.Locked {
//                foundLockedTile = true
//                hasLockedPotentialWord = true
//            }
//            currentCheckYGridNum--
//        }
//        
//        upString = stringToAdd.stringByAppendingString(self.tileText)
//        possibleWordTilesVertical.append(tileSpriteParent)
//        print("\(upString)")
//        
//        //DOWN
//        currentCheckYGridNum = tileYGridDestination
//        stringToAdd = ""
//        while ( (currentCheckYGridNum < 14) && ( (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileState == TileState.Locked) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileState == TileState.Played) ) ) {
//            numVerticalAdjacentLetters++
//            let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileText)"
//            stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
//            possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1]))
//            if mmwGameScene.mmwBoardGrid.grid2DArr[tileXGridDestination][currentCheckYGridNum + 1].tileState == TileState.Locked {
//                foundLockedTile = true
//                hasLockedPotentialWord = true
//            }
//            currentCheckYGridNum++
//        }
//
//        downString = self.tileText.stringByAppendingString(stringToAdd)
//        print("\(downString)")
//        verticalString = upString.stringByAppendingString(stringToAdd)
//        print("\(verticalString)")
//        
//        if (( mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) ) == true) {
//            // updates GUI for feedback on horizonal partial word
//            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(verticalString) is a partial word (vertical) updateAIWordsAtDropSpot LetterTileSprite")
//            print ( "\(verticalString) is a partial word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
//        }
//        
//        if (( mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == true && verticalString.characters.count >= tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
//            // updates GUI for feedback on horizonal partial word
//            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(verticalString) is a word (vertical) updateAIWordsAtDropSpot LetterTileSprite")
//            print ( "\(verticalString) is a whole word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
//
//            scoreTilesInArr(possibleWordTilesVertical, wordString: verticalString)
//            
//            if (tileSpriteParent.tileState != TileState.Locked) {
//                tileSpriteParent.tileState = TileState.Played // if put on valid board location set TileState to played if NOT already locked
//            }
//            
//            // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
//            let replacementPlaceholderTile : MMWTile = MMWTile()
//            replacementPlaceholderTile.gridHome = self.tileSpriteParent.gridHome
//            replacementPlaceholderTile.gridX = self.tileSpriteParent.gridX
//            replacementPlaceholderTile.gridY = self.tileSpriteParent.gridY
//            tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
//            
//            // set value of snap results grid location to the MMWTile if valid location
//            self.tileSpriteParent.gridEnd? = mmwGameScene.mmwBoardGrid
//            self.tileSpriteParent.gridEnd?.grid2DArr[tileXGridDestination][tileYGridDestination] = self.tileSpriteParent
//            self.tileSpriteParent.gridXEnd = tileXGridDestination
//            self.tileSpriteParent.gridYEnd = tileYGridDestination
//            self.isMovable = false // so can't remove tile once placed
//            self.userInteractionEnabled = false
//            //self.lockTile()
//            
//            removeBoardTileHighlights ()
//            
//            self.tileSpriteParent.gridHome?.mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
//        }
//            
//        else {
//            print ( "\(verticalString) is NOT valid whole word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
//        }
//    }
    

    /// func scoreTilesInArr (tileArrToScore: [MMWTile], wordString: String) -> Int
    /// -Parameters: tileArrToScore: [MMWTile], wordString: String
    /// -Returns: Int
    /// counts tile in up/down or left/right tile array and adds poonts for unlocked tiles and complete word and kicks off points awarded animations
    func scoreTilesInArr (var tileArrToScore: [MMWTile], wordString: String) -> Int {
        //tileArrToScore = tileArrToScore.reverse()
        
        print("ScoreTileInArray: ")
        let wordLen = tileArrToScore.count
        var tileNum = 0.0
        
        for tile in tileArrToScore {
            //delay(tileNum*0.5) {  // !! possible issue with timing - doesn't update tiles properly on delay
                self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback2("\(wordString) is a valid word scoreTilesInArr LetterTileSprite")
                print("Tile: \(tile.tileText) \(tile.tileState) \(tile.tileSprite.tileSpriteParent.spritename)")
                if tile.tileState != TileState.Locked {
                    
                    //self.tileSpriteParent.tileState = TileState.Locked
                    
                    let pointsForTile = wordLen * 2
                    // + wordLen points for each unlocked letter to tile owner // SCORE
                    
                    self.adjustPlayerPoints(pointsForTile, player: (self.mmwGameScene.mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1]) )
                    //tile.tileBuilder?.mmwGameSceneViewController?.playerArray[tile.tileOwner.rawValue - 1].playerScore += pointsForTile

                    delay(tileNum*0.5) {
                        tile.tileSprite.showTileScoreTextToGridHome (1, pointsForTile: pointsForTile )
                        tile.mmwGameScene?.explosion(tile.tileSprite.position)
                        tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
                        let tileWordState = tile.tileSprite.testForValidWordsAtDropSpot(tile.gridXEnd, tileSnapResultsYGrid: tile.gridYEnd, isAI: true)
                        if tileWordState.validHorizontalWholeWord && tileWordState.validHorizontalWholeWord {
                            tile.tileSprite.color = UIColor.blackColor()
                        }
                        else {
                            tile.tileSprite.color = UIColor.grayColor()
                        }
                    }
                }
            ++tileNum
        }
  
        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
     
            // + 5 * wordLen points for making a new complete word // SCORE
            //self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerArray[(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerTurn)! - 1].playerScore += (5 * wordLen)
            
            //display points awarded animation (for completing whole word)
            let delay = SKAction.waitForDuration( Double(3) )
            self.runAction( delay ) {
            //run code here after delay secs
                let pointsForCompletingWord = wordLen * 5
                self.showWordScoreTextToGridHome(pointsForCompletingWord) // doesn't actually use value, already set in setting letterScore.2 above
                self.adjustPlayerPoints(pointsForCompletingWord, player: (self.mmwGameScene.mmwGameSceneViewController.playerArray[self.tileSpriteParent.tileOwner.rawValue - 1]) )
                
//                self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
//                self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
//                self.tileSpriteParent.gridHome = self.tileSpriteParent.gridEnd
            }
        }
        
        
        for tile in tileArrToScore {
            if tile.tileState == TileState.Played || tile.tileState == TileState.PlayedMadeWord  {
                
                tile.tileState = TileState.Locked
            }
           

            //tile.tileSprite.tileSpriteParent.tileState
        }
        

//        for playerView in 0..<(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.numPlayers)! {
//            
//            self.adjustPlayerPoints(pointsForTile, player: (self.mmwGameScene.mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1]) )
//
//            self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.playerArray[playerView].playerView.changePlayerScoreDisplay()
//        }
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
    