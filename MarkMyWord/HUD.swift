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

class HUD : MMWGameSceneView
{

    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    func displayTextWord () {
        var partialWordHUD = SKLabelNode(fontNamed: FontHUDName)
        partialWordHUD.text = "UNOBTRUSIVE is a partal word"
        partialWordHUD.fontSize = FontHUDSize
//        partialWordHUD.position = CGPointMake(spriteNode.size.width/2.0 - 222, 13)
//        spriteNode.addChild(partialWordHUD)
    }
}