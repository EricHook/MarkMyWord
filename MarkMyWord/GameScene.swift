//
//  GameScene.swift
//  rayTutorialCardGame
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit

class GameScene: SKScene {
    override func didMoveToView(view: SKView) {
        /* Setup your scene here */
        
//        let navScreenBG = SKNode(fileNamed: "MeyamaMathMenuScreenBG_1024x768")
//        navScreenBG.position = CGPointMake(0, 0)
//        addChild(navScreenBG)
        
        // add BG
        let backgroundNode = SKSpriteNode(imageNamed: "MeyamaMathMenuScreenBG_1024x768.png")
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: size.width/2.0, y: 0.0)
        //        // set scale for various platforms
        //        backgroundNode?.xScale = 0.5
        //        backgroundNode?.yScale = 0.5
        backgroundNode.userInteractionEnabled = false
        self.addChild(backgroundNode)
        
        let myLabel = SKLabelNode(fontNamed:"Chalkduster")
        myLabel.text = "Hello, World TEST!";
        myLabel.fontSize = 65;
        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y:CGRectGetMidY(self.frame));
        
        self.addChild(myLabel)
        
        let wolf = Card(imageNamed: "LetterTest90x90")
        wolf.position = CGPointMake(500,200)
        self.addChild(wolf)
        
        let bear = Card(imageNamed: "HookAvatar80x80")
        bear.position = CGPointMake(300, 200)
        self.addChild(bear)
    }
    
//    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
//        /* Called when a touch begins */
//        
//        if userInteractionEnabled {
//            
//            let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//            runAction(actionSound)
//            
//            for touch in (touches as! Set<UITouch>) {
//                let location = touch.locationInNode(self)
//                let touchedNode = nodeAtPoint(location)
//                touchedNode.zPosition = 15
//                let liftUp = SKAction.scaleTo(1.2, duration: 0.2)
//                touchedNode.runAction(liftUp, withKey: "pickup")
//                
//    //            let sprite = SKSpriteNode(imageNamed:"Spaceship")
//    //            sprite.xScale = 0.5
//    //            sprite.yScale = 0.5
//    //            sprite.position = location
//    //            let action = SKAction.rotateByAngle(CGFloat(M_PI), duration:1)
//    //            sprite.runAction(SKAction.repeatActionForever(action))
//    //            self.addChild(sprite)
//            }
//        }
//    }
//    
//    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
//        for touch in (touches as! Set<UITouch>) {
//            if userInteractionEnabled {
//                let location = touch.locationInNode(self)
//                let touchedNode = nodeAtPoint(location)
//                touchedNode.position = location
//            }
//        }
//    }
//    
//    override func touchesEnded(touches: Set<NSObject>, withEvent event: UIEvent) {
//        for touch in (touches as! Set<UITouch>) {
//            let location = touch.locationInNode(self)
//            let touchedNode = nodeAtPoint(location)
//            touchedNode.zPosition = 0
//            let dropDown = SKAction.scaleTo(1.0, duration: 0.2)
//            touchedNode.runAction(dropDown, withKey: "drop")
//        }
//    }
   
    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
}
