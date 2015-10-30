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
    var frontTexture : SKTexture? = SKTexture(imageNamed: "Tile3D95x")
    var backTexture : SKTexture? = SKTexture(imageNamed: "TileBackTest90x90")
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
    
    //let TileColors : [UIColor] = [UIColorAppleBlue, UIColorAppleGreen, UIColorApplePurple, UIColorAppleRed, UIColorAppleOrange, UIColorGray, UIColor.blackColor()]
    let TileColors : [UIColor] = [UIColor.blackColor(), UIColorAppleBlue, UIColorAppleRed, UIColorAppleGreen, UIColorApplePurple,  UIColorAppleOrange, UIColorGray ]

    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
    let actionSound2 = SKAction.playSoundFileNamed("1007.WAV", waitForCompletion: true)
    let dealTilesSound = SKAction.playSoundFileNamed("1003.WAV", waitForCompletion: true)
    
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
    
//    func moveTileToCenterSquare (cornerPoint : CGPoint) -> CGPoint {
//        cornerPoint.x += 23.75     // 22.5?
//        cornerPoint.y += 23.75     // 22.5?
//        return cornerPoint
//    }
    
    func centerTileToSquare(tile : LetterTileSprite) {
        tile.position.x += 23.75  // 22.5?
        tile.position.y += 23.75  // 22.5?
    }
    
    func lockTile () {
        let textColor = self.color
        //print("/(textColor)")
        letterLabel.fontColor =  textColor // self.color
        self.color = UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00)
        //self.isMovable = false
        self.userInteractionEnabled = true
        //letterLabel.zPosition = ++self.zPosition
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
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        super.touchesBegan(touches, withEvent: event)
        /* Called when a touch begins */
//        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//        runAction(actionSound)
        
        for touch in (touches as Set<UITouch>) {
            //            if touch.tapCount > 1 {
            //                flip()
            //            }
            if touch.tapCount > 1 {
                enlarge()
            }
            
            if enlarged { return }
            zPosition = 99
            //tileShadow.zPosition = self.zPosition - 1
            
            let liftUp = SKAction.scaleTo(1.2, duration: 0.1)
            runAction(liftUp, withKey: "pickup")
            tileShadow.zPosition = self.zPosition - 100
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

    /// returnTileToGridHome() sends tile to return postition, with scaling effects and removes glow from tile and from all tiles on board
    /// - Returns: nothing
    /// - Parameters:
    ///     - none: nothing
    func returnTileToGridHome () {
        self.zPosition = 25
        let returnPosition = (scene as! MMWGameScene).mmwBoardGrid.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridY)
        let slide = SKAction.moveTo(returnPosition, duration:0.3)
        let scaleUp = SKAction.scaleTo(1.5, duration:0.15)
        let scaleDown = SKAction.scaleTo(1.0, duration:0.15)
        runAction(SKAction.group([slide, scaleUp, scaleDown]))
        self.tileGlow.hidden = true
        removeBoardTileHighlights ()
        runAction(actionSound2)
        
        
    }
    
    /// removeBoardTileHighlights() resets rollover highlights for all tiles on board -- otherwise last word remain highlighted
    /// - Returns: nothing
    /// - Parameters: none
    func removeBoardTileHighlights () {
        for tileArr in (tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.mmwGameScene.mmwBoardGrid.grid2DArr)! {
            for tile in tileArr {
                //tile.tileSprite.color = UIColorAppleRed
                tile.tileSprite.tileGlow.hidden = true
            }
        }
    }
    
    func checkForValidWords (gridXSpot: Int, gridYSpot: Int, gameGrid: Grid) {
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
        
        if gameGrid.grid2DArr[gridXSpot][gridYSpot].tileType != TileType.Letter {

            if (gridXSpot >= 0 && gridXSpot < 15 && gridYSpot >= 0 && gridYSpot < 15) {
                removeBoardTileHighlights ()

                // LEFT
                var currentCheckXGridNum = gridXSpot
                var currentCheckYGridNum = gridYSpot
                var stringToAdd : String = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckXGridNum > 0) && (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Played) ) )  {
                    ////////////////////
                    //                        // breaks away after finds locked tile in direction and subsequently finds non-locked tile
                    //                        // UNLESS the remaining letters beyond locked tiles make additional patial word
                    //                        if (gameGrid?.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState != TileState.Locked && foundLockedTile == true) {
                    //                            break
                    //                        }
                    ////////////////////
                    numHorizontalTiles++
                    tilesToAdd.append((gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot]))
                    
                    let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].letterString)"
                    stringToAdd = letterToAdd.stringByAppendingString(gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].letterString)
                    
                    if gameGrid.grid2DArr[currentCheckXGridNum - 1][gridYSpot].tileState == TileState.Locked {
                        foundLockedTile = true // makes sure locked tile in potential word
                        hasLockedPotentialWord = true
                        hasValidLockedTile = true
                    }
                    currentCheckXGridNum--
                }
                
                leftString = stringToAdd.stringByAppendingString(self.tileText)
                
                for tile in tilesToAdd {
                    horizontalLeftTiles.append(tile)
                }
                
                horizontalLeftTiles = horizontalLeftTiles.reverse()
                horizontalLeftTiles.append(self.tileSpriteParent)
                
