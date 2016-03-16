////
////  EndGameScene.swift
////  MarkMyWord
////
////  Created by Eric Hook on 1/24/16.
////  Copyright © 2016 Hook Studios. All rights reserved.
////
//
//import UIKit
//import SpriteKit
//
//class EndGameScene: SKScene {
//
//    /*
//    // Only override drawRect: if you perform custom drawing.
//    // An empty implementation adversely affects performance during animation.
//    override func drawRect(rect: CGRect) {
//        // Drawing code
//    }
//    */
//    
//    var viewSize = screenSize!
//    
//    
//    required init?(coder aDecoder: NSCoder) {
//        super.init(coder: aDecoder)
//        size = viewSize
//    }
//    
//    
//    override init(size: CGSize) {
//        super.init(size: size)
//    }
//    
//    
//    override func didMoveToView(view: SKView) {
//        /* Setup your scene here */
//
//        screenSize = view.bounds.size
//        print("view size MenuScene: \(screenSize)")
//        
//        // add BG
//        let backgroundNode = SKSpriteNode(imageNamed: "MarkMyWordBGCleaniPad@2x.png")
//        //backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        backgroundNode.position = CGPoint(x: viewSize.width/2, y: viewSize.height/2)
//        backgroundNode.userInteractionEnabled = false
//        backgroundNode.size = self.frame.size;
//        self.addChild(backgroundNode)
//    }
//}
