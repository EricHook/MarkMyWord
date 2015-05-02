//
//  TileView.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/30/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

class LetterTile : SKSpriteNode {
    
    var isMovable : Bool = true
    var faceUp : Bool = true
    var hasShadow : Bool = false
    
    var frontTexture : SKTexture
    var backTexture : SKTexture = SKTexture(imageNamed: "TileBackTest90x90")
    var largeTexture : SKTexture?
    var largeTextureFilename : String
    var tileLocation : CGPoint = CGPointMake(0, 0)
    var shadow : SKTexture = SKTexture(imageNamed: "TileShadow90x90")
    var tileShadow : SKSpriteNode

    
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
        
        tileShadow = SKSpriteNode(texture: shadow, color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00), size: CGSizeMake(50.0, 50.0))
        
        switch tileStyle {
            case .basic:
                frontTexture = SKTexture(imageNamed: "tileBase45x45")
                largeTextureFilename = "tileBase45x45"
                
            case .metal:
                frontTexture = SKTexture(imageNamed: "tileBase45x45")
                largeTextureFilename = "tileBase45x45"
                
            default:
                frontTexture = SKTexture(imageNamed: "tileBase45x45")
                largeTextureFilename = "tileBase45x45"
        }
        
        // call designated initializer on super
        super.init(texture: frontTexture, color: nil, size: frontTexture.size())
        
        // initialize properties
            self.name = "tile"
            self.position = atPoint
            self.anchorPoint = CGPointMake(0.5, 0.5)
            self.color = withColor
            self.colorBlendFactor = 1.0
        
            backTexture = SKTexture(imageNamed: "TileBackTest90x90")
        
            tileShadow =  SKSpriteNode(texture: shadow, color: UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 0.00), size: CGSizeMake(50.0, 50.0))
            tileShadow.alpha = 0.15
            tileShadow.position = CGPointMake(-5, -5)
            tileShadow.zPosition = self.zPosition - 1
            tileShadow.hidden = true
            self.addChild(tileShadow)
        
            var letterLabel = SKLabelNode(fontNamed: FontHUDName)
            letterLabel.text = withChar
            letterLabel.fontSize = 40 // FontHUDSize
            //var colorDarkGreen : UIColor = UIColor(red: 0.1, green: 1.0, blue: 0.2, alpha: 0.80)
            letterLabel.fontColor = FontHUDWhite
            letterLabel.position = CGPointMake(0, -14)
            letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
            self.addChild(letterLabel)
            centerTileToSquare(self)
        
//            var letterValueLabel = SKLabelNode(fontNamed: FontHUDName)
//            letterValueLabel.text = "10"
//            letterValueLabel.fontSize = 10
//            letterValueLabel.fontColor = FontHUDWhite
//            letterValueLabel.position = CGPointMake(1, 1)
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
    
    func centerTileToSquare(tile : LetterTile) {
        tile.position.x += 22.5
        tile.position.y += 22.5
    }
    
    var enlarged = false
    var savedPosition = CGPointZero
    
    func enlarge() {
        if enlarged {
            let slide = SKAction.moveTo(savedPosition, duration:0.3)
            let scaleDown = SKAction.scaleTo(1.0, duration:0.3)
            texture = backTexture
            runAction(SKAction.group([slide, scaleDown])) {
                self.enlarged = false
                self.zPosition = 0
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
    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        /* Called when a touch begins */
        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
        runAction(actionSound)
        
        for touch in (touches as! Set<UITouch>) {
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
        }
    }
    
    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
        if enlarged { return }
        for touch in (touches as! Set<UITouch>) {
            if isMovable {
                let location = touch.locationInNode(scene)
                let touchedNode = nodeAtPoint(location)
                touchedNode.position = location
            }
        }
    }
    
    override func touchesEnded(touches: Set<NSObject>, withEvent event: UIEvent) {
        if enlarged { return }
        for touch in (touches as! Set<UITouch>) {
            //            let location = touch.locationInNode(self)
            //            let touchedNode = nodeA                                                                                                                                          Point(location)
            zPosition = 1
            let dropDown = SKAction.scaleTo(1.0, duration: 0.1)
            runAction(dropDown, withKey: "drop")
            tileShadow.zPosition = -1
            tileShadow.hidden = true

            //self.anchorPoint = CGPointMake(0.0, 0.0)
//            let smallSlide = SKAction.moveByX(6, y: 6, duration: 0.1)
//            runAction(smallSlide, withKey: "slideBack")
//            runAction(SKAction.group([dropDown, smallSlide]))
        }
    }
    
}