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

var mmwGameSceneViewController = MMWGameSceneViewController()

var gameViewController = GameViewController()

class GameViewController : UIViewController {
    
    weak var parentController: GameViewController?
    
    @IBOutlet weak var menuView: UIView?
    @IBOutlet weak var playButton: UIButton?
    
    
    
    @IBOutlet weak var ViewOptionsUI: UIView!
    
    
    
//    @IBAction func likedThis(sender: UIButton) {
//        //Hide the menu view
//        menuView!.hidden = true
//        
//        //instantiate and present the scene on the main view
//        let scene = MMWGameScene(size: view.bounds.size)
//        let skView = self.view as! SKView
//        skView.presentScene(scene)
//    }
    
    let button   = UIButton(type: UIButtonType.System) as UIButton

    override func viewDidLoad() {
        super.viewDidLoad()
        print("in GameViewController viewDidLoad 1")
        screenSize = view.bounds.size
        let mainMenuScene = MainMenuScene(size: screenSize!)
        
//        mmwGame.setMainMenuScene(menuScene)
//        mmwGame.setScreenSize(view.bounds.size)
        

        
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
        
        print("Screen width:\(screenSize!.width) , device type: \(mmwGame.deviceType) ")
        
        
    //    if let scene1 = GameScene.unarchiveFromFile("GameScene") as? GameScene {
            // Configure the view.
            
        let skView = self.view as! SKView
        skView.showsFPS = true
        skView.showsNodeCount = true
        
        /* Sprite Kit applies additional optimizations to improve rendering performance */
        skView.ignoresSiblingOrder = true

        /* Set the scale mode to scale to fit the window */
        mainMenuScene.scaleMode = .AspectFill
        
/////////////////
//        NSNotificationCenter.defaultCenter().addObserver(self, selector: "presentView", name: "showController", object: nil)
/////////////////////////////////
        
        //scene.parentController = self

        skView.presentScene(mainMenuScene)
        
        
        
        
//        let controller = (self.view.window?.rootViewController)! as! GameViewController //   .view?.window?.rootViewController as! GameViewController
//        controller.buttonAction()
        

        
        
        var cornerView = UIView()
        
        self.view.insertSubview(cornerView, atIndex: 2)
        
        button.frame = CGRectMake(100, 100, 100, 50)
        button.backgroundColor = UIColor.greenColor()
        button.setTitle("Test Button", forState: UIControlState.Normal)
        button.addTarget(self, action: "buttonAction:", forControlEvents: UIControlEvents.TouchUpInside)
        
        cornerView.addSubview(button)
        
        
        gameViewController = self as GameViewController
        gameViewController.buttonAction()
        
        //self.view.addSubview(cornerView)
        
        

    }

    
    
    func buttonAction(sender:UIButton!)
    {
        print("Manual Button tapped")
        
    }
    
    func buttonAction()
    {
        print("TEST Manual Button tapped")
        
    }
    
    @IBOutlet weak var testSwitch: UISegmentedControl!

    @IBAction func testSwitch(sender: AnyObject) {
        print("test worked")
        button.enabled = false
        button.hidden = true
        //testSwitch.tintColor = UIColorApplePurple
        

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
