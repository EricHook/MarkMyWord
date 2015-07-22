//
//  TileView.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/30/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

// letter distribution 

class LetterTileSprite : SKSpriteNode {
    
    let letterLabel = SKLabelNode(fontNamed: FontHUDName)

    var isMovable : Bool = true
    var faceUp : Bool = true
    var hasShadow : Bool = false
    var tileText = ""
    var frontTexture : SKTexture
    var backTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    var largeTexture : SKTexture?
    var largeTextureFilename : String
    var tileShadow : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "TileShadow90x90"), color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSizeMake(50.0, 50.0))
    
    var tileLocation : CGPoint = CGPointMake(0, 0)
    
    var tileSpriteParent : MMWTile! = nil

    var enlarged = false
    var savedPosition = CGPointZero

    enum TileStyle : Int {
        case basic = 0,
        metal,
        pastel,
        wood,
        neon
    }
    
    let TileColors : [UIColor] = [UIColorAppleBlue, UIColorAppleGreen, UIColorApplePurple, UIColorAppleRed, UIColorAppleOrange, UIColorGray]
    
    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
    
    required init(coder aDecoder: NSCoder) {
        fatalError("NSCoding not supported")
    }
    
    init(tileStyle: TileStyle, withChar : String, withColor : SKColor, atPoint: CGPoint) {  //  // add letter, name, colorize?,
        tileText = withChar
        
        switch tileStyle {
            case .basic:
                frontTexture = SKTexture(imageNamed: "Tile3D95x")
                largeTextureFilename = "Tile3D95x"
                
            case .metal:
                frontTexture = SKTexture(imageNamed: "Tile3D95x")
                largeTextureFilename = "Tile3D95x"
                
            default:
                frontTexture = SKTexture(imageNamed: "Tile3D95x")
                largeTextureFilename = "Tile3D95x"
        }
        
        // call designated initializer on super
        super.init(texture: frontTexture, color: UIColorAppleBlue, size: CGSizeMake (47.5, 47.5) ) // frontTexture.size())
        
        // initialize properties
        self.name = "tileSprite"
        self.position = atPoint
        self.anchorPoint = CGPointMake(0.5, 0.5)
        self.color = withColor
        self.colorBlendFactor = 1.0

        backTexture = SKTexture(imageNamed: "TileBackTest90x90")
        tileShadow.alpha = 0.15
        tileShadow.position = CGPointMake(-5, -5)
        tileShadow.zPosition = self.zPosition - 1
        tileShadow.hidden = true
        tileShadow.name = "tileShadowName"
        self.addChild(tileShadow)

        letterLabel.text = withChar
    
        letterLabel.fontSize = 40 // FontHUDSize

        letterLabel.fontColor = UIColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 0.98)
    
        letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
    
        letterLabel.position = CGPointMake(0, -14)
    
        letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
    
        self.addChild(letterLabel)
        centerTileToSquare(self)
        
//            var letterValueLabel = SKLabelNode(fontNamed: FontHUDName)
//            letterValueLabel.text = "💣"
//            letterValueLabel.fontSize = 10
//            letterValueLabel.fontColor = FontHUDWhite
//            letterValueLabel.position = CGPointMake(-22, -20)
//            letterValueLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
//            self.addChild(letterValueLabel)
        
//            foregroundNode!.addChild(tileNode)
//        }

        // set properties defined in super
        userInteractionEnabled = true
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
    
    func moveTileToCenterSquare (cornerPoint : CGPoint) -> CGPoint {
        cornerPoint.x + 23.75     // 22.5?
        cornerPoint.y + 23.75     // 22.5?
        return cornerPoint
    }
    
    func centerTileToSquare(tile : LetterTileSprite) {
        tile.position.x += 23.75  // 22.5?
        tile.position.y += 23.75  // 22.5?
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
            zPosition = 20
            
            let newPosition = CGPointMake(CGRectGetMidX(parent!.frame), CGRectGetMidY(parent!.frame))
            removeAllActions()
            
            let slide = SKAction.moveTo(newPosition, duration:0.3)
            let scaleUp = SKAction.scaleTo(5.0, duration:0.3)
            runAction(SKAction.group([slide, scaleUp]))
        }
        print("  home\(self.tileSpriteParent.gridHome?.gridName) [\(self.tileSpriteParent.gridX)] [\(self.tileSpriteParent.gridY)]")
        print("  end\(self.tileSpriteParent.gridEnd?.gridName) [\(self.tileSpriteParent.gridXEnd)] [\(self.tileSpriteParent.gridYEnd)]")
        print("  state:\(self.tileSpriteParent.tileState) type:\(self.tileSpriteParent.tileType) owner:\(self.tileSpriteParent.tileOwner)")
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
            //            let location = touch.locationInNode(self)
            //            let touchedNode = nodeAtPoint(location)
            zPosition = 99
            tileShadow.zPosition = -10
            
            let liftUp = SKAction.scaleTo(1.2, duration: 0.1)
            
            runAction(liftUp, withKey: "pickup")
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
    
//    func touchesBeganLetterTile (touch: UITouch, withEvent event: UIEvent) {
//        //super.touchesBegan(touches, withEvent: event)
//        /* Called when a touch begins */
////        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
////        runAction(actionSound)
//        
////        for touch in (touches as! Set<UITouch>) {
//            //            if touch.tapCount > 1 {
//            //                flip()
//            //            }
////            if touch.tapCount > 1 {
////                enlarge()
    
