//
//  Card.swift
//  rayTutorialCardGame
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

class Card : SKSpriteNode {
    
    var isMovable : Bool = true
    
    required init(coder aDecoder: NSCoder) {
        fatalError("NSCoding not supported")
    }
    
    init(imageNamed: String) {
        let cardTexture = SKTexture(imageNamed: imageNamed)
        super.init(texture: cardTexture, color: nil, size: cardTexture.size())
        userInteractionEnabled = true
    }
    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        /* Called when a touch begins */
            
        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
        runAction(actionSound)
        
        for touch in (touches as! Set<UITouch>) {
//            let location = touch.locationInNode(self)
//            let touchedNode = nodeAtPoint(location)
            zPosition = 15
            let liftUp = SKAction.scaleTo(1.2, duration: 0.2)
            runAction(liftUp, withKey: "pickup")
            //            let sprite = SKSpriteNode(imageNamed:"Spaceship")
            //            sprite.xScale = 0.5
            //            sprite.yScale = 0.5
            //            sprite.position = location
            //            let action = SKAction.rotateByAngle(CGFloat(M_PI), duration:1)
            //            sprite.runAction(SKAction.repeatActionForever(action))
            //            self.addChild(sprite)
        }
    
    }
    
    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
        for touch in (touches as! Set<UITouch>) {
            if isMovable {
                let location = touch.locationInNode(scene)
                let touchedNode = nodeAtPoint(location)
                touchedNode.position = location
            }
        }
    }
    
    override func touchesEnded(touches: Set<NSObject>, withEvent event: UIEvent) {
        for touch in (touches as! Set<UITouch>) {
//            let location = touch.locationInNode(self)
//            let touchedNode = nodeAtPoint(location)
            zPosition = 0
            let dropDown = SKAction.scaleTo(1.0, duration: 0.2)
            runAction(dropDown, withKey: "drop")
        }
    }

}