//                if hasLockedPotentialWord {
//                    for tile in horizontalLeftTiles {
//                        tile.tileSprite.tileGlow.hidden = false
//                    }
//                }

                //RIGHT
                currentCheckXGridNum = gridXSpot
                currentCheckYGridNum = gridYSpot
                stringToAdd = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckXGridNum < 14) && (gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Played) ) )  {
                    numHorizontalTiles++
                    tilesToAdd.append((gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot]))
                    
                    let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].letterString)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    
                    if gameGrid.grid2DArr[currentCheckXGridNum + 1][gridYSpot].tileState == TileState.Locked {
                        hasLockedPotentialWord = true
                        hasValidLockedTile = true
                    }
                    currentCheckXGridNum++
                }
                
                horizontalTiles.append(self.tileSpriteParent)
                
                for tile in tilesToAdd {
                    horizontalTiles.append(tile)
                }
                
                rightString = self.tileText.stringByAppendingString(stringToAdd)
                horizontalString = leftString.stringByAppendingString(stringToAdd)
                
//                if hasLockedPotentialWord {
//                    for tile in horizontalTiles {
//                        tile.tileSprite.tileGlow.hidden = false
//                    }
//                }
                
                ////////////////////////////
                
                // UP
                currentCheckXGridNum = gridXSpot
                currentCheckYGridNum = gridYSpot
                stringToAdd = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckYGridNum > 0) && (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Locked || (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Played) ) )  {
                    numVerticalTiles++
                    tilesToAdd.append((gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1]))
                    
                    let letterToAdd : String = "\(gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].letterString)"
                    stringToAdd = letterToAdd.stringByAppendingString(gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].letterString)
                    
                    if gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum - 1].tileState == TileState.Locked {
                        foundLockedTile = true // stops check on last last locked tile
                        hasLockedPotentialWord = true
                        hasValidLockedTile = true
                    }
                    currentCheckYGridNum--
                }
                
                upString = stringToAdd.stringByAppendingString(self.tileText)
                
                for tile in tilesToAdd {
                    verticalUpTiles.append(tile)
                }
                
                verticalUpTiles = verticalUpTiles.reverse()
                verticalUpTiles.append(self.tileSpriteParent)
                
