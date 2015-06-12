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
    
    var isMovable : Bool = true
    var faceUp : Bool = true
    var hasShadow : Bool = false
    var tileText = ""
    
    var frontTexture : SKTexture
    var backTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    var largeTexture : SKTexture?
    var largeTextureFilename : String
    //let shadow : SKTexture = SKTexture(imageNamed: "TileShadow90x90")
    var tileShadow : SKSpriteNode = SKSpriteNode(texture: SKTexture(imageNamed: "TileShadow90x90"), color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSizeMake(50.0, 50.0))
    
    var tileLocation : CGPoint = CGPointMake(0, 0)

    enum TileStyle : Int {
        case basic = 0,
        metal,
        pastel,
        wood,
        neon
    }
    
    required init(coder aDecoder: NSCoder) {
        fatalError("NSCoding not supported")
    }
    
    init(tileStyle: TileStyle, withChar : String, withColor : SKColor, atPoint: CGPoint) {  //  // add letter, name, colorize?,
        
        //tileShadow = SKSpriteNode(texture: shadow, color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSizeMake(50.0, 50.0))
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
            self.name = withChar
            self.position = atPoint
            self.anchorPoint = CGPointMake(0.5, 0.5)
            self.color = withColor
            self.colorBlendFactor = 1.0

            backTexture = SKTexture(imageNamed: "TileBackTest90x90")
        
            //tileShadow =  SKSpriteNode(texture: shadow, color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 0.00), size: CGSizeMake(50.0, 50.0))
            tileShadow.alpha = 0.15
            tileShadow.position = CGPointMake(-5, -5)
            tileShadow.zPosition = self.zPosition - 1
            tileShadow.hidden = true
            tileShadow.name = "tileShadowName"
            self.addChild(tileShadow)
        
            let letterLabel = SKLabelNode(fontNamed: FontHUDName)
            let letterLabelShadow = SKLabelNode(fontNamed: FontHUDName)
            let letterLabelHighlight = SKLabelNode(fontNamed: FontHUDName)
        
            letterLabel.text = withChar
            letterLabelShadow.text = withChar
            letterLabelHighlight.text = withChar
        
            letterLabel.fontSize = 40 // FontHUDSize
            letterLabelShadow.fontSize = 40 // FontHUDSize
            letterLabelHighlight.fontSize = 40 // FontHUDSize
        
            //var colorDarkGreen : UIColor = UIColor(red: 0.1, green: 1.0, blue: 0.2, alpha: 0.80)
            letterLabel.fontColor = UIColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 0.98)
            letterLabelShadow.fontColor = UIColor(red: 0, green: 0, blue: 0, alpha: 0.5) // black
            letterLabelHighlight.fontColor = UIColor(red: 0, green: 0, blue: 0, alpha: 0.5) // black
        
            letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
            letterLabelShadow.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
            letterLabelHighlight.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        
            letterLabel.position = CGPointMake(0, -14)
            letterLabelShadow.position = CGPointMake(-0.5, -13.5)
            letterLabelHighlight.position = CGPointMake(0.5, -14.5)
        
            letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
            letterLabelShadow.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
            letterLabelHighlight.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
        
            self.addChild(letterLabelShadow)
            self.addChild(letterLabelHighlight)
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
        cornerPoint.x + 22.5
        cornerPoint.y + 22.5
        return cornerPoint
    }
    
    func centerTileToSquare(tile : LetterTileSprite) {
        tile.position.x += 22.5
        tile.position.y += 22.5
    }
    
    var enlarged = false
    var savedPosition = CGPointZero
    
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
    
    func touchesBeganLetterTile (touch: UITouch, withEvent event: UIEvent) {
        //super.touchesBegan(touches, withEvent: event)
        /* Called when a touch begins */
//        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//        runAction(actionSound)
        
//        for touch in (touches as! Set<UITouch>) {
            //            if touch.tapCount > 1 {
            //                flip()
            //            }
//            if touch.tapCount > 1 {
//                enlarge()
//            }
        
            if enlarged { return }
            //            let location = touch.locationInNode(self)
            //            let touchedNode = nodeAtPoint(location)
            zPosition = 99
            tileShadow.zPosition = -10
            
            let liftUp = SKAction.scaleTo(1.2, duration: 0.1)
            
            runAction(liftUp, withKey: "pickup")
            
            tileShadow.hidden = false
            
            let location = touch.locationInNode(scene!)
            let testNodes = nodesAtPoint(location)
            print("testNodeCount : \(testNodes.count ) ")
            for _ in testNodes  {
                print("testNode ")
            }
//            checkForTouches(touches)
            
            
//        }
    }
    
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
                //println("touchedNode description: \(touchedNode.description)")
            }
        }
    }
    
    override func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?) {
        if enlarged { return }
        //var location = touch.locationInNode(self)
        
        for touch in (touches as Set<UITouch>) {
            //location = touch.locationInNode(self)
                        //let location = touch.locationInNode(self)
            //            let touchedNode = nodeA                                                                                                                                          Point(location)
            zPosition = 1
            let dropDown = SKAction.scaleTo(1.0, duration: 0.1)
            runAction(dropDown, withKey: "drop")
            tileShadow.zPosition = -1
            tileShadow.hidden = true
            
            let tileSnapTouch = (touch as UITouch).locationInView(scene!.view)
            
            //var getSnapGrid : Grid
            let gameGrid = (scene as! MMWGameScene).getSnapGrid(tileSnapTouch) // .getBoardGrid()
            
            
            //var tileSnap : CGPoint = CGPointMake(165.0, 28.0)
            
            

            
            // get snap location for point touch ended // -15.5 on y touch point adjusts for snapping too high to lower square
            //var tileSnapCalculate = gameGrid.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y))
            // get snap x, y values
            let tileSnapCalculateX : CGFloat = CGFloat(gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y)).GridSquareLowerLeftCornerX)
            let tileSnapCalculateY : CGFloat = CGFloat(gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y - 15.5)).GridSquareLowerLeftCornerY)
            
            // get snap grid array [[]] positions // -15.5 on y touch point adjusts for snapping too high to lower square
            let tileSnapXGrid : Int = (gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y)).GridSquareX)
            let tileSnapYGrid : Int = (gameGrid!.getGridSquare(Float(tileSnapTouch.x), locY: Float(tileSnapTouch.y - 15.5)).GridSquareY)
            
            // move tile to snap point
            self.position.x = tileSnapCalculateX + 22.5  //adjusts 22.5 for tile center in middle of tile
            self.position.y = 768 - (tileSnapCalculateY + 38) //38 adjusts for tile center and for board not in exact middle when flipping coords
            
            
            print("The scene at tile end touch : \(scene?.description) and grid location : \(tileSnapXGrid), \(tileSnapYGrid) ")
            
