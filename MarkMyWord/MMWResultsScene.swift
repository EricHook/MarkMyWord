//
//  MenuScene.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

class MMWResultsScene: SKScene {
    
//    override func viewDidLoad() {
//    
//        super.viewDidLoad()
    
//        let button   = UIButton(type: UIButtonType.System) as UIButton
//        button.frame = CGRectMake(100, 100, 100, 50)
//        button.backgroundColor = UIColor.greenColor()
//        button.setTitle("Test Button", forState: UIControlState.Normal)
//        button.addTarget(self, action: "buttonAction:", forControlEvents: UIControlEvents.TouchUpInside)
//        
//        self.view.addSubview(button)
//    }
    
    func buttonAction(sender:UIButton!)
    {
        print("Button tapped")
    }
    
    
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    init(size: CGSize, gameResult: Bool, score: Int) {
        
        super.init(size: size)
        
        let backgroundNode = SKSpriteNode(imageNamed: "MMWBG.png")
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPointMake(size.width / 2.0, 0)
        backgroundNode.zPosition = -100
        addChild(backgroundNode)
        
        let gameResultTextNode = SKLabelNode(fontNamed: "Copperplate")
        gameResultTextNode.text = "YOU " + (gameResult ? "WON" : "LOST")
        gameResultTextNode.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
        gameResultTextNode.verticalAlignmentMode = SKLabelVerticalAlignmentMode.Center
        gameResultTextNode.fontSize = 20
        gameResultTextNode.fontColor = SKColor.whiteColor()
        gameResultTextNode.position = CGPointMake(size.width / 2.0, size.height - 200.0)
        addChild(gameResultTextNode)
        
        let scoreTextNode = SKLabelNode(fontNamed: "Copperplate")
        scoreTextNode.text = "SCORE :  \(score)"
        scoreTextNode.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
        scoreTextNode.verticalAlignmentMode = SKLabelVerticalAlignmentMode.Center
        scoreTextNode.fontSize = 20
        scoreTextNode.fontColor = SKColor.whiteColor()
        scoreTextNode.position = CGPointMake(size.width / 2.0, gameResultTextNode.position.y - 40.0)
        addChild(scoreTextNode)
        
        let tryAgainTextNodeLine1 = SKLabelNode(fontNamed: "Copperplate")
        tryAgainTextNodeLine1.text = "TAP ANYWHERE"
        tryAgainTextNodeLine1.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
        tryAgainTextNodeLine1.verticalAlignmentMode = SKLabelVerticalAlignmentMode.Center
        tryAgainTextNodeLine1.fontSize = 20
        tryAgainTextNodeLine1.fontColor = SKColor.whiteColor()
        tryAgainTextNodeLine1.position = CGPointMake(size.width / 2.0, 100.0)
        addChild(tryAgainTextNodeLine1)
        
        let tryAgainTextNodeLine2 = SKLabelNode(fontNamed: "Copperplate")
        tryAgainTextNodeLine2.text = "TO PLAY AGAIN!"
        tryAgainTextNodeLine2.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
        tryAgainTextNodeLine2.verticalAlignmentMode = SKLabelVerticalAlignmentMode.Center
        tryAgainTextNodeLine2.fontSize = 20
        tryAgainTextNodeLine2.fontColor = SKColor.whiteColor()
        tryAgainTextNodeLine2.position = CGPointMake(size.width / 2.0,
            tryAgainTextNodeLine1.position.y - 40.0)
        addChild(tryAgainTextNodeLine2)
        
        
        
//        let button   = UIButton(type: UIButtonType.System) as UIButton
//        button.frame = CGRectMake(100, 100, 100, 50)
//        button.backgroundColor = UIColor.greenColor()
//        button.setTitle("Test Button", forState: UIControlState.Normal)
//        button.addTarget(self, action: "buttonAction:", forControlEvents: UIControlEvents.TouchUpInside)
//        
//        var uiView : UIV
//        addChild(button)
//        //self.view!.addSubview(button)
    }
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
        //        if userInteractionEnabled {
        //            let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
        //            runAction(actionSound)
        for _ in (touches as Set<UITouch>) {
            //let location = touch.locationInNode(self)
            //let touchedNode = nodeAtPoint(location)
            
            //playMMWScene()
            
            print("results scene touched")
            
            let transition = SKTransition.crossFadeWithDuration(0.5)
            //        //mmwGameScene = MMWGameScene(size: size)
            //        transitionToScene = mmwGameSceneViewController.mmwGameScene
            //        currentScene = mmwGameScene
            //        mmwGameScene.scaleMode = .AspectFill
            view?.presentScene(mmwGameScene, transition: transition)
            print("mmwResultsScene presentScene mmwGameScene")
            
            
        }
        //        }
    }
    
//    func playMMWScene() {
//        let transition = SKTransition.crossFadeWithDuration(2.0)
//        let MMWScene = MMWGameScene(size: size)
//        MMWScene.name = "MMWScene instance name made in MenuScene"
//        //var currentScene = MMWScene
//        view?.presentScene(MMWScene, transition: transition)
//    }
}