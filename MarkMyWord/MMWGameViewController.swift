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

class MMWGameViewController: UIViewController {
    
    var scene: MMWGameSceneView!
    
    
//    var gameView: UIView!
//    var level: Level!
//    
//    
//    //stopwatch variables
//    private var secondsLeft: Int = 0
//    private var timer: NSTimer?
//    
//    private var data = GameData()
//    
//    private var audioController: AudioController
//    
////    init (){
////        audioController = AudioController()
////        audioController.preloadAudioEffects(AudioEffectFiles)
////    }
//
//    required init(coder aDecoder: NSCoder) {
//        //controller = GameController()
//        audioController = AudioController()
//        audioController.preloadAudioEffects(AudioEffectFiles)
//        super.init(coder: aDecoder)
//        fatalError("init(coder:) has not been implemented")
//    }
    
    override func prefersStatusBarHidden() -> Bool {
        return true
    }
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        // 1. Configure the main view
        ///let skView = view as! SKView
        let skView = self.view as! SKView
        skView.showsFPS = true
        //skView.showsPhysics = true
        skView.showsNodeCount = true
        
        // 2. Create and configure our game scene
        scene = MMWGameSceneView(size: skView.bounds.size)
        scene.scaleMode = .AspectFill
        
        // 3. Show the scene.
        skView.presentScene(scene)
        
         //??? Sprite Kit applies additional optimizations to improve rendering performance
        skView.ignoresSiblingOrder = true
//
//        /* Set the scale mode to scale to fit the window */
//        scene.scaleMode = .AspectFill
//
//        skView.presentScene(scene)
    }
    
    override func shouldAutorotate() -> Bool {
        return true
    }
    
    //    override func didReceiveMemoryWarning() {
    //        super.didReceiveMemoryWarning()
    //        // Release any cached data, images, etc that aren't in use.
    //    }
}



//extension SKNode {
//    class func unarchiveFromFile(file : String) -> SKNode? {
//        if let path = NSBundle.mainBundle().pathForResource(file, ofType: "sks") {
//            var sceneData = NSData(contentsOfFile: path, options: .DataReadingMappedIfSafe, error: nil)!
//            var archiver = NSKeyedUnarchiver(forReadingWithData: sceneData)
//            
//            archiver.setClass(self.classForKeyedUnarchiver(), forClassName: "SKScene")
//            let scene = archiver.decodeObjectForKey(NSKeyedArchiveRootObjectKey) as! GameScene
//            archiver.finishDecoding()
//            return scene
//        } else {
//            return nil
//        }
//    }
//}
//
//class GameViewController: UIViewController {
//
//    override func viewDidLoad() {
//        super.viewDidLoad()
//
//        if let scene = GameScene.unarchiveFromFile("GameScene") as? GameScene {
//            // Configure the view.
//            let skView = self.view as! SKView
//            skView.showsFPS = true
//            skView.showsNodeCount = true
//            
//            /* Sprite Kit applies additional optimizations to improve rendering performance */
//            skView.ignoresSiblingOrder = true
//            
//            /* Set the scale mode to scale to fit the window */
//            scene.scaleMode = .AspectFill
//            
//            skView.presentScene(scene)
//        }
//    }
//
//    override func shouldAutorotate() -> Bool {
//        return true
//    }
//
//    override func supportedInterfaceOrientations() -> Int {
//        if UIDevice.currentDevice().userInterfaceIdiom == .Phone {
//            return Int(UIInterfaceOrientationMask.AllButUpsideDown.rawValue)
//        } else {
//            return Int(UIInterfaceOrientationMask.All.rawValue)
//        }
//    }
//
//    override func didReceiveMemoryWarning() {
//        super.didReceiveMemoryWarning()
//        // Release any cached data, images, etc that aren't in use.
//    }
//
//    override func prefersStatusBarHidden() -> Bool {
//        return true
//    }
//}
