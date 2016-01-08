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
        }
        return nil
    }
}

var mmwGameSceneViewController : MMWGameSceneViewController! = MMWGameSceneViewController()
weak var gameViewController : GameViewController! = GameViewController()

class GameViewController : UIViewController {
    
    //weak var parentController: GameViewController?
    
    //@IBOutlet weak var menuView: UIView?
    //@IBOutlet weak var playButton: UIButton?
    
    
    
    @IBOutlet var GameViewControllerUI: SKView!
    
    @IBOutlet weak var ViewOptionsUI: UIView!
    @IBOutlet weak var ViewAllOptionsUI: UIView!

    @IBOutlet weak var ViewPlayer1UI: UIView!
    @IBOutlet weak var ViewPlayer2UI: UIView!
    @IBOutlet weak var ViewPlayer3UI: UIView!
    @IBOutlet weak var ViewPlayer4UI: UIView!
    @IBOutlet weak var numberPlayersUI: UISegmentedControl!
    
    @IBOutlet weak var secondPerTurnSwitchOutlet: UISegmentedControl!
    
    @IBAction func secondsPerTurnAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            mmwGameSceneViewController.secondsPerTurn = 20
            print("secondsPerTurn = 20")
        case 1:
            mmwGameSceneViewController.secondsPerTurn = 30
            print("secondsPerTurn = 30")
        case 2:
            mmwGameSceneViewController.secondsPerTurn = 45
            print("secondsPerTurn = 45")
        case 3:
            mmwGameSceneViewController.secondsPerTurn = 60
            print("secondsPerTurn = 60")
        case 4:
            mmwGameSceneViewController.secondsPerTurn = 999
            print("secondsPerTurn = 999")
        default:
            break;
        }
    }

    @IBOutlet weak var audioSettingOutlet: UISegmentedControl!
    
    @IBAction func audioSettingAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            mmwGameSceneViewController.audioOn = false
            print("audio turned to off")
            
        case 1:
            mmwGameSceneViewController.audioOn = false
            print("audio turned to on")
        default:
            break;
        }
    }
    
    
    @IBOutlet weak var minWordLengthSwitchOutlet: UISegmentedControl!
    
    
    @IBAction func minWordLengthSwitchAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            mmwGameSceneViewController.minWordSize = 2
            print("minWordSize set to 2")
        case 1:
            mmwGameSceneViewController.minWordSize = 3
            print("minWordSize set to 3")
        case 2:
            mmwGameSceneViewController.minWordSize = 3
            print("minWordSize set to 4")
        default:
            break;
        }
    }
    
    
    @IBOutlet weak var numStarterWordsSwitchOutlet: UIView!
    
    
    @IBAction func numStarterWordsSwitchAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            mmwGameSceneViewController.numStarterWords = 1
            print("numStarterWords = 1")
        case 1:
            mmwGameSceneViewController.numStarterWords = 2
            print("numStarterWords = 2")
        case 2:
            mmwGameSceneViewController.numStarterWords = 3
            print("numStarterWords = 3")
        default:
        break;
        }
    }

    @IBOutlet weak var numPlayersSwitchOutlet: UISegmentedControl!

    @IBAction func numPlayersSwitchAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            mmwGameSceneViewController.numPlayers = 2
            print("numPlayers 2")
            ViewPlayer3UI.alpha = 0.5
            ViewPlayer3UI.userInteractionEnabled = false
            ViewPlayer4UI.alpha = 0.5
            ViewPlayer4UI.userInteractionEnabled = false
        case 1:
            mmwGameSceneViewController.numPlayers = 3
            print("numPlayers 3")
            ViewPlayer3UI.alpha = 1.0
            ViewPlayer3UI.userInteractionEnabled = true
            ViewPlayer4UI.alpha = 0.5
            ViewPlayer4UI.userInteractionEnabled = false
        case 2:
            mmwGameSceneViewController.numPlayers = 4
            print("numPlayers 4")
            ViewPlayer3UI.alpha = 1.0
            ViewPlayer3UI.userInteractionEnabled = true
            ViewPlayer4UI.alpha = 1.0
            ViewPlayer4UI.userInteractionEnabled = true
        default:
            break;
        }
    }

    
    
    
//    @IBAction func numberPlayersUI(sender: AnyObject) {
//        switch (sender.selectedSegmentIndex){
//        case 0:
//            mmwGameSceneViewController.numPlayers = 2
//            print("numPlayers 2")
//            ViewPlayer3UI.alpha = 0.5
//            ViewPlayer3UI.userInteractionEnabled = false
//            ViewPlayer4UI.alpha = 0.5
//            ViewPlayer4UI.userInteractionEnabled = false
//        case 1:
//            mmwGameSceneViewController.numPlayers = 3
//            print("numPlayers 3")
//            ViewPlayer3UI.alpha = 1.0
//            ViewPlayer3UI.userInteractionEnabled = true
//            ViewPlayer4UI.alpha = 0.5
//            ViewPlayer4UI.userInteractionEnabled = false
//        case 2:
//            mmwGameSceneViewController.numPlayers = 4
//            print("numPlayers 4")
//            ViewPlayer3UI.alpha = 1.0
//            ViewPlayer3UI.userInteractionEnabled = true
//            ViewPlayer4UI.alpha = 1.0
//            ViewPlayer4UI.userInteractionEnabled = true
//        default:
//            break;
//        }
//    }
    
    
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
        print("in GameViewController viewDidLoad")
        screenSize = view.bounds.size
        let mainMenuScene = MainMenuScene(size: screenSize!)
        
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
            print("Screen width:\(screenSize!.width) , device type: \(mmwGame.deviceType) ")
        }
            
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

        

        //var cornerView = UIView()
        
        //self.view.insertSubview(cornerView, atIndex: 2)
        
//        button.frame = CGRectMake(100, 100, 100, 50)
//        button.backgroundColor = UIColor.greenColor()
//        button.setTitle("Test Button", forState: UIControlState.Normal)
//        button.addTarget(self, action: "buttonAction:", forControlEvents: UIControlEvents.TouchUpInside)
        
        //cornerView.addSubview(button)
        
        
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
    
    
    @IBOutlet weak var OptionsSwitchOutlet: UISegmentedControl!
    
    
    @IBAction func OptionsSwitchAction(sender: AnyObject) {
        
        if sender.selectedSegmentIndex == 2 {
            print("stats selected")
        }

        switch (self.OptionsSwitchOutlet.selectedSegmentIndex){
        case 0:
            print("stats 0 Options selected")
            ViewOptionsUI.hidden = false
            ViewOptionsUI.userInteractionEnabled = true
        case 1:
            print("stats 1 Rules selected")
            self.ViewOptionsUI.hidden = true
        case 2:
            print("stats 2 Stats selected")

            self.ViewOptionsUI.hidden = true
            self.ViewOptionsUI.alpha = 1.0
            toStatView.sendActionsForControlEvents(.TouchUpInside)

        default:
            break;
        }
        
        print("testSwitch worked")
        button.enabled = false
        button.hidden = true
        //testSwitch.tintColor = UIColorApplePurple

    }
    
    
    
    
    
    
    
    
    


//    @IBAction func OptionsSwitchAction(sender: AnyObject) {
//        
//       //    }
  
    
    @IBOutlet weak var toStatView: UIButton!
    
    @IBAction func toStatView(sender: AnyObject) {
       
            
//            selectedSegmentIndex  .sendActionsForControlEvents(.TouchUpInside)
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
