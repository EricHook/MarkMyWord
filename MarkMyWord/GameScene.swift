//
//  GameScene.swift
//  rayTutorialCardGame
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit

class GameScene: SKScene {

    var currentScene : SKScene?
    var mmwGameScene : MMWGameScene?
    var mmwCont : MMWGameSceneViewController?
    
    required init?(coder aDecoder: NSCoder) {
        //currentScene = MenuScene(size: size, gameResult: true, score: 123)
        super.init(coder: aDecoder)
        //mmwGameScene = MMWGameScene(size: view!.bounds.size)
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        //mmwGameScene = MMWGameScene(size: view!.bounds.size)
        //currentScene = MenuScene(size: size, gameResult: true, score: 123)
    }

    override func didMoveToView(view: SKView) {
        /* Setup your scene here */
        
        // add BG
        let backgroundNode = SKSpriteNode(imageNamed: "MeyamaMathMenuScreenBG_1024x768.png")
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: size.width/2.0, y: 0.0)
        backgroundNode.userInteractionEnabled = false
        self.addChild(backgroundNode)
        
        let myLabel = SKLabelNode(fontNamed:"Chalkduster")
        myLabel.text = "Mark My Word" // scene is: \(scene?.description)"
        print("scene in GameScene is: \(scene?.description)")
        myLabel.fontSize = 65;
        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y:CGRectGetMidY(self.frame))
        self.addChild(myLabel)
    }
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
        if userInteractionEnabled {
            let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
            runAction(actionSound)
        }
                print("going to mmw scene")
                mmwGameScene = presentMMWScene()
        
//        mmwGameScene!.player1View?.playerScoreLabel.text = String(8765)
//        
//        mmwGameScene!.player1!.playerName = "Fred"
//        mmwGameScene!.player1View?.playerNameLabel.text = mmwGameScene!.player1!.playerName
//        //mmwGameScene(mmwGameScene.player1View!, player: mmwGameScene.player1!, score: 22)
//        mmwGameScene!.player1View?.playerScoreLabel.text = String(33)
                //mmwGameScene?.tilesRemainingLabel.text = "touchesBegan test"
                //mmwGameScene?.player1View?.playerScoreLabel.text = "TEST"
        
                mmwCont =  createMMWSceneController()
        
                mmwCont!.testMMWCont()
    }
    
    func presentMenuScene() {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        let menuScene = MenuScene(size: size, gameResult: true, score: 123)
        view?.presentScene(menuScene, transition: transition)
        print("presentMenuScene")
    }
    
    func presentMMWScene() -> MMWGameScene {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        mmwGameScene = MMWGameScene(size: size)
        mmwGameScene!.name = "MMWScene instance name made in GameScene"
        currentScene = mmwGameScene
        view?.presentScene(mmwGameScene!, transition: transition)
        print("presentMMWScene")
//        mmwGameScene!.player1View?.playerScoreLabel.text = String(8765)
//        
//        mmwGameScene!.player1!.playerName = "Fred"
//        mmwGameScene!.player1View?.playerNameLabel.text = mmwGameScene!.player1!.playerName
//        //mmwGameScene(mmwGameScene.player1View!, player: mmwGameScene.player1!, score: 22)
//        mmwGameScene!.player1View?.playerScoreLabel.text = String(33)
        return mmwGameScene!
    }

    func createMMWSceneController() -> MMWGameSceneViewController {
        mmwCont = MMWGameSceneViewController()
        return mmwCont!
    }

    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
}
