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

    var tileShadow : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "TileShadow"), color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSize(width: screenSize!.width * 0.048828 , height: screenSize!.height * 0.06510417) )  // 50.0
    var tileGlow : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "Tile3DGlow"), color: UIColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSize(width: screenSize!.width * 0.048828 , height: screenSize!.height * 0.06510417) )  // 50.0
    
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
    
    //    let actionSound      = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    //    let actionSound2     = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    //    let dealTilesSound   = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
    //    let destroyTileSound = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)  // problem with 1023.wav ???
    
    required init(coder aDecoder: NSCoder) {
        fatalError("NSCoding not supported")
    }
    
    init(tileStyle: TileStyle, withChar : String, withColor : SKColor, atPoint: CGPoint) {  //  // add letter, name, colorize?,
        tileText = withChar
  
        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            tileShadow.xScale *= 0.75
            tileShadow.yScale *= 0.75
            tileGlow.xScale *= 0.75
            tileGlow.yScale *= 0.75
        }
 
        let   tileSize = CGSize(width: 47 , height: 47)  // 50.0
        
        super.init(texture: frontTexture, color: UIColorAppleBlue, size: tileSize)
        // call designated initializer on super
        
        // initialize properties
        //self.mmwGameSceneController = mm
        //tileSpriteParent = self.
        self.name = "tileSprite"
        self.position = atPoint
        self.anchorPoint = CGPointMake(0.5, 0.5)
        self.color = withColor
        self.colorBlendFactor = 1.0
        self.zPosition = 2
        
        tileShadow.alpha = 0.35
        tileShadow.position = CGPointMake(-5, -5)
        tileShadow.zPosition = 1
        tileShadow.hidden = true
        tileShadow.name = "tileShadowName"
        self.addChild(tileShadow)
        
        tileGlow.alpha = 1.0
        tileGlow.color = UIColorAppleRed
        tileGlow.position = CGPointMake(0, 0)
        tileGlow.zPosition = self.zPosition + 1
        tileGlow.hidden = true
        tileGlow.name = "glowName"
        tileGlow.hidden = true
        self.addChild(tileGlow)
        
        letterLabel.text = withChar
        letterLabel.fontColor = UIColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 0.98)
        letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        letterLabel.zPosition = 3
        letterLabel.position = CGPointMake(0, -15)
        letterLabel.fontSize = 35 // FontHUDSize
        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            letterLabel.fontSize = 40
            letterLabel.position = CGPointMake(0, -15)
            
        }
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
        tile.position.x += (size.width) * 0.023193359375  // 23.75 22.5?
        tile.position.y += (size.width) * 0.03094479167  // 23.75 22.5?
    }
    
    
    func lockTile () {
        let textColor = self.color
        letterLabel.fontColor =  textColor
        self.color = UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00)
        self.userInteractionEnabled = false
    }
    
    
    func enlarge() {
        if enlarged {
            var slide = SKAction.moveTo(savedPosition, duration:0.0)
            var scaleDown = SKAction.scaleTo(1.0, duration:0.0)
            
            if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
                slide = SKAction.moveTo(savedPosition, duration:0.3)
                scaleDown = SKAction.scaleTo(1.0, duration:0.3)
            }

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
            
            var slide = SKAction.moveTo(newPosition, duration:0.0)
            var scaleUp = SKAction.scaleTo(5.0, duration:0.0)
            if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
                slide = SKAction.moveTo(newPosition, duration:0.3)
                scaleUp = SKAction.scaleTo(5.0, duration:0.3)
            }

            runAction(SKAction.group([slide, scaleUp]))
            
            self.lockTile()
        }
        if debugMode == true { print("  home \(self.tileSpriteParent.gridHome?.gridName) [\(self.tileSpriteParent.gridX)] [\(self.tileSpriteParent.gridY)]") }
        if debugMode == true { print("  end \(self.tileSpriteParent.gridEnd?.gridName) [\(self.tileSpriteParent.gridXEnd)] [\(self.tileSpriteParent.gridYEnd)]") }
        if debugMode == true { print("  state: \(self.tileSpriteParent.tileState) type:\(self.tileSpriteParent.tileType) owner:\(self.tileSpriteParent.tileOwner)") }
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
            
            self.zPosition = 10
            tileShadow.zPosition = -1
            tileShadow.hidden = false
            if debugMode == true { print ("Z pos: \(self.zPosition)  ,  \(tileShadow.zPosition) ") }

            var liftUp = SKAction.scaleTo(1.5, duration: 0.1)
            if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
                liftUp = SKAction.scaleTo(2.0, duration: 0.1)
            }
    
            runAction(liftUp, withKey: "pickup")

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
        
        self.zPosition = 90
        tileShadow.zPosition = -1
        
        if enlarged { return }
        
        hasLockedPotentialWord = false
        
        for touch in (touches as Set<UITouch>) {
            
            if mmwGameScene.secondsLeft > 0 && (self.tileSpriteParent.tileOwner.rawValue == mmwGameSceneViewController.playerTurn) {
                
                let location = touch.locationInNode(scene!)
                let touchedNode = nodeAtPoint(location)
                touchedNode.position = location
                //}
                
                let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)
                
                if debugMode == true { print("touches moved: \(location)  \(mmwGameScene.getSnapGrid(tileSnapTouch)?.gridName) zPos TileSprite: \(self.zPosition), \(tileShadow.zPosition)") }
                
                // IF VALID DROP LOCATION
                if (tileSnapTouch.x > (screenSize!.width) * 0.153320 && tileSnapTouch.x < (screenSize!.width) * 0.84553) && (tileSnapTouch.y > (screenSize!.height) * 0.04947917 && tileSnapTouch.y < (screenSize!.height) * 0.97135416) { // checks that drag location within game grid boundaries

                    let gameGrid = mmwGameScene.getSnapGrid(tileSnapTouch)
                    let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y)) // gets grid x, y on tile drag
                    
                    let tileSnapResultsXGrid = tileSnapResults.GridSquareX
                    let tileSnapResultsYGrid = tileSnapResults.GridSquareY
                    if debugMode == true { print("touch: \(tileSnapTouch.x), \(tileSnapTouch.y) /  \(tileSnapResultsXGrid), \(tileSnapResultsYGrid)  \(mmwGameScene.getSnapGrid(tileSnapTouch)?.gridName)") }
                    checkForValidWordsOnTileDrag(tileSnapResultsXGrid, gridYSpot: tileSnapResultsYGrid, IsAI: false)
                }
                else {
                    LetterTileSprite.removeAllTileHighlights ()
                }
            }
            
            else  {
                self.returnTileToGridHome()
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
        
        //print( (size.width) )
        
        for touch in (touches as Set<UITouch>) {
            zPosition = 3
        var dropDown = SKAction.scaleTo(1.0, duration: 0.1)
            if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
                dropDown = SKAction.scaleTo(1.33, duration: 0.1)
            }
  
            runAction(dropDown, withKey: "drop")
            self.zPosition = 1
            self.letterLabel.zPosition = 2
            tileShadow.zPosition = self.zPosition - 1
            tileShadow.hidden = true
            
            let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)
            
            // IF NOT VALID DROP LOCATION ON BOARD, RETURN TILE TO PLAYER
            
            if (tileSnapTouch.x <= (screenSize!.width) * 0.153320 || tileSnapTouch.x >= (screenSize!.width) * 0.84553) || (tileSnapTouch.y <= (screenSize!.height) * 0.04947917 || tileSnapTouch.y >= (screenSize!.height) * 0.97135416) {
                if debugMode == true { print("Tried drop tile outside game board touchesEnded") }
                returnTileToGridHome()
                return
            }
            
            
            let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
            let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y))
            let tileSnapResultsXGrid = tileSnapResults.GridSquareX
            let tileSnapResultsYGrid = tileSnapResults.GridSquareY
    
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
                
                let validWordTestAtDropSpot = testForValidWordsAtDropSpot(tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid, isAI: false, isAIScoringPass: false)
                
                if validWordTestAtDropSpot.validHorizontalPartialWord && validWordTestAtDropSpot.validVerticalPartialWord {
                    self.tileSpriteParent.tileState = TileState.Played
    
                    updateWordsAtDropSpot(Float(tileSnapTouch.x), touchY: Float(tileSnapTouch.y)) () // tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid,  touchX:
                }
            }
            if debugMode == true { print("tile drop = \(tileSnapResultsXGrid), \(tileSnapResultsYGrid)") }
        }
        
        mmwGameSceneViewController.resetConsequtivePasses()
        mmwGameSceneViewController.lettersPlayedInTurn += 1
        if debugMode == true { print("letters playedInTurn = \(mmwGameSceneViewController.lettersPlayedInTurn) ViewSize: \(screenSize!.width)") }
        
        // Change turns if player has no letter tiles remaining
        let letterTilesRemaining = mmwGameSceneViewController.playerArray[(mmwGameSceneViewController.playerTurn) - 1].playerLetterGrid.numLetterTilesInGrid()
        if letterTilesRemaining <= 0 {
            mmwGameScene.changePlayerTurn()
        }
    }
    
    
    /// returnTileToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - none: nothing
    func returnTileToGridHome () {

//        mmwGameScene.animationsActive(true)
//        let animationTime = 0.0
        
        self.zPosition = 25
        
        var returnPosition = CGPoint(x: 0, y: 0)
        if (self.tileSpriteParent.gridX != -1) {
            returnPosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridY)
        }

        let slide = SKAction.moveTo(returnPosition, duration:0.5)
        var scaleUp = SKAction.scaleTo(1.5, duration:0.25)
        var scaleDown = SKAction.scaleTo(1.00, duration:0.25)

        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            scaleUp = SKAction.scaleTo(2.5, duration:0.25)
            scaleDown = SKAction.scaleTo(1.33, duration:0.25)
        }

        runAction(SKAction.group([slide, scaleUp, scaleDown]))
        
        
        if mmwGameSceneViewController.audioOn == true {
            runAction(SKAction.playSoundFileNamed("badPlay.WAV", waitForCompletion: false))
        }

        self.tileGlow.hidden = true
        delay(0.5) {
            self.zPosition = 1
            self.letterLabel.zPosition = 2
        }
        
        
        LetterTileSprite.removeAllTileHighlights ()
        //runAction(actionSound2)
        
