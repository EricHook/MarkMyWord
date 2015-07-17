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
    var gridArr = [[MMWTile]]()
    var gridName : String = ""
    var gridPlayer : Player? = nil
    
    init() { // default 10x10 grid at 0,0
        self.gridUpperLeftX = 4
        gridArr = [[MMWTile]](count: gridNumSquaresY, repeatedValue: [MMWTile](count: gridNumSquaresX, repeatedValue: MMWTile() ))
    }
    
    init (gridUpperLeftX: Double, gridUpperLeftY : Double, gridSquareSizeX : Double, gridSquareSizeY : Double, gridNumSquaresX : Int, gridNumSquaresY : Int, gridName : String){
        self.gridUpperLeftX = gridUpperLeftX
        self.gridUpperLeftY = gridUpperLeftY
        self.gridSquareSizeX = gridSquareSizeX
        self.gridSquareSizeY = gridSquareSizeY
        self.gridNumSquaresX = gridNumSquaresX
        self.gridNumSquaresY = gridNumSquaresY
        self.gridName = gridName
        gridArr = [[MMWTile]](count: gridNumSquaresY, repeatedValue: [MMWTile](count: gridNumSquaresX, repeatedValue: MMWTile() ))
        // gridArr[1][1] = 0 // put test placeholder value in grid
    }
    
    func setGridPlayer (player : Player ) {
        self.gridPlayer = player
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
        let tilePositionX = grid.gridUpperLeftX + 23.75 + ( Double(squareX) * grid.gridSquareSizeX )
        let tilePositionY = 768 - grid.gridUpperLeftY - ( Double(squareY) * grid.gridSquareSizeY ) + 7
        let tilePosition = CGPoint( x: tilePositionX, y: tilePositionY )
        return tilePosition
    }
    
    func addToGridArray (tileToAdd: MMWTile, xGrid: Int, yGrid: Int) {
        gridArr[xGrid].insert(tileToAdd, atIndex: [yGrid][xGrid])  // (tileToAdd, atIndex: xGrid)
    }
    
    func tileAtGridSquare (grid: Grid, squareX: Int, squareY: Int) -> (MMWTile?) {
        let tile : MMWTile? = (grid.gridArr[squareY][squareX] as MMWTile)
        return tile
    }
    
    func fillGridFromArray (arrayIn: [MMWTile], gridToFill: Grid ) {
        print("<Grid> fillGridFromArray(arrayIn: [MMWTile], gridToFill: Grid) \(gridToFill.gridName) ")
        var arrayInMarker = 0
        for y in 0...(gridToFill.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(gridToFill.gridNumSquaresX - 1) {
                if arrayInMarker < (gridToFill.gridNumSquaresY) * (gridToFill.gridNumSquaresX) {
                    let tileUpdated : MMWTile = arrayIn[arrayInMarker]
                    gridToFill.gridArr[y][x] = tileUpdated
                    tileUpdated.gridX = x
                    tileUpdated.gridY = y
                    //print("tile: \((arrayIn[arrayInMarker] as MMWTile).tileSprite.tileText) [\(tileUpdated.gridX)] [\(tileUpdated.gridY)] ")
                    arrayInMarker++
                }
            }
        }
        printGrid(gridToFill)
    }
    
    func printGrid (gridToPrint: Grid ) {
        print("<Grid> printGrid \(gridToPrint.gridName) ")
        for y in 0...(gridToPrint.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(gridToPrint.gridNumSquaresX - 1) {
                let gridArr = gridToPrint.gridArr[x][y]
                print(" \( gridArr.tileSprite.tileText) -> [\(gridArr.tileSprite.tileSpriteParent.gridX)] [\(gridArr.tileSprite.tileSpriteParent.gridY)] ")
            }
        }
    }
    
    func fillArrayFromGrid (arrayToFill: [AnyObject], gridOut: Grid ) {
        var arrayInMarker = 0
        for y in 0...(gridOut.gridNumSquaresY - 1) {   // fill Player letter tiles
            for x in 0...(gridOut.gridNumSquaresX - 1) {
                if arrayInMarker < x * y {
                    gridOut.gridArr[y][x] = arrayToFill[arrayInMarker] as! MMWTile
                    arrayInMarker++
                }
            }
        }
    }
}