//
//  OptionScreen.swift
//  MarkMyWord
//
//  Created by Eric Hook on 1/3/16.
//  Copyright © 2016 Hook Studios. All rights reserved.
//

import UIKit

import Foundation
import SpriteKit

class MMWOptionScreen: SKScene {
    
//    let gameGridTex = SKTexture(imageNamed: "GameGrid.png")
//    let OptButtonWordLength2Tex = SKTexture(imageNamed: "OptButtonWordLength2.png")
//    let OptButtonWordLength2BoxedTex = SKTexture(imageNamed: "OptButtonWordLength2Boxed.png")
//    let OptButtonWordLength3Tex = SKTexture(imageNamed: "OptButtonWordLength3.png")
//    let OptButtonWordLength3BoxedTex = SKTexture(imageNamed: "OptButtonWordLength3Boxed.png")
    
    var currentScene : SKScene?

    var inputText:UITextField?
    var keyboardOpen:Bool = false
    var keyboardClosingWillSaveToKey:String = ""
    var keyboardType:UIKeyboardType = UIKeyboardType.Alphabet
    var keyboardAppearance:UIKeyboardAppearance = UIKeyboardAppearance.Default
    
 //   var backgroundNode : SKSpriteNode
//    var newGameSpriteNode : SKSpriteNode
    var backgroundNode = SKSpriteNode(imageNamed: "MMWOptionsScreen.png")
    var newGameSpriteNode = SKSpriteNode(imageNamed: "NewGameScreen.png")


    var defaults:NSUserDefaults = NSUserDefaults.standardUserDefaults()