//        delay(animationTime + 5.0){
//            mmwGameScene.animationsActive(false)
//        }
    }
    
    
    /// playTileToBoardGrid (pauseDuration: Double)
    /// - Returns: nothing
    /// - Parameters: pauseDuration: Double
    ///
    func playTileToBoardGrid (pauseDuration: Double) {
        //        let scaleUp : SKAction!
        //        let scaleDown : SKAction!
        
        //mmwGameScene.animationsActive(true)
        var animationTime = 0.0
        
        self.zPosition = 25
        //self.letterLabel.zPosition = 101
        delay(1.00) {
            self.zPosition = 5
            self.letterLabel.zPosition = 6
        }
        
        var boardPosition = CGPoint(x: 0, y: 0)
        if (self.tileSpriteParent.gridX != -1) {
            boardPosition = Grid.sendToGridSquare(mmwGameScene.mmwBoardGrid, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
        }
        //let boardPosition = Grid.sendToGridSquare(mmwGameScene.mmwBoardGrid, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
        
        let slide = SKAction.moveTo(boardPosition, duration: 0.75)
        var scaleUp   = SKAction.scaleTo(1.5, duration:0.4)
        var scaleDown = SKAction.scaleTo(1.00, duration:0.3)
//        animationTime += 0.75

        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            scaleUp = SKAction.scaleTo(2.5, duration:0.4)
            scaleDown = SKAction.scaleTo(1.33, duration:0.3)
        }

        //let pauseSlide = SKAction.sequence([slide])
        let scaleUpDown = SKAction.sequence([scaleUp, scaleDown])  // , self.actionSound2, ])
        self.runAction(SKAction.group([slide, scaleUpDown]))

        // save original tile info for animations
        self.tileSpriteParent.gridTest = self.tileSpriteParent.gridHome
        self.tileSpriteParent.gridXTest = self.tileSpriteParent.gridX
        self.tileSpriteParent.gridYTest = self.tileSpriteParent.gridY
        
        self.userInteractionEnabled = false
        self.tileGlow.hidden = true
        LetterTileSprite.removeAllTileHighlights()
        
//        delay(0.25) {
//            self.zPosition = 5
//        }
    
//        delay(animationTime + 12.0){
//            mmwGameScene.animationsActive(false)
//        }

    }
    
    
    /// showTileScoreTextToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - none: nothing
    func showTileScoreTextToGridHome (delaySec: Int, pointsForTile: Int ) {
        
//        mmwGameScene.animationsActive(true)
//        var animationTime = 0.0
        
  
//        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
//        dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in
//            print("Currently dispatched thread asynchronously 0 playBtnPlay playAILetters")

            var tileScore = SKLabelNode()
            tileScore = self.createLetterScoreText ( CGPointMake(0.0, 0.0) , endLocation: CGPointMake(0.0, 0.0), textColor: gameColors[self.tileSpriteParent.tileOwner.rawValue], displayText: String(pointsForTile) )
            tileScore.zPosition = 100
            tileScore.text = String(pointsForTile)
            tileScore.fontColor =  gameColors[self.tileSpriteParent.tileOwner.rawValue]
            tileScore.fontSize = 45
            tileScore.fontName = FontHUDName

            var homePosition = CGPoint(x: 0, y: 0)
            if (self.tileSpriteParent.gridX != -1) {
                homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridY)
            }
            var endPosition = CGPoint(x: 0, y: 0)
            if (self.tileSpriteParent.gridX != -1) {
                endPosition  = Grid.sendToGridSquare(self.tileSpriteParent.gridEnd!, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
            }
            
            let changeInX : CGFloat = -(endPosition.x - homePosition.x)
            let changeInY : CGFloat = -(endPosition.y - homePosition.y)
            let returnPosition = CGPointMake(changeInX, changeInY)
            let slide     = SKAction.moveTo(returnPosition, duration:1.0)
            let scaleUp   = SKAction.scaleTo(1.5, duration:0.5)
            let scaleDown = SKAction.scaleTo(0.66, duration:0.5)
            let fadeIn    = SKAction.fadeInWithDuration(0.8)
            let fadeOut   = SKAction.fadeOutWithDuration(0.2)
            let animPart1 = SKAction.group([fadeIn, scaleUp])
            let animPart2 = SKAction.group([fadeOut, scaleDown])
            let removeText = SKAction.removeFromParent()
            let tileEffects = SKAction.sequence([animPart1, animPart2, removeText])
            
            tileScore.runAction(SKAction.group([slide, tileEffects]) )
            
            if mmwGameSceneViewController.audioOn == true {
                self.runAction(SKAction.playSoundFileNamed("points.WAV", waitForCompletion: false))
            }
   
        
//        delay(animationTime + 12.0){
//            mmwGameScene.animationsActive(false)
//        }
        
        //self.runAction(self.actionSound2)
        

//            dispatch_async(dispatch_get_main_queue(), {
//                print("hello from playBtnPlay playAILetters thread executed as dispatch")
//            })
//        })
    
        
                    
    }
    
    
    /// showWordScoreTextToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - none: nothing
    func showWordScoreTextToGridHome (number: Int, isHorizontalScore: Bool) {
        
        //mmwGameScene.animationsActive(true)
        //var animationTime = 0.0

        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
        dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in
            if debugMode == true { print("Currently dispatched thread asynchronously 0 playBtnPlay playAILetters") }
            
            if mmwGameSceneViewController.audioOn == true {
                self.runAction(SKAction.playSoundFileNamed("points.WAV", waitForCompletion: false))
            }

            var tileScore = SKLabelNode()
            
            tileScore = self.createLetterScoreText (CGPointMake(0.0, 0.0), endLocation: CGPointMake(0.0, 0.0), textColor: gameColors[self.tileSpriteParent.tileOwner.rawValue], displayText: String("") )
            tileScore.fontColor = gameColors[self.tileSpriteParent.tileOwner.rawValue]
            tileScore.fontSize = 75
            tileScore.fontName = FontHUDName
            tileScore.text = (String(number) )
            tileScore.zPosition = 95

            var endPosition = CGPoint(x: 0, y: 0)
            var homePosition = CGPoint(x: 0, y: 0)
            if (self.tileSpriteParent.gridX != -1) {
                endPosition = Grid.sendToGridSquare(self.tileSpriteParent.gridEnd!, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
                homePosition = endPosition
                
                if isHorizontalScore == true {
                    homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridX)
                }
                else {
                    homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: 0, squareY: 0)
                }
            }

            let changeInX : CGFloat = -(endPosition.x - homePosition.x)
            let changeInY : CGFloat = -(endPosition.y - homePosition.y)
            let returnPosition = CGPointMake(changeInX, changeInY)
            
            let waitTime = SKAction.waitForDuration( Double(0.5) )
            self.runAction( waitTime ) {  //  !! Delay for show word score text set here
                
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
                
                mmwGameScene.explosion(endPosition, color: tileColors[self.tileSpriteParent.tileOwner.rawValue])

                
            }

            dispatch_async(dispatch_get_main_queue(), {
                if debugMode == true { print("hello from showWordScoreTextToGridHome thread executed as dispatch") }
            })
        })
    }
    
    
    /// showWordScoreTextToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - number: the value to send to home grid
    func showNegativeScoreTextToGridHome (number: Int) {
        
        //mmwGameScene.animationsActive(true)
        //var animationTime = 0.0
        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
        dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in
            //print("Currently dispatched thread asynchronously 0 playBtnPlay playAILetters")

                var homePosition = CGPoint(x: 0, y: 0)
                if (self.tileSpriteParent.gridX != -1) {
                    homePosition = Grid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridY)
                }
                var endPosition = CGPoint(x: 0, y: 0)
                if (self.tileSpriteParent.gridX != -1) {
                    endPosition = self.position
                }

                let negPlayScore = SKLabelNode() // = createLetterScoreText (self.position, endLocation: homePosition, textColor: gameColors[self.tileSpriteParent.tileOwner.rawValue], displayText: String(number) )

                negPlayScore.fontColor = gameColors[self.tileSpriteParent.tileOwner.rawValue]
                negPlayScore.text = "\(number)"
                negPlayScore.fontSize = 65
                negPlayScore.fontName = FontHUDName
                negPlayScore.zPosition = 95
                negPlayScore.removeFromParent()
                mmwGameScene.addChild(negPlayScore)
                negPlayScore.position = self.position
                let returnPosition = homePosition

                let waitTime = SKAction.waitForDuration( Double(0.0) )
                self.runAction( waitTime ) {  //  !! Delay for show word score text set here
                let slide = SKAction.moveTo(returnPosition, duration:1.0)
                let scaleUp = SKAction.scaleTo(2.0, duration:0.75)
                let scaleDown = SKAction.scaleTo(0.5, duration:0.25)
                let fadeIn = SKAction.fadeInWithDuration(0.75)
                let fadeOut = SKAction.fadeOutWithDuration(0.75)
                let remove = SKAction.removeFromParent()

                let animPart1 = SKAction.group([slide])
                let animScale = SKAction.sequence([scaleUp, scaleDown, remove])
                
                negPlayScore.runAction(SKAction.group([fadeIn, animPart1, fadeOut, animScale]) )

                mmwGameScene.explosion(endPosition, color: tileColors[self.tileSpriteParent.tileOwner.rawValue])
            }

            dispatch_async(dispatch_get_main_queue(), {
                if debugMode == true { print("hello from showNegativeScoreTextToGridHome thread executed as dispatch") }
            })
        })
    }
    
    /// removeBoardTileHighlights() resets rollover highlights for all tiles on board -- otherwise last word remain highlighted
    /// - Returns: nothing
    /// - Parameters: none
    class func removeBoardTileHighlights () {
        for tileArr in (mmwGameScene.mmwBoardGrid.grid2DArr) {
            for tile in tileArr {
                tile.tileSprite.tileGlow.hidden = true
            }
        }
    }
    
    
    /// removeAllTileHighlights() resets rollover highlights for all tiles in al grid -- otherwise last word remain highlighted
    /// - Returns: nothing
    /// - Parameters: none
    class func removeAllTileHighlights () {
        let gameGrids = [ mmwGameScene.mmwBoardGrid, mmwGameScene.mmwPlayer1Grid, mmwGameScene.mmwPlayer2Grid, mmwGameScene.mmwPlayer3Grid, mmwGameScene.mmwPlayer4Grid ]
        
        for grid in gameGrids {
            for tileArr in grid.grid2DArr {
                for tile in tileArr {
                    tile.tileSprite.tileGlow.hidden = true
                }
            }
        }
        for tile in (tileCollection?.mmwDiscardedTileArray)! {
            tile.tileSprite.tileGlow.hidden = true
        }
        for tile in (tileCollection?.mmwTileArray)! {
            tile.tileSprite.tileGlow.hidden = true
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
        var verticalUpTiles : [MMWTile] = [MMWTile]()
        var horizontalTiles : [MMWTile] = [MMWTile]()
        var verticalTiles : [MMWTile] = [MMWTile]()
        var foundLockedTile = false
        var numHorizontalTiles = 0
        var numVerticalTiles = 0
        
        var hasValidLockedTile = false
        
        LetterTileSprite.removeBoardTileHighlights()
        if debugMode == true { print("checkForValidWords \(gridXSpot),\(gridYSpot), \(IsAI)") }
        
        if mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][gridYSpot].tileType != TileType.Letter {
            if debugMode == true { print("checkForValidWords") }
            if (gridXSpot >= 0 && gridXSpot < 15 && gridYSpot >= 0 && gridYSpot < 15) {
                LetterTileSprite.removeAllTileHighlights ()
                // LEFT
                var currentCheckXGridNum = gridXSpot
                var currentCheckYGridNum = gridYSpot
                var stringToAdd : String = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckXGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Played) ) )  {
                    
                    numHorizontalTiles++
                    tilesToAdd.append((mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot]))
                    
                    let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileText)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    
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
                if debugMode == true { print(horizontalString + " checkForValidWords") }
                
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
                
                for char in stringToAdd.characters {
                    upString = (String(char) + upString)
                }
                upString = upString.stringByAppendingString(self.tileText) // add current letter string to upward letters

                for tile in tilesToAdd {
                    verticalUpTiles.append(tile)
                }
                verticalUpTiles = verticalUpTiles.reverse()
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
                    
                    if mmwGameScene.mmwBoardGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked {
                        foundLockedTile = true // stops check on last last locked tile
                        hasLockedPotentialWord = true
                        hasValidLockedTile = true
                    }
                    currentCheckYGridNum++
                }

                for tile in tilesToAdd {
                    verticalTiles.append(tile)
                }
                
                downString = self.tileText.stringByAppendingString(stringToAdd)
                verticalString = upString.stringByAppendingString(stringToAdd)
                
                if debugMode == true { print(verticalString + " checkForValidWords") }
                
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
            if debugMode == true { print("Tried drop tile on existing letter touchesEnded gameGrid: \(gameGrid) \(tileSnapResultsXGrid) Y: \(tileSnapResultsYGrid) \(gameGrid.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid])") }
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
    class func testLetterAtDropSpot (tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, letterToTest: String) -> (validHorizontalPartialWord: Bool, validVerticalPartialWord: Bool, validHorizontalWholeWord: Bool, validVerticalWholeWord: Bool, horizontalOnly: Bool, verticalOnly: Bool) {
        var leftString : String = ""
        var rightString : String = ""
        var upString : String = ""
        var downString : String = ""
        var horizontalString : String = ""
        var verticalString : String = ""
        var horizontalOnly = true
        var verticalOnly = true
        
        //        var possibleWordTilesHorizontal : [MMWTile] = []
        //        var possibleWordTilesVertical : [MMWTile] = []
        
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
                    //hasLockedPotentialWord = true
                }
                currentCheckXGridNum--
            }
            
            leftString = stringToAdd.stringByAppendingString(letterToTest) //possibleWordTilesHorizontal = possibleWordTilesHorizontal.reverse()  ///
            //possibleWordTilesHorizontal.append(tileSpriteParent)
            
            //RIGHT
            currentCheckXGridNum = tileSnapResultsXGrid
            stringToAdd = ""
            while ( (currentCheckXGridNum < 14) && (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.PlayedMadeWord) ) )  {
                numHorizontalAdjacentLetters++
                let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileText)"
                stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                if mmwGameScene.mmwBoardGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked {
                    foundLockedTile = true
                    //hasLockedPotentialWord = true
                }
                currentCheckXGridNum++
            }
            
            rightString = stringToAdd
            horizontalString = leftString.stringByAppendingString(rightString)
            

            if ( mmwGameSceneViewController.checkPartialWordMatch(horizontalString)) == true {
                validHorizontalPartialWord = true
                if ( mmwGameSceneViewController.checkWholeWordMatch(horizontalString)) == true {
                    validHorizontalWholeWord = true
                }
            }
            
            stringToAdd = ""
            foundLockedTile = false
            
            currentCheckXGridNum = tileSnapResultsXGrid
            currentCheckYGridNum = tileSnapResultsYGrid
            
            //UP
            while ( (currentCheckYGridNum > 0) && (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Played)  || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.PlayedMadeWord) ) )  {
                numVerticalAdjacentLetters++
                let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileText)"
                stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
                
                //possibleWordTilesVertical.append((mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1]))
                if mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked {
                    foundLockedTile = true
                    //hasLockedPotentialWord = true
                }
                currentCheckYGridNum--
            }
            
            upString = stringToAdd.stringByAppendingString(letterToTest)
            //possibleWordTilesVertical.append(tileSpriteParent)
            
            //DOWN
            currentCheckYGridNum = tileSnapResultsYGrid
            stringToAdd = ""
            while ( (currentCheckYGridNum < 14) && ( (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Played) || (mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.PlayedMadeWord) ) ) {
                numVerticalAdjacentLetters++
                let letterToAdd : String = "\(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileText)"
                stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                if mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked {
                    foundLockedTile = true
                    //hasLockedPotentialWord = true
                }
                
                //possibleWordTilesVertical.append(mmwGameScene.mmwBoardGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1])
                
                currentCheckYGridNum++
            }
            
            downString = stringToAdd
            verticalString = upString.stringByAppendingString(downString)
   
            if mmwGameSceneViewController.checkPartialWordMatch(verticalString) == true {
                validVerticalPartialWord = true
                if mmwGameSceneViewController.checkWholeWordMatch(verticalString) == true {
                    validVerticalWholeWord = true
                }
            }
        }
        
        //        if !isAI {  // BAD DROP BY PLAYER ! placeholder skip allow AI place invalid letter in valid spot
        //            // if horizontal OR vertical invalid partial word > 1 letter (itself) then return tile, invalid play
        //            if (horizontalString.characters.count > 1 && !validHorizontalPartialWord) ||
        //                (verticalString.characters.count > 1 && !validVerticalPartialWord ) {
        //                    //runAction(actionSound)
        //
        //                    // -5 points for non-partial word // SCORE
        //                    self.adjustPlayerPoints(-5, player: (mmwGameSceneViewController.playerArray[self.tileSpriteParent.tileOwner.rawValue - 1]) )
        //                    mmwGameScene.updatePartialWordFeedback("-5 points for invalid word attempt")
        //                    mmwGameScene.updatePartialWordFeedback2("")
        //                    print("# -5 POINTS for bad word attempt: \(-5) : \(self.tileText) ")
        //
        //                    showNegativeScoreTextToGridHome(-5)
        //                    self.tileSpriteParent.tileOwner = TileOwner.None
        //                    tileCollection!.mmwDiscardedTileArray.append(self.tileSpriteParent)
        //                    self.hidden = true
        //                    self.tileSpriteParent.gridHome?.grid2DArr[self.tileSpriteParent.gridX][self.tileSpriteParent.gridY] = MMWTile()
        //                    mmwGameScene.changePlayerTurn()
        //            }
        //            else {
        //                playTileToBoard(tileSnapResultsXGrid, yGrid: tileSnapResultsYGrid)
        //                if validHorizontalWholeWord || validVerticalWholeWord {
        //
        //                }
        //            }
        //        }
        if horizontalString.characters.count == 1 { verticalOnly   = true }
        if verticalString.characters.count   == 1 { horizontalOnly = true }
        return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord, horizontalOnly, verticalOnly)
    }
    
    
    /// func testForValidWordsAtDropSpot (gridXSpot: Int, gridYSpot: Int, IsAI: Bool)
    /// used on touchesMoved to determine highlighted tile hints and possible partial word
    /// -Parameters: tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, isAI: Bool
    /// -Returns: validHorizontalPartialWord: Bool, validVerticalPartialWord: Bool, validHorizontalWholeWord: Bool, validVerticalWholeWord: Bool
    /// AI settings used as Ai iterates through tiles and would be too many false results
    func testForValidWordsAtDropSpot (tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, isAI: Bool, isAIScoringPass: Bool) -> (validHorizontalPartialWord: Bool, validVerticalPartialWord: Bool, validHorizontalWholeWord: Bool, validVerticalWholeWord: Bool, horizontalOnly: Bool, verticalOnly: Bool) {
        var leftString : String = ""
        var rightString : String = ""
        var upString : String = ""
        var downString : String = ""
        var horizontalString : String = ""
        var verticalString : String = ""
        var horizontalOnly = false
        var verticalOnly   = false
        
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
            
            leftString = stringToAdd.stringByAppendingString(self.tileText) //possibleWordTilesHorizontal = possibleWordTilesHorizontal.reverse()  ///
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

            if (( mmwGameSceneViewController.checkPartialWordMatch(horizontalString)) == false) {
                validHorizontalPartialWord = false
                validHorizontalWholeWord = false
            }
            else {
                validHorizontalPartialWord = true
                if (( mmwGameSceneViewController.checkWholeWordMatch(horizontalString)) == true) {
                    validHorizontalWholeWord = true
                }
            }

            stringToAdd = ""
            foundLockedTile = false

            currentCheckXGridNum = tileSnapResultsXGrid
            currentCheckYGridNum = tileSnapResultsYGrid
            
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
            
            downString = stringToAdd
            verticalString = upString.stringByAppendingString(downString)

            if mmwGameSceneViewController.checkPartialWordMatch(verticalString) == false {
                validVerticalPartialWord = false
                validVerticalWholeWord = false
            }
                
            else {
                validVerticalPartialWord = true
                if mmwGameSceneViewController.checkWholeWordMatch(verticalString) == true {
                        validVerticalWholeWord = true
                }
            }
        }
        
        if !isAI {  // BAD DROP BY PLAYER ! placeholder skip allow AI place invalid letter in valid spot
            // if horizontal OR vertical invalid partial word > 1 letter (itself) then return tile, invalid play
            if (horizontalString.characters.count > 1 && !validHorizontalPartialWord) ||
                (verticalString.characters.count > 1 && !validVerticalPartialWord ) {
                    //runAction(actionSound)
                    
                    // -5 points for non-partial word // SCORE
                    self.adjustPlayerPoints(-2, player: (mmwGameSceneViewController.playerArray[self.tileSpriteParent.tileOwner.rawValue - 1]) )
                    mmwGameScene.updatePartialWordFeedback("-5 points for invalid word attempt")
                    mmwGameScene.updatePartialWordFeedback2("")
                    if debugMode == true { print("# -2 POINTS for bad word attempt (\" \(horizontalString) \", \" \(verticalString) \" ): \(-2) : \(self.tileText) ") }

                    showNegativeScoreTextToGridHome(-2)
                    self.tileSpriteParent.tileOwner = TileOwner.None
                    tileCollection!.mmwDiscardedTileArray.append(self.tileSpriteParent)
                    self.hidden = true
                    self.tileSpriteParent.gridHome?.grid2DArr[self.tileSpriteParent.gridX][self.tileSpriteParent.gridY] = MMWTile()
                    delay (1.0){
                        mmwGameScene.changePlayerTurn()
                    }
                    
            }
            else {  // valid drop of tile by human player
                playTileToBoard(tileSnapResultsXGrid, yGrid: tileSnapResultsYGrid)
                if mmwGameSceneViewController.playerTurn == 0 {
                    mmwGameSceneViewController.numLettersPlayed++
                }

                if validHorizontalWholeWord || validVerticalWholeWord {
                    
                }
            }
        }
        if horizontalString.characters.count == 1 { verticalOnly   = true }
        if verticalString.characters.count   == 1 { horizontalOnly = true }
        return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord, horizontalOnly, verticalOnly)
    }
    
 
    /// func updateWordsAtDropSpot (tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, touchX: Float, touchY: Float) ()
    /// used on touchesEnded to determine played tile
    /// -Parameters: tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, touchX: Float, touchY: Float
    /// -Returns: none
    /// used to test when a whole or partial word play exists at drop spot & updates tile data on successful play
    func updateWordsAtDropSpot (touchX: Float, touchY: Float) () {  // tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int,
        
        let tileSnapResults = mmwGameScene.mmwBoardGrid.getGridSquare(touchX, locY: touchY)
        let tileSnapResultsCalculateX = tileSnapResults.GridSquareUpperLeftCornerX
        let tileSnapResultsCalculateY = tileSnapResults.GridSquareUpperLeftCornerY - Double((screenSize!.height) * 0.02018229167) // -15.5 on y touch point adjusts snapping too high to lower square
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
        
        var tileMadeCompleteWord = false
        
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

        if ( ( mmwGameSceneViewController.checkWholeWordMatch(horizontalString) ) == false || horizontalString.characters.count < mmwGameSceneViewController.minWordSize  ) {
            mmwGameScene.updatePartialWordFeedback2("Horizontal: \(horizontalString) is a partial word")
        }
            
        else {
            mmwGameScene.updatePartialWordFeedback2("Horizontal: \(horizontalString) is a valid word")
            for tile in possibleWordTilesHorizontal{
                if debugMode == true { print("\(tile.tileText)  :  \(tile.tileState)") }
            }
            tileSpriteParent.tileState = TileState.PlayedMadeWord
            tileMadeCompleteWord = true
            if self.tileSpriteParent.playedMadeWord != PlayedMadeWord.Both {
                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Both
                }
                else {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Horizontal
                }
            }
            
//            if tileSpriteParent.tileState == TileState.PlayedMadeWord {
//                scoreTilesInArr(possibleWordTilesHorizontal, tileArrToScoreVertical: possibleWordTilesVertical, wordStringHorizontal: horizontalString, wordStringVertical: verticalString)
//                for tile in possibleWordTilesHorizontal{
//                     print("\(tile.tileText)  :  \(tile.tileState)")
//                }
//            }
        }
        
        if (( mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == false || verticalString.characters.count < mmwGameSceneViewController.minWordSize  ) {
            // updates GUI for feedback on horizonal partial word
            mmwGameScene.updatePartialWordFeedback ("Vertical: \(verticalString) is a partial word")
        }
            
        else {
            mmwGameScene.updatePartialWordFeedback ("Vertical: \(verticalString) is a valid word")
            tileSpriteParent.tileState = TileState.PlayedMadeWord
            tileMadeCompleteWord = true
            if self.tileSpriteParent.playedMadeWord != PlayedMadeWord.Both {
                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Both
                }
                else {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Vertical
                }
            }
        }
        
        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
            scoreTilesInArr(possibleWordTilesHorizontal, tileArrToScoreVertical: possibleWordTilesVertical, wordStringHorizontal: horizontalString, wordStringVertical: verticalString)
        }
        
        //////////////////
        if mmwGameSceneViewController.playerTurn == 1 {
            if tileMadeCompleteWord == true { mmwGameSceneViewController.numWordsMade++ }
            mmwGameSceneViewController.numLettersPlayed++
            if debugMode == true { print("played: letters: \(mmwGameSceneViewController.numLettersPlayed),words: \(mmwGameSceneViewController.numWordsMade)") }
        }
        
        playTileToBoard (tileSnapResults.GridSquareX, yGrid: tileSnapResults.GridSquareY)
    
        
