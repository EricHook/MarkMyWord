//
//  GameScene.swift
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit

let backgroundNode = SKSpriteNode(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])

let myLabel = SKLabelNode(fontNamed:"HelveticaNeue")

class MainMenuScene: SKScene {

    var viewSize = screenSize!
    
    var waiting = true
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        size = viewSize
    }

    
    override init(size: CGSize) {
        super.init(size: size)
    }

    
    override func didMoveToView(view: SKView) {
        /* Setup your scene here */

        screenSize = view.bounds.size

        // add BG
        backgroundNode.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.size = self.frame.size;
        self.addChild(backgroundNode)

        myLabel.text = "Mark My Word" // scene is: \(scene?.description)"
        myLabel.fontSize = 95;
        myLabel.position = CGPoint( x:CGRectGetMidX(self.frame), y: CGRectGetMidY(self.frame) )
        self.addChild(myLabel)
        myLabel.zPosition = 100
        
        delay(0.5){
            self.presentMMWScene()
        }
    }
    
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
//        for touch: AnyObject in touches {
//            let location = touch.locationInNode(self)
//            let _node:SKNode = self.nodeAtPoint(location)
//        }
    }
    
    
    func presentMMWScene() -> MMWGameScene {
        view?.presentScene(mmwGameScene)
        if debugMode == true { print("presentMMWScene") }
        mmwGameSceneViewController.setUpGame()
        return mmwGameScene
    }
    
    
    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
    
    
    deinit {
        print("MainMenuScene is being deinitialized")
    }
}
