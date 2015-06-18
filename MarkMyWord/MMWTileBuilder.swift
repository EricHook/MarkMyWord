//
//  MMWTileBuilder.swift
//  MarkMyWord
//
//  Created by Eric Hook on 6/11/15.
//  Copyright © 2015 Hook Studios. All rights reserved.
//

import Foundation

class MMWTileBuilder{
    
    var mmwTileArray = [MMWTile]()
    
    var mmwPlayer1TileArray = [MMWTile]()
    var mmwPlayer2TileArray = [MMWTile]()
    var mmwPlayer3TileArray = [MMWTile]()
    var mmwPlayer4TileArray = [MMWTile]()
    
    var mmwPlayedTileArray = [MMWTile]()
    
    let tileA1 : MMWTile = MMWTile(letterString: "A")
    let tileA2 : MMWTile = MMWTile(letterString: "A")
    let tileA3 : MMWTile = MMWTile(letterString: "A")
    let tileA4 : MMWTile = MMWTile(letterString: "A")
    let tileA5 : MMWTile = MMWTile(letterString: "A")
    let tileA6 : MMWTile = MMWTile(letterString: "A")
    let tileA7 : MMWTile = MMWTile(letterString: "A")
    let tileA8 : MMWTile = MMWTile(letterString: "A")
    let tileA9 : MMWTile = MMWTile(letterString: "A")
    
    let tileB1 : MMWTile = MMWTile(letterString: "B")
    let tileB2 : MMWTile = MMWTile(letterString: "B")
    
    let tileC1 : MMWTile = MMWTile(letterString: "C")
    let tileC2 : MMWTile = MMWTile(letterString: "C")
    
    let tileD1 : MMWTile = MMWTile(letterString: "D")
    let tileD2 : MMWTile = MMWTile(letterString: "D")
    let tileD3 : MMWTile = MMWTile(letterString: "D")
    let tileD4 : MMWTile = MMWTile(letterString: "D")
    
    let tileE1 : MMWTile = MMWTile(letterString: "E")
    let tileE2 : MMWTile = MMWTile(letterString: "E")
    let tileE3 : MMWTile = MMWTile(letterString: "E")
    let tileE4 : MMWTile = MMWTile(letterString: "E")
    let tileE5 : MMWTile = MMWTile(letterString: "E")
    let tileE6 : MMWTile = MMWTile(letterString: "E")
    let tileE7 : MMWTile = MMWTile(letterString: "E")
    let tileE8 : MMWTile = MMWTile(letterString: "E")
    let tileE9 : MMWTile = MMWTile(letterString: "E")
    let tileE10 : MMWTile = MMWTile(letterString: "E")
    let tileE11 : MMWTile = MMWTile(letterString: "E")
    let tileE12 : MMWTile = MMWTile(letterString: "E")
    
    let tileF1 : MMWTile = MMWTile(letterString: "F")
    let tileF2 : MMWTile = MMWTile(letterString: "F")
    
    let tileG1 : MMWTile = MMWTile(letterString: "G")
    let tileG2 : MMWTile = MMWTile(letterString: "G")
    let tileG3 : MMWTile = MMWTile(letterString: "G")
    
    let tileH1 : MMWTile = MMWTile(letterString: "H")
    let tileH2 : MMWTile = MMWTile(letterString: "H")
    
    let tileI1 : MMWTile = MMWTile(letterString: "I")
    let tileI2 : MMWTile = MMWTile(letterString: "I")
    let tileI3 : MMWTile = MMWTile(letterString: "I")
    let tileI4 : MMWTile = MMWTile(letterString: "I")
    let tileI5 : MMWTile = MMWTile(letterString: "I")
    let tileI6 : MMWTile = MMWTile(letterString: "I")
    let tileI7 : MMWTile = MMWTile(letterString: "I")
    let tileI8 : MMWTile = MMWTile(letterString: "I")
    let tileI9 : MMWTile = MMWTile(letterString: "I")
    
    let tileJ1 : MMWTile = MMWTile(letterString: "J")
    
    let tileK1 : MMWTile = MMWTile(letterString: "K")
    
