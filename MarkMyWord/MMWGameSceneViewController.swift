//
//  GameViewController.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/10/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit
import CoreMotion

class MMWGameSceneViewController: UIViewController  {
    
    var scene: MMWGameScene!
    
//    required init(coder aDecoder: NSCoder) {
//        fatalError("init(coder:) has not been implemented")
//    }
//    
//    override init () {
//        super.init()
//    }
    
//    override func prefersStatusBarHidden() -> Bool {
//        return true
//    }
//    
//    func loadMMWGameView() {
//        // 2. Create and configure our game scene
//        scene = MMWGameScene(size: self.view.bounds.size)
//        print(scene.description)
//        print("mmw scene controller")
//        scene.scaleMode = .AspectFill
//    }
//    
//    override func viewDidLoad() {
//        
//        super.viewDidLoad()
//        
//        // 1. Configure the main view
//        ///let skView = view as! SKView
//        let skView = self.view as! SKView
//        skView.showsFPS = true
//        //skView.showsPhysics = true
//        skView.showsNodeCount = true
//        
//        // 2. Create and configure our game scene
//        scene = MMWGameScene(size: view.bounds.size)
//        print(scene.description)
//        print("mmw scene controller")
//        scene.scaleMode = .AspectFill
//        
//        
//        
//        // add PlayerView(s) to background
//        //let player1 : MMWPlayer = MMWPlayer(_playerID: 11, _playerName: "FredPlayer2")
//
//        //player1.playerSeat.rackSize = 6
////        scene.addPlayerView(1, mmwPlayer: player1)
////        scene.topDisplayHUD2("222")
////        scene.topDisplayHUD("CONTROLLER")
////        self.scene.topDisplayHUD("CONTROLLERself")
////        scene.testOutput()
//   
//    }
//    
//    override func shouldAutorotate() -> Bool {
//        return true
//    }
    
//    func presentMMWScene() {
//
//        let transition = SKTransition.crossFadeWithDuration(2.0)
//        let MMWScene = MMWGameScene(size: view.bounds.size)
//        MMWScene.name = "MMWScene instance name made in GameScene"
//        //var currentScene = MMWScene
//        scene.presentMenuScene()
//        //scene.presentScene(MMWScene, transition: transition)
//        print("presentMMWScene")
//        MMWScene.player1View?.playerNameLabel.text = "Fred"
//        
//        //MMWScene.player1View?.playerNameLabel.text = MMWScene.player1!.playerName
//        //MMWScene.changePlayerScore(MMWScene.player1View!, player: MMWScene.player1!, score: 22)
//        MMWScene.player1View?.playerScoreLabel.text = String(33)
//    }
    
    func presentMMWScene() {
        
        print("in GameViewController presentMMWScene 1")
        let mmwScene = MMWGameScene(size: view.bounds.size)
        
        // Configure the view.
        let skView = self.view as! SKView
        skView.showsFPS = true
        skView.showsNodeCount = true
        
        /* Sprite Kit applies additional optimizations to improve rendering performance */
        skView.ignoresSiblingOrder = true
        
        /* Set the scale mode to scale to fit the window */
        mmwScene.scaleMode = .AspectFill
        
        skView.presentScene(mmwScene)
        print("in GameViewController presentMMWScene 2")
        
        //MMWGameSceneViewController()
        //presentMMWScene()
        print("in GameViewController presentMMWScene 3")
        
        mmwScene.changePlayerScore(mmwScene.player2View!, player: mmwScene.player2!, score: 2468)
    }

    func testMMWCont () {
        print("testMMWCont")
        
        //scene1.   .presentMMWScene()
    }
    
    //    override func didReceiveMemoryWarning() {
    //        super.didReceiveMemoryWarning()
    //        // Release any cached data, images, etc that aren't in use.
    //    }
}




