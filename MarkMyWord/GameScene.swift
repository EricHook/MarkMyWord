//
//  GameScene.swift
//  rayTutorialCardGame
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit



//struct currentGameSceneStruct {
//    static var currentGameScene : SKScene = GameViewController.
//}

class GameScene: SKScene {
    
    
//    func getCurrentGameScene () {
//        return currentGameScene
//    }
//    
//    func setCurrentGameScene (newGameScene : SKScene) {
//        currentGameScene = newGameScene
//    }
    
//    var currentScene : SKScene
//    
//    override init() {
//        currentScene = self.scene
//        super.init()
//    }
//
//    required init?(coder aDecoder: NSCoder) {
//        fatalError("init(coder:) has not been implemented")
//    }
    
    
    
    override func didMoveToView(view: SKView) {
        /* Setup your scene here */
        
        // add BG
        let backgroundNode = SKSpriteNode(imageNamed: "MeyamaMathMenuScreenBG_1024x768.png")
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: size.width/2.0, y: 0.0)
        //        // set scale for various platforms
        //        backgroundNode?.xScale = 0.5
        //        backgroundNode?.yScale = 0.5
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
//        if userInteractionEnabled {
//            let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//            runAction(actionSound)
//            
            //for touch in (touches as Set<UITouch>) {
                //let location = touch.locationInNode(self)
                //let touchedNode = nodeAtPoint(location)
//                touchedNode.zPosition = 15
//                let liftUp = SKAction.scaleTo(1.2, duration: 0.2)
//                touchedNode.runAction(liftUp, withKey: "pickup")
//                
//    //            let sprite = SKSpriteNode(imageNamed:"Spaceship")
//    //            sprite.xScale = 0.5
//    //            sprite.yScale = 0.5
//    //            sprite.position = location
//    //            let action = SKAction.rotateByAngle(CGFloat(M_PI), duration:1)
//    //            sprite.runAction(SKAction.repeatActionForever(action))
//    //            self.addChild(sprite)
                

                print("going to mmw scene")
                presentMMWScene()
//                println("going to mmw controller")
//                var mmwGameController = MMWGameSceneViewController()
//                mmwGameController.loadMMWGameView()
//                println("going to menu scene")
//                playMenuScene()
            //}
//        }
    }
//
//    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
//        for touch in (touches as! Set<UITouch>) {
//            if userInteractionEnabled {
//                let location = touch.locationInNode(self)
//                let touchedNode = nodeAtPoint(location)
//                touchedNode.position = location
//            }
//        
//        }
//    }
//
//    override func touchesEnded(touches: Set<NSObject>, withEvent event: UIEvent) {
//        for touch in (touches as! Set<UITouch>) {
//            let location = touch.locationInNode(self)
//            let touchedNode = nodeAtPoint(location)
//            touchedNode.zPosition = 0
//            let dropDown = SKAction.scaleTo(1.0, duration: 0.2)
//            touchedNode.runAction(dropDown, withKey: "drop")
//        }
//    }
    
    func presentMenuScene() {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        let menuScene = MenuScene(size: size,
            gameResult: true,
            score: 123)
        //let skView = view
        view?.presentScene(menuScene, transition: transition)
    }
    
    func presentMMWScene() {
        let transition = SKTransition.crossFadeWithDuration(2.0)
        let MMWScene = MMWGameScene(size: size)
        MMWScene.name = "MMWScene instance name made in GameScene"
        //var currentScene = MMWScene
        view?.presentScene(MMWScene, transition: transition)
        print("in mmw controller")
        
//        var MMWsceneController = MMWGameSceneViewController()
//        MMWsceneController.loadMMWGameView()
    }
    
    func getTestNum () -> (Int) {
        let num = 22
        return num
    }
   
    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
}
