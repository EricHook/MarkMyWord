//
//  HUD.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/28/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit

class HUD : SKSpriteNode
{
    var hudTexture : SKTexture
    var hudColor : UIColor = UIColor(red: 0.0, green: 1.0, blue: 0.0, alpha: 0.5)
    var hudSize : CGSize = CGSize(width: 45, height: 45)
    var hudLabel : SKLabelNode = SKLabelNode(fontNamed: FontHUDName)
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
//    init(texture: SKTexture!, color: UIColor!, size: CGSize) {
//        <#code#>
//    }
    
    init(hudNamed: String) {
        // initialize properties
        hudTexture = SKTexture(imageNamed: "card_back.png")
        
        hudLabel.text = "UNOBTRUSIVE is a partal word"
        hudLabel.fontSize = FontHUDSize
        hudLabel.position = CGPointMake(65, 65)
        
        //addChild(partialWordHUD)
        
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
        
        // call designated initializer on super
        super.init(texture: hudTexture, color: hudColor, size: hudTexture.size())
        
        // set properties defined in super
        userInteractionEnabled = true
    }
    
//    func displayTextWord () {
//        var partialWordHUD = SKLabelNode(fontNamed: FontHUDName)
//        partialWordHUD.text = "UNOBTRUSIVE is a partal word"
//        partialWordHUD.fontSize = FontHUDSize
//        partialWordHUD.position = CGPointMake(55, 55)
//        addChild(partialWordHUD)
//    }
    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        /* Called when a touch begins */
        
        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
        runAction(actionSound)
        
        for touch in (touches as! Set<UITouch>) {
//                        let sprite = SKSpriteNode(imageNamed:"Spaceship")
//                        sprite.xScale = 0.5
//                        sprite.yScale = 0.5
//                        sprite.position = CGPointMake(88, 88)
//                        let action = SKAction.rotateByAngle(CGFloat(M_PI), duration:1)
//                        sprite.runAction(SKAction.repeatActionForever(action))
//                        self.addChild(sprite)
            // GameScene.playMMWScene()
         }
        
    }

    
    
    
}