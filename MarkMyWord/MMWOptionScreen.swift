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
    var loadingIndicator = SKSpriteNode(imageNamed: "PlayButton.png")
    
    
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
        
        

        print("view size MenuScene: \(screenSize)")
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
            
            
            //gameViewController.buttonAction()
            // update controllers to reflect current game data
            
//            // seconds per turn UI
//            switch (mmwGameSceneViewController.secondsPerTurn) {
//            case 20:
//                gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 0
//                print("UI secondsPerTurn = 20")
//            case 30:
//                gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 1
//                print("UI secondsPerTurn = 30")
//            case 45:
//                gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 2
//                print("UI secondsPerTurn = 45")
//            case 60:
//                gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 3
//                print("UI secondsPerTurn = 60")
//            case 999:
//                gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 4
//                print("UI secondsPerTurn = 999 / OFF")
//            default:
//                break;
//            }
//
//            // audio UI
//            if mmwGameSceneViewController.audioOn == true {
//                gameViewController.audioSettingOutlet.selectedSegmentIndex = 1
//            }
//            else {
//                gameViewController.audioSettingOutlet.selectedSegmentIndex = 0
//            }
//            print("UI audioOn = \(mmwGameSceneViewController.audioOn)")
//
//            // minWordSize UI
//            gameViewController.minWordLengthSwitchOutlet.selectedSegmentIndex = mmwGameSceneViewController.minWordSize - 2
//            print("UI minWordSize = \(mmwGameSceneViewController.minWordSize)")
//            
//            // number starter words UI
//            gameViewController.numStarterWordsSwitchOutlet.selectedSegmentIndex = mmwGameSceneViewController.numStarterWords - 1
//            print("UI numStarterWords = \(mmwGameSceneViewController.numStarterWords)")
//            
//            // number of players UI
//            gameViewController.numPlayersSwitchOutlet.selectedSegmentIndex = mmwGameSceneViewController.numPlayers - 2
//            switch (mmwGameSceneViewController.numPlayers - 2){
//            case 0:
//                mmwGameSceneViewController.numPlayers = 2
//                print("UI numPlayers 2")
//                gameViewController.ViewPlayer3UI.alpha = 0.5
//                gameViewController.ViewPlayer3UI.userInteractionEnabled = false
//                gameViewController.ViewPlayer4UI.alpha = 0.5
//                gameViewController.ViewPlayer4UI.userInteractionEnabled = false
//            case 1:
//                mmwGameSceneViewController.numPlayers = 3
//                print("UI numPlayers 3")
//                gameViewController.ViewPlayer3UI.alpha = 1.0
//                gameViewController.ViewPlayer3UI.userInteractionEnabled = true
//                gameViewController.ViewPlayer4UI.alpha = 0.5
//                gameViewController.ViewPlayer4UI.userInteractionEnabled = false
//            case 2:
//                mmwGameSceneViewController.numPlayers = 4
//                print("UI numPlayers 4")
//                gameViewController.ViewPlayer3UI.alpha = 1.0
//                gameViewController.ViewPlayer3UI.userInteractionEnabled = true
//                gameViewController.ViewPlayer4UI.alpha = 1.0
//                gameViewController.ViewPlayer4UI.userInteractionEnabled = true
//            default:
//                break;
//            }
            
//            gameViewController.player1NameLabel.text = mmwGameSceneViewController.playerArray[0].playerName
//            gameViewController.player2NameLabel.text = mmwGameSceneViewController.playerArray[1].playerName
//            gameViewController.player3NameLabel.text = mmwGameSceneViewController.playerArray[2].playerName
//            gameViewController.player4NameLabel.text = mmwGameSceneViewController.playerArray[3].playerName
//            
//            
//            if mmwGameSceneViewController.playerArray[1].isHuman == true {  // Player 1 always human
//                gameViewController.isHumanPlayer2Outlet.selectedSegmentIndex = 0
//                gameViewController.player1ImageOutlet.image = UIImage(named: gameViewController.playerImageArray[mmwGameSceneViewController.playerArray[1].playerAvatarNumber])             }
//            else {
//                gameViewController.isHumanPlayer2Outlet.selectedSegmentIndex = 1
//            }
//            
//            
//            if mmwGameSceneViewController.playerArray[2].isHuman == false { // Player 2 always human
//                gameViewController.isHumanPlayer2Outlet.selectedSegmentIndex = 0
//                gameViewController.player2ImageOutlet.image = UIImage(named: gameViewController.playerImageArray[mmwGameSceneViewController.playerArray[1].playerAvatarNumber])
//            }
//            else {
//                gameViewController.isHumanPlayer2Outlet.selectedSegmentIndex = 1
//                gameViewController.player2ImageOutlet.image = UIImage(named: gameViewController.playerImageArray[mmwGameSceneViewController.playerArray[1].playerAvatarNumber])
//            }

            
            
