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

var mmwGameSceneViewController = MMWGameSceneViewController(size: screenSize!)

class GameViewController : UIViewController {
    
    @IBOutlet weak var menuView: UIView?
    @IBOutlet weak var playButton: UIButton?
    
    @IBAction func likedThis(sender: UIButton) {
        //Hide the menu view
        menuView!.hidden = true
        
        //instantiate and present the scene on the main view
        let scene = MMWGameScene(size: view.bounds.size)
        let skView = self.view as! SKView
        skView.presentScene(scene)
    }
    
    

    override func viewDidLoad() {
        super.viewDidLoad()
        print("in GameViewController viewDidLoad 1")
        screenSize = view.bounds.size
        let menuScene = MainMenuScene(size: view.bounds.size)
        
//        mmwGame.setMainMenuScene(menuScene)
//        mmwGame.setScreenSize(view.bounds.size)
        
        print(screenSize!.width)
        
        switch Int(screenSize!.width) {
            
            case 1024: // [1024x768]
                mmwGame.setDeviceType(MMWGame.DeviceType.iPad)
            case 2048: //  [2048 x 1536]
                mmwGame.setDeviceType(MMWGame.DeviceType.iPadRetina)
            case 1366: // [2732 x 2048] --> Width 1366!
                mmwGame.setDeviceType(MMWGame.DeviceType.iPadPro)
            case 568: // [568.0, 320.0]
                mmwGame.setDeviceType(MMWGame.DeviceType.iPhone5)
            case 2732: // [2732 x 2048]
                mmwGame.setDeviceType(MMWGame.DeviceType.iPhone6)
            case 736: // [2732 x 2048]
               mmwGame.setDeviceType(MMWGame.DeviceType.iPhone6Plus)
            default:
                mmwGame.setDeviceType(MMWGame.DeviceType.iPad)
        }
        
        
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
