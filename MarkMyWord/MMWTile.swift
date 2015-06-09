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