//                if hasLockedPotentialWord {
//                    for tile in verticalUpTiles {
//                        tile.tileSprite.tileGlow.hidden = false
//                    }
//                }
                
                //DOWN
                currentCheckXGridNum = gridXSpot
                currentCheckYGridNum = gridYSpot
                stringToAdd = ""
                tilesToAdd = [MMWTile]()
                
                while ( (currentCheckYGridNum < 14) && (gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum][currentCheckYGridNum + 1].tileState == TileState.Played) ) )  {
                    numVerticalTiles++
                    tilesToAdd.append((gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1]))
                    
                    let letterToAdd : String = "\(gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].letterString)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    
                    if currentCheckYGridNum != 0 {
                        if gameGrid.grid2DArr[gridXSpot][currentCheckYGridNum + 1].tileState == TileState.Locked {
                            foundLockedTile = true // stops check on last last locked tile
                            hasLockedPotentialWord = true
                            hasValidLockedTile = true
                        }
                        currentCheckYGridNum++
                    }
                }
                
                verticalTiles.append(self.tileSpriteParent)
                
                for tile in tilesToAdd {
                    verticalTiles.append(tile)
                }
                
                downString = self.tileText.stringByAppendingString(stringToAdd)
                verticalString = upString.stringByAppendingString(stringToAdd)
                
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

    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        super.touchesMoved(touches, withEvent: event)
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
            if (tileSnapTouch.x > 160 && tileSnapTouch.x < 860) && (tileSnapTouch.y > 50 && tileSnapTouch.y < 700) { // checks that drag location within game grid boundaries
                
                let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
                let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y)) // gets grid x, y on tile drag
                
                let tileSnapResultsXGrid = tileSnapResults.GridSquareX
                let tileSnapResultsYGrid = tileSnapResults.GridSquareY
                
                checkForValidWords(tileSnapResultsXGrid, gridYSpot: tileSnapResultsYGrid, gameGrid: gameGrid!)
                
            }
        }
    }
    
