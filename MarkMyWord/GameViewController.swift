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
        
        
        
        
        
        
//        assert(image != nil, "Image not set; required to use view controller")
//        photoImageView.image = image
//        
//        // Resize if neccessary to ensure it's not pixelated
//        if image.size.height <= photoImageView.bounds.size.height &&
//            image.size.width <= photoImageView.bounds.size.width {
//                photoImageView.contentMode = .Center
//        }
//        
//        dispatch_async(dispatch_get_global_queue(Int(QOS_CLASS_USER_INITIATED.value), 0)) { // 1
//            let overlayImage = self.faceOverlayImageFromImage(self.image)
//            dispatch_async(dispatch_get_main_queue()) { // 2
//                self.fadeInNewImage(overlayImage) // 3
//            }
//        }
//        //func downloadImageURLWithString(urlString: String) {
//            let url = NSURL(string: urlString)
//            let semaphore = dispatch_semaphore_create(0) // 1
//            let photo = DownloadPhoto(url: url!) {
//                image, error in
//                if let error = error {
//                    XCTFail("\(urlString) failed. \(error.localizedDescription)")
//                }
//                dispatch_semaphore_signal(semaphore) // 2
//            }
//            
//            let timeout = dispatch_time(DISPATCH_TIME_NOW, DefaultTimeoutLengthInNanoSeconds)
//            if dispatch_semaphore_wait(semaphore, timeout) != 0 { // 3
//                XCTFail("\(urlString) timed out")
//            }
//        //}
        
        
        
        
//        var starterWord1 = mmwGameSceneViewController.getRandomWord()
//        
//        while mmwGameSceneViewController.checkTilesForWord(starterWord1, letterTileArray: mmwGameSceneViewController.tileCollection.mmwTileArray) == false {
//            starterWord1 = mmwGameSceneViewController.getRandomWord()
//        }
//        var starterWord1TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord1, letterTileArray: &mmwGameSceneViewController.tileCollection.mmwTileArray)
//        
//        var starterWord2 = mmwGameSceneViewController.getRandomWord()
//        while (starterWord2 == starterWord1) || (mmwGameSceneViewController.checkTilesForWord(starterWord2, letterTileArray: mmwGameSceneViewController.tileCollection.mmwTileArray ) == false )  {
//            starterWord2 = mmwGameSceneViewController.getRandomWord()
//        }
//        var starterWord2TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord2, letterTileArray: &mmwGameSceneViewController.tileCollection.mmwTileArray)
//        
//        var starterWord3 = mmwGameSceneViewController.getRandomWord()
//        while (starterWord3 == starterWord2) || (starterWord3 == starterWord1) || (mmwGameSceneViewController.checkTilesForWord(starterWord3, letterTileArray: mmwGameSceneViewController.tileCollection.mmwTileArray ) == false )  {
//            starterWord3 = mmwGameSceneViewController.getRandomWord()
//        }
//        var starterWord3TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord3, letterTileArray: &mmwGameSceneViewController.tileCollection.mmwTileArray)
//        
//        var starterWord4 = mmwGameSceneViewController.getRandomWord()
//        while (starterWord4 == starterWord3) || (starterWord4 == starterWord2) || (starterWord4 == starterWord1) && (mmwGameSceneViewController.checkTilesForWord(starterWord4, letterTileArray: mmwGameSceneViewController.tileCollection.mmwTileArray ) == false )  {
//            starterWord4 = mmwGameSceneViewController.getRandomWord()
//        }
//        var starterWord4TileArray = mmwGameSceneViewController.returnTilesForWord(starterWord4, letterTileArray: &mmwGameSceneViewController.tileCollection.mmwTileArray)

        
        
        
        
        
        
        
        
        
        
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
