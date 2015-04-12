//
//  GameScene.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/10/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit
// import UIKit

class GameScene: SKScene, SKPhysicsContactDelegate {
    
    var backgroundNode : SKSpriteNode?
    var foregroundNode : SKSpriteNode?
    var spriteTest : SKSpriteNode?
//    var orbNode : SKSpriteNode?
    var impulseCount = 4
    
    let CollisionCategoryPlayer : UInt32 = 0x1 << 1
    let CollisionCategoryPowerUp : UInt32 = 0x1 << 2
    
    required init?(coder aDecoder: NSCoder) {
        
        super.init(coder: aDecoder)
    }
    
    override init(size: CGSize) {
        
        super.init(size: size)
        physicsWorld.contactDelegate = self
        
        physicsWorld.gravity = CGVectorMake(0.0, -4.0)
        
        backgroundColor = SKColor(red: 0.5, green: 0.0, blue: 0.0, alpha: 1.0)
        
        userInteractionEnabled = true
        
        // add BG
        backgroundNode = SKSpriteNode(imageNamed: "MarkMyWordBG_iPadR")   // "MarkMyWordBG_iPadR")
        backgroundNode!.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode!.position = CGPoint(x: size.width/2.0, y: 0.0)
        addChild(backgroundNode!)
        
        foregroundNode = SKSpriteNode()
        addChild(foregroundNode!)
        
        // add sprite
        spriteTest = SKSpriteNode (imageNamed: "LetterTest90x90")
        spriteTest!.physicsBody = SKPhysicsBody(circleOfRadius: spriteTest!.size.width / 2)
        spriteTest!.physicsBody!.dynamic = false
        spriteTest!.anchorPoint = CGPoint(x: 0.5, y: 0.5)
        spriteTest!.position = CGPoint(x: self.size.width/2.0, y: 180.0)
        spriteTest!.physicsBody!.linearDamping = 1.0
        spriteTest!.physicsBody!.allowsRotation = false
        spriteTest!.physicsBody!.categoryBitMask = CollisionCategoryPlayer
        spriteTest!.physicsBody!.contactTestBitMask = CollisionCategoryPowerUp
        //spriteTest!.physicsBody!.collisionBitMask = 1
        foregroundNode!.addChild(spriteTest!)
        
//        orbNode = SKSpriteNode(imageNamed: "avatarpic")
//        orbNode!.position = CGPoint(x: size.width / 1.95  , y: size.height - 150)
//        orbNode!.physicsBody = SKPhysicsBody(circleOfRadius: orbNode!.size.width / 2)
//        orbNode!.physicsBody!.dynamic = false
//        orbNode!.physicsBody!.categoryBitMask = CollisionCategoryPowerUp
//        //orbNode!.physicsBody!.collisionBitMask = 1
//        orbNode!.name = "POWER_UP_ORB"
//        foregroundNode!.addChild(orbNode!)
        
        var orbNodePosition = CGPointMake(spriteTest!.position.x, spriteTest!.position.y + 100)
        for i in 0...19 {
            var orbNode = SKSpriteNode(imageNamed: "avatarpic")
            orbNodePosition.y += 140
            orbNode.position = orbNodePosition
            orbNode.physicsBody = SKPhysicsBody(circleOfRadius: orbNode.size.width/2)
            orbNode.physicsBody!.dynamic = false
            orbNode.physicsBody!.categoryBitMask = CollisionCategoryPowerUp
            orbNode.name = "POWER_UP_ORB"
            foregroundNode!.addChild(orbNode)
        }
        
        orbNodePosition = CGPointMake(spriteTest!.position.x+50, spriteTest!.position.y)
        for i in 0...19 {
            var orbNode = SKSpriteNode(imageNamed: "avatarpic")
            orbNodePosition.y += 140
            orbNode.position = orbNodePosition
            orbNode.physicsBody = SKPhysicsBody(circleOfRadius: orbNode.size.width/2)
            orbNode.physicsBody!.dynamic = false
            orbNode.physicsBody!.categoryBitMask = CollisionCategoryPowerUp
            orbNode.name = "POWER_UP_ORB"
            foregroundNode!.addChild(orbNode)
        }
    }
    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        //spriteTest!.physicsBody!.applyImpulse(CGVectorMake(0.0, 40.0))
        
        if !spriteTest!.physicsBody!.dynamic {
            spriteTest!.physicsBody!.dynamic = true
        }
        if impulseCount > 0 {
            spriteTest!.physicsBody!.applyImpulse(CGVectorMake(0.0, 40.0))
            impulseCount--
        }
        
        println("TOUCH: \(event.description) ")
        for touch: AnyObject in touches {
            var location = (touch as! UITouch).locationInView(self.view)
            addTile(location)
        }
    }
    
    func didBeginContact(contact: SKPhysicsContact) {
        var nodeB = contact.bodyB!.node!
        if nodeB.name == "POWER_UP_ORB" {
            impulseCount++
            nodeB.removeFromParent()
        }
        println("CONTACT! contactPoint:\(contact.contactPoint), collisionImpulse:\(contact.collisionImpulse) , contactNormal:(contact.contactNormal)")
    }
    
    func addTile (touchPoint: CGPoint){
        var tileNode = SKSpriteNode(imageNamed: "LetterTest90x90")
        tileNode.position.x = touchPoint.x
        tileNode.position.y = (size.height - (touchPoint.y))
        //convertPoint(touchPoint)
        //tileNode!.physicsBody = SKPhysicsBody(circleOfRadius: orbNode!.size.width / 2)
        //tileNode!.physicsBody!.dynamic = false
        //tileNode!.physicsBody!.categoryBitMask = CollisionCategoryPowerUp
        //orbNode!.physicsBody!.collisionBitMask = 1
        tileNode.name = "tile"
        addChild(tileNode)
    }
    
    override func update(currentTime: NSTimeInterval) {
        
        if spriteTest!.position.y >= 180.0 {
            self.backgroundNode!.position = CGPointMake(self.backgroundNode!.position.x, -((self.spriteTest!.position.y-180.0)/8))
            foregroundNode!.position = CGPointMake(foregroundNode!.position.x, -((spriteTest!.position.y-180.0)/8))

            
        }

    }
}


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
