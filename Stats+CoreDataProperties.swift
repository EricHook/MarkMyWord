//
//  Stats+CoreDataProperties.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/18/16.
//  Copyright © 2016 Hook Studios. All rights reserved.
//
//  Choose "Create NSManagedObject Subclass…" from the Core Data editor menu
//  to delete and recreate this implementation file for your updated model.
//

import Foundation
import CoreData

extension Stats {

    @NSManaged var player: String?
    @NSManaged var gamesWon: NSNumber?
    @NSManaged var gamesLost: NSNumber?
    @NSManaged var numWordsPlayed: NSNumber?
    @NSManaged var numLettersPlayed: NSNumber?
    @NSManaged var purchasedDeluxe: NSNumber?
    

}
