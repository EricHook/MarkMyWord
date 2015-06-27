//
//  GridTools.swift
//  MarkMyWord
//
//  Created by Eric Hook on 5/5/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

class Grid {
    
    var gridUpperLeftX : Double = 0.0
    var gridUpperLeftY : Double = 0.0
    
    var gridSquareSizeX : Double = 10.0
    var gridSquareSizeY : Double = 10.0
    
    var gridNumSquaresX : Int = 5
    var gridNumSquaresY : Int = 5
    
    //var gridArr : [[AnyObject]]
    var gridArr = Array <Array < AnyObject > >()
    var gridName : String = ""
    
    init() { // default 10x10 grid at 0,0
        self.gridUpperLeftX = 4
        gridArr = [[AnyObject]](count: gridNumSquaresY, repeatedValue: [AnyObject](count: gridNumSquaresX, repeatedValue: 0))
    }
    
    init(gridUpperLeftX: Double, gridUpperLeftY : Double, gridSquareSizeX : Double, gridSquareSizeY : Double, gridNumSquaresX : Int, gridNumSquaresY : Int, gridName : String){
        self.gridUpperLeftX = gridUpperLeftX
        self.gridUpperLeftY = gridUpperLeftY
        self.gridSquareSizeX = gridSquareSizeX
        self.gridSquareSizeY = gridSquareSizeY
        self.gridNumSquaresX = gridNumSquaresX
        self.gridNumSquaresY = gridNumSquaresY
        self.gridName = gridName
        gridArr = [[AnyObject]](count: gridNumSquaresY, repeatedValue: [AnyObject](count: gridNumSquaresX, repeatedValue: 0))
        gridArr[1][1] = 1
    }
    
    func getGridSquare (locX: Float, locY: Float) -> (GridSquareX: Int, GridSquareY: Int, GridSquareUpperLeftCornerX: Double, GridSquareUpperLeftCornerY: Double) {
        let locYmod = locY + 35// (adj for flipped coordinates)
        var GridSquareX : Int = 0
        var GridSquareY : Int = 0
        var GridSquareUpperLeftCornerX : Double = 0.0
        var GridSquareUpperLeftCornerY : Double = 0.0
        
        GridSquareX = Int(( Double(locX)    - self.gridUpperLeftX ) / gridSquareSizeX ) // arrary [] x location
        GridSquareY = Int(( Double(locYmod) - self.gridUpperLeftY ) / gridSquareSizeY ) // arrary [] y location -> 0 is top left corner
        
        GridSquareUpperLeftCornerX = self.gridUpperLeftX + (Double(GridSquareX) * gridSquareSizeX)
        GridSquareUpperLeftCornerY = self.gridUpperLeftY + (Double(GridSquareY) * gridSquareSizeY)
        
        print("<Grid> TEST Grid square[]: x[\(GridSquareX)], y[\(GridSquareY)] and UL corner: x\(GridSquareUpperLeftCornerX), y\(GridSquareUpperLeftCornerY), click: \(locX), locY: \(locY), locYmod: \(locYmod)")
        
        return (GridSquareX, GridSquareY, GridSquareUpperLeftCornerX, GridSquareUpperLeftCornerY )
    }
    
    // given a grid and x, y func returns a CGPoint to place sprite
    func sendToGridSquare (grid: Grid, squareX: Int, squareY: Int) -> (CGPoint) {
//        let GridSquareX : Int = 0
//        let GridSquareY : Int = 0
//        let GridSquareUpperLeftCornerX : Double = 0.0
//        let GridSquareUpperLeftCornerY : Double = 0.0
        let tilePositionX = grid.gridUpperLeftX + 23.75 + ( Double(squareX) * grid.gridSquareSizeX )
        let tilePositionY = 768 - grid.gridUpperLeftY - ( Double(squareY) * grid.gridSquareSizeY ) + 7
        let tilePosition = CGPoint( x: tilePositionX, y: tilePositionY )
        return tilePosition
    }
    
    func addToGridArray (tileToAdd: MMWTile, xGrid: Int, yGrid: Int) {
        gridArr[xGrid].insert(tileToAdd, atIndex: [yGrid][xGrid])  // (tileToAdd, atIndex: xGrid)
    }
    
    

}