////
////  RulesViewController.swift
////  MarkMyWord
////
////  Created by Eric Hook on 1/26/16.
////  Copyright © 2016 Hook Studios. All rights reserved.
////
//
//import Foundation
//
//import UIKit
//import SpriteKit
//
////extension SKNode {
////    class func unarchiveFromFile(file : String) -> SKNode? {
////        if let path = NSBundle.mainBundle().pathForResource(file, ofType: "sks") {
////            let sceneData = NSData(contentsOfFile: path)!
////            let archiver = NSKeyedUnarchiver(forReadingWithData: sceneData)
////            
////            archiver.setClass(self.classForKeyedUnarchiver(), forClassName: "SKScene")
////            let scene1 = archiver.decodeObjectForKey(NSKeyedArchiveRootObjectKey) as! MainMenuScene
////            
////            scene1.name = "scene in GameViewController"
////            print("scene is: \(scene1.description)")
////            
////            archiver.finishDecoding()
////            return scene1
////        }
////        return nil
////    }
////}
//
//
//class RulesViewController : UIViewController {
//
//    let button   = UIButton(type: UIButtonType.System) as UIButton
//    
//    override func viewDidLoad() {
//        super.viewDidLoad()
//        print("in GameViewController viewDidLoad")
//        screenSize = view.bounds.size
//        let mainMenuScene = MainMenuScene(size: screenSize!)
//        
//        switch Int(screenSize!.width) {
//        case 1024: // [1024x768]
//            mmwGame.setDeviceType(MMWGame.DeviceType.iPad)
//        case 2048: //  [2048 x 1536]
//            mmwGame.setDeviceType(MMWGame.DeviceType.iPadRetina)
//        case 1366: // [2732 x 2048] --> Width 1366!
//            mmwGame.setDeviceType(MMWGame.DeviceType.iPadPro)
//        case 568: // [568.0, 320.0]
//            mmwGame.setDeviceType(MMWGame.DeviceType.iPhone5)
//        case 2732: // [2732 x 2048]
//            mmwGame.setDeviceType(MMWGame.DeviceType.iPhone6)
//        case 736: // [2732 x 2048]
//            mmwGame.setDeviceType(MMWGame.DeviceType.iPhone6Plus)
//        default:
//            mmwGame.setDeviceType(MMWGame.DeviceType.iPad)
//            print("Screen width:\(screenSize!.width) , device type: \(mmwGame.deviceType) ")
//        }
//        
//        let skView = self.view as! SKView
//        skView.showsFPS = true
//        skView.showsNodeCount = true
//        
//        /* Sprite Kit applies additional optimizations to improve rendering performance */
//        skView.ignoresSiblingOrder = true
//        
//        /* Set the scale mode to scale to fit the window */
//        mainMenuScene.scaleMode = .AspectFill
//        
//        /////////////////
//        //        NSNotificationCenter.defaultCenter().addObserver(self, selector: "presentView", name: "showController", object: nil)
//        /////////////////////////////////
//        
//        //scene.parentController = self
//        
//        skView.presentScene(mainMenuScene)
//        
//        
//        //var cornerView = UIView()
//        
//        //self.view.insertSubview(cornerView, atIndex: 2)
//        
//        //        button.frame = CGRectMake(100, 100, 100, 50)
//        //        button.backgroundColor = UIColor.greenColor()
//        //        button.setTitle("Test Button", forState: UIControlState.Normal)
//        //        button.addTarget(self, action: "buttonAction:", forControlEvents: UIControlEvents.TouchUpInside)
//        
//        //cornerView.addSubview(button)
//        
//        //rulesViewController = self as RulesViewController
//        //rulesViewController.buttonAction()
//        
//        //self.view.addSubview(cornerView)
//    }
//    
//    
//    
//    func buttonAction(sender:UIButton!)
//    {
//        print("Manual Button tapped")
//        
//    }
//    
//    func buttonAction()
//    {
//        print("TEST Manual Button tapped")
//    }
//    
//    
//    //            selectedSegmentIndex  .sendActionsForControlEvents(.TouchUpInside)
//    //}
//    
//    override func shouldAutorotate() -> Bool {
//        return true
//    }
//    
//    
//    override func supportedInterfaceOrientations() -> UIInterfaceOrientationMask {
//        if UIDevice.currentDevice().userInterfaceIdiom == .Phone {
//            return UIInterfaceOrientationMask.AllButUpsideDown
//        } else {
//            return UIInterfaceOrientationMask.All
//        }
//    }
//    
//    
//    override func didReceiveMemoryWarning() {
//        super.didReceiveMemoryWarning()
//        // Release any cached data, images, etc that aren't in use.
//    }
//    
//    
//    override func prefersStatusBarHidden() -> Bool {
//        return true
//    }
//}