//    ////////////  TEST FOR PROXIMITY TO LOCKED TILE
//    func testProximityLockedTile (gridXSpot: Int, gridYSpot: Int) -> Bool {
//        if ( gridXSpot >= 0 && gridXSpot <= 14 && gridYSpot >= 0 && gridYSpot <= 14) { // since + and - 1 -> make sure in array bounds
//            if !hasLockedPotentialWord { // hasLockedPotentialWord tested and set while dragging tile
//                print("No neighboring locked tile! touchesEnded")
//                returnTileToGridHome()
//                return false
//            }
//        }
//        return true
//    }
    
    func testForTileAtDropSpot(gameGrid: Grid, tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int) -> Bool {
        if gameGrid.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid].tileType == TileType.Letter {
            print("Tried drop tile on existing letter touchesEnded gameGrid: \(gameGrid) \(tileSnapResultsXGrid) Y: \(tileSnapResultsYGrid) \(gameGrid.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid])")
            returnTileToGridHome()
            return true
        }
        return false
    }
    
    func testForValidWordsAtDropSpot (gameGrid: Grid, tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int) -> (validHorizontalPartialWord: Bool, validVerticalPartialWord: Bool, validHorizontalWholeWord: Bool, validVerticalWholeWord: Bool) {
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
                while ( (currentCheckXGridNum > 0) && (gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Played) ) )  {
                    let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].letterString)"
                    stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
                    //possibleWordTilesHorizontal.append((gameGrid?.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid])!)
                    //gameGrid?.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileOwner
                    
                    if gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked {
                        foundLockedTile = true // stops check on last last locked tile
                        hasLockedPotentialWord = true
                    }
                    currentCheckXGridNum--
                }
                leftString = stringToAdd.stringByAppendingString(self.tileText)
                possibleWordTilesHorizontal.append(tileSpriteParent)
                
                //RIGHT
                currentCheckXGridNum = tileSnapResultsXGrid
                stringToAdd = ""
                while ( (currentCheckXGridNum < 14) && (gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Played) ) )  {
                    numHorizontalAdjacentLetters++
                    let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].letterString)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    if gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked {
                        foundLockedTile = true
                        hasLockedPotentialWord = true
                    }
                    possibleWordTilesHorizontal.append(gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid])
                    currentCheckXGridNum++
                }
                rightString = self.tileText.stringByAppendingString(stringToAdd)
                horizontalString = leftString.stringByAppendingString(stringToAdd)
                
                if (( gameGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(horizontalString)) == false) {
                    validHorizontalPartialWord = false
                    print ( "Horizontal \(horizontalString)-> INVALID horizontal partial word !!! touchesEnded" )
                }
                else {
                    validHorizontalPartialWord = true
                    print ( "Horizontal \(horizontalString)-> VALID horizontal partial word !!! touchesEnded" )
                    
                }
                
                if (( gameGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(horizontalString)) == false || horizontalString.characters.count < tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize) {
                    validHorizontalWholeWord = false
                    print ( "Horizontal \(horizontalString)-> INVALID horizontal whole word !!! touchesEnded" )
                }
                else {
                    validHorizontalWholeWord = true
                    print ( "Horizontal \(horizontalString)-> VALID horizontal whole word !!! touchesEnded" )
                }
                
                stringToAdd = ""
                foundLockedTile = false
                //UP
                while ( (currentCheckYGridNum > 0) && (gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked || (gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Played) ) )  {
                    numVerticalAdjacentLetters++
                    let letterToAdd : String = "\(gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].letterString)"
                    stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
                    
                    possibleWordTilesVertical.append((gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1]))
                    if gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked {
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
                while ( (currentCheckYGridNum < 14) && ( (gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked) || (gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Played) ) ) {
                    numVerticalAdjacentLetters++
                    let letterToAdd : String = "\(gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].letterString)"
                    stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
                    if gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked {
                        foundLockedTile = true
                        hasLockedPotentialWord = true
                    }
                    currentCheckYGridNum++
                }
            
                downString = self.tileText.stringByAppendingString(stringToAdd)
                verticalString = upString.stringByAppendingString(stringToAdd)
                
                if (( gameGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) ) == false) {
                    // updates GUI for feedback on horizonal partial word
                    validVerticalWholeWord = false
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> Not a valid partial word !!! touchesEnded")
                    print ( "Vertical \(verticalString)-> INVALID vertical partial word !!! touchesEnded" )
                }
                    
                else {
                    validVerticalPartialWord = true
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> Valid partial word !!! touchesEnded")
                    print ( "Vertical \(verticalString)-> VALID vertical partial word !!! touchesEnded" )
                }
                
                if (( gameGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == false || verticalString.characters.count < tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize) {
                    // updates GUI for feedback on horizonal partial word
                    validVerticalWholeWord = false
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> Not a valid whole word !!! touchesEnded")
                    print ( "Vertical \(verticalString)-> INVALID vertical whole word !!! touchesEnded" )
                }
                    
                else {
                    validVerticalWholeWord = true
                    self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> Valid whole word !!! touchesEnded")
                    print ( "Vertical \(verticalString)-> VALID vertical whole word !!! touchesEnded" )
                }
        }
        
        // if horizontal OR vertical invalid partial word > 1 letter (itself) then return tile, invalid play
        
        if (horizontalString.characters.count > 1 && !validHorizontalPartialWord) ||
            (verticalString.characters.count > 1 && !validVerticalPartialWord ) {
                print("NOT !!! VALID PARTIAL WORD Horiz and Vert")
                print("Not valid words BOTH directions")
                
                //if (( gameGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(horizontalString)) == false) {
                    print ( "Horizontal \(horizontalString) -> NOT a valid partial word - 2 points !!! touchesEnded" )
                    runAction(actionSound)
                    runAction(actionSound2)
                    // -2 points for non-partial word // SCORE
                    self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerArray[(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerTurn)! - 1].playerScore -= 2
                
//                    for playerView in 0..<(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.numPlayers)! {
//                        self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.playerArray[playerView].playerView.changePlayerScoreDisplay()
//                    }
                
                //}

                
                returnTileToGridHome()
                
                tileSpriteParent.tileBuilder!.mmwGameSceneViewController!.mmwGameScene.changePlayerTurn()
                
                return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord)
        }
        else {
            print("VALID partial words Horizontal AND Vertical!")
            return (validHorizontalPartialWord, validVerticalPartialWord, validHorizontalWholeWord, validVerticalWholeWord)
        }
    }

    func updateWordsAtDropSpot (gameGrid: Grid, tileSnapResultsXGrid: Int, tileSnapResultsYGrid: Int, touchX: Float, touchY: Float) () {
        
        //let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
        let tileSnapResults = gameGrid.getGridSquare(touchX, locY: touchY)
        let tileSnapResultsCalculateX = tileSnapResults.GridSquareUpperLeftCornerX
        let tileSnapResultsCalculateY = tileSnapResults.GridSquareUpperLeftCornerY - 15.5 // -15.5 on y touch point adjusts snapping too high to lower square
        let tileSnapResultsXGrid = tileSnapResults.GridSquareX
        let tileSnapResultsYGrid = tileSnapResults.GridSquareY
        let tileAtDropSpot : MMWTile = (gameGrid.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid]) // ERROR IF DRAG TOO FAR RIGHT _ INDEX OUT OF RANGE !!! ///////////
        
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

        // LEFT
        var currentCheckXGridNum = tileSnapResultsXGrid
        var currentCheckYGridNum = tileSnapResultsYGrid
        var stringToAdd : String = ""
        while ( (currentCheckXGridNum > 0) && (gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Played) ) )  {
            let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].letterString)"
            stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
            possibleWordTilesHorizontal.append((gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid]))
            //gameGrid?.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileOwner
            if gameGrid.grid2DArr[currentCheckXGridNum - 1][tileSnapResultsYGrid].tileState == TileState.Locked {
                foundLockedTile = true // stops check on last last locked tile
                hasLockedPotentialWord = true
            }
            currentCheckXGridNum--
        }
        leftString = stringToAdd.stringByAppendingString(self.tileText)
        possibleWordTilesHorizontal.append(tileSpriteParent)
        
        //RIGHT
        currentCheckXGridNum = tileSnapResultsXGrid
        stringToAdd = ""
        while ( (currentCheckXGridNum < 14) && (gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked || (gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Played) ) )  {
            numHorizontalAdjacentLetters++
            numHorizontalAdjacentLetters++
            let letterToAdd : String = "\(gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].letterString)"
            stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
            if gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid].tileState == TileState.Locked {
            foundLockedTile = true
            hasLockedPotentialWord = true
        }
        possibleWordTilesHorizontal.append(gameGrid.grid2DArr[currentCheckXGridNum + 1][tileSnapResultsYGrid])
        currentCheckXGridNum++
        }
        rightString = self.tileText.stringByAppendingString(stringToAdd)
        horizontalString = leftString.stringByAppendingString(stringToAdd)
        
        
        if (( gameGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(horizontalString)) == false || horizontalString.characters.count < self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize ) {
            print ( "Horizontal \(horizontalString) -> NOT a valid whole word !!! touchesEnded" )
        }
        
        else {
            print ( "Horizontal \(horizontalString) -> VALID whole word !!! touchesEnded" )
            for tile in possibleWordTilesHorizontal {
                // points for existing locked letter
                //self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerArray[tileSpriteParent.tileOwner.rawValue - 1].playerScore += 1

                if tile.tileState != TileState.Locked {
                    // points for each unlocked letter to tile owner
                    tile.tileBuilder?.mmwGameSceneViewController?.playerArray[tile.tileOwner.rawValue - 1].playerScore += 1
                    tile.tileSprite.letterLabel.fontColor = TileColors[tileSpriteParent.tileOwner.rawValue]
                    tile.tileSprite.color = UIColor.blackColor()
                
                }
                tile.tileState = TileState.Locked
                }
                // 3 points for making a new complete word // SCORE
                self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerArray[(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerTurn)! - 1].playerScore += 3
                
                for playerView in 0..<(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.numPlayers)! {
                    self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.playerArray[playerView].playerView.changePlayerScoreDisplay()
                }
            
//            if gameGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(horizontalString) == false && gameGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) == false {
//                runAction(actionSound)
//                runAction(actionSound2)
//                // -2 points for non-partial word // SCORE
//                self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerArray[(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerTurn)! - 1].playerScore -= 2
//                
//                for playerView in 0..<(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.numPlayers)! {
//                    self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.playerArray[playerView].playerView.changePlayerScoreDisplay()
//     
//                }
        }
            
        
        stringToAdd = ""
        foundLockedTile = false
        //UP
        while ( (currentCheckYGridNum > 0) && (gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked || (gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Played) ) )  {
            numVerticalAdjacentLetters++
            let letterToAdd : String = "\(gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].letterString)"
            stringToAdd = letterToAdd.stringByAppendingString(stringToAdd)
            
            possibleWordTilesVertical.append((gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1]))
            if gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum - 1].tileState == TileState.Locked {
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
        while ( (currentCheckYGridNum < 14) && ( (gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked) || (gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Played) ) ) {
            numVerticalAdjacentLetters++
            let letterToAdd : String = "\(gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].letterString)"
            stringToAdd = stringToAdd.stringByAppendingString(letterToAdd)
            possibleWordTilesVertical.append((gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1]))
            if gameGrid.grid2DArr[tileSnapResultsXGrid][currentCheckYGridNum + 1].tileState == TileState.Locked {
                foundLockedTile = true
                hasLockedPotentialWord = true
            }
        currentCheckYGridNum++
        }
        
        downString = self.tileText.stringByAppendingString(stringToAdd)
        verticalString = upString.stringByAppendingString(stringToAdd)
        
        
        if (( gameGrid.mmwGameScene.mmwGameSceneViewController.checkPartialWordMatch(verticalString) ) == false) {
            // updates GUI for feedback on horizonal partial word
            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> Not a valid partial word !!! touchesEnded")
            print ( "Vertical \(verticalString) -> NOT a valid partial word !!! touchesEnded" )
            
//            runAction(actionSound)
//            runAction(actionSound2)
//            // -2 points for non-partial word // SCORE
//            self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerArray[(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerTurn)! - 1].playerScore -= 2
//            for playerView in 0..<(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.numPlayers)! {
//                self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.playerArray[playerView].playerView.changePlayerScoreDisplay()
//                
//            }
            
        }
            
        if (( gameGrid.mmwGameScene.mmwGameSceneViewController.checkWholeWordMatch(verticalString) ) == false || verticalString.characters.count < tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.minWordSize  ) {
            // updates GUI for feedback on horizonal partial word
            self.tileSpriteParent.gridHome?.mmwGameScene.updatePartialWordFeedback("Vertical \(verticalString)-> Not a valid partial word !!! touchesEnded")
            print ( "Vertical \(verticalString) -> NOT a valid whole word !!! touchesEnded" )
        }
        
        else {
            print ( "Vertical \(verticalString) -> VALID whole word !!! touchesEnded" )

            for tile in possibleWordTilesVertical {
                if tile.tileState != TileState.Locked {
                
                // points for each unlocked letter to tile owner // SCORE
                tile.tileBuilder?.mmwGameSceneViewController?.playerArray[tile.tileOwner.rawValue - 1].playerScore += 1
                print ( "+ 1")
                
                tile.tileSprite.letterLabel.fontColor = TileColors[tileSpriteParent.tileOwner.rawValue]
                tile.tileSprite.color = UIColor.blackColor()
                }
            tile.tileState = TileState.Locked
            }
            // 3 points for making a new complete word // SCORE
            self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerArray[(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController?.playerTurn)! - 1].playerScore += 3
            
            for playerView in 0..<(self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.numPlayers)! {
                self.tileSpriteParent.tileBuilder?.mmwGameSceneViewController!.playerArray[playerView].playerView.changePlayerScoreDisplay()
            }
        }
        
        self.tileSpriteParent.gridEnd = gameGrid // set tileSprite parent (MMWTile) grid to grid snapped to
        tileSpriteParent.gridXEnd = tileSnapResults.GridSquareX
        tileSpriteParent.gridYEnd = tileSnapResults.GridSquareY
        
        if (tileSpriteParent.tileState != TileState.Locked) {
            tileSpriteParent.tileState = TileState.Played // if put on valid board location set TileState to played if NOT already locked
        }
        
        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        let replacementPlaceholderTile : MMWTile = MMWTile()
        replacementPlaceholderTile.gridHome = self.tileSpriteParent.gridHome
        replacementPlaceholderTile.gridX = self.tileSpriteParent.gridX
        replacementPlaceholderTile.gridY = self.tileSpriteParent.gridY
        tileSpriteParent.gridHome?.grid2DArr[tileSpriteParent.gridX][tileSpriteParent.gridY] = replacementPlaceholderTile
        
        // set value of snap results grid location to the MMWTile if valid location
        self.tileSpriteParent.gridHome? = self.tileSpriteParent.gridEnd!
        self.tileSpriteParent.gridHome?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid] = self.tileSpriteParent
        self.tileSpriteParent.gridX = tileSnapResultsXGrid
        self.tileSpriteParent.gridY = tileSnapResultsYGrid
        
        // move tile to snap point
        self.position.x = (CGFloat)(tileSnapResultsCalculateX + 23.75)  //adjusts 22.5 for tile center in middle of tile
        self.position.y = 768 - (CGFloat)(tileSnapResultsCalculateY + 8.25) //38 adjusts for tile center and for board not in exact middle when flipping coords
        
        self.isMovable = false // so can't remove tile once placed
        self.userInteractionEnabled = false
        //self.lockTile()
        
        removeBoardTileHighlights ()
        
        self.tileSpriteParent.gridHome?.mmwGameScene.newTileButtonOff() // placed tile on board so now can only pass turn
        
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
            zPosition = 1
            let dropDown = SKAction.scaleTo(1.0, duration: 0.1)
            runAction(dropDown, withKey: "drop")
            tileShadow.hidden = true

            let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)
            //let tileSnapTouchX = tileSnapTouch.x
            //let tileSnapTouchY = tileSnapTouch.y
  
            // IF NOT VALID DROP LOCATION ON BOARD, RETURN TILE TO PLAYER
            if (tileSnapTouch.x <= 160 || tileSnapTouch.x >= 860) {
                print("Tried drop tile outside game board touchesEnded")
                returnTileToGridHome()
                return
            }
            
            let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
            let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y))
            
            let tileSnapResultsCalculateX = tileSnapResults.GridSquareUpperLeftCornerX
            let tileSnapResultsCalculateY = tileSnapResults.GridSquareUpperLeftCornerY - 15.5 // -15.5 on y touch point adjusts snapping too high to lower square
            let tileSnapResultsXGrid = tileSnapResults.GridSquareX
            let tileSnapResultsYGrid = tileSnapResults.GridSquareY
            let tileAtDropSpot : MMWTile = (gameGrid?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid])! // ERROR IF DRAG TOO FAR RIGHT _ INDEX OUT OF RANGE !!! ///////////
            