////            }
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
//            let location = touch.locationInNode(scene!)
//            let testNodes = nodesAtPoint(location)
//            print("/////////////testNodeCount : \(testNodes.count ) ")
//            for _ in testNodes  {
//                print("testNode ")
//            }
//        
//
//    }
    
//    overide func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?) {
//        <#code#>
//    }
    
    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
        super.touchesMoved(touches, withEvent: event)
        if enlarged { return }
        for touch in (touches as Set<UITouch>) {
            if isMovable {
                let location = touch.locationInNode(scene!)
                let touchedNode = nodeAtPoint(location)
                touchedNode.position = location
                
                //let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)
                //var getSnapGrid : Grid

            }
        }
    }
    
    override func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?){
        if enlarged { return }
        //var location = touch.locationInNode(self)
        
        for touch in (touches as Set<UITouch>) {
            zPosition = 1
            let dropDown = SKAction.scaleTo(1.0, duration: 0.1)
            runAction(dropDown, withKey: "drop")
            tileShadow.zPosition = -1
            tileShadow.hidden = true

            let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)

//             //IF valid drop spot, update tile information
//            // check if array location in grid is an MMWTile and if so .. prints description
//            if ( gameGrid?.gridArr[tileSnapResultsYGrid][tileSnapResultsXGrid].description == "MarkMyWord.MMWTile") {
//                print("<LetterTileSprite> Value of grid square = MarkMyWord.MMWTile ... \( (gameGrid?.gridArr[tileSnapResultsYGrid][tileSnapResultsXGrid])!.description)")
//            }
//            var trialTile : MMWTile = (gameGrid?.gridArr[tileSnapResultsYGrid][tileSnapResultsXGrid])! as! MMWTile
//            // check if array location in grid is an MMWTile and if so .. prints description
//            if ( trialTile.letterString.isEmpty  ) {
//                print("<LetterTileSprite> EMPTY")
//            }
            
            
            // IF VALID DROP LOCATION
            
            let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch) // .getBoardGrid()
            let tileSnapResults = gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y))
            // get snap grid array [[]] positions
            let tileSnapResultsCalculateX = tileSnapResults.GridSquareUpperLeftCornerX
            let tileSnapResultsCalculateY = tileSnapResults.GridSquareUpperLeftCornerY - 15.5 // -15.5 on y touch point adjusts snapping too high to lower square
            let tileSnapResultsXGrid = tileSnapResults.GridSquareX
            let tileSnapResultsYGrid = tileSnapResults.GridSquareY
            let tileAtDropSpot : MMWTile = (gameGrid?.grid2DArr[tileSnapResultsXGrid][tileSnapResultsYGrid])!
            ////////////  TEST FOR TILE UNDER DROP SPOT
            if tileAtDropSpot.tileOwner == TileOwner.Player1 {
                runAction(actionSound)
                self.position = (tileAtDropSpot.gridHome?.sendToGridSquare(self.tileSpriteParent.gridHome!, squareX: self.tileSpriteParent.gridX , squareY: self.tileSpriteParent.gridY ))!
            }
            print("drop location info: state:\(tileAtDropSpot.tileOwner) letter:\(tileAtDropSpot.tileSprite.tileText)")
            
            
            self.tileSpriteParent.gridEnd = gameGrid // set tileSprite parent (MMWTile) grid to grid snapped to

            tileSpriteParent.gridXEnd = tileSnapResults.GridSquareX
            
            tileSpriteParent.gridYEnd = tileSnapResults.GridSquareY

            tileSpriteParent.tileState = TileState.played  // if put on valid board location set TileState to played
            
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
       

            
            
            // IF NOT VALID LOCATION
            
            // RETURN TILE SPRITE TO ORIGINAL POSITION
            let oldPosition = self.tileSpriteParent.gridHome?.sendToGridSquare(self.tileSpriteParent.gridX, squareY: self.tileSpriteParent.gridY)
            let slide = SKAction.moveTo(oldPosition!, duration:0.3)
            let scaleUp = SKAction.scaleTo(1.0, duration:0.3)
            runAction(SKAction.group([slide, scaleUp]))
            
            
            
            
            //(scene as! MMWGameScene).numEmptyTileSlots(gameGrid!)
 
            //tileSpriteParent.updateTileGrid()
            
            //(scene as! MMWGameScene).mmwBoardGrid.fillGridFromArray(tileSpriteParent.gridHome?.gridArr, gridToFill: tileSpriteParent.gridHome!)
            
            //tileSpriteParent.gridHome?.fillArrayFromGrid((tileSpriteParent.gridHome?.gridArr)!, gridOut: tileSpriteParent.gridHome!)

            
            
            //(scene as! MMWGameScene).mmwGameSceneViewController.tileCollection.displayTileArrayValues((gameGrid?.gridPlayer?.playerLetterTiles)!)

//            // prints all values of items in array for grid
//            print("<LetterTileSprite> \(gameGrid?.gridArr)" )
//            
//            print("<LetterTileSprite>The scene at tile end touch : \(scene?.description) and grid location : \(tileSnapResultsXGrid), \(tileSnapResultsYGrid) ")
            
            //self.parent? as MMWTile.parent? as MMWTileBuilder.Type numEmptyTileSlots(gameGrid)
        }
    }
    
    func checkForTouches(touches: NSSet) {
//        let touch = touches.anyObject() as! UITouch
//        
//        let location = touch.locationInNode(self)
//        let nodes = nodesAtPoint(location) as! [SKNode]
//        
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