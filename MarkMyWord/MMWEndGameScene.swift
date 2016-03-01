////
////  MenuScene.swift
////  MarkMyWord
////
////  Created by Eric Hook on 4/29/15.
////  Copyright (c) 2015 Hook Studios. All rights reserved.
////
//
//import Foundation
//import SpriteKit
//
//
////var backgroundNode = SKSpriteNode(imageNamed: "MMWOptionsScreen.png")
////var newGameSpriteNode = SKSpriteNode(imageNamed: "NewGameScreen.png")
////var loadingIndicator = SKSpriteNode(imageNamed: "PlayButton.png")
//
//var backgroundNode = SKSpriteNode(imageNamed: "MMWOptionsScreen.png")
////let backgroundNode = SKSpriteNode(imageNamed: "MMWBG.png")
////let endGameButtonNode = SKSpriteNode(imageNamed: "EndGameButton.png")
////let resumeGameButtonNode = SKSpriteNode(imageNamed: "ResumeButton.png")
//
//class MMWEndGameScene: SKScene {
//    
////    override func viewDidLoad() {
////    
////        super.viewDidLoad()
//    
////        let button   = UIButton(type: UIButtonType.System) as UIButton
////        button.frame = CGRectMake(100, 100, 100, 50)
////        button.backgroundColor = UIColor.greenColor()
////        button.setTitle("Test Button", forState: UIControlState.Normal)
////        button.addTarget(self, action: "buttonAction:", forControlEvents: UIControlEvents.TouchUpInside)
////        
////        self.view.addSubview(button)
////    }
//    
//    func buttonAction(sender:UIButton!)
//    {
//        print("Button tapped")
//    }
//    
//    
//    
//    required init?(coder aDecoder: NSCoder) {
//        super.init(coder: aDecoder)
//    }
//    
//    override init() {
//        
//        super.init(size: screenSize!)
//        
//        
//        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        backgroundNode.position = CGPointMake(size.width / 2.0, 0)
//        backgroundNode.zPosition = -100
//        addChild(backgroundNode)
//        
//        
////        endGameButtonNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
////        endGameButtonNode.name = "endGameButtonNode"
////        endGameButtonNode.position = CGPointMake(size.width * 0.25, size.height * 0.5)
////        endGameButtonNode.zPosition = -99
////        addChild(endGameButtonNode)
////        
////        
////        resumeGameButtonNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
////        resumeGameButtonNode.name = "resumeGameButtonNode"
////        resumeGameButtonNode.position = CGPointMake(size.width * 0.75, size.height * 0.5)
////        resumeGameButtonNode.zPosition = -99
////        addChild(resumeGameButtonNode)
//        
////        let gameResultTextNode = SKLabelNode(fontNamed: "Copperplate")
////        gameResultTextNode.text = "END GAME??? YOU " + (gameResult ? "WON" : "LOST")
////        gameResultTextNode.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
////        gameResultTextNode.verticalAlignmentMode = SKLabelVerticalAlignmentMode.Center
////        gameResultTextNode.fontSize = 20
////        gameResultTextNode.fontColor = SKColor.whiteColor()
////        gameResultTextNode.position = CGPointMake(size.width / 2.0, size.height - 200.0)
////        addChild(gameResultTextNode)
////        
////        let scoreTextNode = SKLabelNode(fontNamed: "Copperplate")
////        scoreTextNode.text = "END GAME ??? SCORE :  \(score)"
////        scoreTextNode.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
////        scoreTextNode.verticalAlignmentMode = SKLabelVerticalAlignmentMode.Center
////        scoreTextNode.fontSize = 20
////        scoreTextNode.fontColor = SKColor.whiteColor()
////        scoreTextNode.position = CGPointMake(size.width / 2.0, gameResultTextNode.position.y - 40.0)
////        addChild(scoreTextNode)
//        
////        let tryAgainTextNodeLine1 = SKLabelNode(fontNamed: "Copperplate")
////        tryAgainTextNodeLine1.text = "TAP ANYWHERE"
////        tryAgainTextNodeLine1.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
////        tryAgainTextNodeLine1.verticalAlignmentMode = SKLabelVerticalAlignmentMode.Center
////        tryAgainTextNodeLine1.fontSize = 20
////        tryAgainTextNodeLine1.fontColor = SKColor.whiteColor()
////        tryAgainTextNodeLine1.position = CGPointMake(size.width / 2.0, 100.0)
////        addChild(tryAgainTextNodeLine1)
////        
////        let tryAgainTextNodeLine2 = SKLabelNode(fontNamed: "Copperplate")
////        tryAgainTextNodeLine2.text = "TO PLAY AGAIN!"
////        tryAgainTextNodeLine2.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode.Center
////        tryAgainTextNodeLine2.verticalAlignmentMode = SKLabelVerticalAlignmentMode.Center
////        tryAgainTextNodeLine2.fontSize = 20
////        tryAgainTextNodeLine2.fontColor = SKColor.whiteColor()
////        tryAgainTextNodeLine2.position = CGPointMake(size.width / 2.0,
////            tryAgainTextNodeLine1.position.y - 40.0)
////        addChild(tryAgainTextNodeLine2)
////
////        let button   = UIButton(type: UIButtonType.System) as UIButton
////        button.frame = CGRectMake(100, 100, 100, 50)
////        button.backgroundColor = UIColor.greenColor()
////        button.setTitle("Test Button", forState: UIControlState.Normal)
////        button.addTarget(self, action: "buttonAction:", forControlEvents: UIControlEvents.TouchUpInside)
//
//        //var uiView : UIV
//        //addChild(button)
//        //self.view!.addSubview(button)
//    }
//    
//    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
//        /* Called when a touch begins */
//        //        if userInteractionEnabled {
//        //            let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//        //            runAction(actionSound)
//        
//        let transition = SKTransition.crossFadeWithDuration(0.5)
//        
//        /* Called when a touch begins */
//        for touch: AnyObject in touches {
//            let location = touch.locationInNode(self)
//            let _node:SKNode = self.nodeAtPoint(location)
//            
////            if(_node.name == "resumeGameButtonNode"){
////                view?.presentScene(mmwGameScene, transition: transition)
////                print("mmwEndGameScene presentScene mmwGameScene")
////                
////                if mmwGameSceneViewController.audioOn == true {
////                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
////                    runAction(actionSound)
////                }
////            }
////            
////            if(_node.name == "endGameButtonNode"){
////                view?.presentScene(mmwResultsScene, transition: transition)
////                print("mmwEndGameScene presentScene mmwResultsScene")
////                
////                if mmwGameSceneViewController.audioOn == true {
////                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
////                    runAction(actionSound)
////                }
////            }
//        }
//
//        
////        for touch in (touches as Set<UITouch>) {
////            let location = touch.locationInNode(self)
////            let touchedNode = nodeAtPoint(location)
////            
////            if touchedNode == resumeGameButtonNode {
////                let transition = SKTransition.crossFadeWithDuration(0.5)
////                //        //mmwGameScene = MMWGameScene(size: size)
////                //        transitionToScene = mmwGameSceneViewController.mmwGameScene
////                //        currentScene = mmwGameScene
////                //        mmwGameScene.scaleMode = .AspectFill
////                view?.presentScene(mmwGameScene, transition: transition)
////                print("mmwResultsScene presentScene mmwGameScene")
////            }
////            
////            if touchedNode == endGameButtonNode {
////                let transition = SKTransition.crossFadeWithDuration(0.5)
////                //        //mmwGameScene = MMWGameScene(size: size)
////                //        transitionToScene = mmwGameSceneViewController.mmwGameScene
////                //        currentScene = mmwGameScene
////                //        mmwGameScene.scaleMode = .AspectFill
////                view?.presentScene(mmwEndGameScene, transition: transition)
////                print("mmwResultsScene presentScene mmwGameScene")
////            }
////                
////            else {
////                //playMMWScene()
////                print("results scene touched")
////            }
////        }
//    }
//    
////    func playMMWScene() {
////        let transition = SKTransition.crossFadeWithDuration(2.0)
////        let MMWScene = MMWGameScene(size: size)
////        MMWScene.name = "MMWScene instance name made in MenuScene"
////        //var currentScene = MMWScene
////        view?.presentScene(MMWScene, transition: transition)
////    }
//}