//        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
//        let replacementPlaceholderTile : MMWTile = MMWTile()
//        replacementPlaceholderTile.gridHome = self.tileSpriteParent.gridHome
//        replacementPlaceholderTile.gridX = self.tileSpriteParent.gridX
//        replacementPlaceholderTile.gridY = self.tileSpriteParent.gridY
//        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
//        
//        // set value of snap results grid location to the MMWTile if valid location
//        self.tileSpriteParent.gridEnd? = self.tileSpriteParent.gridEnd!
//        self.tileSpriteParent.gridEnd?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid] = self.tileSpriteParent
//        
//        self.tileSpriteParent.gridXTest = self.tileSpriteParent.gridX
//        self.tileSpriteParent.gridYTest = self.tileSpriteParent.gridY
//        
//        self.tileSpriteParent.gridXEnd = tileSnapResultsXGrid
//        self.tileSpriteParent.gridYEnd = tileSnapResultsYGrid
//
//        // move tile to snap point
//        self.position.x = CGFloat(tileSnapResultsCalculateX)  + ( CGFloat((screenSize!.width)) * CGFloat(0.023193359375))  //adjusts 23.75 for tile center in middle of tile
//        self.position.y = CGFloat((screenSize!.height)) - ( CGFloat(tileSnapResultsCalculateY) + ( CGFloat((screenSize!.height)) * 0.0107421875) ) //38 adjusts for tile center and for board not in exact middle when flipping coords
//        
//        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
//            tileSpriteParent.tileState = TileState.Locked // if put on valid board location set TileState to played if NOT already locked
//        }
//        
//        self.isMovable = false // so can't remove tile once placed
//        self.userInteractionEnabled = false
//        
//        removeBoardTileHighlights ()
//        
//        mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
    }
    

    func playTileToBoard (xGrid: Int, yGrid: Int) {
        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        let replacementPlaceholderTile = tileCollection?.mmwTileArrayPlaceholder.removeFirst()
        replacementPlaceholderTile!.gridHome = self.tileSpriteParent.gridHome
        replacementPlaceholderTile!.gridX = self.tileSpriteParent.gridX
        replacementPlaceholderTile!.gridY = self.tileSpriteParent.gridY
        replacementPlaceholderTile!.gridXEnd = self.tileSpriteParent.gridX
        replacementPlaceholderTile!.gridYEnd = self.tileSpriteParent.gridY
        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile!
        
        if mmwGameScene.mmwBoardGrid.grid2DArr[xGrid][yGrid].tileType == TileType.Placeholder {
            tileCollection?.mmwTileArrayPlaceholder.append(mmwGameScene.mmwBoardGrid.grid2DArr[xGrid][yGrid])
        }

        // set value of snap results grid location to the MMWTile if valid location

//        self.tileSpriteParent.gridX = xGrid
//        self.tileSpriteParent.gridY = yGrid
//        self.tileSpriteParent.gridXTest = xGrid
//        self.tileSpriteParent.gridYTest = xGrid

        tileSpriteParent.gridXEnd = xGrid
        tileSpriteParent.gridYEnd = yGrid
        
        tileSpriteParent.gridEnd = mmwGameScene.mmwBoardGrid
        
//        tileSpriteParent.gridHome = mmwGameScene.mmwBoardGrid
        
        mmwGameScene.mmwBoardGrid.grid2DArr[xGrid][yGrid] = self.tileSpriteParent

        // move tile to snap point
        let boardPosition = Grid.sendToGridSquare(mmwGameScene.mmwBoardGrid, squareX: self.tileSpriteParent.gridXEnd, squareY: self.tileSpriteParent.gridYEnd)
        self.position.x = boardPosition.x
        self.position.y = boardPosition.y
        self.isMovable = false // so can't remove tile once placed
        self.userInteractionEnabled = false
        
        LetterTileSprite.removeAllTileHighlights ()
        
        mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn

        //mmwGameSceneViewController.numLettersPlayed++
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

        if ( ( mmwGameSceneViewController.checkPartialWordMatch (horizontalString) ) == true ) { // && self.tileSpriteParent.tileState != TileState.PlayedMadeWord {
            mmwGameScene.updatePartialWordFeedback2 ("Horizontal: \(horizontalString) is a partial word")
            //print ( "\(horizontalString) is a partial word string (horizontal) updateAIWordsAtDropSpot LetterTileSprite" )
            self.tileSpriteParent.tileState = TileState.Played
        }
            
        else {
            mmwGameScene.updatePartialWordFeedback2 ("Horizontal: \(horizontalString) is not valid")
            //print ( "\(horizontalString) is NOT a valid word striyng (horizontal) updateAIWordsAtDropSpot LetterTileSprite" )
        }
        
        if ( ( mmwGameSceneViewController.checkWholeWordMatch(horizontalString) ) == true && horizontalString.characters.count >= mmwGameSceneViewController.minWordSize  ) {
            mmwGameScene.updatePartialWordFeedback2 ("Horizontal: \(horizontalString) is a valid word")
            //print ( "\(horizontalString) is a valid whole word string (horizontalString) updateAIWordsAtDropSpot LetterTileSprite" )
 
            
            self.tileSpriteParent.tileState = TileState.PlayedMadeWord
            
            if self.tileSpriteParent.playedMadeWord != PlayedMadeWord.Both {
                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Both
                }
                else {
                    self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Horizontal
                }
            }

            //self.tileSpriteParent.playedMadeWord = PlayedMadeWord.Horizontal
 
            for tile in possibleWordTilesHorizontal{
                if debugMode == true { print("\(tile.tileText)  :  \(tile.tileState)") }
            }
        }
        
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
        
        if ( ( mmwGameSceneViewController.checkPartialWordMatch(verticalString) ) == true ) { // && self.tileSpriteParent.tileState != TileState.PlayedMadeWord {
            mmwGameScene.updatePartialWordFeedback ("Vertical: \(verticalString) is a partial word")
            //print ( "\(verticalString) is a partial word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
            if self.tileSpriteParent.tileState != TileState.PlayedMadeWord {
                self.tileSpriteParent.tileState = TileState.Played
            }
        }
        
        else {
            mmwGameScene.updatePartialWordFeedback ("Vertical: \(verticalString) is not valid")
            //print ( "\(verticalString) is NOT a valid word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
        }
        
        if ( ( mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == true && verticalString.characters.count >= mmwGameSceneViewController.minWordSize  ) {
            mmwGameScene.updatePartialWordFeedback ("Vertical: \(verticalString) is a valid word")
            //print ( "\(verticalString) is a valid whole word string (vertical) updateAIWordsAtDropSpot LetterTileSprite" )
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
                if debugMode == true { print("\(tile.tileText)  :  \(tile.tileState)") }
            }
        }
 
        // set value of snap results grid location to the MMWTile if valid location
        self.tileSpriteParent.gridEnd? = mmwGameScene.mmwBoardGrid
        self.tileSpriteParent.gridEnd?.grid2DArr[tileXGridDestination][tileYGridDestination] = self.tileSpriteParent
        self.tileSpriteParent.gridXEnd = tileXGridDestination
        self.tileSpriteParent.gridYEnd = tileYGridDestination

        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        let replacementPlaceholderTile : MMWTile = MMWTile()
        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
        
        
        
        self.playTileToBoardGrid(0)  // animation to move to position on board and set Grid, x and y values for tile at tile End Position

        if self.tileSpriteParent.tileState == TileState.PlayedMadeWord  {
            delay(0.25 * Double(possibleWordTilesHorizontal.count)) {
                self.scoreTilesInArr(possibleWordTilesHorizontal, tileArrToScoreVertical: possibleWordTilesVertical, wordStringHorizontal: horizontalString, wordStringVertical: verticalString)
            }
        }
        
