//
//  Config.swift
//  Anagrams
//
//  Created by Caroline on 1/08/2014.
//  Copyright (c) 2013 Underplot ltd. All rights reserved.
//

import Foundation
import UIKit

//UI Constants
let ScreenWidth = UIScreen.mainScreen().bounds.size.width
let ScreenHeight = UIScreen.mainScreen().bounds.size.height



//Random number generator
func randomNumber(#minX:UInt32, #maxX:UInt32) -> Int {
  let result = (arc4random() % (maxX - minX + 1)) + minX
  return Int(result)
}

let TileMargin: CGFloat = 20.0

//let FontHUD = UIFont(name:"comic andy", size: 62.0)!
//let FontHUDBig = UIFont(name:"comic andy", size:120.0)!
let FontHUD = UIFont(name:"HelveticaNeue-Bold", size: 18.0)!
let FontHUDName = "HelveticaNeue-Bold"
let FontHUDSize : CGFloat = 18.0

let FontHUDWhite = UIColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
let FontHUDRed = UIColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 1.0)

let FontHUDBig = UIFont(name:"HelveticaNeue-Bold", size:120.0)!
let FontHUDBigName = "HelveticaNeue-Bold"
let FontHUDBigSize : CGFloat = 120.0

// Sound effects
let SoundDing = "ding.mp3"
let SoundWrong = "wrong.m4a"
let SoundWin = "win.mp3"
let AudioEffectFiles = [SoundDing, SoundWrong, SoundWin]
