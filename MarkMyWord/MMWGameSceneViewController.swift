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

class MMWGameSceneViewController { //: UIViewController  {
    
    var mmwGameScene: MMWGameScene!
    var viewSize : CGSize!
    var tileCollection = MMWTileBuilder()
    
    init (size: CGSize) {
        viewSize = size
        mmwGameScene = MMWGameScene(size: viewSize)
    }
    
//    override func prefersStatusBarHidden() -> Bool {
//        return true
//    }
//    
//    func loadMMWGameView() {
//        // 2. Create and configure our game scene
//        mmwGameScene = MMWGameScene(size: self.view.bounds.size)
//        print(mmwGameScene.description)
//        print("mmw scene controller")
//        mmwGameScene.scaleMode = .AspectFill
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
//        mmwGameScene = MMWGameScene(size: view.bounds.size)
//        print(mmwGameScene.description)
//        print("in MMWGmaeSceneViewController viewDidLoad")
//        mmwGameScene.scaleMode = .AspectFill
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
//        mmwGameScene.presentMenuScene()
//        //scene.presentScene(MMWScene, transition: transition)
//        print("presentMMWScene")
//        MMWScene.player1View?.playerNameLabel.text = "Fred"
//        
//        //MMWScene.player1View?.playerNameLabel.text = MMWScene.player1!.playerName
//        //MMWScene.changePlayerScore(MMWScene.player1View!, player: MMWScene.player1!, score: 22)
//        MMWScene.player1View?.playerScoreLabel.text = String(33)
//    }
//    
//    func presentMMWScene() {
//        
//        print("in GameViewController presentMMWScene 1")
//        let mmwScene = MMWGameScene(size: view.bounds.size)
//        
//        // Configure the view.
//        let skView = self.view as! SKView
//        skView.showsFPS = true
//        skView.showsNodeCount = true
//        
//        /* Sprite Kit applies additional optimizations to improve rendering performance */
//        skView.ignoresSiblingOrder = true
//        
//        /* Set the scale mode to scale to fit the window */
//        mmwScene.scaleMode = .AspectFill
//        
//        skView.presentScene(mmwScene)
//        print("in GameViewController presentMMWScene 2")
//        
//        //MMWGameSceneViewController()
//        //presentMMWScene()
//        print("in GameViewController presentMMWScene 3")
//        
//        mmwScene.changePlayerScore(mmwScene.player2View!, player: mmwScene.player2!, score: 2468)
//    }

    func testMMWCont () {
        print("testMMWCont")
        //mmwGameScene.testTile1.tileSprite.letterLabel.text = "%"
        
//        let testTile0 = tileCollection.mmwTileArray[8].tileSprite
//        let testTile1 = tileCollection.mmwTileArray[9].tileSprite
//        let testTile2 = tileCollection.mmwTileArray[10].tileSprite
//        let testTile3 = tileCollection.mmwTileArray[11].tileSprite
//        
//        let tileB1 = tileCollection.tileY1.tileSprite
//        
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(testTile0.letterLabel) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(testTile1.letterLabel) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(testTile2.letterLabel) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(testTile3.letterLabel) ")
//        
////        tileCollection.mmwTileArray[0].letterString = "J"
////        tileCollection.mmwTileArray[0].tileSprite.letterLabel.text = tileCollection.mmwTileArray[0].letterString
//        testTile0.position = CGPoint(x: viewSize.width/2.0, y: 30.0)
//        testTile0.color = UIColorAppleOrange
//        
////        tileCollection.mmwTileArray[1].letterString = "K"
//        testTile1.position = CGPoint(x: viewSize.width/2.0, y: 70.0)
//        testTile1.color = UIColorAppleRed
//        
////        tileCollection.mmwTileArray[2].letterString = "L"
//        testTile2.position = CGPoint(x: viewSize.width/2.0, y: 110.0)
//        testTile2.color = UIColorApplePurple
//
//        testTile3.position = CGPoint(x: viewSize.width/2.0, y: 180.0)
//        testTile3.color = UIColorAppleBlue
//        
//        tileB1.position = CGPoint(x: viewSize.width/2.0, y: 270.0)
//        tileB1.color = UIColorAppleGreen
//
//        mmwGameScene.addChild(testTile0)
//        mmwGameScene.addChild(testTile1)
//        mmwGameScene.addChild(testTile2)
//        mmwGameScene.addChild(testTile3)
//        
//        mmwGameScene.addChild(tileB1)
//        
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(testTile0.letterLabel) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(testTile1.letterLabel) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(testTile2.letterLabel) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(testTile3.letterLabel) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(tileCollection.mmwTileArray[8].letterString) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(tileCollection.mmwTileArray[9].letterString) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(tileCollection.mmwTileArray[10].letterString) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(tileCollection.mmwTileArray[11].letterString) ")
        
//        tileCollection.mmwTileArray.removeAtIndex(8)
//        tileCollection.mmwTileArray.removeAtIndex(9)
//        tileCollection.mmwTileArray.removeAtIndex(10)
//        tileCollection.mmwTileArray.removeAtIndex(11)
//        
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(tileCollection.mmwTileArray[8].letterString) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(tileCollection.mmwTileArray[9].letterString) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(tileCollection.mmwTileArray[10].letterString) ")
//        print("Tile array info size: \(tileCollection.mmwTileArray.count), \(tileCollection.mmwTileArray[11].letterString) ")
//        
//        testTile2.color = UIColorAppleGreen
//        
        
        mmwGameScene.showTiles(tileCollection)
        
        //mmwGameScene.addChild(tileCollection.mmwTileArray[1].tileSprite)
   
        
//        self.testTile1.tileSprite.letterLabel.text = "P"
//        self.testTile1.tileSprite.letterLabel.zPosition = 25
        
        
        //scene1.   .presentMMWScene()
    }
    
    //    override func didReceiveMemoryWarning() {
    //        super.didReceiveMemoryWarning()
    //        // Release any cached data, images, etc that aren't in use.
    //    }
}