//            for player in mmwGameSceneViewController.playerArray {
//                if player.isHuman == false {
//                    gameViewController.isHumanPlayer2Outlet.selectedSegmentIndex = 0
//                    
//                } else {
//                    gameViewController.isHumanPlayer2Outlet.selectedSegmentIndex = 1
//                }
//            }
            
//            if mmwGameSceneViewController.playerArray[0].isHuman {
////                gameViewController.player1ImageOutlet.image = gameViewController.playerImageArray[mmwGameSceneViewController.playerArray[0].playerAvatarNumber]
//            }
            
                
                
            
            
            
        }

        gameViewController.ViewAllOptionsUI.hidden = false

        //gameViewController.GameViewControllerUI.hidden = false   //ViewOptionsUI.hidden = false

        //openKeyboardToSaveValue("Player")
        //presentMMWScene()
        //mmwGameSceneViewController = createMMWSceneController()
        //presentMMWScene()
        
        // NSNotificationCenter.defaultCenter().addObserver(self, selector: "presentView", name: "showController", object: nil)
    }
    
    func returnToGameScene () {
        print("return to mmw scene") //create MMW controller
        //presentMMWScene()
        
        gameViewController.ViewAllOptionsUI.hidden = true
//        if mmwGameSceneViewController.timerIsOn {
//            mmwGameScene.startTimer()
//        }
        view?.presentScene(mmwGameScene)
    }
    
    func newGameScene () {
        print("going to NEW mmw scene from mmwOptionScreen") //create MMW controller
        
        mmwGameScene.foundValidWordOnTurn = true
        
        let loadDelayTimeSecs = 20.0
        
        gameViewController.ViewAllOptionsUI.hidden = true
        newGameSpriteNode.hidden = false
        
        loadingIndicator.position = CGPoint(x: screenSize!.width/2, y: screenSize!.height/5)
        loadingIndicator.size = CGSize.init(width: 100, height: 50)
        loadingIndicator.anchorPoint = CGPoint(x: 0, y: 0)
        loadingIndicator.hidden = false
        loadingIndicator.name = "loadingBtn"
        loadingIndicator.zPosition = 100
        self.addChild(loadingIndicator)
        let scaleHoriz = SKAction.scaleXTo(2, duration: loadDelayTimeSecs)
        let loadingAnim = SKAction.group([scaleHoriz])
        let loadingAnimSequence = SKAction.sequence([loadingAnim, SKAction.removeFromParent()])
        loadingIndicator.runAction(loadingAnimSequence)
        
        delay (loadDelayTimeSecs) {
            gameViewController.ViewAllOptionsUI.hidden = true

            mmwGameScene.resetGameView()   // includes tileCollection?.resetAllTiles()
            self.view?.presentScene(mmwGameScene)
            mmwGameScene.startGame()  // set grids, starter words, ...
            
        }
    }

    
//    func buttonAction(sender:UIButton!)
//    {
//        print("Manual Button tapped sender:UIButton!")
//    }
//    
//    func buttonAction()
//    {
//        print("TEST Manual Button tapped ()")
//    }
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        /* Called when a touch begins */
        for touch: AnyObject in touches {
            let location = touch.locationInNode(self)
            let _node:SKNode = self.nodeAtPoint(location)
//            if(_node.name == "playBtn"){
//                
//                //returnToGameScene()
//                
//                newGameScene()
//            }
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

