//
//  OptionScreen.swift
//  MarkMyWord
//
//  Created by Eric Hook on 1/3/16.
//  Copyright © 2016 Hook Studios. All rights reserved.
//

import UIKit

import Foundation
import UIKit
import SpriteKit

class MMWOptionScreen: SKScene {
    
    var currentScene : SKScene?

    var inputText:UITextField?
    var keyboardOpen:Bool = false
    var keyboardClosingWillSaveToKey:String = ""
    var keyboardType:UIKeyboardType = UIKeyboardType.Alphabet
    var keyboardAppearance:UIKeyboardAppearance = UIKeyboardAppearance.Default
    
    var backgroundNode = SKSpriteNode(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])
//    var backgroundBlackNode = SKSpriteNode(imageNamed: "BGBlack.png")
    var backgroundStripesAll       = SKSpriteNode(imageNamed: "BGStripes.png")

    var defaults:NSUserDefaults = NSUserDefaults.standardUserDefaults()

    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        size = screenSize!
    }
    
    override init(size: CGSize) {
        super.init(size: size)

        mmwGameScene.playAreaWhite.hidden = true
        mmwGameScene.backgroundStripes.hidden = true
        mmwGameScene.gameGrid.hidden = true

        // add BG
        backgroundNode.position = CGPoint(x: screenSize!.width/2, y: screenSize!.height/2)
        backgroundNode.userInteractionEnabled = false
        backgroundNode.size = self.frame.size;
        backgroundNode.zPosition = 1
        if self.childNodeWithName("backgroundNode") == nil {
            addChild(backgroundNode)
        }
        backgroundNode.alpha = 0.5

        backgroundStripesAll.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundStripesAll.position = CGPoint(x: size.width/2.0, y: 0.0)
        backgroundStripesAll.userInteractionEnabled = false
        backgroundStripesAll.name = "backgroundStripes"
        backgroundStripesAll.zPosition = 3
        if self.childNodeWithName("backgroundStripesAll") == nil {
            addChild(backgroundStripesAll)
        }
        backgroundStripesAll.alpha = 0.15

//        // add BGBlack
//        backgroundBlackNode.position = CGPoint(x: screenSize!.width/2, y: screenSize!.height/2)
//        backgroundBlackNode.userInteractionEnabled = false
//        backgroundBlackNode.size = self.frame.size;
//        backgroundBlackNode.zPosition = 5
//        if self.childNodeWithName("backgroundBlackNode") == nil {
//            addChild(backgroundBlackNode)
//        }
//        backgroundBlackNode.alpha = 0.5
        
        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            backgroundStripesAll.xScale = 1.333
            backgroundStripesAll.yScale = 1.333
        }
        
        //gameViewController.updateUIDeluxeVersion()

    }
    
    
    override func didMoveToView(view: SKView) {
        /* Setup your scene here */
        //viewSize = view.bounds.size
        if debugMode == true { print("view size MenuScene: \(screenSize)") }

        delay(0.0){
            gameViewController.updateGameSettings()
        }
        
        backgroundNode.texture = SKTexture(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])
        //gameGridCover.hidden = false

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
        
        mmwGameScene.playAreaWhite.hidden = true
        mmwGameScene.backgroundStripes.hidden = true
        mmwGameScene.gameGrid.hidden = true
        
        //gameViewController.updateUIDeluxeVersion()
        
        // NSNotificationCenter.defaultCenter().addObserver(self, selector: "presentView", name: "showController", object: nil)
    }
    
    func allPlayersPassed () {

        if debugMode == true { print("MMWOptionScreen allPlayersPassed") }
        gameViewController.ViewAllOptionsUI.userInteractionEnabled = true
        gameViewController.ViewAllOptionsUI.hidden = false
        gameViewController.OptionsSwitchOutlet.hidden = true
        
        gameViewController.ViewOptionsUI.hidden = true
        
        gameViewController.viewRulesContainer.hidden = true
        gameViewController.viewStatsContainer.hidden = true
        gameViewController.ViewEndGameUI.hidden = false
        gameViewController.ViewResultsScreenUI.hidden = true
        gameViewController.ViewLoadingGameUIOutlet.hidden = true

    }
    
    func returnToGameScene () {
        if debugMode == true { print("returnToGameScene() return to mmw scene") } //create MMW controller

        gameViewController.ViewAllOptionsUI.hidden = true
        mmwGameScene.playAreaWhite.hidden = false
        mmwGameScene.backgroundStripes.hidden = false
        mmwGameScene.gameGrid.hidden = false
        
        self.view?.presentScene(mmwGameScene)
    }
    
    func returnToGameSceneFinishGame () {
        if debugMode == true { print("returnToGameSceneFinishGame() return to mmw scene") }//create MMW controller

        gameViewController.ViewAllOptionsUI.hidden = true
        mmwGameScene.playAreaWhite.hidden = false
        mmwGameScene.backgroundStripes.hidden = false
        mmwGameScene.gameGrid.hidden = false
        
        self.view?.presentScene(mmwGameScene)
    }
    
    func newGameScene () {
        if debugMode == true { print("going to NEW mmw scene from mmwOptionScreen") } //create MMW controller
        
        let loadingIndicator = SKSpriteNode(imageNamed: "LoadingBarInside.png")
        
        mmwGameScene.foundValidWordOnTurn = true
        
        let loadDelayTimeSecs = 15.0
        
        //gameViewController.ViewAllOptionsUI.hidden = true
        gameViewController.ViewOptionsUI.hidden = true
        gameViewController.ViewLoadingGameUIOutlet.hidden = false
//        gameViewController.bannerViewLowerLeft.hidden = true
//        gameViewController.bannerViewLowerRight.hidden = true
        //newGameSpriteNode.hidden = false
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
        let scaleHoriz = SKAction.scaleXTo(2, duration: loadDelayTimeSecs + 0.2)
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
            
            mmwGameScene.backgroundNode.texture = SKTexture(imageNamed: gameViewController.backgroundImageArray[mmwGameSceneViewController.backgroundNumber])
            
            mmwGameScene.playAreaWhite.hidden = false
            mmwGameScene.backgroundStripes.hidden = false
            mmwGameScene.gameGrid.hidden = false
            
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