//            var location = (touch as! UITouch).locationInView(MMWScene.view)  //(SKScene)   (self.view)
//            location.y = 768 - location.y
//            //////////////let location = touch.locationInNode(self)
//            let nodes = nodesAtPoint(location) as! [SKNode]
//            
//            var gridSquareX = MMWBoardGrid.getGridSquare(MMWBoardGrid, locX: Float(location.x), locY: Float(location.y) ).GridSquareX
//            var gridSquareY = MMWBoardGrid.getGridSquare(MMWBoardGrid, locX: Float(location.x), locY: Float(location.y) ).GridSquareY
//            var gridSquareXCorner = MMWBoardGrid.getGridSquare(MMWBoardGrid, locX: Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerX
//            var gridSquareYCorner = MMWBoardGrid.getGridSquare(MMWBoardGrid, locX: Float(location.x), locY: Float(location.y) ).GridSquareLowerLeftCornerY
//            println("MMWBoardGrid: gridX\(gridSquareX), gridY\(gridSquareY) \(gridSquareXCorner) \(gridSquareYCorner)")
//            println("nodes.count is: \(nodes.count) ")
            
            
            
            
//            var testLocation = CGPoint(x: 22.5, y: 44.4)
//            self.position = testLocation
//            println("Dropped location: \(testLocation) and gridSquare is \( (self.parent as! MMWGameScene).getBoardGrid() )" )

//            self.anchorPoint = CGPointMake(0.0, 0.0)
//            let smallSlide = SKAction.moveByX(6, y: 6, duration: 0.1)
//            runAction(smallSlide, withKey: "slideBack")
//            runAction(SKAction.group([dropDown, smallSlide]))
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