//
//  OptionsUISceneViewController.swift
//  MarkMyWord
//
//  Created by Eric Hook on 1/5/16.
//  Copyright © 2016 Hook Studios. All rights reserved.
//

import UIKit

class OptionsUISceneViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func pushButton(sender: AnyObject) {
    }

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}


//let button   = UIButton(type: UIButtonType.System) as UIButton
//
//override func viewDidLoad() {
//    super.viewDidLoad()
//    print("in GameViewController viewDidLoad")
//    screenSize = view.bounds.size
//    let mainMenuScene = MainMenuScene(size: screenSize!)
//    
//    switch Int(screenSize!.width) {
//    case 1024: // [1024x768]
//        mmwGame.setDeviceType(MMWGame.DeviceType.iPad)
//    case 2048: //  [2048 x 1536]
//        mmwGame.setDeviceType(MMWGame.DeviceType.iPadRetina)
//    case 1366: // [2732 x 2048] --> Width 1366!
//        mmwGame.setDeviceType(MMWGame.DeviceType.iPadPro)
//    case 568: // [568.0, 320.0]
//        mmwGame.setDeviceType(MMWGame.DeviceType.iPhone5)
//    case 2732: // [2732 x 2048]
//        mmwGame.setDeviceType(MMWGame.DeviceType.iPhone6)
//    case 736: // [2732 x 2048]
//        mmwGame.setDeviceType(MMWGame.DeviceType.iPhone6Plus)
//    default:
//        mmwGame.setDeviceType(MMWGame.DeviceType.iPad)
//        print("Screen width:\(screenSize!.width) , device type: \(mmwGame.deviceType) ")
//    }
//    
//    let skView = self.view as! SKView
//    skView.showsFPS = true
//    skView.showsNodeCount = true
//    
//    /* Sprite Kit applies additional optimizations to improve rendering performance */
//    skView.ignoresSiblingOrder = true
//    
//    /* Set the scale mode to scale to fit the window */
//    mainMenuScene.scaleMode = .AspectFill
//    
//    /////////////////
//    //        NSNotificationCenter.defaultCenter().addObserver(self, selector: "presentView", name: "showController", object: nil)
//    /////////////////////////////////
//    
//    //scene.parentController = self
//    
//    skView.presentScene(mainMenuScene)