//
//  Stats.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/18/16.
//  Copyright © 2016 Hook Studios. All rights reserved.
//

import Foundation
import CoreData


class Stats: NSManagedObject {

// Insert code here to add functionality to your managed object subclass
    
    override func awakeFromInsert() {
        super.awakeFromInsert()
        // Give the properties their initial values
        player = ""
        gamesWon = 0
        gamesLost = 0
        numWordsPlayed = 0
        numLettersPlayed = 0
        purchasedDeluxe = 0
    }
    

}
