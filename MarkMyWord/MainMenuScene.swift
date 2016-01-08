//
//  GameScene.swift
//  rayTutorialCardGame
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit

var mmwOptionScreen : MMWOptionScreen! = MMWOptionScreen(size: screenSize!)
var mmwGameScene : MMWGameScene! = MMWGameScene(size: screenSize!)
var mmwResultsScene : MMWResultsScene! = MMWResultsScene(size: screenSize!, gameResult: false, score: 25)

class MainMenuScene: SKScene {

//    var currentScene : SKScene?
//    var viewSize:CGSize!
    var viewSize = screenSize!
    
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
        print("view size MenuScene: \(screenSize)")

        // add BG
        let backgroundNode = SKSpriteNode(imageNamed: "MarkMyWordBGCleaniPad@2x.png")
        //backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.size = self.frame.size;
        self.addChild(backgroundNode)
        
        let myLabel = SKLabelNode(fontNamed:"Chalkduster")
        myLabel.text = "MainMenuScene" // scene is: \(scene?.description)"
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

                presentMMWScene()
                
                if mmwGameSceneViewController.audioOn == true {
                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
                    runAction(actionSound)
                }
            }
        }
    }

//    func presentMenuScene() {
//        let transition = SKTransition.crossFadeWithDuration(2.0)
//        let menuScene = MenuScene(size: size, gameResult: true, score: 123)
//        view?.presentScene(menuScene, transition: transition)
//        print("presentMenuScene")
//    }
    
//    func createMMWSceneController() -> MMWGameSceneViewController {
//        mmwGameSceneViewController = MMWGameSceneViewController(size: size)
//        return mmwGameSceneViewController
//    }
    
    func presentMMWScene() -> MMWGameScene {
//        let transition = SKTransition.crossFadeWithDuration(0.01)
//        view?.presentScene(mmwGameScene, transition: transition)
        view?.presentScene(mmwGameScene)
        print("presentMMWScene")
        mmwGameSceneViewController.setUpGame()
        return mmwGameScene
    }

    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
}
