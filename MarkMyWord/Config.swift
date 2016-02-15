//
//  CardController.swift
//
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit

//// UI Constants /////////////////////////////////
let screenWidth  = UIScreen.mainScreen().bounds.size.width
let screenHeight = UIScreen.mainScreen().bounds.size.height
var screenSize : CGSize?
// Random number generator /////////////////////
func randomNumber(minX minX:UInt32, maxX:UInt32) -> Int {
  let result = (arc4random() % (maxX - minX + 1)) + minX
  return Int(result)
}

let TileMargin: CGFloat = 19.0

//// Fonts ///////////////////////////////////////
//let FontHUD = UIFont(name:"comic andy", size: 62.0)!
//let FontHUDBig = UIFont(name:"comic andy", size:120.0)!

let FontHUDName = "HelveticaNeue"

var FontHUDSize : CGFloat = 19.0

func setFontHUDSize () {
    if mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
        FontHUDSize = 7.5
    }
    if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus {
        FontHUDSize = 8
    }
    if mmwGame.deviceType == MMWGame.DeviceType.iPad {
        FontHUDSize = 19
    }
    if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
        FontHUDSize = 25
    }
}


var FontHUD = UIFont(name:"HelveticaNeue", size: 18.0)

let FontHUDiPhoneSize = 12.0
let FontHUDiPhone = UIFont(name:"HelveticaNeue", size: 12.0)

let fontLetterTileSize = 40

let FontHUDBigName = "HelveticaNeue-Bold"

let FontHUDBigSize : CGFloat = 120.0
var FontHUDBig = UIFont(name:"HelveticaNeue-Bold", size:120.0)

let FontHUDBigiPhoneiPhone = 80.0
let FontHUDBigiPhone = UIFont(name:"HelveticaNeue-Bold", size:80.0)



//// Colors //////////////////////////////////////
let FontHUDWhite        = UIColor(red: 1.0,    green: 1.0,    blue: 1.0,    alpha: 1.0)
let FontHUDRed          = UIColor(red: 1.0,    green: 0.0,    blue: 0.0,    alpha: 1.0)
let FontHUDBlack        = UIColor(red: 0.0,    green: 0.0,    blue: 0.0,    alpha: 1.0)
////Original Apple Colors
let UIColorGray         = UIColor(red: 0.5,    green: 0.5,    blue: 0.5,    alpha: 1.0)
let UIColorAppleBlue    = UIColor(red: 0.0,    green: 0.6133, blue: 0.8594, alpha: 1.0)
let UIColorAppleRed     = UIColor(red: 0.875,  green: 0.2266, blue: 0.2422, alpha: 1.0)
let UIColorApplePurple  = UIColor(red: 0.5859, green: 0.2383, blue: 0.5898, alpha: 1.0)
let UIColorAppleGreen   = UIColor(red: 0.3828, green: 0.7305, blue: 0.2773, alpha: 1.0)
let UIColorAppleOrange  = UIColor(red: 0.9609, green: 0.5117, blue: 0.125,  alpha: 1.0)
let UIColorAppleYellow  = UIColor(red: 0.9609, green: 0.5117, blue: 0.125,  alpha: 1.0)

//let UIColorGray = UIColor(red: 0.5, green: 0.5, blue: 0.5, alpha: 1.0)
//let UIColorAppleGreen = UIColor(red: 27/256, green: 168/256, blue: 14/256, alpha: 1.0)
//let UIColorAppleOrange = UIColor(red: 0.9609, green: 0.5117, blue: 0.125, alpha: 1.0)
//let UIColorAppleBlue = UIColor(colorLiteralRed: 16/256, green: 194/256, blue: 254/256, alpha: 1.0)
//let UIColorAppleRed = UIColor(colorLiteralRed: 254/256, green: 41/256, blue: 137/256, alpha: 1.0)
//let UIColorApplePurple = UIColor(red: 169/256, green: 33/256, blue: 237/256, alpha: 1.0)
//let UIColorAppleYellow = UIColor(colorLiteralRed: 252/256, green: 217/256, blue: 0/256, alpha: 1.0)

let gameColors :  [UIColor] = [UIColorGray, UIColorAppleBlue, UIColorAppleOrange, UIColorAppleRed, UIColorApplePurple, UIColorAppleGreen,  ]
let tileColors :  [UIColor] = gameColors
//let gameColors :  [UIColor] = [UIColorGray, UIColorAppleBlue, UIColorAppleRed, UIColorApplePurple,  UIColorAppleGreen, UIColorAppleYellow, UIColorAppleOrange]

//// Sound effects ///////////////////////////////
let SoundDing = "ding.mp3"
let SoundWrong = "wrong.m4a"
let SoundWin = "win.mp3"
let AudioEffectFiles = [SoundDing, SoundWrong, SoundWin]

func delay(delay:Double, closure:()->()) {
    dispatch_after(
        dispatch_time(
            DISPATCH_TIME_NOW,
            Int64(delay * Double(NSEC_PER_SEC))
        ),
        dispatch_get_main_queue(), closure)
}