    //var viewSize:CGSize!
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        size = screenSize!
    }
    
    override init(size: CGSize) {
        super.init(size: size)

        // add BG
        backgroundNode.position = CGPoint(x: screenSize!.width/2, y: screenSize!.height/2)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.size = self.frame.size;
        self.addChild(backgroundNode)

//        newGameSpriteNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        newGameSpriteNode.position = CGPoint(x: size.width/2.0, y: 0.0)
//        newGameSpriteNode.userInteractionEnabled = false
//        newGameSpriteNode.hidden = true
//        newGameSpriteNode.zPosition = 100
//        newGameSpriteNode.size.height = screenSize!.height
//        newGameSpriteNode.size.width  = screenSize!.width
//        newGameSpriteNode.alpha = 0.75
//        addChild(newGameSpriteNode)

        //        // Placeholder for dictionary data loading progress display
        //        let playBtnTEMP = SKSpriteNode(imageNamed: "PlayButton.png")
        //        playBtnTEMP.position = CGPoint(x: viewSize.width/4, y: viewSize.height/6)
        //        self.addChild(playBtnTEMP)
        //        playBtnTEMP.name = "playBtnTEMP"
        //        playBtnTEMP.zPosition = 100
        //        playBtnTEMP.anchorPoint = CGPointMake(0, 0)
        //        let scaleHoriz = SKAction.scaleXTo(2, duration: 3.0)
        //        let loadingAnim = SKAction.group([scaleHoriz])
        //        let loadingAnimSequence = SKAction.sequence([loadingAnim, SKAction.removeFromParent()])
        //        playBtnTEMP.runAction(loadingAnimSequence)
        
        
//        loadingIndicator.position = CGPoint(x: screenSize!.width/2, y: screenSize!.height/10)
//        loadingIndicator.size = CGSize.init(width: 100, height: 50)
//        loadingIndicator.hidden = true
//        loadingIndicator.name = "loadingBtn"
//        loadingIndicator.zPosition = 100
//        self.addChild(loadingIndicator)

        //        let myLabel = SKLabelNode(fontNamed: "")
        //        myLabel.text = "Option Screen" // scene is: \(scene?.description)"
        //        myLabel.fontSize = 65;
        //        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: (CGRectGetMidY(self.frame) - 50) )
        //        self.addChild(myLabel)
        //        myLabel.zPosition = 100
        
//        let playBtn = SKSpriteNode(imageNamed: "PlayButton.png")
//        playBtn.position = CGPoint(x: screenSize!.width/2, y: screenSize!.height/10)
//        playBtn.size = CGSize.init(width: 100, height: 50)
//        self.addChild(playBtn)
//        playBtn.name = "playBtn"
//        playBtn.zPosition = 98
        
    }
    
    
    override func didMoveToView(view: SKView) {
        /* Setup your scene here */
        //viewSize = view.bounds.size
        if debugMode == true { print("view size MenuScene: \(screenSize)") }
        newGameSpriteNode.hidden = true

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

//        var backgroundNode = SKSpriteNode(imageNamed: "MMWOptionsScreen.png")
//
//        // add BG
//        backgroundNode.position = CGPoint(x: screenSize!.width/2, y: screenSize!.height/2)
//        backgroundNode.userInteractionEnabled = false
//        backgroundNode.size = self.frame.size;
//        self.addChild(backgroundNode)
//
//        newGameSpriteNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        newGameSpriteNode.position = CGPoint(x: size.width/2.0, y: 0.0)
//        newGameSpriteNode.userInteractionEnabled = false
//        newGameSpriteNode.hidden = true
//        newGameSpriteNode.zPosition = 100
//        newGameSpriteNode.size.height = screenSize!.height
//        newGameSpriteNode.size.width  = screenSize!.width
//        addChild(newGameSpriteNode)
//        
////        let myLabel = SKLabelNode(fontNamed: "")
////        myLabel.text = "Option Screen" // scene is: \(scene?.description)"
////        myLabel.fontSize = 65;
////        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y: (CGRectGetMidY(self.frame) - 50) )
////        self.addChild(myLabel)
////        myLabel.zPosition = 100

        delay(0.0){
            gameViewController.updateGameSettings()
        }

        if debugMode == true { print("options button pressed") }
        if debugMode == true { print("MMWOptionScreen didMoveToView") }
        gameViewController.ViewAllOptionsUI.userInteractionEnabled = true
        gameViewController.ViewAllOptionsUI.hidden = false
        gameViewController.OptionsSwitchOutlet.hidden = false
        
        gameViewController.ViewOptionsUI.hidden = false
        gameViewController.returnToGameButtonOutlet.alpha = 1.0
        gameViewController.returnToGameButtonOutlet.userInteractionEnabled = true
        
        gameViewController.viewRulesContainer.hidden = true
        gameViewController.viewStatsContainer.hidden = true
        gameViewController.ViewEndGameUI.hidden = true
        gameViewController.ViewResultsScreenUI.hidden = true
        gameViewController.ViewLoadingGameUIOutlet.hidden = true

        //gameViewController.GameViewControllerUI.hidden = false   //ViewOptionsUI.hidden = false

        //openKeyboardToSaveValue("Player")
        //presentMMWScene()
        //mmwGameSceneViewController = createMMWSceneController()
        //presentMMWScene()
        
        // NSNotificationCenter.defaultCenter().addObserver(self, selector: "presentView", name: "showController", object: nil)
    }
    
    func allPlayersPassed () {
        
        
        
        //mmwGameScene.stopTimer()
        
        
        
        if debugMode == true { print("MMWOptionScreen allPlayersPassed") }
        gameViewController.ViewAllOptionsUI.userInteractionEnabled = true
        gameViewController.ViewAllOptionsUI.hidden = false
        gameViewController.OptionsSwitchOutlet.hidden = true
        
        gameViewController.ViewOptionsUI.hidden = true
//        gameViewController.returnToGameButtonOutlet.alpha = 1.0
//        gameViewController.returnToGameButtonOutlet.userInteractionEnabled = true
        
        gameViewController.viewRulesContainer.hidden = true
        gameViewController.viewStatsContainer.hidden = true
        gameViewController.ViewEndGameUI.hidden = false
        gameViewController.ViewResultsScreenUI.hidden = true
        gameViewController.ViewLoadingGameUIOutlet.hidden = true

    }
    
    func returnToGameScene () {
        if debugMode == true { print("return to mmw scene") } //create MMW controller

//        if mmwGameSceneViewController.timerIsOn {
//            mmwGameScene.startTimer()
//        }

        gameViewController.ViewAllOptionsUI.hidden = true
        self.view?.presentScene(mmwGameScene)
        
        //mmwGameScene.startTimer(mmwGameScene.secondsLeft)
        //delay(1){
            //mmwGameScene.changePlayerTurn()
        //}
    }
    
    func returnToGameSceneFinishGame () {
        if debugMode == true { print("return to mmw scene") }//create MMW controller
        
        //        if mmwGameSceneViewController.timerIsOn {
        //            mmwGameScene.startTimer()
        //        }
        
        gameViewController.ViewAllOptionsUI.hidden = true
        self.view?.presentScene(mmwGameScene)
        
        //mmwGameScene.startTimer(mmwGameScene.secondsLeft)
        //delay(1){
        //mmwGameScene.changePlayerTurn()
        //}
    }
    
    func newGameScene () {
        if debugMode == true { print("going to NEW mmw scene from mmwOptionScreen") } //create MMW controller
        
        let loadingIndicator = SKSpriteNode(imageNamed: "LoadingBarInside.png")
        
        mmwGameScene.foundValidWordOnTurn = true
        
        let loadDelayTimeSecs = 17.0
        
        //gameViewController.ViewAllOptionsUI.hidden = true
        gameViewController.ViewOptionsUI.hidden = true
        gameViewController.ViewLoadingGameUIOutlet.hidden = false
//        gameViewController.bannerViewLowerLeft.hidden = true
//        gameViewController.bannerViewLowerRight.hidden = true
        newGameSpriteNode.hidden = false
        gameViewController.OptionsSwitchOutlet.hidden = true
        gameViewController.ViewResultsScreenUI.hidden = true
        
        loadingIndicator.position = CGPoint(x: screenSize!.width/2 - 150, y: screenSize!.height/2 - 12.5)
        loadingIndicator.size = CGSize.init(width: 150, height: 25)
        loadingIndicator.anchorPoint = CGPoint(x: 0, y: 0)
        loadingIndicator.hidden = false
        loadingIndicator.name = "loadingBtn"
        loadingIndicator.zPosition = 100
        self.addChild(loadingIndicator)
        loadingIndicator.runAction(SKAction.scaleXTo(0, duration: 0.0))
        let scaleHoriz = SKAction.scaleXTo(2, duration: loadDelayTimeSecs + 0.5)
        let loadingAnim = SKAction.group([scaleHoriz])
        let loadingAnimSequence = SKAction.sequence([loadingAnim, SKAction.removeFromParent()])
        loadingIndicator.runAction(loadingAnimSequence)
        loadingIndicator.runAction(SKAction.scaleXTo(1, duration: 0.0))
        gameViewController.ViewLoadingGameUIOutlet.hidden = false

        delay (loadDelayTimeSecs) {
            gameViewController.ViewLoadingGameUIOutlet.hidden = true
            gameViewController.ViewAllOptionsUI.hidden = true
            gameViewController.OptionsSwitchOutlet.hidden = false
//            gameViewController.bannerViewLowerLeft.hidden = false
//            gameViewController.bannerViewLowerRight.hidden = false
            mmwGameScene.resetGameView()   // includes tileCollection?.resetAllTiles()
            self.view?.presentScene(mmwGameScene)
            
            mmwGameScene.playButton.hidden = false
            
            //mmwGameScene.startGame()  // set grids, starter words, ...
            
            //mmwGameScene.startTimer(mmwGameSceneViewController.secondsPerTurn)
        }
    }

    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
//        for touch: AnyObject in touches {
//            let location = touch.locationInNode(self)
//            let _node:SKNode = self.nodeAtPoint(location)
////            if(_node.name == "playBtn"){
////                //returnToGameScene()
////                newGameScene()
////            }
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
//    func presentMMWScene(){
//        gameViewController.ViewAllOptionsUI.hidden = true
//        view?.presentScene(mmwGameScene)
//    }
    
    override func update(currentTime: CFTimeInterval) {
        /* Called before each frame is rendered */
    }
    
    
    
       
//    func openKeyboardToSaveValue(value:String){
//        if keyboardOpen == false {
//            keyboardOpen = true
//            keyboardClosingWillSaveToKey =
//        }
//    }
}