    let tileL1 : MMWTile = MMWTile(letterString: "L")
    let tileL2 : MMWTile = MMWTile(letterString: "L")
    let tileL3 : MMWTile = MMWTile(letterString: "L")
    let tileL4 : MMWTile = MMWTile(letterString: "L")
    
    let tileM1 : MMWTile = MMWTile(letterString: "M")
    let tileM2 : MMWTile = MMWTile(letterString: "M")
    
    let tileN1 : MMWTile = MMWTile(letterString: "N")
    let tileN2 : MMWTile = MMWTile(letterString: "N")
    let tileN3 : MMWTile = MMWTile(letterString: "N")
    let tileN4 : MMWTile = MMWTile(letterString: "N")
    let tileN5 : MMWTile = MMWTile(letterString: "N")
    let tileN6 : MMWTile = MMWTile(letterString: "N")
    
    let tileO1 : MMWTile = MMWTile(letterString: "O")
    let tileO2 : MMWTile = MMWTile(letterString: "O")
    let tileO3 : MMWTile = MMWTile(letterString: "O")
    let tileO4 : MMWTile = MMWTile(letterString: "O")
    let tileO5 : MMWTile = MMWTile(letterString: "O")
    let tileO6 : MMWTile = MMWTile(letterString: "O")
    let tileO7 : MMWTile = MMWTile(letterString: "O")
    let tileO8 : MMWTile = MMWTile(letterString: "O")
    
    let tileP1 : MMWTile = MMWTile(letterString: "P")
    let tileP2 : MMWTile = MMWTile(letterString: "P")

    let tileQ1 : MMWTile = MMWTile(letterString: "Q")

    let tileR1 : MMWTile = MMWTile(letterString: "R")
    let tileR2 : MMWTile = MMWTile(letterString: "R")
    let tileR3 : MMWTile = MMWTile(letterString: "R")
    let tileR4 : MMWTile = MMWTile(letterString: "R")
    let tileR5 : MMWTile = MMWTile(letterString: "R")
    let tileR6 : MMWTile = MMWTile(letterString: "R")

    let tileS1 : MMWTile = MMWTile(letterString: "S")
    let tileS2 : MMWTile = MMWTile(letterString: "S")
    let tileS3 : MMWTile = MMWTile(letterString: "S")
    let tileS4 : MMWTile = MMWTile(letterString: "S")

    let tileT1 : MMWTile = MMWTile(letterString: "T")
    let tileT2 : MMWTile = MMWTile(letterString: "T")
    let tileT3 : MMWTile = MMWTile(letterString: "T")
    let tileT4 : MMWTile = MMWTile(letterString: "T")
    let tileT5 : MMWTile = MMWTile(letterString: "T")
    let tileT6 : MMWTile = MMWTile(letterString: "T")

    let tileU1 : MMWTile = MMWTile(letterString: "U")
    let tileU2 : MMWTile = MMWTile(letterString: "U")
    let tileU3 : MMWTile = MMWTile(letterString: "U")
    let tileU4 : MMWTile = MMWTile(letterString: "U")

    let tileV1 : MMWTile = MMWTile(letterString: "V")
    let tileV2 : MMWTile = MMWTile(letterString: "V")

    let tileW1 : MMWTile = MMWTile(letterString: "W")
    let tileW2 : MMWTile = MMWTile(letterString: "W")

    let tileX1 : MMWTile = MMWTile(letterString: "X")

    let tileY1 : MMWTile = MMWTile(letterString: "Y")
    let tileY2 : MMWTile = MMWTile(letterString: "Y")

    let tileZ1 : MMWTile = MMWTile(letterString: "Z")

