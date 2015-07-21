//
//  GameViewController.swift
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import UIKit
import SpriteKit

extension SKNode {
    class func unarchiveFromFile(file : String) -> SKNode? {
        if let path = NSBundle.mainBundle().pathForResource(file, ofType: "sks") {
            let sceneData = NSData(contentsOfFile: path)!
            let archiver = NSKeyedUnarchiver(forReadingWithData: sceneData)
            
            archiver.setClass(self.classForKeyedUnarchiver(), forClassName: "SKScene")
            let scene1 = archiver.decodeObjectForKey(NSKeyedArchiveRootObjectKey) as! MainMenuScene
            
            scene1.name = "scene in GameViewController"
            print("scene is: \(scene1.description)")
            
            archiver.finishDecoding()
            return scene1
        } else {
            return nil
        }
    }
}

class GameViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        print("in GameViewController viewDidLoad 1")
        
        let menuScene = MainMenuScene(size: view.bounds.size)
        
    //    if let scene1 = GameScene.unarchiveFromFile("GameScene") as? GameScene {
            // Configure the view.
            
            let skView = self.view as! SKView
            skView.showsFPS = true
            skView.showsNodeCount = true
            
            /* Sprite Kit applies additional optimizations to improve rendering performance */
            skView.ignoresSiblingOrder = true
            
            /* Set the scale mode to scale to fit the window */
            menuScene.scaleMode = .AspectFill

            skView.presentScene(menuScene)
    }
    
//    func presentMMWScene() {
//
//            print("in GameViewController presentMMWScene 1")
//            let mmwScene = MMWGameScene(size: view.bounds.size)
//        
//            // Configure the view.
//            let skView = self.view as! SKView
//            skView.showsFPS = true
//            skView.showsNodeCount = true
//            
//            /* Sprite Kit applies additional optimizations to improve rendering performance */
//            skView.ignoresSiblingOrder = true
//            
//            /* Set the scale mode to scale to fit the window */
//            mmwScene.scaleMode = .AspectFill
//            
//            skView.presentScene(mmwScene)
//            print("in GameViewController presentMMWScene 2")
//            
//            //MMWGameSceneViewController()
//            presentMMWScene()
//            print("in GameViewController presentMMWScene 3")
//        
//    }
    
//    func loadMenu() {
//        print("in GameViewController loadMenu 1")
////        let menuScene = SKScene(fileNamed: MenuScene(size: size, gameResult: false, score: 0)  )
//        if let scene = MenuScene.unarchiveFromFile("MenuScene") as? MenuScene {
//            // Configure the view.
//            let skView = self.view as! SKView
//            skView.showsFPS = true
//            skView.showsNodeCount = true
//            
//            /* Sprite Kit applies additional optimizations to improve rendering performance */
//            skView.ignoresSiblingOrder = true
//            
//            /* Set the scale mode to scale to fit the window */
//            scene.scaleMode = .ResizeFill
//            
//            skView.presentScene(scene)
//            print("in GameViewController loadMenu 2")
//        }
//        print("in GameViewController loadMenu 3")
//    }

    override func shouldAutorotate() -> Bool {
        return true
    }
    
    override func supportedInterfaceOrientations() -> UIInterfaceOrientationMask {
        if UIDevice.currentDevice().userInterfaceIdiom == .Phone {
            return UIInterfaceOrientationMask.AllButUpsideDown
        } else {
            return UIInterfaceOrientationMask.All
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Release any cached data, images, etc that aren't in use.
    }

    override func prefersStatusBarHidden() -> Bool {
        return true
    }
}