//            ////////////  TEST FOR PROXIMITY TO LOCKED TILE
//            if ( tileSnapResultsXGrid >= 0 && tileSnapResultsXGrid <= 14 && tileSnapResultsYGrid >= 0 && tileSnapResultsYGrid <= 14) { // since + and - 1 -> make sure in array bounds
//                if !hasLockedPotentialWord { // hasLockedPotentialWord tested and set while dragging tile
//                    print("No neighboring locked tile! touchesEnded")
//                    returnTileToGridHome()
//                    continue
//                }
//            }
            
            if !hasLockedPotentialWord { // checked for locked tile on drag, return
                returnTileToGridHome()
                continue
            }
            
            //////////// TEST FOR TILE UNDER DROP SPOT
            if testForTileAtDropSpot(gameGrid!, tileSnapResultsXGrid: tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid) {
                break
            }

            else {
//                if tileAtDropSpot.tileOwner == TileOwner.Player1 { // placeholder code to add sound for drop action
//                    runAction(actionSound)
//                    self.position = (tileAtDropSpot.gridHome?.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX , squareY: self.tileSpriteParent.gridY ))!
//                }
                let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch)
                let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y))
                let tileSnapResultsXGrid = tileSnapResults.GridSquareX
                let tileSnapResultsYGrid = tileSnapResults.GridSquareY

                //////////// TEST FOR ADJACENT TILE PARTIAL WORDS
                
                let validWordTestAtDropSpot = testForValidWordsAtDropSpot(gameGrid!, tileSnapResultsXGrid: tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid)
                if validWordTestAtDropSpot.validHorizontalPartialWord && validWordTestAtDropSpot.validVerticalPartialWord {
                    updateWordsAtDropSpot(gameGrid!, tileSnapResultsXGrid: tileSnapResultsXGrid, tileSnapResultsYGrid: tileSnapResultsYGrid, touchX: Float(tileSnapTouch.x), touchY: Float(tileSnapTouch.y)) ()
                }
                //////////// TEST FOR ADJACENT TILE PARTIAL WORDS
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    func adjustPlayerPoints (points: Int, playerNum : Player) {
        
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
    