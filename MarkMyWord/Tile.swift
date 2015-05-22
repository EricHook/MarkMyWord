//
//  Tile.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/17/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//


import SpriteKit

enum TileType: Int {
    case Unknown = 0, Letter, ColorBomb, DirectionBomb
    
    var spriteName: String {
        let spriteNames = [
            "Mystery",
            "Letter",
            "ColorBomb",
            "DirectionBomb"]
        
        return spriteNames[rawValue - 1]
    }
    
    var highlightedSpriteName: String {
        return spriteName + "-Highlighted"
    }
    
    static func random() -> TileType {
        return TileType(rawValue: Int(arc4random_uniform(4)) + 1)!
    }
}

class Tile {
    //var grid : Grid
    var column: Int
    var row: Int
    let tileType: TileType
    //var sprite: SKSpriteNode?
    
    init(column: Int, row: Int, tileType: TileType) {
        self.column = column
        self.row = row
        self.tileType = tileType
    }
}




/////////////////////////////////////////////////////////////////////////////
//import Foundation
//
//import UIKit
//import SpriteKit
//
//class Tile : SKSpriteNode {
//
////    var tile : SKSpriteNode?
////    var tile2 : SKSpriteNode?
//
//    //var anchorPoint = CGPoint(x: 0.5, y: 0.0)
//    //var position = CGPoint(x: 0.5, y: 0.0)
//    
//    
//    required init?(coder aDecoder: NSCoder) {
//        fatalError("NSCoding not supported")
//    }
//    
//    init() {
////        tile2 = SKSpriteNode(imageNamed: "OrangeLetterA.png")
////        tile2!.anchorPoint = CGPoint(x: 0.0, y: 0.0)
//        let cardTexture = SKTexture(imageNamed: "OrangeLetterA.png")
//        super.init(texture: cardTexture, color: nil, size: cardTexture.size())
//    }
//    
//    func makeTile () -> SKSpriteNode {
//        var tile2 = SKSpriteNode(imageNamed: "OrangeLetterA.png")
//        tile2.anchorPoint = CGPoint(x: 0.0, y: 0.0)
//        return tile2
//    }
//}


///////////////////////////////////////////////////////////////////////


//class Tile : SKSpriteNode
//{
//    var length: CGFloat!
//    
//    override init(texture: SKTexture!, color: SKColor!, size: CGSize) {
//        self.length = 10 // Some sort of sensible default
//        super.init(texture: texture, color: color, size: size)
//        
//        SKSpriteNode(imageNamed: "OrangeLetterA.png")
//        self.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//    }
//    
//    convenience init(color: SKColor, length: CGFloat = 10) {
//        var size = CGSize(width: length, height: length);
//        self.init(texture:nil, color: color, size: size)
//        self.length = length
//    }
//    
//    required init?(coder aDecoder: NSCoder) {
//        // Decoding length here would be nice...
//        super.init(coder: aDecoder)
//    }
//}

