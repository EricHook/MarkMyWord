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
        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y:CGRectGetMidY(self.frame))
        self.addChild(myLabel)
        
//        let wolf = CardController(imageNamed: "LetterTest90x90")
//        wolf.position = CGPointMake(500,200)
//        self.addChild(wolf)
        
        let cardBear01 = CardController(cardNamed: .CreatureBear)
        cardBear01.position = CGPointMake(400,200)
        self.addChild(cardBear01)
        
        let cardWolf01 = CardController(cardNamed: .CreatureWolf)
        cardWolf01.position = CGPointMake(600,200)
        self.addChild(cardWolf01)
        
        cardWolf01.addChild(newDamageLabel())
        cardBear01.addChild(newDamageLabel())
        
        var letterTest01 = LetterTile(tileStyle: .basic, withChar: "P", withColor: SKColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 1.0), atPoint: CGPointMake(22, 22))
        addChild(letterTest01)
        
        let HUDNav = HUD(hudNamed: "testHUD01")
        HUDNav.position = CGPointMake(712,600)
        //HUDNav.hudLabel =
        self.addChild(HUDNav)
        //self.addChild(HUDNav.hudLabel)
        
//        let bear = LetterTile(imageNamed: "HookAvatar80x80")
//        bear.position = CGPointMake(300, 200)
//        self.addChild(bear)
    }
    
    func newDamageLabel() -> SKLabelNode {
        let damageLabel = SKLabelNode(fontNamed: "OpenSans-Bold")
        damageLabel.name = "damageLabel"
        damageLabel.fontSize = 12
        damageLabel.fontColor = UIColor(red: 0.47, green: 0.0, blue: 0.0, alpha: 1.0)
        damageLabel.text = "X0"
        damageLabel.position = CGPointMake(25, 40)
        
        return damageLabel
    }
    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        /* Called when a touch begins */
//        if userInteractionEnabled {
//            let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//            runAction(actionSound)
//            
            for touch in (touches as! Set<UITouch>) {
                let location = touch.locationInNode(self)
                let touchedNode = nodeAtPoint(location)
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
                
                playMMWScene()
            }
//        }
    }
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
    
    func playMenuScene() {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        let menuScene = MenuScene(size: size,
            gameResult: true,
            score: 123)
        view?.presentScene(menuScene, transition: transition)
    }
    
    func playMMWScene() {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        let MMWScene = MMWGameScene(size: size)
        view?.presentScene(MMWScene, transition: transition)
    }
   
    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
}
