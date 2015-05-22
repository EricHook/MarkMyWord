//
//  MMWPlayer.swift
//  MarkMyWord
//
//  Created by Eric Hook on 5/14/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation

class MMWPlayer {
    
    var playerID : Int = 1
    var playerName : String = ""
    var doIt : Bool = false
    var didIt : Bool = false
    var playerSeat : MMWSeat
    
    init (_playerID : Int, _playerName : String) {
        playerSeat = MMWSeat(_playerSeatNum: 1, _seatColorNumber: 1)
        playerID = _playerID
        playerName = _playerName
    }
}