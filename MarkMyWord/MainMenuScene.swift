//
//  GameScene.swift
//  rayTutorialCardGame
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit


//var endGameScene : EndGameScene = EndGameScene(size: screenSize!)

let backgroundNode = SKSpriteNode(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])

let myLabel = SKLabelNode(fontNamed:"Chalkduster")
//let playBtn = SKSpriteNode(imageNamed: "PlayButton.png")

class MainMenuScene: SKScene {

//    var currentScene : SKScene?
//    var viewSize:CGSize!
    var viewSize = screenSize!
    
    var waiting = true
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        size = viewSize

        
    }

    
    override init(size: CGSize) {
        super.init(size: size)
            //screenSize = view!.bounds.size
        
//        mmwOptionScreen  = MMWOptionScreen(size: screenSize!)
//        mmwGameScene     = MMWGameScene(size: screenSize!)
    }
    
    
    
//    var myTimer = NSTimer()
//    
//    func startTimer() {
//        myTimer = NSTimer.scheduledTimerWithTimeInterval(2, target: self, selector: "myFunction", userInfo: nil, repeats: false)
//    }
//    
//    func myFunction() {
//        myTimer.invalidate()
//        presentMMWScene()
//        //do other stuff
//    }
//    //the selector is "myFunction", this will be the name of the function that you wish to call every time the timer reaches its specified intervl
//    //the interval in this case is 10 seconds. In my experience NSTimer is good down to the second but is not precise enough beyond that
//    //repeats: true ... this will tell the timer to repeat its action consistently firing the selector each time the given time interval is reached. If repeat is set to false then the timer only fires once
//    //use myTimer.invalidate to stop the timer and to stop calling the selector.
    


    override func didMoveToView(view: SKView) {
        /* Setup your scene here */

        screenSize = view.bounds.size
//        print("view size MenuScene: \(screenSize)")

        // add BG
        //let backgroundNode = SKSpriteNode(imageNamed: "MarkMyWordBGCleaniPad.png")
        //backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.size = self.frame.size;
        self.addChild(backgroundNode)

        //let myLabel = SKLabelNode(fontNamed:"Chalkduster")
        myLabel.text = "Mark My Word" // scene is: \(scene?.description)"
        myLabel.fontSize = 65;
        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: (CGRectGetMidY(self.frame) - 50) )
        self.addChild(myLabel)
        myLabel.zPosition = 100

//        //let playBtn = SKSpriteNode(imageNamed: "PlayButton.png")
//        playBtn.position = CGPoint(x: viewSize.width/2, y: viewSize.height/3)
//        self.addChild(playBtn)
//        playBtn.name = "playBtn"
//        playBtn.zPosition = 100

        
        //startTimer()
        
        
//        //var waiting = true
//        print("waiting false A")
//       
//
//        var _ = NSTimer.scheduledTimerWithTimeInterval(1, target: self, selector: #selector(MainMenuScene.waitingFalse),
//                            userInfo: nil, repeats: false)
//        
////        var timer = NSTimer.scheduledTimerWithTimeInterval(1, target:self, selector: Selector("waitingFalse"), userInfo: nil, repeats: true)
//
//        while waiting == true {
//            
//        }
//        
//        waiting = true
//        
//        presentMMWScene()

        
        delay(2){
            self.presentMMWScene()

        }
        
    }
    
    
    func waitingFalse () {
        print("waiting false")
        waiting = false
    }
    
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
//        for touch: AnyObject in touches {
//            let location = touch.locationInNode(self)
//            let _node:SKNode = self.nodeAtPoint(location)
//            
//            if(_node.name == "playBtn"){
//                if debugMode == true { print("going to mmw scene") } //create MMW controller
//                playBtn.alpha = 0.1
//                
//                presentMMWScene()
//                
//                if mmwGameSceneViewController.audioOn == true {
//                    let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
//                    runAction(actionSound)
//                }
//            }
//        }
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
        if debugMode == true { print("presentMMWScene") }
        print("presentMMWScene")
        mmwGameSceneViewController.setUpGame()
        
        mmwGameScene.startDelayTimer(3)
        
        return mmwGameScene
    }
    
    
    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
    
    deinit {
        print("MainMenuScene is being deinitialized")
    }
}