//        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
//        let replacementPlaceholderTile : MMWTile = MMWTile()
//        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
        
        mmwGameScene.newTilesButtonOff() // placed tile on board so now can only pass turn
    }
    
    
//    /// func scoreTilesInArr (tileArrToScore: [MMWTile], wordString: String) -> Int
//    /// -Parameters: tileArrToScore: [MMWTile], wordString: String
//    /// -Returns: Int
//    /// counts tile in up/down or left/right tile array and adds points for unlocked tiles and complete word and kicks off points awarded animations
//    func scoreTilesInArr (tileArrToScoreHorizontal: [MMWTile], tileArrToScoreVertical: [MMWTile], wordStringHorizontal: String, wordStringVertical: String) -> Int {
//        
//        //mmwGameScene.animationsActive(true)
//        var animationTime = 0.0
//        
//        var numPasses = 0
//        var tileArrToScore = tileArrToScoreHorizontal
//        var tileNum = 0
//        var delayTime = 0.0
//        var scoredMadeWordBoth = false
//        
//        while numPasses <= 1 {
//            print("### ScoreTileInArray: \(numPasses)")
//            if numPasses == 1 {
//                tileArrToScore = tileArrToScoreVertical
//            }
//            let wordLen = tileArrToScore.count
//            
//            tileNum = 0
//            // + 2*wordLen points for locking a tile // SCORE  // display points awarded animation (for locked tile)
//            for tile in tileArrToScore {
//                var pointsForTile = 0
//                tile.tileSprite.zPosition = 1
//                tile.tileSprite.letterLabel.zPosition = 2
//
//                print("Tile: \(tile.tileText) \(tile.tileState) \(tile.tileSprite.tileSpriteParent.spritename)")
//                if (tile.tileState == TileState.Played) { // || tile.tileState == TileState.PlayedMadeWord) {
//                    
//                    var doubleCountLockedTile = false
//                    
//                    if numPasses == 1 {
//                        for tileH in tileArrToScoreHorizontal {
//                            if (tileH.gridX == tile.gridX) && (tileH.gridY == tile.gridY) && (tileH.tileState == TileState.Played) && tile.tileState == TileState.Played {
//                                doubleCountLockedTile = true
//                            }
//                        }
//                    }
//                    
//                    // don't double lock the same tile in V and H
//                    if doubleCountLockedTile == false {
//                        
//                        if wordLen >= mmwGameSceneViewController.minWordSize {
//                            let pointsForTile = wordLen * 2     // + wordLen points for each unlocked letter to tile owner // SCORE
//
//                            let letterState = tile.tileSprite.testForValidWordsAtDropSpot(tile.gridX, tileSnapResultsYGrid: tile.gridY, isAI: false, isAIScoringPass: false)
//                            
//                            if (letterState.validHorizontalWholeWord && letterState.horizontalOnly) || (letterState.validVerticalWholeWord && letterState.verticalOnly) {
//                                if tile.tileState == TileState.Played {
//                                    tile.tileState = TileState.Locked
//                                    delayTime += 0.5
//                                    self.adjustPlayerPoints(pointsForTile, player: (mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1]) )
//                                    print("#POINTS for locking tile: \(pointsForTile) : \(self.tileText) :  Player : \(mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1])) ")
//                                    tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                                    tile.tileSprite.color = UIColor.darkGrayColor()
//                                    
//                            }
//                            
//                            
//                            
//                            
//                            if (letterState.validHorizontalWholeWord && letterState.validHorizontalPartialWord) || (letterState.validVerticalWholeWord && letterState.validVerticalPartialWord) {
//                                if tile.tileState == TileState.Played {
//                                    //tile.tileState = TileState.Locked
//                                    //delayTime += 0.5
//                                    //self.adjustPlayerPoints(pointsForTile, player: (mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1]) )
//                                    //print("#POINTS for locking tile: \(pointsForTile) : \(self.tileText) :  Player : \(mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1])) ")
//                                    tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                                    tile.tileSprite.color = UIColor.darkGrayColor()
//                                }
//                            }
//                                
//                            if tile.tileState == TileState.PlayedMadeWord  {
//                                    delayTime += 0.5
//                                }
//                                animationTime += delayTime
//                                
//                                delay(delayTime) {
//                                    tile.tileSprite.showTileScoreTextToGridHome (1, pointsForTile: pointsForTile )
//                                    
//                                    tile.gridHome = tile.gridEnd
//                                    tile.gridX = tile.gridXEnd
//                                    tile.gridY = tile.gridYEnd
//                                    
//                                    mmwGameScene.explosion(tile.tileSprite.position, color: tileColors[self.tileSpriteParent.tileOwner.rawValue])
//                                    
//                                    delay(0.5){
//                                        
//                                    }
//
//                            
//
//                            
//                                
//                                                                        //                                    tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                                        //
//                                        //                                    tile.tileSprite.color = UIColor.blackColor()
//                                        
//                                        if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical || self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal {
//                                            tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                                            tile.tileSprite.color = UIColor.darkGrayColor()
//                                        }
//                                        else if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Both {
//                                            tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                                            tile.tileSprite.color = UIColor.blackColor()
//                                        }
//                                    }
//                                }
//                            }
//                            else {
//                                tile.tileSprite.color = UIColor.darkGrayColor()
//                            }
//                        }
//                    }
//                }
//                pointsForTile = 0
//                ++tileNum
//            }
//            numPasses++
//        }
//        
//        // + 5 * wordLen points for making a new complete word // SCORE  // display points awarded animation (for completing whole word)
//        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
//
//            var pointsForCompletingWord = 0
//     
//            if tileSpriteParent.tileState == TileState.PlayedMadeWord && (tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical){ // || tileSpriteParent.playedMadeWord == PlayedMadeWord.Both) {
//                
//                self.tileSpriteParent.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                self.color = UIColor.grayColor()
//                
//                //pointsForCompletingWord = tileArrToScoreVertical.count * 3
//                //print("+POINTS for completing word VERTICAL : \(pointsForCompletingWord) : \(self.tileText) ")
//                //tileSpriteParent.tileState = TileState.Locked
//            }
//            
//            else if tileSpriteParent.tileState == TileState.PlayedMadeWord && (tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal){ // || tileSpriteParent.playedMadeWord == PlayedMadeWord.Both) {
//                
//                self.tileSpriteParent.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                self.color = UIColor.grayColor()
//                
//                //pointsForCompletingWord = tileArrToScoreHorizontal.count * 3
//                //print("+POINTS for completing word HORIZONTAL : \(pointsForCompletingWord) : \(self.tileText) ")
//                ///tileSpriteParent.tileState = TileState.Locked
//            }
//                
//            // only score both on one direction rather than double score on horizontal AND vertical passes
//            else if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Both { // && scoredMadeWordBoth == false {
//                //self.color = UIColor.blackColor()
//                
//                pointsForCompletingWord = (tileArrToScoreVertical.count * 3) + (tileArrToScoreHorizontal.count * 3)
//                
//                
//                print("+POINTS for completing word BOTH : \(pointsForCompletingWord) : \(self.tileText) ")
//                tileSpriteParent.tileState = TileState.Locked
//                scoredMadeWordBoth = true
//            }
//
//            let waitTime = SKAction.waitForDuration( delayTime + 0.8 )
//            animationTime += 0.8
//            
//            self.runAction( waitTime ) {
//                
//                //run code here after delay secs
//                self.showWordScoreTextToGridHome(pointsForCompletingWord, isHorizontalScore: true)      // doesn't actually use value, already set in setting letterScore.2 above
//                
//                self.tileSpriteParent.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                self.color = UIColor.blackColor()
//                
////                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal || self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
////                    self.color = UIColor.darkGrayColor()
////                }
////                else {
////                    self.color = UIColor.blackColor()
////                }
//
//                self.adjustPlayerPoints(pointsForCompletingWord, player: (mmwGameSceneViewController.playerArray[self.tileSpriteParent.tileOwner.rawValue - 1]) )
//                print("# + POINTS for completing word: \(pointsForCompletingWord) : \(self.tileText) ")
//                
//                self.tileSpriteParent.gridHome = self.tileSpriteParent.gridEnd
//                self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
//                self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
//            }
//        }
//        
////        delay(10.0) {  // animationTime +=
////            mmwGameScene.animationsActive(false)
////        }
//        
//        return 0
//    }
    
    
    
    
//    /// func scoreTilesInArr (tileArrToScore: [MMWTile], wordString: String) -> Int
//    /// -Parameters: tileArrToScore: [MMWTile], wordString: String
//    /// -Returns: Int
//    /// counts tile in up/down or left/right tile array and adds points for unlocked tiles and complete word and kicks off points awarded animations
//    func scoreTilesInArr (tileArrToScoreHorizontal: [MMWTile], tileArrToScoreVertical: [MMWTile], wordStringHorizontal: String, wordStringVertical: String) -> Int {
//        
//        //mmwGameScene.animationsActive(true)
//        var animationTime = 0.0
//        
//        var numPasses = 0
//        var tileArrToScore = tileArrToScoreHorizontal
//        var tileNum = 0
//        var delayTime = 0.0
//        var scoredMadeWordBoth = false
//        
//        while numPasses <= 1 {
//            print("### ScoreTileInArray: \(numPasses)")
//            if numPasses == 1 {
//                tileArrToScore = tileArrToScoreVertical
//            }
//            let wordLen = tileArrToScore.count
//            
//            tileNum = 0
//            // + 2*wordLen points for locking a tile // SCORE  // display points awarded animation (for locked tile)
//            for tile in tileArrToScore {
//                var pointsForTile = 0
//                tile.tileSprite.zPosition = 1
//                tile.tileSprite.letterLabel.zPosition = 2
//                
//                print("Tile: \(tile.tileText) \(tile.tileState) \(tile.tileSprite.tileSpriteParent.spritename)")
//                if (tile.tileState == TileState.Played) { // || tile.tileState == TileState.PlayedMadeWord) {
//                    
//                    var doubleCountLockedTile = false
//                    
//                    if numPasses == 1 {
//                        for tileH in tileArrToScoreHorizontal {
//                            if (tileH.gridX == tile.gridX) && (tileH.gridY == tile.gridY) && (tileH.tileState == TileState.Played) && tile.tileState == TileState.Played {
//                                doubleCountLockedTile = true
//                            }
//                        }
//                    }
//                    
//                    // don't double lock the same tile in V and H
//                    if doubleCountLockedTile == false {
//                        if wordLen >= mmwGameSceneViewController.minWordSize {
//                            let pointsForTile = wordLen * 2     // + wordLen points for each unlocked letter to tile owner // SCORE
//                            self.adjustPlayerPoints(pointsForTile, player: (mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1]) )
//                            
//                            print("#POINTS for locking tile: \(pointsForTile) : \(self.tileText) :  Player : \(mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1])) ")
//                            
//                            var letterState = tile.tileSprite.testForValidWordsAtDropSpot(tile.gridX, tileSnapResultsYGrid: tile.gridY, isAI: false, isAIScoringPass: false)
//                            //
//                            //                            if tile.tileState == TileState.Played && letterState.validHorizontalWholeWord && letterState.validVerticalWholeWord {
//                            var isLockedLetter = false
//                            if (letterState.validHorizontalWholeWord && letterState.horizontalOnly) || (letterState.validVerticalWholeWord && letterState.verticalOnly)
//                            
//                            if tile.tileState == TileState.Played {
//                                
//                                tile.tileState = TileState.Locked
//                                delayTime += 0.5
//                            }
//                            if tile.tileState == TileState.PlayedMadeWord  {
//                                delayTime += 0.5
//                            }
//                            animationTime += delayTime
//                            
//                            delay(delayTime) {
//                                tile.tileSprite.showTileScoreTextToGridHome (1, pointsForTile: pointsForTile )
//                                
//                                tile.gridHome = tile.gridEnd
//                                tile.gridX = tile.gridXEnd
//                                tile.gridY = tile.gridYEnd
//                                
//                                mmwGameScene.explosion(tile.tileSprite.position, color: tileColors[self.tileSpriteParent.tileOwner.rawValue])
//                                
//                                delay(0.5){
//                                    //                                    tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                                    //
//                                    //                                    tile.tileSprite.color = UIColor.blackColor()
//                                    
//                                    if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical || self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal {
//                                        tile.tileSprite.color = UIColor.darkGrayColor()
//                                    }
//                                    else if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Both {
//                                        tile.tileSprite.color = UIColor.blackColor()
//                                    }
//                                }
//                                
//                                
//                                //                                                            if tileWordState.validHorizontalWholeWord || tileWordState.validVerticalWholeWord {
//                                //                                                                tile.tileSprite.color = UIColor.darkGrayColor()
//                                //                                                            }
//                                //                                                            else {
//                                //                                                                tile.tileSprite.color = UIColor.blackColor()
//                                //                                                            }
//                            }
//                        }
//                    }
//                }
//                pointsForTile = 0
//                ++tileNum
//            }
//            numPasses++
//        }
//        
//        // + 5 * wordLen points for making a new complete word // SCORE  // display points awarded animation (for completing whole word)
//        if tileSpriteParent.tileState == TileState.PlayedMadeWord {
//            
//            //let tileWordState = self.testForValidWordsAtDropSpot(self.tileSpriteParent.gridXEnd, tileSnapResultsYGrid: self.tileSpriteParent.gridYEnd, isAI: true)
//            var pointsForCompletingWord = 0
//            
//            //            if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal {
//            //                if tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
//            //
//            //                }
//            //
//            //             if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
//            
//            
//            //            // only score both on one direction rather than double score on horizontal AND vertical passes
//            //            if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Both && scoredMadeWordBoth == false {
//            //                //self.color = UIColor.blackColor()
//            //                pointsForCompletingWord = ( (tileArrToScoreVertical.count * 5) + (tileArrToScoreHorizontal.count * 5) )
//            //                print("+POINTS for completing word BOTH : \(pointsForCompletingWord) : \(self.tileText) ")
//            //                tileSpriteParent.tileState = TileState.Locked
//            //                scoredMadeWordBoth = true
//            //            }
//            
//            if tileSpriteParent.tileState == TileState.PlayedMadeWord && (tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical){ // || tileSpriteParent.playedMadeWord == PlayedMadeWord.Both) {
//                //self.color = UIColor.grayColor()
//                pointsForCompletingWord = tileArrToScoreVertical.count * 3
//                print("+POINTS for completing word VERTICAL : \(pointsForCompletingWord) : \(self.tileText) ")
//                tileSpriteParent.tileState = TileState.Locked
//            }
//                
//            else if tileSpriteParent.tileState == TileState.PlayedMadeWord && (tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal){ // || tileSpriteParent.playedMadeWord == PlayedMadeWord.Both) {
//                //self.color = UIColor.grayColor()
//                pointsForCompletingWord = tileArrToScoreHorizontal.count * 3
//                print("+POINTS for completing word HORIZONTAL : \(pointsForCompletingWord) : \(self.tileText) ")
//                tileSpriteParent.tileState = TileState.Locked
//            }
//                
//                // only score both on one direction rather than double score on horizontal AND vertical passes
//            else if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Both { // && scoredMadeWordBoth == false {
//                //self.color = UIColor.blackColor()
//                
//                pointsForCompletingWord = (tileArrToScoreVertical.count * 3) + (tileArrToScoreHorizontal.count * 3)
//                
//                
//                print("+POINTS for completing word BOTH : \(pointsForCompletingWord) : \(self.tileText) ")
//                tileSpriteParent.tileState = TileState.Locked
//                scoredMadeWordBoth = true
//            }
//            
//            let waitTime = SKAction.waitForDuration( delayTime + 0.8 )
//            animationTime += 0.8
//            
//            self.runAction( waitTime ) {
//                
//                //run code here after delay secs
//                self.showWordScoreTextToGridHome(pointsForCompletingWord, isHorizontalScore: true)      // doesn't actually use value, already set in setting letterScore.2 above
//                self.tileSpriteParent.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                
//                self.color = UIColor.blackColor()
//                
//                //                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal || self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
//                //                    self.color = UIColor.darkGrayColor()
//                //                }
//                //                else {
//                //                    self.color = UIColor.blackColor()
//                //                }
//                
//                self.adjustPlayerPoints(pointsForCompletingWord, player: (mmwGameSceneViewController.playerArray[self.tileSpriteParent.tileOwner.rawValue - 1]) )
//                print("# + POINTS for completing word: \(pointsForCompletingWord) : \(self.tileText) ")
//                
//                self.tileSpriteParent.gridHome = self.tileSpriteParent.gridEnd
//                self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
//                self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
//            }
//        }
//        
//        //        delay(10.0) {  // animationTime +=
//        //            mmwGameScene.animationsActive(false)
//        //        }
//        
//        return 0
//    }

    
    
    
    /// func scoreTilesInArr (tileArrToScore: [MMWTile], wordString: String) -> Int
    /// -Parameters: tileArrToScore: [MMWTile], wordString: String
    /// -Returns: Int
    /// counts tile in up/down or left/right tile array and adds points for unlocked tiles and complete word and kicks off points awarded animations
    func scoreTilesInArr (tileArrToScoreHorizontal: [MMWTile], tileArrToScoreVertical: [MMWTile], wordStringHorizontal: String, wordStringVertical: String) -> Int {

        var animationTime = 0.0
        
        var numPasses = 0
        var tileArrToScore = tileArrToScoreHorizontal
        var tileNum = 0
        var delayTime = 0.0
        var scoredMadeWordBoth = false
        
        while numPasses <= 1 {
            if debugMode == true { print("### ScoreTileInArray: \(numPasses)") }
            if numPasses == 1 {
                tileArrToScore = tileArrToScoreVertical
            }
            let wordLen = tileArrToScore.count
            
            tileNum = 0
            // + 2*wordLen points for locking a tile // SCORE  // display points awarded animation (for locked tile)
            for tile in tileArrToScore {
                let tileWordState = tile.tileSprite.testForValidWordsAtDropSpot(self.tileSpriteParent.gridXEnd, tileSnapResultsYGrid: self.tileSpriteParent.gridYEnd, isAI: true, isAIScoringPass: false)
                
                var pointsForTile = 0
                tile.tileSprite.zPosition = 1
                tile.tileSprite.letterLabel.zPosition = 2
                
                if debugMode == true { print("Tile: \(tile.tileText) \(tile.tileState) \(tile.tileSprite.tileSpriteParent.spritename)") }
                if (tile.tileState == TileState.Played) { // || tile.tileState == TileState.PlayedMadeWord) {
                    
                    var doubleCountLockedTile = false
                    
                    if numPasses == 1 {
                        for tileH in tileArrToScoreHorizontal {
                            if (tileH.gridX == tile.gridX) && (tileH.gridY == tile.gridY) && (tileH.tileState == TileState.Played) && tile.tileState == TileState.Played {
                                doubleCountLockedTile = true
                            }
                        }
                    }
                    
                    // don't double lock the same tile in V and H
                    if doubleCountLockedTile == false {
                        if wordLen >= mmwGameSceneViewController.minWordSize {

                            let pointsForTile = wordLen * 1     // + wordLen points for each unlocked letter to tile owner // SCORE
                            self.adjustPlayerPoints(pointsForTile, player: (mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1]) )
                            if debugMode == true { print("#POINTS for locking tile: \(pointsForTile) : \(self.tileText) :  Player : \(mmwGameSceneViewController.playerArray[tile.tileOwner.rawValue - 1])) ") }
                            if tile.tileState == TileState.Played  {
                                tile.tileState = TileState.Locked
                                delayTime += 0.5
                            }
                            if tile.tileState == TileState.PlayedMadeWord  {
                                delayTime += 0.5
                            }
                            animationTime += delayTime
                            
                            delay(delayTime) {
                                tile.tileSprite.showTileScoreTextToGridHome (1, pointsForTile: pointsForTile )
                                
                                tile.gridHome = tile.gridEnd
                                tile.gridX = tile.gridXEnd
                                tile.gridY = tile.gridYEnd
                                
                                tile.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
                                
                                mmwGameScene.explosion(tile.tileSprite.position, color: tileColors[self.tileSpriteParent.tileOwner.rawValue])
                                
                                tile.tileSprite.color = UIColor.blackColor()
                                self.zPosition = 1
                                self.letterLabel.zPosition = 2
                                
                                //if (tileWordState.validHorizontalWholeWord && tileWordState.horizontalOnly == false)  || (tileWordState.validVerticalWholeWord && tileWordState.verticalOnly == false){
                                    //tile.tileSprite.color = UIColor.darkGrayColor()
                                //}
//                                if (tileWordState.validHorizontalWholeWord && tileWordState.horizontalOnly == true) || (tileWordState.validVerticalWholeWord && tileWordState.verticalOnly == true)  {
//                                    tile.tileSprite.color = UIColor.blackColor()
//                                }
                                
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
            var pointsForCompletingWord = 0

                let tileWordState = self.testForValidWordsAtDropSpot(self.tileSpriteParent.gridXEnd, tileSnapResultsYGrid: self.tileSpriteParent.gridYEnd, isAI: true, isAIScoringPass: false)
            
            
            //            if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal {
            //                if tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
            //
            //                }
            //
            //             if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
            
            
            //            // only score both on one direction rather than double score on horizontal AND vertical passes
            //            if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Both && scoredMadeWordBoth == false {
            //                //self.color = UIColor.blackColor()
            //                pointsForCompletingWord = ( (tileArrToScoreVertical.count * 5) + (tileArrToScoreHorizontal.count * 5) )
            //                print("+POINTS for completing word BOTH : \(pointsForCompletingWord) : \(self.tileText) ")
            //                tileSpriteParent.tileState = TileState.Locked
            //                scoredMadeWordBoth = true
            //            }
            
            if tileSpriteParent.tileState == TileState.PlayedMadeWord && (tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical ) {
                //self.color = UIColor.grayColor()
                pointsForCompletingWord = tileArrToScoreVertical.count * 2
                if debugMode == true { print("+POINTS for completing word VERTICAL : \(pointsForCompletingWord) : \(self.tileText) ") }
                tileSpriteParent.tileState = TileState.Locked
            }
                
            else if tileSpriteParent.tileState == TileState.PlayedMadeWord && (tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal ) {
                //self.color = UIColor.grayColor()
                pointsForCompletingWord = tileArrToScoreHorizontal.count * 2
                if debugMode == true { print("+POINTS for completing word HORIZONTAL : \(pointsForCompletingWord) : \(self.tileText) ") }
                tileSpriteParent.tileState = TileState.Locked
            }
            
            // only score both on one direction rather than double score on horizontal AND vertical passes
            else if tileSpriteParent.tileState == TileState.PlayedMadeWord && tileSpriteParent.playedMadeWord == PlayedMadeWord.Both && scoredMadeWordBoth == false {
                //self.color = UIColor.blackColor()
                pointsForCompletingWord = ( (tileArrToScoreVertical.count * 2) + (tileArrToScoreHorizontal.count * 2) )
                if debugMode == true { print("+POINTS for completing word BOTH : \(pointsForCompletingWord) : \(self.tileText) ") }
                tileSpriteParent.tileState = TileState.Locked
                scoredMadeWordBoth = true
            }
            

            let waitTime = SKAction.waitForDuration( delayTime + 0.8 )
            animationTime += 0.8
            
            self.runAction( waitTime ) {
                
                //run code here after delay secs
                self.showWordScoreTextToGridHome(pointsForCompletingWord, isHorizontalScore: true)      // doesn't actually use value, already set in setting letterScore.2 above
                
                self.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
                self.color = UIColor.blackColor()
                self.zPosition = 1
                self.letterLabel.zPosition = 2
                
//                if (tileWordState.validHorizontalWholeWord && tileWordState.horizontalOnly == false)  || (tileWordState.validVerticalWholeWord && tileWordState.verticalOnly == false){
//                    self.color = UIColor.darkGrayColor()
//                }
//                else if (tileWordState.validHorizontalWholeWord && tileWordState.horizontalOnly == true) || (tileWordState.validVerticalWholeWord && tileWordState.verticalOnly == true)  {
//                    self.color = UIColor.blackColor()
//                }

                
//                self.tileSpriteParent.tileSprite.letterLabel.fontColor = tileColors[self.tileSpriteParent.tileOwner.rawValue]
//                self.color = UIColor.blackColor()
                
                //                if self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Horizontal || self.tileSpriteParent.playedMadeWord == PlayedMadeWord.Vertical {
                //                    self.color = UIColor.darkGrayColor()
                //                }
                //                else {
                //                    self.color = UIColor.blackColor()
                //                }
                
                self.adjustPlayerPoints(pointsForCompletingWord, player: (mmwGameSceneViewController.playerArray[self.tileSpriteParent.tileOwner.rawValue - 1]) )
                if debugMode == true { print("# + POINTS for completing word: \(pointsForCompletingWord) : \(self.tileText) ") }
                
                self.tileSpriteParent.gridHome = self.tileSpriteParent.gridEnd
                self.tileSpriteParent.gridX = self.tileSpriteParent.gridXEnd
                self.tileSpriteParent.gridY = self.tileSpriteParent.gridYEnd
            }
        }
        
        //        delay(10.0) {  // animationTime +=
        //            mmwGameScene.animationsActive(false)
        //        }
        
        return 0
    }

    
    
    
    
    

    
    func adjustPlayerPoints (points: Int, player : Player) {
        player.playerScore += points
        player.playerView.playerScoreLabel.text = String(player.playerScore)
    }
    
    
    func checkForTouches(touches: NSSet) {
        //        let touch = touches.anyObject() as! UITouch
        //        let location = touch.locationInNode(self)
        //        let nodes = nodesAtPoint(location) as! [SKNode]
        //        for node in nodes {
        //                   
        //        }
    }
}