    let tileBlank1 : MMWTile = MMWTile(letterString: "?")
    let tileBlank2 : MMWTile = MMWTile(letterString: "?")

    
    init () {
        mmwTileArray.append(tileA1)
        mmwTileArray.append(tileA2)
        mmwTileArray.append(tileA3)
        mmwTileArray.append(tileA4)
        mmwTileArray.append(tileA5)
        mmwTileArray.append(tileA6)
        mmwTileArray.append(tileA7)
        mmwTileArray.append(tileA8)
        mmwTileArray.append(tileA9)
        
        mmwTileArray.append(tileB1)
        mmwTileArray.append(tileB2)
        
        mmwTileArray.append(tileC1)
        mmwTileArray.append(tileC2)
        
        mmwTileArray.append(tileD1)
        mmwTileArray.append(tileD2)
        mmwTileArray.append(tileD3)
        mmwTileArray.append(tileD4)
        
        mmwTileArray.append(tileE1)
        mmwTileArray.append(tileE2)
        mmwTileArray.append(tileE3)
        mmwTileArray.append(tileE4)
        mmwTileArray.append(tileE5)
        mmwTileArray.append(tileE6)
        mmwTileArray.append(tileE7)
        mmwTileArray.append(tileE8)
        mmwTileArray.append(tileE9)
        mmwTileArray.append(tileE10)
        mmwTileArray.append(tileE11)
        mmwTileArray.append(tileE12)
        
        mmwTileArray.append(tileF1)
        mmwTileArray.append(tileF2)
        
        mmwTileArray.append(tileG1)
        mmwTileArray.append(tileG2)
        mmwTileArray.append(tileG3)
        
        mmwTileArray.append(tileH1)
        mmwTileArray.append(tileH2)
        
        mmwTileArray.append(tileI1)
        mmwTileArray.append(tileI2)
        mmwTileArray.append(tileI3)
        mmwTileArray.append(tileI4)
        mmwTileArray.append(tileI5)
        mmwTileArray.append(tileI6)
        mmwTileArray.append(tileI7)
        mmwTileArray.append(tileI8)
        mmwTileArray.append(tileI9)
        
        mmwTileArray.append(tileJ1)
        
        mmwTileArray.append(tileK1)
        
        mmwTileArray.append(tileL1)
        mmwTileArray.append(tileL2)
        mmwTileArray.append(tileL3)
        mmwTileArray.append(tileL4)
        
        mmwTileArray.append(tileM1)
        mmwTileArray.append(tileM2)
        
        mmwTileArray.append(tileN1)
        mmwTileArray.append(tileN2)
        mmwTileArray.append(tileN3)
        mmwTileArray.append(tileN4)
        mmwTileArray.append(tileN5)
        mmwTileArray.append(tileN6)
        
        mmwTileArray.append(tileO1)
        mmwTileArray.append(tileO2)
        mmwTileArray.append(tileO3)
        mmwTileArray.append(tileO4)
        mmwTileArray.append(tileO5)
        mmwTileArray.append(tileO6)
        mmwTileArray.append(tileO7)
        mmwTileArray.append(tileO8)
        
        mmwTileArray.append(tileP1)
        mmwTileArray.append(tileP2)
        
        mmwTileArray.append(tileQ1)
        
        mmwTileArray.append(tileR1)
        mmwTileArray.append(tileR2)
        mmwTileArray.append(tileR3)
        mmwTileArray.append(tileR4)
        mmwTileArray.append(tileR5)
        mmwTileArray.append(tileR6)
        
        mmwTileArray.append(tileS1)
        mmwTileArray.append(tileS2)
        mmwTileArray.append(tileS3)
        mmwTileArray.append(tileS4)
        
        mmwTileArray.append(tileT1)
        mmwTileArray.append(tileT2)
        mmwTileArray.append(tileT3)
        mmwTileArray.append(tileT4)
        mmwTileArray.append(tileT5)
        mmwTileArray.append(tileT6)
        
        mmwTileArray.append(tileU1)
        mmwTileArray.append(tileU2)
        mmwTileArray.append(tileU3)
        mmwTileArray.append(tileU4)
        
        mmwTileArray.append(tileV1)
        mmwTileArray.append(tileV2)
        
        mmwTileArray.append(tileW1)
        mmwTileArray.append(tileW2)
        
        mmwTileArray.append(tileX1)
        
        mmwTileArray.append(tileY1)
        mmwTileArray.append(tileY2)
        
        mmwTileArray.append(tileZ1)
    }
}