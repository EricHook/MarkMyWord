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
    var mmwGameScene : MMWGameScene!
    var mmwGameSceneViewController : MMWGameSceneViewController!
    var viewSize:CGSize!
    
    required init?(coder aDecoder: NSCoder) {
        //currentScene = MenuScene(size: size, gameResult: true, score: 123)
        super.init(coder: aDecoder)
        //mmwGameScene = MMWGameScene(size: view!.bounds.size)
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        mmwGameSceneViewController = MMWGameSceneViewController(size: size)
        //mmwGameScene = MMWGameScene(size: view!.bounds.size)
        //currentScene = MenuScene(size: size, gameResult: true, score: 123)
    }

    override func didMoveToView(view: SKView) {
        /* Setup your scene here */
        
        viewSize = view.bounds.size
        
        let BG = SKSpriteNode(imageNamed: "MeyamaSplashScreen_iPad.jpg")
        BG.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
        self.addChild(BG)
        
        
        // add BG
        let backgroundNode = SKSpriteNode(imageNamed: "MeyamaMathMenuScreenBG_1024x768.png")
        //backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
        backgroundNode.userInteractionEnabled = false
        self.addChild(backgroundNode)
        
        let myLabel = SKLabelNode(fontNamed:"Chalkduster")
        myLabel.text = "Mark My Word" // scene is: \(scene?.description)"
        print("scene in GameScene is: \(scene?.description)")
        myLabel.fontSize = 65;
        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y:CGRectGetMidY(self.frame))
        self.addChild(myLabel)
        
        let playBtn = SKSpriteNode(imageNamed: "HookAvatar80x80.jpg")
        playBtn.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
        self.addChild(playBtn)
        playBtn.name = "playBtn"
    }
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
        
        for touch: AnyObject in touches {
            let location = touch.locationInNode(self)
            let _node:SKNode = self.nodeAtPoint(location)
            if(_node.name == "playBtn"){
                
//                let scene = MMWGameScene(size: self.size)
//                self.view?.presentScene(scene)
                
                print("going to mmw scene")

                //create MMW controller
                mmwGameSceneViewController = createMMWSceneController()
                mmwGameSceneViewController.testMMWCont() // print to test
                //create MMW view
                mmwGameScene = presentMMWScene(mmwGameSceneViewController)
            
                if userInteractionEnabled {
                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
                    runAction(actionSound)
                }
            }
            if(location.x < viewSize.width/2){
                
                print("[GamePlayScene] touchedLeftSide ")
                
                
            }else if(location.x > viewSize.width/2){
                
                print("[GamePlayScene] touchedRightSide ")
                
            }
            
        }
    }

//    func presentMenuScene() {
//        let transition = SKTransition.crossFadeWithDuration(2.0)
//        let menuScene = MenuScene(size: size, gameResult: true, score: 123)
//        view?.presentScene(menuScene, transition: transition)
//        print("presentMenuScene")
//    }
    
    func createMMWScene() {
        createMMWSceneController()
        mmwGameScene = MMWGameScene(size: size)
    }
    
    func createMMWSceneController() -> MMWGameSceneViewController {
        mmwGameSceneViewController = MMWGameSceneViewController(size: size)
        //MMWGameSceneViewController.
        return mmwGameSceneViewController
    }
    
    func presentMMWScene(mmwGameSceneViewController : MMWGameSceneViewController) -> MMWGameScene {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        //mmwGameScene = MMWGameScene(size: size)
        mmwGameScene = mmwGameSceneViewController.mmwGameScene
        currentScene = mmwGameScene
        view?.presentScene(mmwGameScene, transition: transition)
        print("presentMMWScene")
        return mmwGameScene
    }

    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
}
