//
//  GameScene.swift
//  rayTutorialCardGame
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit

class MainMenuScene: SKScene {

    var currentScene : SKScene?
    var mmwGameSceneViewController : MMWGameSceneViewController!
    var viewSize:CGSize!
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        //mmwGameSceneViewController = MMWGameSceneViewController(size: size)
    }
    

    override func didMoveToView(view: SKView) {
        /* Setup your scene here */
        
        viewSize = view.bounds.size
        print("view size MenuScene: \(viewSize)")

//        if UIDevice.currentDevice().userInterfaceIdiom == .Phone {
//            viewSize.height *= 2
//            viewSize.width *= 2
//        }
//        print("view size MenuScene mod: \(viewSize)")
        
//        let BG = SKSpriteNode(imageNamed: "MeyamaSplashScreen_iPad.jpg")
//        BG.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
//        self.addChild(BG)
        
//        // add BG
//        let backgroundNode = SKSpriteNode(imageNamed: "MeyamaMathMenuScreenBG_1024x768.png")
//        //backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        backgroundNode.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
//        backgroundNode.userInteractionEnabled = false
//        self.addChild(backgroundNode)
        
        // add BG
        let backgroundNode = SKSpriteNode(imageNamed: "MarkMyWordBGCleaniPad@2x.png")
        //backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.size = self.frame.size;
        self.addChild(backgroundNode)
        
        let myLabel = SKLabelNode(fontNamed:"Chalkduster")
        myLabel.text = "Main Game Menu" // scene is: \(scene?.description)"
        myLabel.fontSize = 65;
        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: (CGRectGetMidY(self.frame) - 50) )
        self.addChild(myLabel)
        myLabel.zPosition = 100
        
        let playBtn = SKSpriteNode(imageNamed: "PlayButton.png")
        playBtn.position = CGPoint(x: viewSize.width/2, y: viewSize.height/3)
        self.addChild(playBtn)
        playBtn.name = "playBtn"
        playBtn.zPosition = 100
        
                
    }
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
        for touch: AnyObject in touches {
            let location = touch.locationInNode(self)
            let _node:SKNode = self.nodeAtPoint(location)
            if(_node.name == "playBtn"){
                print("going to mmw scene") //create MMW controller
                mmwGameSceneViewController = createMMWSceneController()
                // mmwGameSceneViewController.mmwGameScene.setViewController(mmwGameSceneViewController)
                presentMMWScene()
                // mmwGameScene.scaleMode = SKSceneScaleMode.ResizeFill
                if userInteractionEnabled {
                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
                    runAction(actionSound)
                }
            }
//            if ( location.x < viewSize.width/2){
//                print("[GamePlayScene] touchedLeftSide ")
//            } else if(location.x > viewSize.width/2){
//                print("[GamePlayScene] touchedRightSide ")
//            }
        }
    }

//    func presentMenuScene() {
//        let transition = SKTransition.crossFadeWithDuration(2.0)
//        let menuScene = MenuScene(size: size, gameResult: true, score: 123)
//        view?.presentScene(menuScene, transition: transition)
//        print("presentMenuScene")
//    }
    
    func createMMWSceneController() -> MMWGameSceneViewController {
        mmwGameSceneViewController = MMWGameSceneViewController(size: size)
        return mmwGameSceneViewController
    }
    
    func presentMMWScene() -> MMWGameScene {
        let transition = SKTransition.crossFadeWithDuration(0.5)
//        //mmwGameScene = MMWGameScene(size: size)
//        transitionToScene = mmwGameSceneViewController.mmwGameScene
//        currentScene = mmwGameScene
//        mmwGameScene.scaleMode = .AspectFill
        view?.presentScene(mmwGameSceneViewController.mmwGameScene, transition: transition)
        print("presentMMWScene")
        return mmwGameSceneViewController.mmwGameScene
    }

    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
}
