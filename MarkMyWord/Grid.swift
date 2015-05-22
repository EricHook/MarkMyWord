//
//  GridTools.swift
//  MarkMyWord
//
//  Created by Eric Hook on 5/5/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation

//protocol Initable {
//    init()
//}

//struct gridArray<T> {
//    let grid2DArray : [T]
//}

class Grid {
    
    var gridLowerLeftX : Double = 0.0
    var gridLowerLeftY : Double = 0.0
    var gridSquareSizeX : Double = 10.0
    var gridSquareSizeY : Double = 10.0
    var gridNumSquaresX : Int = 5
    var gridNumSquaresY : Int = 5
    //var gridArr : [[AnyObject]]
    var gridArr = Array<Array<AnyObject>>()
    
    init() { // default 10x10 grid at 0,0
        self.gridLowerLeftX = 4
        gridArr = [[AnyObject]](count: gridNumSquaresY, repeatedValue: [AnyObject](count: gridNumSquaresX, repeatedValue: 0))
    }
    
    init(gridLowerLeftX: Double, gridLowerLeftY : Double, gridSquareSizeX : Double, gridSquareSizeY : Double, gridNumSquaresX : Int, gridNumSquaresY : Int){
        self.gridLowerLeftX = gridLowerLeftX
        self.gridLowerLeftY = gridLowerLeftY
        self.gridSquareSizeX = gridSquareSizeX
        self.gridSquareSizeY = gridSquareSizeY
        self.gridNumSquaresX = gridNumSquaresX
        self.gridNumSquaresY = gridNumSquaresY
        gridArr = [[AnyObject]](count: gridNumSquaresY, repeatedValue: [AnyObject](count: gridNumSquaresX, repeatedValue: 0))
    }
    
    func getGridSquare (locX: Float, locY: Float) -> (GridSquareX:Int, GridSquareY:Int, GridSquareLowerLeftCornerX: Double, GridSquareLowerLeftCornerY: Double) {
        var GridSquareX : Int = 0
        var GridSquareY : Int = 0
        var GridSquareLowerLeftCornerX : Double = 0.0
        var GridSquareLowerLeftCornerY : Double = 0.0
        GridSquareX = Int(( Double(locX) - self.gridLowerLeftX ) / gridSquareSizeX )// arrary [] x location
        GridSquareY = Int(( Double(locY) - self.gridLowerLeftY ) / gridSquareSizeY ) // arrary [] y location -> 0 is top left corner
        GridSquareLowerLeftCornerX = self.gridLowerLeftX + (Double(GridSquareX) * gridSquareSizeX)
        GridSquareLowerLeftCornerY = self.gridLowerLeftY + (Double(GridSquareY) * gridSquareSizeY)
        println("TEST Grid square[]: [\(GridSquareX)], [\(GridSquareY)] and LL corner: \(GridSquareLowerLeftCornerX), \(GridSquareLowerLeftCornerY), click: \(locX), y: \(locY)")
        return (GridSquareX, GridSquareY, GridSquareLowerLeftCornerX, GridSquareLowerLeftCornerY )
    }
    
    func addToGridArray (tileToAdd: Tile, xGrid: Int, yGrid: Int) {
        //gridArr[xGrid].insert(tileToAdd, atIndex: [yGrid])  // (tileToAdd, atIndex: xGrid)
    }
    
//    func getGridSquareLLCorner (grid: Grid, locX: Float, locY: Float) -> (GridSquareX:Int, GridSquareY:Int) {
//        var GridSquareX : Int = 0
//        var GridSquareY : Int = 0
//        GridSquareX = Int(( locX - grid.gridLowerLeftX ) / gridSquareSizeX )
//        GridSquareY = Int(( locY - grid.gridLowerLeftY ) / gridSquareSizeY )
//        println("TEST Grid square: \(GridSquareX), \(GridSquareY)")
//        return (GridSquareX, GridSquareY)
//    }
    
//    func gridArrayMakeTest () {
//        gridArr[0][0] = "Joe"
//        gridArr[0][1] = "Mo"
//        var doub : Double = 22.2
//        gridArr[0][2] = doub
//        var gridTest : Grid = Grid()
//        gridArr[0][3] = gridTest
//        println("gridArrayTEST count: \(gridArr[0][0]) ... \(gridArr.description)")
//        println("gridArrayTEST count: \(gridArr[0][3]) ... \(gridArr.description)")
//    }
}