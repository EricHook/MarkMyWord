//
//  MarkMyWordSettings.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/17/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit

var mmwGame = MMWGame()

class MMWGame {

    var deviceType: DeviceType?
    
    enum DeviceType : Int {
        case iPad = 0, iPadRetina, iPadPro, iPhone5, iPhone6, iPhone6Plus
    }
    

    init () {
 
    }
    
    
    func setDeviceType (deviceType : DeviceType) {
        self.deviceType = deviceType
    }
}
