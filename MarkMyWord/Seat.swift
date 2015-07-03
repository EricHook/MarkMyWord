////
////  MMWSeat.swift
////  MarkMyWord
////
////  Created by Eric Hook on 5/14/15.
////  Copyright (c) 2015 Hook Studios. All rights reserved.
////
//
//import Foundation
//import UIKit
//
//class Seat {
//    var playerSeatNum : Int = 0
//    var seatColorNumber : Int = 0
//    var seatUIColor : UIColor
//    var rackSize : Int = 6
//    var specialRackSize : Int = 3
//    var score : Int = 0
//    var rack : [Int]
//    var specialRack : [Int]
//    
//    init (_playerSeatNum : Int, _seatColorNumber : Int) {
//        playerSeatNum = _playerSeatNum
//        seatColorNumber = _seatColorNumber
//        rack = Array(count: rackSize, repeatedValue: 0)
//        specialRack = Array(count: rackSize, repeatedValue: 0)
//        seatUIColor = gameColors[seatColorNumber]
//    }
//    
//    func resetRack () -> [Int] {
//        for index in 0...(self.rackSize-1) {
//            self.rack[index] = 0
//        }
//        return rack
//    }
//    
//    func resetSpecialRack () -> [Int] {
//        for index in 0...(specialRackSize-1) {
//            specialRack[index] = 0
//        }
//        return specialRack
//    }
//    
//    func setTile (tileValue : Int, tileNumber : Int) {
//        rack[tileNumber] = tileValue
//    }
//    
//    func getTile (tileNumber : Int) -> Int {
//        return rack[tileNumber]
//    }
//    
//    func setSpecialTile (tileValue : Int, tileNumber : Int) {
//        specialRack[tileNumber] = tileValue
//    }
//    
//    func getSpecialTile (tileNumber : Int) -> Int {
//        return specialRack[tileNumber]
//    }
//    
//        
//}
