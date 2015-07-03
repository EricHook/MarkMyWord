////
////  LetterTileController.swift
////  MarkMyWord
////
////  Created by Eric Hook on 4/30/15.
////  Copyright (c) 2015 Hook Studios. All rights reserved.
////
//
//import Foundation
//import SpriteKit
//
//class LetterTileController : SKSpriteNode {
//    
//    var isMovable : Bool = true
//    var faceUp : Bool = true
//    
//    let frontTexture: SKTexture
//    let backTexture: SKTexture
//    var largeTexture: SKTexture?
//    let largeTextureFilename: String
//    
//    
//    
//    enum TileName: Int {
//        case Ace = 1,
//        Jack = 11,
//        Queen = 12,
//        King = 13
//    }
//    
//    enum CardName: Int {
//        case CreatureWolf = 0,
//        CreatureBear,       // 1
//        CreatureDragon,     // 2
//        Energy,             // 3
//        SpellDeathRay,      // 4
//        SpellRabid,         // 5
//        SpellSleep,         // 6
//        SpellStoneskin      // 7
//    }
//    
//    required init(coder aDecoder: NSCoder) {
//        fatalError("NSCoding not supported")
//    }
//    
//    init(imageNamed: String) {
//        // initialize properties
//        backTexture = SKTexture(imageNamed: "card_back.png")
//        frontTexture = SKTexture(imageNamed: "card_creature_wolf.png")
//        //largeTexture = SKTexture(imageNamed: "card_creature_wolf.png")
//        largeTextureFilename = "card_back_large.png"
//        let cardTexture = SKTexture(imageNamed: imageNamed)
//        super.init(texture: cardTexture, color: UIColorAppleBlue, size: cardTexture.size())
//        self.userInteractionEnabled = true
//    }
//    
//    init(cardNamed: CardName) {
//        // initialize properties
//        backTexture = SKTexture(imageNamed: "card_back.png")
//        
//        switch cardNamed {
//        case .CreatureWolf:
//            frontTexture = SKTexture(imageNamed: "card_creature_wolf.png")
//            largeTextureFilename = "card_creature_wolf_large.png"
//            
//        case .CreatureBear:
//            frontTexture = SKTexture(imageNamed: "card_creature_bear.png")
//            largeTextureFilename = "card_creature_bear_large.png"
//            
//        default:
//            frontTexture = SKTexture(imageNamed: "card_back.png")
//            largeTextureFilename = "card_back_large.png"
//        }
//        
//        // call designated initializer on super
//        super.init(texture: frontTexture, color: UIColorAppleBlue, size: frontTexture.size())
//        
//        // set properties defined in super
//        userInteractionEnabled = true
//    }
//    
//    func flip() {
//        if faceUp {
//            self.texture = self.backTexture
//            if let damageLabel = self.childNodeWithName("damageLabel") {
//                damageLabel.hidden = true
//            }
//            self.faceUp = false
//        } else {
//            self.texture = self.frontTexture
//            if let damageLabel = self.childNodeWithName("damageLabel") {
//                damageLabel.hidden = false
//            }
//            self.faceUp = true
//        }
//    }
//    
//    var enlarged = false
//    var savedPosition = CGPointZero
//    
//    func enlarge() {
//        if enlarged {
//            let slide = SKAction.moveTo(savedPosition, duration:0.3)
//            let scaleDown = SKAction.scaleTo(1.0, duration:0.3)
//            runAction(SKAction.group([slide, scaleDown])) {
//                self.enlarged = false
//                self.zPosition = 0
//            }
//        } else {
//            enlarged = true
//            savedPosition = position
//            
//            if largeTexture != nil {
//                texture = largeTexture
//            } else {
//                largeTexture = SKTexture(imageNamed: largeTextureFilename)
//                texture = largeTexture
//            }
//            
//            zPosition = 20
//            
//            let newPosition = CGPointMake(CGRectGetMidX(parent!.frame), CGRectGetMidY(parent!.frame))
//            removeAllActions()
//            
//            let slide = SKAction.moveTo(newPosition, duration:0.3)
//            let scaleUp = SKAction.scaleTo(5.0, duration:0.3)
//            runAction(SKAction.group([slide, scaleUp]))
//        }
//    }
//
//    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
//        /* Called when a touch begins */
//        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//        runAction(actionSound)
//        
//        for touch in (touches as Set<UITouch>) {
//            if touch.tapCount > 1 {
//                enlarge()
//            }
//            
//            if enlarged { return }
//            zPosition = 15
//            let liftUp = SKAction.scaleTo(1.2, duration: 0.2)
//            runAction(liftUp, withKey: "pickup")
//        }
//    }
//    
//    override func touchesMoved(touches: Set<UITouch>, withEvent event: UIEvent?) {
//        if enlarged { return }
//        for touch in (touches as Set<UITouch>) {
//            if isMovable {
//                let location = touch.locationInNode(scene!)
//                let touchedNode = nodeAtPoint(location)
//                touchedNode.position = location
//            }
//        }
//    }
//    
//    override func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?) {
//        if enlarged { return }
//        zPosition = 0
//        let dropDown = SKAction.scaleTo(1.0, duration: 0.2)
//        runAction(dropDown, withKey: "drop")
//    }
//    
//}