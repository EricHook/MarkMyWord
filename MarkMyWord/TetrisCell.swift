//
//  TetrisCell.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/17/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit

class TetrisCell : SKSpriteNode
{
    var length: CGFloat!
    
    override init(texture: SKTexture!, color: SKColor!, size: CGSize) {
        self.length = 10 // Some sort of sensible default
        super.init(texture: texture, color: color, size: size)
    }
    
    convenience init(color: SKColor, length: CGFloat = 10) {
        var size = CGSize(width: length, height: length);
        self.init(texture:nil, color: color, size: size)
        self.length = length
    }
    
    convenience init() {
        var color = SKColor(red: 0.5, green: 0.5, blue: 0.2, alpha: 0.5)
        var length : CGFloat = 40.0
        var size = CGSize(width: length, height: length);
        self.init(texture:nil, color: color, size: size)
        self.length = length
    }
    
    required init?(coder aDecoder: NSCoder) {
        // Decoding length here would be nice...
        super.init(coder: aDecoder)
    }
    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        println("Tetris touch began")
    }
    
    override func touchesEnded(touches: Set<NSObject>, withEvent event: UIEvent) {
        println("Tetris touch ended")
    }
    
    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
        println("Tetris touch moved")
        
//        for touch: AnyObject in touches {
//            let location: CGPoint! = touch.locationInView(<#view: UIView?#>)   //.locationInNode(self)
//            print("location \(location) .. ")
//            let nodeAtPoint = self.nodeAtPoint(location)
//            println("nodeAtPoint.location \(nodeAtPoint) .. ")
        
            //if (nodeAtPoint.name != nil) {
                //print("nodeAtPoint.location \(nodeAtPoint) .. ")
                //print("Node found \(nodeAtPoint.name) and ?")
                //nodeAtPoint.position = location
            //}
//        }
    }
}
